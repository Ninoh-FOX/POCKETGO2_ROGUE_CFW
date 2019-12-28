/*
 * linux/drivers/misc/jz_vpu.c
 *
 * Virtual device driver to manage VPU for JZ4770.
 *
 * Copyright (C) 2006  Ingenic Semiconductor Inc.
 * Copyright (C) 2013  Wladimir J. van der Laan
 * Copyright (C) 2013  Maarten ter Huurne <maarten@treewalker.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <asm/bitops.h>
#include <asm/pgtable.h>
#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>

#include <asm/irq.h>
#include <asm/thread_info.h>
#include <asm/uaccess.h>

#include <asm/mach-jz4770/jz4770cpm.h>
#include <asm/mach-jz4770/irq.h>

#include <linux/syscalls.h>

#include <uapi/video/jz_vpu.h>


#define AUX_CTRL 		0x00
#define AUX_SPINLK  		0x04
#define AUX_SPIN1  		0x08
#define AUX_SPIN2  		0x0c
#define AUX_MIRQP 		0x10
#define AUX_MESG  		0x14
#define CORE_MIRQP 		0x18
#define CORE_MESG  		0x1c


/* Physical memory heap structure */
struct jz_vpu_mem {
	struct page *page;
	unsigned long physical;
	unsigned long kaddr;
	size_t size;
	struct list_head list;
};

/* Device data */
struct jz_vpu {
	/* mutex to protect structure */
	struct semaphore mutex;
	/* completion ioctl lock */
	spinlock_t ioctl_lock;
	/* usage status bits */
	volatile unsigned long in_use;
	/* memory allocations belonging to this VPU connection */
	struct list_head mem_list;
	/* completion of job signalled by VPU code raising IRQ */
	struct completion completion;
	/* IRQ number for VPU */
	int irq;
	/* Clock for AUX (VPU's CPU core) */
	struct clk *aux_clk;
	/* Clock for other VPU components */
	struct clk *vpu_clk;
	/* Base address for AUX registers */
	void __iomem *aux_base;
};

/*
 * fops routines
 */

__BUILD_SET_C0(config7)

static void jz_vpu_on(struct device *dev)
{
	struct jz_vpu *vpu = dev_get_drvdata(dev->parent);

	/* Do not stop CPUI clock when in idle mode. */
	SETREG32(CPM_OPCR, OPCR_IDLE_DIS);

	clk_enable(vpu->aux_clk);
	clk_enable(vpu->vpu_clk);

	/* enable power to AHB1 (VPU), then wait for it to enable */
	CLRREG32(CPM_LCR, LCR_PDAHB1);
	while (!(REG_CPM_LCR && LCR_PDAHB1S)) ;

	/*
	 * Enable partial kernel mode. This allows user space access
	 * to the TCSM, cache instructions and VPU.
	 */
	set_c0_config7(BIT(6));

	enable_irq(vpu->irq);

	dev_dbg(dev, "VPU enabled, cp0 status=0x%08X\n",
		     (unsigned int)task_pt_regs(current)->cp0_status);
}

static void jz_vpu_off(struct device *dev)
{
	struct jz_vpu *vpu = dev_get_drvdata(dev->parent);

	/* Power down AHB1 (VPU) */
	SETREG32(CPM_LCR, LCR_PDAHB1);
	while (!(REG_CPM_LCR && LCR_PDAHB1S)) ;

	disable_irq_nosync(vpu->irq);

	clk_disable(vpu->aux_clk);
	clk_disable(vpu->vpu_clk);

	/*
	 * Disable partial kernel mode. This disallows user space access
	 * to the TCSM, cache instructions and VPU.
	 */
	clear_c0_config7(BIT(6));

	CLRREG32(CPM_OPCR, OPCR_IDLE_DIS);

	dev_dbg(dev, "VPU disabled\n");
}

/* Allocate a new contiguous memory block, return the physical address
 * that can be mmapped. */
static unsigned long jz_vpu_alloc_phys(struct jz_vpu *vpu, size_t size, unsigned long *physical)
{
	struct jz_vpu_mem *mem;

	mem = kmalloc(sizeof(struct jz_vpu_mem), GFP_KERNEL);
	if (mem == NULL)
		return -ENOMEM;
	INIT_LIST_HEAD(&mem->list);

	mem->size = size;
	mem->page = alloc_pages(GFP_KERNEL | __GFP_NOWARN, get_order(mem->size));
	if (mem->page == NULL) {
		kfree(mem);
		return -ENOMEM;
	}
	mem->kaddr = (unsigned long)page_address(mem->page);
	mem->physical = page_to_phys(mem->page);

	down(&vpu->mutex);
	list_add_tail(&mem->list, &vpu->mem_list);
	up(&vpu->mutex);

	*physical = mem->physical;
	return 0;
}

/* Free one contiguous memory block by pointer */
static int jz_vpu_free_mem(struct device *dev, struct jz_vpu *vpu,
			   struct jz_vpu_mem *mem)
{
	dev_dbg(dev, "Free mem %p 0x%08X size=%d\n",
		     mem, (unsigned int)mem->physical, (unsigned int)mem->size);

	down(&vpu->mutex);
	list_del(&mem->list);
	up(&vpu->mutex);

	free_pages(mem->kaddr, get_order(mem->size));
	kfree(mem);
	return 0;
}

/* Free one contiguous memory block by physical address */
static int jz_vpu_free_phys(struct device *dev, struct jz_vpu *vpu,
			    unsigned long physical)
{
	struct jz_vpu_mem *mem;
	list_for_each_entry(mem, &vpu->mem_list, list) {
		if (mem->physical == physical) {
			jz_vpu_free_mem(dev, vpu, mem);
			return 0;
		}
	}
	dev_err(dev, "Attempt to free non-allocated memory at 0x%08X\n",
		     (unsigned int)physical);
	return -ENOENT;
}

static int jz_vpu_open(struct inode *inode, struct file *file)
{
	struct miscdevice *misc = file->private_data;
	struct device *dev = misc->this_device;
	struct jz_vpu *vpu = dev_get_drvdata(misc->parent);

	dev_dbg(dev, "Device node open\n");

	/* Enforce exclusive VPU access. */
	if (test_and_set_bit(0, &vpu->in_use))
		return -EBUSY;

	jz_vpu_on(dev);

	return 0;
}

static int jz_vpu_release(struct inode *inode, struct file *file)
{
	struct miscdevice *misc = file->private_data;
	struct device *dev = misc->this_device;
	struct jz_vpu *vpu = dev_get_drvdata(misc->parent);
	struct jz_vpu_mem *mem, *next;

	dev_dbg(dev, "Device node close\n");

	jz_vpu_off(dev);

	/* Free all contiguous memory blocks associated with this VPU connection */
	list_for_each_entry_safe(mem, next, &vpu->mem_list, list)
		jz_vpu_free_mem(dev, vpu, mem);

	clear_bit(0, &vpu->in_use);

	return 0;
}

static long jz_vpu_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct miscdevice *misc = file->private_data;
	struct device *dev = misc->this_device;
	struct jz_vpu *vpu = dev_get_drvdata(misc->parent);
	spinlock_t ioctl_lock = vpu->ioctl_lock;
	unsigned long flags;
	long ret = 0;

	spin_lock_irqsave(&ioctl_lock, flags);
	switch (cmd) {
	case JZ_VPU_IOCTL_WAIT_COMPLETE:
		dev_dbg(dev, "ioctl: TCSM_TOCTL_WAIT_COMPLETE\n");
		spin_unlock_irqrestore(&ioctl_lock, flags);
		ret = wait_for_completion_interruptible_timeout(
				&vpu->completion, msecs_to_jiffies(arg));
		spin_lock_irqsave(&ioctl_lock, flags);
		break;
	case JZ_VPU_IOCTL_ALLOC: {
		struct jz_vpu_alloc data;
		copy_from_user(&data, (void*)arg, sizeof(struct jz_vpu_alloc));
		ret = jz_vpu_alloc_phys(vpu, data.size, &data.physical);
		copy_to_user((void*)arg, &data, sizeof(struct jz_vpu_alloc));
		} break;
	case JZ_VPU_IOCTL_FREE:
		jz_vpu_free_phys(dev, vpu, arg);
		break;
	default:
		dev_dbg(dev, "ioctl: unsupported cmd 0x%X\n", cmd);
		ret = -ENOIOCTLCMD;
	}
	spin_unlock_irqrestore(&ioctl_lock, flags);
	return ret;
}

static int jz_vpu_mmap(struct file *file, struct vm_area_struct *vma)
{
	vma->vm_flags |= VM_IO;
	/* XXX only set memory to non-cacheable and ioremap when mapping IO, not phys memory */
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot); /* uncacheable */
	if (io_remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
			       vma->vm_end - vma->vm_start, vma->vm_page_prot))
		return -EAGAIN;
	return 0;
}

static struct file_operations jz_vpu_fops = {
	.open		= jz_vpu_open,
	.release	= jz_vpu_release,
	.unlocked_ioctl	= jz_vpu_ioctl,
	.mmap		= jz_vpu_mmap,
};

static struct miscdevice jz_vpu_misc = {
	MISC_DYNAMIC_MINOR,
	"jz-vpu",
	&jz_vpu_fops
};

static irqreturn_t vpu_interrupt(int irq, void *data)
{
	struct jz_vpu *vpu = data;

	writel(readl(vpu->aux_base + AUX_MIRQP) & ~BIT(0),
	       vpu->aux_base + AUX_MIRQP);
	complete(&vpu->completion);

	return IRQ_HANDLED;
}

static int jz_vpu_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct jz_vpu *vpu;
	int irq;
	int ret;

	vpu = devm_kzalloc(dev, sizeof(*vpu), GFP_KERNEL);
	if (!vpu)
		return -ENOMEM;
	INIT_LIST_HEAD(&vpu->mem_list);
	sema_init(&vpu->mutex, 1);
	init_completion(&vpu->completion);

	vpu->aux_base = devm_ioremap_resource(dev,
			platform_get_resource(pdev, IORESOURCE_MEM, 0));
	if (IS_ERR(vpu->aux_base)) {
		ret = PTR_ERR(vpu->aux_base);
		dev_err(dev, "Failed to get and remap mmio region: %d\n", ret);
		return ret;
	}

	/*
	 * Note: TCSM0 is also declared as a platform resource, but we do not
	 *       have any driver code yet that accesses it.
	 */

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(dev, "Failed to get platform IRQ: %d\n", irq);
		return irq;
	}
	ret = devm_request_irq(dev, irq, vpu_interrupt, 0, "jz-vpu", vpu);
	if (ret < 0) {
		dev_err(dev, "Failed to request IRQ: %d\n", ret);
		return ret;
	}
	disable_irq_nosync(irq);
	vpu->irq = irq;

	vpu->aux_clk = devm_clk_get(&pdev->dev, "aux");
	if (IS_ERR(vpu->aux_clk)) {
		ret = PTR_ERR(vpu->aux_clk);
		dev_err(dev, "Failed to get AUX clock: %d\n", ret);
		return ret;
	}
	vpu->vpu_clk = devm_clk_get(&pdev->dev, "vpu");
	if (IS_ERR(vpu->vpu_clk)) {
		ret = PTR_ERR(vpu->vpu_clk);
		dev_err(dev, "Failed to get VPU clock: %d\n", ret);
		return ret;
	}

	ret = dev_set_drvdata(dev, vpu);
	jz_vpu_misc.parent = dev;
	ret = misc_register(&jz_vpu_misc);
	if (ret < 0) {
		dev_err(dev, "Failed to register misc device: %d\n", ret);
		return ret;
	}
	jz_vpu_misc.this_device->parent = dev;

	dev_info(jz_vpu_misc.this_device, "Driver registered\n");
	return 0;
}

static int jz_vpu_remove(struct platform_device *pdev)
{
	misc_deregister(&jz_vpu_misc);

	return 0;
}

#ifdef CONFIG_PM

static int jz_vpu_suspend(struct platform_device *pdev, pm_message_t state)
{
	/* TODO: Abort any computation in progress. */
	return 0;
}

static int jz_vpu_resume(struct platform_device *pdev)
{
	return 0;
}

#else

#define jz_vpu_suspend	NULL
#define jz_vpu_resume	NULL

#endif

static struct platform_driver jz_vpu_driver = {
	.probe		= jz_vpu_probe,
	.remove		= jz_vpu_remove,
	.suspend	= jz_vpu_suspend,
	.resume		= jz_vpu_resume,
	.driver		= {
		.name	= "jz-vpu",
		.owner	= THIS_MODULE,
	},
};

module_platform_driver(jz_vpu_driver);

MODULE_LICENSE("GPL");

/*
 * JZ4770 SoC ADC driver
 *
 * This driver synchronizes access to the JZ4770 ADC core between the
 * JZ4770 battery and analog joystick drivers.
 *
 * Heavily based on JZ4740 ADC driver:
 * Copyright (C) 2009-2010, Lars-Peter Clausen <lars@metafoo.de>
 *
 * JZ4770 specific modifications:
 * Copyright (C) 2012, Maarten ter Huurne <maarten@treewalker.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#include <linux/clk.h>
#include <linux/mfd/core.h>

#include <linux/jz4770-adc.h>
#include <asm/mach-jz4770/jz4770sadc.h>


#define JZ_REG_ADC_ENABLE		0x00
#define JZ_REG_ADC_CFG			0x04
#define JZ_REG_ADC_CTRL			0x08
#define JZ_REG_ADC_STATUS		0x0C

#define JZ_REG_ADC_TOUCHSCREEN_BASE	0x10


#define JZ_REG_ADC_BATTERY_BASE		0x1C
#define JZ_REG_ADC_AUX_BASE		0x20


#define JZ_REG_ADC_CMD			0x24
#define JZ_REG_ADC_CLKDIV		0x28

#define JZ_ADC_ENABLE_POWER		BIT(7)
#define JZ_ADC_ENABLE_PENDOWN		BIT(3)
#define JZ_ADC_ENABLE_TOUCHSCREEN	BIT(2)
#define JZ_ADC_ENABLE_BATTERY		BIT(1)
#define JZ_ADC_ENABLE_AUX		BIT(0)

/* ADC Command Register (ADCMD) */
#define ADCMD_PIL		BIT(31)
#define ADCMD_RPU(n)		((n) << 26)
#define ADCMD_XPSUP		BIT(25)
#define ADCMD_XNSUP		BIT(24)
#define ADCMD_YPSUP		BIT(23)
#define ADCMD_XPGRU		BIT(22)
#define	ADCMD_XNGRU		BIT(21)
#define	ADCMD_YNGRU		BIT(20)
#define	ADCMD_VREFAUX		BIT(19)
#define	ADCMD_VREFNXN		BIT(18)
#define	ADCMD_VREFNXP		BIT(17)
#define	ADCMD_VREFNYN		BIT(16)
#define	ADCMD_VREFPVDD33	BIT(15)
#define ADCMD_VREFPAUX		BIT(14)
#define ADCMD_VREFPXN		BIT(13)
#define ADCMD_VREFPXP		BIT(12)
#define ADCMD_VREFPYP		BIT(11)
#define ADCMD_XPADC		BIT(10)
#define ADCMD_XNADC		BIT(9)
#define ADCMD_YPADC		BIT(8)
#define ADCMD_YNADC		BIT(7)
#define ADCMD_RPUXP		BIT(2)
#define ADCMD_RPUYP		BIT(1)

#define ADCTRL_SLPENDM		BIT(5)
#define ADCTRL_PENDM		BIT(4)
#define ADCTRL_PENUM		BIT(3)
#define ADCTRL_DTCHM		BIT(2)
#define ADCTRL_VRDYM		BIT(1)
#define ADCTRL_ARDYM		BIT(0)

enum {
	JZ_ADC_IRQ_ADCIN = 0,
	JZ_ADC_IRQ_BATTERY,
	JZ_ADC_IRQ_TOUCHSCREEN,
	JZ_ADC_IRQ_PENUP,
	JZ_ADC_IRQ_PENDOWN,
	JZ_ADC_IRQ_SLEEPPENDOWN,
};

struct jz4770_adc {
	void __iomem *base;

	int irq;
	struct irq_chip_generic *gc;

	struct clk *clk;
	atomic_t clk_ref;

	spinlock_t lock;
};

static void jz4770_adc_irq_demux(unsigned int irq, struct irq_desc *desc)
{
	struct irq_chip_generic *gc = irq_desc_get_handler_data(desc);
	uint8_t status;
	unsigned int i;

	status = readb(gc->reg_base + JZ_REG_ADC_STATUS);
	status &= ~readb(gc->reg_base + JZ_REG_ADC_CTRL);

	for (i = 0; i < SADC_IRQ_NUM; ++i) {
		if (status & BIT(i))
			generic_handle_irq(gc->irq_base + i);
	}
}

/*
 * Refcounting for the ADC clock is done in here instead of in the clock
 * framework, because it is the only clock which is shared between multiple
 * devices and thus is the only clock which needs refcounting.
 */
static inline void jz4770_adc_clk_enable(struct jz4770_adc *adc)
{
	if (atomic_inc_return(&adc->clk_ref) == 1)
		clk_enable(adc->clk);
}

static inline void jz4770_adc_clk_disable(struct jz4770_adc *adc)
{
	if (atomic_dec_return(&adc->clk_ref) == 0)
		clk_disable(adc->clk);
}


static int jz4770_adc_set_clock(struct jz4770_adc *adc, unsigned int freq)
{
	unsigned int val, div, div_ms, div_us;

	/* Check hardware limit. */
	if (freq < 20000 || freq > 200000)
		return -EINVAL;

	div = clk_get_rate(adc->clk) / freq - 1;
	if (div >= 0x100)
		return -EINVAL;

	div_ms = freq / 1000 - 1;

	/* Set "us_clk" to 10 kHz (0.1 ms ticks). */
	div_us = freq / 10000 - 1;

	val = (div_ms << SADC_ADCLK_CLKDIV_MS)
	    | (div_us << SADC_ADCLK_CLKDIV_US)
	    | (div    << SADC_ADCLK_CLKDIV_BIT);
	writel(val, adc->base + JZ_REG_ADC_CLKDIV);

	return 0;
}


static int jz4770_adc_cell_enable(struct platform_device *pdev)
{
	struct jz4770_adc *adc = dev_get_drvdata(pdev->dev.parent);
	uint8_t val, mask = BIT(pdev->id);
	unsigned long flags;

	jz4770_adc_clk_enable(adc);

	spin_lock_irqsave(&adc->lock, flags);

	val = readb(adc->base + JZ_REG_ADC_ENABLE);
	while (val & JZ_ADC_ENABLE_POWER) {
		writeb(val & ~JZ_ADC_ENABLE_POWER,
			adc->base + JZ_REG_ADC_ENABLE);
		spin_unlock_irqrestore(&adc->lock, flags);
		msleep(2);
		spin_lock_irqsave(&adc->lock, flags);
		val = readb(adc->base + JZ_REG_ADC_ENABLE);
	}
	writeb(val | mask, adc->base + JZ_REG_ADC_ENABLE);

	spin_unlock_irqrestore(&adc->lock, flags);

	return 0;
}

static int jz4770_adc_cell_disable(struct platform_device *pdev)
{
	struct jz4770_adc *adc = dev_get_drvdata(pdev->dev.parent);
	uint8_t val, mask = BIT(pdev->id);
	unsigned long flags;

	spin_lock_irqsave(&adc->lock, flags);

	val = readb(adc->base + JZ_REG_ADC_ENABLE) & ~mask;
	writeb(val, adc->base + JZ_REG_ADC_ENABLE);

	/*
	 * The touch screen engine can take a very short time to shut down;
	 * wait for it by polling its enable bit.
	 * We wait for the other engines as well; this is probably not needed
	 * but the overhead is neglible.
	 */
	while (readb(adc->base + JZ_REG_ADC_ENABLE) & mask);

	if (val == 0)
		writeb(JZ_ADC_ENABLE_POWER, adc->base + JZ_REG_ADC_ENABLE);

	spin_unlock_irqrestore(&adc->lock, flags);

	jz4770_adc_clk_disable(adc);

	return 0;
}

int jz4770_adc_set_config(struct device *dev, uint32_t mask, uint32_t val)
{
	struct jz4770_adc *adc = dev_get_drvdata(dev);
	unsigned long flags;
	uint32_t cfg;

	if (!adc)
		return -ENODEV;

	spin_lock_irqsave(&adc->lock, flags);

	cfg = readl(adc->base + JZ_REG_ADC_CFG);

	cfg &= ~mask;
	cfg |= val;

	writel(cfg, adc->base + JZ_REG_ADC_CFG);
	
	spin_unlock_irqrestore(&adc->lock, flags);
	
	return 0;
}
EXPORT_SYMBOL_GPL(jz4770_adc_set_config);

int jz4770_adc_set_adcmd(struct device *dev)
{
	struct jz4770_adc *adc = dev_get_drvdata(dev);
	unsigned long flags;

	if (!adc)
		return -ENODEV;

	spin_lock_irqsave(&adc->lock, flags);
		
	//Initialize adcmd	
	readl(adc->base + JZ_REG_ADC_CMD);

	writel(ADCMD_XPSUP | ADCMD_XNGRU | ADCMD_VREFNXN | ADCMD_VREFPXP | ADCMD_YPADC, adc->base + JZ_REG_ADC_CMD);
	writel(ADCMD_YPSUP | ADCMD_YNGRU | ADCMD_VREFNYN | ADCMD_VREFPYP | ADCMD_XPADC, adc->base + JZ_REG_ADC_CMD);
	writel(ADCMD_XPSUP | ADCMD_XNGRU | ADCMD_VREFNXN | ADCMD_VREFPXP | ADCMD_YNADC, adc->base + JZ_REG_ADC_CMD);
	writel(ADCMD_YPSUP | ADCMD_YNGRU | ADCMD_VREFNYN | ADCMD_VREFPYP | ADCMD_XNADC, adc->base + JZ_REG_ADC_CMD);
	writel(0, adc->base + JZ_REG_ADC_CMD);

	spin_unlock_irqrestore(&adc->lock, flags);
	return 0;
}
EXPORT_SYMBOL_GPL(jz4770_adc_set_adcmd);


static struct resource jz4770_aux_resources[] = {
	{
		.start	= JZ_ADC_IRQ_ADCIN,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= JZ_REG_ADC_AUX_BASE,
		.end	= JZ_REG_ADC_AUX_BASE + 3,
		.flags	= IORESOURCE_MEM,
	},
};

static struct resource jz4770_battery_resources[] = {
	{
		.start	= JZ_ADC_IRQ_BATTERY,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= JZ_REG_ADC_BATTERY_BASE,
		.end	= JZ_REG_ADC_BATTERY_BASE + 3,
		.flags	= IORESOURCE_MEM,
	},
};

static struct resource jz4770_touchscreen_resources[] = {
	{
		.start	= JZ_ADC_IRQ_TOUCHSCREEN,
		.end	= JZ_ADC_IRQ_SLEEPPENDOWN,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= JZ_REG_ADC_TOUCHSCREEN_BASE,
		.end	= JZ_REG_ADC_TOUCHSCREEN_BASE + 11,
		.flags	= IORESOURCE_MEM,
	},
};

static struct mfd_cell jz4770_adc_cells[] = {
	{
		.id = 0,
		.name = "jz4770-aux",
		.num_resources = ARRAY_SIZE(jz4770_aux_resources),
		.resources = jz4770_aux_resources,

		.enable = jz4770_adc_cell_enable,
		.disable = jz4770_adc_cell_disable,
	},
	{
		.id = 1,
		.name = "jz4770-battery",
		.num_resources = ARRAY_SIZE(jz4770_battery_resources),
		.resources = jz4770_battery_resources,

		.enable = jz4770_adc_cell_enable,
		.disable = jz4770_adc_cell_disable,
	},
	{
		.id = 2,
		.name = "jz4770-touchscreen",
		.num_resources = ARRAY_SIZE(jz4770_touchscreen_resources),
		.resources = jz4770_touchscreen_resources,

		.enable = jz4770_adc_cell_enable,
		.disable = jz4770_adc_cell_disable,
	},
};

static int jz4770_adc_probe(struct platform_device *pdev)
{
	struct irq_chip_generic *gc;
	struct irq_chip_type *ct;
	struct jz4770_adc *adc;
	struct resource *mem_base;
	struct resource *mem;
	void __iomem *base2;
	int irq_base;
	int err;

	adc = devm_kzalloc(&pdev->dev, sizeof(*adc), GFP_KERNEL);
	if (!adc) {
		dev_err(&pdev->dev, "Failed to allocate driver structure\n");
		return -ENOMEM;
	}

	/* Get multiplexed SADC IRQ. */

	adc->irq = platform_get_irq(pdev, 0);
	if (adc->irq < 0) {
		dev_err(&pdev->dev, "Failed to get platform irq: %d\n",
			adc->irq);
		return adc->irq;
	}

	/* Get base number for demultiplexed cell IRQs. */

	irq_base = platform_get_irq(pdev, 1);
	if (irq_base < 0) {
		dev_err(&pdev->dev, "Failed to get irq base: %d\n", irq_base);
		return irq_base;
	}

	/*
	 * Map registers, but only the generic SADC portions, not the registers
	 * used by the cells.
	 */

	mem_base = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem_base) {
		dev_err(&pdev->dev, "Failed to get platform MMIO resource\n");
		return -ENOENT;
	}

	mem = devm_request_mem_region(&pdev->dev, mem_base->start, 0x10,
				      pdev->name);
	if (!mem) {
		dev_err(&pdev->dev, "Failed to request first MMIO region\n");
		return -EBUSY;
	}

	adc->base = devm_ioremap_nocache(&pdev->dev, mem->start,
					 resource_size(mem));
	if (!adc->base) {
		dev_err(&pdev->dev, "Failed to ioremap first MMIO region\n");
		return -EBUSY;
	}

	mem = devm_request_mem_region(&pdev->dev, mem_base->start + 0x24, 0x08,
				      pdev->name);
	if (!mem) {
		dev_err(&pdev->dev,"Failed to request second MMIO region\n");
		return -EBUSY;
	}

	base2 = devm_ioremap_nocache(&pdev->dev, mem->start,
				     resource_size(mem));
	if (!base2) {
		dev_err(&pdev->dev, "Failed to ioremap second MMIO region\n");
		return -EBUSY;
	}
	if (base2 != adc->base + 0x24) {
		dev_err(&pdev->dev, "Second MMIO region misplaced\n");
		return -ENXIO;
	}

	/* Initialize hardware. */

	adc->clk = devm_clk_get(&pdev->dev, "adc");
	if (IS_ERR(adc->clk)) {
		dev_err(&pdev->dev,
			"Failed to get clock: %ld\n", PTR_ERR(adc->clk));
		return PTR_ERR(adc->clk);
	}

	/* Register writes have no effect unless clock is running. */
	clk_enable(adc->clk);

	/* Disable all cells and power off. */
	writeb(JZ_ADC_ENABLE_POWER, adc->base + JZ_REG_ADC_ENABLE);

	/* Mask all interrupts. */
	writeb(0xFF, adc->base + JZ_REG_ADC_CTRL);
	

	
	err = jz4770_adc_set_clock(adc, 100000);

	clk_disable(adc->clk);

	if (err) {
		dev_err(&pdev->dev, "Failed to configure clock: %d\n", err);
		return err;
	}


	spin_lock_init(&adc->lock);
	atomic_set(&adc->clk_ref, 0);

	platform_set_drvdata(pdev, adc);

	gc = irq_alloc_generic_chip("INTC", 1, irq_base, adc->base,
				    handle_level_irq);

	ct = gc->chip_types;
	ct->regs.mask = JZ_REG_ADC_CTRL;
	ct->regs.ack = JZ_REG_ADC_STATUS;
	ct->chip.irq_mask = irq_gc_mask_set_bit;
	ct->chip.irq_unmask = irq_gc_mask_clr_bit;
	ct->chip.irq_ack = irq_gc_ack_set_bit;

	irq_setup_generic_chip(gc, IRQ_MSK(SADC_IRQ_NUM),
			       IRQ_GC_INIT_MASK_CACHE,
			       0, IRQ_NOPROBE | IRQ_NOAUTOEN | IRQ_LEVEL);

	adc->gc = gc;

	irq_set_handler_data(adc->irq, gc);
	irq_set_chained_handler(adc->irq, jz4770_adc_irq_demux);

	return mfd_add_devices(&pdev->dev, 0, jz4770_adc_cells,
			       ARRAY_SIZE(jz4770_adc_cells), mem_base,
			       irq_base, NULL);
}

static int jz4770_adc_remove(struct platform_device *pdev)
{
	struct jz4770_adc *adc = platform_get_drvdata(pdev);

	mfd_remove_devices(&pdev->dev);

	irq_remove_generic_chip(adc->gc, IRQ_MSK(SADC_IRQ_NUM),
				IRQ_NOPROBE | IRQ_LEVEL, 0);
	kfree(adc->gc);
	irq_set_handler_data(adc->irq, NULL);
	irq_set_chained_handler(adc->irq, NULL);

	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver jz4770_adc_driver = {
	.probe	= jz4770_adc_probe,
	.remove = jz4770_adc_remove,
	.driver = {
		.name = "jz4770-adc",
		.owner = THIS_MODULE,
	},
};

module_platform_driver(jz4770_adc_driver);

MODULE_DESCRIPTION("JZ4770 SoC ADC driver");
MODULE_AUTHOR("Maarten ter Huurne <maarten@treewalker.org>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:jz4770-adc");

/*
 * linux/arch/mips/jz4770/irq.c
 *
 * JZ4770 interrupt routines.
 *
 * Copyright (c) 2006-2007  Ingenic Semiconductor Inc.
 * Author: <lhhuang@ingenic.cn>
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 */
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/kernel_stat.h>
#include <linux/module.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdesc.h>
#include <linux/ioport.h>
#include <linux/timex.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/delay.h>
#include <linux/bitops.h>

#include <asm/bootinfo.h>
#include <asm/io.h>
#include <asm/mipsregs.h>

#include <asm/mach-jz4770/jz4770dmac.h>
#include <asm/mach-jz4770/jz4770gpio.h>
#include <asm/mach-jz4770/jz4770intc.h>

/*
 * C0 irq type -- this handles only the VPU interrupt
 */

static int c0_irq_pending(void)
{
	unsigned long c0_pending = 0;
	__asm__ __volatile__ (
			"mfc0  %0, $13,  0   \n\t"
			"nop                  \n\t"
			:"=r"(c0_pending)
			:);

	return (c0_pending & 0x800) != 0;
}

static void enable_c0_irq(struct irq_data *data)
{
	unsigned long cpuflags;
	local_irq_save(cpuflags);
	write_c0_status(read_c0_status() | 0x800);
	local_irq_restore(cpuflags);
}

static void disable_c0_irq(struct irq_data *data)
{
	unsigned long cpuflags;
	local_irq_save(cpuflags);
	write_c0_status(read_c0_status() & ~0x800);
	local_irq_restore(cpuflags);
}

static void mask_and_ack_c0_irq(struct irq_data *data)
{
	disable_c0_irq(data);
}

static unsigned int startup_c0_irq(struct irq_data *data)
{
	enable_c0_irq(data);
	return 0;
}

static void shutdown_c0_irq(struct irq_data *data)
{
	disable_c0_irq(data);
}

static struct irq_chip c0_irq_type = {
	.name = "C0",
	.irq_startup = startup_c0_irq,
	.irq_shutdown = shutdown_c0_irq,
	.irq_unmask = enable_c0_irq,
	.irq_mask = disable_c0_irq,
	.irq_ack = mask_and_ack_c0_irq,
};

/*
 * INTC irq type
 */

static void enable_intc_irq(struct irq_data *data)
{
	__intc_unmask_irq(data->irq);
}

static void disable_intc_irq(struct irq_data *data)
{
	__intc_mask_irq(data->irq);
}

static void mask_and_ack_intc_irq(struct irq_data *data)
{
	__intc_mask_irq(data->irq);
	__intc_ack_irq(data->irq);
}

static unsigned int startup_intc_irq(struct irq_data *data)
{
	enable_intc_irq(data);
	return 0;
}

static void shutdown_intc_irq(struct irq_data *data)
{
	disable_intc_irq(data);
}

static struct irq_chip intc_irq_type = {
	.name = "INTC",
	.irq_startup = startup_intc_irq,
	.irq_shutdown = shutdown_intc_irq,
	.irq_unmask = enable_intc_irq,
	.irq_mask = disable_intc_irq,
	// TODO(MtH): There is a dedicated irq_mask_ack as well.
	.irq_ack = mask_and_ack_intc_irq,
};

/*
 * DMA irq type
 */
static void enable_dma_irq(struct irq_data *data)
{
	unsigned int irq = data->irq;
	unsigned int intc_irq;

	if ( irq < (IRQ_DMA_0 + DMA_IRQ_NUM / 2) ) 	/* DMAC Group 0 irq */
		intc_irq = IRQ_DMAC0;
	else if ( irq < (IRQ_DMA_0 + DMA_IRQ_NUM) ) 	/* DMAC Group 1 irq */
		intc_irq = IRQ_DMAC1;
	else {
		printk("%s, unexpected dma irq #%d\n", __FILE__, irq);
		return;
	}
	__intc_unmask_irq(intc_irq);
	__dmac_channel_enable_irq(irq - IRQ_DMA_0);
}

static void disable_dma_irq(struct irq_data *data)
{
	int chan = data->irq - IRQ_DMA_0;
	__dmac_disable_channel(chan);
	__dmac_channel_disable_irq(chan);
}

static void mask_and_ack_dma_irq(struct irq_data *data)
{
	unsigned int irq = data->irq;
	unsigned int intc_irq;

	disable_dma_irq(data);

	if ( irq < (IRQ_DMA_0 + HALF_DMA_NUM) ) 	/* DMAC Group 0 irq */
		intc_irq = IRQ_DMAC0;
	else if ( irq < (IRQ_DMA_0 + MAX_DMA_NUM) ) 	/* DMAC Group 1 irq */
		intc_irq = IRQ_DMAC1;
	else {
		printk("%s, unexpected dma irq #%d\n", __FILE__, irq);
		return ;
	}
	__intc_ack_irq(intc_irq);
	//__dmac_channel_ack_irq(irq-IRQ_DMA_0); /* needed?? add 20080506, Wolfgang */
	//__dmac_channel_disable_irq(irq - IRQ_DMA_0);
}

static unsigned int startup_dma_irq(struct irq_data *data)
{
	enable_dma_irq(data);
	return 0;
}

static void shutdown_dma_irq(struct irq_data *data)
{
	disable_dma_irq(data);
}

static struct irq_chip dma_irq_type = {
	.name = "DMA",
	.irq_startup = startup_dma_irq,
	.irq_shutdown = shutdown_dma_irq,
	.irq_unmask = enable_dma_irq,
	.irq_mask = disable_dma_irq,
	.irq_ack = mask_and_ack_dma_irq,
};

//----------------------------------------------------------------------

void __init arch_init_irq(void)
{
	int i;

	clear_c0_status(0xff04); /* clear ERL */
	set_c0_status(0x0400);   /* set IP2 */

	/* Set up INTC irq. */
	for (i = 0; i < INTC_IRQ_NUM; i++) {
		disable_intc_irq(&irq_desc[i].irq_data);
		irq_set_chip_and_handler(i, &intc_irq_type, handle_level_irq);
	}

	/* Set up DMAC irq. */
	for (i = IRQ_DMA_0; i < IRQ_DMA_0 + DMA_IRQ_NUM; i++) {
		disable_dma_irq(&irq_desc[i].irq_data);
		irq_set_chip_and_handler(i, &dma_irq_type, handle_level_irq);
	}

	/* Set up C0 irq. */
	disable_intc_irq(&irq_desc[IRQ_VPU].irq_data);
	irq_set_chip_and_handler(IRQ_VPU, &c0_irq_type, handle_level_irq);
}

static int plat_real_irq(int irq)
{
	if ((irq >= IRQ_GPIO5) && (irq <= IRQ_GPIO0)) {
		int group = IRQ_GPIO0 - irq;
		irq = __gpio_group_irq(group);
		if (irq >= 0)
			irq += IRQ_GPIO_0 + 32 * group;
	} else {
		switch (irq) {
		case IRQ_DMAC0:
		case IRQ_DMAC1:
			irq = __dmac_get_irq();
			if (irq < 0) {
				printk("REG_DMAC_DMAIPR(0) = 0x%08x\n",
						REG_DMAC_DMAIPR(0));
				printk("REG_DMAC_DMAIPR(1) = 0x%08x\n",
						REG_DMAC_DMAIPR(1));
				return irq;
			}
			irq += IRQ_DMA_0;
			break;
		}
	}

	return irq;
}

asmlinkage void plat_irq_dispatch(void)
{
	unsigned long intc_ipr0 = REG_INTC_IPR(0);
	unsigned long intc_ipr1 = REG_INTC_IPR(1);
	int irq;

	if (intc_ipr0) {
		irq = ffs(intc_ipr0) - 1;
	} else if (intc_ipr1) {
		irq = ffs(intc_ipr1) - 1 + 32;
	} else if (c0_irq_pending()) {
		irq = IRQ_VPU;
	} else {
		spurious_interrupt();
		return;
	}

	irq = plat_real_irq(irq);
	WARN((irq < 0), "irq raised, but no irq pending!\n");
	if (irq < 0)
		return;

	do_IRQ(irq);
}

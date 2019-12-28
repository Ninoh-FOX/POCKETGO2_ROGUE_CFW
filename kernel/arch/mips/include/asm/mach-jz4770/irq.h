/*
 *  linux/arch/mips/include/asm/mach-jz4770/irq.h
 *
 *  JZ4770 IRQ definition.
 *
 *  Copyright (C) 2010 Ingenic Semiconductor Inc.
 *
 *  Author: <yliu@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_JZ4770_IRQ_H__
#define __ASM_JZ4770_IRQ_H__

/* we need 256 irq levels at least */
#define NR_IRQS	384

// 1st-level interrupts
#define IRQ_I2C1	0
#define IRQ_I2C0	1
#define IRQ_UART3	2
#define IRQ_UART2	3
#define IRQ_UART1	4
#define IRQ_UART0	5
#define IRQ_GPU		6
#define IRQ_SSI1   	7
#define IRQ_SSI0   	8
#define IRQ_TSSI	9
#define IRQ_BDMA	10
#define IRQ_KBC		11
#define IRQ_GPIO5	12
#define IRQ_GPIO4	13
#define IRQ_GPIO3	14
#define IRQ_GPIO2	15
#define IRQ_GPIO1	16
#define IRQ_GPIO0	17
#define IRQ_SADC	18
#define IRQ_ETH		19
#define IRQ_UHC		20
#define IRQ_OTG		21
//#define IRQ_MDMA	22
#define IRQ_I2C2	22
#define IRQ_DMAC1	23
#define IRQ_DMAC0	24
#define IRQ_TCU2	25
#define IRQ_TCU1	26
#define IRQ_TCU0	27
#define IRQ_GPS		28
#define IRQ_IPU		29
#define IRQ_CIM		30
#define IRQ_LCD		31

#define IRQ_RTC		32
#define IRQ_OWI		33
#define IRQ_AIC 	34
#define IRQ_MSC2	35
#define IRQ_MSC1	36
#define IRQ_MSC0	37
#define IRQ_SCC		38
#define IRQ_BCH		39
#define IRQ_PCM0	40
#define IRQ_PCM1	41
#define IRQ_UART4	42
#define IRQ_AOSD        43
#define IRQ_HARB2	44
#define IRQ_I2S2	45
#define IRQ_CPM		47
#define INTC_IRQ_NUM	48

// 2nd-level interrupts

#define DMA_IRQ_NUM	12
#define MDMA_IRQ_NUM	3
#define BDMA_IRQ_NUM	3
#define SADC_IRQ_NUM	6
#define GPIO_IRQ_NUM	192	/* GPIO NUM: 192, Jz4770 real num GPIO 178 */

#define IRQ_DMA_0	64    /* 64,65,66,67,68,69 */
#define IRQ_DMA_1	(IRQ_DMA_0 + 6) /* 70,71,72,73,74,75 */
#define IRQ_MDMA_0	(IRQ_DMA_0 + DMA_IRQ_NUM)   /* 76,77,78 */
#define IRQ_BDMA_0	(IRQ_MDMA_0 + MDMA_IRQ_NUM) /* 79,80,81 */
#define IRQ_SADC_BASE	(IRQ_BDMA_0 + BDMA_IRQ_NUM) /* 82-87 */

#define IRQ_GPIO_0	(IRQ_SADC_BASE + SADC_IRQ_NUM)

// Coprocessor 0 interrupts

#define IRQ_VPU         48

#endif

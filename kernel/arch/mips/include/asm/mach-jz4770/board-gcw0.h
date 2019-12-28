/*
 * board-gcw0.h  -  GCW Zero: JZ4770-based handheld game console
 *
 * File based on Pisces board definition.
 * Copyright (C) 2008 Ingenic Semiconductor Inc.
 * Original author: <cwjia@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_JZ4770_GCW0_H__
#define __ASM_JZ4770_GCW0_H__

/*======================================================================
 * Frequencies of on-board oscillators
 */
#define JZ_EXTAL		12000000  /* Main extal freq:	12 MHz */
#define JZ_EXTAL2		32768     /* RTC extal freq:	32.768 KHz */

/*======================================================================
 * GPIO
 */
#define OTG_HOTPLUG_PIN         (32 + 5)
#define GPIO_OTG_ID_PIN         (32*5+18)
#define OTG_HOTPLUG_IRQ         (IRQ_GPIO_0 + OTG_HOTPLUG_PIN)
#define GPIO_OTG_ID_IRQ         (IRQ_GPIO_0 + GPIO_OTG_ID_PIN)
#define GPIO_OTG_STABLE_JIFFIES 10

#define GPIO_POWER_ON           (32 * 0 + 30)  /* GPA30 */
#define ACTIVE_LOW_WAKE_UP 	1

#if 1
/* use uart2 as default */
#define JZ_BOOTUP_UART_TXD	(32 * 2 + 30)
#define JZ_BOOTUP_UART_RXD	(32 * 2 + 28)
#define JZ_EARLY_UART_BASE	UART2_BASE
#else
#define JZ_BOOTUP_UART_TXD	(32 * 4 + 5)
#define JZ_BOOTUP_UART_RXD	(32 * 3 + 12)
#define JZ_EARLY_UART_BASE	UART3_BASE
#endif

#endif /* __ASM_JZ4770_GCW0_H__ */

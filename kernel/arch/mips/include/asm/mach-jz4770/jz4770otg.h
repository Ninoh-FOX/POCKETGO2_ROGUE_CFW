/*
 * linux/include/asm-mips/mach-jz4770/jz4770otg.h
 *
 * JZ4770 OTG register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770OTG_H__
#define __JZ4770OTG_H__

#include <asm/addrspace.h>
#include <asm/mach-jz4770/base.h>

/*************************************************************************
 * USB Device
 *************************************************************************/
#define USB_BASE		CKSEG1ADDR(JZ4770_UDC_BASE_ADDR)

#define USB_REG_FADDR		(USB_BASE + 0x00) /* Function Address 8-bit */
#define USB_REG_POWER		(USB_BASE + 0x01) /* Power Managemetn 8-bit */
#define USB_REG_INTRIN		(USB_BASE + 0x02) /* Interrupt IN 16-bit */
#define USB_REG_INTROUT		(USB_BASE + 0x04) /* Interrupt OUT 16-bit */
#define USB_REG_INTRINE		(USB_BASE + 0x06) /* Intr IN enable 16-bit */
#define USB_REG_INTROUTE	(USB_BASE + 0x08) /* Intr OUT enable 16-bit */
#define USB_REG_INTRUSB		(USB_BASE + 0x0a) /* Interrupt USB 8-bit */
#define USB_REG_INTRUSBE	(USB_BASE + 0x0b) /* Interrupt USB Enable 8-bit */
#define USB_REG_FRAME		(USB_BASE + 0x0c) /* Frame number 16-bit */
#define USB_REG_INDEX		(USB_BASE + 0x0e) /* Index register 8-bit */
#define USB_REG_TESTMODE	(USB_BASE + 0x0f) /* USB test mode 8-bit */

#define USB_REG_CSR0		(USB_BASE + 0x12) /* EP0 CSR 8-bit */
#define USB_REG_INMAXP		(USB_BASE + 0x10) /* EP1-2 IN Max Pkt Size 16-bit */
#define USB_REG_INCSR		(USB_BASE + 0x12) /* EP1-2 IN CSR LSB 8/16bit */
#define USB_REG_INCSRH		(USB_BASE + 0x13) /* EP1-2 IN CSR MSB 8-bit */
#define USB_REG_OUTMAXP		(USB_BASE + 0x14) /* EP1 OUT Max Pkt Size 16-bit */
#define USB_REG_OUTCSR		(USB_BASE + 0x16) /* EP1 OUT CSR LSB 8/16bit */
#define USB_REG_OUTCSRH		(USB_BASE + 0x17) /* EP1 OUT CSR MSB 8-bit */
#define USB_REG_OUTCOUNT	(USB_BASE + 0x18) /* bytes in EP0/1 OUT FIFO 16-bit */

#define USB_FIFO_EP0		(USB_BASE + 0x20)
#define USB_FIFO_EP1		(USB_BASE + 0x24)
#define USB_FIFO_EP2		(USB_BASE + 0x28)

#define USB_REG_EPINFO		(USB_BASE + 0x78) /* Endpoint information */
#define USB_REG_RAMINFO		(USB_BASE + 0x79) /* RAM information */

#define USB_REG_INTR		(USB_BASE + 0x200) /* DMA pending interrupts */
#define USB_REG_CNTL1		(USB_BASE + 0x204) /* DMA channel 1 control */
#define USB_REG_ADDR1		(USB_BASE + 0x208) /* DMA channel 1 AHB memory addr */
#define USB_REG_COUNT1		(USB_BASE + 0x20c) /* DMA channel 1 byte count */
#define USB_REG_CNTL2		(USB_BASE + 0x214) /* DMA channel 2 control */
#define USB_REG_ADDR2		(USB_BASE + 0x218) /* DMA channel 2 AHB memory addr */
#define USB_REG_COUNT2		(USB_BASE + 0x21c) /* DMA channel 2 byte count */


/* Power register bit masks */
#define USB_POWER_SUSPENDM	0x01
#define USB_POWER_RESUME	0x04
#define USB_POWER_HSMODE	0x10
#define USB_POWER_HSENAB	0x20
#define USB_POWER_SOFTCONN	0x40

/* Interrupt register bit masks */
#define USB_INTR_SUSPEND	0x01
#define USB_INTR_RESUME		0x02
#define USB_INTR_RESET		0x04

#define USB_INTR_EP0		0x0001
#define USB_INTR_INEP1		0x0002
#define USB_INTR_INEP2		0x0004
#define USB_INTR_OUTEP1		0x0002

/* CSR0 bit masks */
#define USB_CSR0_OUTPKTRDY	0x01
#define USB_CSR0_INPKTRDY	0x02
#define USB_CSR0_SENTSTALL	0x04
#define USB_CSR0_DATAEND	0x08
#define USB_CSR0_SETUPEND	0x10
#define USB_CSR0_SENDSTALL	0x20
#define USB_CSR0_SVDOUTPKTRDY	0x40
#define USB_CSR0_SVDSETUPEND	0x80

/* Endpoint CSR register bits */
#define USB_INCSRH_AUTOSET	0x80
#define USB_INCSRH_ISO		0x40
#define USB_INCSRH_MODE		0x20
#define USB_INCSRH_DMAREQENAB	0x10
#define USB_INCSRH_DMAREQMODE	0x04
#define USB_INCSR_CDT		0x40
#define USB_INCSR_SENTSTALL	0x20
#define USB_INCSR_SENDSTALL	0x10
#define USB_INCSR_FF		0x08
#define USB_INCSR_UNDERRUN	0x04
#define USB_INCSR_FFNOTEMPT	0x02
#define USB_INCSR_INPKTRDY	0x01
#define USB_OUTCSRH_AUTOCLR	0x80
#define USB_OUTCSRH_ISO		0x40
#define USB_OUTCSRH_DMAREQENAB	0x20
#define USB_OUTCSRH_DNYT	0x10
#define USB_OUTCSRH_DMAREQMODE	0x08
#define USB_OUTCSR_CDT		0x80
#define USB_OUTCSR_SENTSTALL	0x40
#define USB_OUTCSR_SENDSTALL	0x20
#define USB_OUTCSR_FF		0x10
#define USB_OUTCSR_DATAERR	0x08
#define USB_OUTCSR_OVERRUN	0x04
#define USB_OUTCSR_FFFULL	0x02
#define USB_OUTCSR_OUTPKTRDY	0x01

/* Testmode register bits */
#define USB_TEST_SE0NAK		0x01
#define USB_TEST_J		0x02
#define USB_TEST_K		0x04
#define USB_TEST_PACKET		0x08

/* DMA control bits */
#define USB_CNTL_ENA		0x01
#define USB_CNTL_DIR_IN		0x02
#define USB_CNTL_MODE_1		0x04
#define USB_CNTL_INTR_EN	0x08
#define USB_CNTL_EP(n)		((n) << 4)
#define USB_CNTL_BURST_0	(0 << 9)
#define USB_CNTL_BURST_4	(1 << 9)
#define USB_CNTL_BURST_8	(2 << 9)
#define USB_CNTL_BURST_16	(3 << 9)


#endif /* __JZ4770OTG_H__ */

/*
 * linux/include/asm-mips/mach-jz4770/jz4770uart.h
 *
 * JZ4770 UART register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770UART_H__
#define __JZ4770UART_H__


#define	UART0_BASE	0xB0030000
#define	UART1_BASE	0xB0031000
#define	UART2_BASE	0xB0032000
#define	UART3_BASE	0xB0033000

/*************************************************************************
 * UART
 *************************************************************************/

/* Register Offset */
#define OFF_RDR		(0x00)	/* R  8b H'xx */
#define OFF_TDR		(0x00)	/* W  8b H'xx */
#define OFF_DLLR	(0x00)	/* RW 8b H'00 */
#define OFF_DLHR	(0x04)	/* RW 8b H'00 */
#define OFF_IER		(0x04)	/* RW 8b H'00 */
#define OFF_ISR		(0x08)	/* R  8b H'01 */
#define OFF_FCR		(0x08)	/* W  8b H'00 */
#define OFF_LCR		(0x0C)	/* RW 8b H'00 */
#define OFF_MCR		(0x10)	/* RW 8b H'00 */
#define OFF_LSR		(0x14)	/* R  8b H'00 */
#define OFF_MSR		(0x18)	/* R  8b H'00 */
#define OFF_SPR		(0x1C)	/* RW 8b H'00 */
#define OFF_SIRCR	(0x20)	/* RW 8b H'00, UART0 */
#define OFF_UMR		(0x24)	/* RW 8b H'00, UART M Register */
#define OFF_UACR	(0x28)	/* RW 8b H'00, UART Add Cycle Register */

/* Register Address */
#define UART0_RDR	(UART0_BASE + OFF_RDR)
#define UART0_TDR	(UART0_BASE + OFF_TDR)
#define UART0_DLLR	(UART0_BASE + OFF_DLLR)
#define UART0_DLHR	(UART0_BASE + OFF_DLHR)
#define UART0_IER	(UART0_BASE + OFF_IER)
#define UART0_ISR	(UART0_BASE + OFF_ISR)
#define UART0_FCR	(UART0_BASE + OFF_FCR)
#define UART0_LCR	(UART0_BASE + OFF_LCR)
#define UART0_MCR	(UART0_BASE + OFF_MCR)
#define UART0_LSR	(UART0_BASE + OFF_LSR)
#define UART0_MSR	(UART0_BASE + OFF_MSR)
#define UART0_SPR	(UART0_BASE + OFF_SPR)
#define UART0_SIRCR	(UART0_BASE + OFF_SIRCR)
#define UART0_UMR	(UART0_BASE + OFF_UMR)
#define UART0_UACR	(UART0_BASE + OFF_UACR)

#define UART1_RDR	(UART1_BASE + OFF_RDR)
#define UART1_TDR	(UART1_BASE + OFF_TDR)
#define UART1_DLLR	(UART1_BASE + OFF_DLLR)
#define UART1_DLHR	(UART1_BASE + OFF_DLHR)
#define UART1_IER	(UART1_BASE + OFF_IER)
#define UART1_ISR	(UART1_BASE + OFF_ISR)
#define UART1_FCR	(UART1_BASE + OFF_FCR)
#define UART1_LCR	(UART1_BASE + OFF_LCR)
#define UART1_MCR	(UART1_BASE + OFF_MCR)
#define UART1_LSR	(UART1_BASE + OFF_LSR)
#define UART1_MSR	(UART1_BASE + OFF_MSR)
#define UART1_SPR	(UART1_BASE + OFF_SPR)
#define UART1_SIRCR	(UART1_BASE + OFF_SIRCR)

#define UART2_RDR	(UART2_BASE + OFF_RDR)
#define UART2_TDR	(UART2_BASE + OFF_TDR)
#define UART2_DLLR	(UART2_BASE + OFF_DLLR)
#define UART2_DLHR	(UART2_BASE + OFF_DLHR)
#define UART2_IER	(UART2_BASE + OFF_IER)
#define UART2_ISR	(UART2_BASE + OFF_ISR)
#define UART2_FCR	(UART2_BASE + OFF_FCR)
#define UART2_LCR	(UART2_BASE + OFF_LCR)
#define UART2_MCR	(UART2_BASE + OFF_MCR)
#define UART2_LSR	(UART2_BASE + OFF_LSR)
#define UART2_MSR	(UART2_BASE + OFF_MSR)
#define UART2_SPR	(UART2_BASE + OFF_SPR)
#define UART2_SIRCR	(UART2_BASE + OFF_SIRCR)

#define UART3_RDR	(UART3_BASE + OFF_RDR)
#define UART3_TDR	(UART3_BASE + OFF_TDR)
#define UART3_DLLR	(UART3_BASE + OFF_DLLR)
#define UART3_DLHR	(UART3_BASE + OFF_DLHR)
#define UART3_IER	(UART3_BASE + OFF_IER)
#define UART3_ISR	(UART3_BASE + OFF_ISR)
#define UART3_FCR	(UART3_BASE + OFF_FCR)
#define UART3_LCR	(UART3_BASE + OFF_LCR)
#define UART3_MCR	(UART3_BASE + OFF_MCR)
#define UART3_LSR	(UART3_BASE + OFF_LSR)
#define UART3_MSR	(UART3_BASE + OFF_MSR)
#define UART3_SPR	(UART3_BASE + OFF_SPR)
#define UART3_SIRCR	(UART3_BASE + OFF_SIRCR)

#endif /* __JZ4770UART_H__ */

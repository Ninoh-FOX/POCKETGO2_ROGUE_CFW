/*
 * linux/include/asm-mips/mach-jz4770/jz4770intc.h
 *
 * JZ4770 INTC register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770INTC_H__
#define __JZ4770INTC_H__

#include <asm/addrspace.h>
#include <asm/mach-jz4770/base.h>
#include <asm/mach-jz4770/jz4770misc.h>


#define	INTC_BASE	CKSEG1ADDR(JZ4770_INTC_BASE_ADDR)

/*************************************************************************
 * INTC (Interrupt Controller)
 *************************************************************************/
/* n = 0 ~ 1 */
#define INTC_ISR(n)	(INTC_BASE + 0x00 + (n) * 0x20)
#define INTC_IMR(n)	(INTC_BASE + 0x04 + (n) * 0x20)
#define INTC_ICMR(n)    INTC_IMR(n)
#define INTC_IMSR(n)	(INTC_BASE + 0x08 + (n) * 0x20)
#define INTC_ICMSR(n)   INTC_IMSR(n)
#define INTC_IMCR(n)	(INTC_BASE + 0x0c + (n) * 0x20)
#define INTC_ICMCR(n)	INTC_IMCR(n)
#define INTC_IPR(n)	(INTC_BASE + 0x10 + (n) * 0x20)
//#define INTC_ISSR	(INTC_BASE + 0x18)  /* Interrupt Controller Source Set Register */
//#define INTC_ISCR	(INTC_BASE + 0x1c)  /* Interrupt Controller Source Clear Register */

#define REG_INTC_ISR(n)		REG32(INTC_ISR(n))
#define REG_INTC_IMR(n)		REG32(INTC_IMR(n))
#define REG_INTC_IMSR(n)	REG32(INTC_IMSR(n))
#define REG_INTC_IMCR(n)	REG32(INTC_IMCR(n))
#define REG_INTC_IPR(n)		REG32(INTC_IPR(n))
//#define REG_INTC_ISSR   REG32(INTC_ISSR)
//#define REG_INTC_ISCR   REG32(INTC_ISCR)


#ifndef __MIPS_ASSEMBLER


/***************************************************************************
 * INTC
 ***************************************************************************/
#define __intc_unmask_irq(n)	(REG_INTC_IMCR((n)/32) = (1 << ((n)%32)))
#define __intc_mask_irq(n)	(REG_INTC_IMSR((n)/32) = (1 << ((n)%32)))
#define __intc_ack_irq(n)	(REG_INTC_IPR((n)/32) = (1 << ((n)%32))) /* A dummy ack, as the Pending Register is Read Only. Should we remove __intc_ack_irq() */


#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4770INTC_H__ */

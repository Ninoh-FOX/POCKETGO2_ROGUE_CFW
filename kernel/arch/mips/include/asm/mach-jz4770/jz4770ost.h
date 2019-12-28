/*
 * jz4770ost.h
 * JZ4770 operating system timer (OST) register definition
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 *
 * Author: whxu@ingenic.cn
 */

#ifndef __JZ4770OST_H__
#define __JZ4770OST_H__

#include <asm/mach-jz4770/jz4770misc.h>


#define	OST_BASE	0xB0002000

/*
 * OST registers offset address definition
 */
#define OST_OSTDR_OFFSET	(0xe0)  /* rw, 32, 0x???????? */
#define OST_OSTCNTL_OFFSET	(0xe4)
#define OST_OSTCNTH_OFFSET	(0xe8)
#define OST_OSTCSR_OFFSET	(0xec)  /* rw, 16, 0x0000 */

#define OST_OSTCNTH_BUF_OFFSET	(0xfc)


/*
 * OST registers address definition
 */
#define	OST_BASE	0xB0002000
#define OST_OSTDR	(OST_BASE + OST_OSTDR_OFFSET)
#define OST_OSTCNTL	(OST_BASE + OST_OSTCNTL_OFFSET)
#define OST_OSTCNTH	(OST_BASE + OST_OSTCNTH_OFFSET)
#define OST_OSTCSR	(OST_BASE + OST_OSTCSR_OFFSET)

#define OST_OSTCNTH_BUF	(OST_BASE + OST_OSTCNTH_BUF_OFFSET)


/*
 * OST registers common define
 */

/* Operating system control register(OSTCSR) */
#define OSTCSR_CNT_MD		BIT15
#define OSTCSR_SD		BIT9
#define OSTCSR_EXT_EN		BIT2
#define OSTCSR_RTC_EN		BIT1
#define OSTCSR_PCK_EN		BIT0

#define OSTCSR_PRESCALE_LSB	3
#define OSTCSR_PRESCALE_MASK	BITS_H2L(5, OSTCSR_PRESCALE_LSB)
#define OSTCSR_PRESCALE1	(0x0 << OSTCSR_PRESCALE_LSB)
#define OSTCSR_PRESCALE4	(0x1 << OSTCSR_PRESCALE_LSB)
#define OSTCSR_PRESCALE16	(0x2 << OSTCSR_PRESCALE_LSB)
#define OSTCSR_PRESCALE64	(0x3 << OSTCSR_PRESCALE_LSB)
#define OSTCSR_PRESCALE256	(0x4 << OSTCSR_PRESCALE_LSB)
#define OSTCSR_PRESCALE1024	(0x5 << OSTCSR_PRESCALE_LSB)


#ifndef __MIPS_ASSEMBLER

#define REG_OST_OSTDR		REG32(OST_OSTDR)

#define REG_OST_OSTCNTL		REG32(OST_OSTCNTL)
#define REG_OST_OSTCNTH		REG32(OST_OSTCNTH)

#define REG_OST_OSTCSR		REG16(OST_OSTCSR)

#define REG_OST_OSTCNTH_BUF	REG32(OST_OSTCNTH_BUF)

#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4770OST_H__ */

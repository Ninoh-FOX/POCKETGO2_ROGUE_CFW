/*
 * linux/include/asm-mips/mach-jz4770/jz4770tcu.h
 *
 * JZ4770 tcu register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770TCU_H__
#define __JZ4770TCU_H__

#include <asm/addrspace.h>
#include <asm/mach-jz4770/base.h>
#include <asm/mach-jz4770/jz4770misc.h>


#define	TCU_BASE	CKSEG1ADDR(JZ4770_TCU_BASE_ADDR)


/*************************************************************************
 * TCU (Timer Counter Unit)
 *************************************************************************/
#define TCU_TSTR	(TCU_BASE + 0xE0) /* Timer Status Register,Only Used In Tcu2 Mode */
#define TCU_TSTSR	(TCU_BASE + 0xE4) /* Timer Status Set Register */
#define TCU_TSTCR	(TCU_BASE + 0xE8) /* Timer Status Clear Register */
#define TCU_TSR		(TCU_BASE + 0x0C) /* Timer Stop Register */
#define TCU_TSSR	(TCU_BASE + 0x1C) /* Timer Stop Set Register */
#define TCU_TSCR	(TCU_BASE + 0x2C) /* Timer Stop Clear Register */
#define TCU_TER		(TCU_BASE + 0x00) /* Timer Counter Enable Register */
#define TCU_TESR	(TCU_BASE + 0x04) /* Timer Counter Enable Set Register */
#define TCU_TECR	(TCU_BASE + 0x08) /* Timer Counter Enable Clear Register */
#define TCU_TFR		(TCU_BASE + 0x10) /* Timer Flag Register */
#define TCU_TFSR	(TCU_BASE + 0x14) /* Timer Flag Set Register */
#define TCU_TFCR	(TCU_BASE + 0x18) /* Timer Flag Clear Register */
#define TCU_TMR		(TCU_BASE + 0x20) /* Timer Mask Register */
#define TCU_TMSR	(TCU_BASE + 0x24) /* Timer Mask Set Register */
#define TCU_TMCR	(TCU_BASE + 0x28) /* Timer Mask Clear Register */

#define TCU_TDFR0	(TCU_BASE + 0x30) /* Timer Data Full Register */
#define TCU_TDHR0	(TCU_BASE + 0x34) /* Timer Data Half Register */
#define TCU_TCNT0	(TCU_BASE + 0x38) /* Timer Counter Register */
#define TCU_TCSR0	(TCU_BASE + 0x3C) /* Timer Control Register */
#define TCU_TDFR1	(TCU_BASE + 0x40)
#define TCU_TDHR1	(TCU_BASE + 0x44)
#define TCU_TCNT1	(TCU_BASE + 0x48)
#define TCU_TCSR1	(TCU_BASE + 0x4C)
#define TCU_TDFR2	(TCU_BASE + 0x50)
#define TCU_TDHR2	(TCU_BASE + 0x54)
#define TCU_TCNT2	(TCU_BASE + 0x58)
#define TCU_TCSR2	(TCU_BASE + 0x5C)
#define TCU_TDFR3	(TCU_BASE + 0x60)
#define TCU_TDHR3	(TCU_BASE + 0x64)
#define TCU_TCNT3	(TCU_BASE + 0x68)
#define TCU_TCSR3	(TCU_BASE + 0x6C)
#define TCU_TDFR4	(TCU_BASE + 0x70)
#define TCU_TDHR4	(TCU_BASE + 0x74)
#define TCU_TCNT4	(TCU_BASE + 0x78)
#define TCU_TCSR4	(TCU_BASE + 0x7C)
#define TCU_TDFR5	(TCU_BASE + 0x80)
#define TCU_TDHR5	(TCU_BASE + 0x84)
#define TCU_TCNT5	(TCU_BASE + 0x88)
#define TCU_TCSR5	(TCU_BASE + 0x8C)
///////////////////////////////////////////new register for jz4810
#define TCU_MOD(n)	(TCU_BASE + 0xF0 + ((n) ? (((n) - 2) * 0x10) : 0x0))
#define TCU_TFWD(n)	(TCU_BASE + 0xF4 + ((n) ? (((n) - 2) * 0x10) : 0x0))
#define TCU_TFIFOSR(n)	(TCU_BASE + 0xF8 + ((n) ? (((n) - 2) * 0x10) : 0x0))

#define REG_TCU_TSTR	REG32(TCU_TSTR)
#define REG_TCU_TSTSR	REG32(TCU_TSTSR)
#define REG_TCU_TSTCR	REG32(TCU_TSTCR)
#define REG_TCU_TSR	REG32(TCU_TSR)
#define REG_TCU_TSSR	REG32(TCU_TSSR)
#define REG_TCU_TSCR	REG32(TCU_TSCR)
#define REG_TCU_TER	REG16(TCU_TER)
#define REG_TCU_TESR	REG32(TCU_TESR)
#define REG_TCU_TECR	REG32(TCU_TECR)
#define REG_TCU_TFR	REG32(TCU_TFR)
#define REG_TCU_TFSR	REG32(TCU_TFSR)
#define REG_TCU_TFCR	REG32(TCU_TFCR)
#define REG_TCU_TMR	REG32(TCU_TMR)
#define REG_TCU_TMSR	REG32(TCU_TMSR)
#define REG_TCU_TMCR	REG32(TCU_TMCR)
#define REG_TCU_TDFR0	REG16(TCU_TDFR0)
#define REG_TCU_TDHR0	REG16(TCU_TDHR0)
#define REG_TCU_TCNT0	REG16(TCU_TCNT0)
#define REG_TCU_TCSR0	REG16(TCU_TCSR0)
#define REG_TCU_TDFR1	REG16(TCU_TDFR1)
#define REG_TCU_TDHR1	REG16(TCU_TDHR1)
#define REG_TCU_TCNT1	REG16(TCU_TCNT1)
#define REG_TCU_TCSR1	REG16(TCU_TCSR1)
#define REG_TCU_TDFR2	REG16(TCU_TDFR2)
#define REG_TCU_TDHR2	REG16(TCU_TDHR2)
#define REG_TCU_TCNT2	REG16(TCU_TCNT2)
#define REG_TCU_TCSR2	REG16(TCU_TCSR2)
#define REG_TCU_TDFR3	REG16(TCU_TDFR3)
#define REG_TCU_TDHR3	REG16(TCU_TDHR3)
#define REG_TCU_TCNT3	REG16(TCU_TCNT3)
#define REG_TCU_TCSR3	REG16(TCU_TCSR3)
#define REG_TCU_TDFR4	REG16(TCU_TDFR4)
#define REG_TCU_TDHR4	REG16(TCU_TDHR4)
#define REG_TCU_TCNT4	REG16(TCU_TCNT4)
#define REG_TCU_TCSR4	REG16(TCU_TCSR4)
///////////////////////////////////////////new register for jz4770
#define REG_TCU_MOD(n)		REG32(TCU_MOD((n)))
#define REG_TCU_TFWD(n)		REG32(TCU_TFWD((n)))
#define REG_TCU_TFIFOSR(n)	REG32(TCU_TFIFOSR((n)))

// n = 0,1,2,3,4,5
#define TCU_TDFR(n)	(TCU_BASE + (0x30 + (n)*0x10)) /* Timer Data Full Reg */
#define TCU_TDHR(n)	(TCU_BASE + (0x34 + (n)*0x10)) /* Timer Data Half Reg */
#define TCU_TCNT(n)	(TCU_BASE + (0x38 + (n)*0x10)) /* Timer Counter Reg */
#define TCU_TCSR(n)	(TCU_BASE + (0x3C + (n)*0x10)) /* Timer Control Reg */
#define TCU_OSTDR	(TCU_BASE + 0xD0) /* Operating System Timer Data Reg */
#define TCU_OSTCNT	(TCU_BASE + 0xD4) /* Operating System Timer Counter Reg */
#define TCU_OSTCSR	(TCU_BASE + 0xDC) /* Operating System Timer Control Reg */

#define REG_TCU_TDFR(n)	REG16(TCU_TDFR((n)))
#define REG_TCU_TDHR(n)	REG16(TCU_TDHR((n)))
#define REG_TCU_TCNT(n)	REG16(TCU_TCNT((n)))
#define REG_TCU_TCSR(n)	REG16(TCU_TCSR((n)))
#define REG_TCU_OSTDR   REG32(TCU_OSTDR)
#define REG_TCU_OSTCNT  REG32(TCU_OSTCNT)
#define REG_TCU_OSTCSR  REG32(TCU_OSTCSR)

// Register definitions
#define TCU_TSTR_REAL2		(1 << 18) /* only used in TCU2 mode */
#define TCU_TSTR_REAL1		(1 << 17) /* only used in TCU2 mode */
#define TCU_TSTR_BUSY2		(1 << 2)  /* only used in TCU2 mode */
#define TCU_TSTR_BUSY1		(1 << 1)  /* only used in TCU2 mode */

#define TCU_TSTSR_REAL2		(1 << 18)
#define TCU_TSTSR_REAL1		(1 << 17)
#define TCU_TSTSR_BUSY2		(1 << 2)
#define TCU_TSTSR_BUSY1		(1 << 1)

#define TCU_TSTCR_REAL2		(1 << 18)
#define TCU_TSTCR_REAL1		(1 << 17)
#define TCU_TSTCR_BUSY2		(1 << 2)
#define TCU_TSTCR_BUSY1		(1 << 1)

#define TCU_TSR_WDTS		(1 << 16) /*the clock supplies to wdt is stopped */
#define TCU_TSR_OSTS		(1 << 15) /*the clock supplies to osts is stopped */
#define TCU_TSR_STOP5		(1 << 5)  /*the clock supplies to timer5 is stopped */
#define TCU_TSR_STOP4		(1 << 4)  /*the clock supplies to timer4 is stopped */
#define TCU_TSR_STOP3		(1 << 3)  /*the clock supplies to timer3 is stopped */
#define TCU_TSR_STOP2		(1 << 2)  /*the clock supplies to timer2 is stopped */
#define TCU_TSR_STOP1		(1 << 1)  /*the clock supplies to timer1 is stopped */
#define TCU_TSR_STOP0		(1 << 0)  /*the clock supplies to timer0 is stopped */

#define TCU_TSSR_WDTSS		(1 << 16)
#define TCU_TSSR_OSTSS		(1 << 15)
#define TCU_TSSR_STPS5		(1 << 5)
#define TCU_TSSR_STPS4		(1 << 4)
#define TCU_TSSR_STPS3		(1 << 3)
#define TCU_TSSR_STPS2		(1 << 2)
#define TCU_TSSR_STPS1		(1 << 1)
#define TCU_TSSR_STPS0		(1 << 0)

#define TCU_TSCR_WDTSC		(1 << 16)
#define TCU_TSCR_OSTSC		(1 << 15)
#define TCU_TSCR_STPC5		(1 << 5)
#define TCU_TSCR_STPC4		(1 << 4)
#define TCU_TSCR_STPC3		(1 << 3)
#define TCU_TSCR_STPC2		(1 << 2)
#define TCU_TSCR_STPC1		(1 << 1)
#define TCU_TSCR_STPC0		(1 << 0)

#define TCU_TER_OSTEN		(1 << 15) /* enable the counter in ost */
#define TCU_TER_TCEN5		(1 << 5)  /* enable the counter in timer5 */
#define TCU_TER_TCEN4		(1 << 4)
#define TCU_TER_TCEN3		(1 << 3)
#define TCU_TER_TCEN2		(1 << 2)
#define TCU_TER_TCEN1		(1 << 1)
#define TCU_TER_TCEN0		(1 << 0)

#define TCU_TESR_OSTST		(1 << 15)
#define TCU_TESR_TCST5		(1 << 5)
#define TCU_TESR_TCST4		(1 << 4)
#define TCU_TESR_TCST3		(1 << 3)
#define TCU_TESR_TCST2		(1 << 2)
#define TCU_TESR_TCST1		(1 << 1)
#define TCU_TESR_TCST0		(1 << 0)

#define TCU_TECR_OSTCL		(1 << 15)
#define TCU_TECR_TCCL5		(1 << 5)
#define TCU_TECR_TCCL4		(1 << 4)
#define TCU_TECR_TCCL3		(1 << 3)
#define TCU_TECR_TCCL2		(1 << 2)
#define TCU_TECR_TCCL1		(1 << 1)
#define TCU_TECR_TCCL0		(1 << 0)

#define TCU_TFR_HFLAG5		(1 << 21) /* half comparison match flag */
#define TCU_TFR_HFLAG4		(1 << 20)
#define TCU_TFR_HFLAG3		(1 << 19)
#define TCU_TFR_HFLAG2		(1 << 18)
#define TCU_TFR_HFLAG1		(1 << 17)
#define TCU_TFR_HFLAG0		(1 << 16)
#define TCU_TFR_OSTFLAG		(1 << 15) /* ost comparison match flag */
#define TCU_TFR_FFLAG5		(1 << 5)  /* full comparison match flag */
#define TCU_TFR_FFLAG4		(1 << 4)
#define TCU_TFR_FFLAG3		(1 << 3)
#define TCU_TFR_FFLAG2		(1 << 2)
#define TCU_TFR_FFLAG1		(1 << 1)
#define TCU_TFR_FFLAG0		(1 << 0)

#define TCU_TFSR_HFST5		(1 << 21)
#define TCU_TFSR_HFST4		(1 << 20)
#define TCU_TFSR_HFST3		(1 << 19)
#define TCU_TFSR_HFST2		(1 << 18)
#define TCU_TFSR_HFST1		(1 << 17)
#define TCU_TFSR_HFST0		(1 << 16)
#define TCU_TFSR_OSTFST		(1 << 15)
#define TCU_TFSR_FFST5		(1 << 5)
#define TCU_TFSR_FFST4		(1 << 4)
#define TCU_TFSR_FFST3		(1 << 3)
#define TCU_TFSR_FFST2		(1 << 2)
#define TCU_TFSR_FFST1		(1 << 1)
#define TCU_TFSR_FFST0		(1 << 0)

#define TCU_TFCR_HFCL5		(1 << 21)
#define TCU_TFCR_HFCL4		(1 << 20)
#define TCU_TFCR_HFCL3		(1 << 19)
#define TCU_TFCR_HFCL2		(1 << 18)
#define TCU_TFCR_HFCL1		(1 << 17)
#define TCU_TFCR_HFCL0		(1 << 16)
#define TCU_TFCR_OSTFCL		(1 << 15)
#define TCU_TFCR_FFCL5		(1 << 5)
#define TCU_TFCR_FFCL4		(1 << 4)
#define TCU_TFCR_FFCL3		(1 << 3)
#define TCU_TFCR_FFCL2		(1 << 2)
#define TCU_TFCR_FFCL1		(1 << 1)
#define TCU_TFCR_FFCL0		(1 << 0)

#define TCU_TMR_HMASK5		(1 << 21) /* half comparison match interrupt mask */
#define TCU_TMR_HMASK4		(1 << 20)
#define TCU_TMR_HMASK3		(1 << 19)
#define TCU_TMR_HMASK2		(1 << 18)
#define TCU_TMR_HMASK1		(1 << 17)
#define TCU_TMR_HMASK0		(1 << 16)
#define TCU_TMR_OSTMASK		(1 << 15) /* ost comparison match interrupt mask */
#define TCU_TMR_FMASK5		(1 << 5)  /* full comparison match interrupt mask */
#define TCU_TMR_FMASK4		(1 << 4)
#define TCU_TMR_FMASK3		(1 << 3)
#define TCU_TMR_FMASK2		(1 << 2)
#define TCU_TMR_FMASK1		(1 << 1)
#define TCU_TMR_FMASK0		(1 << 0)

#define TCU_TMSR_HMST5		(1 << 21)
#define TCU_TMSR_HMST4		(1 << 20)
#define TCU_TMSR_HMST3		(1 << 19)
#define TCU_TMSR_HMST2		(1 << 18)
#define TCU_TMSR_HMST1		(1 << 17)
#define TCU_TMSR_HMST0		(1 << 16)
#define TCU_TMSR_OSTMST		(1 << 15)
#define TCU_TMSR_FMST5		(1 << 5)
#define TCU_TMSR_FMST4		(1 << 4)
#define TCU_TMSR_FMST3		(1 << 3)
#define TCU_TMSR_FMST2		(1 << 2)
#define TCU_TMSR_FMST1		(1 << 1)
#define TCU_TMSR_FMST0		(1 << 0)

#define TCU_TMCR_HMCL5		(1 << 21)
#define TCU_TMCR_HMCL4		(1 << 20)
#define TCU_TMCR_HMCL3		(1 << 19)
#define TCU_TMCR_HMCL2		(1 << 18)
#define TCU_TMCR_HMCL1		(1 << 17)
#define TCU_TMCR_HMCL0		(1 << 16)
#define TCU_TMCR_OSTMCL		(1 << 15)
#define TCU_TMCR_FMCL5		(1 << 5)
#define TCU_TMCR_FMCL4		(1 << 4)
#define TCU_TMCR_FMCL3		(1 << 3)
#define TCU_TMCR_FMCL2		(1 << 2)
#define TCU_TMCR_FMCL1		(1 << 1)
#define TCU_TMCR_FMCL0		(1 << 0)

#define TCU_TCSR_CNT_CLRZ	(1 << 10) /* clear counter to 0, only used in TCU2 mode */
#define TCU_TCSR_PWM_SD		(1 << 9)  /* shut down the pwm output only used in TCU1 mode */
#define TCU_TCSR_PWM_INITL_HIGH	(1 << 8)  /* selects an initial output level for pwm output */
#define TCU_TCSR_PWM_EN		(1 << 7)  /* pwm pin output enable */
#define TCU_TCSR_IN_EN		(1 << 6)  /* PWM input mode enable */
#define TCU_TCSR_PRESCALE_BIT	3         /* select the tcnt count clock frequency*/
#define TCU_TCSR_PRESCALE_MASK	(0x7 << TCU_TCSR_PRESCALE_BIT)
  #define TCU_TCSR_PRESCALE1	(0x0 << TCU_TCSR_PRESCALE_BIT)
  #define TCU_TCSR_PRESCALE4	(0x1 << TCU_TCSR_PRESCALE_BIT)
  #define TCU_TCSR_PRESCALE16	(0x2 << TCU_TCSR_PRESCALE_BIT)
  #define TCU_TCSR_PRESCALE64	(0x3 << TCU_TCSR_PRESCALE_BIT)
  #define TCU_TCSR_PRESCALE256	(0x4 << TCU_TCSR_PRESCALE_BIT)
  #define TCU_TCSR_PRESCALE1024	(0x5 << TCU_TCSR_PRESCALE_BIT)
#define TCU_TCSR_EXT_EN		(1 << 2)  /* select extal as the timer clock input */
#define TCU_TCSR_RTC_EN		(1 << 1)  /* select rtcclk as the timer clock input */
#define TCU_TCSR_PCK_EN		(1 << 0)  /* select pclk as the timer clock input */

#define TCU_TSTSR_REALS2	(1 << 18)
#define TCU_TSTSR_REALS1	(1 << 17)
#define TCU_TSTSR_BUSYS2	(1 << 2)
#define TCU_TSTSR_BUSYS1	(1 << 1)

#define TCU_TSTCR_REALC2	(1 << 18)
#define TCU_TSTCR_REALC1	(1 << 17)
#define TCU_TSTCR_BUSYC2	(1 << 2)
#define TCU_TSTCR_BUSYC1	(1 << 1)

#define TCU_OSTCR_CNT_MD		(1 << 15) /* when the value counter is equal to compare value,the counter is go on increasing till overflow,and then icrease from 0 */
#define TCU_OSTCR_PWM_SD		(1 << 9) /* shut down the pwm output, only used in TCU1 mode */
#define TCU_OSTCSR_PRESCALE_BIT		3
#define TCU_OSTCSR_PRESCALE_MASK	(0x7 << TCU_OSTCSR_PRESCALE_BIT)
  #define TCU_OSTCSR_PRESCALE1		(0x0 << TCU_OSTCSR_PRESCALE_BIT)
  #define TCU_OSTCSR_PRESCALE4		(0x1 << TCU_OSTCSR_PRESCALE_BIT)
  #define TCU_OSTCSR_PRESCALE16		(0x2 << TCU_OSTCSR_PRESCALE_BIT)
  #define TCU_OSTCSR_PRESCALE64		(0x3 << TCU_OSTCSR_PRESCALE_BIT)
  #define TCU_OSTCSR_PRESCALE256	(0x4 << TCU_OSTCSR_PRESCALE_BIT)
  #define TCU_OSTCSR_PRESCALE1024	(0x5 << TCU_OSTCSR_PRESCALE_BIT)
#define TCU_OSTCSR_EXT_EN		(1 << 2) /* select extal as the timer clock input */
#define TCU_OSTCSR_RTC_EN		(1 << 1) /* select rtcclk as the timer clock input */
#define TCU_OSTCSR_PCK_EN		(1 << 0) /* select pclk as the timer clock input */


#ifndef __MIPS_ASSEMBLER

/***************************************************************************
 * TCU
 ***************************************************************************/
// where 'n' is the TCU channel
#define __tcu_disable_all_clk(n) \
	(REG_TCU_TCSR((n)) = (REG_TCU_TCSR((n)) & ~(TCU_TCSR_EXT_EN | TCU_TCSR_RTC_EN | TCU_TCSR_PCK_EN)))
#define __tcu_select_extalclk(n) \
	(REG_TCU_TCSR((n)) = (REG_TCU_TCSR((n)) & ~(TCU_TCSR_EXT_EN | TCU_TCSR_RTC_EN | TCU_TCSR_PCK_EN)) | TCU_TCSR_EXT_EN)
#define __tcu_select_rtcclk(n) \
	(REG_TCU_TCSR((n)) = (REG_TCU_TCSR((n)) & ~(TCU_TCSR_EXT_EN | TCU_TCSR_RTC_EN | TCU_TCSR_PCK_EN)) | TCU_TCSR_RTC_EN)
#define __tcu_select_pclk(n) \
	(REG_TCU_TCSR((n)) = (REG_TCU_TCSR((n)) & ~(TCU_TCSR_EXT_EN | TCU_TCSR_RTC_EN | TCU_TCSR_PCK_EN)) | TCU_TCSR_PCK_EN)
#define __tcu_disable_pclk(n) \
	REG_TCU_TCSR(n) = (REG_TCU_TCSR((n)) & ~TCU_TCSR_PCK_EN);
#define __tcu_select_clk_div(n, c) \
	(REG_TCU_TCSR((n)) = (REG_TCU_TCSR((n)) & ~TCU_TCSR_PRESCALE_MASK) | ((c) << TCU_TCSR_PRESCALE_BIT))
#define __tcu_select_clk_div1(n) \
	(REG_TCU_TCSR((n)) = (REG_TCU_TCSR((n)) & ~TCU_TCSR_PRESCALE_MASK) | TCU_TCSR_PRESCALE1)
#define __tcu_select_clk_div4(n) \
	(REG_TCU_TCSR((n)) = (REG_TCU_TCSR((n)) & ~TCU_TCSR_PRESCALE_MASK) | TCU_TCSR_PRESCALE4)
#define __tcu_select_clk_div16(n) \
	(REG_TCU_TCSR((n)) = (REG_TCU_TCSR((n)) & ~TCU_TCSR_PRESCALE_MASK) | TCU_TCSR_PRESCALE16)
#define __tcu_select_clk_div64(n) \
	(REG_TCU_TCSR((n)) = (REG_TCU_TCSR((n)) & ~TCU_TCSR_PRESCALE_MASK) | TCU_TCSR_PRESCALE64)
#define __tcu_select_clk_div256(n) \
	(REG_TCU_TCSR((n)) = (REG_TCU_TCSR((n)) & ~TCU_TCSR_PRESCALE_MASK) | TCU_TCSR_PRESCALE256)
#define __tcu_select_clk_div1024(n) \
	(REG_TCU_TCSR((n)) = (REG_TCU_TCSR((n)) & ~TCU_TCSR_PRESCALE_MASK) | TCU_TCSR_PRESCALE1024)

#define __tcu_enable_pwm_output(n)	(REG_TCU_TCSR((n)) |= TCU_TCSR_PWM_EN)
#define __tcu_disable_pwm_output(n)	(REG_TCU_TCSR((n)) &= ~TCU_TCSR_PWM_EN)

#define __tcu_init_pwm_output_high(n)	(REG_TCU_TCSR((n)) |= TCU_TCSR_PWM_INITL_HIGH)
#define __tcu_init_pwm_output_low(n)	(REG_TCU_TCSR((n)) &= ~TCU_TCSR_PWM_INITL_HIGH)

#define __tcu_set_pwm_output_shutdown_graceful(n)	(REG_TCU_TCSR((n)) &= ~TCU_TCSR_PWM_SD)
#define __tcu_set_pwm_output_shutdown_abrupt(n)		(REG_TCU_TCSR((n)) |= TCU_TCSR_PWM_SD)

#define __tcu_clear_counter_to_zero(n)	(REG_TCU_TCSR((n)) |= TCU_TCSR_CNT_CLRZ)

#define __tcu_ost_enabled()		(REG_TCU_TER & TCU_TER_OSTEN)
#define __tcu_enable_ost()		(REG_TCU_TESR = TCU_TESR_OSTST)
#define __tcu_disable_ost()		(REG_TCU_TECR = TCU_TECR_OSTCL)

#define __tcu_counter_enabled(n)	(REG_TCU_TER & (1 << (n)))
#define __tcu_start_counter(n)		(REG_TCU_TESR |= (1 << (n)))
#define __tcu_stop_counter(n)		(REG_TCU_TECR |= (1 << (n)))

#define __tcu_half_match_flag(n)	(REG_TCU_TFR & (1 << ((n) + 16)))
#define __tcu_full_match_flag(n)	(REG_TCU_TFR & (1 << (n)))
#define __tcu_set_half_match_flag(n)	(REG_TCU_TFSR = (1 << ((n) + 16)))
#define __tcu_set_full_match_flag(n)	(REG_TCU_TFSR = (1 << (n)))
#define __tcu_clear_half_match_flag(n)	(REG_TCU_TFCR = (1 << ((n) + 16)))
#define __tcu_clear_full_match_flag(n)	(REG_TCU_TFCR = (1 << (n)))
#define __tcu_mask_half_match_irq(n)	(REG_TCU_TMSR = (1 << ((n) + 16)))
#define __tcu_mask_full_match_irq(n)	(REG_TCU_TMSR = (1 << (n)))
#define __tcu_unmask_half_match_irq(n)	(REG_TCU_TMCR = (1 << ((n) + 16)))
#define __tcu_unmask_full_match_irq(n)	(REG_TCU_TMCR = (1 << (n)))

#define __tcu_ost_match_flag()		(REG_TCU_TFR & TCU_TFR_OSTFLAG)
#define __tcu_set_ost_match_flag()	(REG_TCU_TFSR = TCU_TFSR_OSTFST)
#define __tcu_clear_ost_match_flag()	(REG_TCU_TFCR = TCU_TFCR_OSTFCL)
#define __tcu_ost_match_irq_masked()	(REG_TCU_TMR & TCU_TMR_OSTMASK)
#define __tcu_mask_ost_match_irq()	(REG_TCU_TMSR = TCU_TMSR_OSTMST)
#define __tcu_unmask_ost_match_irq()	(REG_TCU_TMCR = TCU_TMCR_OSTMCL)

#define __tcu_wdt_clock_stopped()	(REG_TCU_TSR & TCU_TSSR_WDTSC)
#define __tcu_ost_clock_stopped()	(REG_TCU_TSR & TCU_TSR_OST)
#define __tcu_timer_clock_stopped(n)	(REG_TCU_TSR & (1 << (n)))

#define __tcu_start_wdt_clock()		(REG_TCU_TSCR = TCU_TSSR_WDTSC)
#define __tcu_start_ost_clock()		(REG_TCU_TSCR = TCU_TSCR_OSTSC)
#define __tcu_start_timer_clock(n)	(REG_TCU_TSCR = (1 << (n)))

#define __tcu_stop_wdt_clock()		(REG_TCU_TSSR = TCU_TSSR_WDTSC)
#define __tcu_stop_ost_clock()		(REG_TCU_TSSR = TCU_TSSR_OSTSS)
#define __tcu_stop_timer_clock(n)	(REG_TCU_TSSR = (1 << (n)))

#define __tcu_get_count(n)		(REG_TCU_TCNT((n)))
#define __tcu_set_count(n,v)		(REG_TCU_TCNT((n)) = (v))
#define __tcu_set_full_data(n,v)	(REG_TCU_TDFR((n)) = (v))
#define __tcu_set_half_data(n,v)	(REG_TCU_TDHR((n)) = (v))

/* TCU2, counter 1, 2*/
#define __tcu_read_real_value(n)	(REG_TCU_TSTR & (1 << ((n) + 16)))
#define __tcu_read_false_value(n)	(REG_TCU_TSTR & (1 << ((n) + 16)))
#define __tcu_counter_busy(n)		(REG_TCU_TSTR & (1 << (n)))
#define __tcu_counter_ready(n)		(REG_TCU_TSTR & (1 << (n)))

#define __tcu_set_read_real_value(n)	(REG_TCU_TSTSR = (1 << ((n) + 16)))
#define __tcu_set_read_false_value(n)	(REG_TCU_TSTCR = (1 << ((n) + 16)))
#define __tcu_set_counter_busy(n)	(REG_TCU_TSTSR = (1 << (n)))
#define __tcu_set_counter_ready(n)	(REG_TCU_TSTCR = (1 << (n)))

/* ost counter */
#define __ostcu_set_pwm_output_shutdown_graceful()	(REG_TCU_OSTCSR &= ~TCU_TCSR_PWM_SD)
#define __ostcu_set_ost_output_shutdown_abrupt()	(REG_TCU_OSTCSR |= TCU_TCSR_PWM_SD)
#define __ostcu_select_clk_div1() \
	(REG_TCU_OSTCSR = (REG_TCU_OSTCSR & ~TCU_OSTCSR_PRESCALE_MASK) | TCU_OSTCSR_PRESCALE1)
#define __ostcu_select_clk_div4() \
	(REG_TCU_OSTCSR = (REG_TCU_OSTCSR & ~TCU_OSTCSR_PRESCALE_MASK) | TCU_OSTCSR_PRESCALE4)
#define __ostcu_select_clk_div16() \
	(REG_TCU_OSTCSR = (REG_TCU_OSTCSR & ~TCU_OSTCSR_PRESCALE_MASK) | TCU_OSTCSR_PRESCALE16)
#define __ostcu_select_clk_div64() \
	(REG_TCU_OSTCSR = (REG_TCU_OSTCSR & ~TCU_OSTCSR_PRESCALE_MASK) | TCU_OSTCSR_PRESCALE64)
#define __ostcu_select_clk_div256() \
	(REG_TCU_OSTCSR = (REG_TCU_OSTCSR & ~TCU_OSTCSR_PRESCALE_MASK) | TCU_OSTCSR_PRESCALE256)
#define __ostcu_select_clk_div1024() \
	(REG_TCU_OSTCSR = (REG_TCU_OSTCSR & ~TCU_OSTCSR_PRESCALE_MASK) | TCU_OSTCSR_PRESCALE1024)
#define __ostcu_select_rtcclk() \
	(REG_TCU_OSTCSR = (REG_TCU_OSTCSR & ~(TCU_OSTCSR_EXT_EN | TCU_OSTCSR_RTC_EN | TCU_OSTCSR_PCK_EN)) | TCU_OSTCSR_RTC_EN)
#define __ostcu_select_extalclk() \
	(REG_TCU_OSTCSR = (REG_TCU_OSTCSR & ~(TCU_OSTCSR_EXT_EN | TCU_OSTCSR_RTC_EN | TCU_OSTCSR_PCK_EN)) | TCU_OSTCSR_EXT_EN)
#define __ostcu_select_pclk() \
	(REG_TCU_OSTCSR = (REG_TCU_OSTCSR & ~(TCU_OSTCSR_EXT_EN | TCU_OSTCSR_RTC_EN | TCU_OSTCSR_PCK_EN)) | TCU_OSTCSR_PCK_EN)

#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4770TCU_H__ */

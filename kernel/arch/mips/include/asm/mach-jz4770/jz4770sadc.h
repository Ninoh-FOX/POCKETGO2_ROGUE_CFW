/*
 * linux/include/asm-mips/mach-jz4770/jz4770sadc.h
 *
 * JZ4770 SADC register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770SADC_H__
#define __JZ4770SADC_H__

#include <asm/addrspace.h>
#include <asm/mach-jz4770/base.h>
#include <asm/mach-jz4770/jz4770misc.h>


#define	SADC_BASE	CKSEG1ADDR(JZ4770_SADC_BASE_ADDR)

/*************************************************************************
 * SADC (Smart A/D Controller)
 *************************************************************************/

#define SADC_ENA	(SADC_BASE + 0x00)  /* ADC Enable Register */
#define SADC_CFG	(SADC_BASE + 0x04)  /* ADC Configure Register */
#define SADC_CTRL	(SADC_BASE + 0x08)  /* ADC Control Register */
#define SADC_STATE	(SADC_BASE + 0x0C)  /* ADC Status Register*/
#define SADC_SAMETIME	(SADC_BASE + 0x10)  /* ADC Same Point Time Register */
#define SADC_WAITTIME	(SADC_BASE + 0x14)  /* ADC Wait Time Register */
#define SADC_TSDAT	(SADC_BASE + 0x18)  /* ADC Touch Screen Data Register */
#define SADC_BATDAT	(SADC_BASE + 0x1C)  /* ADC VBAT Data Register */
#define SADC_SADDAT	(SADC_BASE + 0x20)  /* ADC AUX Data Register */
#define SADC_ADCLK	(SADC_BASE + 0x28)  /* ADC Clock Divide Register */
#define SADC_FLT	(SADC_BASE + 0x24)  /* ADC Filter Register */

#define REG_SADC_ENA		REG8(SADC_ENA)
#define REG_SADC_CFG		REG32(SADC_CFG)
#define REG_SADC_CTRL		REG8(SADC_CTRL)
#define REG_SADC_STATE		REG8(SADC_STATE)
#define REG_SADC_SAMETIME	REG16(SADC_SAMETIME)
#define REG_SADC_WAITTIME	REG16(SADC_WAITTIME)
#define REG_SADC_TSDAT		REG32(SADC_TSDAT)
#define REG_SADC_BATDAT		REG16(SADC_BATDAT)
#define REG_SADC_SADDAT		REG16(SADC_SADDAT)
#define REG_SADC_ADCLK		REG32(SADC_ADCLK)
#define REG_SADC_FLT		REG16(SADC_FLT)
   #define SADC_FLT_ENA		(1 << 15)

/* ADENA: ADC Enable Register */
#define SADC_ENA_POWER		(1 << 7)  /* SADC Power control bit */
#define SADC_ENA_SLP_MD		(1 << 6)  /* SLEEP mode control */
#define SADC_ENA_TSEN		(1 << 2)  /* Touch Screen Enable */
#define SADC_ENA_PBATEN		(1 << 1)  /* PBAT Enable */
#define SADC_ENA_SADCINEN	(1 << 0)  /* AUX n Enable */

/* ADC Configure Register */
#define SADC_CFG_SPZZ           (1 << 30)
#define SADC_CFG_TS_DMA		(1 << 15)  /* Touch Screen DMA Enable */
#define SADC_CFG_XYZ_BIT	13  /* XYZ selection */
#define SADC_CFG_XYZ_MASK	(0x3 << SADC_CFG_XYZ_BIT)
  #define SADC_CFG_XY		(0 << SADC_CFG_XYZ_BIT)
  #define SADC_CFG_XYZ		(1 << SADC_CFG_XYZ_BIT)
  #define SADC_CFG_XYZ1Z2	(2 << SADC_CFG_XYZ_BIT)
#define SADC_CFG_SNUM_BIT	10  /* Sample Number */
#define SADC_CFG_SNUM(x)	(((x) - 1) << SADC_CFG_SNUM_BIT)
#define SADC_CFG_SNUM_MASK	(0x7 << SADC_CFG_SNUM_BIT)
  #define SADC_CFG_SNUM_1	(0x0 << SADC_CFG_SNUM_BIT)
  #define SADC_CFG_SNUM_2	(0x1 << SADC_CFG_SNUM_BIT)
  #define SADC_CFG_SNUM_3	(0x2 << SADC_CFG_SNUM_BIT)
  #define SADC_CFG_SNUM_4	(0x3 << SADC_CFG_SNUM_BIT)
  #define SADC_CFG_SNUM_5	(0x4 << SADC_CFG_SNUM_BIT)
  #define SADC_CFG_SNUM_6	(0x5 << SADC_CFG_SNUM_BIT)
  #define SADC_CFG_SNUM_8	(0x6 << SADC_CFG_SNUM_BIT)
  #define SADC_CFG_SNUM_9	(0x7 << SADC_CFG_SNUM_BIT)
#define SADC_CFG_CMD_BIT	0  /* ADC Command */
#define SADC_CFG_CMD_MASK	(0x3 << SADC_CFG_CMD_BIT)
  #define SADC_CFG_CMD_AUX0	(0x0 << SADC_CFG_CMD_BIT) /* AUX voltage */
  #define SADC_CFG_CMD_AUX1	(0x1 << SADC_CFG_CMD_BIT) /* AUX1 voltage */
  #define SADC_CFG_CMD_AUX2	(0x2 << SADC_CFG_CMD_BIT) /* AUX2 voltage */
  #define SADC_CFG_CMD_RESERVED	(0x3 << SADC_CFG_CMD_BIT) /* Reserved */

/* ADCCTRL: ADC Control Register */
#define SADC_CTRL_SLPENDM	(1 << 5)  /* Sleep Interrupt Mask */
#define SADC_CTRL_PENDM		(1 << 4)  /* Pen Down Interrupt Mask */
#define SADC_CTRL_PENUM		(1 << 3)  /* Pen Up Interrupt Mask */
#define SADC_CTRL_TSRDYM	(1 << 2)  /* Touch Screen Data Ready Interrupt Mask */
#define SADC_CTRL_PBATRDYM	(1 << 1)  /* VBAT Data Ready Interrupt Mask */
#define SADC_CTRL_SRDYM		(1 << 0)  /* AUX Data Ready Interrupt Mask */

/* ADSTATE: ADC Status Register */
#define SADC_STATE_SLP_RDY	(1 << 7)  /* Sleep state bit */
#define SADC_STATE_SLEEPND	(1 << 5)  /* Pen Down Interrupt Flag */
#define SADC_STATE_PEND		(1 << 4)  /* Pen Down Interrupt Flag */
#define SADC_STATE_PENU		(1 << 3)  /* Pen Up Interrupt Flag */
#define SADC_STATE_TSRDY	(1 << 2)  /* Touch Screen Data Ready Interrupt Flag */
#define SADC_STATE_PBATRDY		(1 << 1)  /* VBAT Data Ready Interrupt Flag */
#define SADC_STATE_SRDY		(1 << 0)  /* AUX Data Ready Interrupt Flag */

/* ADTCH: ADC Touch Screen Data Register */
#define SADC_TSDAT_TYPE1	(1 << 31)
#define SADC_TSDAT_DATA1_BIT	16
#define SADC_TSDAT_DATA1_MASK	(0xfff << SADC_TSDAT_DATA1_BIT)
#define SADC_TSDAT_TYPE0	(1 << 15)
#define SADC_TSDAT_DATA0_BIT	0
#define SADC_TSDAT_DATA0_MASK	(0xfff << SADC_TSDAT_DATA0_BIT)

/* ADCLK: ADC Clock Divide Register */
#define SADC_ADCLK_CLKDIV_MS	16
#define SADC_ADCLK_CLKDIV_MS_MASK	(0xffff << SADC_ADCLK_CLKDIV_MS)
#define SADC_ADCLK_CLKDIV_US	8
#define SADC_ADCLK_CLKDIV_US_MASK	(0xff << SADC_ADCLK_CLKDIV_US)
#define SADC_ADCLK_CLKDIV_BIT		0
#define SADC_ADCLK_CLKDIV_MASK		(0xff << SADC_ADCLK_CLKDIV_BIT)

#endif /* __JZ4770SADC_H__ */

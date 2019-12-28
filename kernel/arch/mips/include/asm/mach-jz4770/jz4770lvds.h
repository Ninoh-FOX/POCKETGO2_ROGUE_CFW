/*
 * linux/include/asm-mips/mach-jz4770/jz4770lvds.h
 *
 * JZ4770 LVDS register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770LVDS_H__
#define __JZ4770LVDS_H__

#include <asm/mach-jz4770/jz4770misc.h>


#define LVDS_CTRL	(LCD_BASE + 0x3C0)  /* SLCD Configure Register */
#define LVDS_PLL0	(LCD_BASE + 0x3C4)  /* SLCD Control Register */
#define LVDS_PLL1	(LCD_BASE + 0x3C8)  /* SLCD Status Register */

#define REG_LVDS_CTRL	REG32(LVDS_CTRL)
#define REG_LVDS_PLL0	REG32(LVDS_PLL0)
#define REG_LVDS_PLL1	REG32(LVDS_PLL1)

/* LVDS Control Register */
#define LVDS_CTRL_MODEL_SEL	BIT31
#define LVDS_CTRL_MODEL_JEIDA	(0 << LVDS_CTRL_MODEL_SEL)
#define LVDS_CTRL_MODEL_VESA	(1 << LVDS_CTRL_MODEL_SEL)

#define LVDS_CTRL_TX_PDB	(0 << 30)
#define LVDS_CTRL_TX_PDB_CK	(0 << 29)
#define LVDS_CTRL_TX_RSTB	(0 << 18)
#define LVDS_CTRL_TX_CKBIT_PHA_FALLING	(1 << 17)
#define LVDS_CTRL_TX_CKBYTE_PHA_FALLING	(1 << 16)

#define LVDS_CTRL_TX_CKOUT_PHA_MASK     (0x7 << 13)
#define LVDS_CTRL_TX_CKOUT_PHA_1     (0x1 << 13)
#define LVDS_CTRL_TX_CKOUT_PHA_2     (0x2 << 13)
#define LVDS_CTRL_TX_CKOUT_PHA_3     (0x3 << 13)
#define LVDS_CTRL_TX_CKOUT_PHA_4     (0x4 << 13)
#define LVDS_CTRL_TX_CKOUT_PHA_5     (0x5 << 13)
#define LVDS_CTRL_TX_CKOUT_PHA_6     (0x6 << 13)
#define LVDS_CTRL_TX_CKOUT_PHA_7     (0x7 << 13)


#define LVDS_CTRL_TX_CKOUT_7x     (0x1 << 12)
#define LVDS_CTRL_TX_OUT_SEL     BIT11
#define LVDS_CTRL_TX_OUT_CMOS_RGB     (0x1 << LVDS_CTRL_TX_OUT_SEL)
#define LVDS_CTRL_TX_OUT_LVDS         (0x0 << LVDS_CTRL_TX_OUT_SEL)
#define LVDS_CTRL_TX_DLY_SEL_MASK      (0x7 << 8)
#define LVDS_CTRL_TX_AMP_ADJ     BIT7
#define LVDS_CTRL_TX_LVDS     BIT6
#define LVDS_CTRL_TX_CR_MASK     (0x7 << 3)
#define LVDS_CTRL_TX_CR_CK    BIT2
#define LVDS_CTRL_TX_OD_S    BIT1
#define LVDS_CTRL_TX_OD_EN    BIT0

/* LVDS PLL0 Register */
#define LVDS_PLL0_PLL_LOCK		BIT31
#define LVDS_PLL0_PLL_DIS		BIT30
#define LVDS_PLL0_BG_PWD		BIT29
#define LVDS_PLL0_PLL_SSC_EN		BIT27
#define LVDS_PLL0_PLL_SSC_MODE		BIT26
#define LVDS_PLL0_PLL_TEST		BIT25
#define LVDS_PLL0_PLL_POST_DIVA		(0x3 << 21)
#define LVDS_PLL0_PLL_POST_DIVB		(0x3f << 16)
#define LVDS_PLL0_PLL_PLLN		(0x7f << 8)
#define LVDS_PLL0_PLL_TEST_DIV		(0x2 << 6)
#define LVDS_PLL0_PLL_IN_BYPASS		BIT5
#define LVDS_PLL0_PLL_INDIV		(0x1f << 0)


/* LVDS PLL1 Register */
#define LVDS_PLL1_PLL_ICP_SEL		(0x7 << 29)
#define LVDS_PLL1_PLL_KVCO		(0x3 << 26)
#define LVDS_PLL1_PLL_IVCO_SEL		(0x3 << 24)
#define LVDS_PLL1_PLL_SSCN		(0x7f << 17)
#define LVDS_PLL1_PLL_COUNT		(0x1fff << 4)
#define LVDS_PLL1_PLL_GAIN		(0xf << 0)

#endif /* __JZ4770LVDS_H__ */

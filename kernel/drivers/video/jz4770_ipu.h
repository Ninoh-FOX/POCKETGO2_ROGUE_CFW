/*
 *  drivers/video/fbdev/jz4770_ipu.h
 *
 *  Copyright (C) 2014, Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __JZ4770_IPU_H__
#define __JZ4770_IPU_H__

#include <linux/bitops.h>

/* Register offset */
#define IPU_CTRL		0x0  /* IPU Control Register */
#define IPU_STATUS		0x4  /* IPU Status Register */
#define IPU_D_FMT		0x8  /* Data Format Register */
#define IPU_Y_ADDR		0xc  /* Input Y or YUV422 Packaged Data Address Register */
#define IPU_U_ADDR		0x10 /* Input U Data Address Register */
#define IPU_V_ADDR		0x14 /* Input V Data Address Register */
#define IPU_IN_GS		0x18 /* Input Geometric Size Register */
#define IPU_Y_STRIDE		0x1c /* Input Y Data Line Stride Register */
#define IPU_UV_STRIDE		0x20 /* Input UV Data Line Stride Register */
#define IPU_OUT_ADDR		0x24 /* Output Frame Start Address Register */
#define IPU_OUT_GS		0x28 /* Output Geometric Size Register */
#define IPU_OUT_STRIDE		0x2c /* Output Data Line Stride Register */
#define IPU_RSZ_COEF_INDEX	0x30 /* Resize Coefficients Table Index Register */
#define IPU_CSC_CO_COEF		0x34 /* CSC C0 Coefficient Register */
#define IPU_CSC_C1_COEF		0x38 /* CSC C1 Coefficient Register */
#define IPU_CSC_C2_COEF		0x3c /* CSC C2 Coefficient Register */
#define IPU_CSC_C3_COEF		0x40 /* CSC C3 Coefficient Register */
#define IPU_CSC_C4_COEF		0x44 /* CSC C4 Coefficient Register */
#define IPU_HRSZ_COEF_LUT	0x48 /* Horizontal Resize Coefficients Look Up Table Register group */
#define IPU_VRSZ_COEF_LUT	0x4c /* Virtical Resize Coefficients Look Up Table Register group */
#define IPU_CSC_OFSET_PARA	0x50 /* CSC Offset Parameter Register */
#define IPU_Y_PHY_T_ADDR	0x54 /* Input Y Physical Table Address Register */
#define IPU_U_PHY_T_ADDR	0x58 /* Input U Physical Table Address Register */
#define IPU_V_PHY_T_ADDR	0x5c /* Input V Physical Table Address Register */
#define IPU_OUT_PHY_T_ADDR	0x60 /* Output Physical Table Address Register */

#define IPU_CTRL_ADDR_SEL	BIT(20)		/* address mode selector */
#define IPU_CTRL_ZOOM_SEL	BIT(18)		/* scale 0:bilinear 1:bicubic */
#define IPU_CTRL_DFIX_SEL	BIT(17)		/* fixed dest addr */
#define IPU_CTRL_LCDC_SEL	BIT(11)		/* output to LCDC FIFO */
#define IPU_CTRL_SPKG_SEL	BIT(10)		/* packed input format */
#define IPU_CTRL_STOP		BIT(7)		/* stop conversion */
#define IPU_CTRL_RST		BIT(6)		/* reset IPU */
#define IPU_CTRL_FM_IRQ_EN	BIT(5)		/* Frame process finish IRQ */
#define IPU_CTRL_VRSZ_EN	BIT(3)		/* vertical resize */
#define IPU_CTRL_HRSZ_EN	BIT(2)		/* horizontal resize */
#define IPU_CTRL_RUN		BIT(1)		/* start conversion */
#define IPU_CTRL_CHIP_EN	BIT(0)		/* chip enable */

#define IPU_STATUS_OUT_END	BIT(0)		/* Frame process finish IRQ */

#define IPU_IN_GS_H_BIT		0x0
#define IPU_IN_GS_W_BIT		0x10
#define IPU_OUT_GS_H_BIT	0x0
#define IPU_OUT_GS_W_BIT	0x10

#define IPU_D_FMT_IN_FMT_BIT		0x0
#define IPU_D_FMT_OUT_FMT_BIT		0x13
#define IPU_D_FMT_RGB_OUT_OFT_BIT	0x16

#endif /* __JZ4770_IPU_H__ */
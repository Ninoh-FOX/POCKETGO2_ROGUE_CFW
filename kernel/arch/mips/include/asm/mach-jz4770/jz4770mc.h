/*
 * linux/include/asm-mips/mach-jz4770/jz4770mc.h
 *
 * JZ4770 MC register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770MC_H__
#define __JZ4770MC_H__

#include <asm/mach-jz4770/jz4770misc.h>


#define	MC_BASE		0xB3250000

/*************************************************************************
 * MC (Motion Compensation)
 *************************************************************************/
#define MC_CTRL		(MC_BASE + 0x00) /* MC Control Register */
#define MC_STAT		(MC_BASE + 0x04) /* MC Status Register */
#define MC_REF_ADDR	(MC_BASE + 0x08) /* MC Reference Block Address Register */
#define MC_REF2_ADDR	(MC_BASE + 0x0C) /* MC 2nd Reference Block Address Register */
#define MC_CURR_ADDR	(MC_BASE + 0x10) /* MC Current Block Address Register */
#define MC_REF_STRD	(MC_BASE + 0x14) /* MC Reference Frame Stride Register */
#define MC_CURR_STRD	(MC_BASE + 0x18) /* MC Current Frame Stride Register */
#define MC_ITP_INFO	(MC_BASE + 0x1C) /* MC Block Interpolation Information Register */
#define MC_TAP_COEF1	(MC_BASE + 0x20) /* MC TAP Filter Coefficient 1 Register */
#define MC_TAP_COEF2	(MC_BASE + 0x24) /* MC TAP Filter Coefficient 2 Register */

#define REG_MC_CTRL		REG32(MC_CTRL)
#define REG_MC_STAT		REG32(MC_STAT)
#define REG_MC_REF_ADDR		REG32(MC_REF_ADDR)
#define REG_MC_REF2_ADDR	REG32(MC_REF2_ADDR)
#define REG_MC_CURR_ADDR	REG32(MC_CURR_ADDR)
#define REG_MC_REF_STRD		REG32(MC_REF_STRD)
#define REG_MC_CURR_STRD	REG32(MC_CURR_STRD)
#define REG_MC_ITP_INFO		REG32(MC_ITP_INFO)
#define REG_MC_TAP_COEF1	REG32(MC_TAP_COEF1)
#define REG_MC_TAP_COEF2	REG32(MC_TAP_COEF2)

/* MC Control Register */
#define MC_CTRL_CACHECLR	(1 << 2) /* MC Cache clear */
#define MC_CTRL_RESET		(1 << 1) /* MC Reset */
#define MC_CTRL_ENABLE		(1 << 0) /* MC enable */

/* MC Status Register */
#define MC_STAT_OUT_END		(1 << 0) /* Output DMA termination flag */

/* MC Reference Frame Stride Register, unit: byte */
#define MC_REF_STRD_BIT		16
#define MC_REF_STRD_MASK	(0xfff << MC_REF_STRD_BIT)
#define MC_REF_STRD2_BIT	0
#define MC_REF_STRD2_MASK	(0xfff << MC_REF_STRD2_BIT)

/* MC Current Frame Stride Register, unit: byte */
#define MC_CURR_STRD_BIT	0
#define MC_CURR_STRD_MASK	(0xfff << MC_CURR_STRD_BIT)

/* MC Block Interpolation Information Register */
#define MC_ITP_INFO_RND1_BIT	24  /* Rounding data during interpolation */
#define MC_ITP_INFO_RND1_MASK	(0xff << MC_ITP_INFO_RND1_BIT)
#define MC_ITP_INFO_RND0_BIT	16  /* Rounding data during interpolation */
#define MC_ITP_INFO_RND0_MASK	(0xff << MC_ITP_INFO_RND0_BIT)
#define MC_ITP_INFO_AVG		(1 << 12)  /* 0: output interpolated data directly; 1: doing average operation with 2nd source data after interpolating and output */
#define MC_ITP_INFO_FMT_BIT	8  /* Indicate current interpolation's type */
#define MC_ITP_INFO_RMT_MASK	(0xf << MC_ITP_INFO_RMT_BIT)
  #define MC_ITP_INFO_FMT_MPEG_HPEL  (0x0 << MC_ITP_INFO_RMT_BIT) /* MPEG Half-pixel interpolation */
  #define MC_ITP_INFO_FMT_MPEG_QPEL  (0x1 << MC_ITP_INFO_RMT_BIT) /* MPEG 8-tap Quarter-pixel interpolation */
  #define MC_ITP_INFO_FMT_H264_QPEL  (0x2 << MC_ITP_INFO_RMT_BIT) /* H264 6-tap Quarter-pixel interpolation */
  #define MC_ITP_INFO_FMT_H264_EPEL  (0x3 << MC_ITP_INFO_RMT_BIT) /* H264 2-tap Eight-pixel interpolation */
  #define MC_ITP_INFO_FMT_H264_WPDT  (0x4 << MC_ITP_INFO_RMT_BIT) /* H264 Weighted-prediction */
  #define MC_ITP_INFO_FMT_WMV2_QPEL  (0x5 << MC_ITP_INFO_RMT_BIT) /* WMV2 4-tap Quarter-pixel interpolation */
  #define MC_ITP_INFO_FMT_VC1_QPEL   (0x6 << MC_ITP_INFO_RMT_BIT) /* VC1 4-tap Quarter-pixel interpolation */
  #define MC_ITP_INFO_FMT_RV8_TPEL   (0x7 << MC_ITP_INFO_RMT_BIT) /* RV8 4-tap Third-pixel interpolation */
  #define MC_ITP_INFO_FMT_RV8_CHROM  (0x8 << MC_ITP_INFO_RMT_BIT) /* RV8 2-tap Third-pixel interpolation */
  #define MC_ITP_INFO_FMT_RV9_QPEL   (0x9 << MC_ITP_INFO_RMT_BIT) /* RV9 6-tap Quarter-pixel interpolation */
  #define MC_ITP_INFO_FMT_RV9_CHROM  (0xa << MC_ITP_INFO_RMT_BIT) /* RV9 2-tap Quarter-pixel interpolation */
#define MC_ITP_INFO_BLK_W_BIT	6  /* Indicate reference block's width, unit: pixel */
#define MC_ITP_INFO_BLK_W_MASK	(0x3 << MC_ITP_INFO_BLK_W_BIT)
  #define MC_ITP_INFO_BLK_W_2	(0x0 << MC_ITP_INFO_BLK_W_BIT)
  #define MC_ITP_INFO_BLK_W_4	(0x1 << MC_ITP_INFO_BLK_W_BIT)
  #define MC_ITP_INFO_BLK_W_8	(0x2 << MC_ITP_INFO_BLK_W_BIT)
  #define MC_ITP_INFO_BLK_W_16	(0x3 << MC_ITP_INFO_BLK_W_BIT)
#define MC_ITP_INFO_BLK_H_BIT	4  /* Indicate reference block's height, unit: pixel */
#define MC_ITP_INFO_BLK_H_MASK	(0x3 << MC_ITP_INFO_BLK_H_BIT)
  #define MC_ITP_INFO_BLK_H_2	(0x0 << MC_ITP_INFO_BLK_H_BIT)
  #define MC_ITP_INFO_BLK_H_4	(0x1 << MC_ITP_INFO_BLK_H_BIT)
  #define MC_ITP_INFO_BLK_H_8	(0x2 << MC_ITP_INFO_BLK_H_BIT)
  #define MC_ITP_INFO_BLK_H_16	(0x3 << MC_ITP_INFO_BLK_H_BIT)
#define MC_ITP_INFO_ITP_CASE_BIT	0  /* Indicate interpolation final destination pixel position */
#define MC_ITP_INFO_ITP_CASE_MASK	(0xf << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H0V0	(0x0 << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H1V0	(0x1 << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H2V0	(0x2 << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H3V0	(0x3 << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H0V1	(0x4 << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H1V1	(0x5 << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H2V1	(0x6 << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H3V1	(0x7 << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H0V2	(0x8 << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H1V2	(0x9 << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H2V2	(0xa << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H3V2	(0xb << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H0V3	(0xc << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H1V3	(0xd << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H2V3	(0xe << MC_ITP_INFO_ITP_CASE_BIT)
  #define MC_ITP_INFO_ITP_CASE_H3V3	(0xf << MC_ITP_INFO_ITP_CASE_BIT)

/* MC TAP Filter Coefficient 1 Register */
#define MC_TAP_COEF1_TAP_COEF4_BIT	24
#define MC_TAP_COEF1_TAP_COEF4_MASK	(0xff << MC_TAP_COEF1_TAP_COEF4_BIT)
#define MC_TAP_COEF1_TAP_COEF3_BIT	16
#define MC_TAP_COEF1_TAP_COEF3_MASK	(0xff << MC_TAP_COEF1_TAP_COEF3_BIT)
#define MC_TAP_COEF1_TAP_COEF2_BIT	8
#define MC_TAP_COEF1_TAP_COEF2_MASK	(0xff << MC_TAP_COEF1_TAP_COEF2_BIT)
#define MC_TAP_COEF1_TAP_COEF1_BIT	0
#define MC_TAP_COEF1_TAP_COEF1_MASK	(0xff << MC_TAP_COEF1_TAP_COEF1_BIT)

/* MC TAP Filter Coefficient 2 Register */
#define MC_TAP_COEF2_TAP_COEF8_BIT	24
#define MC_TAP_COEF2_TAP_COEF8_MASK	(0xff << MC_TAP_COEF2_TAP_COEF8_BIT)
#define MC_TAP_COEF2_TAP_COEF7_BIT	16
#define MC_TAP_COEF2_TAP_COEF7_MASK	(0xff << MC_TAP_COEF2_TAP_COEF7_BIT)
#define MC_TAP_COEF2_TAP_COEF6_BIT	8
#define MC_TAP_COEF2_TAP_COEF6_MASK	(0xff << MC_TAP_COEF2_TAP_COEF6_BIT)
#define MC_TAP_COEF2_TAP_COEF5_BIT	0
#define MC_TAP_COEF2_TAP_COEF5_MASK	(0xff << MC_TAP_COEF2_TAP_COEF5_BIT)


#endif /* __JZ4770MC_H__ */

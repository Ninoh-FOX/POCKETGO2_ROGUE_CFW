/*
 * linux/include/asm-mips/mach-jz4770/jz4770aosd.h
 *
 * JZ4770 ALPHA OSD register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770AOSD_H__
#define __JZ4770AOSD_H__

#include <asm/mach-jz4770/jz4770misc.h>


#define AOSD_BASE		0xB3070000

/*************************************************************************
 * OSD (On Screen Display)
 *************************************************************************/
#define AOSD_ADDR0		(AOSD_BASE + 0x00)
#define AOSD_ADDR1		(AOSD_BASE + 0x04)
#define AOSD_ADDR2		(AOSD_BASE + 0x08)
#define AOSD_ADDR3		(AOSD_BASE + 0x0C)
#define AOSD_WADDR		(AOSD_BASE + 0x10)
#define AOSD_ADDRLEN		(AOSD_BASE + 0x14)
#define AOSD_ALPHA_VALUE	(AOSD_BASE + 0x18)
#define AOSD_CTRL		(AOSD_BASE + 0x1C)
#define AOSD_INT		(AOSD_BASE + 0x20)
#define AOSD_CLK_GATE		(AOSD_BASE + 0x48)

#define REG_AOSD_ADDR0		REG32(AOSD_ADDR0)
#define REG_AOSD_ADDR1		REG32(AOSD_ADDR1)
#define REG_AOSD_ADDR2		REG32(AOSD_ADDR2)
#define REG_AOSD_ADDR3		REG32(AOSD_ADDR3)
#define REG_AOSD_WADDR		REG32(AOSD_WADDR)
#define REG_AOSD_ADDRLEN	REG32(AOSD_ADDRLEN)
#define REG_AOSD_ALPHA_VALUE	REG32(AOSD_ALPHA_VALUE)
#define REG_AOSD_CTRL		REG32(AOSD_CTRL)
#define REG_AOSD_INT		REG32(AOSD_INT)
#define REG_AOSD_CLK_GATE	REG32(AOSD_CLK_GATE)

#define AOSD_CTRL_FRMLV_MASK		(0x3 << 18)
#define AOSD_CTRL_FRMLV_2		(0x1 << 18)
#define AOSD_CTRL_FRMLV_3		(0x2 << 18)
#define AOSD_CTRL_FRMLV_4		(0x3 << 18)

#define AOSD_CTRL_FRM_END		(1 << 17)
#define AOSD_CTRL_ALPHA_START		(1 << 16)
#define AOSD_CTRL_INT_MAKS		(1 << 15)
#define AOSD_CTRL_CHANNEL_LEVEL_BIT	7
#define AOSD_CTRL_CHANNEL_LEVEL_MASK	(0xff <<  AOSD_CTRL_CHANNEL_LEVEL_BIT)
#define AOSD_CTRL_ALPHA_MODE_BIT	3
#define AOSD_CTRL_ALPHA_MODE_MASK	(0xf << AOSD_CTRL_ALPHA_MODE_BIT)
#define AOSD_CTRL_ALPHA_PIXEL_MODE	0
#define AOSD_CTRL_ALPHA_FRAME_MODE	1

#define AOSD_CTRL_FORMAT_MODE_BIT	1
#define AOSD_CTRL_FORMAT_MODE_MASK	(0x3 << 1)
#define AOSD_CTRL_RGB565_FORMAT_MODE	(0 << AOSD_CTRL_FORMAT_MODE_BIT)
#define AOSD_CTRL_RGB555_FORMAT_MODE	(1 << AOSD_CTRL_FORMAT_MODE_BIT)
#define AOSD_CTRL_RGB8888_FORMAT_MODE	(2 << AOSD_CTRL_FORMAT_MODE_BIT)

#define AOSD_ALPHA_ENABLE		(1 << 0)

#define AOSD_INT_COMPRESS_END		(1 << 1)
#define AOSD_INT_AOSD_END		(1 << 0)

#define AOSD_CLK_GATE_EN		(1 << 0)

#define __osd_enable_alpha() 	(REG_AOSD_CTRL |= AOSD_ALPHA_ENABLE)
#define __osd_alpha_start()	(REG_AOSD_CTRL |= AOSD_CTRL_ALPHA_START)

/*************************************************************************
 * COMPRESS
 *************************************************************************/

#define COMPRESS_SCR_ADDR	(AOSD_BASE + 0x00)
#define COMPRESS_DES_ADDR	(AOSD_BASE + 0x10)
#define COMPRESS_OFFSIZE	(AOSD_BASE + 0x34)
#define COMPRESS_FRAME_SIZE	(AOSD_BASE + 0x38)
#define COMPRESS_CTRL		(AOSD_BASE + 0x3C)
#define COMPRESS_RATIO		(AOSD_BASE + 0x40)
#define COMPRESS_OFFSET		(AOSD_BASE + 0x44)
#define COMPRESS_RESULT		(AOSD_BASE + 0x4C)

#define REG_COMPRESS_SCR_ADDR	REG32(COMPRESS_SCR_ADDR)
#define REG_COMPRESS_DES_ADDR	REG32(COMPRESS_DES_ADDR)
#define REG_COMPRESS_OFFSIZE	REG32(COMPRESS_OFFSIZE)
#define REG_COMPRESS_FRAME_SIZE	REG32(COMPRESS_FRAME_SIZE)
#define REG_COMPRESS_CTRL	REG32(COMPRESS_CTRL)
#define REG_COMPRESS_RATIO	REG32(COMPRESS_RATIO)
#define REG_COMPRESS_OFFSET	REG32(COMPRESS_OFFSET)
#define REG_COMPRESS_RESULT	REG32(COMPRESS_RESULT)

#define COMPRESS_CTRL_WITHOUT_ALPHA	(1 << 4)
#define COMPRESS_CTRL_WITH_ALPHA	(0 << 4)
#define COMPRESS_CTRL_COMP_START	(1 << 3)
#define COMPRESS_CTRL_COMP_END		(1 << 2)
#define COMPRESS_CTRL_INT_MASK		(1 << 1)
#define COMPRESS_CTRL_COMP_ENABLE	(1 << 0)

#define COMPRESS_RATIO_FRM_BYPASS	(1 << 31)
#define COMPRESS_BYPASS_ROW		(1 << 12)
#define COMPRESS_ROW_QUARTER		(1 << 0)

#define __compress_enable()	(REG_COMPRESS_CTRL |= COMPRESS_INT_AOSD_END)
#define __compress_start()	(REG_COMPRESS_CTRL |= COMPRESS_CTRL_COMP_START)
#define __compress_with_alpha()	(REG_COMPRESS_CTRL |= COMPRESS_CTRL_ALPHA_EN)

#endif /* __JZ4770AOSD_H__ */

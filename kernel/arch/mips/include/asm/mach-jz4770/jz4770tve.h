/*
 * linux/include/asm-mips/mach-jz4770/jz4770tve.h
 *
 * JZ4770 TVE register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770TVE_H__
#define __JZ4770TVE_H__

#include <asm/mach-jz4770/jz4770misc.h>


#define	TVE_BASE	0xB3050100


/*************************************************************************
 * TVE (TV Encoder Controller)
 *************************************************************************/
#define TVE_CTRL	(TVE_BASE + 0x40) /* TV Encoder Control register */
#define TVE_FRCFG	(TVE_BASE + 0x44) /* Frame configure register */
#define TVE_SLCFG1	(TVE_BASE + 0x50) /* TV signal level configure register 1 */
#define TVE_SLCFG2	(TVE_BASE + 0x54) /* TV signal level configure register 2*/
#define TVE_SLCFG3	(TVE_BASE + 0x58) /* TV signal level configure register 3*/
#define TVE_LTCFG1	(TVE_BASE + 0x60) /* Line timing configure register 1 */
#define TVE_LTCFG2	(TVE_BASE + 0x64) /* Line timing configure register 2 */
#define TVE_CFREQ	(TVE_BASE + 0x70) /* Chrominance sub-carrier frequency configure register */
#define TVE_CPHASE	(TVE_BASE + 0x74) /* Chrominance sub-carrier phase configure register */
#define TVE_CBCRCFG	(TVE_BASE + 0x78) /* Chrominance filter configure register */
#define TVE_WSSCR	(TVE_BASE + 0x80) /* Wide screen signal control register */
#define TVE_WSSCFG1	(TVE_BASE + 0x84) /* Wide screen signal configure register 1 */
#define TVE_WSSCFG2	(TVE_BASE + 0x88) /* Wide screen signal configure register 2 */
#define TVE_WSSCFG3	(TVE_BASE + 0x8c) /* Wide screen signal configure register 3 */

#define REG_TVE_CTRL     REG32(TVE_CTRL)
#define REG_TVE_FRCFG    REG32(TVE_FRCFG)
#define REG_TVE_SLCFG1   REG32(TVE_SLCFG1)
#define REG_TVE_SLCFG2   REG32(TVE_SLCFG2)
#define REG_TVE_SLCFG3   REG32(TVE_SLCFG3)
#define REG_TVE_LTCFG1   REG32(TVE_LTCFG1)
#define REG_TVE_LTCFG2   REG32(TVE_LTCFG2)
#define REG_TVE_CFREQ    REG32(TVE_CFREQ)
#define REG_TVE_CPHASE   REG32(TVE_CPHASE)
#define REG_TVE_CBCRCFG	 REG32(TVE_CBCRCFG)
#define REG_TVE_WSSCR    REG32(TVE_WSSCR)
#define REG_TVE_WSSCFG1  REG32(TVE_WSSCFG1)
#define REG_TVE_WSSCFG2	 REG32(TVE_WSSCFG2)
#define REG_TVE_WSSCFG3  REG32(TVE_WSSCFG3)

/* TV Encoder Control register */
#define TVE_CTRL_EYCBCR         (1 << 25)    /* YCbCr_enable */
#define TVE_CTRL_ECVBS          (1 << 24)    /* 1: cvbs_enable 0: s-video*/
#define TVE_CTRL_DAPD3	        (1 << 23)    /* DAC 3 power down */
#define TVE_CTRL_DAPD2	        (1 << 22)    /* DAC 2 power down */
#define TVE_CTRL_DAPD1	        (1 << 21)    /* DAC 1 power down */
#define TVE_CTRL_DAPD           (1 << 20)    /* power down all DACs */
#define TVE_CTRL_YCDLY_BIT      16
#define TVE_CTRL_YCDLY_MASK     (0x7 << TVE_CTRL_YCDLY_BIT)
#define TVE_CTRL_CGAIN_BIT      14
#define TVE_CTRL_CGAIN_MASK     (0x3 << TVE_CTRL_CGAIN_BIT)
  #define TVE_CTRL_CGAIN_FULL		(0 << TVE_CTRL_CGAIN_BIT) /* gain = 1 */
  #define TVE_CTRL_CGAIN_QUTR		(1 << TVE_CTRL_CGAIN_BIT) /* gain = 1/4 */
  #define TVE_CTRL_CGAIN_HALF		(2 << TVE_CTRL_CGAIN_BIT) /* gain = 1/2 */
  #define TVE_CTRL_CGAIN_THREE_QURT	(3 << TVE_CTRL_CGAIN_BIT) /* gain = 3/4 */
#define TVE_CTRL_CBW_BIT        12
#define TVE_CTRL_CBW_MASK       (0x3 << TVE_CTRL_CBW_BIT)
  #define TVE_CTRL_CBW_NARROW	(0 << TVE_CTRL_CBW_BIT) /* Narrow band */
  #define TVE_CTRL_CBW_WIDE	(1 << TVE_CTRL_CBW_BIT) /* Wide band */
  #define TVE_CTRL_CBW_EXTRA	(2 << TVE_CTRL_CBW_BIT) /* Extra wide band */
  #define TVE_CTRL_CBW_ULTRA	(3 << TVE_CTRL_CBW_BIT) /* Ultra wide band */
#define TVE_CTRL_SYNCT          (1 << 9)
#define TVE_CTRL_PAL            (1 << 8) /* 1: PAL, 0: NTSC */
#define TVE_CTRL_FINV           (1 << 7) /* invert_top:1-invert top and bottom fields. */
#define TVE_CTRL_ZBLACK         (1 << 6) /* bypass_yclamp:1-Black of luminance (Y) input is 0.*/
#define TVE_CTRL_CR1ST          (1 << 5) /* uv_order:0-Cb before Cr,1-Cr before Cb */
#define TVE_CTRL_CLBAR          (1 << 4) /* Color bar mode:0-Output input video to TV,1-Output color bar to TV */
#define TVE_CTRL_SWRST          (1 << 0) /* Software reset:1-TVE is reset */

/* Signal level configure register 1 */
#define TVE_SLCFG1_BLACKL_BIT   0
#define TVE_SLCFG1_BLACKL_MASK  (0x3ff << TVE_SLCFG1_BLACKL_BIT)
#define TVE_SLCFG1_WHITEL_BIT   16
#define TVE_SLCFG1_WHITEL_MASK  (0x3ff << TVE_SLCFG1_WHITEL_BIT)

/* Signal level configure register 2 */
#define TVE_SLCFG2_BLANKL_BIT    0
#define TVE_SLCFG2_BLANKL_MASK   (0x3ff << TVE_SLCFG2_BLANKL_BIT)
#define TVE_SLCFG2_VBLANKL_BIT   16
#define TVE_SLCFG2_VBLANKL_MASK  (0x3ff << TVE_SLCFG2_VBLANKL_BIT)

/* Signal level configure register 3 */
#define TVE_SLCFG3_SYNCL_BIT   0
#define TVE_SLCFG3_SYNCL_MASK  (0xff << TVE_SLCFG3_SYNCL_BIT)

/* Line timing configure register 1 */
#define TVE_LTCFG1_BACKP_BIT   0
#define TVE_LTCFG1_BACKP_MASK  (0x7f << TVE_LTCFG1_BACKP_BIT)
#define TVE_LTCFG1_HSYNCW_BIT   8
#define TVE_LTCFG1_HSYNCW_MASK  (0x7f << TVE_LTCFG1_HSYNCW_BIT)
#define TVE_LTCFG1_FRONTP_BIT   16
#define TVE_LTCFG1_FRONTP_MASK  (0x1f << TVE_LTCFG1_FRONTP_BIT)

/* Line timing configure register 2 */
#define TVE_LTCFG2_BURSTW_BIT    0
#define TVE_LTCFG2_BURSTW_MASK   (0x3f << TVE_LTCFG2_BURSTW_BIT)
#define TVE_LTCFG2_PREBW_BIT     8
#define TVE_LTCFG2_PREBW_MASK    (0x1f << TVE_LTCFG2_PREBW_BIT)
#define TVE_LTCFG2_ACTLIN_BIT    16
#define TVE_LTCFG2_ACTLIN_MASK	(0x7ff << TVE_LTCFG2_ACTLIN_BIT)

/* Chrominance sub-carrier phase configure register */
#define TVE_CPHASE_CCRSTP_BIT    0
#define TVE_CPHASE_CCRSTP_MASK   (0x3 << TVE_CPHASE_CCRSTP_BIT)
  #define TVE_CPHASE_CCRSTP_8	(0 << TVE_CPHASE_CCRSTP_BIT) /* Every 8 field */
  #define TVE_CPHASE_CCRSTP_4	(1 << TVE_CPHASE_CCRSTP_BIT) /* Every 4 field */
  #define TVE_CPHASE_CCRSTP_2	(2 << TVE_CPHASE_CCRSTP_BIT) /* Every 2 lines */
  #define TVE_CPHASE_CCRSTP_0	(3 << TVE_CPHASE_CCRSTP_BIT) /* Never */
#define TVE_CPHASE_ACTPH_BIT     16
#define TVE_CPHASE_ACTPH_MASK    (0xff << TVE_CPHASE_ACTPH_BIT)
#define TVE_CPHASE_INITPH_BIT    24
#define TVE_CPHASE_INITPH_MASK   (0xff << TVE_CPHASE_INITPH_BIT)

/* Chrominance filter configure register */
#define TVE_CBCRCFG_CRGAIN_BIT       0
#define TVE_CBCRCFG_CRGAIN_MASK      (0xff << TVE_CBCRCFG_CRGAIN_BIT)
#define TVE_CBCRCFG_CBGAIN_BIT       8
#define TVE_CBCRCFG_CBGAIN_MASK      (0xff << TVE_CBCRCFG_CBGAIN_BIT)
#define TVE_CBCRCFG_CRBA_BIT         16
#define TVE_CBCRCFG_CRBA_MASK        (0xff << TVE_CBCRCFG_CRBA_BIT)
#define TVE_CBCRCFG_CBBA_BIT         24
#define TVE_CBCRCFG_CBBA_MASK        (0xff << TVE_CBCRCFG_CBBA_BIT)

/* Frame configure register */
#define TVE_FRCFG_NLINE_BIT          0
#define TVE_FRCFG_NLINE_MASK         (0x3ff << TVE_FRCFG_NLINE_BIT)
#define TVE_FRCFG_L1ST_BIT           16
#define TVE_FRCFG_L1ST_MASK          (0xff << TVE_FRCFG_L1ST_BIT)

/* Wide screen signal control register */
#define TVE_WSSCR_EWSS0_BIT	0
#define TVE_WSSCR_EWSS1_BIT	1
#define TVE_WSSCR_WSSTP_BIT	2
#define TVE_WSSCR_WSSCKBP_BIT	3
#define TVE_WSSCR_WSSEDGE_BIT	4
#define TVE_WSSCR_WSSEDGE_MASK	(0x7 << TVE_WSSCR_WSSEDGE_BIT)
#define TVE_WSSCR_ENCH_BIT	8
#define TVE_WSSCR_NCHW_BIT	9
#define TVE_WSSCR_NCHFREQ_BIT	12
#define TVE_WSSCR_NCHFREQ_MASK	(0x7 << TVE_WSSCR_NCHFREQ_BIT)


#ifndef __MIPS_ASSEMBLER

/*************************************************************************
 * TVE (TV Encoder Controller) ops
 *************************************************************************/
/* TV Encoder Control register ops */
#define __tve_soft_reset()		(REG_TVE_CTRL |= TVE_CTRL_SWRST)

#define __tve_output_colorbar()		(REG_TVE_CTRL |= TVE_CTRL_CLBAR)
#define __tve_output_video()		(REG_TVE_CTRL &= ~TVE_CTRL_CLBAR)

#define __tve_input_cr_first()		(REG_TVE_CTRL |= TVE_CTRL_CR1ST)
#define __tve_input_cb_first()		(REG_TVE_CTRL &= ~TVE_CTRL_CR1ST)

#define __tve_set_0_as_black()		(REG_TVE_CTRL |= TVE_CTRL_ZBLACK)
#define __tve_set_16_as_black()		(REG_TVE_CTRL &= ~TVE_CTRL_ZBLACK)

#define __tve_ena_invert_top_bottom()	(REG_TVE_CTRL |= TVE_CTRL_FINV)
#define __tve_dis_invert_top_bottom()	(REG_TVE_CTRL &= ~TVE_CTRL_FINV)

#define __tve_set_pal_mode()		(REG_TVE_CTRL |= TVE_CTRL_PAL)
#define __tve_set_ntsc_mode()		(REG_TVE_CTRL &= ~TVE_CTRL_PAL)

#define __tve_set_pal_dura()		(REG_TVE_CTRL |= TVE_CTRL_SYNCT)
#define __tve_set_ntsc_dura()		(REG_TVE_CTRL &= ~TVE_CTRL_SYNCT)

/* n = 0 ~ 3 */
#define __tve_set_c_bandwidth(n) \
do {\
	REG_TVE_CTRL &= ~TVE_CTRL_CBW_MASK;\
	REG_TVE_CTRL |= (n) << TVE_CTRL_CBW_BIT;	\
}while(0)

/* n = 0 ~ 3 */
#define __tve_set_c_gain(n) \
do {\
	REG_TVE_CTRL &= ~TVE_CTRL_CGAIN_MASK;\
	(REG_TVE_CTRL |= (n) << TVE_CTRL_CGAIN_BIT;	\
}while(0)

/* n = 0 ~ 7 */
#define __tve_set_yc_delay(n)				\
do {							\
	REG_TVE_CTRL &= ~TVE_CTRL_YCDLY_MASK		\
	REG_TVE_CTRL |= ((n) << TVE_CTRL_YCDLY_BIT);	\
} while(0)

#define __tve_disable_all_dacs()	(REG_TVE_CTRL |= TVE_CTRL_DAPD)
#define __tve_disable_dac1()		(REG_TVE_CTRL |= TVE_CTRL_DAPD1)
#define __tve_enable_dac1()		(REG_TVE_CTRL &= ~TVE_CTRL_DAPD1)
#define __tve_disable_dac2()		(REG_TVE_CTRL |= TVE_CTRL_DAPD2)
#define __tve_enable_dac2()		(REG_TVE_CTRL &= ~TVE_CTRL_DAPD2)
#define __tve_disable_dac3()		(REG_TVE_CTRL |= TVE_CTRL_DAPD3)
#define __tve_enable_dac3()		(REG_TVE_CTRL &= ~TVE_CTRL_DAPD3)

#define __tve_enable_svideo_fmt()	(REG_TVE_CTRL |= TVE_CTRL_ECVBS)
#define __tve_enable_cvbs_fmt()		(REG_TVE_CTRL &= ~TVE_CTRL_ECVBS)

/* TV Encoder Frame Configure register ops */
/* n = 0 ~ 255 */
#define __tve_set_first_video_line(n)		\
do {\
		REG_TVE_FRCFG &= ~TVE_FRCFG_L1ST_MASK;\
		REG_TVE_FRCFG |= (n) << TVE_FRCFG_L1ST_BIT;\
} while(0)
/* n = 0 ~ 1023 */
#define __tve_set_line_num_per_frm(n)		\
do {\
		REG_TVE_FRCFG &= ~TVE_FRCFG_NLINE_MASK;\
		REG_TVE_CFG |= (n) << TVE_FRCFG_NLINE_BIT;\
} while(0)
#define __tve_get_video_line_num()\
	(((REG_TVE_FRCFG & TVE_FRCFG_NLINE_MASK) >> TVE_FRCFG_NLINE_BIT) - 1 - 2 * ((REG_TVE_FRCFG & TVE_FRCFG_L1ST_MASK) >> TVE_FRCFG_L1ST_BIT))

/* TV Encoder Signal Level Configure register ops */
/* n = 0 ~ 1023 */
#define __tve_set_white_level(n)		\
do {\
		REG_TVE_SLCFG1 &= ~TVE_SLCFG1_WHITEL_MASK;\
		REG_TVE_SLCFG1 |= (n) << TVE_SLCFG1_WHITEL_BIT;\
} while(0)
/* n = 0 ~ 1023 */
#define __tve_set_black_level(n)		\
do {\
		REG_TVE_SLCFG1 &= ~TVE_SLCFG1_BLACKL_MASK;\
		REG_TVE_SLCFG1 |= (n) << TVE_SLCFG1_BLACKL_BIT;\
} while(0)
/* n = 0 ~ 1023 */
#define __tve_set_blank_level(n)		\
do {\
		REG_TVE_SLCFG2 &= ~TVE_SLCFG2_BLANKL_MASK;\
		REG_TVE_SLCFG2 |= (n) << TVE_SLCFG2_BLANKL_BIT;\
} while(0)
/* n = 0 ~ 1023 */
#define __tve_set_vbi_blank_level(n)		\
do {\
		REG_TVE_SLCFG2 &= ~TVE_SLCFG2_VBLANKL_MASK;\
		REG_TVE_SLCFG2 |= (n) << TVE_SLCFG2_VBLANKL_BIT;\
} while(0)
/* n = 0 ~ 1023 */
#define __tve_set_sync_level(n)		\
do {\
		REG_TVE_SLCFG3 &= ~TVE_SLCFG3_SYNCL_MASK;\
		REG_TVE_SLCFG3 |= (n) << TVE_SLCFG3_SYNCL_BIT;\
} while(0)

/* TV Encoder Signal Level Configure register ops */
/* n = 0 ~ 31 */
#define __tve_set_front_porch(n)		\
do {\
		REG_TVE_LTCFG1 &= ~TVE_LTCFG1_FRONTP_MASK;\
		REG_TVE_LTCFG1 |= (n) << TVE_LTCFG1_FRONTP_BIT;	\
} while(0)
/* n = 0 ~ 127 */
#define __tve_set_hsync_width(n)		\
do {\
		REG_TVE_LTCFG1 &= ~TVE_LTCFG1_HSYNCW_MASK;\
		REG_TVE_LTCFG1 |= (n) << TVE_LTCFG1_HSYNCW_BIT;	\
} while(0)
/* n = 0 ~ 127 */
#define __tve_set_back_porch(n)		\
do {\
		REG_TVE_LTCFG1 &= ~TVE_LTCFG1_BACKP_MASK;\
		REG_TVE_LTCFG1 |= (n) << TVE_LTCFG1_BACKP_BIT;	\
} while(0)
/* n = 0 ~ 2047 */
#define __tve_set_active_linec(n)		\
do {\
		REG_TVE_LTCFG2 &= ~TVE_LTCFG2_ACTLIN_MASK;\
		REG_TVE_LTCFG2 |= (n) << TVE_LTCFG2_ACTLIN_BIT;	\
} while(0)
/* n = 0 ~ 31 */
#define __tve_set_breezy_way(n)		\
do {\
		REG_TVE_LTCFG2 &= ~TVE_LTCFG2_PREBW_MASK;\
		REG_TVE_LTCFG2 |= (n) << TVE_LTCFG2_PREBW_BIT;	\
} while(0)

/* n = 0 ~ 127 */
#define __tve_set_burst_width(n)		\
do {\
		REG_TVE_LTCFG2 &= ~TVE_LTCFG2_BURSTW_MASK;\
		REG_TVE_LTCFG2 |= (n) << TVE_LTCFG2_BURSTW_BIT;	\
} while(0)

/* TV Encoder Chrominance filter and Modulation register ops */
/* n = 0 ~ (2^32-1) */
#define __tve_set_c_sub_carrier_freq(n)  REG_TVE_CFREQ = (n)
/* n = 0 ~ 255 */
#define __tve_set_c_sub_carrier_init_phase(n) \
do {   \
	REG_TVE_CPHASE &= ~TVE_CPHASE_INITPH_MASK;	\
	REG_TVE_CPHASE |= (n) << TVE_CPHASE_INITPH_BIT;	\
} while(0)
/* n = 0 ~ 255 */
#define __tve_set_c_sub_carrier_act_phase(n) \
do {   \
	REG_TVE_CPHASE &= ~TVE_CPHASE_ACTPH_MASK;	\
	REG_TVE_CPHASE |= (n) << TVE_CPHASE_ACTPH_BIT;	\
} while(0)
/* n = 0 ~ 255 */
#define __tve_set_c_phase_rst_period(n) \
do {   \
	REG_TVE_CPHASE &= ~TVE_CPHASE_CCRSTP_MASK;	\
	REG_TVE_CPHASE |= (n) << TVE_CPHASE_CCRSTP_BIT;	\
} while(0)
/* n = 0 ~ 255 */
#define __tve_set_cb_burst_amp(n) \
do {   \
	REG_TVE_CBCRCFG &= ~TVE_CBCRCFG_CBBA_MASK;	\
	REG_TVE_CBCRCFG |= (n) << TVE_CBCRCFG_CBBA_BIT;	\
} while(0)
/* n = 0 ~ 255 */
#define __tve_set_cr_burst_amp(n) \
do {   \
	REG_TVE_CBCRCFG &= ~TVE_CBCRCFG_CRBA_MASK;	\
	REG_TVE_CBCRCFG |= (n) << TVE_CBCRCFG_CRBA_BIT;	\
} while(0)
/* n = 0 ~ 255 */
#define __tve_set_cb_gain_amp(n) \
do {   \
	REG_TVE_CBCRCFG &= ~TVE_CBCRCFG_CBGAIN_MASK;	\
	REG_TVE_CBCRCFG |= (n) << TVE_CBCRCFG_CBGAIN_BIT;	\
} while(0)
/* n = 0 ~ 255 */
#define __tve_set_cr_gain_amp(n) \
do {   \
	REG_TVE_CBCRCFG &= ~TVE_CBCRCFG_CRGAIN_MASK;	\
	REG_TVE_CBCRCFG |= (n) << TVE_CBCRCFG_CRGAIN_BIT;	\
} while(0)

/* TV Encoder Wide Screen Signal Control register ops */
/* n = 0 ~ 7 */
#define __tve_set_notch_freq(n) \
do {   \
	REG_TVE_WSSCR &= ~TVE_WSSCR_NCHFREQ_MASK;	\
	REG_TVE_WSSCR |= (n) << TVE_WSSCR_NCHFREQ_BIT;	\
} while(0)
/* n = 0 ~ 7 */
#define __tve_set_notch_width()	(REG_TVE_WSSCR |= TVE_WSSCR_NCHW_BIT)
#define __tve_clear_notch_width()	(REG_TVE_WSSCR &= ~TVE_WSSCR_NCHW_BIT)
#define __tve_enable_notch()		(REG_TVE_WSSCR |= TVE_WSSCR_ENCH_BIT)
#define __tve_disable_notch()		(REG_TVE_WSSCR &= ~TVE_WSSCR_ENCH_BIT)
/* n = 0 ~ 7 */
#define __tve_set_wss_edge(n) \
do {   \
	REG_TVE_WSSCR &= ~TVE_WSSCR_WSSEDGE_MASK;	\
	REG_TVE_WSSCR |= (n) << TVE_WSSCR_WSSEDGE_BIT;	\
} while(0)
#define __tve_set_wss_clkbyp()		(REG_TVE_WSSCR |= TVE_WSSCR_WSSCKBP_BIT)
#define __tve_set_wss_type()		(REG_TVE_WSSCR |= TVE_WSSCR_WSSTP_BIT)
#define __tve_enable_wssf1()		(REG_TVE_WSSCR |= TVE_WSSCR_EWSS1_BIT)
#define __tve_enable_wssf0()		(REG_TVE_WSSCR |= TVE_WSSCR_EWSS0_BIT)

/* TV Encoder Wide Screen Signal Configure register 1, 2 and 3 ops */
/* n = 0 ~ 1023 */
#define __tve_set_wss_level(n) \
do {   \
	REG_TVE_WSSCFG1 &= ~TVE_WSSCFG1_WSSL_MASK;	\
	REG_TVE_WSSCFG1 |= (n) << TVE_WSSCFG1_WSSL_BIT;	\
} while(0)
/* n = 0 ~ 4095 */
#define __tve_set_wss_freq(n) \
do {   \
	REG_TVE_WSSCFG1 &= ~TVE_WSSCFG1_WSSFREQ_MASK;	\
	REG_TVE_WSSCFG1 |= (n) << TVE_WSSCFG1_WSSFREQ_BIT;	\
} while(0)
/* n = 0, 1; l = 0 ~ 255 */
#define __tve_set_wss_line(n,v)			\
do {   \
	REG_TVE_WSSCFG##n &= ~TVE_WSSCFG_WSSLINE_MASK;	\
	REG_TVE_WSSCFG##n |= (v) << TVE_WSSCFG_WSSLINE_BIT;	\
} while(0)
/* n = 0, 1; d = 0 ~ (2^20-1) */
#define __tve_set_wss_data(n, v)			\
do {   \
	REG_TVE_WSSCFG##n &= ~TVE_WSSCFG_WSSLINE_MASK;	\
	REG_TVE_WSSCFG##n |= (v) << TVE_WSSCFG_WSSLINE_BIT;	\
} while(0)


#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4770TVE_H__ */

/*
 * linux/include/asm-mips/mach-jz4770/jz4770ipu.h
 *
 * JZ4770 IPU register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770IPU_H__
#define __JZ4770IPU_H__


/*************************************************************************
 * IPU (Image Processing Unit)
 *************************************************************************/

/* Register offset */
#define REG_CTRL		0x0  /* IPU Control Register */
#define REG_STATUS		0x4  /* IPU Status Register */
#define REG_D_FMT		0x8  /* Data Format Register */
#define REG_Y_ADDR		0xc  /* Input Y or YUV422 Packaged Data Address Register */
#define REG_U_ADDR		0x10 /* Input U Data Address Register */
#define REG_V_ADDR		0x14 /* Input V Data Address Register */
#define REG_IN_FM_GS		0x18 /* Input Geometric Size Register */
#define REG_Y_STRIDE		0x1c /* Input Y Data Line Stride Register */
#define REG_UV_STRIDE		0x20 /* Input UV Data Line Stride Register */
#define REG_OUT_ADDR		0x24 /* Output Frame Start Address Register */
#define REG_OUT_GS		0x28 /* Output Geometric Size Register */
#define REG_OUT_STRIDE		0x2c /* Output Data Line Stride Register */
#define REG_RSZ_COEF_INDEX	0x30 /* Resize Coefficients Table Index Register */
#define REG_CSC_CO_COEF		0x34 /* CSC C0 Coefficient Register */
#define REG_CSC_C1_COEF		0x38 /* CSC C1 Coefficient Register */
#define REG_CSC_C2_COEF 	0x3c /* CSC C2 Coefficient Register */
#define REG_CSC_C3_COEF 	0x40 /* CSC C3 Coefficient Register */
#define REG_CSC_C4_COEF 	0x44 /* CSC C4 Coefficient Register */
#define HRSZ_LUT_BASE 		0x48 /* Horizontal Resize Coefficients Look Up Table Register group */
#define VRSZ_LUT_BASE 		0x4c /* Virtical Resize Coefficients Look Up Table Register group */
#define REG_CSC_OFSET_PARA	0x50 /* CSC Offset Parameter Register */
#define REG_Y_PHY_T_ADDR	0x54 /* Input Y Physical Table Address Register */
#define REG_U_PHY_T_ADDR	0x58 /* Input U Physical Table Address Register */
#define REG_V_PHY_T_ADDR	0x5c /* Input V Physical Table Address Register */
#define REG_OUT_PHY_T_ADDR	0x60 /* Output Physical Table Address Register */

/* REG_CTRL: IPU Control Register */
#define IPU_CE_SFT	0x0
#define IPU_CE_MSK	0x1
#define IPU_RUN_SFT	0x1
#define IPU_RUN_MSK	0x1
#define HRSZ_EN_SFT	0x2
#define HRSZ_EN_MSK	0x1
#define VRSZ_EN_SFT	0x3
#define VRSZ_EN_MSK	0x1
#define CSC_EN_SFT	0x4
#define CSC_EN_MSK	0x1
#define FM_IRQ_EN_SFT	0x5
#define FM_IRQ_EN_MSK	0x1
#define IPU_RST_SFT	0x6
#define IPU_RST_MSK	0x1
#define H_SCALE_SFT	0x8
#define H_SCALE_MSK	0x1
#define V_SCALE_SFT	0x9
#define V_SCALE_MSK	0x1
#define PKG_SEL_SFT	0xA
#define PKG_SEL_MSK	0x1
#define LCDC_SEL_SFT	0xB
#define LCDC_SEL_MSK	0x1
#define SPAGE_MAP_SFT	0xC
#define SPAGE_MAP_MSK	0x1
#define DPAGE_SEL_SFT	0xD
#define DPAGE_SEL_MSK	0x1
#define DISP_SEL_SFT	0xE
#define DISP_SEL_MSK	0x1
#define FIELD_CONF_EN_SFT 15
#define FIELD_CONF_EN_MSK 1
#define FIELD_SEL_SFT	16
#define FIELD_SEL_MSK	1
#define DFIX_SEL_SFT	17
#define DFIX_SEL_MSK	1

/* REG_STATUS: IPU Status Register */
#define OUT_END_SFT	0x0
#define OUT_END_MSK	0x1
#define FMT_ERR_SFT	0x1
#define FMT_ERR_MSK	0x1
#define SIZE_ERR_SFT	0x2
#define SIZE_ERR_MSK	0x1

/* D_FMT: Data Format Register */
#define IN_FMT_SFT	0x0
#define IN_FMT_MSK 	0x3
#define IN_OFT_SFT 	0x2
#define IN_OFT_MSK 	0x3
#define YUV_PKG_OUT_SFT	0x10
#define YUV_PKG_OUT_MSK	0x7
#define OUT_FMT_SFT 	0x13
#define OUT_FMT_MSK 	0x3
#define RGB_OUT_OFT_SFT	0x15
#define RGB_OUT_OFT_MSK	0x7
#define RGB888_FMT_SFT	0x18
#define RGB888_FMT_MSK	0x1

/* IN_FM_GS: Input Geometric Size Register */
#define IN_FM_H_SFT	0x0
#define IN_FM_H_MSK	0xFFF
#define IN_FM_W_SFT	0x10
#define IN_FM_W_MSK	0xFFF

/* Y_STRIDE: Input Y Data Line Stride Register */
#define Y_S_SFT		0x0
#define Y_S_MSK		0x3FFF

/* UV_STRIDE: Input UV Data Line Stride Register */
#define V_S_SFT		0x0
#define V_S_MSK		0x1FFF
#define U_S_SFT 	0x10
#define U_S_MSK		0x1FFF

/* OUT_GS: Output Geometric Size Register */
#define OUT_FM_H_SFT	0x0
#define OUT_FM_H_MSK	0x1FFF
#define OUT_FM_W_SFT	0x10
#define OUT_FM_W_MSK	0x7FFF

/* OUT_STRIDE: Output Data Line Stride Register */
#define OUT_S_SFT	0x0
#define OUT_S_MSK	0xFFFF

/* RSZ_COEF_INDEX: Resize Coefficients Table Index Register */
#define VE_IDX_SFT	0x0
#define VE_IDX_MSK	0x1F
#define HE_IDX_SFT	0x10
#define HE_IDX_MSK	0x1F

/* CSC_CX_COEF: CSC CX Coefficient Register */
#define CX_COEF_SFT	0x0
#define CX_COEF_MSK	0xFFF

/* HRSZ_LUT_BASE, VRSZ_LUT_BASE: Resize Coefficients Look Up Table Register group */
#define LUT_LEN		20

#define OUT_N_SFT	0x0
#define OUT_N_MSK	0x1
#define IN_N_SFT	0x1
#define IN_N_MSK	0x1
#define W_COEF_SFT	0x2
#define W_COEF_MSK	0x3FF

/* CSC_OFSET_PARA: CSC Offset Parameter Register */
#define CHROM_OF_SFT	0x10
#define CHROM_OF_MSK	0xFF
#define LUMA_OF_SFT	0x00
#define LUMA_OF_MSK	0xFF


#ifndef __MIPS_ASSEMBLER


#if 0
/*************************************************************************
 * IPU (Image Processing Unit)
 *************************************************************************/
#define u32 volatile unsigned long

#define write_reg(reg, val)	\
do {				\
	*(u32 *)(reg) = (val);	\
} while(0)

#define read_reg(reg, off)	(*(u32 *)((reg)+(off)))


#define set_ipu_fmt(rgb_888_out_fmt, rgb_out_oft, out_fmt, yuv_pkg_out, in_oft, in_fmt ) \
({ write_reg( (IPU_V_BASE + REG_D_FMT), ((in_fmt) & IN_FMT_MSK)<<IN_FMT_SFT \
| ((in_oft) & IN_OFT_MSK)<< IN_OFT_SFT \
| ((out_fmt) & OUT_FMT_MSK)<<OUT_FMT_SFT \
| ((yuv_pkg_out) & YUV_PKG_OUT_MSK ) << YUV_PKG_OUT_SFT \
| ((rgb_888_out_fmt) & RGB888_FMT_MSK ) << RGB888_FMT_SFT \
| ((rgb_out_oft) & RGB_OUT_OFT_MSK ) << RGB_OUT_OFT_SFT); \
})
#define set_y_addr(y_addr) \
({ write_reg( (IPU_V_BASE + REG_Y_ADDR), y_addr); \
})
#define set_u_addr(u_addr) \
({ write_reg( (IPU_V_BASE + REG_U_ADDR), u_addr); \
})

#define set_v_addr(v_addr) \
({ write_reg( (IPU_V_BASE + REG_V_ADDR), v_addr); \
})

#define set_y_phy_t_addr(y_phy_t_addr) \
({ write_reg( (IPU_V_BASE + REG_Y_PHY_T_ADDR), y_phy_t_addr); \
})

#define set_u_phy_t_addr(u_phy_t_addr) \
({ write_reg( (IPU_V_BASE + REG_U_PHY_T_ADDR), u_phy_t_addr); \
})

#define set_v_phy_t_addr(v_phy_t_addr) \
({ write_reg( (IPU_V_BASE + REG_V_PHY_T_ADDR), v_phy_t_addr); \
})

#define set_out_phy_t_addr(out_phy_t_addr) \
({ write_reg( (IPU_V_BASE + REG_OUT_PHY_T_ADDR), out_phy_t_addr); \
})

#define set_inframe_gsize(width, height, y_stride, u_stride, v_stride) \
({ write_reg( (IPU_V_BASE + REG_IN_FM_GS), ((width) & IN_FM_W_MSK)<<IN_FM_W_SFT \
| ((height) & IN_FM_H_MSK)<<IN_FM_H_SFT); \
 write_reg( (IPU_V_BASE + REG_Y_STRIDE), ((y_stride) & Y_S_MSK)<<Y_S_SFT); \
 write_reg( (IPU_V_BASE + REG_UV_STRIDE), ((u_stride) & U_S_MSK)<<U_S_SFT \
| ((v_stride) & V_S_MSK)<<V_S_SFT); \
})
#define set_out_addr(out_addr) \
({ write_reg( (IPU_V_BASE + REG_OUT_ADDR), out_addr); \
})
#define set_outframe_gsize(width, height, o_stride) \
({ write_reg( (IPU_V_BASE + REG_OUT_GS), ((width) & OUT_FM_W_MSK)<<OUT_FM_W_SFT \
| ((height) & OUT_FM_H_MSK)<<OUT_FM_H_SFT); \
 write_reg( (IPU_V_BASE + REG_OUT_STRIDE), ((o_stride) & OUT_S_MSK)<<OUT_S_SFT); \
})
#define set_rsz_lut_end(h_end, v_end) \
({ write_reg( (IPU_V_BASE + REG_RSZ_COEF_INDEX), ((h_end) & HE_IDX_MSK)<<HE_IDX_SFT \
| ((v_end) & VE_IDX_MSK)<<VE_IDX_SFT); \
})
#define set_csc_c0(c0_coeff) \
({ write_reg( (IPU_V_BASE + REG_CSC_CO_COEF), ((c0_coeff) & CX_COEF_MSK)<<CX_COEF_SFT); \
})
#define set_csc_c1(c1_coeff) \
({ write_reg( (IPU_V_BASE + REG_CSC_C1_COEF), ((c1_coeff) & CX_COEF_MSK)<<CX_COEF_SFT); \
})
#define set_csc_c2(c2_coeff) \
({ write_reg( (IPU_V_BASE + REG_CSC_C2_COEF), ((c2_coeff) & CX_COEF_MSK)<<CX_COEF_SFT); \
})
#define set_csc_c3(c3_coeff) \
({ write_reg( (IPU_V_BASE + REG_CSC_C3_COEF), ((c3_coeff) & CX_COEF_MSK)<<CX_COEF_SFT); \
})
#define set_csc_c4(c4_coeff) \
({ write_reg( (IPU_V_BASE + REG_CSC_C4_COEF), ((c4_coeff) & CX_COEF_MSK)<<CX_COEF_SFT); \
})
#define set_hrsz_lut_coef(coef, in_n, out_n) \
({ write_reg( (IPU_V_BASE + HRSZ_LUT_BASE ), ((coef) & W_COEF_MSK)<<W_COEF_SFT \
| ((in_n) & IN_N_MSK)<<IN_N_SFT | ((out_n) & OUT_N_MSK)<<OUT_N_SFT); \
})
#define set_vrsz_lut_coef(coef, in_n, out_n) \
({ write_reg( (IPU_V_BASE + VRSZ_LUT_BASE), ((coef) & W_COEF_MSK)<<W_COEF_SFT \
| ((in_n) & IN_N_MSK)<<IN_N_SFT | ((out_n) & OUT_N_MSK)<<OUT_N_SFT); \
})

#define set_primary_ctrl(vrsz_en, hrsz_en,csc_en, irq_en) \
({ write_reg( (IPU_V_BASE + REG_CTRL), ((irq_en) & FM_IRQ_EN_MSK)<<FM_IRQ_EN_SFT \
| ((vrsz_en) & VRSZ_EN_MSK)<<VRSZ_EN_SFT \
| ((hrsz_en) & HRSZ_EN_MSK)<<HRSZ_EN_SFT \
| ((csc_en) & CSC_EN_MSK)<<CSC_EN_SFT \
| (read_reg(IPU_V_BASE, REG_CTRL)) \
& ~(CSC_EN_MSK<<CSC_EN_SFT | FM_IRQ_EN_MSK<<FM_IRQ_EN_SFT | VRSZ_EN_MSK<<VRSZ_EN_SFT | HRSZ_EN_MSK<<HRSZ_EN_SFT ) ); \
})

#define set_source_ctrl(pkg_sel, spage_sel) \
({ write_reg( (IPU_V_BASE + REG_CTRL), ((pkg_sel) & PKG_SEL_MSK  )<< PKG_SEL_SFT \
| ((spage_sel) & SPAGE_MAP_MSK )<< SPAGE_MAP_SFT \
| (read_reg(IPU_V_BASE, REG_CTRL)) \
& ~(SPAGE_MAP_MSK << SPAGE_MAP_SFT | PKG_SEL_MSK << PKG_SEL_SFT ) ) ; \
})

#define set_out_ctrl(lcdc_sel, dpage_sel, disp_sel) \
({ write_reg( (IPU_V_BASE + REG_CTRL), ((lcdc_sel) & LCDC_SEL_MSK  )<< LCDC_SEL_SFT \
| ((dpage_sel) & DPAGE_SEL_MSK )<< DPAGE_SEL_SFT \
| ((disp_sel) & DISP_SEL_MSK )<< DISP_SEL_SFT \
| (read_reg(IPU_V_BASE, REG_CTRL)) \
& ~(LCDC_SEL_MSK<< LCDC_SEL_SFT | DPAGE_SEL_MSK << DPAGE_SEL_SFT | DISP_SEL_MSK << DISP_SEL_SFT ) ); \
})

#define set_scale_ctrl(v_scal, h_scal) \
({ write_reg( (IPU_V_BASE + REG_CTRL), ((v_scal) & V_SCALE_MSK)<<V_SCALE_SFT \
| ((h_scal) & H_SCALE_MSK)<<H_SCALE_SFT \
| (read_reg(IPU_V_BASE, REG_CTRL)) & ~(V_SCALE_MSK<<V_SCALE_SFT | H_SCALE_MSK<<H_SCALE_SFT ) ); \
})


#define set_csc_ofset_para(chrom_oft, luma_oft) \
({ write_reg( (IPU_V_BASE + REG_CSC_OFSET_PARA ), ((chrom_oft) & CHROM_OF_MSK ) << CHROM_OF_SFT \
| ((luma_oft) & LUMA_OF_MSK ) << LUMA_OF_SFT ) ; \
})

#define sw_reset_ipu() \
({ write_reg( (IPU_V_BASE + REG_CTRL), (read_reg(IPU_V_BASE, REG_CTRL)) \
| IPU_RST_MSK<<IPU_RST_SFT); \
})
#define enable_ipu() \
({ write_reg( (IPU_V_BASE + REG_CTRL), (read_reg(IPU_V_BASE, REG_CTRL)) | 0x1); \
})
#define disable_ipu() \
({ write_reg( (IPU_V_BASE + REG_CTRL), (read_reg(IPU_V_BASE, REG_CTRL)) & ~0x1); \
})
#define run_ipu() \
({ write_reg( (IPU_V_BASE + REG_CTRL), (read_reg(IPU_V_BASE, REG_CTRL)) | 0x2); \
})
#define stop_ipu() \
({ write_reg( (IPU_V_BASE + REG_CTRL), (read_reg(IPU_V_BASE, REG_CTRL)) & ~0x2); \
})

#define polling_end_flag() \
({ (read_reg(IPU_V_BASE, REG_STATUS)) & 0x01; \
})

#define start_vlut_coef_write() \
({ write_reg( (IPU_V_BASE + VRSZ_LUT_BASE), ( 0x1<<12 ) ); \
})

#define start_hlut_coef_write() \
({ write_reg( (IPU_V_BASE + HRSZ_LUT_BASE), ( 0x01<<12 ) ); \
})

#define clear_end_flag() \
({ write_reg( (IPU_V_BASE + REG_STATUS), 0); \
})
#endif /* #if 0 */


#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4770IPU_H__ */


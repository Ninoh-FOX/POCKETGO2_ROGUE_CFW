/*
 * linux/include/asm-mips/mach-jz4770/jz4770ddrc.h
 *
 * JZ4770 DDRC register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770DDRC_H__
#define __JZ4770DDRC_H__

#include <asm/mach-jz4770/jz4770misc.h>


#define	DDRC_BASE	0xB3020000

/*************************************************************************
 * DDRC (DDR Controller)
 *************************************************************************/
#define DDRC_ST		(DDRC_BASE + 0x0) /* DDR Status Register */
#define DDRC_CFG	(DDRC_BASE + 0x4) /* DDR Configure Register */
#define DDRC_CTRL	(DDRC_BASE + 0x8) /* DDR Control Register */
#define DDRC_LMR	(DDRC_BASE + 0xc) /* DDR Load-Mode-Register */
#define DDRC_TIMING1	(DDRC_BASE + 0x10) /* DDR Timing Config Register 1 */
#define DDRC_TIMING2	(DDRC_BASE + 0x14) /* DDR Timing Config Register 2 */
#define DDRC_REFCNT	(DDRC_BASE + 0x18) /* DDR  Auto-Refresh Counter */
#define DDRC_DQS	(DDRC_BASE + 0x1c) /* DDR DQS Delay Control Register */
#define DDRC_DQS_ADJ	(DDRC_BASE + 0x20) /* DDR DQS Delay Adjust Register */
#define DDRC_MMAP0	(DDRC_BASE + 0x24) /* DDR Memory Map Config Register */
#define DDRC_MMAP1	(DDRC_BASE + 0x28) /* DDR Memory Map Config Register */
#define DDRC_PMEMCTRL0	(DDRC_BASE + 0x54)
#define DDRC_PMEMCTRL1	(DDRC_BASE + 0x50)
#define DDRC_PMEMCTRL2	(DDRC_BASE + 0x58)
#define DDRC_PMEMCTRL3	(DDRC_BASE + 0x5c)
#define DDRC_MPORT	(DDRC_BASE + 0x60)

/* DDRC Register */
#define REG_DDRC_ST		REG32(DDRC_ST)
#define REG_DDRC_CFG		REG32(DDRC_CFG)
#define REG_DDRC_CTRL		REG32(DDRC_CTRL)
#define REG_DDRC_LMR		REG32(DDRC_LMR)
#define REG_DDRC_TIMING1	REG32(DDRC_TIMING1)
#define REG_DDRC_TIMING2	REG32(DDRC_TIMING2)
#define REG_DDRC_REFCNT		REG32(DDRC_REFCNT)
#define REG_DDRC_DQS		REG32(DDRC_DQS)
#define REG_DDRC_DQS_ADJ	REG32(DDRC_DQS_ADJ)
#define REG_DDRC_MMAP0		REG32(DDRC_MMAP0)
#define REG_DDRC_MMAP1		REG32(DDRC_MMAP1)
#define REG_DDRC_PMEMCTRL0	REG32(DDRC_PMEMCTRL0)
#define REG_DDRC_PMEMCTRL1	REG32(DDRC_PMEMCTRL1)
#define REG_DDRC_PMEMCTRL2	REG32(DDRC_PMEMCTRL2)
#define REG_DDRC_PMEMCTRL3	REG32(DDRC_PMEMCTRL3)
#define REG_DDRC_MPORT	        REG32(DDRC_MPORT)

/* DDRC Status Register */
#define DDRC_ST_ENDIAN	(1 << 7) /* 0 Little data endian
					    1 Big data endian */
#define DDRC_ST_DPDN		(1 << 5) /* 0 DDR memory is NOT in deep-power-down state
					    1 DDR memory is in deep-power-down state */
#define DDRC_ST_PDN		(1 << 4) /* 0 DDR memory is NOT in power-down state
					    1 DDR memory is in power-down state */
#define DDRC_ST_AREF		(1 << 3) /* 0 DDR memory is NOT in auto-refresh state
					    1 DDR memory is in auto-refresh state */
#define DDRC_ST_SREF		(1 << 2) /* 0 DDR memory is NOT in self-refresh state
					    1 DDR memory is in self-refresh state */
#define DDRC_ST_CKE1		(1 << 1) /* 0 CKE1 Pin is low
					    1 CKE1 Pin is high */
#define DDRC_ST_CKE0		(1 << 0) /* 0 CKE0 Pin is low
					    1 CKE0 Pin is high */

/* DDRC Configure Register */
#define DDRC_CFG_MSEL_BIT	16 /* Mask delay select */
#define DDRC_CFG_MSEL_MASK	(0x3 << DDRC_CFG_MSEL_BIT)
  #define DDRC_CFG_MSEL_0	(0 << DDRC_CFG_MSEL_BIT) /* 00 No delay */
  #define DDRC_CFG_MSEL_1	(1 << DDRC_CFG_MSEL_BIT) /* 01 delay 1 tCK */
  #define DDRC_CFG_MSEL_2	(2 << DDRC_CFG_MSEL_BIT) /* 10 delay 2 tCK */
  #define DDRC_CFG_MSEL_3	(3 << DDRC_CFG_MSEL_BIT) /* 11 delay 3 tCK */

#define DDRC_CFG_HL		(1 << 15) /* 0: no extra delay 1: one extra half tCK delay */

#define DDRC_CFG_ROW1_BIT	27 /* Row Address width. */
#define DDRC_CFG_COL1_BIT	25 /* Row Address width. */
#define DDRC_CFG_BA1_BIT	(1 << 24)
#define DDRC_CFG_IMBA_BIT	(1 << 23)
#define DDRC_CFG_BTRUN		(1 << 21)

#define DDRC_CFG_TYPE_BIT	12
#define DDRC_CFG_TYPE_MASK	(0x7 << DDRC_CFG_TYPE_BIT)
#define DDRC_CFG_TYPE_DDR1	(2 << DDRC_CFG_TYPE_BIT)
#define DDRC_CFG_TYPE_MDDR	(3 << DDRC_CFG_TYPE_BIT)
#define DDRC_CFG_TYPE_DDR2	(4 << DDRC_CFG_TYPE_BIT)

#define DDRC_CFG_ROW_BIT	10 /* Row Address width. */
#define DDRC_CFG_ROW_MASK	(0x3 << DDRC_CFG_ROW_BIT)
  #define DDRC_CFG_ROW_13	(0 << DDRC_CFG_ROW_BIT) /* 13-bit row address is used */
  #define DDRC_CFG_ROW_14	(1 << DDRC_CFG_ROW_BIT) /* 14-bit row address is used */

#define DDRC_CFG_COL_BIT	8 /* Column Address width.
				     Specify the Column address width of external DDR. */
#define DDRC_CFG_COL_MASK	(0x3 << DDRC_CFG_COL_BIT)
  #define DDRC_CFG_COL_9	(0 << DDRC_CFG_COL_BIT) /* 9-bit Column address is used */
  #define DDRC_CFG_COL_10	(1 << DDRC_CFG_COL_BIT) /* 10-bit Column address is used */

#define DDRC_CFG_CS1EN	(1 << 7) /* 0 DDR Pin CS1 un-used
					    1 There're DDR memory connected to CS1 */
#define DDRC_CFG_CS0EN	(1 << 6) /* 0 DDR Pin CS0 un-used
					    1 There're DDR memory connected to CS0 */

#define DDRC_CFG_TSEL_BIT	18 /* Read delay select */
#define DDRC_CFG_TSEL_MASK	(0x3 << DDRC_CFG_TSEL_BIT)
#define DDRC_CFG_TSEL_0	(0 << DDRC_CFG_TSEL_BIT) /* No delay */
#define DDRC_CFG_TSEL_1	(1 << DDRC_CFG_TSEL_BIT) /* delay 1 tCK */
#define DDRC_CFG_TSEL_2	(2 << DDRC_CFG_TSEL_BIT) /* delay 2 tCK */
#define DDRC_CFG_TSEL_3	(3 << DDRC_CFG_TSEL_BIT) /* delay 3 tCK */

#define DDRC_CFG_CL_BIT	2 /* CAS Latency */
#define DDRC_CFG_CL_MASK	(0xf << DDRC_CFG_CL_BIT)
#define DDRC_CFG_CL_3		(0 << DDRC_CFG_CL_BIT) /* CL = 3 tCK */
#define DDRC_CFG_CL_4		(1 << DDRC_CFG_CL_BIT) /* CL = 4 tCK */
#define DDRC_CFG_CL_5		(2 << DDRC_CFG_CL_BIT) /* CL = 5 tCK */
#define DDRC_CFG_CL_6		(3 << DDRC_CFG_CL_BIT) /* CL = 6 tCK */

#define DDRC_CFG_BA		(1 << 1) /* 0 4 bank device, Pin ba[1:0] valid, ba[2] un-used
					    1 8 bank device, Pin ba[2:0] valid*/
#define DDRC_CFG_DW		(1 << 0) /*0 External memory data width is 16-bit
					   1 External memory data width is 32-bit */

/* DDRC Control Register */
#define DDRC_CTRL_ACTPD	(1 << 15) /* 0 Precharge all banks before entering power-down
					     1 Do not precharge banks before entering power-down */
#define DDRC_CTRL_PDT_BIT	12 /* Power-Down Timer */
#define DDRC_CTRL_PDT_MASK	(0x7 << DDRC_CTRL_PDT_BIT)
  #define DDRC_CTRL_PDT_DIS	(0 << DDRC_CTRL_PDT_BIT) /* power-down disabled */
  #define DDRC_CTRL_PDT_8	(1 << DDRC_CTRL_PDT_BIT) /* Enter power-down after 8 tCK idle */
  #define DDRC_CTRL_PDT_16	(2 << DDRC_CTRL_PDT_BIT) /* Enter power-down after 16 tCK idle */
  #define DDRC_CTRL_PDT_32	(3 << DDRC_CTRL_PDT_BIT) /* Enter power-down after 32 tCK idle */
  #define DDRC_CTRL_PDT_64	(4 << DDRC_CTRL_PDT_BIT) /* Enter power-down after 64 tCK idle */
  #define DDRC_CTRL_PDT_128	(5 << DDRC_CTRL_PDT_BIT) /* Enter power-down after 128 tCK idle */

#define DDRC_CTRL_PRET_BIT	8 /* Precharge Timer */
#define DDRC_CTRL_PRET_MASK	(0x7 << DDRC_CTRL_PRET_BIT) /*  */
  #define DDRC_CTRL_PRET_DIS	(0 << DDRC_CTRL_PRET_BIT) /* PRET function Disabled */
  #define DDRC_CTRL_PRET_8	(1 << DDRC_CTRL_PRET_BIT) /* Precharge active bank after 8 tCK idle */
  #define DDRC_CTRL_PRET_16	(2 << DDRC_CTRL_PRET_BIT) /* Precharge active bank after 16 tCK idle */
  #define DDRC_CTRL_PRET_32	(3 << DDRC_CTRL_PRET_BIT) /* Precharge active bank after 32 tCK idle */
  #define DDRC_CTRL_PRET_64	(4 << DDRC_CTRL_PRET_BIT) /* Precharge active bank after 64 tCK idle */
  #define DDRC_CTRL_PRET_128	(5 << DDRC_CTRL_PRET_BIT) /* Precharge active bank after 128 tCK idle */

#define DDRC_CTRL_SR		(1 << 5) /* 1 Drive external DDR device entering self-refresh mode
					    0 Drive external DDR device exiting self-refresh mode */
#define DDRC_CTRL_UNALIGN	(1 << 4) /* 0 Disable unaligned transfer on AXI BUS
					    1 Enable unaligned transfer on AXI BUS */
#define DDRC_CTRL_ALH		(1 << 3) /* Advanced Latency Hiding:
					    0 Disable ALH
					    1 Enable ALH */
#define DDRC_CTRL_RDC		(1 << 2) /* 0 dclk clock frequency is lower than 60MHz
					    1 dclk clock frequency is higher than 60MHz */
#define DDRC_CTRL_CKE		(1 << 1) /* 0 Not set CKE Pin High
					    1 Set CKE Pin HIGH */
#define DDRC_CTRL_RESET	(1 << 0) /* 0 End resetting ddrc_controller
					    1 Resetting ddrc_controller */

/* DDRC Load-Mode-Register */
#define DDRC_LMR_DDR_ADDR_BIT	16 /* When performing a DDR command, DDRC_ADDR[13:0]
					      corresponding to external DDR address Pin A[13:0] */
#define DDRC_LMR_DDR_ADDR_MASK	(0xff << DDRC_LMR_DDR_ADDR_BIT)

#define DDRC_LMR_BA_BIT		8 /* When performing a DDR command, BA[2:0]
				     corresponding to external DDR address Pin BA[2:0]. */
#define DDRC_LMR_BA_MASK	(0x7 << DDRC_LMR_BA_BIT)
  /* For DDR2 */
  #define DDRC_LMR_BA_MRS	(0 << DDRC_LMR_BA_BIT) /* Mode Register set */
  #define DDRC_LMR_BA_EMRS1	(1 << DDRC_LMR_BA_BIT) /* Extended Mode Register1 set */
  #define DDRC_LMR_BA_EMRS2	(2 << DDRC_LMR_BA_BIT) /* Extended Mode Register2 set */
  #define DDRC_LMR_BA_EMRS3	(3 << DDRC_LMR_BA_BIT) /* Extended Mode Register3 set */
  /* For mobile DDR */
  #define DDRC_LMR_BA_M_MRS	(0 << DDRC_LMR_BA_BIT) /* Mode Register set */
  #define DDRC_LMR_BA_M_EMRS	(2 << DDRC_LMR_BA_BIT) /* Extended Mode Register set */
  #define DDRC_LMR_BA_M_SR	(1 << DDRC_LMR_BA_BIT) /* Status Register set */

#define DDRC_LMR_CMD_BIT	4
#define DDRC_LMR_CMD_MASK	(0x3 << DDRC_LMR_CMD_BIT)
  #define DDRC_LMR_CMD_PREC	(0 << DDRC_LMR_CMD_BIT)/* Precharge one bank/All banks */
  #define DDRC_LMR_CMD_AUREF	(1 << DDRC_LMR_CMD_BIT)/* Auto-Refresh */
  #define DDRC_LMR_CMD_LMR	(2 << DDRC_LMR_CMD_BIT)/* Load Mode Register */

#define DDRC_LMR_START		(1 << 0) /* 0 No command is performed
						    1 On the posedge of START, perform a command
						    defined by CMD field */

/* DDRC Mode Register Set */
#define DDR_MRS_PD_BIT		(1 << 10) /* Active power down exit time */
#define DDR_MRS_PD_MASK		(1 << DDR_MRS_PD_BIT)
  #define DDR_MRS_PD_FAST_EXIT	(0 << 10)
  #define DDR_MRS_PD_SLOW_EXIT	(1 << 10)
#define DDR_MRS_WR_BIT		(1 << 9) /* Write Recovery for autoprecharge */
#define DDR_MRS_WR_MASK		(7 << DDR_MRS_WR_BIT)
#define DDR_MRS_DLL_RST		(1 << 8) /* DLL Reset */
#define DDR_MRS_TM_BIT		7        /* Operating Mode */
#define DDR_MRS_TM_MASK		(1 << DDR_MRS_OM_BIT)
  #define DDR_MRS_TM_NORMAL	(0 << DDR_MRS_OM_BIT)
  #define DDR_MRS_TM_TEST	(1 << DDR_MRS_OM_BIT)
#define DDR_MRS_CAS_BIT		4        /* CAS Latency */
#define DDR_MRS_CAS_MASK	(7 << DDR_MRS_CAS_BIT)
#define DDR_MRS_BT_BIT		3        /* Burst Type */
#define DDR_MRS_BT_MASK		(1 << DDR_MRS_BT_BIT)
  #define DDR_MRS_BT_SEQ	(0 << DDR_MRS_BT_BIT) /* Sequential */
  #define DDR_MRS_BT_INT	(1 << DDR_MRS_BT_BIT) /* Interleave */
#define DDR_MRS_BL_BIT		0        /* Burst Length */
#define DDR_MRS_BL_MASK		(7 << DDR_MRS_BL_BIT)
  #define DDR_MRS_BL_4		(2 << DDR_MRS_BL_BIT)
  #define DDR_MRS_BL_8		(3 << DDR_MRS_BL_BIT)

/* DDRC Extended Mode Register1 Set */
#define DDR_EMRS1_QOFF		(1<<12) /* 0 Output buffer enabled
					   1 Output buffer disabled */
#define DDR_EMRS1_RDQS_EN	(1<<11) /* 0 Disable
					   1 Enable */
#define DDR_EMRS1_DQS_DIS	(1<<10) /* 0 Enable
					   1 Disable */
#define DDR_EMRS1_OCD_BIT	7 /* Additive Latency 0 -> 6 */
#define DDR_EMRS1_OCD_MASK	(0x7 << DDR_EMRS1_OCD_BIT)
  #define DDR_EMRS1_OCD_EXIT		(0 << DDR_EMRS1_OCD_BIT)
  #define DDR_EMRS1_OCD_D0		(1 << DDR_EMRS1_OCD_BIT)
  #define DDR_EMRS1_OCD_D1		(2 << DDR_EMRS1_OCD_BIT)
  #define DDR_EMRS1_OCD_ADJ		(4 << DDR_EMRS1_OCD_BIT)
  #define DDR_EMRS1_OCD_DFLT		(7 << DDR_EMRS1_OCD_BIT)
#define DDR_EMRS1_AL_BIT	3 /* Additive Latency 0 -> 6 */
#define DDR_EMRS1_AL_MASK	(7 << DDR_EMRS1_AL_BIT)
#define DDR_EMRS1_RTT_BIT	2 /*  */
#define DDR_EMRS1_RTT_MASK	(0x11 << DDR_EMRS1_DIC_BIT) /* Bit 6, Bit 2 */
#define DDR_EMRS1_DIC_BIT	1        /* Output Driver Impedence Control */
#define DDR_EMRS1_DIC_MASK	(1 << DDR_EMRS1_DIC_BIT) /* 100% */
  #define DDR_EMRS1_DIC_NORMAL	(0 << DDR_EMRS1_DIC_BIT) /* 60% */
  #define DDR_EMRS1_DIC_HALF	(1 << DDR_EMRS1_DIC_BIT)
#define DDR_EMRS1_DLL_BIT	0        /* DLL Enable  */
#define DDR_EMRS1_DLL_MASK	(1 << DDR_EMRS1_DLL_BIT)
  #define DDR_EMRS1_DLL_EN	(0 << DDR_EMRS1_DLL_BIT)
  #define DDR_EMRS1_DLL_DIS	(1 << DDR_EMRS1_DLL_BIT)

/* Mobile SDRAM Extended Mode Register */
#define DDR_EMRS_DS_BIT		5	/* Driver strength */
#define DDR_EMRS_DS_MASK	(7 << DDR_EMRS_DS_BIT)
  #define DDR_EMRS_DS_FULL	(0 << DDR_EMRS_DS_BIT)	/*Full*/
  #define DDR_EMRS_DS_HALF	(1 << DDR_EMRS_DS_BIT)	/*1/2 Strength*/
  #define DDR_EMRS_DS_QUTR	(2 << DDR_EMRS_DS_BIT)	/*1/4 Strength*/
  #define DDR_EMRS_DS_OCTANT	(3 << DDR_EMRS_DS_BIT)	/*1/8 Strength*/
  #define DDR_EMRS_DS_QUTR3	(4 << DDR_EMRS_DS_BIT)	/*3/4 Strength*/

#define DDR_EMRS_PRSR_BIT	0	/* Partial Array Self Refresh */
#define DDR_EMRS_PRSR_MASK	(7 << DDR_EMRS_PRSR_BIT)
  #define DDR_EMRS_PRSR_ALL	(0 << DDR_EMRS_PRSR_BIT) /*All Banks*/
  #define DDR_EMRS_PRSR_HALF_TL	(1 << DDR_EMRS_PRSR_BIT) /*Half of Total Bank*/
  #define DDR_EMRS_PRSR_QUTR_TL	(2 << DDR_EMRS_PRSR_BIT) /*Quarter of Total Bank*/
  #define DDR_EMRS_PRSR_HALF_B0	(5 << DDR_EMRS_PRSR_BIT) /*Half of Bank0*/
  #define DDR_EMRS_PRSR_QUTR_B0	(6 << DDR_EMRS_PRSR_BIT) /*Quarter of Bank0*/


/* DDRC Timing Config Register 1 */
#define DDRC_TIMING1_TRAS_BIT 	28 /* ACTIVE to PRECHARGE command period (2 * tRAS + 1) */
#define DDRC_TIMING1_TRAS_MASK 	(0xf << DDRC_TIMING1_TRAS_BIT)


#define DDRC_TIMING1_TRTP_BIT		24 /* READ to PRECHARGE command period. */
#define DDRC_TIMING1_TRTP_MASK	(0x3 << DDRC_TIMING1_TRTP_BIT)

#define DDRC_TIMING1_TRP_BIT		20 /* PRECHARGE command period. */
#define DDRC_TIMING1_TRP_MASK 	(0x7 << DDRC_TIMING1_TRP_BIT)

#define DDRC_TIMING1_TRCD_BIT		16 /* ACTIVE to READ or WRITE command period. */
#define DDRC_TIMING1_TRCD_MASK	(0x7 << DDRC_TIMING1_TRCD_BIT)

#define DDRC_TIMING1_TRC_BIT 		12 /* ACTIVE to ACTIVE command period. */
#define DDRC_TIMING1_TRC_MASK 	(0xf << DDRC_TIMING1_TRC_BIT)

#define DDRC_TIMING1_TRRD_BIT		8 /* ACTIVE bank A to ACTIVE bank B command period. */
#define DDRC_TIMING1_TRRD_MASK	(0x3 << DDRC_TIMING1_TRRD_BIT)
#define DDRC_TIMING1_TRRD_DISABLE	(0 << DDRC_TIMING1_TRRD_BIT)
#define DDRC_TIMING1_TRRD_2		(1 << DDRC_TIMING1_TRRD_BIT)
#define DDRC_TIMING1_TRRD_3		(2 << DDRC_TIMING1_TRRD_BIT)
#define DDRC_TIMING1_TRRD_4		(3 << DDRC_TIMING1_TRRD_BIT)

#define DDRC_TIMING1_TWR_BIT 		4 /* WRITE Recovery Time defined by register MR of DDR2 memory */
#define DDRC_TIMING1_TWR_MASK		(0x7 << DDRC_TIMING1_TWR_BIT)
  #define DDRC_TIMING1_TWR_1		(0 << DDRC_TIMING1_TWR_BIT)
  #define DDRC_TIMING1_TWR_2		(1 << DDRC_TIMING1_TWR_BIT)
  #define DDRC_TIMING1_TWR_3		(2 << DDRC_TIMING1_TWR_BIT)
  #define DDRC_TIMING1_TWR_4		(3 << DDRC_TIMING1_TWR_BIT)
  #define DDRC_TIMING1_TWR_5		(4 << DDRC_TIMING1_TWR_BIT)
  #define DDRC_TIMING1_TWR_6		(5 << DDRC_TIMING1_TWR_BIT)

#define DDRC_TIMING1_TWTR_BIT		0 /* WRITE to READ command delay. */
#define DDRC_TIMING1_TWTR_MASK	(0x3 << DDRC_TIMING1_TWTR_BIT)
  #define DDRC_TIMING1_TWTR_1		(0 << DDRC_TIMING1_TWTR_BIT)
  #define DDRC_TIMING1_TWTR_2		(1 << DDRC_TIMING1_TWTR_BIT)
  #define DDRC_TIMING1_TWTR_3		(2 << DDRC_TIMING1_TWTR_BIT)
  #define DDRC_TIMING1_TWTR_4		(3 << DDRC_TIMING1_TWTR_BIT)

/* DDRC Timing Config Register 2 */
#define DDRC_TIMING2_TRFC_BIT         12 /* AUTO-REFRESH command period. */
#define DDRC_TIMING2_TRFC_MASK        (0xf << DDRC_TIMING2_TRFC_BIT)
#define DDRC_TIMING2_TMINSR_BIT       8  /* Minimum Self-Refresh / Deep-Power-Down time */
#define DDRC_TIMING2_TMINSR_MASK      (0xf << DDRC_TIMING2_TMINSR_BIT)
#define DDRC_TIMING2_TXP_BIT          4  /* EXIT-POWER-DOWN to next valid command period. */
#define DDRC_TIMING2_TXP_MASK         (0x7 << DDRC_TIMING2_TXP_BIT)
#define DDRC_TIMING2_TMRD_BIT         0  /* Load-Mode-Register to next valid command period. */
#define DDRC_TIMING2_TMRD_MASK        (0x3 << DDRC_TIMING2_TMRD_BIT)

/* DDRC  Auto-Refresh Counter */
#define DDRC_REFCNT_CON_BIT           16 /* Constant value used to compare with CNT value. */
#define DDRC_REFCNT_CON_MASK          (0xff << DDRC_REFCNT_CON_BIT)
#define DDRC_REFCNT_CNT_BIT           8  /* 8-bit counter */
#define DDRC_REFCNT_CNT_MASK          (0xff << DDRC_REFCNT_CNT_BIT)
#define DDRC_REFCNT_CLKDIV_BIT        1  /* Clock Divider for auto-refresh counter. */
#define DDRC_REFCNT_CLKDIV_MASK       (0x7 << DDRC_REFCNT_CLKDIV_BIT)
#define DDRC_REFCNT_REF_EN            (1 << 0) /* Enable Refresh Counter */

/* DDRC DQS Delay Control Register */
#define DDRC_DQS_ERROR                (1 << 29) /* ahb_clk Delay Detect ERROR, read-only. */
#define DDRC_DQS_READY                (1 << 28) /* ahb_clk Delay Detect READY, read-only. */
#define DDRC_DQS_AUTO                 (1 << 23) /* Hardware auto-detect & set delay line */
#define DDRC_DQS_DET                  (1 << 24) /* Start delay detecting. */
#define DDRC_DQS_CLKD_BIT             16 /* CLKD is reference value for setting WDQS and RDQS.*/
#define DDRC_DQS_CLKD_MASK            (0x7f << DDRC_DQS_CLKD_BIT)
#define DDRC_DQS_WDQS_BIT             8  /* Set delay element number to write DQS delay-line. */
#define DDRC_DQS_WDQS_MASK            (0x3f << DDRC_DQS_WDQS_BIT)
#define DDRC_DQS_RDQS_BIT             0  /* Set delay element number to read DQS delay-line. */
#define DDRC_DQS_RDQS_MASK            (0x3f << DDRC_DQS_RDQS_BIT)

/* DDRC DQS Delay Adjust Register */
#define DDRC_DQS_ADJWDQS_BIT          8 /* The adjust value for WRITE DQS delay */
#define DDRC_DQS_ADJWDQS_MASK         (0x1f << DDRC_DQS_ADJWDQS_BIT)
#define DDRC_DQS_ADJRDQS_BIT          0 /* The adjust value for READ DQS delay */
#define DDRC_DQS_ADJRDQS_MASK         (0x1f << DDRC_DQS_ADJRDQS_BIT)

/* DDRC Memory Map Config Register */
#define DDRC_MMAP_BASE_BIT            8 /* base address */
#define DDRC_MMAP_BASE_MASK           (0xff << DDRC_MMAP_BASE_BIT)
#define DDRC_MMAP_MASK_BIT            0 /* address mask */
#define DDRC_MMAP_MASK_MASK           (0xff << DDRC_MMAP_MASK_BIT)

#define DDRC_MMAP0_BASE		     (0x20 << DDRC_MMAP_BASE_BIT)
#define DDRC_MMAP1_BASE_64M	(0x24 << DDRC_MMAP_BASE_BIT) /*when bank0 is 128M*/
#define DDRC_MMAP1_BASE_128M	(0x28 << DDRC_MMAP_BASE_BIT) /*when bank0 is 128M*/
#define DDRC_MMAP1_BASE_256M	(0x30 << DDRC_MMAP_BASE_BIT) /*when bank0 is 128M*/

#define DDRC_MMAP_MASK_64_64	(0xfc << DDRC_MMAP_MASK_BIT)  /*mask for two 128M SDRAM*/
#define DDRC_MMAP_MASK_128_128	(0xf8 << DDRC_MMAP_MASK_BIT)  /*mask for two 128M SDRAM*/
#define DDRC_MMAP_MASK_256_256	(0xf0 << DDRC_MMAP_MASK_BIT)  /*mask for two 128M SDRAM*/


#ifndef __MIPS_ASSEMBLER

#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4770DDRC_H__ */


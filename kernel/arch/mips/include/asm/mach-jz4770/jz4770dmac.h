/*
 * linux/include/asm-mips/mach-jz4770/jz4770dmac.h
 *
 * JZ4770 DMAC register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770DMAC_H__
#define __JZ4770DMAC_H__

#include <asm/addrspace.h>
#include <asm/mach-jz4770/base.h>
#include <asm/mach-jz4770/jz4770misc.h>


#define DMAC_BASE	CKSEG1ADDR(JZ4770_DMAC_BASE_ADDR)


/*************************************************************************
 * DMAC (DMA Controller)
 *************************************************************************/

#define MAX_DMA_NUM 	12  /* max 12 channels */
#define MAX_MDMA_NUM    3   /* max 3  channels */
#define MAX_BDMA_NUM	3   /* max 3  channels */
#define HALF_DMA_NUM	6   /* the number of one dma controller's channels */

/* m is the DMA controller index (0, 1), n is the DMA channel index (0 - 11) */

#define DMAC_DSAR(n)  (DMAC_BASE + ((n)/HALF_DMA_NUM*0x100 + 0x00 + ((n)-(n)/HALF_DMA_NUM*HALF_DMA_NUM) * 0x20)) /* DMA source address */
#define DMAC_DTAR(n)  (DMAC_BASE + ((n)/HALF_DMA_NUM*0x100 + 0x04 + ((n)-(n)/HALF_DMA_NUM*HALF_DMA_NUM) * 0x20)) /* DMA target address */
#define DMAC_DTCR(n)  (DMAC_BASE + ((n)/HALF_DMA_NUM*0x100 + 0x08 + ((n)-(n)/HALF_DMA_NUM*HALF_DMA_NUM) * 0x20)) /* DMA transfer count */
#define DMAC_DRSR(n)  (DMAC_BASE + ((n)/HALF_DMA_NUM*0x100 + 0x0c + ((n)-(n)/HALF_DMA_NUM*HALF_DMA_NUM) * 0x20)) /* DMA request source */
#define DMAC_DCCSR(n) (DMAC_BASE + ((n)/HALF_DMA_NUM*0x100 + 0x10 + ((n)-(n)/HALF_DMA_NUM*HALF_DMA_NUM) * 0x20)) /* DMA control/status */
#define DMAC_DCMD(n)  (DMAC_BASE + ((n)/HALF_DMA_NUM*0x100 + 0x14 + ((n)-(n)/HALF_DMA_NUM*HALF_DMA_NUM) * 0x20)) /* DMA command */
#define DMAC_DDA(n)   (DMAC_BASE + ((n)/HALF_DMA_NUM*0x100 + 0x18 + ((n)-(n)/HALF_DMA_NUM*HALF_DMA_NUM) * 0x20)) /* DMA descriptor addr */
#define DMAC_DSD(n)   (DMAC_BASE + ((n)/HALF_DMA_NUM*0x100 + 0x1c + ((n)-(n)/HALF_DMA_NUM*HALF_DMA_NUM) * 0x20)) /* DMA stride address */

#define DMAC_DMACR(m)	(DMAC_BASE + 0x0300 + 0x100 * (m))              /* DMA control register */
#define DMAC_DMAIPR(m)	(DMAC_BASE + 0x0304 + 0x100 * (m))              /* DMA interrupt pending */
#define DMAC_DMADBR(m)	(DMAC_BASE + 0x0308 + 0x100 * (m))              /* DMA doorbell */
#define DMAC_DMADBSR(m)	(DMAC_BASE + 0x030C + 0x100 * (m))              /* DMA doorbell set */
#define DMAC_DMACKE(m)	(DMAC_BASE + 0x0310 + 0x100 * (m))
#define DMAC_DMACKES(m)	(DMAC_BASE + 0x0314 + 0x100 * (m))
#define DMAC_DMACKEC(m)	(DMAC_BASE + 0x0318 + 0x100 * (m))

#define REG_DMAC_DSAR(n)	REG32(DMAC_DSAR((n)))
#define REG_DMAC_DTAR(n)	REG32(DMAC_DTAR((n)))
#define REG_DMAC_DTCR(n)	REG32(DMAC_DTCR((n)))
#define REG_DMAC_DRSR(n)	REG32(DMAC_DRSR((n)))
#define REG_DMAC_DCCSR(n)	REG32(DMAC_DCCSR((n)))
#define REG_DMAC_DCMD(n)	REG32(DMAC_DCMD((n)))
#define REG_DMAC_DDA(n)		REG32(DMAC_DDA((n)))
#define REG_DMAC_DSD(n)         REG32(DMAC_DSD(n))
#define REG_DMAC_DMACR(m)	REG32(DMAC_DMACR(m))
#define REG_DMAC_DMAIPR(m)	REG32(DMAC_DMAIPR(m))
#define REG_DMAC_DMADBR(m)	REG32(DMAC_DMADBR(m))
#define REG_DMAC_DMADBSR(m)	REG32(DMAC_DMADBSR(m))
#define REG_DMAC_DMACKE(m)	REG32(DMAC_DMACKE(m))
#define REG_DMAC_DMACKES(m)	REG32(DMAC_DMACKES(m))
#define REG_DMAC_DMACKEC(m)	REG32(DMAC_DMACKEC(m))

// DMA request source register
#define DMAC_DRSR_RS_MASK	0x3f
#define DMAC_DRSR_RS_AUTO	JZ4770_DMA_TYPE_AUTO_REQUEST
#define DMAC_DRSR_RS_TSSIIN	JZ4770_DMA_TYPE_TSSI_RECEIVE
#define DMAC_DRSR_RS_PM1OUT	JZ4770_DMA_TYPE_PCM1_TRANSMIT
#define DMAC_DRSR_RS_PM1IN	JZ4770_DMA_TYPE_PCM1_RECEIVE
#define DMAC_DRSR_RS_EXTERN	JZ4770_DMA_TYPE_EXTERN
#define DMAC_DRSR_RS_UART3OUT	JZ4770_DMA_TYPE_UART3_TRANSMIT
#define DMAC_DRSR_RS_UART3IN	JZ4770_DMA_TYPE_UART3_RECEIVE
#define DMAC_DRSR_RS_UART2OUT	JZ4770_DMA_TYPE_UART2_TRANSMIT
#define DMAC_DRSR_RS_UART2IN	JZ4770_DMA_TYPE_UART2_RECEIVE
#define DMAC_DRSR_RS_UART1OUT	JZ4770_DMA_TYPE_UART1_TRANSMIT
#define DMAC_DRSR_RS_UART1IN	JZ4770_DMA_TYPE_UART1_RECEIVE
#define DMAC_DRSR_RS_UART0OUT	JZ4770_DMA_TYPE_UART0_TRANSMIT
#define DMAC_DRSR_RS_UART0IN	JZ4770_DMA_TYPE_UART0_RECEIVE
#define DMAC_DRSR_RS_SSI0OUT	JZ4770_DMA_TYPE_SPI0_TRANSMIT
#define DMAC_DRSR_RS_SSI0IN	JZ4770_DMA_TYPE_SPI0_RECEIVE
#define DMAC_DRSR_RS_AICOUT	JZ4770_DMA_TYPE_AIC_TRANSMIT
#define DMAC_DRSR_RS_AICIN	JZ4770_DMA_TYPE_AIC_RECEIVE
#define DMAC_DRSR_RS_MSC0OUT	JZ4770_DMA_TYPE_MMC0_TRANSMIT
#define DMAC_DRSR_RS_MSC0IN	JZ4770_DMA_TYPE_MMC0_RECEIVE
#define DMAC_DRSR_RS_TCU	JZ4770_DMA_TYPE_TCU
#define DMAC_DRSR_RS_SADC	JZ4770_DMA_TYPE_SADC
#define DMAC_DRSR_RS_MSC1OUT	JZ4770_DMA_TYPE_MMC1_TRANSMIT
#define DMAC_DRSR_RS_MSC1IN	JZ4770_DMA_TYPE_MMC1_RECEIVE
#define DMAC_DRSR_RS_SSI1OUT	JZ4770_DMA_TYPE_SPI1_TRANSMIT
#define DMAC_DRSR_RS_SSI1IN	JZ4770_DMA_TYPE_SPI1_RECEIVE
#define DMAC_DRSR_RS_PM0OUT	JZ4770_DMA_TYPE_PCM0_TRANSMIT
#define DMAC_DRSR_RS_PM0IN	JZ4770_DMA_TYPE_PCM0_RECEIVE
#define DMAC_DRSR_RS_MSC2OUT	JZ4770_DMA_TYPE_MMC2_TRANSMIT
#define DMAC_DRSR_RS_MSC2IN	JZ4770_DMA_TYPE_MMC2_RECEIVE

// DMA channel control/status register
#define DMAC_DCCSR_NDES		(1 << 31) /* descriptor (0) or not (1) ? */
#define DMAC_DCCSR_DES8    	(1 << 30) /* Descriptor 8 Word */
#define DMAC_DCCSR_DES4    	(0 << 30) /* Descriptor 4 Word */
/* [29:24] reserved */
#define DMAC_DCCSR_CDOA_BIT	16        /* copy of DMA offset address */
#define DMAC_DCCSR_CDOA_MASK	(0xff << DMAC_DCCSR_CDOA_BIT)
/* [15:5] reserved */
#define DMAC_DCCSR_AR		(1 << 4)  /* address error */
#define DMAC_DCCSR_TT		(1 << 3)  /* transfer terminated */
#define DMAC_DCCSR_HLT		(1 << 2)  /* DMA halted */
#define DMAC_DCCSR_CT		(1 << 1)  /* count terminated */
#define DMAC_DCCSR_EN		(1 << 0)  /* channel enable bit */

// DMA channel command register
#define DMAC_DCMD_EACKS_LOW  	(1 << 31) /* External DACK Output Level Select, active low */
#define DMAC_DCMD_EACKS_HIGH  	(0 << 31) /* External DACK Output Level Select, active high */
#define DMAC_DCMD_EACKM_WRITE 	(1 << 30) /* External DACK Output Mode Select, output in write cycle */
#define DMAC_DCMD_EACKM_READ 	(0 << 30) /* External DACK Output Mode Select, output in read cycle */
#define DMAC_DCMD_ERDM_BIT      28        /* External DREQ Detection Mode Select */
#define DMAC_DCMD_ERDM_MASK     (0x03 << DMAC_DCMD_ERDM_BIT)
#define DMAC_DCMD_ERDM_LOW    (0 << DMAC_DCMD_ERDM_BIT)
#define DMAC_DCMD_ERDM_FALL   (1 << DMAC_DCMD_ERDM_BIT)
#define DMAC_DCMD_ERDM_HIGH   (2 << DMAC_DCMD_ERDM_BIT)
#define DMAC_DCMD_ERDM_RISE   (3 << DMAC_DCMD_ERDM_BIT)
/* [27:24] reserved */
#define DMAC_DCMD_SAI		(JZ4770_DMA_SRC_AUTOINC << 22) /* source address increment */
#define DMAC_DCMD_DAI		(JZ4770_DMA_DST_AUTOINC << 22) /* dest address increment */
#define DMAC_DCMD_RDIL_BIT	16        /* request detection interval length */
#define DMAC_DCMD_RDIL_MASK	(0x0f << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_IGN	(0 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_2	(1 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_4	(2 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_8	(3 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_12	(4 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_16	(5 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_20	(6 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_24	(7 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_28	(8 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_32	(9 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_48	(10 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_60	(11 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_64	(12 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_124	(13 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_128	(14 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_RDIL_200	(15 << DMAC_DCMD_RDIL_BIT)
#define DMAC_DCMD_SWDH_BIT	14  /* source port width */
#define DMAC_DCMD_SWDH_MASK	(0x03 << DMAC_DCMD_SWDH_BIT)
#define DMAC_DCMD_SWDH_32	(JZ4770_DMA_WIDTH_32BIT << DMAC_DCMD_SWDH_BIT)
#define DMAC_DCMD_SWDH_8	(JZ4770_DMA_WIDTH_8BIT  << DMAC_DCMD_SWDH_BIT)
#define DMAC_DCMD_SWDH_16	(JZ4770_DMA_WIDTH_16BIT << DMAC_DCMD_SWDH_BIT)
#define DMAC_DCMD_DWDH_BIT	12  /* dest port width */
#define DMAC_DCMD_DWDH_MASK	(0x03 << DMAC_DCMD_DWDH_BIT)
#define DMAC_DCMD_DWDH_32	(JZ4770_DMA_WIDTH_32BIT << DMAC_DCMD_DWDH_BIT)
#define DMAC_DCMD_DWDH_8	(JZ4770_DMA_WIDTH_8BIT  << DMAC_DCMD_DWDH_BIT)
#define DMAC_DCMD_DWDH_16	(JZ4770_DMA_WIDTH_16BIT << DMAC_DCMD_DWDH_BIT)
/* bit11 reserved */
#define DMAC_DCMD_DS_BIT	8  /* transfer data size of a data unit */
#define DMAC_DCMD_DS_MASK	(0x07 << DMAC_DCMD_DS_BIT)
#define DMAC_DCMD_DS_32BIT	(JZ4770_DMA_TRANSFER_SIZE_4BYTE  << DMAC_DCMD_DS_BIT)
#define DMAC_DCMD_DS_8BIT	(JZ4770_DMA_TRANSFER_SIZE_1BYTE  << DMAC_DCMD_DS_BIT)
#define DMAC_DCMD_DS_16BIT	(JZ4770_DMA_TRANSFER_SIZE_2BYTE  << DMAC_DCMD_DS_BIT)
#define DMAC_DCMD_DS_16BYTE	(JZ4770_DMA_TRANSFER_SIZE_16BYTE << DMAC_DCMD_DS_BIT)
#define DMAC_DCMD_DS_32BYTE	(JZ4770_DMA_TRANSFER_SIZE_32BYTE << DMAC_DCMD_DS_BIT)
#define DMAC_DCMD_DS_64BYTE	(JZ4770_DMA_TRANSFER_SIZE_64BYTE << DMAC_DCMD_DS_BIT)
/* [7:3] reserved */
#define DMAC_DCMD_STDE   	(1 << 2)  /* Stride Disable/Enable */
#define DMAC_DCMD_TIE		(1 << 1)  /* DMA transfer interrupt enable */
#define DMAC_DCMD_LINK		(1 << 0)  /* descriptor link enable */

// DMA descriptor address register
#define DMAC_DDA_BASE_BIT	12  /* descriptor base address */
#define DMAC_DDA_BASE_MASK	(0x0fffff << DMAC_DDA_BASE_BIT)
#define DMAC_DDA_OFFSET_BIT	4   /* descriptor offset address */
#define DMAC_DDA_OFFSET_MASK	(0x0ff << DMAC_DDA_OFFSET_BIT)
/* [3:0] reserved */

// DMA stride address register
#define DMAC_DSD_TSD_BIT        16  /* target stride address */
#define DMAC_DSD_TSD_MASK      	(0xffff << DMAC_DSD_TSD_BIT)
#define DMAC_DSD_SSD_BIT        0  /* source stride address */
#define DMAC_DSD_SSD_MASK      	(0xffff << DMAC_DSD_SSD_BIT)

// DMA control register
#define DMAC_DMACR_FMSC		(1 << 31)  /* MSC Fast DMA mode */
#define DMAC_DMACR_FSSI		(1 << 30)  /* SSI Fast DMA mode */
#define DMAC_DMACR_FTSSI	(1 << 29)  /* TSSI Fast DMA mode */
#define DMAC_DMACR_FUART	(1 << 28)  /* UART Fast DMA mode */
#define DMAC_DMACR_FAIC		(1 << 27)  /* AIC Fast DMA mode */
/* [26:10] reserved */
#define DMAC_DMACR_PR_BIT	8  /* channel priority mode */
#define DMAC_DMACR_PR_MASK	(0x03 << DMAC_DMACR_PR_BIT)
#define DMAC_DMACR_PR_012345	(0 << DMAC_DMACR_PR_BIT)
#define DMAC_DMACR_PR_120345	(1 << DMAC_DMACR_PR_BIT)
#define DMAC_DMACR_PR_230145	(2 << DMAC_DMACR_PR_BIT)
#define DMAC_DMACR_PR_340125	(3 << DMAC_DMACR_PR_BIT)
/* [7:4] resered */
#define DMAC_DMACR_HLT		(1 << 3)  /* DMA halt flag */
#define DMAC_DMACR_AR		(1 << 2)  /* address error flag */
/* bit1 reserved */
#define DMAC_DMACR_DMAE		(1 << 0)  /* DMA enable bit */

// DMA doorbell register
#define DMAC_DMADBR_DB5		(1 << 5)  /* doorbell for channel 5 */
#define DMAC_DMADBR_DB4		(1 << 4)  /* doorbell for channel 4 */
#define DMAC_DMADBR_DB3		(1 << 3)  /* doorbell for channel 3 */
#define DMAC_DMADBR_DB2		(1 << 2)  /* doorbell for channel 2 */
#define DMAC_DMADBR_DB1		(1 << 1)  /* doorbell for channel 1 */
#define DMAC_DMADBR_DB0		(1 << 0)  /* doorbell for channel 0 */

// DMA doorbell set register
#define DMAC_DMADBSR_DBS5	(1 << 5)  /* enable doorbell for channel 5 */
#define DMAC_DMADBSR_DBS4	(1 << 4)  /* enable doorbell for channel 4 */
#define DMAC_DMADBSR_DBS3	(1 << 3)  /* enable doorbell for channel 3 */
#define DMAC_DMADBSR_DBS2	(1 << 2)  /* enable doorbell for channel 2 */
#define DMAC_DMADBSR_DBS1	(1 << 1)  /* enable doorbell for channel 1 */
#define DMAC_DMADBSR_DBS0	(1 << 0)  /* enable doorbell for channel 0 */

// DMA interrupt pending register
#define DMAC_DMAIPR_CIRQ5	(1 << 5)  /* irq pending status for channel 5 */
#define DMAC_DMAIPR_CIRQ4	(1 << 4)  /* irq pending status for channel 4 */
#define DMAC_DMAIPR_CIRQ3	(1 << 3)  /* irq pending status for channel 3 */
#define DMAC_DMAIPR_CIRQ2	(1 << 2)  /* irq pending status for channel 2 */
#define DMAC_DMAIPR_CIRQ1	(1 << 1)  /* irq pending status for channel 1 */
#define DMAC_DMAIPR_CIRQ0	(1 << 0)  /* irq pending status for channel 0 */


#ifndef __MIPS_ASSEMBLER


/***************************************************************************
 * DMAC
 ***************************************************************************/

/* m is the DMA controller index (0, 1), n is the DMA channel index (0 - 11) */

#define __dmac_enable_module(m)						\
	( REG_DMAC_DMACR(m) |= DMAC_DMACR_DMAE | DMAC_DMACR_PR_012345 )
#define __dmac_disable_module(m)			\
	( REG_DMAC_DMACR(m) &= ~DMAC_DMACR_DMAE )

/* p=0,1,2,3 */
#define __dmac_set_priority(m,p)					\
	do {								\
		REG_DMAC_DMACR(m) &= ~DMAC_DMACR_PR_MASK;		\
		REG_DMAC_DMACR(m) |= ((p) << DMAC_DMACR_PR_BIT);	\
	} while (0)

#define __dmac_test_halt_error(m) ( REG_DMAC_DMACR(m) & DMAC_DMACR_HLT )
#define __dmac_test_addr_error(m) ( REG_DMAC_DMACR(m) & DMAC_DMACR_AR )

#define __dmac_enable_descriptor(n)			\
	( REG_DMAC_DCCSR((n)) &= ~DMAC_DCCSR_NDES )
#define __dmac_disable_descriptor(n)			\
	( REG_DMAC_DCCSR((n)) |= DMAC_DCCSR_NDES )

#define __dmac_enable_channel(n)			\
	do {						\
		REG_DMAC_DCCSR((n)) |= DMAC_DCCSR_EN;	\
	} while (0)
#define __dmac_disable_channel(n)			\
	do {						\
		REG_DMAC_DCCSR((n)) &= ~DMAC_DCCSR_EN;	\
	} while (0)
#define __dmac_channel_enabled(n)		\
	( REG_DMAC_DCCSR((n)) & DMAC_DCCSR_EN )

#define __dmac_channel_enable_irq(n)		\
	( REG_DMAC_DCMD((n)) |= DMAC_DCMD_TIE )
#define __dmac_channel_disable_irq(n)			\
	( REG_DMAC_DCMD((n)) &= ~DMAC_DCMD_TIE )

#define __dmac_channel_transmit_halt_detected(n)	\
	(  REG_DMAC_DCCSR((n)) & DMAC_DCCSR_HLT )
#define __dmac_channel_transmit_end_detected(n)		\
	(  REG_DMAC_DCCSR((n)) & DMAC_DCCSR_TT )
#define __dmac_channel_address_error_detected(n)	\
	(  REG_DMAC_DCCSR((n)) & DMAC_DCCSR_AR )
#define __dmac_channel_count_terminated_detected(n)	\
	(  REG_DMAC_DCCSR((n)) & DMAC_DCCSR_CT )
#define __dmac_channel_descriptor_invalid_detected(n)	\
	(  REG_DMAC_DCCSR((n)) & DMAC_DCCSR_INV )

#define __dmac_channel_clear_transmit_halt(n)				\
	do {								\
		/* clear both channel halt error and globle halt error */ \
		REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_HLT;			\
		REG_DMAC_DMACR(n/HALF_DMA_NUM) &= ~DMAC_DMACR_HLT;	\
	} while (0)
#define __dmac_channel_clear_transmit_end(n)		\
	(  REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_TT )
#define __dmac_channel_clear_address_error(n)				\
	do {								\
		REG_DMAC_DDA(n) = 0; /* clear descriptor address register */ \
		REG_DMAC_DSAR(n) = 0; /* clear source address register */ \
		REG_DMAC_DTAR(n) = 0; /* clear target address register */ \
		/* clear both channel addr error and globle address error */ \
		REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_AR;			\
		REG_DMAC_DMACR(n/HALF_DMA_NUM) &= ~DMAC_DMACR_AR;	\
	} while (0)
#define __dmac_channel_clear_count_terminated(n)	\
	(  REG_DMAC_DCCSR((n)) &= ~DMAC_DCCSR_CT )
#define __dmac_channel_clear_descriptor_invalid(n)	\
	(  REG_DMAC_DCCSR((n)) &= ~DMAC_DCCSR_INV )

#define __dmac_channel_set_transfer_unit_32bit(n)		\
	do {							\
		REG_DMAC_DCMD((n)) &= ~DMAC_DCMD_DS_MASK;	\
		REG_DMAC_DCMD((n)) |= DMAC_DCMD_DS_32BIT;	\
	} while (0)

#define __dmac_channel_set_transfer_unit_16bit(n)		\
	do {							\
		REG_DMAC_DCMD((n)) &= ~DMAC_DCMD_DS_MASK;	\
		REG_DMAC_DCMD((n)) |= DMAC_DCMD_DS_16BIT;	\
	} while (0)

#define __dmac_channel_set_transfer_unit_8bit(n)		\
	do {							\
		REG_DMAC_DCMD((n)) &= ~DMAC_DCMD_DS_MASK;	\
		REG_DMAC_DCMD((n)) |= DMAC_DCMD_DS_8BIT;	\
	} while (0)

#define __dmac_channel_set_transfer_unit_16byte(n)		\
	do {							\
		REG_DMAC_DCMD((n)) &= ~DMAC_DCMD_DS_MASK;	\
		REG_DMAC_DCMD((n)) |= DMAC_DCMD_DS_16BYTE;	\
	} while (0)

#define __dmac_channel_set_transfer_unit_32byte(n)		\
	do {							\
		REG_DMAC_DCMD((n)) &= ~DMAC_DCMD_DS_MASK;	\
		REG_DMAC_DCMD((n)) |= DMAC_DCMD_DS_32BYTE;	\
	} while (0)

/* w=8,16,32 */
#define __dmac_channel_set_dest_port_width(n,w)			\
	do {							\
		REG_DMAC_DCMD((n)) &= ~DMAC_DCMD_DWDH_MASK;	\
		REG_DMAC_DCMD((n)) |= DMAC_DCMD_DWDH_##w;	\
	} while (0)

/* w=8,16,32 */
#define __dmac_channel_set_src_port_width(n,w)			\
	do {							\
		REG_DMAC_DCMD((n)) &= ~DMAC_DCMD_SWDH_MASK;	\
		REG_DMAC_DCMD((n)) |= DMAC_DCMD_SWDH_##w;	\
	} while (0)

/* v=0-15 */
#define __dmac_channel_set_rdil(n,v)				\
	do {							\
	REG_DMAC_DCMD((n)) &= ~DMAC_DCMD_RDIL_MASK;		\
	REG_DMAC_DCMD((n) |= ((v) << DMAC_DCMD_RDIL_BIT);	\
		      } while (0)

#define __dmac_channel_dest_addr_fixed(n)		\
	(  REG_DMAC_DCMD((n)) &= ~DMAC_DCMD_DAI )
#define __dmac_channel_dest_addr_increment(n)		\
	(  REG_DMAC_DCMD((n)) |= DMAC_DCMD_DAI )

#define __dmac_channel_src_addr_fixed(n)		\
	(  REG_DMAC_DCMD((n)) &= ~DMAC_DCMD_SAI )
#define __dmac_channel_src_addr_increment(n)		\
	(  REG_DMAC_DCMD((n)) |= DMAC_DCMD_SAI )

#define __dmac_channel_set_doorbell(n)					\
	(  REG_DMAC_DMADBSR((n)/HALF_DMA_NUM) = (1 << ((n)-(n)/HALF_DMA_NUM*HALF_DMA_NUM)) )

#define __dmac_channel_irq_detected(n)  ( REG_DMAC_DMAIPR((n)/HALF_DMA_NUM) & (1 << ((n)-(n)/HALF_DMA_NUM*HALF_DMA_NUM)) )
#define __dmac_channel_ack_irq(n)       ( REG_DMAC_DMAIPR((n)/HALF_DMA_NUM) &= ~(1 <<((n)-(n)/HALF_DMA_NUM*HALF_DMA_NUM)) )

static __inline__ int __dmac_get_irq(void)
{
	int i;
	for (i = 0; i < MAX_DMA_NUM; i++)
		if (__dmac_channel_irq_detected(i))
			return i;
	return -1;
}



#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4770DMAC_H__ */


/*
 * linux/include/asm-mips/mach-jz4770/jz4770mdma.h
 *
 * JZ4770 MDMA register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770MDMA_H__
#define __JZ4770MDMA_H__

#include <asm/mach-jz4770/jz4770misc.h>


#define	MDMAC_BASE	0xB3420000 //0xB3030000 /* Memory Copy DMAC */

/*************************************************************************
 * MDMAC (MEM Copy DMA Controller)
 *************************************************************************/

/* m is the DMA controller index (0, 1), n is the DMA channel index (0 - 11) */

#define MDMAC_DSAR(n)		(MDMAC_BASE + (0x00 + (n) * 0x20)) /* DMA source address */
#define MDMAC_DTAR(n)  		(MDMAC_BASE + (0x04 + (n) * 0x20)) /* DMA target address */
#define MDMAC_DTCR(n)  		(MDMAC_BASE + (0x08 + (n) * 0x20)) /* DMA transfer count */
#define MDMAC_DRSR(n)  		(MDMAC_BASE + (0x0c + (n) * 0x20)) /* DMA request source */
#define MDMAC_DCCSR(n) 		(MDMAC_BASE + (0x10 + (n) * 0x20)) /* DMA control/status */
#define MDMAC_DCMD(n)  		(MDMAC_BASE + (0x14 + (n) * 0x20)) /* DMA command */
#define MDMAC_DDA(n)   		(MDMAC_BASE + (0x18 + (n) * 0x20)) /* DMA descriptor address */
#define MDMAC_DSD(n)   		(MDMAC_BASE + (0xc0 + (n) * 0x04)) /* DMA Stride Address */

#define MDMAC_DMACR		(MDMAC_BASE + 0x0300) /* DMA control register */
#define MDMAC_DMAIPR		(MDMAC_BASE + 0x0304) /* DMA interrupt pending */
#define MDMAC_DMADBR		(MDMAC_BASE + 0x0308) /* DMA doorbell */
#define MDMAC_DMADBSR		(MDMAC_BASE + 0x030C) /* DMA doorbell set */
#define MDMAC_DMACKE  		(MDMAC_BASE + 0x0310)

#define REG_MDMAC_DSAR(n)	REG32(MDMAC_DSAR((n)))
#define REG_MDMAC_DTAR(n)	REG32(MDMAC_DTAR((n)))
#define REG_MDMAC_DTCR(n)	REG32(MDMAC_DTCR((n)))
#define REG_MDMAC_DRSR(n)	REG32(MDMAC_DRSR((n)))
#define REG_MDMAC_DCCSR(n)	REG32(MDMAC_DCCSR((n)))
#define REG_MDMAC_DCMD(n)	REG32(MDMAC_DCMD((n)))
#define REG_MDMAC_DDA(n)	REG32(MDMAC_DDA((n)))
#define REG_MDMAC_DSD(n)        REG32(MDMAC_DSD(n))
#define REG_MDMAC_DMACR		REG32(MDMAC_DMACR)
#define REG_MDMAC_DMAIPR	REG32(MDMAC_DMAIPR)
#define REG_MDMAC_DMADBR	REG32(MDMAC_DMADBR)
#define REG_MDMAC_DMADBSR	REG32(MDMAC_DMADBSR)
#define REG_MDMAC_DMACKE     	REG32(MDMAC_DMACKE)

#ifndef __MIPS_ASSEMBLER

/***************************************************************************
 * Mem Copy DMAC
 ***************************************************************************/

/* n is the DMA channel index (0 - 11) */

#define __mdmac_enable_module \
	( REG_MDMAC_DMACR |= DMAC_MDMACR_DMAE | DMAC_MDMACR_PR_012345 )
#define __mdmac_disable_module \
	( REG_MDMAC_DMACR &= ~DMAC_MDMACR_DMAE )

/* p=0,1,2,3 */
#define __mdmac_set_priority(p)			\
do {							\
	REG_MDMAC_DMACR &= ~DMAC_DMACR_PR_MASK;	\
	REG_MDMAC_DMACR |= ((p) << DMAC_DMACR_PR_BIT);	\
} while (0)

#define __mdmac_test_halt_error ( REG_MDMAC_DMACR & DMAC_MDMACR_HLT )
#define __mdmac_test_addr_error ( REG_MDMAC_DMACR & DMAC_MDMACR_AR )

#define __mdmac_channel_enable_clk \
	REG_MDMAC_DMACKE |= 1 << (n);

#define __mdmac_enable_descriptor(n) \
  ( REG_MDMAC_DCCSR((n)) &= ~DMAC_DCCSR_NDES )
#define __mdmac_disable_descriptor(n) \
  ( REG_MDMAC_DCCSR((n)) |= DMAC_DCCSR_NDES )

#define __mdmac_enable_channel(n)                 \
do {                                             \
	REG_MDMAC_DCCSR((n)) |= DMAC_DCCSR_EN;    \
} while (0)
#define __mdmac_disable_channel(n)                \
do {                                             \
	REG_MDMAC_DCCSR((n)) &= ~DMAC_DCCSR_EN;   \
} while (0)
#define __mdmac_channel_enabled(n) \
  ( REG_MDMAC_DCCSR((n)) & DMAC_DCCSR_EN )

#define __mdmac_channel_enable_irq(n) \
  ( REG_MDMAC_DCMD((n)) |= DMAC_DCMD_TIE )
#define __mdmac_channel_disable_irq(n) \
  ( REG_DMAC_DCMD((n)) &= ~DMAC_DCMD_TIE )

#define __mdmac_channel_transmit_halt_detected(n) \
  (  REG_MDMAC_DCCSR((n)) & DMAC_DCCSR_HLT )
#define __mdmac_channel_transmit_end_detected(n) \
  (  REG_MDMAC_DCCSR((n)) & DMAC_DCCSR_TT )
#define __mdmac_channel_address_error_detected(n) \
  (  REG_DMAC_DCCSR((n)) & DMAC_DCCSR_AR )
#define __mdmac_channel_count_terminated_detected(n) \
  (  REG_MDMAC_DCCSR((n)) & DMAC_DCCSR_CT )
#define __mdmac_channel_descriptor_invalid_detected(n) \
  (  REG_MDMAC_DCCSR((n)) & DMAC_DCCSR_INV )

#define __mdmac_channel_clear_transmit_halt(n)				\
	do {								\
		/* clear both channel halt error and globle halt error */ \
		REG_MDMAC_DCCSR(n) &= ~DMAC_DCCSR_HLT;			\
		REG_MDMAC_DMACR &= ~DMAC_DMACR_HLT;	\
	} while (0)
#define __mdmac_channel_clear_transmit_end(n) \
  (  REG_MDMAC_DCCSR(n) &= ~DMAC_DCCSR_TT )
#define __mdmac_channel_clear_address_error(n)				\
	do {								\
		REG_MDMAC_DDA(n) = 0; /* clear descriptor address register */ \
		REG_MDMAC_DSAR(n) = 0; /* clear source address register */ \
		REG_MDMAC_DTAR(n) = 0; /* clear target address register */ \
		/* clear both channel addr error and globle address error */ \
		REG_MDMAC_DCCSR(n) &= ~DMAC_DCCSR_AR;			\
		REG_MDMAC_DMACR &= ~DMAC_DMACR_AR;	\
	} while (0)
#define __mdmac_channel_clear_count_terminated(n) \
  (  REG_MDMAC_DCCSR((n)) &= ~DMAC_DCCSR_CT )
#define __mdmac_channel_clear_descriptor_invalid(n) \
  (  REG_MDMAC_DCCSR((n)) &= ~DMAC_DCCSR_INV )

#define __mdmac_channel_set_transfer_unit_32bit(n)	\
do {							\
	REG_MDMAC_DCMD((n)) &= ~DMAC_DCMD_DS_MASK;	\
	REG_MDMAC_DCMD((n)) |= DMAC_DCMD_DS_32BIT;	\
} while (0)

#define __mdmac_channel_set_transfer_unit_16bit(n)	\
do {							\
	REG_MDMAC_DCMD((n)) &= ~DMAC_DCMD_DS_MASK;	\
	REG_MDMAC_DCMD((n)) |= DMAC_DCMD_DS_16BIT;	\
} while (0)

#define __mdmac_channel_set_transfer_unit_8bit(n)	\
do {							\
	REG_MDMAC_DCMD((n)) &= ~DMAC_DCMD_DS_MASK;	\
	REG_MDMAC_DCMD((n)) |= DMAC_DCMD_DS_8BIT;	\
} while (0)

#define __mdmac_channel_set_transfer_unit_16byte(n)	\
do {							\
	REG_MDMAC_DCMD((n)) &= ~DMAC_DCMD_DS_MASK;	\
	REG_MDMAC_DCMD((n)) |= DMAC_DCMD_DS_16BYTE;	\
} while (0)

#define __mdmac_channel_set_transfer_unit_32byte(n)	\
do {							\
	REG_MDMAC_DCMD((n)) &= ~DMAC_DCMD_DS_MASK;	\
	REG_MDMAC_DCMD((n)) |= DMAC_DCMD_DS_32BYTE;	\
} while (0)

/* w=8,16,32 */
#define __mdmac_channel_set_dest_port_width(n,w)		\
do {							\
	REG_MDMAC_DCMD((n)) &= ~DMAC_DCMD_DWDH_MASK;	\
	REG_MDMAC_DCMD((n)) |= DMAC_DCMD_DWDH_##w;	\
} while (0)

/* w=8,16,32 */
#define __mdmac_channel_set_src_port_width(n,w)		\
do {							\
	REG_MDMAC_DCMD((n)) &= ~DMAC_DCMD_SWDH_MASK;	\
	REG_MDMAC_DCMD((n)) |= DMAC_DCMD_SWDH_##w;	\
} while (0)

/* v=0-15 */
#define __mdmac_channel_set_rdil(n,v)				\
do {								\
	REG_MDMAC_DCMD((n)) &= ~DMAC_DCMD_RDIL_MASK;		\
	REG_MDMAC_DCMD((n) |= ((v) << DMAC_DCMD_RDIL_BIT);	\
} while (0)

#define __mdmac_channel_dest_addr_fixed(n) \
	(REG_MDMAC_DCMD((n)) &= ~DMAC_DCMD_DAI)
#define __mdmac_channel_dest_addr_increment(n) \
	(REG_MDMAC_DCMD((n)) |= DMAC_DCMD_DAI)

#define __mdmac_channel_src_addr_fixed(n) \
	(REG_MDMAC_DCMD((n)) &= ~DMAC_DCMD_SAI)
#define __mdmac_channel_src_addr_increment(n) \
	(REG_MDMAC_DCMD((n)) |= DMAC_DCMD_SAI)

#define __mdmac_channel_set_doorbell(n)	\
	(REG_MDMAC_DMADBSR = (1 << (n)))

#define __mdmac_channel_irq_detected(n)  (REG_MDMAC_DMAIPR & (1 << (n)))
#define __mdmac_channel_ack_irq(n)       (REG_MDMAC_DMAIPR &= ~(1 <<(n)))

static __inline__ int __mdmac_get_irq(void)
{
	int i;
	for (i = 0; i < MAX_MDMA_NUM; i++)
		if (__mdmac_channel_irq_detected(i))
			return i;
	return -1;
}

#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4770MDMA_H__ */


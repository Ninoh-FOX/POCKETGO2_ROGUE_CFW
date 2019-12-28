/*
 * chip-pcm.h
 * JZ4770 PCM register definition
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 *
 * Author: whxu@ingenic.cn
 */

#ifndef __CHIP_PCM_H__
#define __CHIP_PCM_H__

#include <asm/mach-jz4770/jz4770misc.h>


/*
 * Pulse-code modulation module(PCM) address definition
 */
#define	PCM_BASE        0xb0071000
#define PCM0_BASE	0xb0071000
#define PCM1_BASE	0xb0074000


/*
 * PCM registers offset address definition
 */
#define PCM_PCTL_OFFSET		(0x00)	/* rw, 32, 0x00000000 */
#define PCM_PCFG_OFFSET		(0x04)  /* rw, 32, 0x00000110 */
#define PCM_PDP_OFFSET		(0x08)  /* rw, 32, 0x00000000 */
#define PCM_PINTC_OFFSET	(0x0c)  /* rw, 32, 0x00000000 */
#define PCM_PINTS_OFFSET	(0x10)  /* rw, 32, 0x00000100 */
#define PCM_PDIV_OFFSET		(0x14)  /* rw, 32, 0x00000001 */


/*
 * PCM registers address definition
 */
#define PCM_PCTL(n)	(PCM_BASE + 0x3000 * (n) + PCM_PCTL_OFFSET)
#define PCM_PCFG(n)	(PCM_BASE + 0x3000 * (n) + PCM_PCFG_OFFSET)
#define PCM_PDP(n)	(PCM_BASE + 0x3000 * (n) + PCM_PDP_OFFSET)
#define PCM_PINTC(n)	(PCM_BASE + 0x3000 * (n) + PCM_PINTC_OFFSET)
#define PCM_PINTS(n)	(PCM_BASE + 0x3000 * (n) + PCM_PINTS_OFFSET)
#define PCM_PDIV(n)	(PCM_BASE + 0x3000 * (n) + PCM_PDIV_OFFSET)


/*
 * CPM registers common define
 */

/* PCM controller control register (PCTL) */
#define PCTL_ERDMA	BIT9
#define PCTL_ETDMA	BIT8
#define PCTL_LSMP	BIT7
#define PCTL_ERPL	BIT6
#define PCTL_EREC	BIT5
#define PCTL_FLUSH	BIT4
#define PCTL_RST	BIT3
#define PCTL_CLKEN	BIT1
#define PCTL_PCMEN	BIT0

/* PCM controller configure register (PCFG) */
#define PCFG_SLOT_LSB		13
#define PCFG_SLOT_MASK		BITS_H2L(14, PCFG_SLOT_LSB)
#define PCFG_SLOT(n)		((n) << PCFG_SLOT_LSB)

#define PCFG_ISS_16BIT		BIT12
#define PCFG_OSS_16BIT		BIT11
#define PCFG_IMSBPOS		BIT10
#define PCFG_OMSBPOS		BIT9

#define	PCFG_RFTH_LSB		5
#define	PCFG_RFTH_MASK		BITS_H2L(8, PCFG_RFTH_LSB)

#define	PCFG_TFTH_LSB		1
#define	PCFG_TFTH_MASK		BITS_H2L(4, PCFG_TFTH_LSB)

#define PCFG_MODE_SLAVE		BIT0

/* PCM controller interrupt control register(PINTC) */
#define PINTC_ETFS	BIT3
#define PINTC_ETUR	BIT2
#define PINTC_ERFS	BIT1
#define PINTC_EROR	BIT0

/* PCM controller interrupt status register(PINTS) */
#define PINTS_RSTS	BIT14

#define PINTS_TFL_LSB		9
#define PINTS_TFL_MASK		BITS_H2L(13, PINTS_TFL_LSB)

#define PINTS_TFS	BIT8
#define PINTS_TUR	BIT7

#define PINTS_RFL_LSB		2
#define PINTS_RFL_MASK		BITS_H2L(6, PINTS_RFL_LSB)

#define PINTS_RFS	BIT1
#define PINTS_ROR	BIT0

/* PCM controller clock division register(PDIV) */
#define PDIV_SYNL_LSB		11
#define PDIV_SYNL_MASK		BITS_H2L(16, PDIV_SYNL_LSB)

#define PDIV_SYNDIV_LSB		6
#define PDIV_SYNDIV_MASK	BITS_H2L(10, PDIV_SYNDIV_LSB)

#define PDIV_CLKDIV_LSB		0
#define PDIV_CLKDIV_MASK	BITS_H2L(5, PDIV_CLKDIV_LSB)


#ifndef __MIPS_ASSEMBLER


#define REG_PCM_PCTL(n)		REG32(PCM_PCTL(n))
#define REG_PCM_PCFG(n)		REG32(PCM_PCFG(n))
#define REG_PCM_PDP(n)		REG32(PCM_PDP(n))
#define REG_PCM_PINTC(n)	REG32(PCM_PINTC(n))
#define REG_PCM_PINTS(n)	REG32(PCM_PINTS(n))
#define REG_PCM_PDIV(n)		REG32(PCM_PDIV(n))

#define __pcm_enable(n)          (REG_PCM_PCTL(n) |= PCTL_PCMEN)
#define __pcm_disable(n)         (REG_PCM_PCTL(n) &= ~PCTL_PCMEN)

#define __pcm_clk_enable(n)      (REG_PCM_PCTL(n) |= PCTL_CLKEN)
#define __pcm_clk_disable(n)     (REG_PCM_PCTL(n) &= ~PCTL_CLKEN)

#define __pcm_reset(n)           (REG_PCM_PCTL(n) |= PCTL_RST)
#define __pcm_flush_fifo(n)	(REG_PCM_PCTL(n) |= PCTL_FLUSH)

#define __pcm_enable_record(n)		(REG_PCM_PCTL(n) |= PCTL_EREC)
#define __pcm_disable_record(n)		(REG_PCM_PCTL(n) &= ~PCTL_EREC)
#define __pcm_enable_playback(n)		(REG_PCM_PCTL(n) |= PCTL_ERPL)
#define __pcm_disable_playback(n)	(REG_PCM_PCTL(n) &= ~PCTL_ERPL)

#define __pcm_enable_rxfifo(n)           __pcm_enable_record(n)
#define __pcm_disable_rxfifo(n)          __pcm_disable_record(n)
#define __pcm_enable_txfifo(n)           __pcm_enable_playback(n)
#define __pcm_disable_txfifo(n)          __pcm_disable_playback(n)

#define __pcm_last_sample(n)     (REG_PCM_PCTL(n) |= PCTL_LSMP)
#define __pcm_zero_sample(n)     (REG_PCM_PCTL(n) &= ~PCTL_LSMP)

#define __pcm_enable_transmit_dma(n)    (REG_PCM_PCTL(n) |= PCTL_ETDMA)
#define __pcm_disable_transmit_dma(n)   (REG_PCM_PCTL(n) &= ~PCTL_ETDMA)
#define __pcm_enable_receive_dma(n)     (REG_PCM_PCTL(n) |= PCTL_ERDMA)
#define __pcm_disable_receive_dma(n)    (REG_PCM_PCTL(n) &= ~PCTL_ERDMA)

#define __pcm_as_master(n)     (REG_PCM_PCFG(n) &= ~PCFG_MODE_SLAVE)
#define __pcm_as_slave(n)      (REG_PCM_PCFG(n) |= PCFG_MODE_SLAVE)

#define __pcm_set_transmit_trigger(idx, n)		\
	do {						\
		REG_PCM_PCFG(idx) &= ~PCFG_TFTH_MASK;	\
		REG_PCM_PCFG(idx) |= ((n) << PCFG_TFTH_LSB);	\
	} while(0)

#define __pcm_set_receive_trigger(idx, n)		\
	do {						\
		REG_PCM_PCFG(idx) &= ~PCFG_RFTH_MASK;	\
		REG_PCM_PCFG(idx) |= ((n) << PCFG_RFTH_LSB);	\
	} while(0)

#define __pcm_omsb_same_sync(n)   (REG_PCM_PCFG(n) &= ~PCFG_OMSBPOS)
#define __pcm_omsb_next_sync(n)   (REG_PCM_PCFG(n) |= PCFG_OMSBPOS)

#define __pcm_imsb_same_sync(n)   (REG_PCM_PCFG(n) &= ~PCFG_IMSBPOS)
#define __pcm_imsb_next_sync(n)   (REG_PCM_PCFG(n) |= PCFG_IMSBPOS)

#define __pcm_set_iss(idx,n)					\
	do {							\
		if (n == 16)					\
			REG_PCM_PCFG(idx) |= PCFG_ISS_16BIT;	\
		else						\
			REG_PCM_PCFG(idx) &= ~PCFG_ISS_16BIT;	\
	} while(0)


#define __pcm_set_oss(idx, n)					\
	do {							\
		if (n == 16)					\
			REG_PCM_PCFG(idx) |= PCFG_OSS_16BIT;	\
		else						\
			REG_PCM_PCFG(idx) &= ~PCFG_OSS_16BIT;	\
	} while(0)

#define __pcm_set_valid_slot(idx, n)					\
	do {								\
		REG_PCM_PCFG((idx)) = (REG_PCM_PCFG((idx)) & ~PCFG_SLOT_MASK) | ((n) << PCFG_SLOT_LSB); \
	} while(0)							\

#define __pcm_write_data(idx, v)	(REG_PCM_PDP(idx) = (v))
#define __pcm_read_data(idx)	(REG_PCM_PDP(idx))

#define __pcm_enable_tfs_intr(n)	(REG_PCM_PINTC(n) |= PINTC_ETFS)
#define __pcm_disable_tfs_intr(n)	(REG_PCM_PINTC(n) &= ~PINTC_ETFS)

#define __pcm_enable_tur_intr(n)	(REG_PCM_PINTC(n) |= PINTC_ETUR)
#define __pcm_disable_tur_intr(n)	(REG_PCM_PINTC(n) &= ~PINTC_ETUR)

#define __pcm_enable_rfs_intr(n)	(REG_PCM_PINTC(n) |= PINTC_ERFS)
#define __pcm_disable_rfs_intr(n)	(REG_PCM_PINTC(n) &= ~PINTC_ERFS)

#define __pcm_enable_ror_intr(n)	(REG_PCM_PINTC(n) |= PINTC_EROR)
#define __pcm_disable_ror_intr(n)	(REG_PCM_PINTC(n) &= ~PINTC_EROR)

#define __pcm_ints_valid_tx(n)	(((REG_PCM_PINTS(n) & PINTS_TFL_MASK) >> PINTS_TFL_LSB))
#define __pcm_ints_valid_rx(n)	(((REG_PCM_PINTS(n) & PINTS_RFL_MASK) >> PINTS_RFL_LSB))

#define __pcm_set_clk_div(idx, n)					\
	(REG_PCM_PDIV(idx) = (REG_PCM_PDIV(idx) & ~PDIV_CLKDIV_MASK) | ((n) << PDIV_CLKDIV_LSB))

#define __pcm_set_clk_rate(idx, sysclk, pcmclk)		\
	__pcm_set_clk_div(idx, ((sysclk) / (pcmclk) - 1))

#define __pcm_set_sync_div(idx, n)					\
	do {								\
		REG_PCM_PDIV(idx) = (REG_PCM_PDIV(idx) & ~PDIV_SYNDIV_MASK) | ((n) << PDIV_SYNDIV_LSB);	\
	} while(0)

#define __pcm_set_sync_rate(idx, pcmclk, sync)			\
	__pcm_set_sync_div(idx, ((pcmclk) / (8 * (sync)) - 1))

#define __pcm_set_sync_len(idx, n)					\
	do {								\
		REG_PCM_PDIV(idx) = (REG_PCM_PDIV(idx) & (~PDIV_SYNL_MASK)) | ((n) << PDIV_SYNL_LSB); \
	} while(0)

#endif /* __MIPS_ASSEMBLER */

#endif /* __CHIP_PCM_H__ */

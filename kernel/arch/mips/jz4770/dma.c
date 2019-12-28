/*
 * linux/arch/mips/jz4770/dma.c
 *
 * Support functions for the JZ4770 internal DMA channels.
 * No-descriptor transfer only.
 * Descriptor transfer should also call jz_request_dma() to get a free
 * channel and call jz_free_dma() to free the channel. And driver should
 * build the DMA descriptor and setup the DMA channel by itself.
 *
 * Copyright (C) 2006 - 2008 Ingenic Semiconductor Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/soundcard.h>
#include <linux/clk.h>

#include <asm/addrspace.h>

#include <asm/mach-jz4770/dma.h>
#include <asm/mach-jz4770/jz4770aic.h>
#include <asm/mach-jz4770/jz4770dmac.h>
#include <asm/mach-jz4770/jz4770intc.h>
#include <asm/mach-jz4770/jz4770pcm.h>
#include <asm/mach-jz4770/jz4770uart.h>


/*
 * A note on resource allocation:
 *
 * All drivers needing DMA channels, should allocate and release them
 * through the public routines `jz_request_dma()' and `jz_free_dma()'.
 *
 * In order to avoid problems, all processes should allocate resources in
 * the same sequence and release them in the reverse order.
 *
 * So, when allocating DMAs and IRQs, first allocate the DMA, then the IRQ.
 * When releasing them, first release the IRQ, then release the DMA. The
 * main reason for this order is that, if you are requesting the DMA buffer
 * done interrupt, you won't know the irq number until the DMA channel is
 * returned from jz_request_dma().
 */

struct jz_dma_chan {
	int dev_id;	/* DMA ID: this channel is allocated if >=0, free otherwise */
	unsigned int io;        /* DMA channel number */
	const char *dev_str;    /* string describes the DMA channel */
	int irq;                /* DMA irq number */
	void *irq_dev;          /* DMA private device structure */
	unsigned int fifo_addr; /* physical fifo address of the requested device */
	unsigned int cntl;	/* DMA controll */
	unsigned int mode;      /* DMA configuration */
	unsigned int source;    /* DMA request source */
};

static struct jz_dma_chan jz_dma_table[MAX_DMA_NUM] = {
	{ dev_id: DMA_ID_MSC0, },	/* DMAC0 channel 0, reserved for MSC0 */
	{ dev_id: -1, },		/* DMAC0 channel 1 */
	{ dev_id: -1, },		/* DMAC0 channel 2 */
	{ dev_id: -1, },		/* DMAC0 channel 3 */
	{ dev_id: -1, },		/* DMAC0 channel 4 */
	{ dev_id: -1, },			/* DMAC0 channel 5 --- unavailable */

	/* To avoid bug, reserved channel 6 & 7 for AIC_TX & AIC_RX */
	{ dev_id: DMA_ID_AIC_TX, },	/* DMAC1 channel 0 */
	{ dev_id: DMA_ID_AIC_RX, },	/* DMAC1 channel 1 */
	{ dev_id: DMA_ID_MSC1, },	/* DMAC1 channel 2, reserved for MSC1 */
	{ dev_id: -1, },		/* DMAC1 channel 3 */
	{ dev_id: -1, },		/* DMAC0 channel 4 */
	{ dev_id: -1, },			/* DMAC0 channel 5 --- unavailable */
};

#define DMA_8BIT_RX_CMD					\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_8 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_8BIT | DMAC_DCMD_RDIL_IGN

#define DMA_8BIT_TX_CMD					\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_8 |		\
	DMAC_DCMD_DS_8BIT | DMAC_DCMD_RDIL_IGN

#define DMA_16BIT_RX_CMD				\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_16BIT | DMAC_DCMD_RDIL_IGN

#define DMA_16BIT_TX_CMD				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_16 |		\
	DMAC_DCMD_DS_16BIT | DMAC_DCMD_RDIL_IGN

#define DMA_32BIT_RX_CMD				\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_32BIT | DMAC_DCMD_RDIL_IGN

#define DMA_32BIT_TX_CMD				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_32BIT | DMAC_DCMD_RDIL_IGN

#define DMA_16BYTE_RX_CMD				\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_8 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_16BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_16BYTE_TX_CMD				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_8 |		\
	DMAC_DCMD_DS_16BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_32BYTE_RX_CMD				\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_8 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_32BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_32BYTE_TX_CMD				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_8 |		\
	DMAC_DCMD_DS_32BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_32_32BYTE_TX_CMD		       	\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_32BYTE | DMAC_DCMD_RDIL_IGN
#define DMA_AIC_32_16BYTE_TX_CMD		       	\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_16BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_32_16BYTE_RX_CMD			\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_16BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_16BIT_TX_CMD				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_16 |		\
	DMAC_DCMD_DS_16BIT | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_16BIT_RX_CMD				\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_16 |		\
	DMAC_DCMD_DS_16BIT | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_16BYTE_RX_CMD				\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_16 |		\
	DMAC_DCMD_DS_16BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_16BYTE_TX_CMD				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_16 |		\
	DMAC_DCMD_DS_16BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_16BYTE_TX_CMD_UC			\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_16 |		\
	DMAC_DCMD_DS_16BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_TX_CMD_UNPACK				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_16 |		\
	DMAC_DCMD_DS_32BIT | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_TX_CMD_PACK				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_32BIT | DMAC_DCMD_RDIL_IGN

// Device FIFO addresses and default DMA modes
static const struct {
	unsigned int fifo_addr;
	unsigned int dma_mode;
	unsigned int dma_source;
} dma_dev_table[DMA_ID_MAX] = {
	[DMA_ID_AUTO] = {0, DMA_AUTOINIT, DMAC_DRSR_RS_AUTO},
//	{CPHYSADDR(TSSI_FIFO), DMA_32BIT_RX_CMD | DMA_MODE_READ, DMAC_DRSR_RS_TSSIIN},
	[DMA_ID_UART3_TX] = {CPHYSADDR(UART3_TDR), DMA_8BIT_TX_CMD | DMA_MODE_WRITE, DMAC_DRSR_RS_UART3OUT},
	[DMA_ID_UART3_RX] = {CPHYSADDR(UART3_RDR), DMA_8BIT_RX_CMD | DMA_MODE_READ, DMAC_DRSR_RS_UART3IN},
	[DMA_ID_UART2_TX] = {CPHYSADDR(UART2_TDR), DMA_8BIT_TX_CMD | DMA_MODE_WRITE, DMAC_DRSR_RS_UART2OUT},
	[DMA_ID_UART2_RX] = {CPHYSADDR(UART2_RDR), DMA_8BIT_RX_CMD | DMA_MODE_READ, DMAC_DRSR_RS_UART2IN},
	[DMA_ID_UART1_TX] = {CPHYSADDR(UART1_TDR), DMA_8BIT_TX_CMD | DMA_MODE_WRITE, DMAC_DRSR_RS_UART1OUT},
	[DMA_ID_UART1_RX] = {CPHYSADDR(UART1_RDR), DMA_8BIT_RX_CMD | DMA_MODE_READ, DMAC_DRSR_RS_UART1IN},
	[DMA_ID_UART0_TX] = {CPHYSADDR(UART0_TDR), DMA_8BIT_TX_CMD | DMA_MODE_WRITE, DMAC_DRSR_RS_UART0OUT},
	[DMA_ID_UART0_RX] = {CPHYSADDR(UART0_RDR), DMA_8BIT_RX_CMD | DMA_MODE_READ, DMAC_DRSR_RS_UART0IN},
	//[DMA_ID_SSI0_TX] = {CPHYSADDR(SSI_DR(0)), DMA_32BIT_TX_CMD | DMA_MODE_WRITE, DMAC_DRSR_RS_SSI0OUT},
	//[DMA_ID_SSI0_RX] = {CPHYSADDR(SSI_DR(0)), DMA_32BIT_RX_CMD | DMA_MODE_READ, DMAC_DRSR_RS_SSI0IN},
	[DMA_ID_AIC_TX] = {CPHYSADDR(AIC_DR), DMA_AIC_TX_CMD_UNPACK | DMA_MODE_WRITE, DMAC_DRSR_RS_AICOUT},
	[DMA_ID_AIC_RX] = {CPHYSADDR(AIC_DR), DMA_32BIT_RX_CMD | DMA_MODE_READ, DMAC_DRSR_RS_AICIN},
	[DMA_ID_MSC0] = {0, 0, 0},
	[DMA_ID_TCU_OVERFLOW] = {0, DMA_AUTOINIT, DMAC_DRSR_RS_TCU},
	//[DMA_ID_SADC] = {CPHYSADDR(SADC_ADTCH), DMA_32BIT_RX_CMD | DMA_MODE_READ, DMAC_DRSR_RS_SADC},/* Touch Screen Data Register */
	[DMA_ID_SADC] = { 0, 0, 0 },
	[DMA_ID_MSC1] = {0, 0, 0},
	[DMA_ID_MSC2] = {0, 0, 0},
	//[DMA_ID_SSI1_TX] = {CPHYSADDR(SSI_DR(1)), DMA_32BIT_TX_CMD | DMA_MODE_WRITE, DMAC_DRSR_RS_SSI1OUT},
	//[DMA_ID_SSI1_RX] = {CPHYSADDR(SSI_DR(1)), DMA_32BIT_RX_CMD | DMA_MODE_READ, DMAC_DRSR_RS_SSI1IN},
	[DMA_ID_PCM0_TX] = {CPHYSADDR(PCM_PDP(0)), DMA_16BIT_TX_CMD | DMA_MODE_WRITE, DMAC_DRSR_RS_PM0OUT},
	[DMA_ID_PCM0_RX] = {CPHYSADDR(PCM_PDP(0)), DMA_16BIT_RX_CMD | DMA_MODE_READ, DMAC_DRSR_RS_PM0IN},
	[DMA_ID_PCM1_TX] = {CPHYSADDR(PCM_PDP(1)), DMA_16BIT_TX_CMD | DMA_MODE_WRITE, DMAC_DRSR_RS_PM1OUT},
	[DMA_ID_PCM1_RX] = {CPHYSADDR(PCM_PDP(1)), DMA_16BIT_RX_CMD | DMA_MODE_READ, DMAC_DRSR_RS_PM1IN},
	[DMA_ID_I2C0_RX] = { 0, 0, 0 },
	[DMA_ID_I2C1_RX] = { 0, 0, 0 },
	[DMA_ID_I2C2_RX] = { 0, 0, 0 },
	[DMA_ID_I2C0_TX] = { 0, 0, 0 },
	[DMA_ID_I2C1_TX] = { 0, 0, 0 },
	[DMA_ID_I2C2_TX] = { 0, 0, 0 },
};

static struct jz_dma_chan *get_dma_chan(unsigned int dmanr)
{
	if (dmanr > MAX_DMA_NUM
	    || jz_dma_table[dmanr].dev_id < 0)
		return NULL;
	return &jz_dma_table[dmanr];
}


static struct clk *dma_clk = NULL;

/**
 * jz_request_dma - dynamically allcate an idle DMA channel to return
 * @dev_id: the specified dma device id or DMA_ID_RAW_SET
 * @dev_str: the specified dma device string name
 * @irqhandler: the irq handler, or NULL
 * @irqflags: the irq handler flags
 * @irq_dev_id: the irq handler device id for shared irq
 *
 * Finds a free channel, and binds the requested device to it.
 * Returns the allocated channel number, or negative on error.
 * Requests the DMA done IRQ if irqhandler != NULL.
 *
*/
/*int jz_request_dma(int dev_id, const char *dev_str,
		   void (*irqhandler)(int, void *, struct pt_regs *),
		   unsigned long irqflags,
		   void *irq_dev_id)
*/

int jz_request_dma(int dev_id, const char *dev_str,
		   irqreturn_t (*irqhandler)(int, void *),
		   unsigned long irqflags,
		   void *irq_dev_id)
{
	struct jz_dma_chan *chan;
	int i, ret;

	if (!dma_clk) {
		struct clk *clk = clk_get(NULL, "dma");
		if (IS_ERR(clk)) {
			int ret = PTR_ERR(clk);
			printk("Failed to get DMA clock: %d\n", ret);
			return ret;
		}
		dma_clk = clk;
		clk_enable(clk);
		/*
		 * Note: We don't bother to disable the clock again when the
		 *       DMAC is idle, since this code is going to be replaced
		 *       by the dmaengine driver.
		 */
	}

	if (dev_id < 0 || dev_id >= DMA_ID_MAX)
		return -EINVAL;

	for (i = 0; i < MAX_DMA_NUM; i++) {
		    if (jz_dma_table[i].dev_id == dev_id)
			    break;
	}

	if (i == MAX_DMA_NUM) {
		for (i = 0; i < MAX_DMA_NUM; i++) {
			if (jz_dma_table[i].dev_id < 0)
				break;
		}
	}
	if (i == MAX_DMA_NUM)  /* no free channel */
		return -ENODEV;

	/* we got a free channel */
	chan = &jz_dma_table[i];

	if (irqhandler) {
		chan->irq = IRQ_DMA_0 + i;	// allocate irq number
		chan->irq_dev = irq_dev_id;
		if ((ret = request_irq(chan->irq, irqhandler, irqflags,
				       dev_str, chan->irq_dev))) {
			chan->irq = -1;
			chan->irq_dev = NULL;
			return ret;
		}
	} else {
		chan->irq = -1;
		chan->irq_dev = NULL;
	}

	// fill it in
	chan->io = i;
	chan->dev_id = dev_id;
	chan->dev_str = dev_str;
	chan->fifo_addr = dma_dev_table[dev_id].fifo_addr;
	chan->mode = dma_dev_table[dev_id].dma_mode;
	chan->source = dma_dev_table[dev_id].dma_source;

	if (i < HALF_DMA_NUM) {
		REG_DMAC_DMACKES(0) = 1 << i;
	} else {
		REG_DMAC_DMACKES(1) = 1 << (i - HALF_DMA_NUM);
	}

	return i;
}
EXPORT_SYMBOL(jz_request_dma);

/**
 * can be called while wait dma finish interrupt
 * can NOT be called from atomic or interrupt context
 *	(because we use schedule_timeout internally)
 **/
void jz_stop_dma(unsigned int chan)
{
	u32 old_counter = REG_DMAC_DTCR(chan);
	u32 cur_counter;

	/* wait for the counter not change */
	while (1) {
		schedule_timeout(HZ / 10); /* 100ms */
		cur_counter = REG_DMAC_DTCR(chan);
		if (cur_counter == old_counter)
			break;
		old_counter = cur_counter;
	}


	REG_DMAC_DCCSR(chan) = 0;

	REG_DMAC_DCMD(chan) = 0;
	REG_DMAC_DSAR(chan) = 0;
	REG_DMAC_DTAR(chan) = 0;
	REG_DMAC_DTCR(chan) = 0;
	REG_DMAC_DRSR(chan) = 0;
	REG_DMAC_DDA(chan) = 0;
}

void jz_free_dma(unsigned int dmanr)
{
	struct jz_dma_chan *chan = get_dma_chan(dmanr);

	if (!chan) {
		printk("Trying to free DMA%d\n", dmanr);
		return;
	}

	disable_dma(dmanr);
	if (chan->irq)
		free_irq(chan->irq, chan->irq_dev);

	chan->irq = -1;
	chan->irq_dev = NULL;
	chan->dev_id = -1;
}
EXPORT_SYMBOL(jz_free_dma);

void enable_dma(unsigned int dmanr)
{
	struct jz_dma_chan *chan = get_dma_chan(dmanr);

	if (!chan)
		return;

	REG_DMAC_DCCSR(dmanr) &= ~(DMAC_DCCSR_HLT | DMAC_DCCSR_TT | DMAC_DCCSR_AR);
	REG_DMAC_DCCSR(dmanr) |= DMAC_DCCSR_NDES; /* No-descriptor transfer */
	__dmac_enable_channel(dmanr);
	if (chan->irq)
		__dmac_channel_enable_irq(dmanr);
}
EXPORT_SYMBOL(enable_dma);

#define DMA_DISABLE_POLL 0x10000

void disable_dma(unsigned int dmanr)
{
	int i;
	struct jz_dma_chan *chan = get_dma_chan(dmanr);

	if (!chan)
		return;

	if (!__dmac_channel_enabled(dmanr))
		return;

	for (i = 0; i < DMA_DISABLE_POLL; i++)
		if (__dmac_channel_transmit_end_detected(dmanr))
			break;
#if 0
	if (i == DMA_DISABLE_POLL)
		printk(KERN_INFO "disable_dma: poll expired!\n");
#endif

	__dmac_disable_channel(dmanr);
	if (chan->irq)
		__dmac_channel_disable_irq(dmanr);
}
EXPORT_SYMBOL(disable_dma);

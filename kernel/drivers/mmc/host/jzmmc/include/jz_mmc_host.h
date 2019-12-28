/*
 *  linux/drivers/mmc/host/jz_mmc/jz_mmc_host.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Copyright (c) Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ_MMC_HOST_H__
#define __JZ_MMC_HOST_H__

#define USE_DMA_DESC
//#define USE_DMA_UNCACHE
//#define MSC_DEBUG_DMA

#define USE_DMA_BUSRT_64

#define MMC_CLOCK_SLOW    400000      /* 400 kHz for initial setup */
#define MMC_CLOCK_FAST  20000000      /* 20 MHz for maximum for normal operation */
#define SD_CLOCK_HIGH   48000000      /* 24 MHz for SD Cards */
#define SD_CLOCK_FAST   24000000      /* 24 MHz for SD Cards */


struct clk;

struct jz_mmc_host {
	struct mmc_host *mmc;

	struct clk *clk;

	/* host resources */
	void __iomem *base;
	unsigned int pdev_id;
	int irq;
	int dma_id;
	struct jz_mmc_platform_data *pdata;

	/* mmc request related */
	unsigned int cmdat;
	struct mmc_request *curr_mrq;
	int curr_res_type;

	/* data transter related */
	struct {
		int len;
		int dir;
		int channel;
	} dma;
#ifdef USE_DMA_DESC
#ifdef MSC_DEBUG_DMA
	int num_desc;
	int last_direction;
#endif
	struct jz_dma_desc_8word *dma_desc;
#endif
	wait_queue_head_t data_wait_queue;
	volatile int data_ack;
	volatile int data_err;

	/* card detect related */
	int card_detect_irq;

	/* labels for gpio pins */
	char *label_power;
};

void jz_mmc_finish_request(struct jz_mmc_host *host, struct mmc_request *mrq);

#endif /* __JZ_MMC_HOST_H__ */

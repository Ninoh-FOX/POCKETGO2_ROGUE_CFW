/*
 *  linux/drivers/mmc/host/jz_mmc/dma/jz_mmc_dma.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Copyright (c) Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ_MMC_DMA_H__
#define __JZ_MMC_DMA_H__

struct jz_mmc_host;

void jz_mmc_start_dma(struct jz_mmc_host *host);
void jz_mmc_stop_dma(struct jz_mmc_host *host);

void jz_mmc_start_normal_dma(struct jz_mmc_host *host, unsigned long phyaddr,
			     int count, int mode, int ds);
void jz_mmc_start_scatter_dma(int chan, struct jz_mmc_host *host,
			      struct scatterlist *sg, unsigned int sg_len,
			      int mode);

int jz_mmc_init_dma(struct jz_mmc_host *host);
void jz_mmc_deinit_dma(struct jz_mmc_host *host);

#endif /* __JZ_MMC_DMA_H__ */

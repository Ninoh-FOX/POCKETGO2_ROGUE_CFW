/*
 *  linux/drivers/mmc/host/jz_mmc/msc/jz_mmc_msc.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Copyright (c) Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ_MMC_MSC_H__
#define __JZ_MMC_MSC_H__

struct jz_mmc_host;

void jz_mmc_data_start(struct jz_mmc_host *host);

void jz_mmc_reset(struct jz_mmc_host *host);

void jz_mmc_set_clock(struct jz_mmc_host *host, int rate);

void jz_mmc_execute_cmd(struct jz_mmc_host *host);

int jz_mmc_msc_init(struct jz_mmc_host *host);
void jz_mmc_msc_deinit(struct jz_mmc_host *host);

#endif /* __JZ_MMC_MSC_H__ */

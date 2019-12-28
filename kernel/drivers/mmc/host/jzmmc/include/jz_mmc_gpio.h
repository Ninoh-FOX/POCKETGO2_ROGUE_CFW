/*
 *  linux/drivers/mmc/host/jz_mmc/gpio/jz_mmc_gpio.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Copyright (c) Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ_MMC_GPIO_H__
#define __JZ_MMC_GPIO_H__

struct jz_mmc_host;
struct platform_device;

int jz_mmc_gpio_init(struct jz_mmc_host *host, struct platform_device *pdev);
void jz_mmc_gpio_deinit(struct jz_mmc_host *host, struct platform_device *pdev);

#endif /* __JZ_MMC_GPIO_H__ */

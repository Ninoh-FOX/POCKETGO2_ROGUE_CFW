/*
 *  linux/drivers/mmc/host/jz_mmc/jz_mmc_main.c - JZ SD/MMC driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Copyright (c) Ingenic Semiconductor Co., Ltd.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/card.h>
#include <linux/mmc/slot-gpio.h>
#include <linux/mm.h>
#include <linux/signal.h>
#include <linux/pm.h>
#include <linux/scatterlist.h>
#include <asm/io.h>
#include <asm/scatterlist.h>

#include <asm/mach-jz4770/mmc.h>

#include "include/chip-msc.h"
#include "include/jz_mmc_dma.h"
#include "include/jz_mmc_gpio.h"
#include "include/jz_mmc_host.h"
#include "include/jz_mmc_msc.h"


#define JZ_MAX_MSC_NUM 3

void jz_mmc_finish_request(struct jz_mmc_host *host, struct mmc_request *mrq)
{
	host->curr_mrq = NULL;
	mmc_request_done(host->mmc, mrq);
}

static void jz_mmc_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
	struct jz_mmc_host *host = mmc_priv(mmc);

	if (SD_IO_SEND_OP_COND == mrq->cmd->opcode) {
		if(host->pdata->support_sdio == 0) {
			mrq->cmd->error = -ETIMEDOUT;
			jz_mmc_finish_request(host, mrq);
			return;
		}
	}

	BUG_ON (host->curr_mrq);
	host->curr_mrq = mrq;
	jz_mmc_execute_cmd(host);
	jz_mmc_finish_request(host, mrq);
}

/* set clock and power */
static void jz_mmc_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct jz_mmc_host *host = mmc_priv(mmc);

	if (ios->clock)
		jz_mmc_set_clock(host, ios->clock);

	switch (ios->power_mode) {
	case MMC_POWER_UP:
		jz_mmc_reset(host);
		if (gpio_is_valid(host->pdata->gpio_power))
			gpio_set_value(host->pdata->gpio_power,
				       !host->pdata->power_active_low);
		host->cmdat |= MSC_CMDAT_INIT;
		clk_enable(host->clk);
		break;
	case MMC_POWER_ON:
		break;
	default:
		if (gpio_is_valid(host->pdata->gpio_power))
			gpio_set_value(host->pdata->gpio_power,
				       host->pdata->power_active_low);
		clk_disable(host->clk);
		break;
	}

	if (ios->bus_width == MMC_BUS_WIDTH_4) {

		host->cmdat &= ~MSC_CMDAT_BUS_WIDTH_MASK;

		if(host->pdata->bus_width == 4)
			host->cmdat |= MSC_CMDAT_BUS_WIDTH_4BIT;
		else
			host->cmdat |= host->pdata->bus_width;
	} else if (ios->bus_width == MMC_BUS_WIDTH_8) {

		host->cmdat &= ~MSC_CMDAT_BUS_WIDTH_MASK;

		if(host->pdata->bus_width == 8)
			host->cmdat |= MSC_CMDAT_BUS_WIDTH_8BIT;
//		else
//			host->cmdat |= host->pdata->bus_width;
	} else {
		/* 1 bit bus*/
		host->cmdat &= ~MSC_CMDAT_BUS_WIDTH_8BIT;
	}
}

static const struct mmc_host_ops jz_mmc_ops = {
	.request = jz_mmc_request,
	.set_ios = jz_mmc_set_ios,
	.get_cd = mmc_gpio_get_cd,
	.get_ro = mmc_gpio_get_ro,
};

static int jz_mmc_probe(struct platform_device *pdev)
{
	int ret;
	struct jz_mmc_platform_data *pdata = pdev->dev.platform_data;
	struct mmc_host *mmc;
	struct jz_mmc_host *host;

	struct resource *dmares;

	char clk_name[5];

	if (pdev->id < 0 || pdev->id >= JZ_MAX_MSC_NUM) {
		dev_err(&pdev->dev, "Device ID out of range: %d\n", pdev->id);
		return -EINVAL;
	}

	if (!pdata) {
		dev_err(&pdev->dev, "Missing platform data\n");
		return -EINVAL;
	}

	mmc = mmc_alloc_host(sizeof(struct jz_mmc_host), &pdev->dev);
	if (!mmc) {
		dev_err(&pdev->dev, "Failed to alloc mmc host structure\n");
		return -ENOMEM;
	}

	host = mmc_priv(mmc);
	host->pdata = pdata;

	host->irq = platform_get_irq(pdev, 0);
	if (host->irq < 0) {
		ret = host->irq;
		dev_err(&pdev->dev, "Failed to get platform irq: %d\n", ret);
		goto err_free_host;
	}

	sprintf(clk_name, "mmc%i", pdev->id);
	host->clk = devm_clk_get(&pdev->dev, clk_name);
	if (IS_ERR(host->clk)) {
		ret = PTR_ERR(host->clk);
		dev_err(&pdev->dev, "Failed to get mmc clock\n");
		goto err_free_host;
	}

	host->base = devm_ioremap_resource(&pdev->dev,
			platform_get_resource(pdev, IORESOURCE_MEM, 0));
	if (IS_ERR(host->base)) {
		ret = PTR_ERR(host->base);
		dev_err(&pdev->dev, "Failed to get and remap mmio region\n");
		goto err_free_host;
	}

	dmares = platform_get_resource(pdev, IORESOURCE_DMA, 0);
	if (!dmares) {
		ret = -ENOENT;
		dev_err(&pdev->dev, "Failed to get platform dma\n");
		goto err_free_host;
	}
	host->dma_id = dmares->start;

	/*
	 * Setup MMC host structure
	 */
	mmc->ops = &jz_mmc_ops;
	mmc->f_min = MMC_CLOCK_SLOW;
	mmc->f_max = SD_CLOCK_HIGH;
	mmc->ocr_avail = pdata->ocr_mask;

	mmc->caps = MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED |
		    MMC_CAP_ERASE;
	if (pdata->bus_width >= 4)
		mmc->caps |= MMC_CAP_4_BIT_DATA;
	if (pdata->bus_width >= 8)
		mmc->caps |= MMC_CAP_8_BIT_DATA;
	if (pdata->nonremovable)
		mmc->caps |= MMC_CAP_NONREMOVABLE;

	if (!pdata->card_detect_active_low)
		mmc->caps2 |= MMC_CAP2_CD_ACTIVE_HIGH;
	if (!pdata->read_only_active_low)
		mmc->caps2 |= MMC_CAP2_RO_ACTIVE_HIGH;

	mmc->max_blk_size = 4095;
	mmc->max_blk_count = 65535;
	mmc->max_req_size = PAGE_SIZE * 16;

	mmc->max_segs = 1;
	mmc->max_seg_size = mmc->max_req_size;

	host->pdev_id = pdev->id;
	host->mmc = mmc;
	//spin_lock_init(&host->lock);

	ret = jz_mmc_msc_init(host);
	if (ret)
		goto err_free_host;

	ret = jz_mmc_gpio_init(host, pdev);
	if (ret)
		goto err_deinit_msc;

	ret = jz_mmc_init_dma(host);
	if (ret)
		goto err_deinit_gpio;

	if (gpio_is_valid(host->pdata->gpio_power))
		gpio_set_value(host->pdata->gpio_power,
			       !host->pdata->power_active_low);

	platform_set_drvdata(pdev, host);
	ret = mmc_add_host(mmc);

	if (ret) {
		dev_err(&pdev->dev, "Failed to add mmc host: %d\n", ret);
		goto err_deinit_dma;
	}
	dev_info(&pdev->dev, "JZ SD/MMC card driver registered\n");

	return 0;

err_deinit_dma:
	jz_mmc_deinit_dma(host);
err_deinit_gpio:
	jz_mmc_gpio_deinit(host, pdev);
err_deinit_msc:
	jz_mmc_msc_deinit(host);
err_free_host:
	mmc_free_host(mmc);

	return ret;
}

static int jz_mmc_remove(struct platform_device *pdev)
{
	struct jz_mmc_host *host = platform_get_drvdata(pdev);

	if (gpio_is_valid(host->pdata->gpio_power))
		gpio_set_value(host->pdata->gpio_power,
				host->pdata->power_active_low);

	jz_mmc_deinit_dma(host);
	jz_mmc_gpio_deinit(host, pdev);
	jz_mmc_msc_deinit(host);

	mmc_remove_host(host->mmc);
	mmc_free_host(host->mmc);

	return 0;
}

#ifdef CONFIG_PM

static int jz_mmc_suspend(struct device *dev)
{
	struct jz_mmc_host *host = dev_get_drvdata(dev);

	return mmc_suspend_host(host->mmc);
}

static int jz_mmc_resume(struct device *dev)
{
	struct jz_mmc_host *host = dev_get_drvdata(dev);
	int ret;

	ret = mmc_resume_host(host->mmc);
	if (!ret)
		return ret;

	if (host->card_detect_irq < 0)
		if (clk_get_rate(host->clk) > SD_CLOCK_FAST)
			REG_MSC_LPM(host->pdev_id) |= 1 << 31;

	return 0;
}

const struct dev_pm_ops jz_mmc_pm_ops = {
	.suspend	= jz_mmc_suspend,
	.resume		= jz_mmc_resume,
	.poweroff	= jz_mmc_suspend,
	.restore	= jz_mmc_resume,
};

#define JZ_MMC_PM_OPS (&jz_mmc_pm_ops)
#else
#define JZ_MMC_PM_OPS NULL
#endif

static struct platform_driver jz_msc_driver = {
	.probe		= jz_mmc_probe,
	.remove		= jz_mmc_remove,
	.driver = {
		.name	= "jz-msc",
		.owner	= THIS_MODULE,
		.pm	= JZ_MMC_PM_OPS,
	},
};

module_platform_driver(jz_msc_driver);

MODULE_DESCRIPTION("JZ47XX SD/Multimedia Card Interface Driver");
MODULE_LICENSE("GPL");

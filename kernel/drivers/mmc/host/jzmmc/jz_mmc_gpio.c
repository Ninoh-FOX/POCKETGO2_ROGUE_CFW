/*
 *  linux/drivers/mmc/host/jz_mmc/gpio/jz_mmc_gpio.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Copyright (c) Ingenic Semiconductor Co., Ltd.
 */

#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/mmc/host.h>
#include <linux/mmc/slot-gpio.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <asm/mach-jz4770/mmc.h>

#include "include/chip-msc.h"
#include "include/jz_mmc_gpio.h"
#include "include/jz_mmc_host.h"
#include "include/jz_mmc_msc.h"


static int jz_mmc_request_card_gpios(struct platform_device *pdev,
				     struct jz_mmc_host *host)
{
	/*
	 * We rely on the board setting a static pinmux mapping to match its
	 * platform data.
	 */
	return 0;
}

static int jz_mmc_request_power_gpio(struct platform_device *pdev,
				     struct jz_mmc_host *host)
{
	struct device *dev = &pdev->dev;
	struct jz_mmc_platform_data *pdata = dev->platform_data;
	char *label = kasprintf(GFP_KERNEL, "%s power", dev_name(dev));
	int ret;

	ret = devm_gpio_request(dev, pdata->gpio_power, label);
	if (ret) {
		dev_err(dev, "Failed to request power gpio: %d\n", ret);
		kfree(label);
		return ret;
	}
	host->label_power = label;

	gpio_direction_output(pdata->gpio_power, pdata->power_active_low);

	return 0;
}

int jz_mmc_gpio_init(struct jz_mmc_host *host, struct platform_device *pdev)
{
	struct jz_mmc_platform_data *pdata = pdev->dev.platform_data;
	int ret;

	ret = jz_mmc_request_card_gpios(pdev, host);
	if (ret)
		return ret;

	if (gpio_is_valid(pdata->gpio_power)) {
		ret = jz_mmc_request_power_gpio(pdev, host);
		if (ret)
			return ret;
	}

	if (gpio_is_valid(pdata->gpio_read_only)) {
		ret = mmc_gpio_request_ro(host->mmc, pdata->gpio_read_only);
		if (ret)
			return ret;
	}

	if (gpio_is_valid(pdata->gpio_card_detect)) {
		ret = mmc_gpio_request_cd(host->mmc, pdata->gpio_card_detect,
					  0);
		if (ret)
			return ret;
	} else if (!pdata->nonremovable) {
		dev_info(&pdev->dev, "No card detect facilities available\n");
	}

	return 0;
}

void jz_mmc_gpio_deinit(struct jz_mmc_host *host, struct platform_device *pdev)
{
	if (host->card_detect_irq >= 0) {
		disable_irq(host->card_detect_irq);
		device_init_wakeup(&pdev->dev, 0);
	}

	kfree(host->label_power);
}

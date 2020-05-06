/*`
 * Joystick driver for analog stick connected to JZ4770 SADC touch screen
 * input pins.
 *
 * Copyright (C) 2012, Maarten ter Huurne <maarten@treewalker.org>
 *
 *Pocketgo2 specific modifications:
 * Copyright (C) 2019, Ninoh-FOX <ninohfox@gmail.com>

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/bug.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <linux/mfd/core.h>

#include <linux/jz4770-adc.h>


#define JZ_REG_ADC_TS_SAME	0
#define JZ_REG_ADC_TS_WAIT	4
#define JZ_REG_ADC_TS_DATA	8


#define JOYSTICK_MIN_X		0
#define JOYSTICK_MIN_Y  	0
#define JOYSTICK_MAX_X		3300
#define JOYSTICK_MAX_Y		3300

#define JOYSTICK_NOISE_X	4
#define JOYSTICK_NOISE_Y	4
#define JOYSTICK_FLAT_X		200
#define JOYSTICK_FLAT_Y		200


struct jz_joystick {
	struct input_dev *input_dev;
	struct platform_device *pdev;

	void __iomem *base;
	int irq;

	const struct mfd_cell *cell;

	bool is_open;
};

static irqreturn_t jz_joystick_irq_handler(int irq, void *devid)
{
	struct jz_joystick *joystick = devid;
	unsigned long val;
	int x, y;

	val = readl(joystick->base + JZ_REG_ADC_TS_DATA);
    x = val & 0xFFF;
	y = (val >> 16) & 0xFFF;

	input_report_abs(joystick->input_dev, ABS_X, JOYSTICK_MAX_X - x);
	input_report_abs(joystick->input_dev, ABS_Y, y);
	
	input_sync(joystick->input_dev);

	return IRQ_HANDLED;
}

static void jz_joystick_enable(struct jz_joystick *joystick)
{
	joystick->cell->enable(joystick->pdev);
	enable_irq(joystick->irq);
}

static void jz_joystick_disable(struct jz_joystick *joystick)
{
	disable_irq(joystick->irq);
	joystick->cell->disable(joystick->pdev);
}

static int jz_joystick_open(struct input_dev *input)
{
	struct jz_joystick *joystick = input_get_drvdata(input);

	joystick->is_open = true;
	jz_joystick_enable(joystick);

	return 0;
}

static void jz_joystick_close(struct input_dev *input)
{
	struct jz_joystick *joystick = input_get_drvdata(input);

	jz_joystick_disable(joystick);
	joystick->is_open = false;
}

static int jz_joystick_probe(struct platform_device *pdev)
{
	struct jz_joystick *joystick;
	struct input_dev *input_dev;
	int ret;


	/* Acquire resources. */

	joystick = devm_kzalloc(&pdev->dev, sizeof(*joystick), GFP_KERNEL);
	if (!joystick) {
		dev_err(&pdev->dev, "Failed to allocate driver structure\n");
		return -ENOMEM;
	}

	joystick->base = devm_ioremap_resource(&pdev->dev,
			platform_get_resource(pdev, IORESOURCE_MEM, 0));
	if (IS_ERR(joystick->base)) {
		ret = PTR_ERR(joystick->base);
		dev_err(&pdev->dev,
			"Failed to get and remap mmio region: %d\n", ret);
		return ret;
	}

	joystick->irq = platform_get_irq(pdev, 0);
	if (joystick->irq < 0) {
		dev_err(&pdev->dev, "Failed to get platform irq: %d\n",
			joystick->irq);
		return joystick->irq;
	}
	dev_info(&pdev->dev, "Got platform irq: %d\n", joystick->irq);

	joystick->cell = mfd_get_cell(pdev);
	joystick->pdev = pdev;

	platform_set_drvdata(pdev, joystick);


	/* Set up input device. */

	input_dev = devm_input_allocate_device(&pdev->dev);
	if (!input_dev) {
		dev_err(&pdev->dev, "Failed to allocate input device\n");
		return -ENOMEM;
	}
	joystick->input_dev = input_dev;

	input_dev->name = "analog joystick";
	input_dev->id.bustype = BUS_HOST;
	input_dev->dev.parent = &pdev->dev;

	__set_bit(EV_ABS, input_dev->evbit);
	input_set_abs_params(input_dev, ABS_X, JOYSTICK_MIN_X, JOYSTICK_MAX_X,
			     JOYSTICK_NOISE_X, JOYSTICK_FLAT_X);
	input_set_abs_params(input_dev, ABS_Y, JOYSTICK_MIN_Y, JOYSTICK_MAX_Y,
			     JOYSTICK_NOISE_Y, JOYSTICK_FLAT_Y);

	input_set_drvdata(input_dev, joystick);
	input_dev->open = jz_joystick_open;
	input_dev->close = jz_joystick_close;


	/* Initialize touch screen registers. */
	joystick->cell->enable(pdev);

	/* Read 4 samples for each measurement. */
	jz4770_adc_set_config(pdev->dev.parent,
					JZ_ADC_CONFIG_CMD_SEL
					| JZ_ADC_CONFIG_SAMPLE_NUM_MASK,
					JZ_ADC_CONFIG_CMD_SEL
					| JZ_ADC_CONFIG_SAMPLE_NUM(4)
					);
	/* This timing results in approximately 50 measurements per second. */
	jz4770_adc_set_adcmd(pdev->dev.parent);
	writew(2, joystick->base + JZ_REG_ADC_TS_SAME);
	writew(80, joystick->base + JZ_REG_ADC_TS_WAIT);

	joystick->cell->disable(pdev);


	/* Set up "result ready" IRQ. */
	ret = devm_request_irq(&pdev->dev, joystick->irq,
			       jz_joystick_irq_handler, 0, pdev->name,
			       joystick);
	if (ret) {
		dev_err(&pdev->dev, "Failed to request irq %d\n", ret);
		return ret;
	}

	ret = input_register_device(input_dev);
	if (ret) {
		dev_err(&pdev->dev,
			"Failed to register input device: %d\n", ret);
		return ret;
	}

	return 0;
}

static int jz_joystick_remove(struct platform_device *pdev)
{
	struct jz_joystick *joystick = platform_get_drvdata(pdev);

	input_unregister_device(joystick->input_dev);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int jz_joystick_suspend(struct device *dev)
{
	struct jz_joystick *joystick = dev_get_drvdata(dev);

	if (joystick->is_open)
		jz_joystick_disable(joystick);

	return 0;
}

static int jz_joystick_resume(struct device *dev)
{
	struct jz_joystick *joystick = dev_get_drvdata(dev);

	if (joystick->is_open)
		jz_joystick_enable(joystick);

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(jz_joystick_pm_ops,
		jz_joystick_suspend, jz_joystick_resume);

static struct platform_driver jz_joystick_driver = {
	.probe		= jz_joystick_probe,
	.remove		= jz_joystick_remove,
	.driver = {
		/* Name needs to match the mfd cell name. */
		.name = "jz4770-touchscreen",
		.owner = THIS_MODULE,
		.pm = &jz_joystick_pm_ops,
	},
};

module_platform_driver(jz_joystick_driver);

MODULE_ALIAS("platform:jz-sadc-joy");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maarten ter Huurne <maarten@treewalker.org>");
MODULE_DESCRIPTION("JZ4770 SoC SADC analog joystick driver");

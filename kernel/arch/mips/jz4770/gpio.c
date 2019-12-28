/*
 * JZ4770 platform GPIO support
 *
 * Original JZ4770 support by hlguo <hlguo@ingenic.cn>
 * Copyright (C) Ingenic Semiconductor Inc.
 *
 * JZ4740 platform GPIO support.
 * Copyright (C) 2009-2010, Lars-Peter Clausen <lars@metafoo.de>
 *
 * Stiched the JZ4740 and JZ4770 code together.
 * Copyright (C) 2012, Maarten ter Huurne <maarten@treewalker.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/bitops.h>
#include <linux/export.h>
#include <linux/pinctrl/consumer.h>

#include <asm/mach-jz4770/gpio.h>
#include <asm/mach-jz4770/jz4770gpio.h>
#include <asm/mach-jz4770/jz4770intc.h>


#define JZ_REG_GPIO_PIN			0x00
#define JZ_REG_GPIO_INT			0x10
#define JZ_REG_GPIO_INT_SET		0x14
#define JZ_REG_GPIO_INT_CLEAR		0x18
#define JZ_REG_GPIO_MASK		0x20
#define JZ_REG_GPIO_MASK_SET		0x24
#define JZ_REG_GPIO_MASK_CLEAR		0x28
#define JZ_REG_GPIO_PAT1		0x30
#define JZ_REG_GPIO_PAT1_SET		0x34
#define JZ_REG_GPIO_PAT1_CLEAR		0x38
#define JZ_REG_GPIO_PAT0		0x40
#define JZ_REG_GPIO_PAT0_SET		0x44
#define JZ_REG_GPIO_PAT0_CLEAR		0x48
#define JZ_REG_GPIO_FLAG		0x50
#define JZ_REG_GPIO_FLAG_CLEAR		0x58

struct jz_gpio_chip {
	unsigned int irq;
	unsigned int irq_base;
	uint32_t edge_trigger_both;

	void __iomem *base;

	struct gpio_chip gpio_chip;
};

static inline struct jz_gpio_chip *to_jz4770_chip(struct gpio_chip *gpc)
{
	return container_of(gpc, struct jz_gpio_chip, gpio_chip);
}

static inline struct jz_gpio_chip *irq_to_jz_gpio_chip(struct irq_data *data)
{
	struct irq_chip_generic *gc = irq_data_get_irq_chip_data(data);
	return gc->private;
}

static void jz4770_gpiolib_set(struct gpio_chip *chip,
				unsigned offset, int value)
{
	unsigned long flags;
	unsigned int n = 0;

	n = chip->base +offset;
	local_irq_save(flags);
	if (value)
		__gpio_set_pin(n);
	else
		__gpio_clear_pin(n);
	local_irq_restore(flags);
}

static int jz4770_gpiolib_get(struct gpio_chip *chip, unsigned offset)
{
	unsigned long flags;
	unsigned int n = 0;
	int state = -1;

	n = chip->base +offset;
	local_irq_save(flags);
	state = __gpio_get_pin(n);
	local_irq_restore(flags);
	return state;

}

static int jz4770_gpiolib_input(struct gpio_chip *chip, unsigned offset)
{
	return pinctrl_gpio_direction_input(chip->base + offset);
}

static int jz4770_gpiolib_output(struct gpio_chip *chip,
					unsigned offset, int value)
{
	int ret = pinctrl_gpio_direction_output(chip->base + offset);
	if (!ret) {
		if (value)
			__gpio_set_pin(chip->base + offset);
		else
			__gpio_clear_pin(chip->base + offset);
	}

	return ret;
}

void jz_gpio_enable_pullup(unsigned gpio)
{
	__gpio_enable_pull(gpio);
}
EXPORT_SYMBOL_GPL(jz_gpio_enable_pullup);

void jz_gpio_disable_pullup(unsigned gpio)
{
	__gpio_disable_pull(gpio);
}
EXPORT_SYMBOL_GPL(jz_gpio_disable_pullup);

int gpio_to_irq(unsigned gpio)
{
	return IRQ_GPIO_0 + gpio;
}
EXPORT_SYMBOL_GPL(gpio_to_irq);

int irq_to_gpio(unsigned irq)
{
	return irq - IRQ_GPIO_0;
}
EXPORT_SYMBOL_GPL(irq_to_gpio);

static int jz4770_gpio_request(struct gpio_chip *chip, unsigned offset)
{
	return pinctrl_request_gpio(chip->base + offset);
}

static void jz4770_gpio_free(struct gpio_chip *chip, unsigned offset)
{
	pinctrl_free_gpio(chip->base + offset);
}

#define IRQ_TO_BIT(irq) BIT(irq_to_gpio(irq) & 0x1f)

static void jz_gpio_check_trigger_both(struct jz_gpio_chip *chip, unsigned int irq)
{
	uint32_t value;
	void __iomem *reg;
	uint32_t mask = IRQ_TO_BIT(irq);

	if (!(chip->edge_trigger_both & mask))
		return;

	reg = chip->base;

	value = readl(chip->base + JZ_REG_GPIO_PIN);
	if (value & mask)
		reg += JZ_REG_GPIO_PAT0_CLEAR; /* falling edge */
	else
		reg += JZ_REG_GPIO_PAT0_SET; /* rising edge */

	writel(mask, reg);
}

static void jz_gpio_irq_demux_handler(unsigned int irq, struct irq_desc *desc)
{
	uint32_t flag;
	unsigned int gpio_irq;
	struct jz_gpio_chip *chip = irq_desc_get_handler_data(desc);

	flag = readl(chip->base + JZ_REG_GPIO_FLAG);
	if (!flag)
		return;

	gpio_irq = chip->irq_base + __fls(flag);

	jz_gpio_check_trigger_both(chip, gpio_irq);

	generic_handle_irq(gpio_irq);
};

static inline void jz_gpio_set_irq_bit(struct irq_data *data, unsigned int reg)
{
	struct jz_gpio_chip *chip = irq_to_jz_gpio_chip(data);
	writel(IRQ_TO_BIT(data->irq), chip->base + reg);
}

static void jz_gpio_irq_unmask(struct irq_data *data)
{
	struct jz_gpio_chip *chip = irq_to_jz_gpio_chip(data);

	jz_gpio_check_trigger_both(chip, data->irq);
	irq_gc_unmask_enable_reg(data);
};

/* TODO: Check if function is gpio */
static unsigned int jz_gpio_irq_startup(struct irq_data *data)
{
	jz_gpio_set_irq_bit(data, JZ_REG_GPIO_INT_SET);
	jz_gpio_irq_unmask(data);
	return 0;
}

static void jz_gpio_irq_shutdown(struct irq_data *data)
{
	irq_gc_mask_disable_reg(data);

	/* Set direction to input */
	jz_gpio_set_irq_bit(data, JZ_REG_GPIO_PAT0_CLEAR);
	jz_gpio_set_irq_bit(data, JZ_REG_GPIO_INT_CLEAR);
}

static int jz_gpio_irq_set_type(struct irq_data *data, unsigned int flow_type)
{
	struct jz_gpio_chip *chip = irq_to_jz_gpio_chip(data);
	unsigned int irq = data->irq;

	if (flow_type == IRQ_TYPE_EDGE_BOTH) {
		uint32_t value = readl(chip->base + JZ_REG_GPIO_PIN);
		if (value & IRQ_TO_BIT(irq))
			flow_type = IRQ_TYPE_EDGE_FALLING;
		else
			flow_type = IRQ_TYPE_EDGE_RISING;
		chip->edge_trigger_both |= IRQ_TO_BIT(irq);
	} else {
		chip->edge_trigger_both &= ~IRQ_TO_BIT(irq);
	}

	switch (flow_type) {
	case IRQ_TYPE_EDGE_RISING:
		jz_gpio_set_irq_bit(data, JZ_REG_GPIO_PAT0_SET);
		jz_gpio_set_irq_bit(data, JZ_REG_GPIO_PAT1_SET);
		break;
	case IRQ_TYPE_EDGE_FALLING:
		jz_gpio_set_irq_bit(data, JZ_REG_GPIO_PAT0_CLEAR);
		jz_gpio_set_irq_bit(data, JZ_REG_GPIO_PAT1_SET);
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		jz_gpio_set_irq_bit(data, JZ_REG_GPIO_PAT0_SET);
		jz_gpio_set_irq_bit(data, JZ_REG_GPIO_PAT1_CLEAR);
		break;
	case IRQ_TYPE_LEVEL_LOW:
		jz_gpio_set_irq_bit(data, JZ_REG_GPIO_PAT0_CLEAR);
		jz_gpio_set_irq_bit(data, JZ_REG_GPIO_PAT1_CLEAR);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int jz_gpio_irq_set_wake(struct irq_data *data, unsigned int on)
{
	struct jz_gpio_chip *chip = irq_to_jz_gpio_chip(data);

	irq_gc_set_wake(data, on);
	irq_set_irq_wake(chip->irq, on);

	return 0;
}

// TODO(MtH): These three functions belong in irq.c, but that has not been
//            migrated to struct irq_chip_generic yet.

static void jz4770_irq_set_mask(struct irq_chip_generic *gc, uint32_t mask)
{
	struct irq_chip_regs *regs = &gc->chip_types->regs;

	writel(mask, gc->reg_base + regs->enable);
	writel(~mask, gc->reg_base + regs->disable);
}

void jz4770_irq_suspend(struct irq_data *data)
{
	struct irq_chip_generic *gc = irq_data_get_irq_chip_data(data);
	jz4770_irq_set_mask(gc, gc->wake_active);
}

void jz4770_irq_resume(struct irq_data *data)
{
	struct irq_chip_generic *gc = irq_data_get_irq_chip_data(data);
	jz4770_irq_set_mask(gc, gc->mask_cache);
}

static struct jz_gpio_chip jz4770_gpios[] = {
	[0] = {
		.base				= (unsigned *)GPIO_BASEA,
		.irq				= IRQ_GPIO0,
		.gpio_chip = {
			.base			= 0*32,
			.owner			= THIS_MODULE,
			.label			= "GPIOA",
			.ngpio			= 32,
			.direction_input	= jz4770_gpiolib_input,
			.direction_output	= jz4770_gpiolib_output,
			.set			= jz4770_gpiolib_set,
			.get			= jz4770_gpiolib_get,
			.request		= jz4770_gpio_request,
			.free			= jz4770_gpio_free,
		},
	},
	[1] = {
		.base				= (unsigned *)GPIO_BASEB,
		.irq				= IRQ_GPIO1,
		.gpio_chip = {
			.base			= 1*32,
			.owner			= THIS_MODULE,
			.label			= "GPIOB",
			.ngpio			= 32,
			.direction_input	= jz4770_gpiolib_input,
			.direction_output	= jz4770_gpiolib_output,
			.set			= jz4770_gpiolib_set,
			.get			= jz4770_gpiolib_get,
			.request		= jz4770_gpio_request,
			.free			= jz4770_gpio_free,
		},
	},
	[2] = {
		.base				= (unsigned *)GPIO_BASEC,
		.irq				= IRQ_GPIO2,
		.gpio_chip = {
			.base			= 2*32,
			.owner			= THIS_MODULE,
			.label			= "GPIOC",
			.ngpio			= 32,
			.direction_input	= jz4770_gpiolib_input,
			.direction_output	= jz4770_gpiolib_output,
			.set			= jz4770_gpiolib_set,
			.get			= jz4770_gpiolib_get,
			.request		= jz4770_gpio_request,
			.free			= jz4770_gpio_free,
		},
	},
	[3] = {
		.base				= (unsigned *)GPIO_BASED,
		.irq				= IRQ_GPIO3,
		.gpio_chip = {
			.base			= 3*32,
			.owner			= THIS_MODULE,
			.label			= "GPIOD",
			.ngpio			= 32,
			.direction_input	= jz4770_gpiolib_input,
			.direction_output	= jz4770_gpiolib_output,
			.set			= jz4770_gpiolib_set,
			.get			= jz4770_gpiolib_get,
			.request		= jz4770_gpio_request,
			.free			= jz4770_gpio_free,
		},
	},
	[4] = {
		.base				= (unsigned *)GPIO_BASEE,
		.irq				= IRQ_GPIO4,
		.gpio_chip = {
			.base			= 4*32,
			.label			= "GPIOE",
			.owner			= THIS_MODULE,
			.ngpio			= 32,
			.direction_input	= jz4770_gpiolib_input,
			.direction_output	= jz4770_gpiolib_output,
			.set			= jz4770_gpiolib_set,
			.get			= jz4770_gpiolib_get,
			.request		= jz4770_gpio_request,
			.free			= jz4770_gpio_free,
		},
	},
	[5] = {
		.base				= (unsigned *)GPIO_BASEF,
		.irq				= IRQ_GPIO5,
		.gpio_chip = {
			.base			= 5*32,
			.owner			= THIS_MODULE,
			.label			= "GPIOF",
			.ngpio			= 32,
			.direction_input	= jz4770_gpiolib_input,
			.direction_output	= jz4770_gpiolib_output,
			.set			= jz4770_gpiolib_set,
			.get			= jz4770_gpiolib_get,
			.request		= jz4770_gpio_request,
			.free			= jz4770_gpio_free,
		},
	},
};

static void jz4770_gpio_chip_init(struct jz_gpio_chip *chip, unsigned int id)
{
	struct irq_chip_generic *gc;
	struct irq_chip_type *ct;

	chip->base = ioremap(CPHYSADDR(GPIO_BASE) + (id * 0x100), 0x100);
	chip->irq_base = gpio_to_irq(chip->gpio_chip.base);

	irq_set_handler_data(chip->irq, chip);
	irq_set_chained_handler(chip->irq, jz_gpio_irq_demux_handler);

	gc = irq_alloc_generic_chip(chip->gpio_chip.label, 1, chip->irq_base,
		chip->base, handle_level_irq);

	gc->wake_enabled = IRQ_MSK(chip->gpio_chip.ngpio);
	gc->private = chip;

	ct = gc->chip_types;
	ct->regs.enable = JZ_REG_GPIO_MASK_CLEAR;
	ct->regs.disable = JZ_REG_GPIO_MASK_SET;
	ct->regs.ack = JZ_REG_GPIO_FLAG_CLEAR;

	ct->chip.name = "GPIO";
	ct->chip.irq_mask = irq_gc_mask_disable_reg;
	ct->chip.irq_unmask = jz_gpio_irq_unmask;
	ct->chip.irq_ack = irq_gc_ack_set_bit;
	ct->chip.irq_suspend = jz4770_irq_suspend;
	ct->chip.irq_resume = jz4770_irq_resume;
	ct->chip.irq_startup = jz_gpio_irq_startup;
	ct->chip.irq_shutdown = jz_gpio_irq_shutdown;
	ct->chip.irq_set_type = jz_gpio_irq_set_type;
	ct->chip.irq_set_wake = jz_gpio_irq_set_wake;
	ct->chip.flags = IRQCHIP_SET_TYPE_MASKED;

	irq_setup_generic_chip(gc, IRQ_MSK(chip->gpio_chip.ngpio),
		IRQ_GC_INIT_NESTED_LOCK, 0, IRQ_NOPROBE | IRQ_LEVEL);

	gpiochip_add(&chip->gpio_chip);

	gpiochip_add_pin_range(&chip->gpio_chip, "jz4770-pinctrl",
			0, chip->gpio_chip.base, chip->gpio_chip.ngpio);
}

static __init int jz4770_gpiolib_init(void)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(jz4770_gpios); i++)
		jz4770_gpio_chip_init(&jz4770_gpios[i], i);

	printk(KERN_INFO "JZ4770 GPIO initialized\n");

	return 0;
}

arch_initcall(jz4770_gpiolib_init);

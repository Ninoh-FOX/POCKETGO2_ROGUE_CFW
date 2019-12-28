/*
 * Pinctrl driver for Ingenic JZ4770 SoC
 *
 * Copyright (c) 2014 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <asm/io.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/platform_device.h>

#define PORTS		6
#define PINS		32
#define PORT(x)		((x) / PINS)
#define PORT_PIN(x)	((x) % PINS)

#define GPA(x)		((x) + 32 * 0)
#define GPB(x)		((x) + 32 * 1)
#define GPC(x)		((x) + 32 * 2)
#define GPD(x)		((x) + 32 * 3)
#define GPE(x)		((x) + 32 * 4)
#define GPF(x)		((x) + 32 * 5)

#define INT_OFFSET(port)	((port) * 0x100 + 0x10)
#define INT_SET_OFFSET(port)	((port) * 0x100 + 0x14)
#define INT_CLR_OFFSET(port)	((port) * 0x100 + 0x18)

#define MASK_OFFSET(port)	((port) * 0x100 + 0x20)
#define MASK_SET_OFFSET(port)	((port) * 0x100 + 0x24)
#define MASK_CLR_OFFSET(port)	((port) * 0x100 + 0x28)

#define PAT1_OFFSET(port)	((port) * 0x100 + 0x30)
#define PAT1_SET_OFFSET(port)	((port) * 0x100 + 0x34)
#define PAT1_CLR_OFFSET(port)	((port) * 0x100 + 0x38)

#define PAT0_OFFSET(port)	((port) * 0x100 + 0x40)
#define PAT0_SET_OFFSET(port)	((port) * 0x100 + 0x44)
#define PAT0_CLR_OFFSET(port)	((port) * 0x100 + 0x48)

#define PULL_OFFSET(port)	((port) * 0x100 + 0x70)
#define PULL_SET_OFFSET(port)	((port) * 0x100 + 0x74)
#define PULL_CLR_OFFSET(port)	((port) * 0x100 + 0x78)

struct jz_pinctrl {
	struct device *dev;
	struct pinctrl_dev *pctl;
	void __iomem *base;
	spinlock_t lock;
};

struct jz_pin_group {
	const char *name;
	const unsigned int *pins;
	const u8 *sels;
	unsigned int num_pins;
};

struct jz_pmux_func {
	const char *name;
	const char * const *groups;
	unsigned int num_groups;
};

static struct pinctrl_pin_desc jz_pins[] = {
#define JZ_PIN_DESC(_name, pin) { \
	.number = pin, \
	.name = _name, \
}
	/* Port A */
	JZ_PIN_DESC("A00", GPA(0)),
	JZ_PIN_DESC("A01", GPA(1)),
	JZ_PIN_DESC("A02", GPA(2)),
	JZ_PIN_DESC("A03", GPA(3)),
	JZ_PIN_DESC("A04", GPA(4)),
	JZ_PIN_DESC("A05", GPA(5)),
	JZ_PIN_DESC("A06", GPA(6)),
	JZ_PIN_DESC("A07", GPA(7)),
	JZ_PIN_DESC("A08", GPA(8)),
	JZ_PIN_DESC("A09", GPA(9)),
	JZ_PIN_DESC("A10", GPA(10)),
	JZ_PIN_DESC("A11", GPA(11)),
	JZ_PIN_DESC("A12", GPA(12)),
	JZ_PIN_DESC("A13", GPA(13)),
	JZ_PIN_DESC("A14", GPA(14)),
	JZ_PIN_DESC("A15", GPA(15)),
	JZ_PIN_DESC("A16", GPA(16)),
	JZ_PIN_DESC("A17", GPA(17)),
	JZ_PIN_DESC("A18", GPA(18)),
	JZ_PIN_DESC("A19", GPA(19)),
	JZ_PIN_DESC("A20", GPA(20)),
	JZ_PIN_DESC("A21", GPA(21)),
	JZ_PIN_DESC("A22", GPA(22)),
	JZ_PIN_DESC("A23", GPA(23)),
	JZ_PIN_DESC("A24", GPA(24)),
	JZ_PIN_DESC("A25", GPA(25)),
	JZ_PIN_DESC("A26", GPA(26)),
	JZ_PIN_DESC("A27", GPA(27)),
	JZ_PIN_DESC("A28", GPA(28)),
	JZ_PIN_DESC("A29", GPA(29)),
	JZ_PIN_DESC("A30", GPA(30)),
	/* A31 does not exist */

	/* Port B */
	JZ_PIN_DESC("B00", GPB(0)),
	JZ_PIN_DESC("B01", GPB(1)),
	JZ_PIN_DESC("B02", GPB(2)),
	JZ_PIN_DESC("B03", GPB(3)),
	JZ_PIN_DESC("B04", GPB(4)),
	JZ_PIN_DESC("B05", GPB(5)),
	JZ_PIN_DESC("B06", GPB(6)),
	JZ_PIN_DESC("B07", GPB(7)),
	JZ_PIN_DESC("B08", GPB(8)),
	JZ_PIN_DESC("B09", GPB(9)),
	JZ_PIN_DESC("B10", GPB(10)),
	JZ_PIN_DESC("B11", GPB(11)),
	JZ_PIN_DESC("B12", GPB(12)),
	JZ_PIN_DESC("B13", GPB(13)),
	JZ_PIN_DESC("B14", GPB(14)),
	JZ_PIN_DESC("B15", GPB(15)),
	JZ_PIN_DESC("B16", GPB(16)),
	JZ_PIN_DESC("B17", GPB(17)),
	JZ_PIN_DESC("B18", GPB(18)),
	JZ_PIN_DESC("B19", GPB(19)),
	JZ_PIN_DESC("B20", GPB(20)),
	JZ_PIN_DESC("B21", GPB(21)),
	JZ_PIN_DESC("B22", GPB(22)),
	JZ_PIN_DESC("B23", GPB(23)),
	JZ_PIN_DESC("B24", GPB(24)),
	JZ_PIN_DESC("B25", GPB(25)),
	JZ_PIN_DESC("B26", GPB(26)),
	JZ_PIN_DESC("B27", GPB(27)),
	JZ_PIN_DESC("B28", GPB(28)),
	JZ_PIN_DESC("B29", GPB(29)),
	JZ_PIN_DESC("B30", GPB(30)),
	JZ_PIN_DESC("B31", GPB(31)),

	/* Port C */
	JZ_PIN_DESC("C00", GPC(0)),
	JZ_PIN_DESC("C01", GPC(1)),
	JZ_PIN_DESC("C02", GPC(2)),
	JZ_PIN_DESC("C03", GPC(3)),
	JZ_PIN_DESC("C04", GPC(4)),
	JZ_PIN_DESC("C05", GPC(5)),
	JZ_PIN_DESC("C06", GPC(6)),
	JZ_PIN_DESC("C07", GPC(7)),
	JZ_PIN_DESC("C08", GPC(8)),
	JZ_PIN_DESC("C09", GPC(9)),
	JZ_PIN_DESC("C10", GPC(10)),
	JZ_PIN_DESC("C11", GPC(11)),
	JZ_PIN_DESC("C12", GPC(12)),
	JZ_PIN_DESC("C13", GPC(13)),
	JZ_PIN_DESC("C14", GPC(14)),
	JZ_PIN_DESC("C15", GPC(15)),
	JZ_PIN_DESC("C16", GPC(16)),
	JZ_PIN_DESC("C17", GPC(17)),
	JZ_PIN_DESC("C18", GPC(18)),
	JZ_PIN_DESC("C19", GPC(19)),
	JZ_PIN_DESC("C20", GPC(20)),
	JZ_PIN_DESC("C21", GPC(21)),
	JZ_PIN_DESC("C22", GPC(22)),
	JZ_PIN_DESC("C23", GPC(23)),
	JZ_PIN_DESC("C24", GPC(24)),
	JZ_PIN_DESC("C25", GPC(25)),
	JZ_PIN_DESC("C26", GPC(26)),
	JZ_PIN_DESC("C27", GPC(27)),
	JZ_PIN_DESC("C28", GPC(28)),
	JZ_PIN_DESC("C29", GPC(29)),
	JZ_PIN_DESC("C30", GPC(30)),
	JZ_PIN_DESC("C31", GPC(31)),

	/* Port D */
	JZ_PIN_DESC("D00", GPD(0)),
	JZ_PIN_DESC("D01", GPD(1)),
	JZ_PIN_DESC("D02", GPD(2)),
	JZ_PIN_DESC("D03", GPD(3)),
	JZ_PIN_DESC("D04", GPD(4)),
	JZ_PIN_DESC("D05", GPD(5)),
	JZ_PIN_DESC("D06", GPD(6)),
	JZ_PIN_DESC("D07", GPD(7)),
	JZ_PIN_DESC("D08", GPD(8)),
	JZ_PIN_DESC("D09", GPD(9)),
	JZ_PIN_DESC("D10", GPD(10)),
	JZ_PIN_DESC("D11", GPD(11)),
	JZ_PIN_DESC("D12", GPD(12)),
	JZ_PIN_DESC("D13", GPD(13)),
	JZ_PIN_DESC("D14", GPD(14)),
	JZ_PIN_DESC("D15", GPD(15)),
	JZ_PIN_DESC("D16", GPD(16)),
	JZ_PIN_DESC("D17", GPD(17)),
	JZ_PIN_DESC("D18", GPD(18)),
	JZ_PIN_DESC("D19", GPD(19)),
	JZ_PIN_DESC("D20", GPD(20)),
	JZ_PIN_DESC("D21", GPD(21)),
	JZ_PIN_DESC("D22", GPD(22)),
	JZ_PIN_DESC("D23", GPD(23)),
	JZ_PIN_DESC("D24", GPD(24)),
	JZ_PIN_DESC("D25", GPD(25)),
	JZ_PIN_DESC("D26", GPD(26)),
	JZ_PIN_DESC("D27", GPD(27)),
	JZ_PIN_DESC("D28", GPD(28)),
	JZ_PIN_DESC("D29", GPD(29)),
	JZ_PIN_DESC("D30", GPD(30)),
	JZ_PIN_DESC("D31", GPD(31)),

	/* Port E */
	JZ_PIN_DESC("E00", GPE(0)),
	JZ_PIN_DESC("E01", GPE(1)),
	JZ_PIN_DESC("E02", GPE(2)),
	JZ_PIN_DESC("E03", GPE(3)),
	JZ_PIN_DESC("E04", GPE(4)),
	JZ_PIN_DESC("E05", GPE(5)),
	JZ_PIN_DESC("E06", GPE(6)),
	JZ_PIN_DESC("E07", GPE(7)),
	JZ_PIN_DESC("E08", GPE(8)),
	JZ_PIN_DESC("E09", GPE(9)),
	JZ_PIN_DESC("E10", GPE(10)),
	JZ_PIN_DESC("E11", GPE(11)),
	JZ_PIN_DESC("E12", GPE(12)),
	JZ_PIN_DESC("E13", GPE(13)),
	JZ_PIN_DESC("E14", GPE(14)),
	JZ_PIN_DESC("E15", GPE(15)),
	JZ_PIN_DESC("E16", GPE(16)),
	JZ_PIN_DESC("E17", GPE(17)),
	JZ_PIN_DESC("E18", GPE(18)),
	JZ_PIN_DESC("E19", GPE(19)),
	JZ_PIN_DESC("E20", GPE(20)),
	JZ_PIN_DESC("E21", GPE(21)),
	JZ_PIN_DESC("E22", GPE(22)),
	JZ_PIN_DESC("E23", GPE(23)),
	JZ_PIN_DESC("E24", GPE(24)),
	JZ_PIN_DESC("E25", GPE(25)),
	JZ_PIN_DESC("E26", GPE(26)),
	JZ_PIN_DESC("E27", GPE(27)),
	JZ_PIN_DESC("E28", GPE(28)),
	JZ_PIN_DESC("E29", GPE(29)),
	JZ_PIN_DESC("E30", GPE(30)),
	JZ_PIN_DESC("E31", GPE(31)),

	/* Port F */
	JZ_PIN_DESC("F00", GPF(0)),
	JZ_PIN_DESC("F01", GPF(1)),
	JZ_PIN_DESC("F02", GPF(2)),
	JZ_PIN_DESC("F03", GPF(3)),
	JZ_PIN_DESC("F04", GPF(4)),
	JZ_PIN_DESC("F05", GPF(5)),
	JZ_PIN_DESC("F06", GPF(6)),
	JZ_PIN_DESC("F07", GPF(7)),
	JZ_PIN_DESC("F08", GPF(8)),
	JZ_PIN_DESC("F09", GPF(9)),
	JZ_PIN_DESC("F10", GPF(10)),
	JZ_PIN_DESC("F11", GPF(11)),
	JZ_PIN_DESC("F12", GPF(12)),
	JZ_PIN_DESC("F13", GPF(13)),
	JZ_PIN_DESC("F14", GPF(14)),
	JZ_PIN_DESC("F15", GPF(15)),
	JZ_PIN_DESC("F16", GPF(16)),
	JZ_PIN_DESC("F17", GPF(17)),
	JZ_PIN_DESC("F18", GPF(18)),
	JZ_PIN_DESC("F19", GPF(19)),
	JZ_PIN_DESC("F20", GPF(20)),
	JZ_PIN_DESC("F21", GPF(21)),
	JZ_PIN_DESC("F22", GPF(22)),
#undef JZ_PIN_DESC
};

static const unsigned int jz_pulldown_pins[] = {
	GPB(0),  GPB(1),  GPB(10), GPB(11),
	GPB(16), GPB(17), GPB(18), GPB(19),
	GPD(12), GPD(13), GPD(15), GPE(0),
	GPE(1),  GPE(7),  GPE(10), GPF(4),
	GPF(5),  GPF(6),  GPF(7),  GPF(8),
	GPF(9),  GPF(10), GPF(11), GPF(16),
	GPF(17), GPF(19), GPF(20), GPF(22),
};

static const unsigned int msc0_4bit_pins[] = {
	GPA(18), GPA(19), GPA(20), GPA(21), GPA(22), GPA(23),
};
static const u8           msc0_4bit_sels[] = {
	1,       1,       0,       1,       1,       1,
};

static const unsigned int msc1_4bit_pins[] = {
	GPD(20), GPD(21), GPD(22), GPD(23), GPD(24), GPD(25),
};
static const u8           msc1_4bit_sels[] = {
	0,       0,       0,       0,       0,       0,
};

static const unsigned int msc2_4bit_pins[] = {
	GPB(20), GPB(21), GPB(28), GPB(29), GPB(30), GPB(31),
};
static const u8           msc2_4bit_sels[] = {
	0,       0,       0,       0,       0,       0,
};

static const unsigned int msc_8bit_pins[] = {
	GPE(20), GPE(21), GPE(22), GPE(23), GPE(24),
	GPE(25), GPE(26), GPE(27), GPE(28), GPE(29),
};
static const u8           msc0_8bit_sels[] = {
	0,       0,       0,       0,       0,
	0,       0,       0,       0,       0,
};
static const u8           msc1_8bit_sels[] = {
	1,       1,       1,       1,       1,
	1,       1,       1,       1,       1,
};
static const u8           msc2_8bit_sels[] = {
	2,       2,       2,       2,       2,
	2,       2,       2,       2,       2,
};
#define msc0_8bit_pins msc_8bit_pins
#define msc1_8bit_pins msc_8bit_pins
#define msc2_8bit_pins msc_8bit_pins

static const unsigned int uart0_pins[] = { GPF( 0), GPF( 1), GPF( 2), GPF( 3), };
static const u8           uart0_sels[] = { 0,       0,       0,       0,       };
static const unsigned int uart1_pins[] = { GPD(26), GPD(27), GPD(28), GPD(29), };
static const u8           uart1_sels[] = { 0,       0,       0,       0,       };
static const unsigned int uart2_pins[] = { GPC(28), GPC(29), GPC(30), GPC(31), };
static const u8           uart2_sels[] = { 0,       0,       0,       0,       };
static const unsigned int uart3_pins[] = { GPD(12), GPE( 5), GPE( 8), GPE( 9), };
static const u8           uart3_sels[] = { 0,       1,       0,       0,       };

static const unsigned int i2c0_pins[] = { GPD(30), GPD(31), };
static const u8           i2c0_sels[] = { 0,       0,       };
static const unsigned int i2c1_pins[] = { GPE(30), GPE(31), };
static const u8           i2c1_sels[] = { 0,       0,       };
static const unsigned int i2c2_pins[] = { GPD(4), GPD(5), };
static const u8           i2c2_sels[] = { 2,       2,       };

static const unsigned int pwm0_pins[] = { GPE(0),  };
static const unsigned int pwm1_pins[] = { GPE(1),  };
static const unsigned int pwm2_pins[] = { GPE(2),  };
static const unsigned int pwm3_pins[] = { GPE(3),  };
static const unsigned int pwm4_pins[] = { GPE(4),  };
static const unsigned int pwm5_pins[] = { GPE(5),  };
static const unsigned int pwm6_pins[] = { GPD(10), };
static const unsigned int pwm7_pins[] = { GPD(11), };
static const u8           pwm_sels[]  = { 0,       };
#define pwm0_sels pwm_sels
#define pwm1_sels pwm_sels
#define pwm2_sels pwm_sels
#define pwm3_sels pwm_sels
#define pwm4_sels pwm_sels
#define pwm5_sels pwm_sels
#define pwm6_sels pwm_sels
#define pwm7_sels pwm_sels

static const unsigned int otg_pins[] = { GPE(10), };
static const u8           otg_sels[] = { 0,       };

static const unsigned int lcd_pins[] = {
	/* PCLK, DE, HSYNC, VSYNC */
	GPC( 8), GPC( 9), GPC(18), GPC(19),
	/* B0..B4 */
	GPC( 0), GPC( 1), GPC( 2), GPC( 3), GPC( 4),
	/* G0..G5 */
	GPC(10), GPC(11), GPC(12), GPC(13), GPC(14), GPC(15),
	/* R0..R4 */
	GPC(20), GPC(21), GPC(22), GPC(23), GPC(24),
	/* 18 bpp: B5, R5 */
	GPC( 5), GPC(25),
	/* 24 bpp: B6/7, G6/7, R6/7 */
	GPC( 6), GPC( 7), GPC(16), GPC(17), GPC(26), GPC(27),
};
static const u8 lcd_sels[ARRAY_SIZE(lcd_pins)] = { 0 };

static const struct jz_pin_group jz_pin_groups[] = {
#define JZ_GROUP(group) { \
	.name = #group, \
	.pins = group##_pins, \
	.sels = group##_sels, \
	.num_pins = ARRAY_SIZE(group##_pins), \
}
	JZ_GROUP(msc0_4bit),
	JZ_GROUP(msc1_4bit),
	JZ_GROUP(msc2_4bit),
	JZ_GROUP(msc0_8bit),
	JZ_GROUP(msc1_8bit),
	JZ_GROUP(msc2_8bit),
	JZ_GROUP(uart0),
	JZ_GROUP(uart1),
	JZ_GROUP(uart2),
	JZ_GROUP(uart3),
	JZ_GROUP(i2c0),
	JZ_GROUP(i2c1),
	JZ_GROUP(i2c2),
	JZ_GROUP(pwm0),
	JZ_GROUP(pwm1),
	JZ_GROUP(pwm2),
	JZ_GROUP(pwm3),
	JZ_GROUP(pwm4),
	JZ_GROUP(pwm5),
	JZ_GROUP(pwm6),
	JZ_GROUP(pwm7),
	JZ_GROUP(otg),
	{ "lcd_rgb565", lcd_pins, lcd_sels, 4 + 16 },
	{ "lcd_rgb666", lcd_pins, lcd_sels, 4 + 18 },
	{ "lcd_rgb888", lcd_pins, lcd_sels, 4 + 24 },
	{ "no_pins", NULL, NULL, 0 },
#undef JZ_GROUP
};

static int jz_get_groups_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(jz_pin_groups);
}

static const char *jz_get_group_name(struct pinctrl_dev *pctldev,
		unsigned int group)
{
	return jz_pin_groups[group].name;
}

static int jz_get_group_pins(struct pinctrl_dev *pctldev, unsigned int group,
		const unsigned int **pins,
		unsigned int *num_pins)
{
	*pins = jz_pin_groups[group].pins;
	*num_pins = jz_pin_groups[group].num_pins;
	return 0;
}

static const struct pinctrl_ops jz_pctrl_ops = {
	.get_groups_count = jz_get_groups_count,
	.get_group_name = jz_get_group_name,
	.get_group_pins = jz_get_group_pins,
};

static const char * const msc0_groups[] = { "msc0_4bit", "msc0_8bit", };
static const char * const msc1_groups[] = { "msc1_4bit", "msc1_8bit", };
static const char * const msc2_groups[] = { "msc2_4bit", "msc2_8bit", };
static const char * const uart0_groups[] = { "uart0", };
static const char * const uart1_groups[] = { "uart1", };
static const char * const uart2_groups[] = { "uart2", };
static const char * const uart3_groups[] = { "uart3", };
static const char * const i2c0_groups[] = { "i2c0", };
static const char * const i2c1_groups[] = { "i2c1", };
static const char * const i2c2_groups[] = { "i2c2", };
static const char * const pwm0_groups[] = { "pwm0", };
static const char * const pwm1_groups[] = { "pwm1", };
static const char * const pwm2_groups[] = { "pwm2", };
static const char * const pwm3_groups[] = { "pwm3", };
static const char * const pwm4_groups[] = { "pwm4", };
static const char * const pwm5_groups[] = { "pwm5", };
static const char * const pwm6_groups[] = { "pwm6", };
static const char * const pwm7_groups[] = { "pwm7", };
static const char * const otg_groups[] = { "otg", };
static const char * const lcd_groups[] = {
	"lcd_rgb888", "lcd_rgb666", "lcd_rgb565", "no_pins"
};

static const struct jz_pmux_func jz_functions[] = {
#define JZ_GROUP(group) { \
	.name = #group, \
	.groups = group##_groups, \
	.num_groups = ARRAY_SIZE(group##_groups), \
}
	JZ_GROUP(msc0),
	JZ_GROUP(msc1),
	JZ_GROUP(msc2),
	JZ_GROUP(uart0),
	JZ_GROUP(uart1),
	JZ_GROUP(uart2),
	JZ_GROUP(uart3),
	JZ_GROUP(i2c0),
	JZ_GROUP(i2c1),
	JZ_GROUP(i2c2),
	JZ_GROUP(pwm0),
	JZ_GROUP(pwm1),
	JZ_GROUP(pwm2),
	JZ_GROUP(pwm3),
	JZ_GROUP(pwm4),
	JZ_GROUP(pwm5),
	JZ_GROUP(pwm6),
	JZ_GROUP(pwm7),
	JZ_GROUP(otg),
	JZ_GROUP(lcd),
#undef JZ_GROUP
};

int jz_get_functions_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(jz_functions);
}

const char *jz_get_fname(struct pinctrl_dev *pctrldev, unsigned int func)
{
	return jz_functions[func].name;
}

static int jz_get_groups(struct pinctrl_dev *pctrldev, unsigned int func,
		const char * const **groups, unsigned int * const num_groups)
{
	*groups = jz_functions[func].groups;
	*num_groups = jz_functions[func].num_groups;
	return 0;
}

static void jz_set_gpio_input(struct pinctrl_dev *pctldev, unsigned int pin)
{
	struct jz_pinctrl *jz = pinctrl_dev_get_drvdata(pctldev);
	const unsigned int bit = BIT(PORT_PIN(pin)), port = PORT(pin);
	unsigned long flags;

	spin_lock_irqsave(&jz->lock, flags);
	writel(bit, jz->base + INT_CLR_OFFSET(port));
	writel(bit, jz->base + MASK_SET_OFFSET(port));
	writel(bit, jz->base + PAT1_SET_OFFSET(port));
	spin_unlock_irqrestore(&jz->lock, flags);
}

static void jz_set_gpio_output(struct pinctrl_dev *pctldev, unsigned int pin)
{
	struct jz_pinctrl *jz = pinctrl_dev_get_drvdata(pctldev);
	const unsigned int bit = BIT(PORT_PIN(pin)), port = PORT(pin);
	unsigned long flags;

	spin_lock_irqsave(&jz->lock, flags);
	writel(bit, jz->base + INT_CLR_OFFSET(port));
	writel(bit, jz->base + MASK_SET_OFFSET(port));
	writel(bit, jz->base + PAT1_CLR_OFFSET(port));
	spin_unlock_irqrestore(&jz->lock, flags);
}

static void jz_set_function(struct pinctrl_dev *pctldev,
		unsigned int pin, u8 sel)
{
	struct jz_pinctrl *jz = pinctrl_dev_get_drvdata(pctldev);
	const unsigned int bit = BIT(PORT_PIN(pin)), port = PORT(pin);
	unsigned long flags;

	spin_lock_irqsave(&jz->lock, flags);

	writel(bit, jz->base + INT_CLR_OFFSET(port));
	writel(bit, jz->base + MASK_CLR_OFFSET(port));
	if (sel & BIT(0))
		writel(bit, jz->base + PAT0_SET_OFFSET(port));
	else
		writel(bit, jz->base + PAT0_CLR_OFFSET(port));
	if (sel & BIT(1))
		writel(bit, jz->base + PAT1_SET_OFFSET(port));
	else
		writel(bit, jz->base + PAT1_CLR_OFFSET(port));

	spin_unlock_irqrestore(&jz->lock, flags);
}

static int jz_enable(struct pinctrl_dev *pctrldev,
		unsigned int func, unsigned int group)
{
	const struct jz_pin_group *pin_group = &jz_pin_groups[group];
	unsigned int i;

	for (i = 0; i < pin_group->num_pins; i++)
		jz_set_function(pctrldev, pin_group->pins[i],
					  pin_group->sels[i]);

	return 0;
}

static void jz_disable(struct pinctrl_dev *pctldev,
		unsigned int func, unsigned int group)
{
	const struct jz_pin_group *pin_group = &jz_pin_groups[group];
	unsigned int i;

	for (i = 0; i < pin_group->num_pins; i++)
		jz_set_gpio_input(pctldev, pin_group->pins[i]);
}

static int jz_pmux_gpio_set_direction(struct pinctrl_dev *pctldev,
		struct pinctrl_gpio_range *range,
		unsigned int offset, bool input)
{
	if (input)
		jz_set_gpio_input(pctldev, offset);
	else
		jz_set_gpio_output(pctldev, offset);
	return 0;
}

static const struct pinmux_ops jz_pmux_ops = {
	.get_functions_count = jz_get_functions_count,
	.get_function_name = jz_get_fname,
	.get_function_groups = jz_get_groups,
	.enable = jz_enable,
	.disable = jz_disable,
	.gpio_set_direction = jz_pmux_gpio_set_direction,
};

static bool jz_pin_is_pulldown(unsigned int pin)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(jz_pulldown_pins); i++)
		if (jz_pulldown_pins[i] == pin)
			return true;
	return false;
}

static void jz_pin_disable_pullup_pulldown(struct jz_pinctrl *jz,
		unsigned int pin)
{
	writel(BIT(PORT_PIN(pin)), jz->base + PULL_SET_OFFSET(PORT(pin)));
}

static void jz_pin_enable_pullup_pulldown(struct jz_pinctrl *jz,
		unsigned int pin)
{
	writel(BIT(PORT_PIN(pin)), jz->base + PULL_CLR_OFFSET(PORT(pin)));
}

static bool jz_pin_is_pullup_pulldown_enabled(struct jz_pinctrl *jz,
		unsigned int pin)
{
	u32 val = readl(jz->base + PULL_OFFSET(PORT(pin)));
	return !(val & BIT(PORT_PIN(pin)));
}

static int jz_pin_config_get(struct pinctrl_dev *pctldev,
		unsigned int pin, unsigned long *config)
{
	struct jz_pinctrl *jz = pinctrl_dev_get_drvdata(pctldev);
	enum pin_config_param param = pinconf_to_config_param(*config);
	u16 param_val = pinconf_to_config_argument(*config);

	switch (param) {
		case PIN_CONFIG_BIAS_PULL_UP:
			if (jz_pin_is_pulldown(pin))
				return -EINVAL;
			param_val = jz_pin_is_pullup_pulldown_enabled(jz, pin);
			break;

		case PIN_CONFIG_BIAS_PULL_DOWN:
			if (!jz_pin_is_pulldown(pin))
				return -EINVAL;
			param_val = jz_pin_is_pullup_pulldown_enabled(jz, pin);
			break;

		case PIN_CONFIG_BIAS_PULL_PIN_DEFAULT:
			param_val = jz_pin_is_pullup_pulldown_enabled(jz, pin);
			break;

		default:
			return -ENOTSUPP;
	};

	*config = pinconf_to_config_packed(param, param_val);
	return 0;
}

static int jz_pin_config_set(struct pinctrl_dev *pctldev,
		unsigned int pin, unsigned long *configs,
		unsigned int num_configs)
{
	struct jz_pinctrl *jz = pinctrl_dev_get_drvdata(pctldev);
	unsigned int i;
	enum pin_config_param param;
	u16 param_val;

	for (i = 0; i < num_configs; i++) {
		param = pinconf_to_config_param(configs[i]);
		param_val = pinconf_to_config_argument(configs[i]);

		switch (param) {
		case PIN_CONFIG_BIAS_PULL_UP:
			if (jz_pin_is_pulldown(pin))
				return -EINVAL;
			jz_pin_enable_pullup_pulldown(jz, pin);
			break;

		case PIN_CONFIG_BIAS_PULL_DOWN:
			if (!jz_pin_is_pulldown(pin))
				return -EINVAL;
			jz_pin_enable_pullup_pulldown(jz, pin);
			break;

		case PIN_CONFIG_BIAS_PULL_PIN_DEFAULT:
			jz_pin_enable_pullup_pulldown(jz, pin);
			break;

		case PIN_CONFIG_BIAS_DISABLE:
			jz_pin_disable_pullup_pulldown(jz, pin);
			break;

		default:
			return -ENOTSUPP;
		}
	}

	return 0;
}

static int jz_pin_config_group_get(struct pinctrl_dev *pctldev,
		unsigned int group, unsigned long *config)
{
	/* TODO */
	return -ENOTSUPP;
}

static int jz_pin_config_group_set(struct pinctrl_dev *pctldev,
		unsigned int group, unsigned long *configs,
		unsigned int num_configs)
{
	unsigned int nb_pins;
	const unsigned int *pins;
	int ret = jz_get_group_pins(pctldev, group, &pins, &nb_pins);

	while (!ret && nb_pins--)
		ret = jz_pin_config_set(pctldev, pins[nb_pins],
				configs, num_configs);
	return ret;
}

static const struct pinconf_ops jz_pconf_ops = {
	.is_generic = true,
	.pin_config_get = jz_pin_config_get,
	.pin_config_set = jz_pin_config_set,
	.pin_config_group_get = jz_pin_config_group_get,
	.pin_config_group_set = jz_pin_config_group_set,
};

static struct pinctrl_desc jz_desc = {
	.name    = "jz4770-pinctrl",
	.pins    = jz_pins,
	.npins   = ARRAY_SIZE(jz_pins),
	.pctlops = &jz_pctrl_ops,
	.pmxops  = &jz_pmux_ops,
	.confops = &jz_pconf_ops,
	.owner   = THIS_MODULE,
};

static int jz4770_pinctrl_probe(struct platform_device *pdev)
{
	struct jz_pinctrl *jz;
	struct resource *res;

	jz = devm_kzalloc(&pdev->dev, sizeof(*jz), GFP_KERNEL);
	if (!jz) {
		dev_err(&pdev->dev, "Unable to allocate memory\n");
		return -ENOMEM;
	}

	jz->dev = &pdev->dev;
	platform_set_drvdata(pdev, jz);
	spin_lock_init(&jz->lock);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	jz->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(jz->base)) {
		dev_err(&pdev->dev, "Failed to remap registers\n");
		return PTR_ERR(jz->base);
	}

	jz->pctl = pinctrl_register(&jz_desc, &pdev->dev, jz);
	if (!jz->pctl) {
		dev_err(&pdev->dev, "Failed to register pinctrl driver\n");
		return -ENODEV;
	}

	dev_dbg(&pdev->dev, "pinctrl driver registered\n");
	return 0;
}

static int jz4770_pinctrl_remove(struct platform_device *pdev)
{
	struct jz_pinctrl *jz = platform_get_drvdata(pdev);
	pinctrl_unregister(jz->pctl);
	return 0;
}

static struct platform_driver jz4770_pinctrl_driver = {
	.driver = {
		.name = "jz4770-pinctrl",
		.owner = THIS_MODULE,
	},
	.probe =  jz4770_pinctrl_probe,
	.remove = jz4770_pinctrl_remove,
};

static int __init jz4770_pinctrl_drv_register(void)
{
	return platform_driver_register(&jz4770_pinctrl_driver);
}
postcore_initcall(jz4770_pinctrl_drv_register);

static void __exit jz4770_pinctrl_drv_unregister(void)
{
	platform_driver_unregister(&jz4770_pinctrl_driver);
}
module_exit(jz4770_pinctrl_drv_unregister);

MODULE_DESCRIPTION("Ingenic JZ4770 pin control driver");
MODULE_AUTHOR("Paul Cercueil <paul@crapouillou.net>");
MODULE_ALIAS("platform:jz4770-pinctrl");
MODULE_LICENSE("GPL v2");

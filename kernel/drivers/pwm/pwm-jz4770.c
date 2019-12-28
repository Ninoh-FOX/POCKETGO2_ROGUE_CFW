/*
 *  Copyright (C) 2010, Lars-Peter Clausen <lars@metafoo.de>
 *  Copyright (C) 2014, Paul Cercueil <paul@crapouillou.net>
 *  JZ4770 platform PWM support
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under  the terms of the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/clk.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>

#define NUM_PWM 8

#define TCU_TSR_OFFSET	0x0C /* Timer Stop register */
#define TCU_TSSR_OFFSET	0x1C
#define TCU_TSCR_OFFSET	0x2C

#define TCU_TER_OFFSET	0x00 /* Timer Counter Enable register */
#define TCU_TESR_OFFSET	0x04
#define TCU_TECR_OFFSET	0x08

#define TCU_TDFR_OFFSET(pwm)	(0x30 + (pwm) * 0x10) /* Timer Data Full reg */
#define TCU_TDHR_OFFSET(pwm)	(0x34 + (pwm) * 0x10) /* Timer Data Half reg */
#define TCU_TCNT_OFFSET(pwm)	(0x38 + (pwm) * 0x10) /* Timer Counter reg */
#define TCU_TCSR_OFFSET(pwm)	(0x3C + (pwm) * 0x10) /* Timer Control reg */

#define TCU_TCSR_PWM_SD		BIT(9)  /* Shutdown: 0=gracefully, 1=abruptly */
#define TCU_TCSR_PWM_INITL_HIGH	BIT(8)  /* Sets the initial output level */
#define TCU_TCSR_PWM_EN		BIT(7)  /* PWM pin output enable */
#define TCU_TCSR_PRESCALE_MASK	0x0038
#define TCU_TCSR_PRESCALE_SHIFT	3

struct jz4770_pwm_chip {
	struct pwm_chip chip;
	void __iomem *base;
	struct clk *clk;
};

static inline struct jz4770_pwm_chip *to_jz4770(struct pwm_chip *chip)
{
	return container_of(chip, struct jz4770_pwm_chip, chip);
}

static int jz4770_pwm_request(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct jz4770_pwm_chip *jz = to_jz4770(chip);

	/* Start clock */
	writel(BIT(pwm->hwpwm), jz->base + TCU_TSCR_OFFSET);
	return 0;
}

static void jz4770_pwm_free(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct jz4770_pwm_chip *jz = to_jz4770(chip);

	/* Stop clock */
	writel(BIT(pwm->hwpwm), jz->base + TCU_TSSR_OFFSET);
}

static int jz4770_pwm_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct jz4770_pwm_chip *jz = to_jz4770(chip);
	u16 reg;

	/* Enable PWM output */
	reg = readw(jz->base + TCU_TCSR_OFFSET(pwm->hwpwm));
	writew(reg | TCU_TCSR_PWM_EN, jz->base + TCU_TCSR_OFFSET(pwm->hwpwm));

	/* Start counter */
	writew(BIT(pwm->hwpwm), jz->base + TCU_TESR_OFFSET);
	return 0;
}

static void jz4770_pwm_disable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct jz4770_pwm_chip *jz = to_jz4770(chip);
	u16 reg;

	/* Stop counter */
	writew(BIT(pwm->hwpwm), jz->base + TCU_TECR_OFFSET);

	/* Disable PWM output */
	reg = readw(jz->base + TCU_TCSR_OFFSET(pwm->hwpwm));
	writew(reg & ~TCU_TCSR_PWM_EN, jz->base + TCU_TCSR_OFFSET(pwm->hwpwm));
}

static bool tcu_counter_enabled(struct jz4770_pwm_chip *jz, unsigned int pwm)
{
	return readw(jz->base + TCU_TER_OFFSET) & BIT(pwm);
}

static int jz4770_pwm_config(struct pwm_chip *chip, struct pwm_device *pwm,
			     int duty_ns, int period_ns)
{
	struct jz4770_pwm_chip *jz = to_jz4770(pwm->chip);
	unsigned long long tmp;
	unsigned long period, duty;
	unsigned int prescaler = 0;
	bool is_enabled;
	u16 reg;

	tmp = (unsigned long long) clk_get_rate(jz->clk) * period_ns;
	do_div(tmp, 1000000000);
	period = tmp;

	while (period > 0xffff && prescaler < 6) {
		period >>= 2;
		++prescaler;
	}

	if (prescaler == 6)
		return -EINVAL;

	tmp = (unsigned long long)period * duty_ns;
	do_div(tmp, period_ns);
	duty = period - tmp;

	if (duty >= period)
		duty = period - 1;

	is_enabled = tcu_counter_enabled(jz, pwm->hwpwm);
	if (is_enabled)
		jz4770_pwm_disable(chip, pwm);

	/* Set abrupt shutdown, clock divider */
	reg = readw(jz->base + TCU_TCSR_OFFSET(pwm->hwpwm));
	reg &= ~TCU_TCSR_PRESCALE_MASK;
	reg |= TCU_TCSR_PWM_SD | (prescaler << TCU_TCSR_PRESCALE_SHIFT);
	writew(reg, jz->base + TCU_TCSR_OFFSET(pwm->hwpwm));

	/* Reset counter to 0 */
	writew(0, jz->base + TCU_TCNT_OFFSET(pwm->hwpwm));

	/* Set duty */
	writew(duty, jz->base + TCU_TDHR_OFFSET(pwm->hwpwm));

	/* Set period */
	writew(period, jz->base + TCU_TDFR_OFFSET(pwm->hwpwm));

	if (is_enabled)
		jz4770_pwm_enable(chip, pwm);

	return 0;
}

static int jz4770_pwm_set_polarity(struct pwm_chip *chip,
		struct pwm_device *pwm, enum pwm_polarity polarity)
{
	struct jz4770_pwm_chip *jz = to_jz4770(chip);
	u16 reg = readw(jz->base + TCU_TCSR_OFFSET(pwm->hwpwm));

	/* Note: On disable, output becomes initial state. */
	switch (polarity) {
	case PWM_POLARITY_NORMAL:
		reg &= ~TCU_TCSR_PWM_INITL_HIGH;
		break;
	case PWM_POLARITY_INVERSED:
		reg |= TCU_TCSR_PWM_INITL_HIGH;
		break;
	}

	writew(reg, jz->base + TCU_TCSR_OFFSET(pwm->hwpwm));
	return 0;
}

static const struct pwm_ops jz4770_pwm_ops = {
	.request = jz4770_pwm_request,
	.free = jz4770_pwm_free,
	.config = jz4770_pwm_config,
	.set_polarity = jz4770_pwm_set_polarity,
	.enable = jz4770_pwm_enable,
	.disable = jz4770_pwm_disable,
	.owner = THIS_MODULE,
};

static int jz4770_pwm_probe(struct platform_device *pdev)
{
	struct jz4770_pwm_chip *jz4770;
	struct resource *res;
	int ret;

	jz4770 = devm_kzalloc(&pdev->dev, sizeof(*jz4770), GFP_KERNEL);
	if (!jz4770)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	jz4770->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(jz4770->base))
		return PTR_ERR(jz4770->base);

	jz4770->clk = clk_get(NULL, "ext");
	if (IS_ERR(jz4770->clk))
		return PTR_ERR(jz4770->clk);

	jz4770->chip.dev = &pdev->dev;
	jz4770->chip.ops = &jz4770_pwm_ops;
	jz4770->chip.npwm = NUM_PWM;
	jz4770->chip.base = -1;

	ret = pwmchip_add(&jz4770->chip);
	if (ret < 0) {
		clk_put(jz4770->clk);
		return ret;
	}

	platform_set_drvdata(pdev, jz4770);

	return 0;
}

static int jz4770_pwm_remove(struct platform_device *pdev)
{
	struct jz4770_pwm_chip *jz4770 = platform_get_drvdata(pdev);
	int ret;

	ret = pwmchip_remove(&jz4770->chip);
	if (ret < 0)
		return ret;

	clk_put(jz4770->clk);

	return 0;
}

static struct platform_driver jz4770_pwm_driver = {
	.driver = {
		.name = "jz4770-pwm",
		.owner = THIS_MODULE,
	},
	.probe = jz4770_pwm_probe,
	.remove = jz4770_pwm_remove,
};
module_platform_driver(jz4770_pwm_driver);

MODULE_AUTHOR("Paul Cercueil <paul@crapouillou.net>");
MODULE_DESCRIPTION("Ingenic JZ4770 PWM driver");
MODULE_ALIAS("platform:jz4770-pwm");
MODULE_LICENSE("GPL");

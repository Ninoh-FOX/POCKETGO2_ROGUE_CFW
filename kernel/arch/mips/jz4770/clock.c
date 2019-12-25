/*
 *  Copyright (C) 2010, Lars-Peter Clausen <lars@metafoo.de>
 *  Copyright (C) 2012, Paul Cercueil <paul@crapouillou.net>
 *  JZ4770 SoC clock support
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

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/err.h>

#include <asm/mach-jz4770/base.h>
#include <asm/mach-jz4770/jz4770cpm.h>

#include "clock.h"

#define JZ_CLK_DIVIDED_NO_EXT ((uint32_t)-1)

#define TCU_TSR_OFFSET  0x0C /* Timer stop (r) */
#define TCU_TSSR_OFFSET 0x1C /* Timer stop set (w) */
#define TCU_TSCR_OFFSET 0x2C /* Timer stop clear (w) */
#define TCU_TCSR_OFFSET(timer) (0x3C + (timer) * 0x10) /* Timer Control reg. */

#define WDT_TCSR_OFFSET 0xC
#define OST_OSTCSR_OFFSET 0xEC

static void __iomem *jz_clock_base, __iomem *tcu_base,
	    __iomem *wdt_tcsr, __iomem *ost_tcsr;
static spinlock_t jz_clock_lock;
static LIST_HEAD(jz_clk_list);

struct main_clk {
	struct clk clk;
	uint32_t div_offset;
};

struct divided_clk {
	struct clk clk;
	uint32_t reg;
	uint32_t mask;
	uint32_t pll_mask; /* Bit(s) to toggle PLL0 / PLL1 */
	uint32_t ext_mask; /* Bit(s) to toggle EXT / PLL */
};

struct static_clk {
	struct clk clk;
	unsigned long rate;
};

struct tcu_clk {
	struct clk clk;
	void * __iomem *base_ptr;
	unsigned int reg;
};

static uint32_t jz_clk_reg_read(int reg)
{
	return readl(jz_clock_base + reg);
}

static void jz_clk_reg_write(uint32_t val, int reg)
{
	writel(val, jz_clock_base + reg);
}


static void jz_clk_reg_set_or_clear_bits(int reg, uint32_t mask, int set)
{
	uint32_t val;

	spin_lock(&jz_clock_lock);
	val = jz_clk_reg_read(reg);
	if (set)
		val |= mask;
	else
		val &= ~mask;
	jz_clk_reg_write(val, reg);
	spin_unlock(&jz_clock_lock);
}

static inline void jz_clk_reg_set_bits(int reg, uint32_t mask)
{
	jz_clk_reg_set_or_clear_bits(reg, mask, 1);
}

static inline void jz_clk_reg_clear_bits(int reg, uint32_t mask)
{
	jz_clk_reg_set_or_clear_bits(reg, mask, 0);
}

static void jz_clk_reg_write_mask(int reg, uint32_t val, uint32_t mask)
{
	jz_clk_reg_set_or_clear_bits(reg, mask, 0);
	jz_clk_reg_set_or_clear_bits(reg, val, 1);
}

static int jz_clk_enable_gating(struct clk *clk)
{
	if (clk->gate_bit == JZ_CLK_NOT_GATED)
		return -EINVAL;

	jz_clk_reg_clear_bits(clk->gate_register, clk->gate_bit);
	return 0;
}

static int jz_clk_enable_gating_msc(struct clk *clk)
{
	struct divided_clk *dclk = (struct divided_clk *)clk;
	int ret;

	ret = jz_clk_enable_gating(clk);
	if (ret)
		return ret;

	jz_clk_reg_clear_bits(dclk->reg, MSCCDR_MCSG);

	return 0;
}

static int jz_clk_disable_gating(struct clk *clk)
{
	if (clk->gate_bit == JZ_CLK_NOT_GATED)
		return -EINVAL;

	jz_clk_reg_set_bits(clk->gate_register, clk->gate_bit);
	return 0;
}

static int jz_clk_is_enabled_gating(struct clk *clk)
{
	if (clk->gate_bit == JZ_CLK_NOT_GATED)
		return 1;

	return !(jz_clk_reg_read(clk->gate_register) & clk->gate_bit);
}

static unsigned long jz_clk_pll0_get_rate(struct clk *clk)
{
	uint32_t val;
	int m, n, od;

	val = jz_clk_reg_read(CPM_CPPSR_OFFSET);
	if (val & CPPSR_PLLBP || val & CPPSR_PLLOFF)
		return clk_get_rate(clk->parent);

	val = jz_clk_reg_read(CPM_CPPCR0_OFFSET);
	m = ((val & CPPCR0_PLLM_MASK) >> CPPCR0_PLLM_LSB) + 1;
	n = ((val & CPPCR0_PLLN_MASK) >> CPPCR0_PLLN_LSB) + 1;
	od = (val & CPPCR0_PLLOD_MASK) >> CPPCR0_PLLOD_LSB;

	return ((clk_get_rate(clk->parent) / n) * m) >> od;
}

static unsigned long jz_clk_pll0_half_get_rate(struct clk *clk)
{
	uint32_t reg;

	reg = jz_clk_reg_read(CPM_CPCCR_OFFSET);
	if (reg & CPCCR_PCS)
		return jz_clk_pll0_get_rate(clk->parent) >> 1;
	return jz_clk_pll0_get_rate(clk->parent);
}

static unsigned long jz_clk_pll1_get_rate(struct clk *clk)
{
	uint32_t val;
	int m, n, od;

	val = jz_clk_reg_read(CPM_CPPCR1_OFFSET);
	if (val & CPPCR1_PLL1OFF)
		return clk_get_rate(clk->parent);

	m = ((val & CPPCR1_PLL1M_MASK) >> CPPCR1_PLL1M_LSB) + 1;
	n = ((val & CPPCR1_PLL1N_MASK) >> CPPCR1_PLL1N_LSB) + 1;
	od = (val & CPPCR1_PLL1OD_MASK) >> CPPCR1_PLL1OD_LSB;

	return ((clk_get_rate(clk->parent) / n) * m) >> od;
}

#define FVCO_MIN  300*1000*1000	/* 300 MHz */
#define FVCO_MAX  1080*1000*1000	/* 1,1 GHz */

/* The frequency after the input divider must be between 10 and 50 MHz.
The highest divider yields the best resolution. */
#define INDIV(ext) (ext / 10000000)

#define OUTDIV(f) \
  (((f) >= FVCO_MIN) ? 1 : \
   ((f) >= (FVCO_MIN / 2)) ? 2 : \
   ((f) >= (FVCO_MIN / 4)) ? 4 : 8)

#define RATE(f) \
  ((f) < (FVCO_MIN / 8) ? FVCO_MIN : \
	(((f) > FVCO_MAX) ? FVCO_MAX : (f)))

#define FEEDBACK(f, ext) \
  ((((RATE(f) * OUTDIV(f)) / 1000) * INDIV(ext)) / (ext / 1000))

#define BS(f) ((f) > (FVCO_MAX / 2))

static int jz_clk_pll1_set_rate(struct clk *clk, unsigned long rate)
{
	unsigned long parent_rate = clk_get_rate(clk->parent);
	uint32_t val = ((FEEDBACK(rate, parent_rate) - 1) << CPPCR1_PLL1M_LSB) |
			((INDIV(parent_rate) - 1) << CPPCR1_PLL1N_LSB) |
			((OUTDIV(rate) - 1) << CPPCR1_PLL1OD_LSB);
	if (BS(rate))
		val |= CPPCR1_PLL1BS;

	jz_clk_reg_write_mask(CPM_CPPCR1_OFFSET, val,
				CPPCR1_PLL1M_MASK | CPPCR1_PLL1N_MASK |
				CPPCR1_PLL1OD_MASK | CPPCR1_PLL1BS);
	return 0;
}

static int jz_clk_pll1_enable(struct clk *clk)
{
	jz_clk_reg_set_bits(CPM_CPPCR1_OFFSET, CPPCR1_PLL1EN);

	/* Wait for a stable output... */
	while (!(jz_clk_reg_read(CPM_CPPCR1_OFFSET) & CPPCR1_PLL1S));
	return 0;
}

static int jz_clk_pll1_disable(struct clk *clk)
{
	jz_clk_reg_clear_bits(CPM_CPPCR1_OFFSET, CPPCR1_PLL1EN);
	return 0;
}

static int jz_clk_pll1_is_enabled(struct clk *clk)
{
	return !!(jz_clk_reg_read(CPM_CPPCR1_OFFSET) & CPPCR1_PLL1EN);
}

static const int jz_clk_main_divs[] = {1, 2, 3, 4, 6, 8, 12};

static unsigned long jz_clk_main_round_rate(struct clk *clk, unsigned long rate)
{
	unsigned long parent_rate = jz_clk_pll0_get_rate(clk->parent);
	int div;

	div = parent_rate / rate;
	/* round divider up => round rate down */
	if (parent_rate > div * rate)
		div++;

	if (div > 12)
		return parent_rate / 12;
	else if (div < 1)
		return parent_rate;

	div &= (0x3 << (ffs(div) - 1));

	return parent_rate / div;
}

static unsigned long jz_clk_main_get_rate(struct clk *clk)
{
	struct main_clk *mclk = (struct main_clk *)clk;
	uint32_t div;

	div = jz_clk_reg_read(CPM_CPCCR_OFFSET);

	div >>= mclk->div_offset;
	div &= 0xf;

	if (div >= ARRAY_SIZE(jz_clk_main_divs))
		div = ARRAY_SIZE(jz_clk_main_divs) - 1;

	return jz_clk_pll0_get_rate(clk->parent) / jz_clk_main_divs[div];
}

static int jz_clk_main_set_rate(struct clk *clk, unsigned long rate)
{
	struct main_clk *mclk = (struct main_clk *)clk;
	int div;
	unsigned long parent_rate = jz_clk_pll0_get_rate(clk->parent);
	unsigned long current_rate = jz_clk_main_get_rate(clk);

	if (rate == current_rate)
		return 0;

	rate = jz_clk_main_round_rate(clk, rate);

	div = parent_rate / rate;

	/* Wait here if previous dividers are still being applied */
	while (!(jz_clk_reg_read(CPM_CPPSR_OFFSET) & CPPSR_FS))
		msleep(1);

	jz_clk_reg_clear_bits(CPM_CPPSR_OFFSET, CPPSR_FS);
	jz_clk_reg_set_bits(CPM_CPPSR_OFFSET, CPPSR_FM);

	jz_clk_reg_write_mask(CPM_CPCCR_OFFSET, (div - 1) << mclk->div_offset,
				0xf << mclk->div_offset);

	return 0;
}

static unsigned long jz_clk_static_get_rate(struct clk *clk)
{
	return ((struct static_clk *)clk)->rate;
}

static struct clk_ops jz_clk_static_ops = {
	.get_rate = jz_clk_static_get_rate,
	.enable = jz_clk_enable_gating,
	.disable = jz_clk_disable_gating,
	.is_enabled = jz_clk_is_enabled_gating,
};

static struct static_clk jz_clk_ext = {
	.clk = {
		.name = "ext",
		.gate_bit = JZ_CLK_NOT_GATED,
		.ops = &jz_clk_static_ops,
	},
};

static struct static_clk jz_clk_rtc = {
	.clk = {
		.name = "rtc",
		.gate_bit = JZ_CLK_NOT_GATED,
		.ops = &jz_clk_static_ops,
	},
};

static struct clk_ops jz_clk_pll0_ops = {
	.get_rate = jz_clk_pll0_get_rate,
};

static struct clk jz_clk_pll0 = {
	.name = "pll0",
	.parent = &jz_clk_ext.clk,
	.ops = &jz_clk_pll0_ops,
};

static struct clk_ops jz_clk_pll0_half_ops = {
	.get_rate = jz_clk_pll0_half_get_rate,
};

static struct clk jz_clk_pll0_half = {
	.name = "pll0 half",
	.parent = &jz_clk_pll0,
	.ops = &jz_clk_pll0_half_ops,
};

static int jz_clk_pll1_set_parent(struct clk *clk, struct clk *parent)
{
	if (parent == &jz_clk_ext.clk)
		jz_clk_reg_clear_bits(CPM_CPPCR1_OFFSET, CPPCR1_P1SCS);
	else if (parent == &jz_clk_pll0)
		jz_clk_reg_set_bits(CPM_CPPCR1_OFFSET, CPPCR1_P1SCS);
	else
		return -EINVAL;

	clk->parent = parent;
	return 0;
}

static struct clk_ops jz_clk_pll1_ops = {
	.get_rate = jz_clk_pll1_get_rate,
	.set_parent = jz_clk_pll1_set_parent,
	.set_rate = jz_clk_pll1_set_rate,
	.enable = jz_clk_pll1_enable,
	.disable = jz_clk_pll1_disable,
	.is_enabled = jz_clk_pll1_is_enabled,
};

static struct clk jz_clk_pll1 = {
	.name = "pll1",
	.parent = &jz_clk_ext.clk,
	.ops = &jz_clk_pll1_ops,
};

static const struct clk_ops jz_clk_main_ops = {
	.get_rate = jz_clk_main_get_rate,
	.set_rate = jz_clk_main_set_rate,
	.round_rate = jz_clk_main_round_rate,
};

enum {
	JZ_CLK_MAIN_CCLK,
	JZ_CLK_MAIN_H0CLK,
	JZ_CLK_MAIN_H1CLK,
	JZ_CLK_MAIN_H2CLK,
	JZ_CLK_MAIN_C1CLK,
	JZ_CLK_MAIN_PCLK,
};

static struct main_clk jz_clk_main_clks[] = {
	[JZ_CLK_MAIN_CCLK] = {
		.clk = {
			.name = "cclk",
			.parent = &jz_clk_pll0,
			.ops = &jz_clk_main_ops,
		},
		.div_offset = CPCCR_CDIV_LSB,
	},
	[JZ_CLK_MAIN_H0CLK] = {
		.clk = {
			.name = "h0clk",
			.parent = &jz_clk_pll0,
			.ops = &jz_clk_main_ops,
		},
		.div_offset = CPCCR_H0DIV_LSB,
	},
	[JZ_CLK_MAIN_H1CLK] = {
		.clk = {
			.name = "h1clk",
			.parent = &jz_clk_pll0,
			.ops = &jz_clk_main_ops,
		},
		.div_offset = CPCCR_H1DIV_LSB,
	},
	[JZ_CLK_MAIN_H2CLK] = {
		.clk = {
			.name = "h2clk",
			.parent = &jz_clk_pll0,
			.ops = &jz_clk_main_ops,
		},
		.div_offset = CPCCR_H2DIV_LSB,
	},
	[JZ_CLK_MAIN_C1CLK] = {
		.clk = {
			.name = "c1clk",
			.parent = &jz_clk_pll0,
			.ops = &jz_clk_main_ops,
		},
		.div_offset = CPCCR_C1DIV_LSB,
	},
	[JZ_CLK_MAIN_PCLK] = {
		.clk = {
			.name = "pclk",
			.parent = &jz_clk_pll0,
			.ops = &jz_clk_main_ops,
		},
		.div_offset = CPCCR_PDIV_LSB,
	},
};

static unsigned long jz_clk_divided_get_rate(struct clk *clk)
{
	struct divided_clk *dclk = (struct divided_clk *)clk;
	int div;

	if (clk->parent == &jz_clk_ext.clk)
		return clk_get_rate(clk->parent);

	div = (jz_clk_reg_read(dclk->reg) & dclk->mask) + 1;

	return clk_get_rate(clk->parent) / div;
}

static int jz_clk_divided_set_rate(struct clk *clk, unsigned long rate)
{
	struct divided_clk *dclk = (struct divided_clk *)clk;
	unsigned long parent_rate;
	int div;

	if (clk->parent == &jz_clk_ext.clk)
		return -EINVAL;

	parent_rate = clk_get_rate(clk->parent);

	div = (int)(parent_rate / rate) - 1;
	/* round divider up => round rate down */
	if (parent_rate > (div + 1) * rate)
		div++;

	if (div < 0)
		div = 0;
	else if (div > dclk->mask)
		div = dclk->mask;

	//printk("Updating divider: parent=%lu rate=%lu div=%i\n",
	//       clk_get_rate(clk->parent), rate, div + 1);
	jz_clk_reg_write_mask(dclk->reg, div, dclk->mask);

	return 0;
}

static int jz_clk_divided_set_parent(struct clk *clk, struct clk *parent)
{
	struct divided_clk *dclk = (struct divided_clk *)clk;

	if (parent == &jz_clk_ext.clk) {
		if (dclk->ext_mask == JZ_CLK_DIVIDED_NO_EXT)
			return -EINVAL;

		jz_clk_reg_clear_bits(dclk->reg, dclk->ext_mask);
	} else {
		if (dclk->ext_mask != JZ_CLK_DIVIDED_NO_EXT)
			jz_clk_reg_set_bits(dclk->reg, dclk->ext_mask);

		if (parent == &jz_clk_pll1)
			jz_clk_reg_set_bits(dclk->reg, dclk->pll_mask);
		else if (parent == &jz_clk_pll0_half)
			jz_clk_reg_clear_bits(dclk->reg, dclk->pll_mask);
		else
			return -EINVAL;
	}

	clk->parent = parent;
	return 0;
}

static const struct clk_ops jz_clk_divided_ops = {
	.set_parent = jz_clk_divided_set_parent,
	.set_rate = jz_clk_divided_set_rate,
	.get_rate = jz_clk_divided_get_rate,
	.enable = jz_clk_enable_gating,
	.disable = jz_clk_disable_gating,
	.is_enabled = jz_clk_is_enabled_gating,
};

static const struct clk_ops jz_clk_msc_ops = {
	.set_parent = jz_clk_divided_set_parent,
	.set_rate = jz_clk_divided_set_rate,
	.get_rate = jz_clk_divided_get_rate,
	.enable = jz_clk_enable_gating_msc,
	.disable = jz_clk_disable_gating,
	.is_enabled = jz_clk_is_enabled_gating,
};

/* Those clocks can connect to PLL0_half or PLL1 */
static struct divided_clk jz_clk_divided_clks[] = {
	/* TODO: Complete this */
	{
		.clk = {
			.name = "mmc0",
			.gate_register = CPM_CLKGR0_OFFSET,
			.gate_bit = CLKGR0_MSC0,
			.ops = &jz_clk_msc_ops,
		},
		.reg = CPM_MSC0CDR_OFFSET,
		.mask = MSCCDR_MSCDIV_MASK,
		.pll_mask = MSCCDR_MPCS,
		.ext_mask = JZ_CLK_DIVIDED_NO_EXT,
	},
	{
		.clk = {
			.name = "mmc1",
			.gate_register = CPM_CLKGR0_OFFSET,
			.gate_bit = CLKGR0_MSC1,
			.ops = &jz_clk_msc_ops,
		},
		.reg = CPM_MSC1CDR_OFFSET,
		.mask = MSCCDR_MSCDIV_MASK,
		.pll_mask = MSCCDR_MPCS,
		.ext_mask = JZ_CLK_DIVIDED_NO_EXT,
	},
	{
		.clk = {
			.name = "mmc2",
			.gate_register = CPM_CLKGR0_OFFSET,
			.gate_bit = CLKGR0_MSC2,
			.ops = &jz_clk_msc_ops,
		},
		.reg = CPM_MSC2CDR_OFFSET,
		.mask = MSCCDR_MSCDIV_MASK,
		.pll_mask = MSCCDR_MPCS,
		.ext_mask = JZ_CLK_DIVIDED_NO_EXT,
	},
	{
		.clk = {
			.name = "cim",
			.gate_register = CPM_CLKGR0_OFFSET,
			.gate_bit = CLKGR0_CIM,
			.ops = &jz_clk_divided_ops,
		},
		.reg = CPM_CIMCDR_OFFSET,
		.mask = CIMCDR_CIMDIV_MASK,
		.pll_mask = CIMCDR_CIMPCS,
		.ext_mask = JZ_CLK_DIVIDED_NO_EXT,
	},
	{
		.clk = {
			.name = "uhc",
			.gate_register = CPM_CLKGR0_OFFSET,
			.gate_bit = CLKGR0_UHC,
			.ops = &jz_clk_divided_ops,
		},
		.reg = CPM_UHCCDR_OFFSET,
		.mask = UHCCDR_UHCDIV_MASK,
		.pll_mask = UHCCDR_UHPCS,
		.ext_mask = JZ_CLK_DIVIDED_NO_EXT,
	},
	{
		.clk = {
			.name = "gpu",
			.gate_register = CPM_CLKGR1_OFFSET,
			.gate_bit = CLKGR1_GPU,
			.ops = &jz_clk_divided_ops,
		},
		.reg = CPM_GPUCDR_OFFSET,
		.mask = GPUCDR_GPUDIV_MASK,
		.pll_mask = GPUCDR_GPCS,
		.ext_mask = JZ_CLK_DIVIDED_NO_EXT,
	},
	{
		.clk = {
			.name = "bch",
			.gate_register = CPM_CLKGR0_OFFSET,
			.gate_bit = CLKGR0_BCH,
			.ops = &jz_clk_divided_ops,
		},
		.reg = CPM_BCHCDR_OFFSET,
		.mask = CPM_BCHCDR_BCHDIV_MASK,
		.pll_mask = CPM_BCHCDR_BPCS,
		.ext_mask = JZ_CLK_DIVIDED_NO_EXT,
	},
	{
		.clk = {
			.name = "lpclk",
			.gate_register = CPM_CLKGR0_OFFSET,
			.gate_bit = CLKGR0_LCD,
			.ops = &jz_clk_divided_ops,
		},
		.reg = CPM_LPCDR_OFFSET,
		.mask = LPCDR_PIXDIV_MASK,
		.pll_mask = LPCDR_LPCS,
		.ext_mask = JZ_CLK_DIVIDED_NO_EXT,
	},
};

/* Those clocks can connect to EXT, PLL0_half or PLL1 */
static struct divided_clk jz_clk_divided_clks_ext[] = {
	{
		.clk = {
			.name = "spi",
			.gate_register = CPM_CLKGR0_OFFSET,
			.gate_bit = CLKGR0_SSI1 | CLKGR0_SSI2,
			.ops = &jz_clk_divided_ops,
		},
		.reg = CPM_SSICDR_OFFSET,
		.mask = SSICDR_SSIDIV_MASK,
		.pll_mask = SSICDR_SPCS,
		.ext_mask = SSICDR_SCS,
	},
	{
		.clk = {
			.name = "i2s",
			.gate_register = CPM_CLKGR1_OFFSET,
			.gate_bit = CLKGR1_I2S2,
			.ops = &jz_clk_divided_ops,
		},
		.reg = CPM_I2SCDR_OFFSET,
		.mask = I2SCDR_I2SDIV_MASK,
		.pll_mask = I2SCDR_I2PCS,
		.ext_mask = I2SCDR_I2CS,
	},
	{
		.clk = {
			.name = "pcm",
			.gate_register = CPM_CLKGR1_OFFSET,
			.gate_bit = CLKGR1_PCM0 | CLKGR1_PCM1,
			.ops = &jz_clk_divided_ops,
		},
		.reg = CPM_PCMCDR_OFFSET,
		.mask = PCMCDR_PCMDIV_MASK,
		.pll_mask = PCMCDR_PCMPCS,
		.ext_mask = PCMCDR_PCMS,
	},
	{
		.clk = {
			.name = "usb",
			.gate_register = CPM_CLKGR0_OFFSET,
			.gate_bit = CLKGR0_OTG,
			.ops = &jz_clk_divided_ops,
		},
		.reg = CPM_USBCDR_OFFSET,
		.mask = USBCDR_OTGDIV_MASK,
		.pll_mask = USBCDR_UPCS,
		.ext_mask = USBCDR_UCS,
	},
};

static const struct clk_ops jz_clk_simple_ops = {
	.enable = jz_clk_enable_gating,
	.disable = jz_clk_disable_gating,
	.is_enabled = jz_clk_is_enabled_gating,
};

static struct clk jz_clk_simple_clks[] = {
	{
		.name = "dma",
		.parent = &jz_clk_main_clks[JZ_CLK_MAIN_H2CLK].clk,
		.gate_register = CPM_CLKGR0_OFFSET,
		.gate_bit = CLKGR0_DMAC,
		.ops = &jz_clk_simple_ops,
	},
	{
		.name = "i2c0",
		.parent = &jz_clk_ext.clk,
		.gate_register = CPM_CLKGR0_OFFSET,
		.gate_bit = CLKGR0_I2C0,
		.ops = &jz_clk_simple_ops,
	},
	{
		.name = "i2c1",
		.parent = &jz_clk_ext.clk,
		.gate_register = CPM_CLKGR0_OFFSET,
		.gate_bit = CLKGR0_I2C1,
		.ops = &jz_clk_simple_ops,
	},
	{
		.name = "i2c2",
		.parent = &jz_clk_ext.clk,
		.gate_register = CPM_CLKGR1_OFFSET,
		.gate_bit = CLKGR1_I2C2,
		.ops = &jz_clk_simple_ops,
	},
	{
		.name = "uart0",
		.parent = &jz_clk_ext.clk,
		.gate_register = CPM_CLKGR0_OFFSET,
		.gate_bit = CLKGR0_UART0,
		.ops = &jz_clk_simple_ops,
	},
	{
		.name = "uart1",
		.parent = &jz_clk_ext.clk,
		.gate_register = CPM_CLKGR0_OFFSET,
		.gate_bit = CLKGR0_UART1,
		.ops = &jz_clk_simple_ops,
	},
	{
		.name = "uart2",
		.parent = &jz_clk_ext.clk,
		.gate_register = CPM_CLKGR0_OFFSET,
		.gate_bit = CLKGR0_UART2,
		.ops = &jz_clk_simple_ops,
	},
	{
		.name = "uart3",
		.parent = &jz_clk_ext.clk,
		.gate_register = CPM_CLKGR0_OFFSET,
		.gate_bit = CLKGR0_UART3,
		.ops = &jz_clk_simple_ops,
	},
	{
		.name = "ipu",
		.parent = &jz_clk_main_clks[JZ_CLK_MAIN_H0CLK].clk,
		.gate_register = CPM_CLKGR0_OFFSET,
		.gate_bit = CLKGR0_IPU,
		.ops = &jz_clk_simple_ops,
	},
	{
		.name = "adc",
		.parent = &jz_clk_ext.clk,
		.gate_register = CPM_CLKGR0_OFFSET,
		.gate_bit = CLKGR0_SADC,
		.ops = &jz_clk_simple_ops,
	},
	{
		.name = "aic",
		.parent = &jz_clk_ext.clk,
		.gate_register = CPM_CLKGR0_OFFSET,
		.gate_bit = CLKGR0_AIC,
		.ops = &jz_clk_simple_ops,
	},
	{
		.name = "aux",
		.parent = &jz_clk_main_clks[JZ_CLK_MAIN_C1CLK].clk,
		.gate_register = CPM_CLKGR1_OFFSET,
		.gate_bit = CLKGR1_AUX,
		.ops = &jz_clk_simple_ops,
	},
	{
		.name = "vpu",
		.parent = &jz_clk_main_clks[JZ_CLK_MAIN_H1CLK].clk,
		.gate_register = CPM_CLKGR1_OFFSET,
		.gate_bit = CLKGR1_VPU,
		.ops = &jz_clk_simple_ops,
	},
};

static int jz_clk_tcu_set_parent(struct clk *clk, struct clk *parent)
{
	struct tcu_clk *tclk = (struct tcu_clk *) clk;
	u16 val = readw(*tclk->base_ptr + tclk->reg) & ~0x7;

	if (parent == &jz_clk_ext.clk)
		val |= BIT(2);
	else if (parent == &jz_clk_rtc.clk)
		val |= BIT(1);
	else if (parent == &jz_clk_main_clks[JZ_CLK_MAIN_PCLK].clk)
		val |= BIT(0);
	else
		return -EINVAL;

	clk->parent = parent;
	writew(val, *tclk->base_ptr + tclk->reg);
	return 0;
}

static unsigned long jz_clk_tcu_get_rate(struct clk *clk)
{
	struct tcu_clk *tclk = (struct tcu_clk *) clk;
	unsigned long parent_rate = clk_get_rate(clk->parent);
	u16 val = (readw(*tclk->base_ptr + tclk->reg) >> 3) & 0x7;

	return parent_rate >> (val * 2);
}

static unsigned long jz_clk_tcu_round_rate(struct clk *clk, unsigned long rate)
{
	unsigned long parent_rate = clk_get_rate(clk->parent);
	unsigned int i;

	for (i = 5; i && (rate > (parent_rate >> (i * 2))); i--);
	return parent_rate >> (i * 2);
}

static int jz_clk_tcu_set_rate(struct clk *clk, unsigned long rate)
{
	struct tcu_clk *tclk = (struct tcu_clk *) clk;
	unsigned long parent_rate = clk_get_rate(clk->parent);
	u16 val = readw(*tclk->base_ptr + tclk->reg) & ~0x38;

	rate = jz_clk_tcu_round_rate(clk, rate);

	val |= (ffs(parent_rate / rate) / 2) << 3;
	writew(val, *tclk->base_ptr + tclk->reg);
	return 0;
}

static int jz_clk_tcu_enable(struct clk *clk)
{
	writel(clk->gate_bit, tcu_base + TCU_TSCR_OFFSET);
	return 0;
}

static int jz_clk_tcu_disable(struct clk *clk)
{
	writel(clk->gate_bit, tcu_base + TCU_TSSR_OFFSET);
	return 0;
}

static int jz_clk_tcu_is_enabled(struct clk *clk)
{
	if (clk->gate_bit == JZ_CLK_NOT_GATED)
		return 1;

	return !(readl(tcu_base + clk->gate_register) & clk->gate_bit);
}

static const struct clk_ops jz_clk_tcu_ops = {
	.set_parent = jz_clk_tcu_set_parent,
	.round_rate = jz_clk_tcu_round_rate,
	.get_rate = jz_clk_tcu_get_rate,
	.set_rate = jz_clk_tcu_set_rate,
	.enable = jz_clk_tcu_enable,
	.disable = jz_clk_tcu_disable,
	.is_enabled = jz_clk_tcu_is_enabled,
};

enum {
	JZ_CLK_WDT,
	JZ_CLK_OST,
	JZ_CLK_TCU_TIMER0,
	JZ_CLK_TCU_TIMER1,
	JZ_CLK_TCU_TIMER2,
	JZ_CLK_TCU_TIMER3,
	JZ_CLK_TCU_TIMER4,
	JZ_CLK_TCU_TIMER5,
	JZ_CLK_TCU_TIMER6,
	JZ_CLK_TCU_TIMER7,
};

static struct tcu_clk jz_clk_tcu_clks[] = {
	[JZ_CLK_WDT] = {
		.clk = {
			.name = "wdt",
			.gate_bit = BIT(16),
			.gate_register = TCU_TSR_OFFSET,
			.ops = &jz_clk_tcu_ops,
		},
		.base_ptr = &wdt_tcsr,
		.reg = 0,
	},
	[JZ_CLK_OST] = {
		.clk = {
			.name = "ost",
			.gate_bit = BIT(15),
			.gate_register = TCU_TSR_OFFSET,
			.ops = &jz_clk_tcu_ops,
		},
		.base_ptr = &ost_tcsr,
		.reg = 0,
	},
#define _TIMER(x) \
	[JZ_CLK_TCU_TIMER##x] = { \
	.clk = { \
		.name = "timer" #x, \
		.gate_bit = BIT(x), \
		.gate_register = TCU_TSR_OFFSET, \
		.ops = &jz_clk_tcu_ops, \
	}, \
	.base_ptr = &tcu_base, \
	.reg = TCU_TCSR_OFFSET(x), \
}
	_TIMER(0),
	_TIMER(1),
	_TIMER(2),
	_TIMER(3),
	_TIMER(4),
	_TIMER(5),
	_TIMER(6),
	_TIMER(7),
#undef _TIMER
};

int clk_enable(struct clk *clk)
{
	if (!clk->ops->enable)
		return -EINVAL;

	return clk->ops->enable(clk);
}
EXPORT_SYMBOL_GPL(clk_enable);

void clk_disable(struct clk *clk)
{
	if (clk->ops->disable)
		clk->ops->disable(clk);
}
EXPORT_SYMBOL_GPL(clk_disable);

int clk_is_enabled(struct clk *clk)
{
	if (clk->ops->is_enabled)
		return clk->ops->is_enabled(clk);

	return 1;
}

unsigned long clk_get_rate(struct clk *clk)
{
	if (clk->ops->get_rate)
		return clk->ops->get_rate(clk);
	if (clk->parent)
		return clk_get_rate(clk->parent);

	return -EINVAL;
}
EXPORT_SYMBOL_GPL(clk_get_rate);

int clk_set_rate(struct clk *clk, unsigned long rate)
{
	if (!clk->ops->set_rate)
		return -EINVAL;
	return clk->ops->set_rate(clk, rate);
}
EXPORT_SYMBOL_GPL(clk_set_rate);

long clk_round_rate(struct clk *clk, unsigned long rate)
{
	if (clk->ops->round_rate)
		return clk->ops->round_rate(clk, rate);

	return -EINVAL;
}
EXPORT_SYMBOL_GPL(clk_round_rate);

struct clk *clk_get_parent(struct clk *clk)
{
	return clk->parent;
}
EXPORT_SYMBOL_GPL(clk_get_parent);

int clk_set_parent(struct clk *clk, struct clk *parent)
{
	int ret;
	int enabled;

	if (!clk->ops->set_parent)
		return -EINVAL;

	enabled = clk_is_enabled(clk);
	if (enabled)
		clk_disable(clk);
	ret = clk->ops->set_parent(clk, parent);
	if (enabled)
		clk_enable(clk);

	return ret;
}
EXPORT_SYMBOL_GPL(clk_set_parent);

struct clk *clk_get(struct device *dev, const char *name)
{
	struct clk *clk;

	list_for_each_entry(clk, &jz_clk_list, list) {
		if (strcmp(clk->name, name) == 0)
			return clk;
	}
	return ERR_PTR(-ENXIO);
}
EXPORT_SYMBOL_GPL(clk_get);

void clk_put(struct clk *clk)
{
}
EXPORT_SYMBOL_GPL(clk_put);

void jz4770_toggle_sleep_mode(bool enable_sleep)
{
	if (enable_sleep)
		jz_clk_reg_set_bits(CPM_LCR_OFFSET, LCR_LPM_SLEEP);
	else
		jz_clk_reg_clear_bits(CPM_LCR_OFFSET, LCR_LPM_SLEEP);
}

static inline void clk_add(struct clk *clk)
{
	list_add_tail(&clk->list, &jz_clk_list);
}

static void __init clk_register_clks(void)
{
	size_t i;

	clk_add(&jz_clk_ext.clk);
	clk_add(&jz_clk_rtc.clk);
	clk_add(&jz_clk_pll0);
	clk_add(&jz_clk_pll0_half);
	clk_add(&jz_clk_pll1);

	for (i = 0; i < ARRAY_SIZE(jz_clk_main_clks); ++i)
		clk_add(&jz_clk_main_clks[i].clk);

	for (i = 0; i < ARRAY_SIZE(jz_clk_divided_clks); ++i)
		clk_add(&jz_clk_divided_clks[i].clk);

	for (i = 0; i < ARRAY_SIZE(jz_clk_divided_clks_ext); ++i)
		clk_add(&jz_clk_divided_clks_ext[i].clk);

	for (i = 0; i < ARRAY_SIZE(jz_clk_simple_clks); ++i)
		clk_add(&jz_clk_simple_clks[i]);

	for (i = 0; i < ARRAY_SIZE(jz_clk_tcu_clks); ++i)
		clk_add(&jz_clk_tcu_clks[i].clk);
}

static int __init jz_clk_init(void)
{
	size_t i;

	jz_clock_base = ioremap_nocache(JZ4770_CPM_BASE_ADDR, 0x100);
	if (!jz_clock_base)
		return -EBUSY;

	tcu_base = ioremap_nocache(JZ4770_TCU_BASE_ADDR, 0x100);
	if (!tcu_base)
		return -EBUSY;

	wdt_tcsr = ioremap_nocache(JZ4770_WDT_BASE_ADDR + WDT_TCSR_OFFSET, 2);
	if (!wdt_tcsr)
		return -EBUSY;

	ost_tcsr = ioremap_nocache(JZ4770_OST_BASE_ADDR + OST_OSTCSR_OFFSET, 2);
	if (!ost_tcsr)
		return -EBUSY;

	spin_lock_init(&jz_clock_lock);

	jz_clk_ext.rate = jz_clk_bdata.ext_rate;
	jz_clk_rtc.rate = jz_clk_bdata.rtc_rate;

	/* Connect as much clocks to EXT as possible */
	for (i = 0; i < ARRAY_SIZE(jz_clk_divided_clks_ext); ++i) {
		struct clk *clk = &jz_clk_divided_clks_ext[i].clk;
		clk->ops->set_parent(clk, &jz_clk_ext.clk);
	}

	for (i = 0; i < ARRAY_SIZE(jz_clk_tcu_clks); i++) {
		struct clk *clk = &jz_clk_tcu_clks[i].clk;

		if (i == JZ_CLK_WDT)
			clk->ops->set_parent(clk, &jz_clk_rtc.clk);
		else
			clk->ops->set_parent(clk, &jz_clk_ext.clk);
	}

	jz_clk_pll1.ops->set_rate(&jz_clk_pll1, jz_clk_bdata.pll1_rate);
	jz_clk_pll1.ops->enable(&jz_clk_pll1);
	printk("PLL0 rate: %lu\n", jz_clk_pll0.ops->get_rate(&jz_clk_pll0));
	printk("PLL1 rate: %lu\n", jz_clk_pll1.ops->get_rate(&jz_clk_pll1));

	/* Apply new dividers immediately. */
	jz_clk_reg_set_bits(CPM_CPCCR_OFFSET, CPCCR_CE);

	/* Connect all other divided clocks to PLL1 */
	for (i = 0; i < ARRAY_SIZE(jz_clk_divided_clks); ++i) {
		struct clk *clk = &jz_clk_divided_clks[i].clk;
		clk->ops->set_parent(clk, &jz_clk_pll1);
	}

	clk_register_clks();
	return 0;
}
arch_initcall(jz_clk_init);

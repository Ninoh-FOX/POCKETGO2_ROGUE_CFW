/*
 *  Copyright (C) 2010, Lars-Peter Clausen <lars@metafoo.de>
 *  JZ4740 SoC clock support
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __MIPS_JZ4740_CLOCK_H__
#define __MIPS_JZ4740_CLOCK_H__

#include <linux/list.h>

struct jz_clk_board_data {
	unsigned long ext_rate;
	unsigned long rtc_rate;
	unsigned long pll1_rate;
};

extern struct jz_clk_board_data jz_clk_bdata;

struct clk;

struct clk_ops {
	unsigned long (*get_rate)(struct clk *clk);
	unsigned long (*round_rate)(struct clk *clk, unsigned long rate);
	int (*set_rate)(struct clk *clk, unsigned long rate);
	int (*enable)(struct clk *clk);
	int (*disable)(struct clk *clk);
	int (*is_enabled)(struct clk *clk);

	int (*set_parent)(struct clk *clk, struct clk *parent);

};

struct clk {
	const char *name;
	struct clk *parent;

	uint32_t gate_bit;
	unsigned int gate_register;

	const struct clk_ops *ops;

	struct list_head list;
};

#define JZ_CLK_NOT_GATED ((uint32_t)-1)

int clk_is_enabled(struct clk *clk);
void jz4770_toggle_sleep_mode(bool enable_sleep);

#endif

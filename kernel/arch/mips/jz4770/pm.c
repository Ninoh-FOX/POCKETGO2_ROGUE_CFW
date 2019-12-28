/*
 * linux/arch/mips/jz4770/pm.c
 *
 * JZ4770 Power Management Routines
 *
 * Copyright (C) 2006 - 2010 Ingenic Semiconductor Inc.
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 */

#include <linux/clk.h>
#include <linux/init.h>
#include <linux/pm.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/suspend.h>
#include <linux/proc_fs.h>
#include <linux/sysctl.h>
#include <linux/delay.h>

#include <asm/cacheops.h>

#include <asm/mach-jz4770/jz4770cpm.h>
#include <asm/mach-jz4770/jz4770gpio.h>
#include <asm/mach-jz4770/jz4770intc.h>
#include <asm/mach-jz4770/jz4770misc.h>
#include <asm/mach-jz4770/jz4770rtc.h>
#include <asm/mach-jz4770/jz4770sadc.h>

#include "clock.h"

static int jz4770_pm_enter(suspend_state_t state)
{
	bool dma_clk_enabled, uart0_clk_enabled;
	struct clk *dma_clk = clk_get(NULL, "dma"),
		   *uart0_clk = clk_get(NULL, "uart0");
	dma_clk_enabled = clk_is_enabled(dma_clk);
	uart0_clk_enabled = clk_is_enabled(uart0_clk);

	if (uart0_clk_enabled)
		clk_disable(uart0_clk);
	if (dma_clk_enabled)
		clk_disable(dma_clk);

	jz4770_toggle_sleep_mode(true);

	__asm__(".set\tmips3\n\t"
		"wait\n\t"
		".set\tmips0");

	jz4770_toggle_sleep_mode(false);

	if (dma_clk_enabled)
		clk_enable(dma_clk);
	if (uart0_clk_enabled)
		clk_enable(uart0_clk);

	clk_put(dma_clk);
	clk_put(uart0_clk);
	return 0;
}

static struct platform_suspend_ops jz4770_pm_ops = {
	.valid		= suspend_valid_only_mem,
	.enter		= jz4770_pm_enter,
};

static int __init jz4770_pm_init(void)
{
	suspend_set_ops(&jz4770_pm_ops);
	return 0;

}
late_initcall(jz4770_pm_init);

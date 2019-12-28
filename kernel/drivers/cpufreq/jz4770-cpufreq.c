/*
 * drivers/cpufreq/jz4770-cpufreq.c
 *
 * cpufreq driver for JZ4770
 *
 * Copyright (c) 2014 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/err.h>

#include <linux/cpufreq.h>

#include <linux/clk.h>
#include <asm/mach-jz4770/base.h>

static struct clk *cclk;
static struct cpufreq_driver cpufreq_jz4770_driver;

/* TODO: Support all dividers: 1, 2, 3, 4, 6, 8, 12 */
static const unsigned int jz4770_cpu_divs[] = { 1, 3, };
static struct cpufreq_frequency_table table[ARRAY_SIZE(jz4770_cpu_divs) + 1];

static void jz4770_freq_fill_table(struct cpufreq_policy *policy)
{
	int i;

#ifdef CONFIG_CPU_FREQ_STAT_DETAILS
	/* for showing /sys/devices/system/cpu/cpuX/cpufreq/stats/ */
	static bool init = false;
	if (init)
		cpufreq_frequency_table_put_attr(policy->cpu);
	else
		init = true;
#endif

	for (i = 0; i < ARRAY_SIZE(jz4770_cpu_divs); i++) {
		unsigned int freq = policy->cpuinfo.max_freq / jz4770_cpu_divs[i];
		if (freq < policy->cpuinfo.min_freq)
			break;
		table[i].frequency = freq;
	}
	table[i].frequency = CPUFREQ_TABLE_END;

	policy->min = table[i - 1].frequency;
	policy->max = table[0].frequency;

#ifdef CONFIG_CPU_FREQ_STAT_DETAILS
	cpufreq_frequency_table_get_attr(table, policy->cpu);
#endif
}

static unsigned int jz4770_freq_get(unsigned int cpu)
{
	if (cpu > 0)
		return -EINVAL;

	return clk_get_rate(cclk) / 1000;
}

static int jz4770_freq_verify(struct cpufreq_policy *policy)
{
	cpufreq_verify_within_limits(policy, policy->cpuinfo.min_freq,
				     policy->cpuinfo.max_freq);
	return 0;
}

static int jz4770_freq_target(struct cpufreq_policy *policy,
			  unsigned int target_freq,
			  unsigned int relation)
{
	struct cpufreq_freqs freqs;
	unsigned int new_index = 0;
	int ret = 0;

	if (cpufreq_frequency_table_target(policy, table,
					   target_freq, relation, &new_index))
		return -EINVAL;

	freqs = (struct cpufreq_freqs) {
		.old = jz4770_freq_get(policy->cpu),
		.new = table[new_index].frequency,
		.cpu = policy->cpu,
		.flags = cpufreq_jz4770_driver.flags,
	};

	if (freqs.old == freqs.new && policy->cur == freqs.new)
		return 0;

	cpufreq_notify_transition(policy, &freqs, CPUFREQ_PRECHANGE);
	pr_debug("%s: setting from %d to %d\n",
				__FUNCTION__, freqs.old, freqs.new);
	ret = clk_set_rate(cclk, freqs.new * 1000);
	cpufreq_notify_transition(policy, &freqs, CPUFREQ_POSTCHANGE);
	return ret;
}

static int jz4770_cpufreq_driver_init(struct cpufreq_policy *policy)
{
	struct clk *pll0;
	unsigned int max_divider = jz4770_cpu_divs[ARRAY_SIZE(jz4770_cpu_divs) - 1];

	pr_debug("Jz4770 cpufreq driver\n");

	if (policy->cpu != 0)
		return -EINVAL;

	pll0 = clk_get(NULL, "pll0");
	if (IS_ERR(pll0))
		return PTR_ERR(pll0);

	cclk = clk_get(NULL, "cclk");
	if (IS_ERR(cclk))
		return PTR_ERR(cclk);

	policy->cpuinfo.max_freq = clk_get_rate(pll0) / 1000;
	policy->cpuinfo.min_freq = policy->cpuinfo.max_freq / max_divider;
	jz4770_freq_fill_table(policy);
	clk_put(pll0);

	policy->cpuinfo.transition_latency = 400000; /* in nanoseconds */
	policy->cur = jz4770_freq_get(policy->cpu);
	policy->governor = CPUFREQ_DEFAULT_GOVERNOR;
	/* min and max are set by jz4770_freq_fill_table() */

	return 0;
}

static struct cpufreq_driver cpufreq_jz4770_driver = {
	.init	= jz4770_cpufreq_driver_init,
	.verify	= jz4770_freq_verify,
	.target	= jz4770_freq_target,
	.get	= jz4770_freq_get,
	.name	= "jz4770-cpufreq",
};

static int __init jz4770_cpufreq_init(void)
{
	return cpufreq_register_driver(&cpufreq_jz4770_driver);
}

static void __exit jz4770_cpufreq_exit(void)
{
	cpufreq_unregister_driver(&cpufreq_jz4770_driver);
}

module_init(jz4770_cpufreq_init);
module_exit(jz4770_cpufreq_exit);

MODULE_AUTHOR("Paul Cercueil <paul@crapouillou.net>");
MODULE_DESCRIPTION("cpufreq driver for JZ4770");
MODULE_LICENSE("GPL");

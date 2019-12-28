/*
 * drivers/power/act8600_power.h -- Core interface for ACT8600
 *
 * Copyright 2010 Ingenic Semiconductor LTD.
 * Copyright 2012 Maarten ter Huurne <maarten@treewalker.org>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#ifndef __ACT8600_POWER_H__
#define __ACT8600_POWER_H__

#include <linux/types.h>

#define ACT8600_NAME		"act8600"
#define ACT8600_I2C_ADDR	0x5A

struct act8600_outputs_t {
	int outnum;
	int value;
	bool enable;
};

struct act8600_platform_pdata_t {
	struct act8600_outputs_t *outputs;
	int nr_outputs;
};

/*
 * The Q[123] switches are used to control the USB VBUS line.
 * Q1 connects the line to the 5V input line, powering the line.
 * Q2 connects it to the VSYS output, powering the line.
 * Q3 connects it to CHGIN, and is used to power the system from USB.
 *
 * Q1 and Q2 switch off automatically when the current is over 700 ma.
 * Q2 switches off when the CHGIN voltage is over 6v.
 */
enum act8600_power_mode {
	VBUS_UNPOWERED = 0,
	VBUS_POWERED_BY_5VIN = 1, /* Q1 enabled */
	VBUS_POWERED_BY_CHGIN = 2, /* Q2 enabled */
	VBUS_POWERED_EXTERNALLY = 3, /* Q3 enabled */
};

/**
 * act8600_output_enable - Enable or disable one of the power outputs.
 * @outnum: output pin: 1-8
 * @enable: true to enable, false to disable
 * @returns zero on success and error code upon failure
 */
int act8600_output_enable(int outnum, bool enable);

/**
 * act8600_set_power_mode - sets the power mode.
 *
 * @mode: the power mode to set
 * @returns zero on success, or negative error code on failure
 */
int act8600_set_power_mode(enum act8600_power_mode mode);

/**
 * act8600_get_battery_state - get current battery state.
 *
 * @returns the battery state as POWER_SUPPLY_STATUS_*
 */
int act8600_get_battery_state(void);

#endif  /* __ACT8600_POWER_H__ */

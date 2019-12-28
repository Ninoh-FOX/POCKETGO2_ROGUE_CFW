/*
 * MXC6225 Thermal Accelerometer driver
 *
 * Copyright (C) 2014 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __PLATFORM_DATA_MXC6225_H__
#define __PLATFORM_DATA_MXC6225_H__

#define MXC6225_I2C_ADDR 0x15

struct mxc6225_platform_data {
	unsigned int poll_interval; /* ms */

	/* Number of times to rotate by 90 degrees to get correct values */
	unsigned int chip_orientation;

	int fuzz, flat;
};

#endif /* __PLATFORM_DATA_MXC6225_H__ */

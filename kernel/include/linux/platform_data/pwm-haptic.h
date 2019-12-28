/*
 * Driver for PWM based haptic devices
 *
 * Copyright (C) 2014 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __PLATFORM_DATA_PWM_HAPTIC_H__
#define __PLATFORM_DATA_PWM_HAPTIC_H__

struct pwm_haptic_platform_data {
	unsigned long pwm_period_ns;
};

#endif /* __PLATFORM_DATA_PWM_HAPTIC_H__ */

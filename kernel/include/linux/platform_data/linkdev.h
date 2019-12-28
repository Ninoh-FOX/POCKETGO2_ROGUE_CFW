/*
 * linkdev platform data.
 *
 * Copyright (C) 2013, Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __INCLUDE_LINUX_PLATFORM_DATA_LINKDEV_H__
#define __INCLUDE_LINUX_PLATFORM_DATA_LINKDEV_H__

struct linkdev_pdata_device_info {
	const char *name; /* Name of the input device, e.g. gpio-keys */
	const char **handlers_whitelist;
	unsigned int nb_handlers;
};

struct linkdev_pdata_abs_map {
	const char *name; /* Name of the input device, e.g. gpio-keys */
	short int axis; /* axis of the input device to map to */
	short int axis_dest;
};

struct linkdev_pdata_key_map {
	signed short code;
	struct input_value event;
};

struct linkdev_platform_data {
	const struct linkdev_pdata_device_info *devices;
	unsigned int nb_devices;

	/* Event translation map for keys.
	 * key_map[0] events will be transformed into key_map[1].
	 */
	const struct linkdev_pdata_key_map *key_map;
	unsigned int key_map_size;

	/* Event translation map for joystick axis */
	const struct linkdev_pdata_abs_map *abs_map;
	unsigned int abs_map_size;

	/* Name of the linkdev device (defaults to "linkdev-device" if NULL) */
	const char *name;

	/* Reserved for internal use */
	void *__private;
};

#endif /* __INCLUDE_LINUX_PLATFORM_DATA_LINKDEV_H__ */

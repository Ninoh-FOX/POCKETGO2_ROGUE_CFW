/*
 * JZ4770 framebuffer platform data.
 *
 * Copyright (C) 2012, Maarten ter Huurne <maarten@treewalker.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __PLATFORM_DATA_JZ4770_FB_H__
#define __PLATFORM_DATA_JZ4770_FB_H__

struct jzfb_platform_data {
	struct panel_ops *panel_ops;
	void *panel_pdata;
};

#endif /* __PLATFORM_DATA_JZ4770_FB_H__ */

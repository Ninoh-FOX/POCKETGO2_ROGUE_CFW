#ifndef __PANEL_NT39016_H
#define __PANEL_NT39016_H

struct nt39016_platform_data {
	int gpio_reset;
	int gpio_clock;
	int gpio_enable;
	int gpio_data;
};

extern struct panel_ops nt39016_panel_ops;

#endif /* __PANEL_NT39016_H */

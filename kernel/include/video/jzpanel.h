#ifndef __JZPANEL_H
#define __JZPANEL_H

struct panel_ops {
	int (*init)(void **out_panel, struct device *dev, void *panel_pdata);
	void (*exit)(void *panel);
	void (*enable)(void *panel);
	void (*disable)(void *panel);
};

#endif /* __JZPANEL_H */

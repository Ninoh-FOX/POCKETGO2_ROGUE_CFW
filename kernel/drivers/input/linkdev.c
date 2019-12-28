/*
 * Input device merger interface.
 *
 * This driver is used to merge together two or more input devices,
 * so that they appear unified to the userspace.
 * It can be used, for instance, on a gaming device where the buttons
 * are handled by the 'gpio-keys' driver  while the analog sticks are
 * handled by another driver, to make the controls appear as an unique
 * joystick device to the userspace.
 *
 * Copyright (c) 2013 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/async.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/major.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <linux/platform_data/linkdev.h>

static bool alt_key_map = false;

struct linkdev {
	struct platform_device *pdev;
	struct input_handler handler;
	struct input_dev *idev;
	struct linkdev_platform_data *pdata;
	unsigned int nb_connected;
};

static int linkdev_drop_other_handlers_from_idev(struct linkdev *linkdev,
			struct input_dev *idev, struct input_handler *handler)
{
	struct input_handle *cur, *next;
	struct linkdev_platform_data *pdata = linkdev->pdata;
	const struct linkdev_pdata_device_info *info = NULL;
	unsigned int i;

	for (i = 0; i < pdata->nb_devices; i++) {
		if (!strcmp(pdata->devices[i].name, idev->name)) {
			info = &pdata->devices[i];
			break;
		}
	}

	if (!info) {
		dev_err(&linkdev->pdev->dev, "Unable to retrieve device info "
					"for input device \'%s\'\n", idev->name);
		return -ENOENT;
	}

	list_for_each_entry_safe(cur, next, &idev->h_list, d_node) {
		bool in_whitelist = false;

		if (cur->handler == handler)
			continue;

		for (i = 0; i < info->nb_handlers; i++) {
			if (!strcmp(info->handlers_whitelist[i], cur->handler->name)) {
				in_whitelist = true;
				break;
			}
		}

		if (!in_whitelist) {
			dev_info(&idev->dev,
						"Dropping handle \'%s\' from handler \'%s\'\n",
						cur->name, cur->handler->name);
			cur->handler->disconnect(cur);
		}
	}

	return 0;
}

static void linkdev_get_button(struct linkdev *linkdev,
			unsigned int *type, unsigned int *code, int *value)
{
	unsigned int i;
	struct linkdev_platform_data *pdata = linkdev->pdata;

	for (i = 0; i < pdata->key_map_size; i++) {
		if (pdata->key_map[i].code == *code) {
			const struct input_value *val = &pdata->key_map[i].event;
			dev_dbg(&linkdev->pdev->dev, "Translating event code %hi to %hi",
						*code, val->code);

			if (val->type)
				*type = val->type;
			*code = val->code;
			if (*type == EV_ABS && *value)
				*value = val->value;
			break;
		}
	}
}

static short int linkdev_get_axis(struct linkdev *linkdev,
			struct input_dev *idev, short int axis)
{
	unsigned int i;
	struct linkdev_platform_data *pdata = linkdev->pdata;

	if (pdata->abs_map_size) {
		for (i = 0; i < pdata->abs_map_size; i++)
		  if (!strcmp(pdata->abs_map[i].name, idev->name)
					  && pdata->abs_map[i].axis == axis)
			return pdata->abs_map[i].axis_dest;
	}

	return axis;
}

static void linkdev_event(struct input_handle *handle,
			unsigned int type, unsigned int code, int value)
{
	struct linkdev *linkdev = handle->handler->private;
	struct input_dev *idev = linkdev->idev;

	if (!idev)
		return;

	if (type == EV_KEY && alt_key_map)
		linkdev_get_button(linkdev, &type, &code, &value);
	else if (type == EV_ABS)
		code = linkdev_get_axis(linkdev, idev, code);

	dev_dbg(&linkdev->pdev->dev, "Injecting code %hi\n", code);
	input_event(idev, type, code, value);
}

static void linkdev_events(struct input_handle *handle,
			const struct input_value *vals, unsigned int count)
{
	while (count--) {
		linkdev_event(handle, vals->type, vals->code, vals->value);
		vals++;
	}
}

static bool linkdev_match(struct input_handler *handler, struct input_dev *idev)
{
	struct linkdev *linkdev = handler->private;
	struct linkdev_platform_data *pdata = linkdev->pdata;
	unsigned int i;

	dev_dbg(&linkdev->pdev->dev, "Testing match for device %s\n", idev->name);

	for (i = 0; i < pdata->nb_devices; i++)
		if (!strcmp(idev->name, pdata->devices[i].name)) {
			dev_dbg(&linkdev->pdev->dev, "Match found for device %s\n",
						idev->name);
			return true;
		}

	return false;
}

static int linkdev_connect(struct input_handler *handler,
			struct input_dev *idev,
			const struct input_device_id *id)
{
	struct linkdev *linkdev = handler->private;
	struct linkdev_platform_data *pdata = linkdev->pdata;
	struct device *dev = &linkdev->pdev->dev;
	struct input_handle *handle;
	int ret;

	WARN_ON(linkdev->nb_connected == pdata->nb_devices);

	handle = kzalloc(sizeof(*handle), GFP_KERNEL);
	if (!handle) {
		dev_err(dev, "Failed to allocate handle\n");
		return -ENOMEM;
	}

	handle->dev = input_get_device(idev);
	handle->name = dev_name(&linkdev->pdev->dev);
	handle->handler = handler;

	ret = input_register_handle(handle);
	if (ret) {
		dev_err(dev, "Failed to register handle\n");
		goto err_free_handle;
	}

	ret = input_open_device(handle);
	if (ret) {
		dev_err(dev, "Failed to open device\n");
		goto err_unregister_handle;
	}

	ret = linkdev_drop_other_handlers_from_idev(linkdev, idev, handler);
	if (ret) {
		dev_err(dev, "Unable to drop handlers of device %s\n", idev->name);
		goto err_close_device;
	}

	linkdev->nb_connected++;
	dev_info(dev, "Connected to device %s\n", idev->name);
	return 0;

err_close_device:
	input_close_device(handle);
err_unregister_handle:
	input_unregister_handle(handle);
err_free_handle:
	kfree(handle);
	return ret;
}

static void linkdev_disconnect(struct input_handle *handle)
{
	input_close_device(handle);
	input_unregister_handle(handle);
	kfree(handle);
}

static const struct input_device_id linkdev_ids[] = {
	{ .driver_info = 1 },	/* Matches all devices */
	{ },			/* Terminating zero entry */
};

MODULE_DEVICE_TABLE(input, evdev_ids);

static void linkdev_set_bits(struct linkdev *linkdev)
{
	struct linkdev_platform_data *pdata = linkdev->pdata;
	struct input_dev *idev = linkdev->idev;
	const struct linkdev_pdata_key_map *key_map = pdata->key_map;
	struct input_handle *cur, *next;
	unsigned int i;

	input_alloc_absinfo(idev);

	list_for_each_entry_safe(cur, next, &linkdev->handler.h_list, h_node) {
		dev_dbg(&linkdev->pdev->dev, "Setting bits from device %s\n",
					cur->dev->name);
		for (i = 0; i < pdata->abs_map_size; i++) {
			short int axis, axis_dest;

			if (strcmp(cur->dev->name, pdata->abs_map[i].name))
				continue;

			axis = pdata->abs_map[i].axis;
			if (!test_bit(axis, cur->dev->absbit)) {
				dev_err(&linkdev->pdev->dev, "Axis %i not found on device %s\n",
							axis, cur->dev->name);
				continue;
			}

			axis_dest = pdata->abs_map[i].axis_dest;
			set_bit(axis_dest, idev->absbit);
			memcpy(&idev->absinfo[axis_dest], &cur->dev->absinfo[axis],
						sizeof(struct input_absinfo));
		}

		for (i = 0; i < BITS_TO_LONGS(EV_CNT); i++)
			idev->evbit[i] |= cur->dev->evbit[i];
		for (i = 0; i < BITS_TO_LONGS(KEY_CNT); i++)
			idev->keybit[i] |= cur->dev->keybit[i];
	}

	for (i = 0; i < pdata->key_map_size; i++) {
		const struct input_value *ev = &key_map[i].event;
		input_set_capability(idev, ev->type ?: EV_KEY, ev->code);
		if (ev->type == EV_ABS)
			input_set_abs_params(idev, ev->code, -1, 1, 0, 0);
	}
}

static int linkdev_create_device(struct linkdev *linkdev)
{
	int ret;
	struct input_dev *idev;
	struct platform_device *pdev = linkdev->pdev;
	struct linkdev_platform_data *pdata = linkdev->pdata;

	idev = input_allocate_device();
	if (!idev) {
		dev_err(&linkdev->pdev->dev, "Unable to allocate input device\n");
		return -ENOMEM;
	}

	linkdev->idev = idev;
	idev->id.bustype = BUS_HOST;
	idev->dev.parent = &pdev->dev;
	if (pdata->name)
		idev->name = pdata->name;
	else
		idev->name = "linkdev-device";

	linkdev_set_bits(linkdev);

	ret = input_register_device(idev);
	if (ret) {
		dev_err(&pdev->dev, "Failed to register device: %d\n", ret);
		input_free_device(idev);
		return ret;
	}

	dev_dbg(&linkdev->pdev->dev, "Input device created\n");
	return 0;
}

static ssize_t alt_key_map_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%c\n", alt_key_map ? 'Y' : 'N');
}

static ssize_t alt_key_map_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct linkdev *linkdev = dev_get_drvdata(dev);
	struct input_dev *idev = linkdev->idev;
	bool new_value = false;
	unsigned int i, j;

	if (strtobool(buf, &new_value) < 0)
		return -EINVAL;

	/* When switching between the regular and the alternative key maps,
	 * it is required that the input device does not report any of its keys
	 * as pressed, otherwise the input handlers get confused. To avoid that,
	 * we send a "key released" event for each of the keys we support, even
	 * if the key isn't pressed in the first place. */
	for (i = 0; i < BITS_TO_LONGS(KEY_CNT); i++)
		for (j = 0; j < sizeof(long) * 8; j++)
			if (idev->keybit[i] & BIT(j))
				input_event(idev, EV_KEY,
						i * sizeof(long) * 8 + j, 0);

	alt_key_map = new_value;
	return count;
}

static DEVICE_ATTR_RW(alt_key_map);

static int linkdev_probe(struct platform_device *pdev)
{
	int ret;
	struct linkdev *linkdev;
	struct linkdev_platform_data *pdata = pdev->dev.platform_data;

	if (!pdata) {
		dev_err(&pdev->dev, "Missing platform data\n");
		return -ENXIO;
	}

	linkdev = pdata->__private;

	if (!linkdev) {
		struct input_handler *handler;

		linkdev = kzalloc(sizeof(*linkdev), GFP_KERNEL);
		if (!linkdev) {
			dev_err(&pdev->dev, "Failed to allocate private structure\n");
			return -ENOMEM;
		}

		linkdev->pdata = pdata;
		linkdev->pdev = pdev;

		handler = &linkdev->handler;
		handler->event		= linkdev_event;
		handler->events		= linkdev_events;
		handler->match		= linkdev_match;
		handler->connect	= linkdev_connect;
		handler->disconnect	= linkdev_disconnect;
		handler->name		= "linkdev-handler";
		handler->id_table	= linkdev_ids;
		handler->private	= linkdev;
		INIT_LIST_HEAD(&handler->h_list);

		ret = input_register_handler(handler);
		if (ret) {
			dev_err(&pdev->dev, "Failed to register handler: %d\n", ret);
			goto err_free_linkdev;
		}

		/* pdata->private survives the probe defer, that's why we use it here */
		pdata->__private = linkdev;

		dev_info(&pdev->dev, "linkdev handler successfully registered\n");
	}

	/* We don't want to create the global input device before the
	 * other input device connected to our "linkdev" handler, so
	 * we defer the probe until all the devices are connected. */
	if (linkdev->nb_connected < pdata->nb_devices)
		return -EPROBE_DEFER;

	platform_set_drvdata(pdev, linkdev);

	ret = device_create_file(&pdev->dev, &dev_attr_alt_key_map);
	if (ret) {
		dev_err(&pdev->dev, "Unable to create sysfs node: %d\n", ret);
		goto err_unregister_handler;
	}

	ret = linkdev_create_device(linkdev);
	if (ret) {
		dev_err(&pdev->dev, "Failed to create input device: %d\n", ret);
		goto err_remove_file;
	}

	return 0;

err_remove_file:
	device_remove_file(&pdev->dev, &dev_attr_alt_key_map);
err_unregister_handler:
	input_unregister_handler(&linkdev->handler);
err_free_linkdev:
	kfree(linkdev);
	return ret;
}

static int linkdev_remove(struct platform_device *pdev)
{
	struct linkdev *linkdev = platform_get_drvdata(pdev);

	device_remove_file(&pdev->dev, &dev_attr_alt_key_map);
	input_unregister_handler(&linkdev->handler);
	input_unregister_device(linkdev->idev);
	return 0;
}

static struct platform_driver linkdev_driver = {
	.driver		= {
		.name	= "linkdev",
		.owner	= THIS_MODULE,
	},
	.probe		= linkdev_probe,
	.remove		= linkdev_remove,
};

static int __init linkdev_init(void)
{
	return platform_driver_register(&linkdev_driver);
}

static void __exit linkdev_exit(void)
{
	platform_driver_unregister(&linkdev_driver);
}

late_initcall(linkdev_init);
module_exit(linkdev_exit);

MODULE_ALIAS("platform:linkdev");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Paul Cercueil <paul@crapouillou.net>");
MODULE_DESCRIPTION("Input device merger interface");

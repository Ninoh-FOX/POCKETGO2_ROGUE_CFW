/*
 * Copyright (c) 2014 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Driver for Memsic MXC6225 Thermal Accelerometer
 */

#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input-polldev.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_data/mxc6225.h>

#define MXC6225_REG_XOUT	0x0
#define MXC6225_REG_YOUT	0x1
#define MXC6225_REG_STATUS	0x2
#define MXC6225_REG_RANGE	0x3
#define MXC6225_REG_DETECTION	0x4

#define MXC6225_STATUS_OR_0DEGREES	0x00
#define MXC6225_STATUS_OR_90DEGREES	0x01
#define MXC6225_STATUS_OR_180DEGREES	0x02
#define MXC6225_STATUS_OR_270DEGREES	0x03

#define MXC6225_STATUS_ORI_0DEGREES	0x00
#define MXC6225_STATUS_ORI_90DEGREES	0x04
#define MXC6225_STATUS_ORI_180DEGREES	0x08
#define MXC6225_STATUS_ORI_270DEGREES	0x0c

#define MXC6225_STATUS_TILT		0x10

#define MXC6225_STATUS_SH_NO_SHAKE	0x00
#define MXC6225_STATUS_SH_SHAKE_LEFT	0x20
#define MXC6225_STATUS_SH_SHAKE_RIGHT	0x40

#define MXC6225_STATUS_INT		0x80

/*
 * NOTE: The range register is not documented in any datasheet,
 * it has been reverse-engineered; some other bits might be worthwile.
 *
 * Also, the mxc6225 does not seem to be able to measure beyond
 * -90°/+90°, so the 360° mode will only return at most -64/+63
 * in each direction.
 */
#define MXC6225_RANGE_360_DEGREES	0x00
#define MXC6225_RANGE_180_DEGREES	0x02
#define MXC6225_RANGE_120_DEGREES	0x20
#define MXC6225_RANGE_90_DEGREES	0x22

#define MXC6225_DETECTION_ORC_16	0x00
#define MXC6225_DETECTION_ORC_32	0x01
#define MXC6225_DETECTION_ORC_64	0x02
#define MXC6225_DETECTION_ORC_128	0x03

#define MXC6225_DETECTION_SHC_16	0x00
#define MXC6225_DETECTION_SHC_32	0x04
#define MXC6225_DETECTION_SHC_64	0x08
#define MXC6225_DETECTION_SHC_128	0x0c

#define MXC6225_DETECTION_SHTH_0_5G	0x00
#define MXC6225_DETECTION_SHTH_1_0G	0x10
#define MXC6225_DETECTION_SHTH_1_5G	0x20
#define MXC6225_DETECTION_SHTH_2_0G	0x30

#define MXC6225_DETECTION_SHM		0x40

#define MXC6225_DETECTION_PD		0x80

#define MXC6225_MIN_AXIS	(-128)
#define MXC6225_MAX_AXIS	(+126)
#define MXC6225_FUZZ	4
#define MXC6225_FLAT	(-1)
#define MXC6225_POLL_INTERVAL 16

static unsigned int chip_orientation;
module_param(chip_orientation, uint, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(chip_orientation, "Set the chip orientation");

struct mxc6225_device {
	struct input_polled_dev *poll_dev;
	struct i2c_client *i2c_client;
	bool is_open;

	s8 last_x, last_y;
};

static int mxc6225_write_detection(struct i2c_client *client, u8 data)
{
	s32 ret = i2c_smbus_write_byte_data(client,
					    MXC6225_REG_DETECTION, data);
	if (ret < 0)
		dev_warn(&client->dev, "I2C write failed: %d\n", ret);
	return ret;
}

static int mxc6225_enable(struct i2c_client *client)
{
	return mxc6225_write_detection(client, 0);
}

static int mxc6225_disable(struct i2c_client *client)
{
	return mxc6225_write_detection(client, MXC6225_DETECTION_PD);
}

static void mxc6225_open(struct input_polled_dev *input)
{
	struct mxc6225_device *mxc = input->private;

	mxc->is_open = true;
	mxc6225_enable(mxc->i2c_client);
}

static void mxc6225_close(struct input_polled_dev *input)
{
	struct mxc6225_device *mxc = input->private;

	mxc6225_disable(mxc->i2c_client);
	mxc->is_open = false;
}

static void mxc6225_poll(struct input_polled_dev *input)
{
	struct mxc6225_device *mxc = input->private;
	struct i2c_client *client = mxc->i2c_client;
	bool new_x = false, new_y = false;
	s8 x, y;
	s32 data = 0;

	if (i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_WORD_DATA)) {
		data = i2c_smbus_read_word_data(client, MXC6225_REG_XOUT);
		if (data < 0)
			goto read_fail;
		x = (s8)data;
		y = (s8)(data >> 8);
	} else {
		data = i2c_smbus_read_byte_data(client, MXC6225_REG_XOUT);
		if (data < 0)
			goto read_fail;
		x = (s8)data;
		data = i2c_smbus_read_byte_data(client, MXC6225_REG_YOUT);
		if (data < 0)
			goto read_fail;
		y = (s8)data;
	}

	dev_dbg(&input->input->dev, "Polled values: %hhi %hhi\n", x, y);

	if (x != mxc->last_x) {
		if (chip_orientation & 2)
			x = (s8) (-2 - (s16) x);
		if (chip_orientation & 1)
			input_event(input->input, EV_ABS, ABS_Y, x);
		else
			input_event(input->input, EV_ABS, ABS_X, x);
		mxc->last_x = x;
		new_x = true;
	}

	if (y != mxc->last_y) {
		if (chip_orientation & 2)
			y = (s8) (-2 - (s16) y);
		if (chip_orientation & 1)
			input_event(input->input, EV_ABS, ABS_X, y);
		else
			input_event(input->input, EV_ABS, ABS_Y, y);
		mxc->last_y = y;
		new_y = true;
	}

	if (new_x || new_y)
		input_sync(input->input);

	return;

read_fail:
	dev_warn(&client->dev, "I2C read failed: %d\n", data);
}

static int mxc6225_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	const struct mxc6225_platform_data *pdata;
	struct mxc6225_device *mxc;
	struct input_polled_dev *poll_dev;
	struct input_dev *input_dev;
	int error, fuzz, flat, poll_interval;

	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_SMBUS_BYTE_DATA)) {
		dev_err(&client->dev, "Incapable I2C adapter\n");
		return -ENODEV;
	}

	/* Disable the chip by default, to save power */
	error = mxc6225_disable(client);
	if (error < 0) {
		dev_err(&client->dev, "Write failed; assuming no device\n");
		return -ENODEV;
	}

	pdata = dev_get_platdata(&client->dev);
	if (pdata) {
		fuzz = pdata->fuzz ?: MXC6225_FUZZ;
		flat = pdata->flat ?: MXC6225_FLAT;
		poll_interval = pdata->poll_interval ?: MXC6225_POLL_INTERVAL;
		chip_orientation = pdata->chip_orientation;
	} else {
		fuzz = MXC6225_FUZZ;
		flat = MXC6225_FLAT;
		poll_interval = MXC6225_POLL_INTERVAL;
	}

	mxc = devm_kzalloc(&client->dev, sizeof(*mxc), GFP_KERNEL);
	poll_dev = input_allocate_polled_device();
	if (!mxc || !poll_dev) {
		dev_err(&client->dev,
			"Can't allocate memory for device structure\n");
		return -ENOMEM;
	}

	poll_dev->open = mxc6225_open;
	poll_dev->close = mxc6225_close;
	poll_dev->poll = mxc6225_poll;
	poll_dev->poll_interval = poll_interval;

	mxc->i2c_client = client;
	mxc->poll_dev = poll_dev;
	poll_dev->private = mxc;
	dev_set_drvdata(&client->dev, mxc);

	input_dev = poll_dev->input;

	input_dev->name = "mxc6225";
	input_dev->id.bustype = BUS_I2C;
	input_dev->dev.parent = &client->dev;

	__set_bit(EV_ABS, input_dev->evbit);

	input_set_abs_params(input_dev, ABS_X,
		MXC6225_MIN_AXIS, MXC6225_MAX_AXIS, fuzz, flat);
	input_set_abs_params(input_dev, ABS_Y,
		MXC6225_MIN_AXIS, MXC6225_MAX_AXIS, fuzz, flat);

	/* Use a 180° range by default */
	i2c_smbus_write_byte_data(client, MXC6225_REG_RANGE,
			MXC6225_RANGE_180_DEGREES);

	error = input_register_polled_device(poll_dev);
	if (error) {
		dev_err(&client->dev, "Failed to register input device\n");
		input_free_polled_device(poll_dev);
		return error;
	}

	i2c_set_clientdata(client, mxc);
	return 0;
}

static int mxc6225_remove(struct i2c_client *client)
{
	struct mxc6225_device *mxc = i2c_get_clientdata(client);

	if (mxc->is_open)
		mxc6225_disable(client);
	input_unregister_polled_device(mxc->poll_dev);
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int mxc6225_suspend(struct device *dev)
{
	struct mxc6225_device *mxc = dev_get_drvdata(dev);

	if (mxc->is_open)
		mxc6225_disable(mxc->i2c_client);
	return 0;
}

static int mxc6225_resume(struct device *dev)
{
	struct mxc6225_device *mxc = dev_get_drvdata(dev);

	if (mxc->is_open)
		mxc6225_enable(mxc->i2c_client);
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(mxc6225_pm_ops, mxc6225_suspend, mxc6225_resume);

static const struct i2c_device_id mxc6225_id[] = {
	{ "mxc6225", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, mxc6225_id);

static struct i2c_driver mxc6225_driver = {
	.driver = {
		.name	= "mxc6225",
		.owner	= THIS_MODULE,
		.pm	= &mxc6225_pm_ops,
	},
	.probe		= mxc6225_probe,
	.remove		= mxc6225_remove,
	.id_table	= mxc6225_id,
};

module_i2c_driver(mxc6225_driver);

MODULE_ALIAS("mxc6225");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Paul Cercueil <paul@crapouillou.net>");
MODULE_DESCRIPTION("Memsic MXC6225 thermal accelerometer driver");

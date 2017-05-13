/**
 * @file		i2c_l3gd20.c
 * @brief		Linux Kernel Module Template
 *
 * @author		T. Ngtk
 * @copyright	Copyright (c) 2017 T. Ngtk
 *
 * @par License
 *	Released under the MIT and GPL Licenses.
 *	- https://github.com/ngtkt0909/linux-kernel-module-template/blob/master/LICENSE-MIT
 *	- https://github.com/ngtkt0909/linux-kernel-module-template/blob/master/LICENSE-GPL
 */

#include <linux/module.h>	/* MODULE_*, module_* */
#include <linux/slab.h>		/* kzalloc(), kfree() */
#include <linux/i2c.h>		/* i2c_* */
#include "i2c_l3gd20.h"

/*------------------------------------------------------------------------------
	Prototype Declaration
------------------------------------------------------------------------------*/
/* external function */
static int i2cProbe(struct i2c_client *client, const struct i2c_device_id *id);
static int i2cRemove(struct i2c_client *client);

/* internal function */
static int sInitializeDev(struct i2c_client *client);
static int sGyroShow(struct device *dev, struct device_attribute *attr, char *buf);
static int16_t sI2cSnsRead(struct i2c_client *client, uint8_t reg_h, uint8_t reg_l);

/*------------------------------------------------------------------------------
	Defined Macros
------------------------------------------------------------------------------*/
#define D_DEV_NAME			"i2c_l3gd20"	/**< device name */

/*------------------------------------------------------------------------------
	Type Definition
------------------------------------------------------------------------------*/
/** @brief private data */
typedef struct t_private_data {
	struct i2c_client *client;
} T_PRIVATE_DATA;

/*------------------------------------------------------------------------------
	Global Variables
------------------------------------------------------------------------------*/
/** instance of device file */
static DEVICE_ATTR(gyro, S_IRUSR | S_IRGRP | S_IROTH, sGyroShow, NULL);

/** I2C ID list */
static const struct i2c_device_id g_i2c_dev_id_list[] = {
	/* device name, device id */
	{ D_DEV_NAME, 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, g_i2c_dev_id_list);

/** I2C address list */
static const unsigned short g_i2c_addr_list[] = {
	ADDR_L3GD20,
	I2C_CLIENT_END
};

/** I2C driver registration */
static struct i2c_driver g_i2c_driver = {
	.probe        = i2cProbe,
	.remove       = i2cRemove,
	.id_table     = g_i2c_dev_id_list,
	.address_list = g_i2c_addr_list,
	.driver = {
		.owner = THIS_MODULE,
		.name  = D_DEV_NAME,
	},
};
module_i2c_driver(g_i2c_driver);

/*------------------------------------------------------------------------------
	Macro Calls
------------------------------------------------------------------------------*/
MODULE_AUTHOR("T. Ngtk");
MODULE_DESCRIPTION("I2C L3GD20 driver");
MODULE_LICENSE("Dual MIT/GPL");

/*------------------------------------------------------------------------------
	Functions (External)
------------------------------------------------------------------------------*/
/**
 * @brief Kernel Module Prove
 *
 * @param [in]		client	i2c_client structure
 * @param [in]		id		i2c_device_id structure
 *
 * @retval 0		success
 * @retval others	failure
 */
static int i2cProbe(struct i2c_client *client, const struct i2c_device_id *id)
{
	T_PRIVATE_DATA *data;
	int ret;

	printk(KERN_INFO "%s: probing ... addr=%d\n", D_DEV_NAME, client->addr);

	/* check I2C I/F functionality */
	ret = i2c_check_functionality(client->adapter,
			I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_I2C_BLOCK);
	if (ret == 0) {
		printk(KERN_ERR "%s: i2c_check_functionality failed\n", __func__);
		return -ENODEV;
	}

	/* initialize device */
	ret = sInitializeDev(client);
	if (ret) {
		printk(KERN_ERR "%s: sInitializeDev failed\n", __func__);
		return -ENODEV;
	}

	/* allocate private data */
	data = kzalloc(sizeof(T_PRIVATE_DATA), GFP_KERNEL);
	if (data == NULL) {
		printk(KERN_ERR "%s: kzalloc failed\n", __func__);
		return -ENOMEM;
	}

	data->client = client;
	i2c_set_clientdata(client, data);

	/* create device file */
	ret = device_create_file(&client->dev, &dev_attr_gyro);
	if (ret) {
		printk(KERN_ERR "%s: device_create_file failed\n", __func__);
	}

	return 0;
}

/**
 * @brief Kernel Module Remove
 *
 * @param [in]		client	i2c_client structure
 *
 * @retval 0		success
 * @retval others	failure
 */
static int i2cRemove(struct i2c_client *client)
{
	T_PRIVATE_DATA *data = (T_PRIVATE_DATA *)i2c_get_clientdata(client);

	printk(KERN_INFO "%s: removing ... \n", D_DEV_NAME);

	/* remove device file */
	device_remove_file(&client->dev,  &dev_attr_gyro);

	/* deallocate private data */
	kfree(data);

	return 0;
}

/*------------------------------------------------------------------------------
	Functions (Internal)
------------------------------------------------------------------------------*/
/**
 * @brief Initialize Device
 *
 * @param [in]		client	i2c_client structure
 *
 * @retval 0		success
 * @retval others	failure
 */
static int sInitializeDev(struct i2c_client *client)
{
	uint8_t val;
	int ret;

	/* check connection */
	val = i2c_smbus_read_byte_data(client, REG_WHO_AM_I);
	if (val != VAL_WHO_AM_I) {
		return -ENODEV;
	}

	/* initialize device */
	ret = i2c_smbus_write_byte_data(client, REG_CTRL_REG1, VAL_CTRL_REG1);
	if (ret < 0) {
		printk(KERN_ERR "%s: i2c_smbus_write_byte_data failed\n", __func__);
		return -ENODEV;
	}

	return 0;
}

/**
 * @brief Call-back Function for Reading
 *
 * @param [in]		dev		device structure
 * @param [in]		attr	device_attribute structure
 * @param [in]		buf		buffer address
 *
 * @return	number of read byte
 */
static int sGyroShow(struct device *dev, struct device_attribute *attr, char *buf)
{
	T_PRIVATE_DATA *data = (T_PRIVATE_DATA *)dev_get_drvdata(dev);
	int16_t val_x, val_y, val_z;

	/* read 3-axis gyro sensor */
	val_x = sI2cSnsRead(data->client, REG_OUT_X_H, REG_OUT_X_L);
	val_y = sI2cSnsRead(data->client, REG_OUT_Y_H, REG_OUT_Y_L);
	val_z = sI2cSnsRead(data->client, REG_OUT_Z_H, REG_OUT_Z_L);

	return snprintf(buf, PAGE_SIZE, "%d,%d,%d", val_x, val_y, val_z);
}

/**
 * @brief I2C Read Register
 *
 * @param [in]		client	i2c_client structure
 * @param [in]		reg_h	register address (high byte)
 * @param [in]		reg_l	register address (low byte)
 *
 * @return	value of register
 */
static int16_t sI2cSnsRead(struct i2c_client *client, uint8_t reg_h, uint8_t reg_l)
{
	uint8_t val_h, val_l;

	val_h = i2c_smbus_read_byte_data(client, reg_h);
	val_l = i2c_smbus_read_byte_data(client, reg_l);

	return (int16_t)(((val_h & 0xFF) << 8) | (val_l & 0xFF));
}

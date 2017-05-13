/**
 * @file		spi_l3gd20.c
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
#include <linux/mutex.h>	/* mutex_lock(), mutex_unlock() */
#include <linux/spi/spi.h>	/* spi_* */
#include "spi_l3gd20.h"

/*------------------------------------------------------------------------------
	Prototype Declaration
------------------------------------------------------------------------------*/
/* external function */
static int spiInit(void);
static void spiExit(void);
static int spiProbe(struct spi_device *spi);
static int spiRemove(struct spi_device *spi);

/* internal function */
static void sSpiRemoveDev(struct spi_master *master, unsigned int cs);
static int sInitializeDev(struct spi_device *spi);
static int sGyroShow(struct device *dev, struct device_attribute *attr, char *buf);
static int16_t sSpiSnsRead(struct spi_device *spi, uint8_t reg_h, uint8_t reg_l);

/*------------------------------------------------------------------------------
	Defined Macros
------------------------------------------------------------------------------*/
#define D_DEV_NAME			"spi_l3gd20"	/**< device name */
#define D_SPI_SPEED_HZ		1000000			/**< SPI clock frequency [Hz] */
#define D_SPI_BUS			0				/**< SPI bus id (default) */
#define D_SPI_CS			0				/**< SPI chip select (default) */
#define D_SPI_MODE			SPI_MODE_3		/**< SPI mode (default) */

/*------------------------------------------------------------------------------
	Type Definition
------------------------------------------------------------------------------*/
/** @brief private data */
typedef struct t_private_data {
	struct spi_device *spi;
	struct mutex lock;
} T_PRIVATE_DATA;

/*------------------------------------------------------------------------------
	Global Variables
------------------------------------------------------------------------------*/
/** instance of device file */
static DEVICE_ATTR(gyro, S_IRUSR | S_IRGRP | S_IROTH, sGyroShow, NULL);

/** SPI ID list */
static struct spi_device_id g_spi_dev_id_list[] = {
	/* device name, device id */
	{ D_DEV_NAME, 0 },
	{ },
};
MODULE_DEVICE_TABLE(spi, g_spi_dev_id_list);

/** SPI driver registration */
static struct spi_driver g_spi_driver = {
	.probe    = spiProbe,
	.remove   = spiRemove,
	.id_table = g_spi_dev_id_list,
	.driver = {
		.owner = THIS_MODULE,
		.name  = D_DEV_NAME,
	},
};

/** SPI board info */
static struct spi_board_info g_spi_board_info = {
	.modalias     = D_DEV_NAME,
	.max_speed_hz = D_SPI_SPEED_HZ,
	.bus_num      = D_SPI_BUS,
	.chip_select  = D_SPI_CS,
	.mode         = D_SPI_MODE,
};

/* export variables */
static int spi_bus = D_SPI_BUS;		/**< SPI bus id */
static int spi_cs  = D_SPI_CS;		/**< SPI chip select */
module_param(spi_bus, int, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
module_param(spi_cs, int, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);

/*------------------------------------------------------------------------------
	Macro Calls
------------------------------------------------------------------------------*/
module_init(spiInit);
module_exit(spiExit);

MODULE_AUTHOR("T. Ngtk");
MODULE_DESCRIPTION("SPI L3GD20 driver");
MODULE_LICENSE("Dual MIT/GPL");

/*------------------------------------------------------------------------------
	Functions (External)
------------------------------------------------------------------------------*/
/**
 * @brief Kernel Module Init
 *
 * @param nothing
 *
 * @retval 0		success
 * @retval others	failure
 */
static int spiInit(void)
{
	struct spi_master *master;
	struct spi_device *spi_device;
	int ret;

	printk(KERN_INFO "%s: initializing ...\n", D_DEV_NAME);

	/* register SPI driver */
	ret = spi_register_driver(&g_spi_driver);
	if (ret) {
		printk(KERN_ERR "%s: spi_register_driver failed\n", __func__);
		return -ENODEV;
	}

	/* get spi_master from spi_board_info */
	g_spi_board_info.bus_num     = spi_bus;
	g_spi_board_info.chip_select = spi_cs;
	master = spi_busnum_to_master(g_spi_board_info.bus_num);
	if (!master) {
		printk(KERN_ERR "%s: spi_busnum_to_master failed\n", __func__);
		spi_unregister_driver(&g_spi_driver);
		return -ENODEV;
	}

	/* remove already occupying SPI device */
	sSpiRemoveDev(master, g_spi_board_info.chip_select);

	/* register SPI device to bus (if succeeded, probe() is called) */
	spi_device = spi_new_device(master, &g_spi_board_info);
	if (!spi_device) {
		printk(KERN_ERR "%s: spi_new_device failed\n", __func__);
		spi_unregister_driver(&g_spi_driver);
		return -ENODEV;
	}

	return 0;
}

/**
 * @brief Kernel Module Exit
 *
 * @param nothing
 *
 * @retval nothing
 */
static void spiExit(void)
{
	struct spi_master *master;

	printk(KERN_INFO "%s: exiting ...\n", D_DEV_NAME);

	/* remove SPI device */
	master = spi_busnum_to_master(g_spi_board_info.bus_num);
	if (!master) {
		printk(KERN_ERR "%s: spi_busnum_to_master failed\n", __func__);
	} else {
		sSpiRemoveDev(master, g_spi_board_info.chip_select);
	}

	/* unregister SPI driver */
	spi_unregister_driver(&g_spi_driver);
}

/**
 * @brief Kernel Module Prove
 *
 * @param [in]		spi		spi_device structure
 *
 * @retval 0		success
 * @retval others	failure
 */
static int spiProbe(struct spi_device *spi)
{
	T_PRIVATE_DATA *data;
	int ret;
	
	printk(KERN_INFO "%s: probing ...\n", D_DEV_NAME);

	/* setup SPI */
	spi->max_speed_hz  = g_spi_board_info.max_speed_hz;
	spi->mode          = g_spi_board_info.mode;
	spi->bits_per_word = 8;

	ret = spi_setup(spi);
	if (ret) {
		printk(KERN_ERR "%s: spi_setup failed\n", __func__);
		return -ENODEV;
	}

	/* initialize device */
	ret = sInitializeDev(spi);
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

	data->spi = spi;
	mutex_init(&data->lock);
	spi_set_drvdata(spi, data);

	/* create device file */
	ret = device_create_file(&spi->dev, &dev_attr_gyro);
	if (ret) {
		printk(KERN_ERR "%s: device_create_file failed\n", __func__);
	}

	return 0;
}

/**
 * @brief Kernel Module Remove
 *
 * @param [in]		spi		spi_device structure
 *
 * @retval 0		success
 * @retval others	failure
 */
static int spiRemove(struct spi_device *spi)
{
	T_PRIVATE_DATA *data = (T_PRIVATE_DATA *)spi_get_drvdata(spi);

	printk(KERN_INFO "%s: removing ... \n", D_DEV_NAME);

	/* remove device file */
	device_remove_file(&spi->dev, &dev_attr_gyro);
	
	/* deallocate private data */
	kfree(data);

	return 0;
}

/*------------------------------------------------------------------------------
	Functions (Internal)
------------------------------------------------------------------------------*/
/**
 * @brief Remove Device
 *
 * @param [in]		spi		spi_master structure
 * @param [in]		cs		chip select
 *
 * @retval nothing
 */
static void sSpiRemoveDev(struct spi_master *master, unsigned int cs)
{
	struct device *dev;
	char str[128];

	snprintf(str, sizeof(str), "%s.%u", dev_name(&master->dev), cs);

	/* if SPI device is found, remove it */
	dev = bus_find_device_by_name(&spi_bus_type, NULL, str);
	if (dev) {
		printk(KERN_INFO "delete %s\n", str);
		device_del(dev);
	}
}

/**
 * @brief Initialize Device
 *
 * @param [in]		spi		spi_device structure
 *
 * @retval 0		success
 * @retval others	failure
 */
static int sInitializeDev(struct spi_device *spi)
{
	unsigned char tx[2];
	unsigned char rx[2];
	int ret;

	/* check connection */
	tx[0] = REG_WHO_AM_I | D_SPI_READ | D_SPI_SINGLE;
	ret = spi_write_then_read(spi, tx, 1, rx, 1);
	if (ret) {
		printk(KERN_INFO "%s: spi_write_then_read failed\n", __func__);
		return -ENODEV;
	} else if (rx[0] != VAL_WHO_AM_I) {
		printk(KERN_INFO "%s: illegal who_am_i=0x%02x\n", __func__, rx[0]);
		return -ENODEV;
	}

	/* initialize device */
	tx[0] = REG_CTRL_REG1 | D_SPI_WRITE | D_SPI_SINGLE;
	tx[1] = VAL_CTRL_REG1;
	ret = spi_write(spi, tx, 2);
	if (ret) {
		printk(KERN_INFO "%s: spi_write failed\n", __func__);
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

	/* critical section begin */
	mutex_lock(&data->lock);

	/* read 3-axis gyro sensor */
	val_x = sSpiSnsRead(data->spi, REG_OUT_X_H, REG_OUT_X_L);
	val_y = sSpiSnsRead(data->spi, REG_OUT_Y_H, REG_OUT_Y_L);
	val_z = sSpiSnsRead(data->spi, REG_OUT_Z_H, REG_OUT_Z_L);

	/* critical section end */
	mutex_unlock(&data->lock);

	return snprintf(buf, PAGE_SIZE, "%d,%d,%d", val_x, val_y, val_z);
}

/**
 * @brief SPI Read Register
 *
 * @param [in]		client	spi_device structure
 * @param [in]		reg_h	register address (high byte)
 * @param [in]		reg_l	register address (low byte)
 *
 * @return	value of register
 */
static int16_t sSpiSnsRead(struct spi_device *spi, uint8_t reg_h, uint8_t reg_l)
{
	unsigned char tx;
	unsigned char rx_h, rx_l;
	int ret;

	/* high byte of sensor value */
	tx = reg_h | D_SPI_READ | D_SPI_SINGLE;
	ret = spi_write_then_read(spi, &tx, 1, &rx_h, 1);
	if (ret) {
		printk(KERN_INFO "%s: spi_write_then_read failed\n", __func__);
	}

	/* low byte of sensor value */
	tx = reg_l | D_SPI_READ | D_SPI_SINGLE;
	ret = spi_write_then_read(spi, &tx, 1, &rx_l, 1);
	if (ret) {
		printk(KERN_INFO "%s: spi_write_then_read failed\n", __func__);
	}

	return (int16_t)(((rx_h & 0xFF) << 8) | (rx_l & 0xFF));
}

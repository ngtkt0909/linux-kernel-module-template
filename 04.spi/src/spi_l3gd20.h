/**
 * @file		spi_l3gd20.h
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

#ifndef __SPI_L3GD20_H__
#define __SPI_L3GD20_H__			/**< include guard */

#define ADDR_L3GD20		0x6A		/**< slave address */

#define REG_WHO_AM_I	0x0F		/**< register address: WHO_AM_I */
#define REG_CTRL_REG1	0x20		/**< register address: CTRL_REG1 */
#define REG_OUT_X_L		0x28		/**< register address: OUT_X_L */
#define REG_OUT_X_H		0x29		/**< register address: OUT_X_H */
#define REG_OUT_Y_L		0x2A		/**< register address: OUT_Y_L */
#define REG_OUT_Y_H		0x2B		/**< register address: OUT_Y_H */
#define REG_OUT_Z_L		0x2C		/**< register address: OUT_Z_L */
#define REG_OUT_Z_H		0x2D		/**< register address: OUT_Z_H */

#define VAL_WHO_AM_I	0xD4		/**< value: WHO_AM_I */
#define VAL_CTRL_REG1	0x0F		/**< value: CTRL_REG1 */

#define DPS_PER_LSB		0.00875		/**< degree per second / LSB */

#define D_SPI_READ		0x80		/**< data type flag : read */
#define D_SPI_WRITE		0x00		/**< data type flag : write */
#define D_SPI_MULTI		0x40		/**< data type flag : multi */
#define D_SPI_SINGLE	0x00		/**< data type flag : single */

#endif	/* __SPI_L3GD20_H__ */

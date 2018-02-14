/* 
 * File:   hal_i2c.h
 * Author: Anthony Merlino
 *
 * Created on February 26, 2015, 1:31 PM
 */

#ifndef _HAL_I2C_H_
#define _HAL_I2C_H_

#include <stdint.h>

#define I2C_B0 0
#define I2C_B1 1

typedef struct i2c_settings_t {
	uint8_t channel;
	uint32_t bit_rate;
	uint8_t address_length : 1;
}i2c_settings_t;

#endif //_HAL_I2C_H_

/* 
 * File:   hal_i2c.h
 * Author: Anthony Merlino
 *
 * Created on February 26, 2015, 1:31 PM
 */

#ifndef _HAL_I2C_H_
#define _HAL_I2C_H_

#include <stdint.h>

#define I2C0 0
#define I2C1 1
#define I2C2 2
#define I2C3 3

typedef struct i2c_settings_t {
	uint8_t channel;
}i2c_settings_t;

#endif //_HAL_I2C_H_

/**
 * @defgroup accelerometer Accelerometer module
 *
 *
 *  Created on: March 25, 2019
 *  Updated on: April 12, 2019
 *      Author: Stephen Glass, Lonnie Souder
 *
 * @{
 * @ brief Code to read data from an accelerometer. Designed for
 * SparkFun Triple Axis Accelerometer Breakout (MMA8452Q)
 * @author Stephen Glass
 * @author Lonnie Souder
 * @version 1.0
 *
 *
 * following macros should be defined project_settings
 *   #define I2C_MAX_TX_SIZE 2
 *   #define I2C_MAX_RX_SIZE 6
 */

#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

#include <stdint.h>
#include "project_settings.h"
#include "task.h"
#include "hal_general.h"

#ifndef USE_MODULE_TASK
#error "USE_MODULE_TASK not defined in project_settings.h. Accelerometer not able to function without."
#endif

/** @brief Initialize the accelerometer to read data at a period returning to a callback
 *
 * @param period period to check data from accelerometer (milliseconds)
 * @param period function pointer for callback where data is returned
 * @return returns success of initialization, (< 0 is error)
 */
int8_t Accelerometer_Init(uint16_t period, void(*callback)(float x, float y, float z));

/** @brief Deinitialize the accelerometer task scheduler
 */
void Accelerometer_Deinit(void);


 /** @} */

#endif // _ACCELEROMETER_H_

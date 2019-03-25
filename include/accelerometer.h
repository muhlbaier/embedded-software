/**
 * @defgroup accelerometer Accelerometer module
 * 
 * 
 *  Created on: March 25, 2019
 *      Author: Stephen Glass
 *
 * @{
 * @ brief Code to read data from an accelerometer. Designed for
 * SparkFun Triple Axis Accelerometer Breakout (MMA8452Q)
 * @author Stephen Glass
 * @version 0.1
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
 */
void Accelerometer_Init(uint16_t period, void(*callback)(uint16_t x, uint16_t y, uint16_t z));

/** @brief Deinitialize the accelerometer task scheduler
 */
void Accelerometer_Deinit(void);


 /** @} */

#endif // _ACCELEROMETER_H_
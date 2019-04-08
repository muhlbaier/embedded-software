
#ifndef _ULTRASONIC_H_
#define _ULTRASONIC_H_


#include <stdint.h>
#include "timing.h"


/**
 * @defgroup ultrasonic_module Ultrasonic Module
 * @{
 */

/** @brief Initialization of ultrasonic module, returns a uint8_t sensor_handle
 * @param period Period between the end of one measurement and the beginning of another
 * @param filter An optional paramater for to filter any data
 */

uint8_t Ultrasonic_Init(uint16_t period, filter_t * filter, void(*callback)(uint16_t));

/** @brief Deinitialization of specific ultrasonic module
 *
 * @param sensor_handle This will tell the function which sensor needs to be deinitialized/turned off
 */

void Ultrasonic_Deinit(uint8_t sensor_handle);

 


#endif // _ULTRASONIC_H_

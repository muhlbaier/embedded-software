#include <stdint.h>
#include "task.h"
#include "timing.h"


#ifndef _BUZZER_H_
#define _BUZZER_H_

/**
 * @defgroup buzzer_module Buzzer Module
 * @{
 */

/** @brief initializes the buzzer module for use
 * 
 * Initializes the task manager and timing modules to allow the
 * modules to be used within the Buzzer_Sound function
 *
 */

void Buzzer_Init(void);

/** @brief 
 *
 * @param frequency Frequency of the buzzer
 * @param timeout Length at which the buzzer is active
 */

void Buzzer_Sound(uint16_t frequency, uint16_t timeout);

/** @brief hardware abstaction layer buzzer initialization
 *
 * Must be implemented for each MCU in hal_buzzer.c and configure 
 * the pin which will utilize the buzzer
 */
 
void hal_Buzzer_Init(void);
 
 /** @brief hardware abstraction layer to start buzzer and
 * set the frequency
 * 
 */
 
void hal_Start_Buzzer(uint16_t frequency);

 /** @brief hardware abstraction layer to stop buzzer
 * 
 */
 
void hal_Stop_Buzzer(void);
 
 /** @} */

#endif // _BUZZER_H_
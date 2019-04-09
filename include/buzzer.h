#ifndef _BUZZER_H_
#define _BUZZER_H_

#include <stdint.h>

/**
 * @defgroup buzzer_module Buzzer Module
 * 
 * Created on: Mar 26, 2019
 *    Authors: Nikola Kosaric, Nate Hoffman, Brian Harlow, DJ Stahlberger 
 *
 * The following module utilizes the task manager and timing module
 * in order to inact a buzzer to produce a sound at a determined frequency.
 * The timing module is used to control the frequency of the sound produced
 * and the task module is used to control the length of time the buzzer will 
 * sound.
 *
 * In oder to utilize this module, first, the Buzzer_Init must be called.
 * Then, the Buzzer_Sound function must be called in order to determine the
 * frequency at which the buzzer will sound and the length of time the buzzer
 * will sound. The channel used for the buzzer will be determined in the 
 * hardware abstraction layer.
 * 
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
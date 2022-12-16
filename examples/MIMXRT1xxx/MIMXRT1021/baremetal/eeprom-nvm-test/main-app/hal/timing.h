#ifndef _TIMING_H_
#define _TIMING_H_

#include <stdint.h>
#include "project_settings.h"

/** @file
 * @defgroup timing Timing module
 *
 * This module provides some very basic functions to make timing easier. Several
 * other modules rely on the services provided by this module.
 *
 * The module uses the HAL layer to provide the hardware specific functionality for the
 * timing module.  In this module, the hal_timing.c file is included to allow the ISR
 * to access the time variable without externing the variable or making a function call.
 * This allows the ISR to be very efficient.
 *
 * @{
 */

/** Max time that can be held.
 */
#define TIME_MAX 0xFFFFFFFFU

/**
 * @brief Initialize the timing module
 *
 * Call the HAL layer to initialize a timer to interrupt every 1ms.
 * Clears the timing count
 */
void Timing_Init(void);

/** @brief Returns the current system time in milliseconds
 *
 * @return the current system time in milliseconds
 *
 * @see TimeSince
 */
uint32_t TimeNow(void);
uint32_t TimeNowUs(void);

/**
 * @brief Get the elapsed time
 *
 * Calculate and return the time elapsed since a given time. Typically used
 * with TimeNow(). See the example usage.
 *
 * @code
 * uint32_t t;
 * t = TimeNow();
 * while(TimeSince(t) < 5000){
 * 	//Insert code here
 * }
 * @endcode
 *
 * @see TimeNow
 *
 * @param t - The time in milliseconds to calculate the elapsed time since
 * @return - The time elapsed since the time given in milliseconds
 */
uint32_t TimeSince(uint32_t t);
uint32_t TimeSinceUs(uint32_t t);

/** @brief Delay a specific number of milliseconds
 *
 * DelayMs uses TimeNow() and TimeSince() to implement the delay
 *
 * @param delay specifies the number of milliseconds to delay
 */
void DelayMs(uint32_t delay);
void DelayUs(uint32_t us);

/** Reset the system time
 * 
 * Reset system time (e.g. TimeNow() will return 0) and set appropriate 
 * rollover times so TimeSince will work properly.
 * 
 * @warning Timing discrepancies will occur if TimerRoll is called and not called
 * again for the next timer rollover. TimeSince() will result in a shorter time
 * than expected if the timer is allowed to naturally roll over after calling
 * TimerRoll. The difference will be MAX_TIME - the time when TimerRoll was last
 * called. This could be fixed by adding a check for the timer rolling over naturally
 * and setting rollover_time to TIME_MAX in timing.c. Note: This is handled by the
 * task management system which typically will be the only user of Timing_Roll()
 */
void Timing_Roll(void);

/** @} */
#endif // _TIMING_H_

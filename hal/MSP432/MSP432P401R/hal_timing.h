/*
 * hal_timing.h
 *
 *  Created on: Feb 25, 2017
 *      Author: Autumn
 */

#ifndef HAL_TIMING_H_
#define HAL_TIMING_H_

/** @file
 * 
 * @addtogroup hal_timing (HAL) Timing module
 * @ingroup timing
 *
 * This module provides the hardware abstraction needed for the timing module
 *
 * This module needs to do the following:
 * - Define hal_Timing_Init() as detailed below hal_Timing_Init()
 * - Call TimingISR() macro from within the timer ISR and clear the interrupt
 *   flag if required
 *
 * @{
 */

/** Initialize hardware timer peripheral
 *
 */
void hal_Timing_Init(void);

/** @} */
#endif /* HAL_TIMING_H_ */

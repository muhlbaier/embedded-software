/*
 * hal_timing.h
 *
 * Created on: May 5th, 2017
 *     Author: Zach Norris
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
 * This is a copy of the hal_timing used for the CubeSat pic, but used for the
 * dsPIC33FJ128MC802.
 *
 * @{
 */

/** Initialize hardware timer peripheral
 *
 */
void hal_Timing_Init(void);

/** @} */
#endif /* HAL_TIMING_H_ */

/**
 * @file hal_timing.h
 * 
 * @defgroup hal_timing_dspic33ev256gm102 HAL Timing Module for dsPIC33EV256GM102
 * @ingroup timing
 * 
 * This module provides the hardware abstraction needed for the timing module
 * 
 * This module needs to do the following:
 * - Define hal_Timing_Init() as detailed below hal_Timing_Init()
 * - Call TimingISR() macro from within the timer ISR and clear the interrupt
 *   flag if required
 * 
 * Created on: Mar 4, 2019
 *    @author: Nate Hoffman
 * 
 * @version 1.0.0 Initial function definitions
 */

#ifndef HAL_TIMING_H
#define HAL_TIMING_H

/**
 * Initialize the hardware timer peripheral.
 */
void hal_Timing_Init();

#endif // HAL_TIMING_H

/**
 * @file hal_general.h
 * 
 * @defgroup hal_general_dspic33ev256gm102 HAL general configuration for dsPIC33EV256GM102
 * @ingroup general
 * 
 * Configuration of the dsPIC33EV256GM102.
 * 
 * Created on: Mar 4, 2019
 *    @author: Nate Hoffman
 * 
 * @version 1.0.0 Initial general configuration
 */

#ifndef HAL_GENERAL_H
#define HAL_GENERAL_H

#include <xc.h>

// Nop() is defined by the XC16 compiler and is not needed here

// XC16 enable and disable interrupt routines
#define DisableInterrupts() SET_CPU_IPL(7)
#define EnableInterrupts() RESTORE_CPU_IPL(0)

// TODO: Improve the interrupt blocking code
#define BlockInterrupts() int current_ipl; SET_AND_SAVE_CPU_IPL(current_ipl, 7)
#define RestoreInterrupts() RESTORE_CPU_IPL(current_ipl)

#define hal_Watchdog_Clear() ClrWdt()
#define hal_Watchdog_Enable() RCONbits.SWDTEN = 1

#endif // HAL_GENERAL_H

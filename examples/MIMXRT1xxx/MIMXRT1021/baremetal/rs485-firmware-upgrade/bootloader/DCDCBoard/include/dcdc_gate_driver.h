/**
*	@file 	DCDCGateDriver.h
*	@brief  Low level control of peripherals associated with
*         the DCDC gate driver. For high level controls
          see DCDCBridgeController.
*	@date
*	@author	James Ethridge <jethridge@alenconsystems.com>
*
*  Copyright (c) 2016, Alencon Systems
*/





/** DCDCBoard
*	@addtogroup DCDCBoard
*	@{
*/



/** DCDCGateDriver
*	@addtogroup DCDCGateDriver
*	@{
*/

/* ***** Inclusion Gaurd ****** */
#ifndef __DCDCGateDriver_H__
#define __DCDCGateDriver_H__



/* ***** Dependencies ****** */
#include <stdint.h>
/** DCDCGateDriver_Public_Macros
*	@{
*/

/** DCDCGateDriver_Public_Macros
*	@}
*/



/** DCDCGateDriver_Public_Types
*	@{
*/

/** DCDCGateDriver_Public_Types
*	@}
*/

/** DCDCGateDriver_Public_Functions
*	@{
*/

#if defined(__cplusplus)

extern "C" {

#endif /* __cplusplus */

/********************************************************************//**
 * @brief				Initialize the DCDC Gate Driver and enable
                                the associated PWM peripherals
 * @return 			none
 *********************************************************************/
void DCDCGateDriver_Init(uint32_t resonant);
/********************************************************************//**
* @brief			Load the most recent set of configuration parameters into
                            and update the DCDC Gate Drievr PWM registers.
* @return 		none
*********************************************************************/
extern void (*DCDCGateDriver_Update)(void);
/********************************************************************//**
* @brief			Start the DCDC Gate Driver by enabling PWM output
* @param[in]	a handle to the DCDC gate driver controller
* @return 		none
*********************************************************************/
extern void (*DCDCGateDriver_Start)(void);
/********************************************************************//**
* @brief            Stop the DCDC Gate Driver by disabling PWM output
* @param[in]    a handle to the DCDC gate driver controller
* @return       none
*********************************************************************/
extern void (*DCDCGateDriver_Stop)(void);
/********************************************************************//**
* @brief			Get the DCDC Gate Driver params directly from 
* 					controller and timing registers
* @return 		the corresponding parameter value
*********************************************************************/
extern uint32_t (*DCDCGateDriver_get_frequency)(void);
extern uint32_t (*DCDCGateDriver_get_period_ns)(void);
extern uint32_t (*DCDCGateDriver_get_pulse_width_ns)(void);
extern uint32_t (*DCDCGateDriver_get_active_rect_pulse_width_ns)(void);
extern uint32_t (*DCDCGateDriver_get_active_rect_delay_ns)(void);
extern uint32_t (*DCDCGateDriver_get_input_deadtime_ns)(void);
extern uint32_t (*DCDCGateDriver_get_output_deadtime_ns)(void);
extern float (*DCDCGateDriver_get_alpha)(void);
extern uint8_t  (*DCDCGateDriver_get_running)(void);

/** Enable overflow interrupt */
extern void (*DCDCGateDriver_SetInterrupt)(void);

#if defined(__cplusplus)

}
#endif
/** DCDCGateDriver_Public_Functions
*	@}
*/
#endif









/** DCDCGateDriver

*	@}

*/



/** DCDCBoard

*	@}

*/





/* ~~~~~~~~~~~~~~~~~~~~~~~~~ EOF ~~~~~~~~~~~~~~~~~~~~~~~~~ */


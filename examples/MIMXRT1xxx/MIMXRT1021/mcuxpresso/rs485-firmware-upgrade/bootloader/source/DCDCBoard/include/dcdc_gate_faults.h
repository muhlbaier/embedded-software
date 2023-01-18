/**
*	@file 	dcdc_gate_faults.h
*	@brief  Helper functions for gate fault and feedback signals
*	@author	Michael Muhlbaier
*
*  Copyright (c) 2019, Alencon Systems
*/





/** DCDCBoard
*	@addtogroup DCDCBoard
*	@{
*/



/** DCDCGateDriver
*	@addtogroup DCDCGateDriver
*	@{
*/

/** DCDCGateFaults
*	@addtogroup DCDCGateFaults
*	@{
*/

/* ***** Inclusion Gaurd ****** */
#ifndef __DCDC_GATE_FAULTS_H__
#define __DCDC_GATE_FAULTS_H__


/* ***** Dependencies ****** */
#include <stdint.h>

#if defined(__cplusplus)

extern "C" {

#endif /* __cplusplus */

/********************************************************************//**
 * @brief			Check if any fault or feedback signals are active and handle them
 * @param[in]		none
 * @return 			number of fault or feedback signals active
 *********************************************************************/
uint8_t check_gate_faults(void);

#if defined(__cplusplus)

}
#endif
#endif

/** DCDCGateFaults
*	@}
*/

/** DCDCGateDriver
*	@}
*/

/** DCDCBoard
*	@}
*/

/* ~~~~~~~~~~~~~~~~~~~~~~~~~ EOF ~~~~~~~~~~~~~~~~~~~~~~~~~ */

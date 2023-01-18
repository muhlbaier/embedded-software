/**
*	@file 	DCDCGateDriverInteg.h
*	@brief  Low level integration test for DCDC gate driver
*	@date
*	@author	James Ethridge <jethridge@alenconsystems.com>
*
*  Copyright (c) 2016, Alencon Systems
*/

/** DCDCBoard
*	@addtogroup DCDCBoard
*	@{
*/


/** DCDCGateDriverInteg
*	@addtogroup DCDCGateDriverInteg
*	@{
*/

/* ***** Inclusion Gaurd ****** */
#ifndef __DCDCGateDriverInteg_H__
#define __DCDCGateDriverInteg_H__

/* ***** Dependencies ****** */
#include <stdint.h>
/** DCDCGateDriverInteg_Public_Macros
*	@{
*/

/** DCDCGateDriverInteg_Public_Macros
*	@}
*/


/** DCDCGateDriverInteg_Public_Types
*	@{
*/
typedef struct dcdc_gate_driver_integ_t{
	uint32_t id;
}dcdc_gate_driver_integ_t;

/** DCDCGateDriverInteg_Public_Types
*	@}
*/


/** DCDCGateDriverInteg_Public_Functions
*	@{
*/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */


/********************************************************************//**
 * @brief
 * @param[in]
 * @return 		none
 *********************************************************************/
void DCDCGateDriverInteg_init(void);
void DCDCGateDriverInteg_start(void);
void DCDCGateDriverInteg_update(uint32_t tickms);
void DCDCGateDriverInteg_stop(void);

#if defined(__cplusplus)

}
#endif
/** DCDCGateDriverInteg_Public_Functions
*	@}
*/
#endif



/** DCDCGateDriverInteg
*	@}
*/


/** DCDCBoard
*	@}
*/

/* ~~~~~~~~~~~~~~~~~~~~~~~~~ EOF ~~~~~~~~~~~~~~~~~~~~~~~~~ */

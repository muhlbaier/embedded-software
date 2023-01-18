/**
*	@file CmdDispatcher.c
*	@brief Command Dispatcher for the DCDC Comms.
*	@date		Nov 25, 2015
*	@author	James Ethridge
*
* Copyright (c) 2015, Alencon Systems
* All rights reserved.
*
*/

/** DCDCComms
*	@addtogroup DCDCComms
*	@{
*/

/** CmdDispatcher
*	@addtogroup CmdDispatcher
*	@{
*/
/* ***** Inclusion Gaurd ****** */
#ifndef __CmdDispatcher_H__
#define __CmdDispatcher_H__

/* ***** Dependencies ****** */
#include <stdint.h>
//#include "DCDCPacket.h"
#include "DCDCModel.h"
#include "RS485Controller.h"
/** CmdDispatcher_Public_Macros
*	@{
*/

/** CmdDispatcher_Public_Macros
*	@}
*/

/** CmdDispatcher_Public_Types
*	@{
*/


/** CmdDispatcher_Public_Types
*	@}
*/

/** CmdDispatcher_Public_Functions
*	@{
*/

/********************************************************************//**
 * @brief 		Initializes the RS485 Command Dispatcher
 * @param[in] None
 * @return		None
 *********************************************************************/
void CmdDispatcher_init(void);

/********************************************************************//**
 * @brief 		None
 * @param[in]	None
 * @param[in]	None
 * @return 		None
 *********************************************************************/
void CmdDispatcher_on_new_cmd(const dcdc_command_packet_t* packet,
																			dcdc_model_handle_t model,
																			rs485ctrlr_handle_t rs485ctl);


/** CmdDispatcher_Public_Functions
*	@}
*/

#endif

/** CmdDispatcher
*	@}
*/

/** DCDCComms
*	@}
*/


/* ~~~~~~~~~~~~~~~~~~~~~~~~~ EOF ~~~~~~~~~~~~~~~~~~~~~~~~~ */

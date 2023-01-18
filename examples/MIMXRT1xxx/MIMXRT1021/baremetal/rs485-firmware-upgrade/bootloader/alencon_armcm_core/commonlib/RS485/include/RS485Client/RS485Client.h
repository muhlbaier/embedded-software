/**
*	@file 	RS485Client.h
*	@brief
*	@date
*	@author	
*
*
* Copyright (c) 2015, Alencon Systems
* All rights reserved.
*/


/** PackageName
*	@addtogroup PackageName
*	@{
*/

/** RS485Client
*	@addtogroup RS485Client
*	@{
*/
/* ***** Inclusion Gaurd ****** */
#ifndef __RS485Client_H__
#define __RS485Client_H__

/* ***** Dependencies ****** */
#include <stdint.h>
#include "RS485Packet.h"

/** RS485Client_Public_Macros
*	@{
*/


/** RS485Client_Public_Macros
*	@}
*/

/** RS485Client_Public_Types
*	@{
*/
typedef enum rs485_client_state_t
{
  RS485CLIENT_SEND=0,
  RS485CLIENT_RCV=1,
	RS485CLIENT_INIT=255
}rs485_client_state_t;

typedef struct rs485_client_t* rs485_client_handle_t;
typedef void(*RS485Callback)(rs485_client_handle_t client,uint8_t* buffer, uint8_t length);
/** RS485Client_Public_Types
*	@}
*/

/** RS485Client_Public_Functions
*	@{
*/

/********************************************************************//**
 * @brief 		Create an instance of an RS485Client
 * @param[in]	None
 * @return 		None
 *********************************************************************/
rs485_client_handle_t RS485Client_Create(void);

/********************************************************************//**
 * @brief 		Initialize a client (and RS485 Serial HAL)
 * @param[in]	client - Handle for the client instance
 * @return 		None
 *********************************************************************/
void    RS485Client_init(rs485_client_handle_t client);

/********************************************************************//**
 * @brief 		Destroy a client (and close RS485 Serial HAL)
 * @param[in]	client - Handle for the client instance
 * @return 		None
 *********************************************************************/
void    RS485Client_Destroy(rs485_client_handle_t client);

/********************************************************************//**
 * @brief 		Register a callback function with the client
 *
 * Example Usage:
 * @code
 * void callback(rs485_client_handle_t client, uint8_t* buffer, uint8_t length) {
 *    // buffer will point to the first byte in the rs485_payload_t received\
 *    // length will be the length of the rs485_payload_t
 * }
 * // ...
 * RS485Client_register_callback(client, callback);
 * @endcode
 *
 * Only one callback can be registered.
 *
 * @param[in]	client - handle for the client instance
 * @param[in]	on_new_data - reference to the callback function to
 *              be called when a new packet arrives
 * @return 		None
 *********************************************************************/
void    RS485Client_register_callback(rs485_client_handle_t client, RS485Callback on_new_data);

/********************************************************************//**
 * @brief 		Get the reference for the callback function assigned to the client
 * @param[in]	client - handle for the client instance
 * @return 		Function pointer to currently set callback.
 *********************************************************************/
RS485Callback    RS485Client_get_callback(rs485_client_handle_t client);

/********************************************************************//**
 * @brief 		Return the ID of the client (useful when multiple client objects exist)
 * @param[in]	client - handle for the client instance
 * @return 		Client ID (not currently used 5/12/2016)
 *********************************************************************/
uint32_t    RS485Client_get_id(rs485_client_handle_t client);

/********************************************************************//**
 * @brief 		Get the current state of the RS485 Client
 * @param[in]	client - handle for the client instance
 * @return 		State of the client (refer to rs485_client_state_t enumeration)
 *********************************************************************/
uint8_t    RS485Client_get_client_state(rs485_client_handle_t client);

/********************************************************************//**
 * @brief 		Set the state of the client
 *
 * Required to enter the RS485CLIENT_SEND state to send a packet.
 *
 * @param[in]	client - handle for the client instance
 * @return 		None
 *********************************************************************/
void RS485Client_set_client_state(rs485_client_handle_t client, uint8_t state);

/********************************************************************//**
 * @brief 		Send a packet with the client instance
 *
 * Only one packet can be sent at a time.
 *
 * Example Usage
 * @code
 * // force state to send mode
 * RS485Client_set_client_state(controller->client,RS485CLIENT_SEND);
 * // send packet (pass the pointer)
 * RS485Client_send_packet(controller->client,&controller->packet);
 * // call the run method to do the actual sending
 * RS485Client_run(controller->client);
 * @endcode
 *
 * @param[in] client - handle for the client instance
 * @param[in] Pointer to rs485_packet_t to send
 * @return 		None
 *
 * @warning packet to send is passed by reference and no copy is made
 * be careful not to destroy the data before RS485Client_run has a chance
 * to send the data.
 *********************************************************************/
void RS485Client_send_packet(rs485_client_handle_t client, rs485_packet_t *packet);

/********************************************************************//**
 * @brief 		Work on sending or receiving packet depending on client state.
 *
 * If the client state is 0 (RS485CLIENT_SEND) and RS485Client_send_packet 
 * was used to set a send_pending flag:
 * - send the packet
 * - clear the send_pending flag
 * - change the state to 1 (RS485CLIENT_RCV)
 *
 * If the client state is 1 (RS485CLIENT_RCV) 
 * - check for a new packet, if one is avaialble call the callback with the packet pointer
 * 
 * @warning state is never automatically changed from RS485CLIENT_RCV to RS485CLIENT_SEND
 * it is the responsibility of the controller to us RS485Client_set_client_state() to
 * put the client into the send state.
 *
 * @param[in]	client - handle for the client instance
 * @return 		None
 *********************************************************************/
void RS485Client_run(rs485_client_handle_t client);

/********************************************************************//**
 * @brief 		Reset to default values, reset state, reset serial HAL
 * @param[in]	client - handle for the client instance
 * @return 		None
 *********************************************************************/
void    RS485Client_reset(rs485_client_handle_t client);

/** RS485Client_Public_Functions
*	@}
*/

#endif

/** RS485Client
*	@}
*/

/** PackageName
*	@}
*/


/* ~~~~~~~~~~~~~~~~~~~~~~~~~ EOF ~~~~~~~~~~~~~~~~~~~~~~~~~ */

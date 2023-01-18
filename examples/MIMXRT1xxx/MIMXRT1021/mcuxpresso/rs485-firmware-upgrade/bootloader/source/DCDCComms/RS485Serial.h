/**
*	@file 	RS485Serial.h
*	@brief
*	@date
*	@author	James Ethridge
*
*
*  Copyright (c) 2016, Alencon Systems
* All rights reserved.
*/


/** PackageName
*	@addtogroup PackageName
*	@{
*/

/** RS485Serial
*	@addtogroup RS485Serial
*	@{
*/
/* ***** Inclusion Gaurd ****** */
#ifndef __RS485Serial_H__
#define __RS485Serial_H__

/* ***** Dependencies ****** */
#include <stdint.h>

/** RS485Serial_Public_Macros
*	@{
*/

/** RS485Serial_Public_Macros
*	@}
*/

/** RS485Serial_Public_Types
*	@{
*/

/** RS485Serial_Public_Types
*	@}
*/

/** RS485Serial_Public_Functions
*	@{
*/

// methods used for USART channel 0
extern int32_t    RS485_Serial_init(uint32_t baud_rate);/**< Initialize the Serial Peripheral */
extern uint32_t   RS485_Serial_Write(uint8_t *txbuf, uint32_t buflen); /**< Write to the serial buffer */
extern uint32_t   RS485_Serial_Read(uint8_t *rxbuf, uint32_t buflen);/**< Read from the buffer */
extern int32_t    RS485_Serial_Close(void); /**< De-Init the serial device */
void 			  RS485_Serial_RegisterTxCompleteCallback(void(*callback)(void));
    
int32_t RS485_Init(uint32_t channel);
uint32_t RS485_Write(uint32_t channel, uint8_t *txbuf, uint32_t buflen);
uint32_t RS485_Read(uint32_t channel, uint8_t *rxbuf, uint32_t buflen);

/** RS485Serial_Public_Functions
*	@}
*/

#endif

/** RS485Serial
*	@}
*/

/** PackageName
*	@}
*/


/* ~~~~~~~~~~~~~~~~~~~~~~~~~ EOF ~~~~~~~~~~~~~~~~~~~~~~~~~ */

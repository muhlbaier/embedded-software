/**
 * @defgroup uart UART Module
 * @file uart.h
 *
 *  Created on: Mar 12, 2014
 *  @author: Michael Muhlbaier
 *  Updated on: Feb 7, 2015
 *  @author Anthony Merlino
 *
 * Use this module for basic UART communications. Completely interrupt driven
 * and buffered. 
 *
 * Normal usage is:
 * - UART_Init() to initialize a UART channel.
 * - Transmit data
 * -- UART_WriteByte() to send a byte
 * -- UART_Write() to send an array of bytes
 * -- UART_Printf() to send a formatted string
 * - UART_RegisterReceiver() to register a character receiver to process received data
 * - Call UART_Tick() as frequently as possible (every 1ms or more). Note: this happens
 * automatically if using @ref task with USE_MODULE_TASK defined in project_settings.h
 * @{
 */

#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

// Get project/library configuration
#include "project_settings.h"

#ifdef USE_MODULE_SUBSYSTEM
#include "subsystem.h"
#endif

#ifdef USE_MODULE_TASK
#include "task.h"
#endif

#ifdef USE_MODULE_BUFFER_PRINTF
#include "buffer_printf.h"
#endif

#include "buffer.h"

/** NOTE:  This enumeration has to occur BEFORE the include for
 * the hal_uart.h because hal_uart.h includes this uart.h file
 * so that it can see this error enumeration.
 */
enum uart_errors {
    BREAK_ERROR = 0,
    PARITY_ERROR,
    FRAMING_ERROR,
    OVERRUN_ERROR,
    NUM_UART_ERRORS
};

/**************************************
 * Hardware Abstraction Layer Includes
 *************************************/
#include "hal_uart.h"

/** Initialize UART module
 *
 * Example usage:
 * @code
 * UART_Init(UART0_CHANNEL);
 * @endcode
 *
 * @param channel - The channel of UART to be used.  Macros for these should be defined in the
 * HAL of the specific device.
 */
void UART_Init(uint8_t channel);

/** @brief Change baud rate
 *
 * @param channel - The channel of the UART to have its baud rate changed.
 * @param baud - The new baud rate.
 */
void UART_ReconfigureBaud(uint8_t channel, uint32_t baud);

/** @brief Write a single byte to UART
 * 
 * Transmit a single byte over the UART channel
 *
 * @param channel - The UART channel to send the byte over
 * @param c - the byte to send
 */
void UART_WriteByte(uint8_t channel, uint8_t c);

/**
 * @brief Write chunk of data to UART tx buffer
 *
 * @param channel - the UART channel to send on
 * @param data - a pointer to the data to send
 * @param length - the amount of data to send
 * @return 0 = success, -1 = not enough room in buffer, -2 = Invalid channel
 */
int8_t UART_Write(uint8_t channel, uint8_t * data, uint16_t length);

// this module could be used without buffer_printf if these functions are
// not used. Flag added to make it easy to not require buffer_printf
#ifdef _BUFFER_PRINTF_H_
/**
 * @brief print formatted string to UART
 * 
 * See @ref Push_printf() for string format details.
 *
 * Uses Push_vprintf() to send formatted string out UART buffer
 *
 * @param channel - the UART channel to send on
 * @param str - formatted string
 * @param ... - variable list of replacement data
 */
void UART_printf(uint8_t channel, char * str, ...);

/**
 * @brief print formatted string to UART
 * 
 * See @ref Push_printf() for string format details.
 *
 * Use this function from another function which had a variable argument list input.
 *
 * @param channel - the UART channel to send on
 * @param str - formatted string
 * @param vars - variable list of replacement data
 */
void UART_vprintf(uint8_t channel, char * str, va_list vars);
#endif

/**
 * @brief Returns 0 or num bytes to indicate if the UART is transmitting
 *
 * @param channel - UART channel to check
 * @return 0 if not transmitting, number of bytes in buffer otherwise
 */
uint8_t UART_IsTransmitting(uint8_t channel);

/**
 * @brief Main UART routine which calls subscribers and handles errors
 *
 * If the task management module is used and USE_MODULE_TASK is defined then
 * this function will automatically run via the task management system. Otherwise
 * the user should call UART_Tick() frequently to handle incoming data.
 */
void UART_Tick(void);

/**
 * @brief register a character receiver for a UART channel to receive data
 *
 * The receiver function registered will be called by UART_Tick whenever a byte
 * is received. The receiver function should have no return value and a char input.
 *
 * @param channel - UART channel to register receiver with
 * @param fn - function pointer to receiver function: void ReceiverName(char c)
 * @return -1 if channel is out of range, -2 if receiver list is full, 0 if successful
 */
int8_t UART_RegisterReceiver(uint8_t channel, void(*fn)(uint8_t));

/**
 * @brief Unregister a receiver added by UART_RegisterReceiver()
 *
 * @param channel - UART channel to unregister receiver on
 * @param fn - function pointer to receiver function: void ReceiverName(char c)
 */
void UART_UnregisterReceiver(uint8_t channel, void(*fn)(uint8_t));

/**
 * @brief Register callback function to handle UART errors
 *
 * Will call the callback function when a UART error occurs.
 * The callback should have no return value and a enum uart_errors input.
 *
 * @param channel - UART channel to tie the error callback function to
 * @param callback - function pointer to callback function: void CallbackName(enum uart_errors error)
 */
void UART_RegisterErrorCallback(uint8_t channel, void(*callback)(enum uart_errors));

/**
 * @brief Register callback function to be called when/if the UART Tx buffer overflows
 *
 * Will be called when trying to transmit data and the Tx buffer is full.
 * The callback should have no return value and no input.
 *
 * @param channel - UART channel to tie the tx overwrite callback to
 * @param overwriteCallback - function pointer to callback function: void CallbackName(void)
 */
void UART_RegisterTxOverwriteCallback(uint8_t channel, void(*overwriteCallback)(void));

/**
 * @brief Register callback function to be called when/if the UART Rx buffer overflows
 *
 * Will be called when trying to transmit data and the Rx buffer is full.
 * The callback should have no return value and no input.
 *
 * @param channel - UART channel to tie the Rx overwrite callback to
 * @param overwriteCallback - function pointer to callback function: void CallbackName(void)
 */
void UART_RegisterRxOverwriteCallback(uint8_t channel, void(*overwriteCallback)(void));


/**************************************
 * Hardware Abstraction Layer Declarations
 *************************************/

 // In some cases the HAL will implement these with a #define which is why each is
 // surrounded with a #ifndef / #endif
 
/** @brief initialize UART channel at specified baud rate
 *
 * REQUIRED
 *
 * This function should:
 * - Initialize the UART module for the channel specified
 * - Map UART pins if needed
 * - Configure UART interrupts
 *
 * @param channel - UART channel to initialize
 * @param baud - integer baud rate (bits/s) to configure UART for
 */
 #ifndef hal_UART_Init
void hal_UART_Init(uint8_t channel, uint32_t baud);
#endif


/** @brief change the baud rate for the UART channel
 *
 * @param channel - UART channel
 * @param baud - integer baud rate (bits/s) to configure UART for
 */
#ifndef hal_UART_ReconfigureBaud
void hal_UART_ReconfigureBaud(uint8_t channel, uint32_t baud);
#endif

/** @brief UART transmit interrupt handler
 *
 * REQUIRED to be called by HAL
 *
 * This function is implemented by the UART module (uart.c) but should be
 * called by the ISR for the given UART channel.
 *
 * @param channel - channel the interrupt occurred on
 */
#ifndef UART_Tx_Handler
void UART_Tx_Handler(uint8_t channel);
#endif

/** @brief UART receive interrupt handler
 *
 * REQUIRED to be called by HAL
 *
 * This function is implemented by the UART module (uart.c) but should be
 * called by the ISR for the given UART channel.
 *
 * @param channel - channel the interrupt occurred on
 */
#ifndef UART_Rx_Handler
void UART_Rx_Handler(uint8_t channel);
#endif

/** @brief UART error (interrupt) handler
 *
 * This function is implemented by the UART module (uart.c) but should be
 * called by the error ISR for the given UART channel or when an error condition
 * is detected by the HAL.
 *
 * @param channel - channel the interrupt occurred on
 */
#ifndef UART_Error_Handler
void UART_Error_Handler(uint8_t channel, enum uart_errors error);
#endif

/** @brief Enable UART peripheral for given channel
 */
#ifndef hal_UART_Enable
void hal_UART_Enable(uint8_t channel);
#endif

/** @brief Disable UART peripheral for given channel
 */
#ifndef hal_UART_Disable
void hal_UART_Disable(uint8_t channel);
#endif

/** @brief return 0 if no space is available in the hardware UART tx buffer
 *
 * REQUIRED
 */
#ifndef hal_UART_SpaceAvailable
uint8_t hal_UART_SpaceAvailable(uint8_t channel);
#endif

/** @brief return 0 if no data is available in the hardware UART rx buffer
 *
 * REQUIRED
 */
#ifndef hal_UART_DataAvailable
uint8_t hal_UART_DataAvailable(uint8_t channel);
#endif

/** @brief clear the UART rx interrupt flags
 */
#ifndef hal_UART_ClearRxIF
void hal_UART_ClearRxIF(uint8_t channel);
#endif

/** @brief clear the UART tx interrupt flags
 */
#ifndef hal_UART_ClearTxIF
void hal_UART_ClearTxIF(uint8_t channel);
#endif

/** @brief enable the UART rx interrupt
 */
#ifndef hal_UART_EnableRxInterrupt
void hal_UART_EnableRxInterrupt(uint8_t channel);
#endif

/** @brief enable the UART tx interrupt
 *
 * REQUIRED
 */
#ifndef hal_UART_EnableTxInterrupt
void hal_UART_EnableTxInterrupt(uint8_t channel);
#endif

/** @brief disable the UART rx interrupt
 */
#ifndef hal_UART_DisableRxInterrupt
void hal_UART_DisableRxInterrupt(uint8_t channel);
#endif

/** @brief disable the UART tx interrupt
 *
 * REQUIRED
 */
#ifndef hal_UART_DisableTxInterrupt
void hal_UART_DisableTxInterrupt(uint8_t channel);
#endif

/** @brief Is UART rx interrupt enabled
 */
#ifndef hal_UART_RxInterruptEnabled
uint8_t hal_UART_RxInterruptEnabled(uint8_t channel);
#endif

/** @brief Is UART tx interrupt enabled
 *
 * REQUIRED
 */
#ifndef hal_UART_TxInterruptEnabled
uint8_t hal_UART_TxInterruptEnabled(uint8_t channel);
#endif

/** @brief Get character from UART hardware rx register
 *
 * REQUIRED
 */
#ifndef hal_UART_RxByte
uint8_t hal_UART_RxByte(uint8_t channel);
#endif

/** @brief send character to UART hardware tx register
 * 
 * REQUIRED
 */
#ifndef hal_UART_TxByte
void hal_UART_TxByte(uint8_t channel, uint8_t data);
#endif

/** @}*/
#endif /* _UART_H_ */

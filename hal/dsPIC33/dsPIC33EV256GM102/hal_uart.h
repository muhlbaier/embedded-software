/**
 * @file hal_uart.h
 * 
 * @defgroup hal_uart_dspic33ev256gm102 HAL UART Module for dsPIC33EV256GM102
 * @ingroup uart
 * 
 * Implementation of UART on the dsPIC33EV256GM102.
 * 
 * Pin configuration:
 * - UART 1 RX = Pin 11 = RP36
 * - UART 1 TX = Pin 12 = RP20
 * - UART 2 RX = Pin 17 = RP40
 * - UART 2 TX = Pin 18 = RP41
 * 
 * Provides extra functions to use the extra features of the dsPIC UART module
 * that are not provided in the HAL.
 * 
 * Created on: Mar 4, 2019
 *    @author: Nate Hoffman
 * 
 * @version 1.0.0 Initial function definitions
 */

#ifndef HAL_UART_H
#define HAL_UART_H

#define UART1_CH 0
#define UART2_CH 1

/**
 * Enable transmit on the selected UART channel.
 * 
 * @param channel - channel to enable
 */
void hal_UART_TxEnable(uint8_t channel);

/**
 * Check if the UART channel has finished transmitting.
 * 
 * Returns the transmission status:
 * - 0 if the UART has not finished transmitting the data
 * - 1 if the UART has finished transmitting the data
 * 
 * @param channel - channel to check
 * @return transmission status
 */
uint8_t hal_UART_DoneTransmitting(uint8_t channel);

#endif // HAL_UART_H

/**
 * @defgroup hal_uart HAL UART Module
 * @ingroup uart
 * @file hal_uart.h
 *
 *  Created on: Feb 25, 2017
 *       Author: Zach Norris
 *
 * @todo DOCUMENT
 * @{
 */

#ifndef _HAL_UART_H_
#define _HAL_UART_H_

#define UART1_CH 0
#define UART2_CH 1

void hal_UART_TxEnable(uint8_t channel);

uint8_t hal_UART_DoneTransmitting(uint8_t channel);

/** @}*/
#endif /* _HAL_UART_H_ */

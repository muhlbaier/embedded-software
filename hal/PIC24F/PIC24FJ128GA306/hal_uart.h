
 /**
 * @defgroup hal_uart HAL UART Module
 * @ingroup uart
 * @file hal_uart.h
 *
 *  Created on: Feb 8, 2015
 *      Author: Muhlbaier
 * @todo Muhlbaier document this
 * @{
 */

#ifndef _HAL_UART_H_
#define _HAL_UART_H_

#define UART1_CH 1
#define UART2_CH 2
#define UART3_CH 3
#define UART4_CH 4

void hal_UART_TxEnable(uint8_t channel);
uint8_t hal_UART_DoneTransmitting(uint8_t channel);

/** @}*/
#endif /* _HAL_UART_H_ */

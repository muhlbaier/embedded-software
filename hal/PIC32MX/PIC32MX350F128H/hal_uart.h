
 /**
 * @defgroup hal_uart HAL UART Module
 * @ingroup uart
 * @file hal_uart.h
 *
 * @{
 */

#ifndef _HAL_UART_H_
#define _HAL_UART_H_

#define UART1_CH 1
#define UART2_CH 2
#define UART3_CH 3
#define UART4_CH 4

uint8_t hal_UART_DoneTransmitting(uint8_t channel);

/** @}*/
#endif /* _HAL_UART_H_ */

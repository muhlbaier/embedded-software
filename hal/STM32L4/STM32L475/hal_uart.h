
 /**
 * @defgroup hal_uart HAL UART Module
 * @ingroup uart
 * @file hal_uart.h
 *
 * @author Michael Muhlbaier
 *
 * @{
 */

#ifndef _HAL_UART_H_
#define _HAL_UART_H_

#define HAL_UART1 1
#define HAL_UART2 2
#define HAL_UART3 3
#define HAL_UART4 4
#define HAL_UART5 5

/**
 * Enable NVIC interrutps for the UART channel (does not enable any specific interrupts just for the UART module itself)
 */
void hal_UART_EnableInterrupts(uint8_t channel);

/** @}*/
#endif /* _HAL_UART_H_ */

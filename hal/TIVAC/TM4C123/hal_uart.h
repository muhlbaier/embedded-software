
 /**
 * @defgroup hal_uart HAL UART Module
 * @ingroup uart
 * @file hal_uart.h
 *
 *  Created on: Feb 8, 2015
 *      Author: Anthony Merlino
 * @todo Anthony Merlino document this
 * @{
 */

#ifndef _HAL_UART_H_
#define _HAL_UART_H_

#include "driverlib/uart.h"
#include "inc/hw_uart.h"
#include "inc/hw_types.h"

#define UART0 0
#define UART1 1
#define UART2 2
#define UART3 3
#define UART4 4
#define UART5 5
#define UART6 6
#define UART7 7

#define UART0_BASE              0x4000C000  // UART0
#define UART1_BASE              0x4000D000  // UART1
#define UART2_BASE              0x4000E000  // UART2
#define UART3_BASE              0x4000F000  // UART3
#define UART4_BASE              0x40010000  // UART4
#define UART5_BASE              0x40011000  // UART5
#define UART6_BASE              0x40012000  // UART6
#define UART7_BASE              0x40013000  // UART7

#warning "TODO - Find alternate method to optimize this"
#define UART_BASE(channel) (0x4000C000 + channel*0x1000)

#define hal_UART_Enable(channel) UARTEnable(UART_BASE(channel))
#define hal_UART_Disable(channel) UARTDisable(UART_BASE(channel))
#define hal_UART_EnableRxInterrupt(channel) UARTIntEnable(UART_BASE(channel), UART_INT_RX)
#define hal_UART_EnableTxInterrupt(channel) UARTIntEnable(UART_BASE(channel), UART_INT_TX)
#define hal_UART_DisableRxInterrupt(channel) UARTIntDisable(UART_BASE(channel), UART_INT_RX)
#define hal_UART_DisableTxInterrupt(channel) UARTIntDisable(UART_BASE(channel), UART_INT_TX)
#define hal_UART_RxInterruptEnabled(channel) ((HWREG(UART_BASE(channel) + UART_O_IM) & UART_INT_RX) >> 4)
#define hal_UART_TxInterruptEnabled(channel) ((HWREG(UART_BASE(channel) + UART_O_IM) & UART_INT_TX) >> 5)
#define hal_UART_TxByte(channel, c) UARTCharPutNonBlocking(UART_BASE(channel), (char)c)
#define hal_UART_RxByte(channel) UARTCharGetNonBlocking(UART_BASE(channel))
#define hal_UART_ClearTxIF(channel) UARTIntClear(UART_BASE(channel), UART_INT_TX)
#define hal_UART_ClearRxIF(channel) UARTIntClear(UART_BASE(channel), UART_INT_RX)
#define hal_UART_DataAvailable(channel) UARTCharsAvail(UART_BASE(channel))
#define hal_UART_SpaceAvailable(channel) UARTSpaceAvail(UART_BASE(channel))

/** @}*/
#endif /* _HAL_UART_H_ */


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

#include "hw_types.h"
#include "driverlib/uart.h"
#include "hw_uart.h"

#define UART0 0
#define UART1 1

#define UART0_BASE              0x4000C000  // UART0
#define UART1_BASE              0x4000D000  // UART1

#define UART_BASE(channel) (0x4000C000 + channel*0x1000)

#define hal_UART_Enable(channel) HWREG(UART_BASE(channel) + UART_O_CTL) |= (UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE)
#define hal_UART_Disable(channel) HWREG(UART_BASE(channel) + UART_O_CTL) &= ~(UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE)
#define hal_UART_EnableRxInterrupt(channel) UARTIntEnable(UART_BASE(channel), UART_INT_RX)
#define hal_UART_EnableTxInterrupt(channel) UARTIntEnable(UART_BASE(channel), UART_INT_TX)
#define hal_UART_EnableErrorInterrupts(channel) UARTIntEnable(UART_BASE(channel), UART_INT_BE | UART_INT_OE | UART_INT_PE | UART_INT_FE)
#define hal_UART_DisableRxInterrupt(channel) UARTIntDisable(UART_BASE(channel), UART_INT_RX)
#define hal_UART_DisableTxInterrupt(channel) UARTIntDisable(UART_BASE(channel), UART_INT_TX)
#define hal_UART_DisableErrorInterrupts(channel) UARTIntDisable(UART_BASE(channel), UART_INT_BE | UART_INT_OE | UART_INT_PE | UART_INT_FE)
#define hal_UART_RxInterruptEnabled(channel) ((HWREG(UART_BASE(channel) + UART_O_IM) & UART_INT_RX) >> 4)
#define hal_UART_TxInterruptEnabled(channel) ((HWREG(UART_BASE(channel) + UART_O_IM) & UART_INT_TX) >> 5)
#define hal_UART_TxByte(channel, c) UARTCharPutNonBlocking(UART_BASE(channel), c)
#define hal_UART_RxByte(channel) UARTCharGetNonBlocking(UART_BASE(channel))
#define hal_UART_ClearTxIF(channel) UARTIntClear(UART_BASE(channel), UART_INT_TX)
#define hal_UART_ClearRxIF(channel) UARTIntClear(UART_BASE(channel), UART_INT_RX)
#define hal_UART_ClearFrameErrorIF(channel) UARTIntClear(UART_BASE(channel), UART_INT_FE)
#define hal_UART_ClearParityErrorIF(channel) UARTIntClear(UART_BASE(channel), UART_INT_PE)
#define hal_UART_ClearBreakErrorIF(channel) UARTIntClear(UART_BASE(channel), UART_INT_BE)
#define hal_UART_ClearOverrunErrorIF(channel) UARTIntClear(UART_BASE(channel), UART_INT_OE)
#define hal_UART_DataAvailable(channel) UARTCharsAvail(UART_BASE(channel))
#define hal_UART_SpaceAvailable(channel) UARTSpaceAvail(UART_BASE(channel))

/** @}*/
#endif /* _HAL_UART_H_ */

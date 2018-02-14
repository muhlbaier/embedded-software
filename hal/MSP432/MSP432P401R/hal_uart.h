/**
 * @defgroup hal_uart_msp432p401r UART Hardware Abstraction Layer for MSP432P401R
 * @ingroup uart
 * @file hal_uart.h
 *
 * @author Autumn Brown
 *
 * Created on February 25, 2017
 * @{
 */

#ifndef _HAL_UART_H_
#define _HAL_UART_H_

/**
 * Channel for UART module 0
 *
 * RX: P1.2 (HW Pin 5)
 * TX: P1.3 (HW Pin 6)
 */
#define UART0 0

/**
 * Channel for UART module 1
 *
 * RX: P2.2 (HW Pin 18)
 * TX: P2.3 (HW Pin 19)
 */
#define UART1 1

/**
 * Channel for UART module 2
 *
 * RX: P3.2 (HW Pin 34)
 * TX: P3.3 (HW Pin 35)
 */
#define UART2 2

/**
 * Channel for UART module 3
 *
 * RX: P9.6 (HW Pin 98)
 * TX: P9.7 (HW Pin 99)
 */
#define UART3 3

///@}
#endif /* _HAL_UART_H_ */

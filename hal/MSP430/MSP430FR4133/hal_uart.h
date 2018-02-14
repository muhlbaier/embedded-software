 /**
 * @defgroup hal_uart HAL UART Module
 * @ingroup uart
 * @file hal_uart.h
 *
 *  Created on: Mar 21, 2016
 *      Author: Mike Bucceroni
 *      Modified from hal_uart.h by Anthony Merlino
 *  Pins must be correctly set to enable uart
 *  include the following in your main.c
 * 
 *	 void Init_GPIO();
 *	 
 * 	 main{
 *		 Init_GPIO();
 *		PM5CTL0 &= ~LOCKLPM5;
 *	 }
 *  	 
 *	 void Init_GPIO(){
 *		P1DIR = 0xFF; P2DIR = 0xFF; P3DIR = 0xFF; P4DIR = 0xFF;
 *		P5DIR = 0xFF; P6DIR = 0xFF; P7DIR = 0xFF; P8DIR = 0xFF;
 *		P1REN = 0xFF; P2REN = 0xFF; P3REN = 0xFF; P4REN = 0xFF;
 *		P5REN = 0xFF; P6REN = 0xFF; P7REN = 0xFF; P8REN = 0xFF;
 *		P1OUT = 0x00; P2OUT = 0x00; P3OUT = 0x00; P4OUT = 0x00;
 *		P5OUT = 0x00; P6OUT = 0x00; P7OUT = 0x00; P8OUT = 0x00;
 *	}
 *
 * @{ 
 */

#ifndef _HAL_UART_H_
#define _HAL_UART_H_

#define UART0 0

/** @}*/
#endif /* _HAL_UART_H_ */

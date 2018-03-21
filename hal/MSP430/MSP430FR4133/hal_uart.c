/*
 * hal_uart.c
 *
 *  Created on: Mar 21, 2016
 *      Author: Mike Bucceroni
 */
#include <msp430.h>
#include <stdint.h>
#include "hal_uart.h"
#include "project_settings.h"

void hal_UART_Init(uint8_t channel, uint32_t baud){
	if(channel != UART0)return;
	P1SEL0 |= BIT0 + BIT1;
	hal_UART_Disable(channel);
	UCA0CTLW0 |= UCSSEL__SMCLK;
	UCA0BR0 = FCPU / baud;
	UCA0BR1 = FCPU / baud / 256;
	UCA0MCTLW = ((FCPU*8) / baud - (FCPU / baud) * 8) << 1;
	hal_UART_Enable(channel);
	hal_UART_EnableTxInterrupt(channel);
	hal_UART_EnableRxInterrupt(channel);
}

void hal_UART_Enable(uint8_t channel){
	if(channel != UART0)return;
	UCA0CTLW0 &= ~UCSWRST;
}

void hal_UART_Disable(uint8_t channel){
	if(channel != UART0)return;
	UCA0CTLW0 |= UCSWRST;
}

void hal_UART_EnableRxInterrupt(uint8_t channel){
	if(channel != UART0)return;
	UCA0IE |= UCRXIE;
}

void hal_UART_EnableTxInterrupt(uint8_t channel){
	if(channel != UART0)return;
	UCA0IE |= UCTXIE;
}

void hal_UART_DisableRxInterrupt(uint8_t channel){
	if(channel != UART0)return;
	UCA0IE &= ~UCRXIE;
}

void hal_UART_DisableTxInterrupt(uint8_t channel){
	if(channel != UART0)return;
	UCA0IE &= ~UCTXIE;
}

uint8_t hal_UART_RxInterruptEnabled(uint8_t channel) {
	if(channel != UART0) return 0;

    return (UCA0IE & UCRXIE) >> 0;
}

uint8_t hal_UART_TxInterruptEnabled(uint8_t channel) {
	if(channel != UART0) return 0;

    return (UCA0IE & UCTXIE) >> 1;
}

void hal_UART_TxByte(uint8_t channel, uint8_t c){
	if(channel != UART0)return;
	UCA0TXBUF = c;
}

uint8_t hal_UART_RxByte(uint8_t channel){
	if(channel != UART0)return 0;
	return UCA0RXBUF;
}

void hal_UART_ClearTxIF(uint8_t channel){
	if(channel != UART0)return;
	UCA0IFG &= ~UCTXIFG;
}

void hal_UART_ClearRxIF(uint8_t channel){
	if(channel != UART0)return;
	UCA0IFG &= ~UCRXIFG;
}

uint8_t hal_UART_DataAvailable(uint8_t channel){
	if(channel != UART0)return 0;
	return (UCA0IFG & UCRXIFG);
}

uint8_t hal_UART_SpaceAvailable(uint8_t channel){
	if(channel != UART0)return 0;
	return (UCA0IFG & UCTXIFG);
}

uint8_t hal_UART_RxOverrun(uint8_t channel){
	if(channel != UART0)return 0;
	return (UCA0STATW & UCOE);
}

#ifdef USE_UART0
#ifdef MSPGCC
__attribute__((interrupt(USCI_A0_VECTOR)))
void _UART0_ISR(void) {
#else
#pragma vector=USCI_A0_VECTOR
__interrupt void _UART0_ISR(void){
#endif
	UART_Rx_Handler(UART0);
	hal_UART_ClearRxIF(UART0);
	UART_Tx_Handler(UART0);
}
#endif

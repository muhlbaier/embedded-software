#include <msp430.h>
#include <stdint.h>

#include "uart.h"
#include "hal_uart.h"
#include "project_settings.h"

void hal_UART_Init(uint8_t channel, uint32_t baud){
	switch(channel){
		case UART0:
			P3SEL |= BIT3 + BIT4;
			hal_UART_Disable(channel);
			UCA0CTL1 = UCSSEL_2;
			if(FCPU / baud < 16) {
				UCA0BR0 = FCPU / baud;
				UCA0BR1 = FCPU / baud / 256;
				UCA0MCTL = ((FCPU*8) / baud - (FCPU / baud) * 8) << 1;
			}else {
				UCA0BR0 = FCPU / baud / 16;
				UCA0BR1 = FCPU / baud / 4096;
				// UCBRFx = round(((N/16)-INT(N/16))*16)
				// where N = FCPU / baud including the fractional component
				UCA0MCTL = ((((FCPU*16) / baud) / 16 - ((FCPU / baud) / 16) * 16) << 4) + UCOS16;
			}
			hal_UART_Enable(channel);
			break;
		case UART1:
			P4SEL |= BIT4 + BIT5;
			hal_UART_Disable(channel);
			UCA1CTL1 = UCSSEL_2;
			if(FCPU / baud < 16) {
				UCA1BR0 = FCPU / baud;
				UCA1BR1 = FCPU / baud / 256;
				UCA1MCTL = ((FCPU*8) / baud - (FCPU / baud) * 8) << 1;
			}else {
				UCA1BR0 = FCPU / baud / 16;
				UCA1BR1 = FCPU / baud / 4096;
				// UCBRFx = round(((N/16)-INT(N/16))*16)
				// where N = FCPU / baud including the fractional component
				UCA1MCTL = ((((FCPU*16) / baud) / 16 - ((FCPU / baud) / 16) * 16) << 4) + UCOS16;
			}
			hal_UART_Enable(channel);
			break;
	}
	hal_UART_EnableTxInterrupt(channel);
	hal_UART_EnableRxInterrupt(channel);
}

void hal_UART_Enable(uint8_t channel){
	switch(channel){
		case UART0:
			UCA0CTL1 &= ~(UCSWRST);
			break;
		case UART1:
			UCA1CTL1 &= ~(UCSWRST);
			break;
		default:
			return;
	}
}

void hal_UART_Disable(uint8_t channel){
	switch(channel){
		case UART0:
			UCA0CTL1 |= UCSWRST;
			break;
		case UART1:
			UCA1CTL1 |= UCSWRST;
			break;
		default:
			return;
	}
}

void hal_UART_EnableRxInterrupt(uint8_t channel){
	switch(channel){
		case UART0:
			UCA0IE |= UCRXIE;
			break;
		case UART1:
			UCA1IE |= UCRXIE;
			break;
		default:
			return;
	}
}

void hal_UART_EnableTxInterrupt(uint8_t channel){
	switch(channel){
		case UART0:
			UCA0IE |= UCTXIE;
			break;
		case UART1:
			UCA1IE |= UCTXIE;
			break;
		default:
			return;
	}
}

void hal_UART_DisableRxInterrupt(uint8_t channel){
	switch(channel){
		case UART0:
			UCA0IE &= ~UCRXIE;
			break;
		case UART1:
			UCA1IE &= ~UCRXIE;
			break;
		default:
			return;
	}
}

void hal_UART_DisableTxInterrupt(uint8_t channel){
	switch(channel){
		case UART0:
			UCA0IE &= ~UCTXIE;
			break;
		case UART1:
			UCA1IE &= ~UCTXIE;
			break;
		default:
			return;
	}
}

uint8_t hal_UART_RxInterruptEnabled(uint8_t channel) {
    switch(channel){
        case UART0:
            return (UCA0IE & UCRXIE) >> 0;
        case UART1:
            return (UCA1IE & UCRXIE) >> 0;
        default:
            return 0;
	}
}

uint8_t hal_UART_TxInterruptEnabled(uint8_t channel) {
    switch(channel){
        case UART0:
            return (UCA0IE & UCTXIE) >> 1;
        case UART1:
            return (UCA1IE & UCTXIE) >> 1;
        default:
            return 0;
    }
}


void hal_UART_TxByte(uint8_t channel, uint8_t c){
	switch(channel){
		case UART0:
			UCA0TXBUF = c;
			break;
		case UART1:
			UCA1TXBUF = c;
			break;
		default:
			return;
	}
}

uint8_t hal_UART_RxByte(uint8_t channel){
	switch(channel){
		case UART0:
			return UCA0RXBUF;
		case UART1:
			return UCA1RXBUF;
		default:
			return 0;
	}
}

void hal_UART_ClearTxIF(uint8_t channel){
	switch(channel){
		case UART0:
			UCA0IFG &= ~UCTXIFG;
			break;
		case UART1:
			UCA1IFG &= ~UCTXIFG;
			break;
		default:
			return;
	}
}

void hal_UART_ClearRxIF(uint8_t channel){
	switch(channel){
		case UART0:
			UCA0IFG &= ~UCRXIFG;
			break;
		case UART1:
			UCA1IFG &= ~UCRXIFG;
			break;
		default:
			return;
	}
}

uint8_t hal_UART_DataAvailable(uint8_t channel){
	switch(channel){
		case UART0:
			return (UCA0IFG & UCRXIFG);
		case UART1:
			return (UCA1IFG & UCRXIFG);
		default:
			return 0;
	}
}

uint8_t hal_UART_SpaceAvailable(uint8_t channel){
	switch(channel){
		case UART0:
			return (UCA0IFG & UCTXIFG);
		case UART1:
			return (UCA1IFG & UCTXIFG);
		default:
			return 0;
	}
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

#ifdef USE_UART1
#ifdef MSPGCC
__attribute__((interrupt(USCI_A1_VECTOR)))
void _UART1_ISR(void) {
#else
#pragma vector=USCI_A1_VECTOR
__interrupt void _UART1_ISR(void){
#endif
	UART_Rx_Handler(UART1);
	hal_UART_ClearRxIF(UART1);
	UART_Tx_Handler(UART1);
}
#endif

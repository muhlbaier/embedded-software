#include <msp432.h>
#include <stdint.h>

#include "uart.h"
#include "hal_uart.h"
#include "project_settings.h"

#define ROUND_2_INT(f) ((int)(f >= 0.0 ? (f + 0.5) : (f - 0.5)))

void hal_UART_Init(uint8_t channel, uint32_t baud)
{
    // Configuration flow for USCI modules on MSP432P401R
    // 1: Hold reset
    // 2: Configure control registers
    // 3: Configure ports
    // 4: Release reset
    // 5: Clear and enable interrupts

    // Hold reset
    hal_UART_Disable(channel);

    // Configure control registers
    EUSCI_A_Type* module;
    switch (channel) {
        case UART0: module = EUSCI_A0; break;
        case UART1: module = EUSCI_A1; break;
        case UART2: module = EUSCI_A2; break;
        case UART3: module = EUSCI_A3; break;
        default: return;
    }
    module->CTLW0 &= ~0xFFFE; //Clear all settings bits except for reset
    module->CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK; //Use SMCLK as source
    module->CTLW0 |= EUSCI_A_CTLW0_MODE_0; //UART mode w/out auto-baud detect

    double N = FCPU/baud;

    //UCBRSx = round((N - INT(N))*8) -> integer rounding of fractional part of N times 8
    module->MCTLW = (ROUND_2_INT((N-(int)N)*8) & EUSCI_A_MCTLW_BRS_MASK) << EUSCI_A_MCTLW_BRS_OFS;

    if(N < 16) { //Low Frequency Baud-Rate Mode Setting
        //UCBRx = INT(N) -> integer part of N
        module->BRW = (int) N;
    } else { //Oversampling Baud-Rate Mode Setting
        //UCBRx = INT(N/16) -> integer part of N divided by 16
        //UCBRFx = round(((N/16) - INT(N/16))*16) -> integer rounding of fractional part of N divided by 16 times 16
        module->BRW = (int) N/16;
        module->MCTLW |= EUSCI_A_MCTLW_OS16; //oversample mode
        module->MCTLW |= (ROUND_2_INT((N/16-(int)N/16)*16) & EUSCI_A_MCTLW_BRF_MASK) << EUSCI_A_MCTLW_BRF_OFS;
    }

    // Configure ports
    switch (channel) {
        case UART0:
            P1SEL0 |=  0x0C; //XXXX 11XX
            P1SEL1 &= ~0x0C; //XXXX 00XX
            //UCA0_RX: P1.2 (HW Pin 5) (SEL 01)
            //UCA0_TX: P1.3 (HW Pin 6) (SEL 01)
            break;
        case UART1:
            P2SEL0 |=  0x0C; //XXXX 11XX
            P2SEL1 &= ~0x0C; //XXXX 00XX
            //UCA1_RX: P2.2 (HW Pin 18) (SEL 01)
            //UCA1_TX: P2.3 (HW Pin 19) (SEL 01)
            break;
        case UART2:
            P3SEL0 |=  0x0C; //XXXX 11XX
            P3SEL1 &= ~0x0C; //XXXX 00XX
            //UCA2_RX: P3.2 (HW Pin 34) (SEL 01)
            //UCA2_TX: P3.3 (HW Pin 35) (SEL 01)
            break;
        case UART3:
            P9SEL0 |=  0x0C; //XXXX 11XX
            P9SEL1 &= ~0x0C; //XXXX 00XX
            //UCA3_RX: P9.6 (HW Pin 98) (SEL 01)
            //UCA3_TX: P9.7 (HW Pin 99) (SEL 01)
            break;
    }

	//Release reset
	hal_UART_Enable(channel);

	//Clear and enable interrupts
//	hal_UART_ClearRxIF(channel);
//  hal_UART_ClearTxIF(channel);
    hal_UART_EnableRxInterrupt(channel);
    hal_UART_EnableTxInterrupt(channel);
	switch (channel) { //Register NVIC flag
        case UART0: NVIC_EnableIRQ(EUSCIA0_IRQn); break;
        case UART1: NVIC_EnableIRQ(EUSCIA1_IRQn); break;
        case UART2: NVIC_EnableIRQ(EUSCIA2_IRQn); break;
        case UART3: NVIC_EnableIRQ(EUSCIA3_IRQn); break;
    }
}

void hal_UART_Enable(uint8_t channel)
{
	switch (channel) {
		case UART0: EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; break;
		case UART1: EUSCI_A1->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; break;
		case UART2: EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; break;
		case UART3: EUSCI_A3->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; break;
		default: return;
	}
}

void hal_UART_Disable(uint8_t channel)
{
    switch (channel) {
        case UART0: EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; break;
        case UART1: EUSCI_A1->CTLW0 |= EUSCI_A_CTLW0_SWRST; break;
        case UART2: EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; break;
        case UART3: EUSCI_A3->CTLW0 |= EUSCI_A_CTLW0_SWRST; break;
        default: return;
    }
}

void hal_UART_EnableRxInterrupt(uint8_t channel)
{
    switch (channel) {
        case UART0: EUSCI_A0->IE |= EUSCI_A_IE_RXIE; break;
        case UART1: EUSCI_A1->IE |= EUSCI_A_IE_RXIE; break;
        case UART2: EUSCI_A2->IE |= EUSCI_A_IE_RXIE; break;
        case UART3: EUSCI_A3->IE |= EUSCI_A_IE_RXIE; break;
        default: return;
    }
}

void hal_UART_EnableTxInterrupt(uint8_t channel)
{
    switch (channel) {
        case UART0: EUSCI_A0->IE |= EUSCI_A_IE_TXIE; break;
        case UART1: EUSCI_A1->IE |= EUSCI_A_IE_TXIE; break;
        case UART2: EUSCI_A2->IE |= EUSCI_A_IE_TXIE; break;
        case UART3: EUSCI_A3->IE |= EUSCI_A_IE_TXIE; break;
        default: return;
    }
}

void hal_UART_DisableRxInterrupt(uint8_t channel)
{
    switch (channel) {
        case UART0: EUSCI_A0->IE &= ~EUSCI_A_IE_RXIE; break;
        case UART1: EUSCI_A1->IE &= ~EUSCI_A_IE_RXIE; break;
        case UART2: EUSCI_A2->IE &= ~EUSCI_A_IE_RXIE; break;
        case UART3: EUSCI_A3->IE &= ~EUSCI_A_IE_RXIE; break;
        default: return;
    }
}

void hal_UART_DisableTxInterrupt(uint8_t channel)
{
    switch (channel) {
        case UART0: EUSCI_A0->IE &= ~EUSCI_A_IE_TXIE; break;
        case UART1: EUSCI_A1->IE &= ~EUSCI_A_IE_TXIE; break;
        case UART2: EUSCI_A2->IE &= ~EUSCI_A_IE_TXIE; break;
        case UART3: EUSCI_A3->IE &= ~EUSCI_A_IE_TXIE; break;
        default: return;
    }
}

uint8_t hal_UART_RxInterruptEnabled(uint8_t channel)
{
    switch (channel) {
        case UART0: return (EUSCI_A0->IE & EUSCI_A_IE_RXIE) != 0;
        case UART1: return (EUSCI_A1->IE & EUSCI_A_IE_RXIE) != 0;
        case UART2: return (EUSCI_A2->IE & EUSCI_A_IE_RXIE) != 0;
        case UART3: return (EUSCI_A3->IE & EUSCI_A_IE_RXIE) != 0;
        default: return 0;
    }
}

uint8_t hal_UART_TxInterruptEnabled(uint8_t channel)
{
    switch (channel) {
        case UART0: return (EUSCI_A0->IE & EUSCI_A_IE_TXIE) != 0;
        case UART1: return (EUSCI_A1->IE & EUSCI_A_IE_TXIE) != 0;
        case UART2: return (EUSCI_A2->IE & EUSCI_A_IE_TXIE) != 0;
        case UART3: return (EUSCI_A3->IE & EUSCI_A_IE_TXIE) != 0;
        default: return 0;
    }
}


void hal_UART_TxByte(uint8_t channel, uint8_t c)
{
    switch (channel) {
        case UART0: EUSCI_A0->TXBUF = c; break;
        case UART1: EUSCI_A1->TXBUF = c; break;
        case UART2: EUSCI_A2->TXBUF = c; break;
        case UART3: EUSCI_A3->TXBUF = c; break;
        default: return;
    }
}

uint8_t hal_UART_RxByte(uint8_t channel)
{
    switch (channel) {
        case UART0: return EUSCI_A0->RXBUF;
        case UART1: return EUSCI_A1->RXBUF;
        case UART2: return EUSCI_A2->RXBUF;
        case UART3: return EUSCI_A3->RXBUF;
        default: return 0;
    }
}

void hal_UART_ClearTxIF(uint8_t channel)
{
    switch (channel) {
        case UART0: EUSCI_A0->IFG &= ~EUSCI_A_IFG_TXIFG; break;
        case UART1: EUSCI_A1->IFG &= ~EUSCI_A_IFG_TXIFG; break;
        case UART2: EUSCI_A2->IFG &= ~EUSCI_A_IFG_TXIFG; break;
        case UART3: EUSCI_A3->IFG &= ~EUSCI_A_IFG_TXIFG; break;
        default: return;
    }
}

void hal_UART_ClearRxIF(uint8_t channel)
{
    switch (channel) {
        case UART0: EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG; break;
        case UART1: EUSCI_A1->IFG &= ~EUSCI_A_IFG_RXIFG; break;
        case UART2: EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG; break;
        case UART3: EUSCI_A3->IFG &= ~EUSCI_A_IFG_RXIFG; break;
        default: return;
    }
}

uint8_t hal_UART_DataAvailable(uint8_t channel)
{
    switch (channel) {
        case UART0: return EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG;
        case UART1: return EUSCI_A1->IFG & EUSCI_A_IFG_RXIFG;
        case UART2: return EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG;
        case UART3: return EUSCI_A3->IFG & EUSCI_A_IFG_RXIFG;
        default: return 0;
    }
}

uint8_t hal_UART_SpaceAvailable(uint8_t channel)
{
    switch (channel) {
        case UART0: return EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG;
        case UART1: return EUSCI_A1->IFG & EUSCI_A_IFG_TXIFG;
        case UART2: return EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG;
        case UART3: return EUSCI_A3->IFG & EUSCI_A_IFG_TXIFG;
        default: return 0;
    }
}

#ifdef USE_UART0
void EUSCIA0_IRQHandler(void) {
    UART_Rx_Handler(UART0);
    hal_UART_ClearRxIF(UART0);
    UART_Tx_Handler(UART0);
}
#endif

#ifdef USE_UART1
void EUSCIA1_IRQHandler(void) {
    UART_Rx_Handler(UART1);
    hal_UART_ClearRxIF(UART1);
    UART_Tx_Handler(UART1);
}
#endif

#ifdef USE_UART2
void EUSCIA2_IRQHandler(void) {
    UART_Rx_Handler(UART2);
    hal_UART_ClearRxIF(UART2);
    UART_Tx_Handler(UART2);
}
#endif

#ifdef USE_UART3
void EUSCIA3_IRQHandler(void) {
    UART_Rx_Handler(UART3);
    hal_UART_ClearRxIF(UART3);
    UART_Tx_Handler(UART3);
}
#endif

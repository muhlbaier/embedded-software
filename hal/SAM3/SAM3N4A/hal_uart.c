#include <stdint.h>
#include <stdbool.h>
#include "hal_uart.h"
#include "uart.h"
#include "chip.h"

extern uint32_t SystemCoreClock;
#define BAUD(b) ((SystemCoreClock + 8*b)/(16*b))

void hal_UART_Init(uint8_t channel, uint32_t baud) {
    if (channel) {
        PMC->PMC_WPMR = 0x504D4300; /* Disable write protect            */
        PMC->PMC_PCER = ((1UL << ID_PIOB) | /* enable PIOA clock                */
                (1UL << ID_UART1) | (1UL << ID_USART0)); /* enable UART clock                */
        PMC->PMC_WPMR = 0x504D4301; /* Enable write protect             */
        /* Configure UART1 Pins (PB3 = TX, PB2 = RX). */
        PIOB->PIO_WPMR = 0x50494F00; /* Disable write protect            */
        PIOB->PIO_IDR = (PIO_PB2A_URXD1 | PIO_PB3A_UTXD1);
        PIOB->PIO_PUDR = (PIO_PB2A_URXD1 | PIO_PB3A_UTXD1);
        PIOB->PIO_ABCDSR[0] &= ~(PIO_PB2A_URXD1 | PIO_PB3A_UTXD1);
        PIOB->PIO_ABCDSR[1] &= ~(PIO_PB2A_URXD1 | PIO_PB3A_UTXD1);
        PIOB->PIO_PDR = (PIO_PB2A_URXD1 | PIO_PB3A_UTXD1);
        PIOB->PIO_WPMR = 0x50494F01; /* Enable write protect             */
        /* Configure UART1 for baud. */
        UART1->UART_CR = (UART_CR_RSTRX | UART_CR_RSTTX) |
                (UART_CR_RXDIS | UART_CR_TXDIS);
        UART1->UART_IDR = 0xFFFFFFFF;
        UART1->UART_MR = (0x4 << 9) | /* No Parity                        */
                (0x0 << 14); /* Normal Mode                      */
        UART1->UART_BRGR = BAUD(baud);
        UART1->UART_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;
    } else {
        PMC->PMC_WPMR = 0x504D4300; /* Disable write protect            */
        PMC->PMC_PCER = ((1UL << ID_PIOA) | /* enable PIOA clock                */
                (1UL << ID_UART0)); /* enable UART clock                */
        PMC->PMC_WPMR = 0x504D4301; /* Enable write protect             */
        /* Configure UART0 Pins (PA10 = TX, PA9 = RX). */
        PIOA->PIO_WPMR = 0x50494F00; /* Disable write protect            */
        PIOA->PIO_IDR = (PIO_PA9A_URXD0 | PIO_PA10A_UTXD0);
        PIOA->PIO_PUDR = (PIO_PA9A_URXD0 | PIO_PA10A_UTXD0);
        PIOA->PIO_ABCDSR[0] &= ~(PIO_PA9A_URXD0 | PIO_PA10A_UTXD0);
        PIOA->PIO_ABCDSR[1] &= ~(PIO_PA9A_URXD0 | PIO_PA10A_UTXD0);
        PIOA->PIO_PDR = (PIO_PA9A_URXD0 | PIO_PA10A_UTXD0);
        /** @TODO there is some issue with the PIO_WPMR write protect register that is 
        preventing the USART from being configured when this happens.
        For now the workaround is to set-up the USART first.
        Commenting the following line of code out will leave the write protect in a 
        disabled state. This is how I (JE?) initially discovered the workaround.
         */
        PIOA->PIO_WPMR = 0x50494F01; /* Enable write protect             */
        /* Configure UART0 for baud. */
        UART0->UART_CR = (UART_CR_RSTRX | UART_CR_RSTTX) |
                (UART_CR_RXDIS | UART_CR_TXDIS);
        UART0->UART_IDR = 0xFFFFFFFF;
        UART0->UART_MR = (0x4 << 9) | /* No Parity                        */
                (0x0 << 14); /* Normal Mode                      */
        UART0->UART_BRGR = BAUD(baud);
        UART0->UART_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;
    }

    hal_UART_Enable(channel);
    hal_UART_EnableRxInterrupt(channel);
}

void hal_UART_Enable(uint8_t channel) {
    if (channel) {
        UART1->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
    } else {
        UART0->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
    }
}

void hal_UART_Disable(uint8_t channel) {
    if (channel) {
        UART1->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;
    } else {
        UART0->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;
    }
}

void hal_UART_EnableRxInterrupt(uint8_t channel) {
    if (channel) {
        UART1->UART_IER = UART_IER_RXRDY;
    } else {
        UART0->UART_IER = UART_IER_RXRDY;
    }
}

void hal_UART_EnableTxInterrupt(uint8_t channel) {
    if (channel) {
        UART1->UART_IER = UART_IER_TXEMPTY;
    } else {
        UART0->UART_IER = UART_IER_TXEMPTY;
    }
}

void hal_UART_DisableRxInterrupt(uint8_t channel) {
    if (channel) {
        UART1->UART_IDR = UART_IDR_RXRDY;
    } else {
        UART0->UART_IDR = UART_IDR_RXRDY;
    }
}

void hal_UART_DisableTxInterrupt(uint8_t channel) {
    if (channel) {
        UART1->UART_IDR = UART_IDR_TXEMPTY;
    } else {
        UART0->UART_IDR = UART_IDR_TXEMPTY;
    }
}

void hal_UART_TxByte(uint8_t channel, uint8_t c) {
    if (channel) {
        UART1->UART_THR = c;
    } else {
        UART0->UART_THR = c;
    }
}

uint8_t hal_UART_RxByte(uint8_t channel) {
    if (channel) {
        return UART1->UART_RHR;
    } else {
        return UART0->UART_RHR;
    }
}

uint8_t hal_UART_DataAvailable(uint8_t channel) {
    if (channel) {
        return ((UART1->UART_SR & UART_SR_RXRDY) != 0) ? 1 : 0;
    } else {
        return ((UART0->UART_SR & UART_SR_RXRDY) != 0) ? 1 : 0;
    }
}

uint8_t hal_UART_SpaceAvailable(uint8_t channel) {
    if (channel) {
        return (UART1->UART_SR & UART_SR_TXRDY) ? 1 : 0;
    } else {
        return (UART0->UART_SR & UART_SR_TXRDY) ? 1 : 0;
    }
}

#ifdef USE_UART0
void UART0_IRQHandler(void) {
    UART_Rx_Handler(0);
    UART_Tx_Handler(0);
}
#endif
#ifdef USE_UART1
void UART1_IRQHandler(void) {
    UART_Rx_Handler(1);
    UART_Tx_Handler(1);
}
#endif

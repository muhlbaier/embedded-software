#include <stdint.h>
#include "hal_general.h"
#include "hal_uart.h"
#incldue "uart.h"
#include "project_settings.h"
#include "macros.h"

#ifndef UART_INTERRUPT_PRIORITY
#define UART_INTERRUPT_PRIORITY 3
#endif

#ifndef PERIPHERAL_CLOCK
#define PERIPHERAL_CLOCK FCPU
#endif

// private function, could be changed to public if needed
static void SetBaud(uint8_t n, uint32_t baud);

void hal_UART_Init(uint8_t channel, uint32_t baud) {
    switch (channel) {
        case UART1_CH:
            TRISDbits.TRISD2 = 1; // set RX as input
            TRISDbits.TRISD3 = 0; // set TX as output
            IPC6bits.U1IP = UART_INTERRUPT_PRIORITY;
            U1STA = 0b1001010000000000;
            break;
        case UART2_CH:
            TRISFbits.TRISF4 = 1; // set RX as input
            TRISFbits.TRISF5 = 0; // set TX as output
            IPC8bits.U2IP = UART_INTERRUPT_PRIORITY;
            U2STA = 0b1001010000000000;
            break;
        default:
            return;
    }
             
    hal_UART_Disable(channel);
    SetBaud(channel, baud);
    hal_UART_ClearRxIF(channel);
    hal_UART_EnableRxInterrupt(channel);
    hal_UART_Enable(channel);
}

void hal_UART_Enable(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            U1MODEbits.ON = 1;
            break;
        case UART2_CH:
            U2MODEbits.ON = 1;
            break;
    }
}

void hal_UART_Disable(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            U1MODEbits.ON = 0;
            break;
        case UART2_CH:
            U2MODEbits.ON = 0;
            break;
    }
}

void hal_UART_EnableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            IEC0SET = _IEC0_U1RXIE_MASK;
            break;
        case UART2_CH:
            IEC1SET = _IEC1_U2RXIE_MASK;
            break;
        default:
            return;
    }
}

void hal_UART_EnableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            IEC0SET = _IEC0_U1TXIE_MASK;
            break;
        case UART2_CH:
            IEC1SET = _IEC1_U2TXIE_MASK;
            break;
        default:
            return;
    }
}

void hal_UART_DisableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            IEC0CLR = _IEC0_U1RXIE_MASK;
            break;
        case UART2_CH:
            IEC1CLR = _IEC1_U2RXIE_MASK;
            break;
        default:
            return;
    }
}

void hal_UART_DisableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            IEC0CLR = _IEC0_U1TXIE_MASK;
            break;
        case UART2_CH:
            IEC1CLR = _IEC1_U2TXIE_MASK;
            break;
        default:
            return;
    }
}

void hal_UART_TxByte(uint8_t channel, uint8_t c) {
    switch (channel) {
        case UART1_CH:
            U1TXREG = c;
            break;
        case UART2_CH:
            U2TXREG = c;
            break;
        default:
            return;
    }
}

uint8_t hal_UART_RxByte(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            return U1RXREG;
        case UART2_CH:
            return U2RXREG;  
        default:
            return 0;
    }
}

void hal_UART_ClearTxIF(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            IFS0CLR = _IFS0_U1TXIF_MASK;
            break;
        case UART2_CH:
            IFS1CLR = _IFS1_U2TXIF_MASK;
            break;
        default:
            return;
    }
}

void hal_UART_ClearRxIF(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            IFS0CLR = _IFS0_U1RXIF_MASK;
            break;
        case UART2_CH:
            IFS1CLR = _IFS1_U2RXIF_MASK;
            break;
        default:
            return;
    }
}

uint8_t hal_UART_DataAvailable(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            return U1STAbits.URXDA;
        case UART2_CH:
            return U2STAbits.URXDA;
        default:
            return 0;
    }
}

uint8_t hal_UART_SpaceAvailable(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            return !U1STAbits.UTXBF;
        case UART2_CH:
            return !U2STAbits.UTXBF;
        default:
            return 0;
    }
}

void SetBaud(uint8_t n, uint32_t baud) {
    int16_t error, error1, error2;
    uint16_t brg, brg1, brg2;
    uint32_t baud2, baud1;
    uint8_t brgh;
    brg1 = PERIPHERAL_CLOCK / (16 * baud) - 1;
    brg2 = brg1 + 1;
    baud1 = PERIPHERAL_CLOCK / (16 * (brg1 + 1));
    baud2 = PERIPHERAL_CLOCK / (16 * (brg2 + 1));
    if (baud1 > baud) error1 = baud1 - baud;
    else error1 = baud - baud1;
    if (baud2 > baud) error2 = baud2 - baud;
    else error2 = baud - baud2;
    if (error1 < error2) {
        error = error1;
        brg = brg1;
    } else {
        error = error2;
        brg = brg2;
    }
    if (error * 1000 / baud > 25) {
        brg1 = PERIPHERAL_CLOCK / (4 * baud) - 1;
        brg2 = brg1 + 1;
        baud1 = PERIPHERAL_CLOCK / (4 * (brg1 + 1));
        baud2 = PERIPHERAL_CLOCK / (4 * (brg2 + 1));
        if (baud1 > baud) error1 = baud1 - baud;
        else error1 = baud - baud1;
        if (baud2 > baud) error2 = baud2 - baud;
        else error2 = baud - baud2;
        if (error1 < error2) {
            brg = brg1;
        } else {
            brg = brg2;
        }
        brgh = 1;
    } else {
        brgh = 0;
    }

    switch (n) {
        case UART1_CH:
            U1MODEbits.BRGH = brgh;
            U1BRG = brg;
            break;
        case UART2_CH:
            U2MODEbits.BRGH = brgh;
            U2BRG = brg;
            break;
        default:
            break;
    }
}

uint8_t hal_UART_RxInterruptEnabled(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            return IEC0bits.U1RXIE;
        case UART2_CH:
            return IEC1bits.U2RXIE;
        default:
            return 0;
    }
}

uint8_t hal_UART_TxInterruptEnabled(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            return IEC0bits.U1TXIE;
        case UART2_CH:
            return IEC1bits.U2TXIE;
        default:
            return 0;
    }
}

#ifdef USE_UART1
void __attribute__((vector(_UART_1_VECTOR), interrupt(), nomips16)) _UART1_ISR(void) {
    UART_Rx_Handler(UART1_CH);
    hal_UART_ClearRxIF(UART1_CH);
    UART_Tx_Handler(UART1_CH);
}
#endif

#ifdef USE_UART2
void __attribute__((vector(_UART_2_VECTOR), interrupt(), nomips16)) _UART2_ISR(void) {
    UART_Rx_Handler(UART2_CH);
    hal_UART_ClearRxIF(UART2_CH);
    UART_Tx_Handler(UART2_CH);
}
#endif

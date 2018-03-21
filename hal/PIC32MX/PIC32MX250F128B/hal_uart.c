#include <stdint.h>
#include "hal_general.h"
#include "uart.h"
#include "project_settings.h"
#include "macros.h"

#define UART1 1
#define UART2_CH 2

#define Set_U1Tx_PPS(pin) CAT2(pin, R) = 1 // U1TX
#define Set_U2Tx_PPS(pin) CAT2(pin, R) = 2 // U2TX
#define Set_U1Rx_PPS(pin) U1RXR = CAT2(IN_PIN_PPS3_, pin)
#define Set_U2Rx_PPS(pin) U2RXR = CAT2(IN_PIN_PPS2_, pin)

// defaults for PPS if not set in hal_settings.h
#ifndef UART1_TX_PIN
#define UART1_TX_PIN RPA0
#endif
#ifndef UART1_RX_PIN
#define UART1_RX_PIN RPA2
#endif
// UART2 is connected to the pins used on the Microstick II header
#ifndef UART2_TX_PIN
#define UART2_TX_PIN RPB10
#endif
#ifndef UART2_RX_PIN
#define UART2_RX_PIN RPB11
#endif

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
        case UART1:
            Set_U1Tx_PPS(UART1_TX_PIN);       // UART1_TX_PIN should be defined in hal_settings.h
                                              // e.g. #define UART1_TX_PIN PRB10
                                              // Options are PR: A0, B3, B4, B7, B15, C0, C5, C7
            Set_U1Rx_PPS(UART1_RX_PIN);       // UART1_RX_PIN should be defined in hal_settings.h
                                              // Options are PR: A2, A4, B2, B6, B13, C1, C3, C6
            IPC8bits.U1IP = UART_INTERRUPT_PRIORITY;
            // interrupt when transmit buffer is empty, enable RX and TX, rx interrupt
            // when not empty
                    //FEDCBA9876543210
            U1STA = 0b1001010000000000;
            break;
        case UART2_CH:
            Set_U2Tx_PPS(UART2_TX_PIN);       // UART2_TX_PIN should be defined in hal_settings.h
                                              // e.g. #define UART2_TX_PIN PRB10
                                              // Options are PR: A3, B0, B9, B10, B14, C2, C4, C9
            Set_U2Rx_PPS(UART2_RX_PIN);       // UART2_RX_PIN should be defined in hal_settings.h
                                              // Options are PR: A1, A8, A9, B1, B5, B8, B11, C8
            IPC9bits.U2IP = UART_INTERRUPT_PRIORITY;
            // interrupt when transmit buffer is empty, enable RX and TX, rx interrupt
            // when not empty
                    //FEDCBA9876543210
            U2STA = 0b1001010000000000;
            break;
    }
    hal_UART_Disable(channel);
    SetBaud(channel, baud);
    hal_UART_ClearRxIF(channel);
    hal_UART_EnableRxInterrupt(channel);
    hal_UART_Enable(channel);
}

void hal_UART_Enable(uint8_t channel) {
    switch (channel) {
        case UART1:
            U1MODEbits.ON = 1;
            break;
        case UART2_CH:
            U2MODEbits.ON = 1;
            break;
        default:
            return;
    }
}

void hal_UART_Disable(uint8_t channel) {
    switch (channel) {
        case UART1:
            U1MODEbits.ON = 0;
            break;
        case UART2_CH:
            U2MODEbits.ON = 0;
            break;
        default:
            return;
    }
}

void hal_UART_EnableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1:
            IEC1SET = _IEC1_U1RXIE_MASK;
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
        case UART1:
            IEC1SET = _IEC1_U1TXIE_MASK;
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
        case UART1:
            IEC1CLR = _IEC1_U1RXIE_MASK;
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
        case UART1:
            IEC1CLR = _IEC1_U1TXIE_MASK;
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
        case UART1:
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
        case UART1:
            return U1RXREG;
        case UART2_CH:
            return U2RXREG;
        default:
            return 0;
    }
}

void hal_UART_ClearTxIF(uint8_t channel) {
    switch (channel) {
        case UART1:
            IFS1CLR = _IFS1_U1TXIF_MASK;
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
        case UART1:
            IFS1CLR = _IFS1_U1RXIF_MASK;
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
        case UART1:
            return U1STAbits.URXDA;
        case UART2_CH:
            return U2STAbits.URXDA;
        default:
            return 0;
    }
}

uint8_t hal_UART_SpaceAvailable(uint8_t channel) {
    switch (channel) {
        case UART1:
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
        case UART1:
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
            return IEC1bits.U1RXIE;
        case UART2_CH:
            return IEC1bits.U2RXIE;
        default:
            return 0;
    }
}

uint8_t hal_UART_TxInterruptEnabled(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            return IEC1bits.U1TXIE;
        case UART2_CH:
            return IEC1bits.U2TXIE;
        default:
            return 0;
    }
}

void __attribute__((vector(_UART_1_VECTOR), interrupt(), nomips16)) _UART1_ISR(void) {
    UART_Rx_Handler(UART1);
    hal_UART_ClearRxIF(UART1);
    UART_Tx_Handler(UART1);
    if(U1STAbits.OERR) {
        UART_Error_Handler(UART1_CH, OVERRUN_ERROR);
        U1STAbits.OERR = 0;
    }
}

void __attribute__((vector(_UART_2_VECTOR), interrupt(), nomips16)) _UART2_ISR(void) {
    UART_Rx_Handler(UART2_CH);
    hal_UART_ClearRxIF(UART2_CH);
    UART_Tx_Handler(UART2_CH);
    if(U2STAbits.OERR) {
        UART_Error_Handler(UART2_CH, OVERRUN_ERROR);
        U2STAbits.OERR = 0;
    }
}

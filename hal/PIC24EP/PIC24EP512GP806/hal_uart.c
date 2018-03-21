#include <stdint.h>
#include "hal_general.h"
#include "hal_uart.h"
#include "uart.h"
#include "project_settings.h"
#include "macros.h"

#define Set_U1Tx_PPS(pin) SetPPS_Out(pin) = 1 // U1TX
#define Set_U2Tx_PPS(pin) SetPPS_Out(pin) = 3 // U2TX
#define Set_U3Tx_PPS(pin) SetPPS_Out(pin) = 27 // U3TX
#define Set_U4Tx_PPS(pin) SetPPS_Out(pin) = 29 // U4TX

#define Set_U1Rx_PPS(pin) RPINR18bits.U1RXR = pin
#define Set_U2Rx_PPS(pin) RPINR19bits.U2RXR = pin
#define Set_U3Rx_PPS(pin) RPINR27bits.U3RXR = pin
#define Set_U4Rx_PPS(pin) RPINR28bits.U4RXR = pin

// defaults for PPS if not set in settings.h
#ifdef USE_UART1
#ifndef UART1_TX_PIN
#error "Need to have define for UART1_TX_PIN ex. #define UART1_TX_PIN 19"
#endif
#ifndef UART1_RX_PIN
#error "Need to have define for UART1_RX_PIN ex. #define UART1_RX_PIN 19"
#endif
#endif

#ifdef USE_UART2
#ifndef UART2_TX_PIN
#error "Need to have define for UART2_TX_PIN ex. #define UART2_TX_PIN 19"
#endif
#ifndef UART2_RX_PIN
#error "Need to have define for UART2_RX_PIN ex. #define UART2_RX_PIN 19"
#endif
#endif

#ifdef USE_UART3
#ifndef UART3_TX_PIN
#error "Need to have define for UART3_TX_PIN ex. #define UART3_TX_PIN 19"
#endif
#ifndef UART3_RX_PIN
#error "Need to have define for UART3_RX_PIN ex. #define UART3RX_PIN 19"
#endif
#endif

#ifdef USE_UART4
#ifndef UART4_TX_PIN
#error "Need to have define for UART4_TX_PIN ex. #define UART4_TX_PIN 19"
#endif
#ifndef UART4_RX_PIN
#error "Need to have define for UART4_RX_PIN ex. #define UART4_RX_PIN 19"
#endif
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
#ifdef USE_UART1
        case UART1_CH:
            Set_U1Tx_PPS(UART1_TX_PIN);       // UART1_TX_PIN should be defined in settings.h
                                              // e.g. #define UART1_TX_PIN 10
                                              // Options are 1-25
            Set_U1Rx_PPS(UART1_RX_PIN);       // UART1_RX_PIN should be defined in settings.h
                                              // Options are 1-25
            IPC2bits.U1RXIP = UART_INTERRUPT_PRIORITY;
			IPC3bits.U1TXIP = UART_INTERRUPT_PRIORITY;
            // interrupt when transmit buffer is empty, enable RX and TX, rx interrupt
            // when not empty
                    //FEDCBA9876543210
            U1STA = 0b1000000000000000;
            break;
#endif
#ifdef USE_UART2
        case UART2_CH:
            Set_U2Tx_PPS(UART2_TX_PIN);       // UART2_TX_PIN should be defined in settings.h
            Set_U2Rx_PPS(UART2_RX_PIN);       // UART2_RX_PIN should be defined in settings.h
            IPC7bits.U2RXIP = UART_INTERRUPT_PRIORITY;
			IPC7bits.U2TXIP = UART_INTERRUPT_PRIORITY;
            // interrupt when transmit buffer is empty, enable RX and TX, rx interrupt
            // when not empty
                    //FEDCBA9876543210
            U2STA = 0b1000000000000000;
            break;
#endif
#ifdef USE_UART3
        case UART3_CH:
            Set_U3Tx_PPS(UART3_TX_PIN);       // UART3_TX_PIN should be defined in settings.h
            Set_U3Rx_PPS(UART3_RX_PIN);       // UART3_RX_PIN should be defined in settings.h
            IPC20bits.U3RXIP = UART_INTERRUPT_PRIORITY;
			IPC20bits.U3TXIP = UART_INTERRUPT_PRIORITY;
            // interrupt when transmit buffer is empty, enable RX and TX, rx interrupt
            // when not empty
                    //FEDCBA9876543210
            U3STA = 0b1000000000000000;
            break;
#endif
#ifdef USE_UART4
        case UART4_CH:
            Set_U4Tx_PPS(UART4_TX_PIN);       // UART4_TX_PIN should be defined in settings.h
            Set_U4Rx_PPS(UART4_RX_PIN);       // UART4_RX_PIN should be defined in settings.h
            IPC22bits.U4RXIP = UART_INTERRUPT_PRIORITY;
			IPC22bits.U4TXIP = UART_INTERRUPT_PRIORITY;
            // interrupt when transmit buffer is empty, enable RX and TX, rx interrupt
            // when not empty
                    //FEDCBA9876543210
            U4STA = 0b1000000000000000;
            break;
#endif
    }
    hal_UART_Disable(channel);
    SetBaud(channel, baud);
    hal_UART_ClearRxIF(channel);
    hal_UART_EnableRxInterrupt(channel);
    hal_UART_Enable(channel);
    hal_UART_TxEnable(channel);
}

void hal_UART_Enable(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            U1MODEbits.UARTEN = 1;
            break;
        case UART2_CH:
            U2MODEbits.UARTEN = 1;
            break;
        case UART3_CH:
            U3MODEbits.UARTEN = 1;
            break;
        case UART4_CH:
            U4MODEbits.UARTEN = 1;
            break;
        default:
            return;
    }
}

void hal_UART_Disable(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            U1MODEbits.UARTEN = 0;
            break;
        case UART2_CH:
            U2MODEbits.UARTEN = 0;
            break;
        case UART3_CH:
            U3MODEbits.UARTEN = 0;
            break;
        case UART4_CH:
            U4MODEbits.UARTEN = 0;
            break;            
        default:
            return;
    }
}

void hal_UART_TxEnable(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            U1STAbits.UTXEN = 1;
            break;
        case UART2_CH:
            U2STAbits.UTXEN = 1;
            break;
        case UART3_CH:
            U3STAbits.UTXEN = 1;
            break;
        case UART4_CH:
            U4STAbits.UTXEN = 1;
            break;
        default:
            return;
    }
}

void hal_UART_EnableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            IEC0bits.U1RXIE = 1;
            break;
        case UART2_CH:
            IEC1bits.U2RXIE = 1;
            break;
        case UART3_CH:
            IEC5bits.U3RXIE = 1;
            break;
        case UART4_CH:
            IEC5bits.U4RXIE = 1;
            break;
        default:
            return;
    }
}

void hal_UART_EnableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            IEC0bits.U1TXIE = 1;
            break;
        case UART2_CH:
            IEC1bits.U2TXIE = 1;
            break;
        case UART3_CH:
            IEC5bits.U3TXIE = 1;
            break;
        case UART4_CH:
            IEC5bits.U4TXIE = 1;
            break;
        default:
            return;
    }
}

void hal_UART_DisableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            IEC0bits.U1RXIE = 0;
            break;
        case UART2_CH:
            IEC1bits.U2RXIE = 0;
            break;
        case UART3_CH:
            IEC5bits.U3RXIE = 0;
            break;
        case UART4_CH:
            IEC5bits.U4RXIE = 0;
            break;
        default:
            return;
    }
}

void hal_UART_DisableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            IEC0bits.U1TXIE = 0;
            break;
        case UART2_CH:
            IEC1bits.U2TXIE = 0;
            break;
        case UART3_CH:
            IEC5bits.U3TXIE = 0;
            break;
        case UART4_CH:
            IEC5bits.U4TXIE = 0;
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
        case UART3_CH:
            U3TXREG = c;
            break;
        case UART4_CH:
            U4TXREG = c;
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
        case UART3_CH:
            return U3RXREG;
        case UART4_CH:
            return U4RXREG;
        default:
            return 0;
    }
}

void hal_UART_ClearTxIF(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            IFS0bits.U1TXIF = 0;
            break;
        case UART2_CH:
            IFS1bits.U2TXIF = 0;
            break;
        case UART3_CH:
            IFS5bits.U3TXIF = 0;
            break;
        case UART4_CH:
            IFS5bits.U4TXIF = 0;
            break;
        default:
            return;
    }
}

void hal_UART_ClearRxIF(uint8_t channel) {
    switch (channel) {
        case UART1_CH:
            IFS0bits.U1RXIF = 0;
            break;
        case UART2_CH:
            IFS1bits.U2RXIF = 0;
            break;
        case UART3_CH:
            IFS5bits.U3RXIF = 0;
            break;
        case UART4_CH:
            IFS5bits.U4RXIF = 0;
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
        case UART3_CH:
            return U3STAbits.URXDA;
        case UART4_CH:
            return U4STAbits.URXDA;
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
        case UART3_CH:
            return !U3STAbits.UTXBF;
        case UART4_CH:
            return !U4STAbits.UTXBF;
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
        case UART3_CH:
            U3MODEbits.BRGH = brgh;
            U3BRG = brg;
            break;
        case UART4_CH:
            U4MODEbits.BRGH = brgh;
            U4BRG = brg;
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
        case UART3_CH:
            return IEC5bits.U3RXIE;
        case UART4_CH:
            return IEC5bits.U4RXIE;
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
        case UART3_CH:
            return IEC5bits.U3TXIE;
        case UART4_CH:
            return IEC5bits.U4TXIE;
        default:
            return 0;
    }
}

void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void) {
    UART_Rx_Handler(UART1_CH);
    hal_UART_ClearRxIF(UART1_CH);
}

void __attribute__((interrupt, auto_psv)) _U2RXInterrupt(void) {
    UART_Rx_Handler(UART2_CH);
    hal_UART_ClearRxIF(UART2_CH);
}

void __attribute__((interrupt, auto_psv)) _U3RXInterrupt(void) {
    UART_Rx_Handler(UART3_CH);
    hal_UART_ClearRxIF(UART3_CH);
}

void __attribute__((interrupt, auto_psv)) _U4RXInterrupt(void) {
    UART_Rx_Handler(UART4_CH);
    hal_UART_ClearRxIF(UART4_CH);
}

void __attribute__((interrupt, auto_psv)) _U1TXInterrupt(void) {
    UART_Tx_Handler(UART1_CH);
    hal_UART_ClearTxIF(UART1_CH);
}

void __attribute__((interrupt, auto_psv)) _U2TXInterrupt(void) {
    UART_Tx_Handler(UART2_CH);
    hal_UART_ClearTxIF(UART2_CH);
}

void __attribute__((interrupt, auto_psv)) _U3TXInterrupt(void) {
    UART_Tx_Handler(UART3_CH);
    hal_UART_ClearTxIF(UART3_CH);
}

void __attribute__((interrupt, auto_psv)) _U4TXInterrupt(void) {
    UART_Tx_Handler(UART4_CH);
    hal_UART_ClearTxIF(UART4_CH);
}

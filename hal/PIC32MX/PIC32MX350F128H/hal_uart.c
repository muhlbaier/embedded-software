#include <stdint.h>
#include "hal_general.h"
#include "uart.h"
#include "project_settings.h"
#include "macros.h"

#include "hal_uart.h"

// not mappable to any pin, see output pin selection (table 12-2) of datasheet
#define Set_U1Tx_PPS(pin) CAT2(pin, R) = 3 // U1TX
#define Set_U2Tx_PPS(pin) CAT2(pin, R) = 1 // U2TX
#define Set_U3Tx_PPS(pin) CAT2(pin, R) = 1 // U3TX
#define Set_U4Tx_PPS(pin) CAT2(pin, R) = 2 // U4TX
// not mappable to any pin, see input pin selection (table 12-2) of datasheet
#define Set_U1Rx_PPS(pin) U1RXR = CAT2(IN_PIN_PPS1_, pin)
#define Set_U2Rx_PPS(pin) U2RXR = CAT2(IN_PIN_PPS1_, pin)
#define Set_U3Rx_PPS(pin) U3RXR = CAT2(IN_PIN_PPS2_, pin)
#define Set_U4Rx_PPS(pin) U4RXR = CAT2(IN_PIN_PPS4_, pin)

// defaults for PPS if not set in project_settings.h
#ifndef UART1_TX_PIN
#define UART1_TX_PIN RPD3
#endif
#ifndef UART1_RX_PIN
#define UART1_RX_PIN RPD2
#endif
#ifndef UART2_TX_PIN
#define UART2_TX_PIN RPG7
#endif
#ifndef UART2_RX_PIN
#define UART2_RX_PIN RPG8
#endif
#ifndef UART3_TX_PIN
#define UART3_TX_PIN RPF4
#endif
#ifndef UART3_RX_PIN
#define UART3_RX_PIN RPF5
#endif
#ifndef UART4_TX_PIN
#define UART4_TX_PIN RPD9
#endif
#ifndef UART4_RX_PIN
#define UART4_RX_PIN RPD8
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
    hal_UART_Disable(channel);
    SetBaud(channel, baud);
    switch (channel) {
        case UART1_CH:
            Set_U1Tx_PPS(UART1_TX_PIN);       // UART1_TX_PIN should be defined in project_settings.h
                                              // e.g. #define UART1_TX_PIN PRB10
                                              // Options are RP: D3 G7 F4 D11 F0 B1 E5 C13 B3 F3
            Set_U1Rx_PPS(UART1_RX_PIN);       // UART1_RX_PIN should be defined in project_settings.h
                                              // Options are RP: D2 G8 F4 D10 F1 B9 B10 C14 B5 F2
            IPC7bits.U1IP = UART_INTERRUPT_PRIORITY;
            // interrupt when transmit buffer is empty, enable RX and TX, rx interrupt
            // when not empty
                    //FEDCBA9876543210
            U1STA = 0b1001010000000000;
            break;
        case UART2_CH:
            Set_U2Tx_PPS(UART2_TX_PIN);       // UART2_TX_PIN should be defined in project_settings.h
                                              // e.g. #define UART2_TX_PIN PRB10
                                              // Options are RP: D3 G7 F4 D11 F0 B1 E5 C13 B3 F3
            Set_U2Rx_PPS(UART2_RX_PIN);       // UART2_RX_PIN should be defined in project_settings.h
                                              // Options are RP: D2 G8 F4 D10 F1 B9 B10 C14 B5 F2
            IPC9bits.U2IP = UART_INTERRUPT_PRIORITY;
            // interrupt when transmit buffer is empty, enable RX and TX, rx interrupt
            // when not empty
                    //FEDCBA9876543210
            U2STA = 0b1001010000000000;
            break;
        case UART3_CH:
            Set_U3Tx_PPS(UART3_TX_PIN);       // UART3_TX_PIN should be defined in project_settings.h
                                              // e.g. #define UART3_TX_PIN PRB10
                                              // Options are RP: D2 G8 F4 D10 C14 B5
            Set_U3Rx_PPS(UART3_RX_PIN);       // UART3_RX_PIN should be defined in project_settings.h
                                              // Options are RP: D3 G7 F5 D11 F0 B1 E5 C13 B3 F2
            IPC9bits.U3IP = UART_INTERRUPT_PRIORITY;
            // interrupt when transmit buffer is empty, enable RX and TX, rx interrupt
            // when not empty
                    //FEDCBA9876543210
            U3STA = 0b1001010000000000;
            break;
        case UART4_CH:
            Set_U4Tx_PPS(UART4_TX_PIN);       // UART4_TX_PIN should be defined in project_settings.h
                                              // e.g. #define UART4_TX_PIN PRB10
                                              // Options are RP: D9 G6 B8 B15 D4 B0 E3 B7 B2
            Set_U4Rx_PPS(UART4_RX_PIN);       // UART4_RX_PIN should be defined in project_settings.h
                                              // Options are RP: D1 G9 B14 D0 D8 B6 D5 B2 F3 F2
            IPC9bits.U4IP = UART_INTERRUPT_PRIORITY;
            // interrupt when transmit buffer is empty, enable RX and TX, rx interrupt
            // when not empty
                    //FEDCBA9876543210
            U4STA = 0b1001010000000000;
            break;
    }
    hal_UART_ClearRxIF(channel);
    hal_UART_EnableRxInterrupt(channel);
    hal_UART_Enable(channel);
}

void hal_UART_Enable(uint8_t channel) {
    switch (channel) {
        case UART1_CH: U1MODEbits.ON = 1; break;
        case UART2_CH: U2MODEbits.ON = 1; break;
        case UART3_CH: U3MODEbits.ON = 1; break;
        case UART4_CH: U4MODEbits.ON = 1; break;
        default: return;
    }
}

void hal_UART_Disable(uint8_t channel) {
    switch (channel) {
        case UART1_CH: U1MODEbits.ON = 0; break;
        case UART2_CH: U2MODEbits.ON = 0; break;
        case UART3_CH: U3MODEbits.ON = 0; break;
        case UART4_CH: U4MODEbits.ON = 0; break;
        default: return;
    }
}

void hal_UART_EnableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH: IEC1SET = _IEC1_U1RXIE_MASK; break;
        case UART2_CH: IEC1SET = _IEC1_U2RXIE_MASK; break;
        case UART3_CH: IEC1SET = _IEC1_U3RXIE_MASK; break;
        case UART4_CH: IEC2SET = _IEC2_U4RXIE_MASK; break;
        default: return;
    }
}

void hal_UART_EnableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH: IEC1SET = _IEC1_U1TXIE_MASK; break;
        case UART2_CH: IEC1SET = _IEC1_U2TXIE_MASK; break;
        case UART3_CH: IEC2SET = _IEC2_U3TXIE_MASK; break;
        case UART4_CH: IEC2SET = _IEC2_U4TXIE_MASK; break;
        default: return;
    }
}

void hal_UART_DisableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH: IEC1CLR = _IEC1_U1RXIE_MASK; break;
        case UART2_CH: IEC1CLR = _IEC1_U2RXIE_MASK; break;
        case UART3_CH: IEC1CLR = _IEC1_U3RXIE_MASK; break;
        case UART4_CH: IEC2CLR = _IEC2_U4RXIE_MASK; break;
        default: return;
    }
}

void hal_UART_DisableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH: IEC1CLR = _IEC1_U1TXIE_MASK; break;
        case UART2_CH: IEC1CLR = _IEC1_U2TXIE_MASK; break;
        case UART3_CH: IEC2CLR = _IEC2_U3TXIE_MASK; break;
        case UART4_CH: IEC2CLR = _IEC2_U4TXIE_MASK; break;
        default: return;
    }
}

void hal_UART_TxByte(uint8_t channel, uint8_t c) {
    switch (channel) {
        case UART1_CH: U1TXREG = c; break;
        case UART2_CH: U2TXREG = c; break;
        case UART3_CH: U3TXREG = c; break;
        case UART4_CH: U4TXREG = c; break;
        default: return;
    }
}

uint8_t hal_UART_RxByte(uint8_t channel) {
    switch (channel) {
        case UART1_CH: return U1RXREG;
        case UART2_CH: return U2RXREG;
        case UART3_CH: return U3RXREG;
        case UART4_CH: return U4RXREG;
        default: return 0;
    }
}

void hal_UART_ClearTxIF(uint8_t channel) {
    switch (channel) {
        case UART1_CH: IFS1CLR = _IFS1_U1TXIF_MASK; break;
        case UART2_CH: IFS1CLR = _IFS1_U2TXIF_MASK; break;
        case UART3_CH: IFS2CLR = _IFS2_U3TXIF_MASK; break;
        case UART4_CH: IFS2CLR = _IFS2_U4TXIF_MASK; break;
        default: return;
    }
}

void hal_UART_ClearRxIF(uint8_t channel) {
    switch (channel) {
        case UART1_CH: IFS1CLR = _IFS1_U1RXIF_MASK; break;
        case UART2_CH: IFS1CLR = _IFS1_U2RXIF_MASK; break;
        case UART3_CH: IFS1CLR = _IFS1_U3RXIF_MASK; break;
        case UART4_CH: IFS2CLR = _IFS2_U4RXIF_MASK; break;
        default: return;
    }
}

uint8_t hal_UART_DataAvailable(uint8_t channel) {
    switch (channel) {
        case UART1_CH: return U1STAbits.URXDA;
        case UART2_CH: return U2STAbits.URXDA;
        case UART3_CH: return U3STAbits.URXDA;
        case UART4_CH: return U4STAbits.URXDA;
        default: return 0;
    }
}

uint8_t hal_UART_SpaceAvailable(uint8_t channel) {
    switch (channel) {
        case UART1_CH: return !U1STAbits.UTXBF;
        case UART2_CH: return !U2STAbits.UTXBF;
        case UART3_CH: return !U3STAbits.UTXBF;
        case UART4_CH: return !U4STAbits.UTXBF;
        default: return 0;
    }
}

// non typical function
uint8_t hal_UART_DoneTransmitting(uint8_t channel) {
    switch (channel) {
        case UART1_CH: return U1STAbits.TRMT;
        case UART2_CH: return U2STAbits.TRMT;
        case UART3_CH: return U3STAbits.TRMT;
        case UART4_CH: return U4STAbits.TRMT;
        default: return 0;
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
        case UART1_CH: return IEC1bits.U1RXIE;
        case UART2_CH: return IEC1bits.U2RXIE;
        case UART3_CH: return IEC1bits.U3RXIE;
        case UART4_CH: return IEC2bits.U4RXIE;
        default: return 0;
    }
}

uint8_t hal_UART_TxInterruptEnabled(uint8_t channel) {
    switch (channel) {
        case UART1_CH: return IEC1bits.U1TXIE;
        case UART2_CH: return IEC1bits.U2TXIE;
        case UART3_CH: return IEC2bits.U3TXIE;
        case UART4_CH: return IEC2bits.U4TXIE;
        default: return 0;
    }
}

void __attribute__((vector(_UART_1_VECTOR), interrupt(), nomips16)) _UART1_ISR(void) {
    UART_Rx_Handler(UART1_CH);
    hal_UART_ClearRxIF(UART1_CH);
    UART_Tx_Handler(UART1_CH);
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

void __attribute__((vector(_UART_3_VECTOR), interrupt(), nomips16)) _UART3_ISR(void) {
    UART_Rx_Handler(UART3_CH);
    hal_UART_ClearRxIF(UART3_CH);
    UART_Tx_Handler(UART3_CH);
    if(U3STAbits.OERR) {
        UART_Error_Handler(UART3_CH, OVERRUN_ERROR);
        U3STAbits.OERR = 0;
    }
}

void __attribute__((vector(_UART_4_VECTOR), interrupt(), nomips16)) _UART4_ISR(void) {
    UART_Rx_Handler(UART4_CH);
    hal_UART_ClearRxIF(UART4_CH);
    UART_Tx_Handler(UART4_CH);
    if(U4STAbits.OERR) {
        UART_Error_Handler(UART4_CH, OVERRUN_ERROR);
        U4STAbits.OERR = 0;
    }
}

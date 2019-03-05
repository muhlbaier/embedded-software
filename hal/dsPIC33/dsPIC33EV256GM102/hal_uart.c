/**
 * @file hal_uart.c
 * 
 * @addtogroup hal_uart_dspic33ev256gm102 HAL UART Module for dsPIC33EV256GM102
 * @ingroup uart
 * 
 * Implementation of UART on the dsPIC33EV256GM102.
 * 
 * Pin configuration:
 * - UART 1 RX = Pin 11 = RP36
 * - UART 1 TX = Pin 12 = RP20
 * - UART 2 RX = Pin 17 = RP40
 * - UART 2 TX = Pin 18 = RP41
 * 
 * Provides extra functions to use the extra features of the dsPIC UART module
 * that are not provided in the HAL.
 * 
 * Created on: Mar 4, 2019
 *    @author: Nate Hoffman
 * 
 * @version 1.0.0
 */

#include "hal_general.h"
#include "hal_uart.h"
#include "uart.h"
#include "project_settings.h"
#include "macros.h"

#define UART1_RX_PIN 0b0100100  // UART1 RX = Pin 11 = RP36
#define UART2_RX_PIN 0b0101000  // UART2 RX = Pin 17 = RP40
#define UART1_TX_PIN 0b000001   // UART1 TX = Pin 12 = RP20
#define UART2_TX_PIN 0b000011   // UART2 TX = Pin 18 = RP41

#define UART_INTERRUPT_PRIORITY 2

#ifdef USE_UART0
    #ifndef UART1_TX_PIN
    #error "Need to have define for UART1_TX_PIN ex define UART1_TX_PIN 19"
    #endif
    #ifndef UART1_RX_PIN
    #error "Need to have define for UART1_RX_PIN ex define UART1_RX_PIN 19"
    #endif
#endif

#ifdef USE_UART1
    #ifndef UART2_TX_PIN
    #error "Need to have define for UART2_TX_PIN ex define UART2_TX_PIN 19"
    #endif
    #ifndef UART2_RX_PIN
    #error "Need to have define for UART2_RX_PIN ex define UART2_RX_PIN 19"
    #endif
#endif

static void SetBaud(uint8_t channel, uint32_t baud);

void hal_UART_Init(uint8_t channel, uint32_t baud)
{
    switch(channel)
    {
#ifdef USE_UART0
        case UART1_CH:
            RPOR0bits.RP20R = UART1_TX_PIN; // UART1 TX = Pin 12 = RP20
            RPINR18bits.U1RXR = UART1_RX_PIN; // UART1 RX = Pin 11 = RP36
            IPC2bits.U1RXIP = UART_INTERRUPT_PRIORITY;
            IPC3bits.U1TXIP = UART_INTERRUPT_PRIORITY;
            U1STA = 0b1000000000000000;
            break;
#endif
#ifdef USE_UART1
        case UART2_CH:
            RPOR3bits.RP40R = UART2_TX_PIN; // UART2 TX = Pin 18 = RP41
            RPINR19bits.U2RXR = UART2_RX_PIN; // UART2 RX = Pin 17 = RP40
            IPC7bits.U2RXIP = UART_INTERRUPT_PRIORITY;
            IPC7bits.U2TXIP = UART_INTERRUPT_PRIORITY;
            U2STA = 0b1000000000000000;
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

void hal_UART_Enable(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
            U1MODEbits.UARTEN = 1;
            break;
        case UART2_CH:
            U2MODEbits.UARTEN = 1;
            break;
        default:
            return;
    }
}

void hal_UART_Disable(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
            U1MODEbits.UARTEN = 0;
            break;
        case UART2_CH:
            U2MODEbits.UARTEN = 0;
            break;
        default:
            return;
    }
}

void hal_UART_TxEnable(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
            U1STAbits.UTXEN = 1;
            break;
        case UART2_CH:
            U2STAbits.UTXEN = 1;
            break;
        default:
            return;
    }
}

void hal_UART_EnableRxInterrupt(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
            IEC0bits.U1RXIE = 1;
            break;
        case UART2_CH:
            IEC1bits.U2RXIE = 1;
            break;
        default:
            return;
    }
}

void hal_UART_DisableRxInterrupt(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
            IEC0bits.U1RXIE = 0;
            break;
        case UART2_CH:
            IEC1bits.U2RXIE = 0;
            break;
        default:
            return;
    }
}

uint8_t hal_UART_RxInterruptEnabled(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
            return IEC0bits.U1RXIE;
        case UART2_CH:
            return IEC1bits.U2RXIE;
        default:
            return 0;
    }
}

void hal_UART_EnableTxInterrupt(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
            IEC0bits.U1TXIE = 1;
            break;
        case UART2_CH:
            IEC1bits.U2TXIE = 1;
            break;
        default:
            return;
    }
}

void hal_UART_DisableTxInterrupt(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
            IEC0bits.U1TXIE = 0;
            break;
        case UART2_CH:
            IEC1bits.U2TXIE = 0;
            break;
        default:
            return;
    }
}

uint8_t hal_UART_TxInterruptEnabled(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
            return IEC0bits.U1TXIE;
        case UART2_CH:
            return IEC1bits.U2TXIE;
        default:
            return 0;
    }
}

uint8_t hal_UART_RxByte(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
            return U1RXREG;
        case UART2_CH:
            return U2RXREG;
        default:
            return 0;
    }
}

void hal_UART_TxByte(uint8_t channel, uint8_t c)
{
    switch(channel)
    {
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

void hal_UART_ClearTxIF(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
            IFS0bits.U1TXIF = 0;
            break;
        case UART2_CH:
            IFS1bits.U2TXIF = 0;
            break;
        default:
            return;
    }
}

void hal_UART_ClearRxIF(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
            IFS0bits.U1RXIF = 0;
            break;
        case UART2_CH:
            IFS1bits.U2RXIF = 0;
            break;
        default:
            return;
    }
}

uint8_t hal_UART_DataAvailable(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
           return U1STAbits.URXDA;
        case UART2_CH:
           return U2STAbits.URXDA;
        default:
           return 0;     
    }
}

uint8_t hal_UART_SpaceAvailable(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
           return !U1STAbits.UTXBF;
        case UART2_CH:
           return !U2STAbits.UTXBF;
        default:
           return 0;
    }
}

uint8_t hal_UART_DoneTransmitting(uint8_t channel)
{
    switch(channel)
    {
        case UART1_CH:
            return U1STAbits.TRMT;
        case UART2_CH:
            return U1STAbits.TRMT;
        default:
            return 0;
    }
}

void SetBaud(uint8_t channel, uint32_t baud)
{
    uint8_t brgh;

    switch(channel)
    {
        case UART1_CH:
            brgh = ((FCPU / baud) / 16) - 1;
            U1BRG = brgh;
            break;
        case UART2_CH:
            brgh = ((FCPU / baud) / 16) - 1;
            U2BRG = brgh;
            break;
        default:
            break;
    }
}

void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void)
{
    UART_Rx_Handler(UART1_CH);
    hal_UART_ClearRxIF(UART1_CH);
}

void __attribute__((interrupt, auto_psv)) _U2RXInterrupt(void)
{
    UART_Rx_Handler(UART2_CH);
    hal_UART_ClearRxIF(UART2_CH);
}

void __attribute__((interrupt, auto_psv)) _U1TXInterrupt(void)
{
    UART_Tx_Handler(UART1_CH);
    hal_UART_ClearTxIF(UART1_CH);
}

void __attribute__((interrupt, auto_psv)) _U2TXInterrupt(void)
{
    UART_Tx_Handler(UART2_CH);
    hal_UART_ClearTxIF(UART2_CH);
}
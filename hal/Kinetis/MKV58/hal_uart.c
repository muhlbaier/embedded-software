#include <stdint.h>
#include <stdbool.h>
#include "hal_uart.h"
#include "uart.h"
#include "fsl_uart_driver.h"
#include "project_settings.h"

extern uint32_t SystemCoreClock;
#define BAUD(b) ((SystemCoreClock + 8*b)/(16*b))

void hal_UART_EnableFifoErrorHandling(uint8_t channel);
void hal_UART_DisableFifoErrorHandling(uint8_t channel);

void hal_UART_Init(uint8_t channel, uint32_t baud) {
    if(channel > 2) return;
    //uart_status_t status;
    uart_user_config_t uartConfig;
    uartConfig.baudRate = baud;
    uartConfig.bitCountPerChar = kUart8BitsPerChar;
    uartConfig.parityMode = kUartParityDisabled;
    uartConfig.stopBitCount = kUartOneStopBit;
    uart_state_t uartState;
    /*status = */UART_DRV_Init(channel, &uartState, &uartConfig);
    hal_UART_Enable(channel);
    hal_UART_EnableRxInterrupt(channel);
	// Enable FIFO error handling for supported channels
	hal_UART_EnableFifoErrorHandling(channel); 
}

void hal_UART_Enable(uint8_t channel) {
    switch (channel) {
        case 0: UART_HAL_EnableTransmitter(UART0); UART_HAL_EnableReceiver(UART0); break;
        case 1: UART_HAL_EnableTransmitter(UART1); UART_HAL_EnableReceiver(UART1); break;
        case 2: UART_HAL_EnableTransmitter(UART2); UART_HAL_EnableReceiver(UART2); break;
        default: break;
    }
}

void hal_UART_Disable(uint8_t channel) {
    switch (channel) {
        case 0: UART_HAL_DisableTransmitter(UART0); UART_HAL_DisableReceiver(UART0); break;
        case 1: UART_HAL_DisableTransmitter(UART1); UART_HAL_DisableReceiver(UART1); break;
        case 2: UART_HAL_DisableTransmitter(UART2); UART_HAL_DisableReceiver(UART2); break;
        default: break;
    }
}

void hal_UART_EnableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case 0: UART_HAL_SetIntMode(UART0, kUartIntRxDataRegFull, 1); break;
        case 1: UART_HAL_SetIntMode(UART1, kUartIntRxDataRegFull, 1); break;
        case 2: UART_HAL_SetIntMode(UART2, kUartIntRxDataRegFull, 1); break;
        default: break;
    }
}

void hal_UART_EnableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case 0: UART_HAL_SetIntMode(UART0, kUartIntTxDataRegEmpty, 1); break;
        case 1: UART_HAL_SetIntMode(UART1, kUartIntTxDataRegEmpty, 1); break;
        case 2: UART_HAL_SetIntMode(UART2, kUartIntTxDataRegEmpty, 1); break;
        default: break;
    }
}

void hal_UART_DisableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case 0: UART_HAL_SetIntMode(UART0, kUartIntRxDataRegFull, 0); break;
        case 1: UART_HAL_SetIntMode(UART1, kUartIntRxDataRegFull, 0); break;
        case 2: UART_HAL_SetIntMode(UART2, kUartIntRxDataRegFull, 0); break;
        default: break;
    }
}

void hal_UART_DisableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case 0: UART_HAL_SetIntMode(UART0, kUartIntTxDataRegEmpty, 0); break;
        case 1: UART_HAL_SetIntMode(UART1, kUartIntTxDataRegEmpty, 0); break;
        case 2: UART_HAL_SetIntMode(UART2, kUartIntTxDataRegEmpty, 0); break;
        default: break;
    }
}

void hal_UART_TxByte(uint8_t channel, uint8_t c) {
    switch (channel) {
        case 0: UART_HAL_Putchar(UART0, c); break;
        case 1: UART_HAL_Putchar(UART1, c); break;
        case 2: UART_HAL_Putchar(UART2, c); break;
        default: break;
    }
}

uint8_t hal_UART_RxByte(uint8_t channel) {
    uint8_t data = 0;
    switch (channel) {
        case 0: UART_HAL_Getchar(UART0, &data); break;
        case 1: UART_HAL_Getchar(UART1, &data); break;
        case 2: UART_HAL_Getchar(UART2, &data); break;
        default: break;
    }
    return data;
}

void hal_UART_ClearTxIF(uint8_t channel) {
    // auto cleared in hardware
}

void hal_UART_ClearRxIF(uint8_t channel) {
    // auto cleared in hardware
}

uint8_t hal_UART_DataAvailable(uint8_t channel) {
    uint8_t da = 0;
    switch (channel) {
        case 0: da = !UART_HAL_IsRxFifoEmpty(UART0); break;
        case 1: da = !UART_HAL_IsRxFifoEmpty(UART1); break;
        case 2: da = !UART_HAL_IsRxFifoEmpty(UART2); break;
        default: break;
    }
    return da;
}

uint8_t hal_UART_SpaceAvailable(uint8_t channel) {
    uint8_t sa = 0;
    switch (channel) {
        case 0: sa = (UART0->TCFIFO < 8) ? 1 : 0; break;
        case 1: sa = (UART1->S1 & 0x80) ? 1 : 0; break;
        case 2: sa = (UART2->S1 & 0x80) ? 1 : 0; break;
        default: break;
    }
    return sa;
}

void hal_UART_EnableFifoErrorHandling(uint8_t channel) {
    switch (channel) {
        case 0: 
            UART_HAL_SetIntMode(UART0, kUartIntTxFifoOverflow, 1); 
            UART_HAL_SetIntMode(UART0, kUartIntRxFifoUnderflow, 1); 
            NVIC_EnableIRQ(UART0_ERR_IRQn);
            break;
        case 1: break;	// No FIFO
        case 2: break;	// No FIFO
        default: break;
    }
}

void hal_UART_DisableFifoErrorHandling(uint8_t channel) {
    switch (channel) {
        case 0: 
            UART_HAL_SetIntMode(UART0, kUartIntTxFifoOverflow, 0); 
            UART_HAL_SetIntMode(UART0, kUartIntRxFifoUnderflow, 0); 
            NVIC_DisableIRQ(UART0_ERR_IRQn);
            break;
        case 1: break;	// No FIFO
        case 2: break;	// No FIFO
        default: break;
    }
}

#ifdef USE_UART0
#ifndef OVERRIDE_UART0_INTERRUPT
void UART0_RX_TX_IRQHandler(void)
#else
void Overridden_UART0_RX_TX_IRQHandler(void)
#endif
{
    UART_Rx_Handler(0);
    UART_Tx_Handler(0);
}

void UART0_ERR_IRQHandler(){
    // Handle TX FIFO Overflow
    if(UART_RD_CFIFO_TXOFE(UART0)){
        // Need to flush TX FIFO to realign pointer, recommended to disable transmitter before flush
        // IMPORTANT: Possible data loss but will realign the FIFO pointer
        UART_HAL_DisableTransmitter(UART0);
        UART_HAL_FlushTxFifo(UART0);
        UART_WR_CFIFO_TXOFE(UART0, 0);
        UART_HAL_EnableTransmitter(UART0);
    }
    // Handle RX FIFO Underflow
    if(UART_RD_CFIFO_RXUFE(UART0)){
        // Need to flush RX FIFO to realign pointer, recommended to disable receiver before flush
        // IMPORTANT: Possible data loss but will realign the FIFO pointer
        UART_HAL_DisableReceiver(UART0);
        UART_HAL_FlushRxFifo(UART0);
        UART_WR_CFIFO_RXOFE(UART0, 0);
        UART_HAL_EnableReceiver(UART0);
    }
}
#endif
#ifdef USE_UART1
void UART1_RX_TX_IRQHandler(void) {
    UART_Rx_Handler(1);
    UART_Tx_Handler(1);
}
#endif
#ifdef USE_UART2
void UART2_RX_TX_IRQHandler(void) {
    UART_Rx_Handler(2);
    UART_Tx_Handler(2);
}
#endif

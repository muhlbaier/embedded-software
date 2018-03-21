#include <stdint.h>
#include "hal_general.h"
#incldue "uart.h"
#include "project_settings.h"
#include "macros.h"

#define UART1_CH USART_ID_1
#define UART2_CH USART_ID_2
#define UART3_CH USART_ID_3
#define UART4_CH USART_ID_4
#define UART5_CH USART_ID_5
#define UART6_CH USART_ID_6

#define Set_U1Tx_PPS(pin) PLIB_PORTS_RemapOutput(PORTS_ID_0, OUTPUT_FUNC_U1TX, CAT2(OUTPUT_PIN_, pin))
#define Set_U2Tx_PPS(pin) PLIB_PORTS_RemapOutput(PORTS_ID_0, OUTPUT_FUNC_U2TX, CAT2(OUTPUT_PIN_, pin))
#define Set_U3Tx_PPS(pin) PLIB_PORTS_RemapOutput(PORTS_ID_0, OUTPUT_FUNC_U2TX, CAT2(OUTPUT_PIN_, pin))
#define Set_U4Tx_PPS(pin) PLIB_PORTS_RemapOutput(PORTS_ID_0, OUTPUT_FUNC_U2TX, CAT2(OUTPUT_PIN_, pin))
#define Set_U5Tx_PPS(pin) PLIB_PORTS_RemapOutput(PORTS_ID_0, OUTPUT_FUNC_U2TX, CAT2(OUTPUT_PIN_, pin))
#define Set_U6Tx_PPS(pin) PLIB_PORTS_RemapOutput(PORTS_ID_0, OUTPUT_FUNC_U2TX, CAT2(OUTPUT_PIN_, pin))
#define Set_U1Rx_PPS(pin) PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_U1RX, CAT2(INPUT_PIN_, pin))
#define Set_U2Rx_PPS(pin) PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_U2RX, CAT2(INPUT_PIN_, pin))
#define Set_U3Rx_PPS(pin) PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_U3RX, CAT2(INPUT_PIN_, pin))
#define Set_U4Rx_PPS(pin) PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_U4RX, CAT2(INPUT_PIN_, pin))
#define Set_U5Rx_PPS(pin) PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_U5RX, CAT2(INPUT_PIN_, pin))
#define Set_U6Rx_PPS(pin) PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_U6RX, CAT2(INPUT_PIN_, pin))

// for uart channels used UARTx_TX_PIN and UARTx_RX_PIN must be set in project_settings.h

#ifndef UART_INTERRUPT_PRIORITY
#define UART_INTERRUPT_PRIORITY 3
#endif

#ifndef PERIPHERAL_CLOCK
#define PERIPHERAL_CLOCK FCPU
#endif

#ifndef UART_PERIPHERAL_CLOCK
#define UART_PERIPHERAL_CLOCK PERIPHERAL_CLOCK
#endif

// private function, could be changed to public if needed
static void SetBaud(uint8_t n, uint32_t baud);

void hal_UART_Init(uint8_t channel, uint32_t baud) {
    switch (channel) {
#ifdef USE_UART1
        case UART1_CH:
            Set_U1Tx_PPS(UART1_TX_PIN);       // UART1_TX_PIN should be defined in project_settings.h
                                              // e.g. #define UART1_TX_PIN PRB10
            Set_U1Rx_PPS(UART1_RX_PIN);       // UART1_RX_PIN should be defined in project_settings.h
            PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_UART1_RX, UART_INTERRUPT_PRIORITY);
            PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_UART1_TX, UART_INTERRUPT_PRIORITY);
            break;
#endif
#ifdef USE_UART2
        case UART2_CH:
            Set_U2Tx_PPS(UART2_TX_PIN);       // UART2_TX_PIN should be defined in project_settings.h
            Set_U2Rx_PPS(UART2_RX_PIN);       // UART2_RX_PIN should be defined in project_settings.h
            PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_UART2_RX, UART_INTERRUPT_PRIORITY);
            PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_UART2_TX, UART_INTERRUPT_PRIORITY);
            break;
#endif
#ifdef USE_UART3
        case UART3_CH:
            Set_U3Tx_PPS(UART3_TX_PIN);       // UART3_TX_PIN should be defined in project_settings.h
            Set_U3Rx_PPS(UART3_RX_PIN);       // UART3_RX_PIN should be defined in project_settings.h
            PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_UART3_RX, UART_INTERRUPT_PRIORITY);
            PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_UART3_TX, UART_INTERRUPT_PRIORITY);
            break;
#endif
#ifdef USE_UART4
        case UART4_CH:
            Set_U4Tx_PPS(UART4_TX_PIN);       // UART4_TX_PIN should be defined in project_settings.h
            Set_U4Rx_PPS(UART4_RX_PIN);       // UART4_RX_PIN should be defined in project_settings.h
            PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_UART4_RX, UART_INTERRUPT_PRIORITY);
            PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_UART4_TX, UART_INTERRUPT_PRIORITY);
            break;
#endif
#ifdef USE_UART5
        case UART5_CH:
            Set_U5Tx_PPS(UART5_TX_PIN);       // UART5_TX_PIN should be defined in project_settings.h
            Set_U5Rx_PPS(UART5_RX_PIN);       // UART5_RX_PIN should be defined in project_settings.h
            PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_UART5_RX, UART_INTERRUPT_PRIORITY);
            PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_UART5_TX, UART_INTERRUPT_PRIORITY);
            break;
#endif
#ifdef USE_UART6
        case UART6_CH:
            Set_U6Tx_PPS(UART6_TX_PIN);       // UART6_TX_PIN should be defined in project_settings.h
            Set_U6Rx_PPS(UART6_RX_PIN);       // UART6_RX_PIN should be defined in project_settings.h
            PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_UART6_RX, UART_INTERRUPT_PRIORITY);
            PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_UART6_TX, UART_INTERRUPT_PRIORITY);
            break;
#endif
    }
    hal_UART_Disable(channel);
    PLIB_USART_LineControlModeSelect(channel, USART_8N1);
    PLIB_USART_TransmitterInterruptModeSelect(channel, USART_TRANSMIT_FIFO_EMPTY);
    PLIB_USART_ReceiverInterruptModeSelect(channel, USART_RECEIVE_FIFO_ONE_CHAR);
    SetBaud(channel, baud);
    PLIB_USART_TransmitterEnable(channel);
    PLIB_USART_ReceiverEnable(channel);
    hal_UART_ClearRxIF(channel);
    hal_UART_EnableRxInterrupt(channel);
    hal_UART_Enable(channel);
}

void hal_UART_Enable(uint8_t channel) {
    PLIB_USART_Enable(channel);
}

void hal_UART_Disable(uint8_t channel) {
    PLIB_USART_Disable(channel);
}

void hal_UART_EnableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH: PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_RECEIVE); break;
        case UART2_CH: PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_2_RECEIVE); break;
        case UART3_CH: PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_3_RECEIVE); break;
        case UART4_CH: PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_4_RECEIVE); break;
        case UART5_CH: PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_5_RECEIVE); break;
        case UART6_CH: PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_6_RECEIVE); break;
        default: break;
    }
}

void hal_UART_EnableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH: PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT); break;
        case UART2_CH: PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_2_TRANSMIT); break;
        case UART3_CH: PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_3_TRANSMIT); break;
        case UART4_CH: PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_4_TRANSMIT); break;
        case UART5_CH: PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_5_TRANSMIT); break;
        case UART6_CH: PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_6_TRANSMIT); break;
        default: break;
    }
}

void hal_UART_DisableRxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH: PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_RECEIVE); break;
        case UART2_CH: PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_2_RECEIVE); break;
        case UART3_CH: PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_3_RECEIVE); break;
        case UART4_CH: PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_4_RECEIVE); break;
        case UART5_CH: PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_5_RECEIVE); break;
        case UART6_CH: PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_6_RECEIVE); break;
        default: break;
    }
}

void hal_UART_DisableTxInterrupt(uint8_t channel) {
    switch (channel) {
        case UART1_CH: PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT); break;
        case UART2_CH: PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_2_TRANSMIT); break;
        case UART3_CH: PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_3_TRANSMIT); break;
        case UART4_CH: PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_4_TRANSMIT); break;
        case UART5_CH: PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_5_TRANSMIT); break;
        case UART6_CH: PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_6_TRANSMIT); break;
        default: break;
    }
}

void hal_UART_TxByte(uint8_t channel, uint8_t c) {
    PLIB_USART_TransmitterByteSend(channel, c);
}

uint8_t hal_UART_RxByte(uint8_t channel) {
    uint8_t my_byte;
    my_byte = PLIB_USART_ReceiverByteReceive(channel);
    if(PLIB_USART_ReceiverOverrunHasOccurred(channel))
    {
        PLIB_USART_ReceiverOverrunErrorClear(channel);
    }
    return my_byte;
}

void hal_UART_ClearTxIF(uint8_t channel) {
    switch (channel) {
        case UART1_CH: PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT); break;
        case UART2_CH: PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_2_TRANSMIT); break;
        case UART3_CH: PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_3_TRANSMIT); break;
        case UART4_CH: PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_4_TRANSMIT); break;
        case UART5_CH: PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_5_TRANSMIT); break;
        case UART6_CH: PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_6_TRANSMIT); break;
        default: break;
    }
}

void hal_UART_ClearRxIF(uint8_t channel) {
    switch (channel) {
        case UART1_CH: PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE); break;
        case UART2_CH: PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE); break;
        case UART3_CH: PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE); break;
        case UART4_CH: PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE); break;
        case UART5_CH: PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE); break;
        case UART6_CH: PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE); break;
        default: break;
    }
}

uint8_t hal_UART_DataAvailable(uint8_t channel) {
    PLIB_USART_ReceiverDataIsAvailable(channel);
}

uint8_t hal_UART_SpaceAvailable(uint8_t channel) {
    return !PLIB_USART_TransmitterBufferIsFull(channel);
}

void SetBaud(uint8_t n, uint32_t baud) {
    int16_t error;
    uint32_t baud1;
    
    PLIB_USART_BaudRateSet(n, UART_PERIPHERAL_CLOCK, baud);
    baud1 = PLIB_USART_BaudRateGet(n, UART_PERIPHERAL_CLOCK);
    if (baud1 > baud) error = baud1 - baud;
    else error = baud - baud1;

    if (error * 1000 / baud > 25) {
        PLIB_USART_BaudRateHighSet(n, UART_PERIPHERAL_CLOCK, baud);
        PLIB_USART_BaudRateHighEnable(n);
    } else {
        PLIB_USART_BaudRateHighDisable(n);
    }
}

uint8_t hal_UART_RxInterruptEnabled(uint8_t channel) {
    #warning "not implemented"
}

uint8_t hal_UART_TxInterruptEnabled(uint8_t channel) {
    #warning "not implemented"
}

#ifdef USE_UART1
void __attribute__((vector(_UART1_RX_VECTOR), interrupt(), nomips16)) UART_1_RX_Interrrupt(void) {
    UART_Rx_Handler(UART1_CH);
    hal_UART_ClearRxIF(UART1_CH);
}
void __attribute__((vector(_UART1_TX_VECTOR), interrupt(), nomips16)) UART_1_TX_Interrrupt(void) {
    UART_Tx_Handler(UART1_CH);
}
#endif
#ifdef USE_UART2
void __attribute__((vector(_UART2_RX_VECTOR), interrupt(), nomips16)) UART_2_RX_Interrrupt(void) {
    UART_Rx_Handler(UART2_CH);
    hal_UART_ClearRxIF(UART2_CH);
}
void __attribute__((vector(_UART2_TX_VECTOR), interrupt(), nomips16)) UART_2_TX_Interrrupt(void) {
    UART_Tx_Handler(UART2_CH);
}
#endif
#ifdef USE_UART3
void __attribute__((vector(_UART3_RX_VECTOR), interrupt(), nomips16)) UART_3_RX_Interrrupt(void) {
    UART_Rx_Handler(UART3_CH);
    hal_UART_ClearRxIF(UART3_CH);
}
void __attribute__((vector(_UART3_TX_VECTOR), interrupt(), nomips16)) UART_3_TX_Interrrupt(void) {
    UART_Tx_Handler(UART3_CH);
}
#endif
#ifdef USE_UART4
void __attribute__((vector(_UART4_RX_VECTOR), interrupt(), nomips16)) UART_4_RX_Interrrupt(void) {
    UART_Rx_Handler(UART4_CH);
    hal_UART_ClearRxIF(UART4_CH);
}
void __attribute__((vector(_UART4_TX_VECTOR), interrupt(), nomips16)) UART_4_TX_Interrrupt(void) {
    UART_Tx_Handler(UART4_CH);
}
#endif
#ifdef USE_UART5
void __attribute__((vector(_UART5_RX_VECTOR), interrupt(), nomips16)) UART_5_RX_Interrrupt(void) {
    UART_Rx_Handler(UART5_CH);
    hal_UART_ClearRxIF(UART5_CH);
}
void __attribute__((vector(_UART5_TX_VECTOR), interrupt(), nomips16)) UART_5_TX_Interrrupt(void) {
    UART_Tx_Handler(UART5_CH);
}
#endif
#ifdef USE_UART6
void __attribute__((vector(_UART6_RX_VECTOR), interrupt(), nomips16)) UART_6_RX_Interrrupt(void) {
    UART_Rx_Handler(UART6_CH);
    hal_UART_ClearRxIF(UART6_CH);
}
void __attribute__((vector(_UART6_TX_VECTOR), interrupt(), nomips16)) UART_6_TX_Interrrupt(void) {
    UART_Tx_Handler(UART6_CH);
}
#endif


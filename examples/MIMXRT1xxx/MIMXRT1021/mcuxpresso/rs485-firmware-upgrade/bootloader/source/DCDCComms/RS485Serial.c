/**
*	@file RS485Serial.c
*	@brief
*	@date
*	@author	James Ethridge
*   @author Michael Muhlbaier
*
*   @todo refactor this to use fsl_uart_edma
* 
* Copyright (c) 2021, Alencon Systems
* All rights reserved.
*
*/
// external dependencies
//txSize = EDMA_DRV_GetUnfinishedBytes(&uartEdmaState->edmaUartTx);
//UART_HAL_SetIntMode(RS485_UART_BASE_PTR, kUartIntTxComplete, false);
//EDMA_DRV_RequestChannel(RS485_RX_DMA_CHAN, uartRxEdmaRequest, &uartEdmaStatePtr->edmaUartRx);
//EDMA_DRV_InstallCallback(&uartEdmaStatePtr->edmaUartRx, UART_DRV_EdmaRxCallback, (void *)instance);

/* ***** Dependencies ***** */

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#if !(defined GARD_BOARD || defined CUBE_BOARD || defined AFD_BOARD)
#include "hardware.h"
#endif
#ifdef GARD_BOARD
#include "gard_board.h"
#else
#include "board.h"
#endif
#include "RS485Serial.h"
#include "mcuid.h"
#include "uart.h"
 
//#include "fsl_uart_hal.h"
//#include "fsl_edma_driver.h"
//#include "fsl_uart_irq.h"
//#include "hal_fsl_clock.h"

//#include "fsl_os_abstraction.h"


/*!
 * @brief User configuration structure for the UART driver.
 *
 * Use an instance of this structure with the UART_DRV_Init()function. This enables configuration of the
 * most common settings of the UART peripheral with a single function call. Settings include:
 * UART baud rate, UART parity mode: disabled (default), or even or odd, the number of stop bits, and
 * the number of bits per data word.
 */
//typedef struct UartEdmaUserConfig {
    //uint32_t baudRate;            /*!< UART baud rate*/
    //uart_parity_mode_t parityMode;      /*!< parity mode, disabled (default), even, odd */
    //uart_stop_bit_count_t stopBitCount; /*!< number of stop bits, 1 stop bit (default) or 2 stop bits */
    //uart_bit_count_per_char_t bitCountPerChar; /*!< number of bits, 8-bit (default) or 9-bit in
                                                    //a word (up to 10-bits in some UART instances) */
//} uart_edma_user_config_t;

///* Forward declaration of the handle typedef. */
//typedef struct _uart_edma_handle uart_edma_handle_t;

///*!
 //* @brief Runtime state structure for UART driver with EDMA.
 //*/
//typedef struct UartEdmaState {
    //volatile bool isTxBusy;     /*!< True if there is an active transmit. */
    //volatile bool isRxBusy;     /*!< True if there is an active receive. */
    //volatile bool isTxBlocking; /*!< True if transmit is blocking transaction. */
    //volatile bool isRxBlocking; /*!< True if receive is blocking transaction. */
    //semaphore_t txIrqSync;      /*!< Used to wait for ISR to complete its TX business. */
    //semaphore_t rxIrqSync;      /*!< Used to wait for ISR to complete its RX business. */
    //edma_chn_state_t edmaUartTx; /*!< Structure definition for the EDMA channel */
    //edma_chn_state_t edmaUartRx; /*!< Structure definition for the EDMA channel */
//} uart_edma_state_t;

//// end copied from KSDK fsl_uart_edma_driver.h

#include "timing.h" // required for initialization sync

#define BUFFER_SIZE 136*2

/** PackageName
*	@addtogroup PackageName
*	@{
*/

/** RS485Serial
*	@addtogroup RS485Serial
*	@{
*/

/** RS485Serial_Private_Macros
*	@{
*/

/** RS485Serial_Private_Macros
*	@}
*/

/** RS485Serial_Private_Types
*	@{
*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
 
/*!
 * @brief Runtime state structure for UART driver with EDMA.
 */
 /// @todo refactor this to be part of a structure that gets passed around so multiple instances of this module could potentially be used
//static uint8_t buffer[BUFFER_SIZE];
//static uint32_t last_bytes_remaining;



///** RS485Serial_Private_Types
//*	@}
//*/
////

//static uart_edma_state_t* _uartStatePtr;
static uint32_t RS485_UART_INSTANCE;
//static uart_edma_state_t           uartStateEdma;
static void(*_rs485_tx_complete_callback)(void);
//static UART_Type * RS485_UART_BASE_PTR;
//static IRQn_Type RS485_UART_IRQ_HANDLER;

static void InitInstancePointers(void) {
    static uint8_t initialized = 0;
    if(initialized == 0) {
        #if defined GARD_BOARD || defined CUBE_BOARD || defined AFD_BOARD
        RS485_UART_INSTANCE = BOARD_RS485_UART_INSTANCE;
        #else
        //if(Hardware_HasMKV5Pinout()) {
            //RS485_UART_INSTANCE = BOARD_ALT_RS485_UART_INSTANCE;
        //}else {
            RS485_UART_INSTANCE = BOARD_RS485_UART_INSTANCE;
        //}
        #endif
    }
    //if(RS485_UART_INSTANCE == 0) {
        //RS485_UART_BASE_PTR = UART0;
        //RS485_UART_IRQ_HANDLER = UART0_RX_TX_IRQn;
    //}else { // assume UART1
        //RS485_UART_BASE_PTR = UART1;
        //RS485_UART_IRQ_HANDLER = UART1_RX_TX_IRQn;
    //}
}

//////static uart_edma_handle_t hard_edma_handle;
//typedef struct UartUserConfig {
    //uint32_t baudRate;            /*!< UART baud rate @internal gui name="Baud rate" id="BaudRate" */
    //uart_parity_mode_t parityMode;      /*!< parity mode, disabled (default), even, odd @internal gui name="Parity mode" id="Parity" */
    //uart_stop_bit_count_t stopBitCount; /*!< number of stop bits, 1 stop bit (default) or 2 stop bits @internal gui name="Stop bits" id="StopBits" */
    //uart_bit_count_per_char_t bitCountPerChar; /*!< number of bits, 8-bit (default) or 9-bit in
                                                    //a word (up to 10-bits in some UART instances) @internal gui name="Bits per char" id="DataBits" */
//} uart_user_config_t;

//// Config the UART driver
//static uart_user_config_t uartConfig = {
        //.bitCountPerChar = kUart8BitsPerChar,
        //.parityMode      = kUartParityDisabled,
        //.stopBitCount    = kUartOneStopBit,
        //.baudRate        = 115200
			//};

///** RS485Serial_Private_Variables
//*	@{
//*/


//static void UART_DRV_EdmaCompleteSendData(uint32_t instance);
//static void UART_DRV_EdmaTxCallback(void *param, edma_chn_status_t status);
//static uart_status_t UART_DRV_EdmaStartSendData(uint32_t instance,
                                               //const uint8_t * txBuff,
                                               //uint32_t txSize);
//static void UART_DRV_EdmaCompleteReceiveData(uint32_t instance);
//static void UART_DRV_EdmaRxCallback(void *param, edma_chn_status_t status);
//static uart_status_t UART_DRV_EdmaStartReceiveData(uint32_t instance,
                                                  //uint8_t * rxBuff,
                                                  //uint32_t rxSize);
///** RS485Serial_Private_Variables
//*	@}
//*/
///*!
 //* @brief Initializes an UART instance to work with EDMA.
 //*
 //* This function initializes the run-time state structure to keep track of the on-going
 //* transfers, un-gates the clock to the UART module, initializes the module
 //* to user-defined settings and default settings, configures the IRQ state structure and enables
 //* the module-level interrupt to the core, and enables the UART module transmitter and receiver.
 //* This example shows how to set up the uart_edma_state_t and the
 //* uart_user_config_t parameters and how to call the UART_DRV_EdmaInit function by passing
 //* in these parameters:
   //@code
    //uart_user_config_t uartConfig;
    //uartConfig.baudRate = 9600;
    //uartConfig.bitCountPerChar = kUart8BitsPerChar;
    //uartConfig.parityMode = kUartParityDisabled;
    //uartConfig.stopBitCount = kUartOneStopBit;
    //uart_edma_state_t uartEdmaState;
    //UART_DRV_EdmaInit(instance, &uartEdmaState, &uartConfig);
   //@endcode
 //*
 //* @param instance The UART instance number.
 //* @param uartEdmaStatePtr A pointer to the UART driver state structure memory. The user
 //*  passes in the memory for the run-time state structure. The UART driver
 //*  populates the members. This run-time state structure keeps track of the
 //*  current transfer in progress.
 //* @param uartUserConfig The user configuration structure of type uart_user_config_t. The user
 //*  populates the members of this structure and passes the pointer of this structure
 //*  to this function.
 //* @return An error code or kStatus_UART_Success.
 //*/
//uart_status_t UART_DRV_EdmaInit(uint32_t instance, uart_edma_state_t * uartEdmaStatePtr,
                               //const uart_edma_user_config_t * uartUserConfig);
///*!
 //* @brief Shuts down the UART.
 //*
 //* This function disables the UART-EDMA trigger and disables the transmitter and receiver.
 //*
 //* @param instance The UART instance number.
 //* @return An error code or kStatus_UART_Success.
 //*/
//uart_status_t UART_DRV_EdmaDeinit(uint32_t instance);

///*!
 //* @brief Sends (transmits) data out through the UART-EDMA module using a blocking method.
 //*
 //* @param instance The UART instance number.
 //* @param txBuff A pointer to the source buffer containing 8-bit data chars to send.
 //* @param txSize The number of bytes to send.
 //* @param timeout A timeout value for RTOS abstraction sync control in milliseconds (ms).
 //* @return An error code or kStatus_UART_Success.
 //*/
//uart_status_t UART_DRV_EdmaSendDataBlocking(uint32_t instance,
                                        //const uint8_t * txBuff,
                                        //uint32_t txSize,
                                        //uint32_t timeout);

///*!
 //* @brief Sends (transmits) data through the UART-EDMA module using a non-blocking method.
 //*
 //* @param instance The UART module base address.
 //* @param txBuff A pointer to the source buffer containing 8-bit data chars to send.
 //* @param txSize The number of bytes to send.
 //* @return An error code or kStatus_UART_Success.
 //*/
//uart_status_t UART_DRV_EdmaSendData(uint32_t instance,
                                //const uint8_t * txBuff,
                                //uint32_t txSize);
///*!
 //* @brief Returns whether the previous UART-EDMA transmit has finished.
 //*
 //* @param instance The UART module base address.
 //* @param bytesRemaining A pointer to a value that is populated with the number of bytes that
 //*                       are remaining in the active transfer.
 //* @return The transmit status.
 //* @retval kStatus_UART_Success The transmit has completed successfully.
 //* @retval kStatus_UART_TxBusy The transmit is still in progress. @a bytesTransmitted is
 //*     filled with the number of bytes which are transmitted up to that point.
 //*/
//uart_status_t UART_DRV_EdmaGetTransmitStatus(uint32_t instance, uint32_t * bytesRemaining);

///*!
 //* @brief Terminates a non-blocking UART-EDMA transmission early.
 //*
 //* @param instance The UART module base address.
 //* @return Whether the aborting success or not.
 //* @retval kStatus_UART_Success The transmit was successful.
 //* @retval kStatus_UART_NoTransmitInProgress No transmission is currently in progress.
 //*/
//uart_status_t UART_DRV_EdmaAbortSendingData(uint32_t instance);

///*!
 //* @brief Gets (receives) data from the UART-EDMA module using a blocking method.
 //*
 //* @param instance The UART module base address.
 //* @param rxBuff A pointer to the buffer containing 8-bit read data chars received.
 //* @param rxSize The number of bytes to receive.
 //* @param timeout A timeout value for RTOS abstraction sync control in milliseconds (ms).
 //* @return An error code or kStatus_UART_Success.
 //*/
//uart_status_t UART_DRV_EdmaReceiveDataBlocking(uint32_t instance, uint8_t * rxBuff,
                                              //uint32_t rxSize, uint32_t timeout);
///*!
 //* @brief Gets (receives) data from the UART-EDMA module using a non-blocking method.
 //*
 //* @param instance The UART module base address.
 //* @param rxBuff  A pointer to the buffer containing 8-bit read data chars received.
 //* @param rxSize The number of bytes to receive.
 //* @return An error code or kStatus_UART_Success.
 //*/
//uart_status_t UART_DRV_EdmaReceiveData(uint32_t instance, uint8_t * rxBuff, uint32_t rxSize);

///*!
 //* @brief Returns whether the previous UART-EDMA receive is complete.
 //*
 //* @param instance The UART module base address.
 //* @param bytesRemaining A pointer to a value that is populated with the number of bytes which
 //*                       still need to be received in the active transfer.
 //* @return The receive status.
 //* @retval kStatus_UART_Success The receive has completed successfully.
 //* @retval kStatus_UART_RxBusy The receive is still in progress. @a bytesReceived is
 //*     filled with the number of bytes which are received up to that point.
 //*/
//uart_status_t UART_DRV_EdmaGetReceiveStatus(uint32_t instance, uint32_t * bytesRemaining);

///*!
 //* @brief Terminates a non-blocking UART-EDMA receive early.
 //*
 //* @param instance The UART module base address.
 //* @return Whether the aborting success or not.
 //* @retval kStatus_UART_Success The receive was successful.
 //* @retval kStatus_UART_NoTransmitInProgress No receive is currently in progress.
 //*/
//uart_status_t UART_DRV_EdmaAbortReceivingData(uint32_t instance);


///** RS485Serial_Private_Functions
//*	@{
//*/

///*-----Prototypes-----*/
//static void uart_irq_callback(void);
///*------Private Function Definitions-----*/

//void uart_irq_callback(void){
    ///* Handle tx complete / transmitter idle interrupt */
    //uart_status_t uartEdmaStatus;
    //uint32_t bytesRemaining;

    //uartEdmaStatus= UART_DRV_EdmaGetTransmitStatus(RS485_UART_INSTANCE,&bytesRemaining);

    //if( UART_RD_C2_TCIE(RS485_UART_BASE_PTR) //if the transmit complete interrupt is enabled
        //&&UART_RD_S1_TC(RS485_UART_BASE_PTR) //and the transmit complete flag is high
        //&&(uartEdmaStatus==kStatus_UART_Success) //all data transmitted successfully
        //&&(bytesRemaining==0)//no data remaining in txbuffer
    //)
    //{
        //RS485DE_OFF;

        ///* Disable the transmit complete interrupt */
        //UART_HAL_SetIntMode(RS485_UART_BASE_PTR, kUartIntTxComplete, false);
        //if(_rs485_tx_complete_callback) {
            //_rs485_tx_complete_callback();
        //}
    //}
//}

///** RS485Serial_Private_Functions
//*	@}
//*/
///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaInit
 //* Description   : This function initializes a UART instance for operation.
 //* This function will initialize the run-time state structure to keep track of
 //* the on-going transfers, ungate the clock to the UART module, initialize the
 //* module to user defined settings and default settings, configure UART DMA
 //* and enable the UART module transmitter and receiver.
 //* The following is an example of how to set up the uart_edma_state_t and the
 //* uart_user_config_t parameters and how to call the UART_DRV_EdmaInit function
 //* by passing in these parameters:
 //*    uart_user_config_t uartConfig;
 //*    uartConfig.baudRate = 9600;
 //*    uartConfig.bitCountPerChar = kUart8BitsPerChar;
 //*    uartConfig.parityMode = kUartParityDisabled;
 //*    uartConfig.stopBitCount = kUartOneStopBit;
 //*    uart_edma_state_t uartEdmaState;
 //*    UART_DRV_EdmaInit(instance, &uartEdmaState, &uartConfig);
 //*
 //*END**************************************************************************/
//uart_status_t UART_DRV_EdmaInit(uint32_t instance,
                                //uart_edma_state_t * uartEdmaStatePtr,
                                //const uart_edma_user_config_t * uartUserConfig)
//{
    //assert(uartEdmaStatePtr && uartUserConfig);

    ////assert(instance < UART_INSTANCE_COUNT);
    ///* This driver only support UART instances with separate DMA channels for
     //* both Tx and Rx.*/
    //assert(FSL_FEATURE_UART_HAS_SEPARATE_DMA_RX_TX_REQn(instance) == 1);
    //InitInstancePointers();
    //UART_Type * base = RS485_UART_BASE_PTR;
    //uint32_t uartSourceClock = 0;
    //dma_request_source_t uartTxEdmaRequest = kDmaRequestMux0Disable;
    //dma_request_source_t uartRxEdmaRequest = kDmaRequestMux0Disable;

    ///* Exit if current instance is already initialized. */
    //if (_uartStatePtr)
    //{
        //return kStatus_UART_Initialized;
    //}

    ///* Clear the state structure for this instance. */
    //memset(uartEdmaStatePtr, 0, sizeof(uart_edma_state_t));

    ///* Save runtime structure pointer.*/
    //_uartStatePtr = uartEdmaStatePtr;

    ///* Un-gate UART module clock */
    //HAL_CLOCK_UartClockEnable(instance);
    ///* Initialize UART to a known state. */
    //UART_HAL_Init(base);

    ///* Create Semaphore for txIrq and rxIrq. */
    //OSA_SemaCreate(&uartEdmaStatePtr->txIrqSync, 0);
    //OSA_SemaCreate(&uartEdmaStatePtr->rxIrqSync, 0);

    ///* UART clock source is either system or bus clock depending on instance */ 
    //uartSourceClock = HAL_CLOCK_UartClockGetFreq(instance);
    
    ///* Initialize UART baud rate, bit count, parity and stop bit. */
    //UART_HAL_SetBaudRate(base, uartSourceClock, uartUserConfig->baudRate);
    //UART_WR_C1_M(base, uartUserConfig->bitCountPerChar); // was UART_HAL_SetBitCountPerChar
    //UART_HAL_SetParityMode(base, uartUserConfig->parityMode);

//#if FSL_FEATURE_UART_HAS_STOP_BIT_CONFIG_SUPPORT
    //UART_WR_C1_M(base, uartUserConfig->stopBitCount); // was UART_HAL_SetStopBitCount
//#endif

//#if defined CPU_MKV58F1M0VLL24 || defined CPU_MKV56F512VLL24
//#define UART2_RX_DMA_REQUEST kDmaRequestMux0Group1UART2Rx
//#define UART2_TX_DMA_REQUEST kDmaRequestMux0Group1UART2Tx
//#define UART3_RX_DMA_REQUEST kDmaRequestMux0Group1UART3Rx
//#define UART3_TX_DMA_REQUEST kDmaRequestMux0Group1UART3Tx
//#define UART4_RX_DMA_REQUEST kDmaRequestMux0Group1UART4Rx
//#define UART4_TX_DMA_REQUEST kDmaRequestMux0Group1UART4Tx
//#define UART5_RX_DMA_REQUEST kDmaRequestMux0Group1UART5Rx
//#define UART5_TX_DMA_REQUEST kDmaRequestMux0Group1UART5Tx
//#elif defined CPU_MKV31F256VLL12 || defined CPU_MKV31F256VLH12
//#define UART2_RX_DMA_REQUEST kDmaRequestMux0UART2Rx
//#define UART2_TX_DMA_REQUEST kDmaRequestMux0UART2Tx
//#define UART3_RX_DMA_REQUEST kDmaRequestMux0UART3Rx
//#define UART3_TX_DMA_REQUEST kDmaRequestMux0UART3Tx
//#define UART4_RX_DMA_REQUEST kDmaRequestMux0UART4Rx
//#define UART4_TX_DMA_REQUEST kDmaRequestMux0UART4Tx
//#define UART5_RX_DMA_REQUEST kDmaRequestMux0UART5Rx
//#define UART5_TX_DMA_REQUEST kDmaRequestMux0UART5Tx
//#else
//#error "cpu"
//#endif
    //switch (instance)
    //{
//#if (FSL_FEATURE_UART_HAS_SEPARATE_DMA_RX_TX_REQn(0) == 1)
        //case 0:
            //uartRxEdmaRequest = kDmaRequestMux0UART0Rx;
            //uartTxEdmaRequest = kDmaRequestMux0UART0Tx;
            //break;
//#endif
//#if (FSL_FEATURE_UART_HAS_SEPARATE_DMA_RX_TX_REQn(1) == 1)
        //case 1:
            //uartRxEdmaRequest = kDmaRequestMux0UART1Rx;
            //uartTxEdmaRequest = kDmaRequestMux0UART1Tx;
            //break;
//#endif
//#if (FSL_FEATURE_UART_HAS_SEPARATE_DMA_RX_TX_REQn(2) == 1)
        //case 2:
            //uartRxEdmaRequest = UART2_RX_DMA_REQUEST;
            //uartTxEdmaRequest = UART2_TX_DMA_REQUEST;
            //break;
//#endif
//#if (FSL_FEATURE_UART_HAS_SEPARATE_DMA_RX_TX_REQn(3) == 1)
        //case 3:
            //uartRxEdmaRequest = UART3_RX_DMA_REQUEST;
            //uartTxEdmaRequest = UART3_TX_DMA_REQUEST;
            //break;
//#endif
//#if (FSL_FEATURE_UART_HAS_SEPARATE_DMA_RX_TX_REQn(4) == 1)
        //case 4:
            //uartRxEdmaRequest = UART4_RX_DMA_REQUEST;
            //uartTxEdmaRequest = UART4_TX_DMA_REQUEST;
            //break;
//#endif
//#if (FSL_FEATURE_UART_HAS_SEPARATE_DMA_RX_TX_REQn(5) == 1)
        //case 5:
            //uartRxEdmaRequest = UART5_RX_DMA_REQUEST;
            //uartTxEdmaRequest = UART5_TX_DMA_REQUEST;
            //break;
//#endif
        //default :
            //break;
    //}

    ///*--------------- Setup RX ------------------*/
    ///* Request DMA channels for RX FIFO. */
		////chan used to be kEDMAAnyChannel
    //EDMA_DRV_RequestChannel(RS485_RX_DMA_CHAN, uartRxEdmaRequest,
                            //&uartEdmaStatePtr->edmaUartRx);
    //EDMA_DRV_InstallCallback(&uartEdmaStatePtr->edmaUartRx,
                    //UART_DRV_EdmaRxCallback, (void *)instance);

    ///*--------------- Setup TX ------------------*/
    ///* Request DMA channels for TX FIFO. */
    //EDMA_DRV_RequestChannel(RS485_TX_DMA_CHAN, uartTxEdmaRequest,
                            //&uartEdmaStatePtr->edmaUartTx);
    //EDMA_DRV_InstallCallback(&uartEdmaStatePtr->edmaUartTx,
                    //UART_DRV_EdmaTxCallback, (void *)instance);

        //// setup receive DMA
    //last_bytes_remaining = BUFFER_SIZE;
    //UART_DRV_EdmaStartReceiveData(RS485_UART_INSTANCE, buffer, BUFFER_SIZE);
    
    ///* Finally, enable the UART transmitter and receiver.
     //* Enable DMA trigger when transmit data register empty,
     //* and receive data register full. */
    //UART_HAL_SetTxDmaCmd(base, true);
    //UART_HAL_SetRxDmaCmd(base, true);
    //UART_HAL_EnableTransmitter(base);
    //UART_HAL_EnableReceiver(base);

    //return kStatus_UART_Success;
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_NoEdmaInit
 //* Description   : This function initializes a UART instance for receive only.
 //* This function will ungate the clock to the UART module, initialize the
 //* module to user defined settings and default settings, and enable the UART
 //* module receiver.
 //*
 //*END**************************************************************************/
//uart_status_t UART_DRV_NoEdmaInit(uint32_t instance,
                                //uart_edma_state_t * uartEdmaStatePtr,
                                //const uart_edma_user_config_t * uartUserConfig)
//{
    //assert(uartEdmaStatePtr && uartUserConfig);

    ////assert(instance < UART_INSTANCE_COUNT);
    ///* This driver only support UART instances with separate DMA channels for
     //* both Tx and Rx.*/
    //assert(FSL_FEATURE_UART_HAS_SEPARATE_DMA_RX_TX_REQn(instance) == 1);
    //InitInstancePointers();
    //UART_Type * base = RS485_UART_BASE_PTR;
    //uint32_t uartSourceClock = 0;

    //// Un-gate UART module clock
    //HAL_CLOCK_UartClockEnable(instance);
    
    //// Initialize UART to a known state.
    //UART_HAL_Init(base);

    //// UART clock source is either system or bus clock depending on instance
    //uartSourceClock = HAL_CLOCK_UartClockGetFreq(instance);
    
    //// Initialize UART baud rate, bit count, parity and stop bit.
    //UART_HAL_SetBaudRate(base, uartSourceClock, uartUserConfig->baudRate);
    //UART_HAL_SetBitCountPerChar(base, uartUserConfig->bitCountPerChar);
    //UART_HAL_SetParityMode(base, uartUserConfig->parityMode);

//#if FSL_FEATURE_UART_HAS_STOP_BIT_CONFIG_SUPPORT
    //UART_HAL_SetStopBitCount(base, uartUserConfig->stopBitCount);
//#endif


    //// Finally, enable the UART receiver. and receive data register full.
    //UART_HAL_EnableReceiver(base);

    //return kStatus_UART_Success;
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaDeinit
 //* Description   : This function shuts down the UART by disabling UART DMA and
 //*                 the transmitter/receiver.
 //*
 //*END**************************************************************************/
//uart_status_t UART_DRV_EdmaDeinit(uint32_t instance)
//{
    ////assert(instance < UART_INSTANCE_COUNT);
    
    ///* Exit if current instance is already de-initialized or is gated.*/
    //if ((!_uartStatePtr) || (!HAL_CLOCK_UartClockGetEnable(instance)))
    //{
        //return kStatus_UART_Fail;
    //}

    //UART_Type * base = RS485_UART_BASE_PTR;
    //uart_edma_state_t * uartEdmaState = (uart_edma_state_t *)_uartStatePtr;

    ///* Wait until the data is completely shifted out of shift register */
    //while(!(UART_RD_S1_TC(base))) { }

    //UART_HAL_SetTxDmaCmd(base, false);
    //UART_HAL_SetRxDmaCmd(base, false);

    ///* Release DMA channel. */
    //EDMA_DRV_ReleaseChannel(&uartEdmaState->edmaUartRx);
    //EDMA_DRV_ReleaseChannel(&uartEdmaState->edmaUartTx);

    ///* Disable TX and RX */
    //UART_HAL_DisableTransmitter(base);
    //UART_HAL_DisableReceiver(base);

    ///* Destroy TX and RX sema. */
    ////OSA_SemaDestroy(&uartEdmaState->txIrqSync);
    ////OSA_SemaDestroy(&uartEdmaState->rxIrqSync);

    ///* Cleared state pointer. */
    //_uartStatePtr = NULL;

    ///* Gate UART module clock */
    //HAL_CLOCK_UartClockEnable(instance);
    
    //return kStatus_UART_Success;
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaSendData
 //* Description   : This function sends (transmits) data through the UART module
 //*                 using a non-blocking method.
 //* A non-blocking (also known as synchronous) function means that the function
 //* returns immediately after initiating the transmit function. The application
 //* has to get the transmit status to see when the transmit is complete. In
 //* other words, after calling non-blocking (asynchronous) send function, the
 //* application must get the transmit status to check if transmit is completed
 //* or not. The asynchronous method of transmitting and receiving allows the UART
 //* to perform a full duplex operation (simultaneously transmit and receive).
 //*
 //*END**************************************************************************/
//uart_status_t UART_DRV_EdmaSendData(uint32_t instance,
                                    //const uint8_t * txBuff,
                                    //uint32_t txSize)
//{
    //assert(txBuff);
    ////assert(instance < UART_INSTANCE_COUNT);

    //uart_status_t retVal = kStatus_UART_Success;
    //uart_edma_state_t * uartEdmaState = (uart_edma_state_t *)_uartStatePtr;

    ///* Indicates current transaction is non-blocking. */
    //uartEdmaState->isTxBlocking = false;

    ///* Start the transmission process*/
    //retVal = UART_DRV_EdmaStartSendData(instance, txBuff, txSize);

    //return retVal;
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaGetTransmitStatus
 //* Description   : This function returns whether the previous UART transmit
 //* has finished. When performing an async transmit, the user can call this
 //* function to ascertain the state of the current transmission: in progress
 //* (or busy) or complete (success). In addition, if the transmission is still
 //* in progress, the user can obtain the number of words that have been
 //* currently transferred.
 //*
 //*END**************************************************************************/
//uart_status_t UART_DRV_EdmaGetTransmitStatus(uint32_t instance,
                                             //uint32_t * bytesRemaining)
//{
    ////assert(instance < UART_INSTANCE_COUNT);

    //uart_edma_state_t * uartEdmaState = (uart_edma_state_t *)_uartStatePtr;
    //uart_status_t retVal = kStatus_UART_Success;
    //uint32_t txSize = 0;

    ///* EDMA will reload the major count after finish transfer, need to set
    //* the count to 0 manually. */
    //if (uartEdmaState->isTxBusy)
    //{
        //txSize = EDMA_DRV_GetUnfinishedBytes(&uartEdmaState->edmaUartTx);
        //retVal = kStatus_UART_TxBusy;
    //}

    ///* Fill in the bytes transferred. */
    //if (bytesRemaining)
    //{
        //*bytesRemaining = txSize;
    //}

    //return retVal;
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaAbortSendingData
 //* Description   : This function terminates an asynchronous UART transmission
 //* early. During an async UART transmission, the user has the option to
 //* terminate the transmission early if the transmission is still in progress.
 //*
 //*END**************************************************************************/
//uart_status_t UART_DRV_EdmaAbortSendingData(uint32_t instance)
//{
    ////assert(instance < UART_INSTANCE_COUNT);

    //uart_edma_state_t * uartEdmaState = (uart_edma_state_t *)_uartStatePtr;

    ///* Check if a transfer is running. */
    //if (!uartEdmaState->isTxBusy)
    //{
        //return kStatus_UART_NoTransmitInProgress;
    //}

    ///* Stop the running transfer. */
    //UART_DRV_EdmaCompleteSendData(instance);

    //return kStatus_UART_Success;
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaReceiveData
 //* Description   : This function gets (receives) data from the UART module using
 //* a non-blocking method.
 //* A non-blocking (also known as synchronous) function means that the function
 //* returns immediately after initiating the receive function. The application
 //* has to get the receive status to see when the receive is complete. In other
 //* words, after calling non-blocking (asynchronous) get function, the
 //* application must get the receive status to check if receive is completed or
 //* not. The asynchronous method of transmitting and receiving allows the UART
 //* to perform a full duplex operation (simultaneously transmit and receive).
 //*
 //*END**************************************************************************/
//uart_status_t UART_DRV_EdmaReceiveData(uint32_t instance,
                                       //uint8_t * rxBuff,
                                       //uint32_t rxSize)
//{
    //assert(rxBuff);
    ////assert(instance < UART_INSTANCE_COUNT);

    //uart_status_t retVal = kStatus_UART_Success;
    //uart_edma_state_t * uartEdmaState = (uart_edma_state_t *)_uartStatePtr;

    ///* Indicates current transaction is non-blocking. */
    //uartEdmaState->isRxBlocking = false;

    //retVal = UART_DRV_EdmaStartReceiveData(instance, rxBuff, rxSize);

    //return retVal;
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaGetReceiveStatus
 //* Description   : This function returns whether the previous UART receive is
 //* complete. When performing an async receive, the user can call this function
 //* to ascertain the state of the current receive progress: in progress (or busy)
 //* or complete (success). In addition, if the receive is still in progress,
 //* the user can obtain the number of words that have been currently received.
 //*
 //*END**************************************************************************/
//uart_status_t UART_DRV_EdmaGetReceiveStatus(uint32_t instance,
                                            //uint32_t * bytesRemaining)
//{
    ////assert(instance < UART_INSTANCE_COUNT);
    //uart_edma_state_t * uartEdmaState = (uart_edma_state_t *)_uartStatePtr;
    //uart_status_t retVal = kStatus_UART_Success;
    //uint32_t rxSize = 0;

    ///* EDMA will reload the major count after finish transfer, need to set
     //* the count to 0 manually. */
    //if (uartEdmaState->isRxBusy)
    //{
        //rxSize = EDMA_DRV_GetUnfinishedBytes(&uartEdmaState->edmaUartRx);
        //retVal = kStatus_UART_RxBusy;
    //}

    ///* Fill in the bytes transferred. */
    //if (bytesRemaining)
    //{
        //*bytesRemaining = rxSize;
    //}

    //return retVal;
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaAbortReceivingData
 //* Description   : This function shuts down the UART by disabling interrupts and
 //* the transmitter/receiver.
 //*
 //*END**************************************************************************/
//uart_status_t UART_DRV_EdmaAbortReceivingData(uint32_t instance)
//{
    ////assert(instance < UART_INSTANCE_COUNT);
    //uart_edma_state_t * uartEdmaState = (uart_edma_state_t *)_uartStatePtr;

    ///* Check if a transfer is running. */
    //if (!uartEdmaState->isRxBusy)
    //{
        //return kStatus_UART_NoReceiveInProgress;
    //}

    ///* Stop the running transfer. */
    //UART_DRV_EdmaCompleteReceiveData(instance);

    //return kStatus_UART_Success;
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaCompleteSendData
 //* Description   : Finish up a transmit by completing the process of sending
 //* data and disabling the interrupt.
 //* This is not a public API as it is called from other driver functions.
 //*
 //*END**************************************************************************/
//static void UART_DRV_EdmaCompleteSendData(uint32_t instance)
//{
    ////assert(instance < UART_INSTANCE_COUNT);

    //uart_edma_state_t * uartEdmaState = (uart_edma_state_t *)_uartStatePtr;

    ///* Stop DMA channel. */
    //EDMA_DRV_StopChannel(&uartEdmaState->edmaUartTx);

    ///* Signal the synchronous completion object. */
    //if (uartEdmaState->isTxBlocking)
    //{
        //OSA_SemaPost(&uartEdmaState->txIrqSync);
    //}

    ///* Update the information of the module driver state */
    //uartEdmaState->isTxBusy = false;
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaTxCallback
 //* Description   : This is not a public interface.
 //*
 //*END**************************************************************************/
//static void UART_DRV_EdmaTxCallback(void *param, edma_chn_status_t status)
//{
    //UART_DRV_EdmaCompleteSendData((uint32_t)param);
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaStartSendData
 //* Description   : This is not a public interface.
 //*
 //*END**************************************************************************/
//static uart_status_t UART_DRV_EdmaStartSendData(uint32_t instance,
                                                //const uint8_t * txBuff,
                                                //uint32_t txSize)
//{
    ////assert(instance < UART_INSTANCE_COUNT);

    //UART_Type * base = RS485_UART_BASE_PTR;

    //edma_transfer_config_t edmaTxConfig;
    //edmaTxConfig.srcAddr            = (uint32_t)txBuff;
    //edmaTxConfig.destAddr           = UART_HAL_GetDataRegAddr(base);
    //edmaTxConfig.srcTransferSize    = kEDMATransferSize_1Bytes;
    //edmaTxConfig.destTransferSize   = kEDMATransferSize_1Bytes;
    //edmaTxConfig.srcOffset          = 1U;
    //edmaTxConfig.destOffset         = 0U;
    //edmaTxConfig.srcLastAddrAdjust  = 0U;
    //edmaTxConfig.destLastAddrAdjust = 0U;
    //edmaTxConfig.srcModulo          = kEDMAModuloDisable;
    //edmaTxConfig.destModulo         = kEDMAModuloDisable;
    //edmaTxConfig.minorLoopCount     = 1U;
    //edmaTxConfig.majorLoopCount     = txSize;

    //edma_software_tcd_t   stcd;

    ///* Get current runtime structure. */
    //uart_edma_state_t * uartEdmaState = (uart_edma_state_t *)_uartStatePtr;

    ///* Check that we're not busy already transmitting data from a previous function call. */
    //if (uartEdmaState->isTxBusy)
    //{
        //return kStatus_UART_TxBusy;
    //}

    ///* Update UART DMA run-time structure. */
    //uartEdmaState->isTxBusy = true;

    //memset(&stcd, 0, sizeof(edma_software_tcd_t));

    ///* Sets the descriptor basic transfer for the descriptor. */
    //EDMA_DRV_PrepareDescriptorTransfer(&uartEdmaState->edmaUartTx, &stcd, &edmaTxConfig, true, true);

    ///* Copies the software TCD configuration to the hardware TCD */
    //EDMA_DRV_PushDescriptorToReg(&uartEdmaState->edmaUartTx, &stcd);

    ///* Start DMA channel */
    //EDMA_DRV_StartChannel(&uartEdmaState->edmaUartTx);

    //return kStatus_UART_Success;
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaCompleteReceiveData
 //* Description   : Finish up a receive by completing the process of receiving data
 //* and disabling the interrupt.
 //* This is not a public API as it is called from other driver functions.
 //*
 //*END**************************************************************************/
//static void UART_DRV_EdmaCompleteReceiveData(uint32_t instance)
//{
    ////assert(instance < UART_INSTANCE_COUNT);

    //uart_edma_state_t * uartEdmaState = (uart_edma_state_t *)_uartStatePtr;

    ///* Stop DMA channel. */
    //EDMA_DRV_StopChannel(&uartEdmaState->edmaUartRx);

    ///* Signal the synchronous completion object. */
    //if (uartEdmaState->isRxBlocking)
    //{
        //OSA_SemaPost(&uartEdmaState->rxIrqSync);
    //}

    ///* Update the information of the module driver state */
    //uartEdmaState->isRxBusy = false;
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaRxCallback
 //* Description   : This is not a public interface.
 //*
 //*END**************************************************************************/
//static void UART_DRV_EdmaRxCallback(void *param, edma_chn_status_t status)
//{
    //// I don't know that we actually need to do anything here now, it may all be handled by the DMA driver
    ////UART_DRV_EdmaCompleteReceiveData((uint32_t)param);
//}

///*FUNCTION**********************************************************************
 //*
 //* Function Name : UART_DRV_EdmaStartReceiveData
 //* Description   : Initiate (start) a receive by beginning the process of
 //* receiving data and enabling the interrupt.
 //* This is not a public API as it is called from other driver functions.
 //*
 //*END**************************************************************************/
//// there is a bug in the fsl_edma_driver that changes the address of the tcd pointer to the next 32 byte aligned address
//// so we need this array to be one longer than we actually need
//__align(32) static edma_software_tcd_t rx_stcd[2]; // Need 32 bytes aligned, two buffer block, needs 2 TCD.

//static uart_status_t UART_DRV_EdmaStartReceiveData(uint32_t instance,
                                                   //uint8_t * rxBuff,
                                                   //uint32_t rxSize)
//{
    ////assert(instance < UART_INSTANCE_COUNT);

    //UART_Type * base = RS485_UART_BASE_PTR;

    ///* Get current runtime structure. */
    //uart_edma_state_t * uartEdmaState = (uart_edma_state_t *)_uartStatePtr;

    ///* Check that we're not busy already receiving data from a previous function call. */
    //if (uartEdmaState->isRxBusy)
    //{
        //return kStatus_UART_RxBusy;
    //}

    ///* Update UART DMA run-time structure. */
    //uartEdmaState->isRxBusy = true;

    //EDMA_DRV_ConfigLoopTransfer(&uartEdmaState->edmaUartRx, rx_stcd, kEDMAPeripheralToMemory, UART_HAL_GetDataRegAddr(base),
        //(uint32_t)rxBuff, 1, 1, rxSize, 1) ;
   
    ///* Start DMA channel */
    //EDMA_DRV_StartChannel(&uartEdmaState->edmaUartRx);
    
    //return kStatus_UART_Success;
//}
/** RS485Serial_Public_Functions
*	@{
*/

#define RS485_RX_BUFF_SIZE			1024
typedef struct
{
	uint8_t buff[RS485_RX_BUFF_SIZE];
	uint16_t ridx;
	uint16_t widx;
} RS485_FIFO;

RS485_FIFO g_RS485_FIFO;

void RS485_Rx_Push(uint8_t c)
{
	//debugMSG(0, "RS485 Rx Push: %xb\n",c);
	g_RS485_FIFO.buff[g_RS485_FIFO.widx] = c;
	g_RS485_FIFO.widx++;
	if(g_RS485_FIFO.widx >= (RS485_RX_BUFF_SIZE))
	{
		g_RS485_FIFO.widx = 0;
	}
	
	if(g_RS485_FIFO.widx == g_RS485_FIFO.ridx)
	{
		g_RS485_FIFO.ridx++;
		if(g_RS485_FIFO.ridx >= (RS485_RX_BUFF_SIZE))
		{
			g_RS485_FIFO.ridx = 0;
		}
	}
}

uint16_t RS485_ReadByte(void)
{
	uint8_t data = 0;
	
	if(g_RS485_FIFO.widx != g_RS485_FIFO.ridx)
	{
		data = g_RS485_FIFO.buff[g_RS485_FIFO.ridx];
		g_RS485_FIFO.ridx++;
		if(g_RS485_FIFO.ridx >= (RS485_RX_BUFF_SIZE))
		{
			g_RS485_FIFO.ridx = 0;
		}
	}
	
	
	//debugMSG(0, "RS485 Ready byte: %xb\n",data);
	return data;
}

uint16_t RS485_Get_Buffer_Size(void)
{
	if(g_RS485_FIFO.widx == g_RS485_FIFO.ridx)
	{
		return 0;
	}
	else if(g_RS485_FIFO.widx > g_RS485_FIFO.ridx)
	{
		return (g_RS485_FIFO.widx - g_RS485_FIFO.ridx);
	}
	else
	{
		return (g_RS485_FIFO.widx +  ((RS485_RX_BUFF_SIZE) - g_RS485_FIFO.ridx));
	}
}
/*------Public Function Definitions-----*/


void RS485_Serial_RegisterTxCompleteCallback(void(*callback)(void)) {
    _rs485_tx_complete_callback = callback;
}

/********************************************************************//**
 * @brief 		Initialize the Serial Peripheral
 *
 * Initialize with basic receive setup and then wait for 5ms delay since
 * data received. Will always return -1 on first call. Should be called
 * as quickly as possible (<1ms) until properly initialized (returns 0)
 *
 * Worst case initialization time on a half-duplex setup would be packet
 * time + 5ms. Worst case initialization time on a full-duplex setup would
 * be (packet time)*(endpoints - 1) + 5ms.
 *
 * @return 		0 (good), -1 (waiting for receive gap)
 *********************************************************************/
int32_t    RS485_Serial_init(uint32_t baud_rate){
	//debugMSG(0, "RS485_Serial_init()\n");
    static uint8_t init_state = 0;
    //static uint32_t timestamp;
    InitInstancePointers();
    //uartConfig.baudRate = baud_rate;
    switch(init_state) {
        case 0:
			g_RS485_FIFO.widx = 0;
			g_RS485_FIFO.ridx = 0;
        
            _rs485_tx_complete_callback = 0;
            // Note, pin muxing done dueing hardware_init in board.c
			// configure & enable the RE and DE pins
            RS485RE_INIT;
            RS485DE_INIT;
            // Set up for receive enable */
            RS485RE_ON;
            RS485DE_OFF;
            // initailize standard UART receiver with no EDMA
            //UART_DRV_NoEdmaInit(RS485_UART_INSTANCE, &uartStateEdma, &uartConfig);
			UART_Init(RS485_UART_INSTANCE);
			UART_RegisterReceiver(RS485_UART_INSTANCE, RS485_Rx_Push);
			//debugMSG(0, "RS485_Serial_init() RES: %d\n",res); 
			
            // initialize timestamp
            //timestamp = TimeNow();
            // switch to state 1
            init_state = 1;
            break;
        case 1:
            //// check for UART data received
            ////UART_HAL_Getchar(UART_Type * base, uint8_t *readData);
            //if(UART_HAL_GetStatusFlag(RS485_UART_BASE_PTR, kUartRxActive)) {
                //timestamp = TimeNow();
                //UART_HAL_ClearStatusFlag(RS485_UART_BASE_PTR, kUartRxActive);
            //}
            //if(UART_HAL_GetStatusFlag(RS485_UART_BASE_PTR, kUartRxOverrun)) {
                //UART_HAL_ClearStatusFlag(RS485_UART_BASE_PTR, kUartRxOverrun);
            //}
            // if receive time since last byte is greater than 5ms then we must not be actively receiving anymore
            //if(TimeSince(timestamp) >= 5) {
                //// Initialize driver
                //UART_DRV_EdmaInit(RS485_UART_INSTANCE, &uartStateEdma, &uartConfig);
                ////register the irq callback ui
                //fsl_uart_callback_register((void(*)(void))&uart_irq_callback, RS485_UART_INSTANCE, true);
                //// enable interrupts, uart irqs not used by DMA so we set it up manually for the direction control
                //NVIC_EnableIRQ(RS485_UART_IRQ_HANDLER);
                //// clear overflow
                //UART_HAL_ClearStatusFlag(RS485_UART_BASE_PTR, kUartRxOverrun);
                //// reset state
                //init_state = 0;
                //return 0;
            //}
            break;
        default:
            init_state = 0;
            break;
    }
    return -1;
}

/********************************************************************//**
 * @brief 		Write to the serial buffer
 * @param[in]	None
 * @return 		None
 *********************************************************************/
uint32_t   RS485_Serial_Write(uint8_t *txbuf, uint32_t buflen){
	//debugMSG(0, "RS485_Serial_Write()\n"); 
	RS485DE_ON;

	//UART_DRV_EdmaSendData(RS485_UART_INSTANCE, txbuf, buflen);

	///* Enable the Transmit Complete interrupt
		//so that we know when the transmitter has gone idle
	//*/
	//UART_HAL_SetIntMode(RS485_UART_BASE_PTR, kUartIntTxComplete, true);
	
	uint32_t k;
	
	//debugMSG(DWARNING, "RS485 Tx size: %d\n",buflen);
	UART_printf(SUBSYSTEM_UART, "RS485 Tx size: %d\r\n",buflen);
	for(k=0;k<buflen;k++)
	{
		//debugMSG(DAPPEND, "%xb ",txbuf[k]);
		UART_WriteByte(RS485_UART_INSTANCE, txbuf[k]);
	}
	//debugMSG(DAPPEND, "\n");
	
	_rs485_tx_complete_callback();
	return 0;
}
/********************************************************************//**
 * @brief 		Read from the buffer
 * @param[in]   rxbuf: pointer fo buffer to populate
 * @param[in]   buflen: maximum number of bytes to return
 * @return 		the number of bytes actually returned
 *********************************************************************/
uint32_t   RS485_Serial_Read(uint8_t *rxbuf, uint32_t buflen){
	
	uint16_t bytes_received = RS485_Get_Buffer_Size();
	uint16_t k;
	
	if(bytes_received > 0)
	{	
		if(bytes_received > buflen)
		{
			bytes_received = buflen;
		}
		
		//debugMSG(DNOTIY, "RS485 Rx size: %d - %d\n",bytes_received,buflen);
		for(k=0;k<bytes_received;k++)
		{
			rxbuf[k] = RS485_ReadByte();
			//debugMSG(DAPPEND, "%xb ",rxbuf[k]);
		}
		//debugMSG(DAPPEND, "\n");
	}
	else
	{
		bytes_received = 0;
	}
	
	
	//debugMSG(0, "RS485_Serial_Read()\n");
	//uint32_t bytes_remaining;
    //uint32_t bytes_received = 0;
	////uart_status_t status;
	
    ///* Get current runtime structure. */
    //uart_edma_state_t * uartEdmaState = (uart_edma_state_t *)_uartStatePtr;
    
	////status= UART_DRV_EdmaGetReceiveStatus(RS485_UART_INSTANCE,
    ////                                         &bytesRemaining);
    //bytes_remaining = EDMA_DRV_GetUnfinishedBytes(&uartEdmaState->edmaUartRx);
    //// not sure why but sometimes the above function returns 0 instead of the correct value
    //// if bytes_remaining is 0 then return
    //if(bytes_remaining == 0) {
        //return 0;
    //}
	//if(bytes_remaining != last_bytes_remaining) {
        //// move the new bytes to the buffer passed in
        //if(bytes_remaining < last_bytes_remaining) {
            //bytes_received = last_bytes_remaining - bytes_remaining;
            //if(bytes_received <= buflen) {
                //memcpy(rxbuf, &buffer[0] + BUFFER_SIZE - last_bytes_remaining, bytes_received);
                //last_bytes_remaining = bytes_remaining;
            //}else {
                //memcpy(rxbuf, &buffer[0] + BUFFER_SIZE - last_bytes_remaining, buflen);
                //last_bytes_remaining = last_bytes_remaining - buflen;
                //bytes_received = buflen;
            //}
        //}else {
            //// we have wrapped around
            //// copy the last_bytes_remaining at the end of the buffer first
            //if(last_bytes_remaining < buflen) {
                //memcpy(rxbuf, &buffer[0] + BUFFER_SIZE - last_bytes_remaining, last_bytes_remaining);
                //// copy the bytes received at the front of the buffer
                //bytes_received = BUFFER_SIZE + last_bytes_remaining - bytes_remaining;
                //if(bytes_received <= buflen) {
                    //memcpy((rxbuf + last_bytes_remaining), &buffer[0], BUFFER_SIZE - bytes_remaining);
                    //last_bytes_remaining = bytes_remaining;
                //}else {
                    //memcpy((rxbuf + last_bytes_remaining), &buffer[0], buflen - last_bytes_remaining);
                    //last_bytes_remaining = BUFFER_SIZE - buflen + last_bytes_remaining;
                    //bytes_received = buflen;
                //}
            //}else {
                //// if the user requested less than is in the top end of the buffer
                //memcpy(rxbuf, &buffer[0] + BUFFER_SIZE - last_bytes_remaining, buflen);
                //last_bytes_remaining = last_bytes_remaining - buflen;
                //bytes_received = buflen;
            //}
        //}
	//}

    //if( UART_RD_S1_FE(RS485_UART_BASE_PTR) )
    //{
        //uint8_t badByte = UART_D_REG(RS485_UART_BASE_PTR);
    //}

    return bytes_received;
}
/********************************************************************//**
 * @brief 		De-Init the serial device
 * @param[in]	None
 * @return 		None
 *********************************************************************/
int32_t    RS485_Serial_Close(void){
    // don't try and abort things if it isn't running
    //if(_uartStatePtr) {
		//UART_DRV_EdmaAbortReceivingData(RS485_UART_INSTANCE);
		//UART_DRV_EdmaAbortSendingData(RS485_UART_INSTANCE);
		//UART_DRV_EdmaDeinit(RS485_UART_INSTANCE);
    //}
	//NVIC_DisableIRQ(RS485_UART_IRQ_HANDLER);
    return 0;
}
/********************************************************************//**
 * @brief 		return TX FiFo Length
 * @param[in]	None
 * @return 		None
 *********************************************************************/
uint32_t   RS485_Serial_Get_Tx_Fifo_length(void){
	return 0;
}


/** RS485Serial_Public_Functions
*	@}
*/


/** RS485Serial
*	@}
*/

/** PackageName
*	@}
*/



/* ~~~~~~~~~~~~~~~~~~~~~~~~~ EOF ~~~~~~~~~~~~~~~~~~~~~~~~~ */

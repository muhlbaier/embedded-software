/*
 * uart.c
 *
 *  Created on: Mar 12, 2014
 *      Author: Michael Muhlbaier
 *  Updated on: Feb 7, 2015
 *      Author: Anthony Merlino
 */

#include "uart.h"
#include "hal_general.h"
// uart.h includes project_settings.h which would define USE_MODULE_TASK if it is used
#ifdef USE_MODULE_TASK
#include "task.h"
#endif

// helper function to initiate UART transmission
static void UART_Tx_Start(uint8_t channel);

// receiver callback prototype
typedef void(*receiver_callback_t)(uint8_t);

// receiver_list structure
typedef struct {
	uint16_t max_size; // max number of receivers
	uint16_t size; // current number of receivers
	receiver_callback_t *receivers; // points to array of receivers
} receiver_list_t;

/**************************************
 * Default Macros
 *************************************/
#ifndef UART_BAUD
#define UART_BAUD 115200
#endif
#define NUM_RECEIVERS 2
#ifndef UART_TX_BUFFER_LENGTH
#define UART_TX_BUFFER_LENGTH 128
#endif
#ifndef UART_RX_BUFFER_LENGTH
#define UART_RX_BUFFER_LENGTH 32
#endif
#ifndef UART_MAX_CHARS_PER_CALL
#define UART_MAX_CHARS_PER_CALL 20
#endif

#ifdef USE_UART7
    #ifndef UART7_BAUD
        #define UART7_BAUD UART_BAUD
    #endif
    #ifndef NUM_UART7_RECEIVERS
        #define NUM_UART7_RECEIVERS NUM_RECEIVERS
    #endif
    #ifndef UART7_RX_BUFFER_LENGTH
        #define UART7_RX_BUFFER_LENGTH UART_RX_BUFFER_LENGTH
    #endif
    #ifndef UART7_TX_BUFFER_LENGTH
        #define UART7_TX_BUFFER_LENGTH UART_TX_BUFFER_LENGTH
    #endif
    buffer_t rx7, tx7;
    uint8_t tx_buffer_array7[UART7_TX_BUFFER_LENGTH];
    uint8_t rx_buffer_array7[UART7_RX_BUFFER_LENGTH];
    receiver_list_t receiverList7;
    receiver_callback_t receiver_array7[NUM_UART7_RECEIVERS];
#endif
#ifdef USE_UART6
    #ifndef UART6_BAUD
        #define UART6_BAUD UART_BAUD
    #endif
    #ifndef NUM_UART6_RECEIVERS
        #define NUM_UART6_RECEIVERS NUM_RECEIVERS
    #endif
    #ifndef UART6_RX_BUFFER_LENGTH
        #define UART6_RX_BUFFER_LENGTH UART_RX_BUFFER_LENGTH
    #endif
    #ifndef UART6_TX_BUFFER_LENGTH
        #define UART6_TX_BUFFER_LENGTH UART_TX_BUFFER_LENGTH
    #endif
    buffer_t rx6, tx6;
    uint8_t tx_buffer_array6[UART6_TX_BUFFER_LENGTH];
    uint8_t rx_buffer_array6[UART6_RX_BUFFER_LENGTH];
    receiver_list_t receiverList6;
    receiver_callback_t receiver_array6[NUM_UART6_RECEIVERS];
#endif
#ifdef USE_UART5
    #ifndef UART5_BAUD
        #define UART5_BAUD UART_BAUD
    #endif
    #ifndef NUM_UART5_RECEIVERS
        #define NUM_UART5_RECEIVERS NUM_RECEIVERS
    #endif
    #ifndef UART5_RX_BUFFER_LENGTH
        #define UART5_RX_BUFFER_LENGTH UART_RX_BUFFER_LENGTH
    #endif
    #ifndef UART5_TX_BUFFER_LENGTH
        #define UART5_TX_BUFFER_LENGTH UART_TX_BUFFER_LENGTH
    #endif
    buffer_t rx5, tx5;
    uint8_t tx_buffer_array5[UART5_TX_BUFFER_LENGTH];
    uint8_t rx_buffer_array5[UART5_RX_BUFFER_LENGTH];
    receiver_list_t receiverList5;
    receiver_callback_t receiver_array5[NUM_UART5_RECEIVERS];
#endif
#ifdef USE_UART4
    #ifndef UART4_BAUD
        #define UART4_BAUD UART_BAUD
    #endif
    #ifndef NUM_UART4_RECEIVERS
        #define NUM_UART4_RECEIVERS NUM_RECEIVERS
    #endif
    #ifndef UART4_RX_BUFFER_LENGTH
        #define UART4_RX_BUFFER_LENGTH UART_RX_BUFFER_LENGTH
    #endif
    #ifndef UART4_TX_BUFFER_LENGTH
        #define UART4_TX_BUFFER_LENGTH UART_TX_BUFFER_LENGTH
    #endif
    buffer_t rx4, tx4;
    uint8_t tx_buffer_array4[UART4_TX_BUFFER_LENGTH];
    uint8_t rx_buffer_array4[UART4_RX_BUFFER_LENGTH];
    receiver_list_t receiverList4;
    receiver_callback_t receiver_array4[NUM_UART4_RECEIVERS];
#endif
#ifdef USE_UART3
    #ifndef UART3_BAUD
        #define UART3_BAUD UART_BAUD
    #endif
    #ifndef NUM_UART3_RECEIVERS
        #define NUM_UART3_RECEIVERS NUM_RECEIVERS
    #endif
    #ifndef UART3_RX_BUFFER_LENGTH
        #define UART3_RX_BUFFER_LENGTH UART_RX_BUFFER_LENGTH
    #endif
    #ifndef UART3_TX_BUFFER_LENGTH
        #define UART3_TX_BUFFER_LENGTH UART_TX_BUFFER_LENGTH
    #endif
    buffer_t rx3, tx3;
    uint8_t tx_buffer_array3[UART3_TX_BUFFER_LENGTH];
    uint8_t rx_buffer_array3[UART3_RX_BUFFER_LENGTH];
    receiver_list_t receiverList3;
    receiver_callback_t receiver_array3[NUM_UART3_RECEIVERS];
#endif
#ifdef USE_UART2
    #ifndef UART2_BAUD
        #define UART2_BAUD UART_BAUD
    #endif
    #ifndef NUM_UART2_RECEIVERS
        #define NUM_UART2_RECEIVERS NUM_RECEIVERS
    #endif
    #ifndef UART2_RX_BUFFER_LENGTH
        #define UART2_RX_BUFFER_LENGTH UART_RX_BUFFER_LENGTH
    #endif
    #ifndef UART2_TX_BUFFER_LENGTH
        #define UART2_TX_BUFFER_LENGTH UART_TX_BUFFER_LENGTH
    #endif
    buffer_t rx2, tx2;
    uint8_t tx_buffer_array2[UART2_TX_BUFFER_LENGTH];
    uint8_t rx_buffer_array2[UART2_RX_BUFFER_LENGTH];
    receiver_list_t receiverList2;
    receiver_callback_t receiver_array2[NUM_UART2_RECEIVERS];
#endif
#ifdef USE_UART1
    #ifndef UART1_BAUD
        #define UART1_BAUD UART_BAUD
    #endif
    #ifndef NUM_UART1_RECEIVERS
        #define NUM_UART1_RECEIVERS NUM_RECEIVERS
    #endif
    #ifndef UART1_RX_BUFFER_LENGTH
        #define UART1_RX_BUFFER_LENGTH UART_RX_BUFFER_LENGTH
    #endif
    #ifndef UART1_TX_BUFFER_LENGTH
        #define UART1_TX_BUFFER_LENGTH UART_TX_BUFFER_LENGTH
    #endif
    buffer_t rx1, tx1;
    uint8_t tx_buffer_array1[UART1_TX_BUFFER_LENGTH];
    uint8_t rx_buffer_array1[UART1_RX_BUFFER_LENGTH];
    receiver_list_t receiverList1;
    receiver_callback_t receiver_array1[NUM_UART1_RECEIVERS];
#endif
#ifdef USE_UART0
    #ifndef UART0_BAUD
        #define UART0_BAUD UART_BAUD
    #endif
    #ifndef NUM_UART0_RECEIVERS
        #define NUM_UART0_RECEIVERS NUM_RECEIVERS
    #endif
    #ifndef UART0_RX_BUFFER_LENGTH
        #define UART0_RX_BUFFER_LENGTH UART_RX_BUFFER_LENGTH
    #endif
    #ifndef UART0_TX_BUFFER_LENGTH
        #define UART0_TX_BUFFER_LENGTH UART_TX_BUFFER_LENGTH
    #endif
    buffer_t rx0, tx0;
    uint8_t tx_buffer_array0[UART0_TX_BUFFER_LENGTH];
    uint8_t rx_buffer_array0[UART0_RX_BUFFER_LENGTH];
    receiver_list_t receiverList0;
    receiver_callback_t receiver_array0[NUM_UART0_RECEIVERS];
#endif

#ifdef USE_UART7
    #define NUM_UARTS 8
#elif defined USE_UART6
    #define NUM_UARTS 7
#elif defined USE_UART5
    #define NUM_UARTS 6
#elif defined USE_UART4
    #define NUM_UARTS 5
#elif defined USE_UART3
    #define NUM_UARTS 4
#elif defined USE_UART2
    #define NUM_UARTS 3
#elif defined USE_UART1
    #define NUM_UARTS 2
#elif defined USE_UART0
    #define NUM_UARTS 1
#else
#error No UARTS used, remove this file from project
#endif

struct {
    buffer_t * rx;
    buffer_t * tx;
    receiver_list_t * receiverList;
    uint8_t errorStatus;
    void(*errorNotificationCallback)(enum uart_errors);
} uart[NUM_UARTS];

uint8_t rx_flags;
uint8_t error_flags;

void UART_Init(uint8_t channel) {
#if defined(_TASK_H_) && !defined(UART_TICK_OVERRIDE)
    // use flag so init calls to Task_Init get called only once
    static uint8_t init_flag = 0;
    // if the Task Management Module is being used then schedule
    // the UART_Tick to happen every 1 ms
    if(init_flag == 0) {
        // make sure the task manager is initialized
        Task_Init();
        Task_Schedule(UART_Tick,0,1,1);
    }
    init_flag = 1;
#endif
		
    rx_flags = 0;
    error_flags = 0;

    switch(channel){
#ifdef USE_UART0
        case 0:
            hal_UART_Init(channel, UART0_BAUD);
            Buffer_Init(&rx0, &rx_buffer_array0[0], UART0_RX_BUFFER_LENGTH);
            Buffer_Init(&tx0, &tx_buffer_array0[0], UART0_TX_BUFFER_LENGTH);
            receiverList0.max_size = NUM_UART0_RECEIVERS;
			receiverList0.size = 0;
			receiverList0.receivers = &receiver_array0[0];
            uart[0].rx = &rx0;
            uart[0].tx = &tx0;
            uart[0].receiverList = &receiverList0;
            uart[0].errorNotificationCallback = 0;
            uart[0].errorStatus = 0;
            break;
#endif
#ifdef USE_UART1
        case 1:
            hal_UART_Init(channel, UART1_BAUD);
            Buffer_Init(&rx1, &rx_buffer_array1[0], UART1_RX_BUFFER_LENGTH);
            Buffer_Init(&tx1, &tx_buffer_array1[0], UART1_TX_BUFFER_LENGTH);
            receiverList1.max_size = NUM_UART1_RECEIVERS;
			receiverList1.size = 0;
			receiverList1.receivers = &receiver_array1[0];
			uart[1].rx = &rx1;
            uart[1].tx = &tx1;
            uart[1].receiverList = &receiverList1;
            uart[1].errorNotificationCallback = 0;
            uart[1].errorStatus = 0;
            break;
#endif
#ifdef USE_UART2
        case 2:
            hal_UART_Init(channel, UART2_BAUD);
            Buffer_Init(&rx2, &rx_buffer_array2[0], UART2_RX_BUFFER_LENGTH);
            Buffer_Init(&tx2, &tx_buffer_array2[0], UART2_TX_BUFFER_LENGTH);
            receiverList2.max_size = NUM_UART2_RECEIVERS;
			receiverList2.size = 0;
			receiverList2.receivers = &receiver_array2[0];
			uart[2].rx = &rx2;
            uart[2].tx = &tx2;
            uart[2].receiverList = &receiverList2;
            uart[2].errorNotificationCallback = 0;
            uart[2].errorStatus = 0;
            break;
#endif
#ifdef USE_UART3
        case 3:
            hal_UART_Init(channel, UART3_BAUD);
            Buffer_Init(&rx3, &rx_buffer_array3[0], UART3_RX_BUFFER_LENGTH);
            Buffer_Init(&tx3, &tx_buffer_array3[0], UART3_TX_BUFFER_LENGTH);
            receiverList3.max_size = NUM_UART3_RECEIVERS;
			receiverList3.size = 0;
			receiverList3.receivers = &receiver_array3[0];
			uart[3].rx = &rx3;
            uart[3].tx = &tx3;
            uart[3].receiverList = &receiverList3;
            uart[3].errorNotificationCallback = 0;
            uart[3].errorStatus = 0;
            break;
#endif
#ifdef USE_UART4
        case 4:
            hal_UART_Init(channel, UART4_BAUD);
            Buffer_Init(&rx4, &rx_buffer_array4[0], UART4_RX_BUFFER_LENGTH);
            Buffer_Init(&tx4, &tx_buffer_array4[0], UART4_TX_BUFFER_LENGTH);
            receiverList4.max_size = NUM_UART4_RECEIVERS;
			receiverList4.size = 0;
			receiverList4.receivers = &receiver_array4[0];
			uart[4].rx = &rx4;
            uart[4].tx = &tx4;
            uart[4].receiverList = &receiverList4;
            uart[4].errorNotificationCallback = 0;
            uart[4].errorStatus = 0;
            break;
#endif
#ifdef USE_UART5
        case 5:
            hal_UART_Init(channel, UART5_BAUD);
            Buffer_Init(&rx5, &rx_buffer_array5[0], UART5_RX_BUFFER_LENGTH);
            Buffer_Init(&tx5, &tx_buffer_array5[0], UART5_TX_BUFFER_LENGTH);
            receiverList5.max_size = NUM_UART5_RECEIVERS;
			receiverList5.size = 0;
			receiverList5.receivers = &receiver_array5[0];
			uart[5].rx = &rx5;
            uart[5].tx = &tx5;
            uart[5].receiverList = &receiverList5;
            uart[5].errorNotificationCallback = 0;
            uart[5].errorStatus = 0;
            break;
#endif
#ifdef USE_UART6
        case 6:
            hal_UART_Init(channel, UART6_BAUD);
            Buffer_Init(&rx6, &rx_buffer_array6[0], UART6_RX_BUFFER_LENGTH);
            Buffer_Init(&tx6, &tx_buffer_array6[0], UART6_TX_BUFFER_LENGTH);
            receiverList6.max_size = NUM_UART6_RECEIVERS;
			receiverList6.size = 0;
			receiverList6.receivers = &receiver_array6[0];
			uart[6].rx = &rx6;
            uart[6].tx = &tx6;
            uart[6].receiverList = &receiverList6;
            uart[6].errorNotificationCallback = 0;
            uart[6].errorStatus = 0;
            break;
#endif
#ifdef USE_UART7
        case 7:
            hal_UART_Init(channel, UART7_BAUD);
            Buffer_Init(&rx7, &rx_buffer_array7[0], UART7_RX_BUFFER_LENGTH);
            Buffer_Init(&tx7, &tx_buffer_array7[0], UART7_TX_BUFFER_LENGTH);
            receiverList7.max_size = NUM_UART7_RECEIVERS;
			receiverList7.size = 0;
			receiverList7.receivers = &receiver_array7[0];
			uart[7].rx = &rx7;
            uart[7].tx = &tx7;
            uart[7].receiverList = &receiverList7;
            uart[7].errorNotificationCallback = 0;
            uart[7].errorStatus = 0;
            break;
#endif
    }
}

void UART_Tick(void){
    uint8_t c;
    volatile unsigned int i, j;
    volatile unsigned int b;

    // Handle any received data
    if(rx_flags != 0){
        for(i = 0; i < NUM_UARTS; i++){
            if(rx_flags & (1 << i)){
                b = Buffer_GetSize(uart[i].rx);
                if(b > UART_MAX_CHARS_PER_CALL){
					b = UART_MAX_CHARS_PER_CALL;
				} else {
					rx_flags &= ~(1 << i); // clear the flag
				}
                while(b--){
                    c = Buffer_Dequeue(uart[i].rx);
					// call receivers
					for(j = 0; j < uart[i].receiverList->size; j++){
						uart[i].receiverList->receivers[j](c);
					}
                }
            }
        }
    }

    // Handle any errors
    if(error_flags){
    	for(i = 0; i < NUM_UARTS; i++){
    		// Check for each type of error
    		for(b = 0; b < NUM_UART_ERRORS; b++){
    			if(uart[i].errorStatus & (1 << b)){
					if(uart[i].errorNotificationCallback){
						uart[i].errorNotificationCallback((enum uart_errors)b);
					}
					// Clear the error flag
					uart[i].errorStatus &= ~(1 << b);
				}
    		}

    		// Check to make sure all the errors are clear before clearing the
    		// channel error flag.  Block interrupts while checking so that
    		// there is not a race condition created.
    		BlockInterrupts();
    		if(!uart[i].errorStatus)
    		{
    			error_flags &= ~(1 << i);
    		}
    		RestoreInterrupts();
    	}
    }
}

void UART_ReconfigureBaud(uint8_t channel, uint32_t baud){
	hal_UART_Init(channel, baud);
}

void UART_WriteByte(uint8_t channel, uint8_t data) {
    if(channel >= NUM_UARTS) return;
    Buffer_Enqueue(uart[channel].tx, data);
    UART_Tx_Start(channel);
}

int8_t UART_Write(uint8_t channel, uint8_t * data, uint16_t length) {
    if(channel >= NUM_UARTS) return -2;
    if(Buffer_EnqueueData(uart[channel].tx, data, length) == BUFFER_ENQUEUE_SUCCEEDED) {
        UART_Tx_Start(channel);
        return 0;
    }
    return -1;
}

// this module could be used without buffer_printf if these functions are
// not used. Flag added to make it easy to not require buffer_printf
#ifdef _BUFFER_PRINTF_H_
void UART_printf(uint8_t channel, char * str,...){
    // variable argument list type
    va_list vars;
    // initialize the variable argument list pointer by specifying the
    // input argument immediately preceding the variable list
    va_start(vars, str);

    Buffer_vprintf(uart[channel].tx, str, vars);
    va_end(vars);
    UART_Tx_Start(channel);
}

void UART_vprintf(uint8_t channel, char * str, va_list vars) {
    if(channel >= NUM_UARTS) return;
    Buffer_vprintf(uart[channel].tx, str, vars);
    UART_Tx_Start(channel);
}
#endif

int8_t UART_RegisterReceiver(uint8_t channel, receiver_callback_t fn){
    if(channel >= NUM_UARTS) return -1;
	receiver_list_t * list;
	list = uart[channel].receiverList;
	if(list->size == list->max_size) {
#ifdef USE_MODULE_SUBSYS
		LogWarning(0x00, "No space left in char receiver list\r\n");
#endif
        return -2;
	}
	// save the receiver callback
	list->receivers[list->size++] = fn; 
    return 0;
}

void UART_UnregisterReceiver(uint8_t channel, receiver_callback_t fn){
    if(channel >= NUM_UARTS) return;
	volatile uint16_t i, j;
	receiver_list_t * list;
	list = uart[channel].receiverList;
	// loop through the receivers array for the function pointer (fn)
	// when found shift remaining array items left to overwrite (fn)
	// then set the last function pointer to 0
	for(i = 0; i < list->size; i++) {
		if(list->receivers[i] == fn) {
			for(j = i+1; j < list->size; j++) {
				list->receivers[j-1] = list->receivers[j];
			}
			list->receivers[j-1] = 0;
			list->size--;
			return;
		}
	}
}

uint8_t UART_IsTransmitting(uint8_t channel) {
    if(channel >= NUM_UARTS) return 0;
    return Buffer_GetSize(uart[channel].tx);
}

void UART_Tx_Handler(uint8_t channel){
    while(hal_UART_SpaceAvailable(channel)) {
        if(Buffer_GetSize(uart[channel].tx)){
            hal_UART_TxByte(channel, Buffer_Dequeue(uart[channel].tx));
        } else {
            hal_UART_DisableTxInterrupt(channel);
            break;
        }
    }
}

void UART_Tx_Start(uint8_t channel){

	// If the interrupt hasn't been enabled, we need to kick off transmission
	if(!hal_UART_TxInterruptEnabled(channel)){
	    while(hal_UART_SpaceAvailable(channel)) {
	        if(Buffer_GetSize(uart[channel].tx)){
	            hal_UART_TxByte(channel, Buffer_Dequeue(uart[channel].tx));
	        } else {
	        	/* It's okay to return here without enabling interrupts even if
	        	 * data was added to the transmit buffer.  An interrupt isn't required
	        	 * if data doesn't need to be loaded into the buffer */
	            return;
	        }
	    }

	    hal_UART_EnableTxInterrupt(channel);
	}
}

void UART_Rx_Handler(uint8_t channel) {
    while(hal_UART_DataAvailable(channel)) {
        Buffer_Enqueue(uart[channel].rx, hal_UART_RxByte(channel));
        rx_flags |= (1 << channel);
    }
}

void UART_Error_Handler(uint8_t channel, enum uart_errors error){
	// Update the error status for the UART channel
	uart[channel].errorStatus |= (1 << error);
	// Mark the UART channel as having an error so the user
	// can be notified from the UART_Tick update
	error_flags |= (1 << channel);
}

void UART_RegisterErrorCallback(uint8_t channel, void(*callback)(enum uart_errors)){
	uart[channel].errorNotificationCallback = callback;
}

void UART_RegisterTxOverwriteCallback(uint8_t channel, void(*overwriteCallback)(void)){
	uart[channel].tx->OverwriteCallback = overwriteCallback;
}

void UART_RegisterRxOverwriteCallback(uint8_t channel, void(*overwriteCallback)(void)){
	uart[channel].rx->OverwriteCallback = overwriteCallback;
}

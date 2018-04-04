#ifndef PROJECT_SETTINGS_H_
#define PROJECT_SETTINGS_H_

#define FCPU 80000000

#define UART1_TX_PIN			GPIO_PIN_6
#define UART1_TX_ALT_FUNCTION 	GPIO_AF7_USART1
#define UART1_TX_GPIO_PORT      GPIOB
#define UART1_RX_PIN            GPIO_PIN_7
#define UART1_RX_ALT_FUNCTION   GPIO_AF7_USART1
#define UART1_RX_GPIO_PORT      GPIOB

// include the library header
#include "library.h"
// let the system know which lower level modules are in use
// this way higher modules can selectively utilize their resources
#define USE_MODULE_TASK
#define USE_MODULE_SUBSYSTEM
#define USE_MODULE_BUFFER_PRINTF

// STM's HAL is using the SysTick_Handler but they provide a callback so the user code can get a tick callback
// MODULE_TIMING_USING_HAL_SYSTICK_CALLBACK enables the use of that callback instead of SysTick_Handler
#define MODULE_TIMING_USING_HAL_SYSTICK_CALLBACK

#define SUBSYSTEM_UART 1

#define RECEIVE_STOP_CHAR '\n' // this is normally \r but the STM32 app uses \n so we can override to match

#define UART1_TX_BUFFER_LENGTH 1024
#define NUM_UART1_RECEIVERS 4
#define USE_UART1

#define TASK_MAX_LENGTH 50

#endif /* PROJECT_SETTINGS_H_ */

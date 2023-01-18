#ifndef __SYSTEM_HAL_H__
#define __SYSTEM_HAL_H__                    

#define FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE		4


enum nvm_object_id {
    ZERO_ID_INVALID_OBJECT_ID = 0,      ///< [REQUIRED] used to detect invalid objects
    NVM_OBJECT_ID_START = 1,            ///< [REQUIRED] used as the start id when searching for objects to repack
    NAME_OF_OBJECT_1 = 1,
    NAME_OF_OBJECT_2,
    NVM_OBJECT_ID_END,                  ///< [REQUIRED] used as the end id when searching for objects to repack
    NVM_OBJECT_MAX_VALUE = 0xFFFF       ///< [REQUIRED] this is very important to ensure the compiler make this a 16-bit enum
};

// enable anonymous structs in unions (like in u_control_mode in SpotTypes.h)
//#pragma anon_unions

#ifdef GARD_BOARD
#include "gard_board.h"
#else
#include "board.h"
#endif

#include "library.h"


#define BOOTLOADER


//#define FCPU CORE_CLOCK_FREQ

#define WATCHDOG_TIMEOUT_MS 2048

//#if defined CUBE_BOARD || defined GARD_BOARD
//#define SUBSYS_UART MCU_DEBUG_UART_INSTANCE
//#else
//// for SPOT/BOSS the debug UART might change based on HW_ID so it needs to be a variable
//#define SUBSYS_UART subsys_uart
//extern uint8_t subsys_uart;
//#endif


//#if MCU_DEBUG_UART_INSTANCE == 2
#define SUBSYS_UART 2
//#define USE_UART2
//#define UART2_TX_BUFFER_LENGTH 512
//#else
//#define USE_UART0
//#define UART0_TX_BUFFER_LENGTH 512
//#ifdef CPU_MKV56F512VLL24
//#define USE_UART3
//#define UART3_TX_BUFFER_LENGTH 512
//#define OVERRIDE_UART0_INTERRUPT
//extern void Overridden_UART0_RX_TX_IRQHandler(void);
//#endif
//#endif

#include "buffer.h"
#include "buffer_printf.h"
#include "list.h"
#include "subsys.h"
#include "timing.h"
#include "uart.h"
#include "task.h"
#include "charReceiverList.h"
#include "project_settings.h"

// version set in startup_bootloader_MKV31F25612.s near lines 329 using the first 
// two bytes of the backdoor comparisson key BackDoorK0 and BackDoorK1

#endif

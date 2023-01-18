/**
 * @version 4.0 Changes clock to 80MHz
 * @version 4.1 Makes error_code in telemetry messages = DCDC_ERROR_BOOTLOADER_MODE
 * @version 4.2 No functional changes, modified as bootloader_rev5 build
 * @version 4.3 Add support for Rev 6.1/BOSS (this version is the rev 5.x build)
 * @version 5.0 Switch to Modbus bootloader and increase bootloader size to 0x8000 (from 0x6000). Current build needs ~0x6600. Also user app vectors start at 0x8000 instead of 0x6000 now.
 * @version 5.1 Same as 5.0 but with specific hardware support for Rev 6.0
 * @version 5.2 Add support for Rev 6.1/BOSS
 * @version 5.3 Add diagnostic UART interface to assist in automated testing of firmware updates (current build size ~0x7300)
 * @version 5.4 Identical to 5.3, used for testing bootloader_loader
 * @version 5.5 Add support for baud rate other than 115200 (works with FW version 16+)
 * @version 5.6 Identical to 5.5, used for testing bootloader loader
 * @version 5.7 Fix baud rate issue in bootloader.
 * @version 5.8 board.h changed to make the gate drive disable work properly for BOSS and SPOT v6
 * @version 5.9 changed scatter file to support hard_fault_error reporting
 * @version 5.10 Add feature to copy Modbus blocks before writing so user can't change data while it is being written
 * @version 5.11 Extend uninitialized memory from 40 to 80 to support logging stackTrace
 * @version 5.12 Added support for MKV58 bootloader
 * @version 5.13 Don't erase firmware until firmware start command received. Allow firmware update to begin during the jump to app LED flash sequence.
 * @version 5.14 Adds support for DAB and AFD product codes.
 * @version 5.15 Fix issue causing HW ID 7, 16, and 25 to read as 8, 17, 26.
 * @version 5.16 Add Us timing hardware initialization to account for differences in processor clock speeds.
 * @version 5.17 Change to 512KB for MKV5. Add deinitialization for serial peripherals used in bootloader
 * @version 5.18 Add support for DCDC 7.2-MKV5 pinout
 * 
 * @todo consider renaming this file to main.c since it no longer has the bootloader in it.
 */
 
// system includes
#include "stdint.h"

//Board Drivers
#include "board.h"
#include "hardware.h"
#include "mcuid.h"
//#include "hal_fsl_wdog.h"
//#include "fsl_edma_driver.h"
//#include "fsl_uart_irq.h"

// bootloader
#include "bootloader.h"
#include "bootloader_app.h"

// embedded library
#include "timing.h"
#include "uart.h"

// modbus
#include "AlenconModbus.h"

// block size in words
#define BLOCK_SIZE 512 // 2 KB

// half period in ms of status LED
#define STATUS_LED_FLASH_PERIOD 500
#define BOOT_DELAY_PERIOD 500 

uint8_t dcdc_id, my_address;

// global variable:
uint8_t subsys_uart;

static uint8_t jump_to_app;

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

void JumpToUserApplication(uint32_t userSP)
{
	JumpAddress = *(__IO uint32_t*) (userSP + 4);			// here we jump to new address
	Jump_To_Application = (pFunction) JumpAddress;
	/* Initialize user application's Stack Pointer */
	__set_MSP(*(__IO uint32_t*) userSP);
	Jump_To_Application();
    while(1);
}

void Bootloader_DontLoadApp(void) {
    jump_to_app = 0;
}

// Deinitialize app modules to reinitialize in application (mainly to prevent bad interrupts)
void Bootloader_DeinitAppModules(void) {
	/* Items initialized in bootloader
		- Watchdog / leave initialized
		- Timing / leave initialized
		- Hardware / leave initialized
		- GPIO / leave initialized
		- UART Debug / needs to be deinitialized
		- UART Modbus / needs to be deinitialized
	*/
	hal_UART_Disable(MCU_DEBUG_UART_INSTANCE);
    AlenconModbus_Stop();
}

void JumpToAppStateMachine(uint8_t state) 
{
	static uint32_t status_led_time = 0;
	
	switch(state) 
	{
		case(0):
		{
			jump_to_app = 1;
			break;
		}
		case 1:
		{
			jump_to_app++;
			debugMSG(DWARNING, "jumping to Main App...\n");
			JumpToUserApplication(FLASH_APP_JUMP_ADDR);
			break;
		}
        case 2:
        {
            if(TimeSince(status_led_time) > 1000) {
                jump_to_app++;
                //HAL_WDOG_Refresh(WDOG);
                //STATUS_LED_OFF;
                status_led_time = TimeNow();
            }
            break;
		}
        case 3:
        {
            if(TimeSince(status_led_time) > 300) {
                jump_to_app++;
                //HAL_WDOG_Refresh(WDOG);
                //STATUS_LED_ON;
                status_led_time = TimeNow();
            }
            break;
		}
        case 4:
        {
            if(TimeSince(status_led_time) > 300) {
                jump_to_app++;
                //HAL_WDOG_Refresh(WDOG);
                //STATUS_LED_OFF;
                status_led_time = TimeNow();
            }
            break;
		}
        case 5:
        {
            if(TimeSince(status_led_time) > 300) {
                jump_to_app++;
                //HAL_WDOG_Refresh(WDOG);
                //STATUS_LED_ON;
                status_led_time = TimeNow();
            }
            break;
		}
        case 6:
        {
            if(TimeSince(status_led_time) > 1000) {
                jump_to_app++;
                //HAL_WDOG_Refresh(WDOG);
                //STATUS_LED_OFF;
                status_led_time = TimeNow();
            }
            break;
		}
        case 7:
        {
            if(TimeSince(status_led_time) > 300) {
                jump_to_app++;
                //HAL_WDOG_Refresh(WDOG);
                //STATUS_LED_ON;
                status_led_time = TimeNow();
                
				//debugMSG(DWARNING, "JUMPING TO MAIN APP\n");
            }
            break;
		}
        case 8:
        {
            if(TimeSince(status_led_time) > 300) {
                jump_to_app++;
                //HAL_WDOG_Refresh(WDOG);
                //STATUS_LED_OFF;
                status_led_time = TimeNow();
                // Restart the watchdog so it doesn't reset.
                //HAL_WDOG_Refresh(WDOG);
				Bootloader_DeinitAppModules();
				
                //JumpToUserApplication(FLASH_APP_JUMP_ADDR);
                
            }
            break;
		}
        default:
        {
            jump_to_app = 1;
            break;  
		} 
    }
}

void Bootloader_Init(void)
{
    //uint32_t status_led_time = 0;

    //// Init hardware
    Timing_Init();
    UART_Init(SUBSYSTEM_UART);
    hardware_init();
    
    
    //debugMSG(DWARNING, "FORCING jumping to Main App...\n");
	//JumpToUserApplication(0x60103000);
	//while(1);
    //STATUS_LED_INIT;//enable status LED
    //STATUS_LED_OFF;//status LED off by default

    //RS485RE_INIT;	//RS485 receive enable pin - used for flow control

    //GATE_ENABLE_INIT;

    //DCDC_GATE_DRIVE_DISABLE;//Make sure DCDC driver is intially in shutdown state
    
    //DCDC_GATE_DRIVER_POWER_INIT;
    //DCDC_GATE_DRIVER_POWER_DISABLE;

    //// UART0 is RS485 with MKV5 pinout and DEBUG for ther versions
    //// if not MKV5 pinout then the overridden embedded library interrupt must
    //// be called from the fsl_uart_irq.c interface
    //if(Hardware_HasMKV5Pinout()) {
        //subsys_uart = ALT_MCU_DEBUG_UART_INSTANCE;
    //}else {
        //subsys_uart = MCU_DEBUG_UART_INSTANCE;
        //#ifdef OVERRIDE_UART0_INTERRUPT
        //fsl_uart_callback_register(Overridden_UART0_RX_TX_IRQHandler, 0, true);
        //#endif
    //}
    //UART_Init(subsys_uart);
    
    AlenconModbus_Init();
    jump_to_app = AlenconModbus_InitBootloader();
    
    uint32_t cnt = 0;
    uint32_t status_led_time = TimeNow();
    
	debugMSG(0, "bootloader ready %d...\n",jump_to_app);
    while(1) 
    {
        if(jump_to_app) 
        {
            //Signal load into application for troubleshooting
            JumpToAppStateMachine(jump_to_app);
        }
        else 
        {
            // toggle the status LED every STATUS_LED_FLASH_PERIOD ms
            if(TimeSince(status_led_time) > STATUS_LED_FLASH_PERIOD) 
            {
                //STATUS_LED_TOGGLE;
                
				debugMSG(0, "CNT: %d\n",cnt++);
                status_led_time = TimeNow();
                // Restart the watchdog so it doesn't reset.
                //HAL_WDOG_Refresh(WDOG);
            }
        }
        Modbus_Routine();				// performs the init and as well 
        ModbusBootloader_Task();
        
        UART_Tick();					// mandatory in order to dequeue data from UART buffer
    }
}



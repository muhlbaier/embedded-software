#include "main.h"


#include "timing.h"
#include "uart.h"
#include "lpuart.h"

//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
// IRQDESIGN d.o.o
// Bosnia and Herzegovina
// 2022.11.08.
//
// RT1021
// CPU: 500 MHz
// 
//------------------------------------------------------------------------------
// NOTE:
//------------------------------------------------------------------------------
// GPIO_AD_B0_05 	- LED
// GPIO_AD_B0_06 	- UART1 Tx
// GPIO_AD_B0_07 	- UART1 Rx
// GPIO_AD_B0_10	- LPSPI1 SCK	
// GPIO_AD_B0_11	- LPSPI1 PCS0		
// GPIO_AD_B0_12	- LPSPI1 SDO	
// GPIO_AD_B0_13	- LPSPI1 SDI	
// GPIO_AD_B0_14	- FLEXPWM2_PWMA00
// GPIO_AD_B0_15	- FLEXPWM2_PWMB00
// GPIO_AD_B1_08	- QDC
// GPIO_AD_B1_09	- QDC
// GPIO_AD_B1_14	- SDA	
// GPIO_AD_B1_15	- SCL
//
// 0x00000000 64kB ITCM (default)
// 0x00200000 96kB ROM Bootloader (built in)
// 0x08000000 8MB Flex SPI ROM Alias
// 0x20000000 128kB DTCM (64kB is default)
// 0x20200000 64kB OCRAM (128kB is default)
// 0x60000000 64kB Alencon Bootloader
// 0x60010000 64kB Virtual EEPROM
// 0x60020000 128kB NVM Object Space
// 0x60040000 768kB Reserved
// 0x60100000 1MB Program Space 1
// 0x60200000 1MB Program Space 2
// 0x60300000 1MB Program Space 3 (3rd party users)
// 0x60400000 4MB Reserved

//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
typedef void (*pFunction)(void);

#define MAIN_APP_FW_RUN_ADDR				0x60102000


void runMainAppDFU(void);
int main(void)
{
	initSYSTEM();
	{// safety mechanism for not being able to halt the CPU if there is issue with bus/hardfault/init process
		
		// GPIO1 pin AD_B0_05 as digital output
		IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_05] = 0x00000005;
		IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_05] = 0x00000008;	// R0, 50MHz slow slew rate
		GPIO1->GDIR |= (1<<5);
	
		volatile uint8_t n;
		for(n=0;n<4;n++)
		{
			volatile uint32_t k;
			for(k=0;k<50000000;k++);
			
			GPIO1->DR_TOGGLE = (1<<5);
		}
	}
	
	initSYSTIM();
	initDEBUG("[BOOT]", '4', "Bootloader MIMXRT1021");
	
	//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	// Alencon LLC functions
	// 
	//--------------------------------------------------------------------------
	{
		Timing_Init();	
		UART_Init(SUBSYSTEM_UART);	
		UART_printf(SUBSYSTEM_UART, "System Initialized\r\n");
		
		// IS25LP064A-JBLE QSPI NOR 64Mbit
	}	
		
	debugMAIN(0, "system ready %d...\n",g_lpuart1_cnt);
	
	
	
	uint32_t timer = getSYSTIM();
	uint32_t cnt = 0;
    while (1)
    {
		if(chk4TimeoutSYSTIM(timer, 100) == (SYSTIM_TIMEOUT))
		{
			
			timer = getSYSTIM();
			debugMAIN(0, "CNT %d TimeNow(): %d\n", cnt++,TimeNow());
			UART_printf(SUBSYSTEM_UART, "UART2: %d\r\n",cnt);
			
			GPIO1->DR_TOGGLE = (1<<5);
		}
		if(cnt >= 2)
		{
			GPIO1->DR_TOGGLE = (1<<5);
			runMainAppDFU();
		}
    }
    
    return 0;
}


void runMainAppDFU(void)
{
	
	debugMAIN(DWARNING, "jumping to main app ADDR: %x \n", MAIN_APP_FW_RUN_ADDR);
	pFunction Jump_To_Application;
	//printUSART1("curr_fw_addr %x\n",DFU_CURR_FW_RUN_ADDR+4);
    uint32_t JumpAddress = *(__IO uint32_t*) ((MAIN_APP_FW_RUN_ADDR) + 4);
	//uint32_t JumpAddress = (__IO uint32_t*) ((DFU_CURR_FW_RUN_ADDR) + 0);            // here we jump to new address
	//printUSART1("jump address %x\n",(__IO uint32_t*) ((DFU_CURR_FW_RUN_ADDR) + 0));
    Jump_To_Application = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*) (MAIN_APP_FW_RUN_ADDR));
    Jump_To_Application();
    while(1);
}

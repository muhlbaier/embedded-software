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
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww

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
	initDEBUG("[CM7]", '4', "Alencon LLC MIMXRT1021 Hal Test");
	
	//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	// Alencon LLC functions
	//
	//--------------------------------------------------------------------------
	{
		Timing_Init();	
		UART_Init(SUBSYSTEM_UART);	
		UART_printf(SUBSYSTEM_UART, "System Initialized\r\n");
	}	
		
	debugMAIN(0, "system ready %d...\n",g_lpuart1_cnt);
	
	
	
	uint32_t timer = getSYSTIM();
	uint32_t cnt = 0;
    while (1)
    {
		if(chk4TimeoutSYSTIM(timer, 1000) == (SYSTIM_TIMEOUT))
		{
			
			timer = getSYSTIM();
			debugMAIN(0, "CNT %d TimeNow(): %d\n", cnt++,TimeNow());
			UART_printf(SUBSYSTEM_UART, "UART2: %d\r\n",cnt);
			
			GPIO1->DR_TOGGLE = (1<<5);
		}
    }
    
    return 0;
}

#include "main.h"
#include "systemc.h"

#include "timing.h"
#include "uart.h"
#include "lpuart.h"

#include "hal_eeprom.h"
#include "nvm_objects.h"
#include "eeprom.h"


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
// Bootloader is required for this example to run as application is sitting
// 0x60100000 address
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww


int main(void)
{	
	initSYSTEM();
	
	initSYSTIM();
	initDEBUG("APP1", '4', "Main app MIMXRT1021 Hal Test");
	IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_05] = 0x00000005;
	IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_05] = 0x00000008;	// R0, 50MHz slow slew rate
	GPIO1->GDIR |= (1<<5);
	
	
	debugMAIN(0, "system ready...\n");
	
	
	{
		debugMAIN(DWARNING, "EEPROM test\n");
		
		EEPROM_Init();
		
		uint32_t k;
		for(k=0;k<16;k++)
		{
			uint16_t val1, val2, val3;
			val1 = EEPROM_Read(k);
			EEPROM_Write(k,0xFFFF);
			val2 = EEPROM_Read(k);
			EEPROM_Write(k,val1*2);
			val3 = EEPROM_Read(k);
			debugMAIN(0, "ADDR: %xh -- %xh %xh %xh\n", k, val1, val2, val3);
		}
		debugMAIN(DNOTIFY, "EEPROM test completed\n");
		delay_ms(1000);
	}
	
	{
		uint16_t data[8] = {0};
		uint32_t k;
		uint16_t size;
		uint16_t * data_ptr;
		
		debugMAIN(DWARNING, "NVM OBJECTs test\n");
		
		data_ptr = (uint16_t *)NVM_Object_Get(NVM_OBJECT_ID_START, &size);
		debugMAIN(0, "NVM before erase OBJ: %d size: %d\n",NVM_OBJECT_ID_START, size);
		for(k=0;k<size;k++)
		{
			debugMAIN(0, "ADDR: %xh -- %xh\n", k, data_ptr[k]);
		}
		data[0] = data_ptr[0]+1;
		data[1] = data_ptr[1]+1;
		
		
		NVM_Object_EraseAll();
		
		data_ptr = (uint16_t *)NVM_Object_Get(NVM_OBJECT_ID_START, &size);
		debugMAIN(0, "NVM after erase OBJ: %d size: %d\n",NVM_OBJECT_ID_START, size);
		for(k=0;k<size;k++)
		{
			debugMAIN(0, "ADDR: %xh -- %xh\n", k, data_ptr[k]);
		}
		
		
		//data[0] = 0xA5B6;
		//data[1] = 0x3456;
		NVM_Object_Set(NVM_OBJECT_ID_START, 2, data) ;
		
		data_ptr = (uint16_t *)NVM_Object_Get(NVM_OBJECT_ID_START, &size);
		debugMAIN(0, "NVM after set OBJ: %d size: %d\n",NVM_OBJECT_ID_START, size);
		for(k=0;k<size;k++)
		{
			debugMAIN(0, "ADDR: %xh -- %xh\n", k, data_ptr[k]);
		}
		
		
		debugMAIN(DNOTIFY, "NVM OBJECTs test completed\n");
	}
	
	while(1);
	uint32_t timer = getSYSTIM();
	uint32_t cnt = 0;
    while (1)
    {
		if(chk4TimeoutSYSTIM(timer, 1000) == (SYSTIM_TIMEOUT))
		{
			
			timer = getSYSTIM();
			debugMAIN(0, "CNT %d TimeNow(): %d\n", cnt++,TimeNow());
			//UART_printf(SUBSYSTEM_UART, "UART2: %d\r\n",cnt);
			
			GPIO1->DR_TOGGLE = (1<<5);
		}
    }
    
    return 0;
}
//[29]APP1~SYS: system ready 0...
//[34]APP1~SYS: begin RAM copy BADDR: 2AE98568 AFAC4C81 ready 8...
//[43]APP1~SYS: 00000000 - 00008000 - 2AE98568
//[49]APP1~SYS: 00000001 - 00008004 - 12C366C1
//[56]APP1~SYS: 00000002 - 00008008 - 21A9239D
//[63]APP1~SYS: 00000003 - 0000800C - 886C7CBB
//[69]APP1~SYS: 00000004 - 00008010 - BDAF011A
//[76]APP1~SYS: 00000005 - 00008014 - 444B100C
//[82]APP1~SYS: 00000006 - 00008018 - 14567313
//[89]APP1~SYS: 00000007 - 0000801C - C97B66CF
//[96]APP1~SYS: 00000008 - 00008020 - EE28AEBB
//[102]APP1~SYS: 00000009 - 00008024 - 3B0D79D5

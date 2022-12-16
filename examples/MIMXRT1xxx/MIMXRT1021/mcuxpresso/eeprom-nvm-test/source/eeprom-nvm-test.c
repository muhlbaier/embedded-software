/*
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    MIMXRT1021_HAL_demo.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1021.h"
#include "fsl_debug_console.h"

#include "./hal/project_settings.h"
#include "./hal/hal_eeprom.h"
#include "./hal/nvm_objects.h"
#include "./hal/eeprom.h"


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

int main(void) {

	
	//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	// Alencon LLC functions
	//
	//--------------------------------------------------------------------------
	{
		Timing_Init();
		UART_Init(SUBSYSTEM_UART);
		UART_printf(SUBSYSTEM_UART, "\r\nMain App MIMXRT1021 Hal Test\r\n");
	}

	uint32_t cnt = 0;

	{
		UART_printf(SUBSYSTEM_UART,"EEPROM test\r\n");
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
			UART_printf(SUBSYSTEM_UART,"ADDR: %d -- %d %d %d\n\r", k, val1, val2, val3);
		}
		UART_printf(SUBSYSTEM_UART,"EEPROM test completed\n\r");
	}

	{
		uint16_t data[8] = {0};
		uint32_t k;
		uint16_t size;
		uint16_t * data_ptr;

		UART_printf(SUBSYSTEM_UART,"NVM OBJECTs test\n\r");
		data_ptr = (uint16_t *)NVM_Object_Get(NVM_OBJECT_ID_START, &size);
		UART_printf(SUBSYSTEM_UART,"NVM before erase OBJ: %d size: %d\n\r",NVM_OBJECT_ID_START, size);
		for(k=0;k<size;k++)
		{
			UART_printf(SUBSYSTEM_UART,"ADDR: %d -- %d\n\r", k, data_ptr[k]);
		}
		data[0] = data_ptr[0]+1;
		data[1] = data_ptr[1]+1;

		NVM_Object_EraseAll();

		data_ptr = (uint16_t *)NVM_Object_Get(NVM_OBJECT_ID_START, &size);
		UART_printf(SUBSYSTEM_UART,"NVM after erase OBJ: %d size: %d\n\r",NVM_OBJECT_ID_START, size);
		for(k=0;k<size;k++)
		{
			UART_printf(SUBSYSTEM_UART,"ADDR: %d -- %d\n\r", k, data_ptr[k]);
		}

		//data[0] = 0xA5B6;
		//data[1] = 0x3456;
		NVM_Object_Set(NVM_OBJECT_ID_START, 2, data) ;

		data_ptr = (uint16_t *)NVM_Object_Get(NVM_OBJECT_ID_START, &size);
		UART_printf(SUBSYSTEM_UART, "NVM after set OBJ: %d size: %d\n\r",NVM_OBJECT_ID_START, size);
		for(k=0;k<size;k++)
		{
			UART_printf(SUBSYSTEM_UART,"ADDR: %d -- %d\n\r", k, data_ptr[k]);
		}

		UART_printf(SUBSYSTEM_UART,"NVM OBJECTs test completed\n\r");
	}

	while (1)
	{
		UART_printf(SUBSYSTEM_UART, "UART2: %d\r\n",cnt++);
		GPIO1->DR_TOGGLE = (1<<5);
		DelayMs(100);
	}


    return 0 ;
}



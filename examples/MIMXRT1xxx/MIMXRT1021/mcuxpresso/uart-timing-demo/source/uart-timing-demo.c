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
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    //BOARD_InitBootPeripherals();

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

	//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	// Alencon LLC functions
	//
	//--------------------------------------------------------------------------
	{
		Timing_Init();
		UART_Init(SUBSYSTEM_UART);
		UART_printf(SUBSYSTEM_UART, "System Initialized\r\n");
	}

	uint32_t cnt = 0;
	while (1)
	{
		UART_printf(SUBSYSTEM_UART, "UART2: %d\r\n",cnt++);
		GPIO1->DR_TOGGLE = (1<<5);
		DelayMs(100);
	}


    return 0 ;
}

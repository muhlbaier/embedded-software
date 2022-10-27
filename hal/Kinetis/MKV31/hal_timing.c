#include <stdint.h>
#include <stdbool.h>

#include "project_settings.h"

/* Initializes a timer peripheral with a period
of 1ms. For Cortex-M cores we can use the built-in
SysTick interrupt
*/
void hal_Timing_Init(void){
    SysTick_Config(FCPU/1000);
	NVIC_SetPriority(SysTick_IRQn, 15);
}

void SysTick_Handler(void)
{
	TimingISR();
}

#define TimingUsNow() (1000 - (SysTick->VAL / (FCPU/1000000)))


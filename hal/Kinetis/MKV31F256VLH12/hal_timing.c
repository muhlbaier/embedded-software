#include <stdint.h>
#include <stdbool.h>

#include "project_settings.h"

/* Initializes a timer peripheral with a period
of 1ms. For Cortex-M cores we can use the built-in
SysTick interrupt
*/
void hal_Timing_Init(void){
  SysTick_Config(FCPU/1000);
	NVIC_SetPriority(SysTick_IRQn, 5);
}

void SysTick_Handler(void)
{
	TimingISR();
}

#define US_PER_TICK 1000
#define TimingUsNow() (time_ms*US_PER_TICK)


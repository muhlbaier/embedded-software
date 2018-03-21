#include <stdint.h>
#include <stdbool.h>

#include "project_settings.h"
#include "macros.h"


#ifndef TIMING_TIMER
#define TIMING_TIMER 1
#endif

#define TIMING_BASE CAT3(TIMER,TIMING_TIMER,_BASE)

void hal_timing_isr(void);

/* Initializes a timer peripheral with a period
of 1ms. For Cortex-M cores we can use the built-in
SysTick interrupt
*/
void hal_Timing_Init(void){
 SysTick_Config(FCPU/1000);
}


void SysTick_Handler (void){
		TimingISR();
}

#define US_PER_TICK 1000
#define TimingUsNow() (time_ms*US_PER_TICK)


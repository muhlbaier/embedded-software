#include <stdint.h>
#include <stdbool.h>

#include "project_settings.h"
#include "macros.h"
#include "hal_timing.h"
#include "timing.h"
//#include <sysclk.h>

/* Initializes a timer peripheral with a period
of 1ms. For Cortex-M cores we can use the built-in
SysTick interrupt
*/
void hal_Timing_Init(void)
{
    SysTick_Config(FCPU/1000);
}

#ifdef MODULE_TIMING_USING_HAL_SYSTICK_CALLBACK
void HAL_SYSTICK_Callback (void){
#else
void SysTick_Handler(void){
#endif
    TimingISR();
}



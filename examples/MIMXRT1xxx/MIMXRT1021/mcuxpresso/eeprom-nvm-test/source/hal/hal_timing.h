#ifndef __HAL_TIMING_H__
#define __HAL_TIMING_H__

#include <stdint.h>
#include <stdbool.h>

#include "mcuid.h"

#define US_PER_TICK 1000
#define TimingUsNow() (time_ms*US_PER_TICK)

void hal_Timing_Init(void);


#ifdef MODULE_TIMING_USING_HAL_SYSTICK_CALLBACK
void HAL_SYSTICK_Callback (void);
#else
void SysTick_Handler(void);
#endif


#endif

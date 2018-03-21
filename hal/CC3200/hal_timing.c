#ifndef _TIMING_H_
#error "Don't include this file in your project directly, it will be included by timing.c"
#endif
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/prcm.h"
#include "driverlib/timer.h"
#include "project_settings.h"
#include "macros.h"
#include "timing.h"

#ifndef TIMING_TIMER
#define TIMING_TIMER 1
#endif

#define TIMING_BASE CAT3(TIMERA,TIMING_TIMER,_BASE)

void hal_timing_isr(void);

void hal_Timing_Init(void){
	PRCMPeripheralClkEnable(CAT2(PRCM_TIMERA,TIMING_TIMER), PRCM_RUN_MODE_CLK);
	TimerLoadSet(TIMING_BASE, TIMER_A, PRCMPeripheralClockGet(CAT2(PRCM_TIMERA,TIMING_TIMER))/1000); //Set the period to 1ms
	TimerIntClear(TIMING_BASE, TIMER_TIMA_TIMEOUT);
	TimerConfigure(TIMING_BASE, (TIMER_CFG_PERIODIC_UP));
	TimerIntRegister(TIMING_BASE, TIMER_A, hal_timing_isr);
	TimerEnable(TIMING_BASE, TIMER_A);
	TimerIntEnable(TIMING_BASE, TIMER_TIMA_TIMEOUT);
}

void hal_timing_isr(void) {
	TimingISR();
	TimerIntClear(TIMING_BASE, TIMER_TIMA_TIMEOUT);
}

#define TICKS_PER_US (PRCMPeripheralClockGet(CAT2(PRCM_TIMERA,TIMING_TIMER)) / 1000000)
#define TimingUsNow()  TimerValueGet(TIMING_BASE, TIMER_A) / TICKS_PER_US


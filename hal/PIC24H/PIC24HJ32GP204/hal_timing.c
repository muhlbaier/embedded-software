#ifndef _TIMING_H_
#error "Don't add this file in your project directly, it will be included by timing.c"
#endif
#include <stdint.h>
#include "project_settings.h"
#include "macros.h"

// if peripheral clock not set default to FCPU
#ifndef PERIPHERAL_CLOCK
#define PERIPHERAL_CLOCK FCPU
#endif
// default priority is 2
#ifndef TIMING_INTERRUPT_PRIORITY
#define TIMING_INTERRUPT_PRIORITY 2
#endif
// default timer is number 2
#ifndef TIMING_NUMBER
#define TIMING_NUMBER 2
#endif

#if TIMING_NUMBER == 1
#define TIMING_IP IPC0bits.T1IP
#define TIMING_INT_NUM 0
#elif TIMING_NUMBER == 2
#define TIMING_IP IPC1bits.T2IP
#define TIMING_INT_NUM 0
#elif TIMING_NUMBER == 3
#define TIMING_IP IPC2bits.T3IP
#define TIMING_INT_NUM 0
#else
#warning "invalid TIMING_NUMBER"
#endif

#define TIMING_CON CAT3(T, TIMING_NUMBER, CON)
#define TIMING_PR CAT2(PR, TIMING_NUMBER)
#define TIMING_INTERRUPT_FLAG CAT5(IFS, TIMING_INT_NUM, bits.T, TIMING_NUMBER, IF)
#define TIMING_INTERRUPT_ENABLE CAT5(IEC, TIMING_INT_NUM, bits.T, TIMING_NUMBER, IE)
#define TIMING_INT_VECTOR CAT3(_T, TIMING_NUMBER, Interrupt)
#define TIMING_TIMER CAT2(TMR, TIMING_NUMBER)

#if TIMING_NUMBER == 1
#warning "not tested, check the datasheet if it doesn't work"
#endif

void hal_Timing_Init(void){
#if PERIPHERAL_CLOCK < 65000000
	TIMING_CON = 0;
	TIMING_PR = PERIPHERAL_CLOCK / 1000; //Set the period to 1ms
#else
    TIMING_CON = 0x0010; // 1:8 prescale
	TIMING_PR = PERIPHERAL_CLOCK / 8000; //Set the period to 1ms
#endif
	TIMING_INTERRUPT_FLAG = 0;  //Clear the interrupt flag
	TIMING_CON |= 0x8000; // turn timer on
	TIMING_IP = TIMING_INTERRUPT_PRIORITY;  // set priority to 2
	TIMING_INTERRUPT_ENABLE = 1;  //Enable the interrupt
}

void __attribute__((interrupt, auto_psv)) TIMING_INT_VECTOR(void) {
    TimingISR();
    TIMING_INTERRUPT_FLAG = 0;  //Clear the interrupt flag
}

#define TICKS_PER_US (PERIPHERAL_CLOCK / 1000000)
#define TimingUsNow() TIMING_TIMER / TICKS_PER_US

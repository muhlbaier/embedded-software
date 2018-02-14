#ifndef _TIMING_H_
#error "Don't add this file in your project directly, it will be included by timing.c"
#endif
#include <stdint.h>
#include "timing.h"
#include "hal_settings.h"
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

#define TIMING_CON CAT3(T, TIMING_NUMBER, CON)
#define TIMING_PR CAT2(PR, TIMING_NUMBER)
#define TIMING_IP CAT5(IPC, TIMING_NUMBER, bits.T, TIMING_NUMBER, IP)
#define TIMING_INT_MASK CAT3(_IEC0_T, TIMING_NUMBER, IE_MASK)
#define TIMING_INT_VECTOR CAT3(_TIMER_, TIMING_NUMBER, _VECTOR)

#if TIMING_NUMBER == 1
#warning "not tested, check the datasheet if it doesn't work"
#endif

void hal_Timing_Init(void){
	TIMING_CON = 0;
	TIMING_PR = PERIPHERAL_CLOCK / 1000; //Set the period to 1ms
	IFS0CLR = TIMING_INT_MASK;  //Clear the interrupt flag
	TIMING_CON |= 0x8000; // turn timer on
	TIMING_IP = TIMING_INTERRUPT_PRIORITY;  // set priority to 2
	IEC0SET = TIMING_INT_MASK;  //Enable the interrupt
}

void __attribute__((vector(TIMING_INT_VECTOR), interrupt(ipl1AUTO), nomips16)) TimingInterrupt(void) {
//void __ISR(TIMING_INT_VECTOR, ipl1AUTO) TimingInterrupt(void) {
    TimingISR();
    IFS0CLR = TIMING_INT_MASK;  //Clear the interrupt flag
}

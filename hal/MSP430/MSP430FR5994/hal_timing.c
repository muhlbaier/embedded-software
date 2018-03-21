#ifndef _TIMING_H_
#error "Don't include this file in your project directly, it will be included by timing.c"
#endif
#include <stdint.h>
#include <msp430.h>
#include "timing.h"
#include "project_settings.h"

void hal_Timing_Init(void){
    TA0CCR0 = FCPU/1000;    //Set the period to 1ms
    TA0CTL &= ~TAIFG;   //Clear the interrupt flag
    TA0CTL |= TASSEL_2 | MC_1 | TACLR; //Set SMCLK, UP Mode
    TA0CCTL0 |= CCIE;
}

#ifdef MSPGCC
__attribute__((interrupt(TIMER0_A0_VECTOR)))
void TIMER0_A0_ISR(void) {
#else
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void){
#endif
    TimingISR();
    TA0CTL &= ~TAIFG;   //Clear the interrupt flag
}

#define TICKS_PER_US (FCPU / 1000000)
#define TimingUsNow() TA0R / TICKS_PER_US


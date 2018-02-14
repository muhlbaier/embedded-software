#ifndef _TIMING_H_
#error "Don't include this file in your project directly, it will be included by timing.c"
#endif

#include <msp432.h>

void hal_Timing_Init(void)
{
    TIMER_A0->CCR[0] = FCPU/1000; //Set the period to 1ms
    TIMER_A0->CTL &= ~0x03F7; //Clear all non-reserved settings bits
    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC_1 | TIMER_A_CTL_CLR; //Source = SMCLK, Up mode
    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG; //Clear interrupt flag
    TIMER_A0->CTL |= TIMER_A_CTL_IE;
//    TIMER_A0->CCTL[0] = 0xF9F7; //Clear all non-reserved settings bits
//    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; //Enable interrupt for CCR0

    NVIC_EnableIRQ(TA0_N_IRQn); //Enable gobal timer A CCR0 interrupts
}

void TA0_N_IRQHandler(void)
{
    if (TIMER_A0->CTL & TIMER_A_CTL_IFG) {
        TimingISR();
        TIMER_A0->CTL &= ~TIMER_A_CTL_IFG; //Clear interrupt flag
    }
}

#define TICKS_PER_US (FCPU / 1000000)
#define TimingUsNow() TIMER_A0->R / TICKS_PER_US


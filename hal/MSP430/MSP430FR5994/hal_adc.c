/*
 * hal_adc.c
 *
 *  Created on: Mar 3, 2019
 *      Author: Lonnie Souder II
 */

#include <msp430fr5994.h>
#include "hal_adc.h"
#include "adc.h"
#include <stdint.h>

void hal_ADC_Init(void)
{
    // By default, REFMSTR=1 => REFCTL is used to configure the internal reference
    while(REFCTL0 & REFGENBUSY);            // If ref generator busy, WAIT
    REFCTL0 |= REFVSEL_3 | REFON;           // Select internal ref = 2.5V
                                            // Internal Reference ON

    // Configure ADC12
    ADC12CTL0 = ADC12SHT0_3 | ADC12ON;
    ADC12CTL1 = ADC12SHP;                   // Pulse mode
    ADC12CTL2 |= ADC12RES_2;                // 12-bit conversion results
    ADC12MCTL0 |= ADC12VRSEL_3;             // 2.5V ref
    ADC12IER0 |= ADC12IE0;

    while(!(REFCTL0 & REFGENRDY));          // Wait for reference generator
                                            // to settle
}

void hal_ADC_StartChannel(uint8_t channel)
{
    ADC12MCTL0 |= ADC12INCH_0 + channel;  // ADC input select
    ADC12CTL0 |= ADC12SC | ADC12ENC;      // Sampling and conversion start
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_B_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_B_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch (__even_in_range(ADC12IV, ADC12IV__ADC12RDYIFG))
    {
        case ADC12IV__NONE:        break;   // Vector  0:  No interrupt
        case ADC12IV__ADC12OVIFG:  break;   // Vector  2:  ADC12MEMx Overflow
        case ADC12IV__ADC12TOVIFG: break;   // Vector  4:  Conversion time overflow
        case ADC12IV__ADC12HIIFG:  break;   // Vector  6:  ADC12BHI
        case ADC12IV__ADC12LOIFG:  break;   // Vector  8:  ADC12BLO
        case ADC12IV__ADC12INIFG:  break;   // Vector 10:  ADC12BIN
        case ADC12IV__ADC12IFG0:            // Vector 12:  ADC12MEM0 Interrupt
            ADC_ProcessMeasurementFromISR(ADC12MEM0); break;
        case ADC12IV__ADC12RDYIFG: break;   // Vector 76:  ADC12RDY
        default: break;
    }
}

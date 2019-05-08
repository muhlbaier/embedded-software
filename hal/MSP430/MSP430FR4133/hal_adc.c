
/*
 * hal_adc.c
 *
 *  Created on: Mar 26, 2019
 *      Author: Daniel Morano
 */

#include <msp430.h>
#include <stdint.h>
#include "adc.h"

void hal_ADC_Init(void)
{
	/* Configure ADC A1 pin */
	SYSCFG2 |= ADCPCTL1;                                     

    /* Configure ADC10 */
    ADCCTL0 |= ADCSHT_2 | ADCON;                             // ADCON, S&H=16 ADC clks
    ADCCTL1 |= ADCSHP;                                       // ADCCLK = MODOSC; sampling timer
    ADCCTL2 |= ADCRES;                                       // 10-bit conversion results
    ADCIE |= ADCIE0;                                         // Enable ADC conv complete interrupt      
}

void hal_ADC_StartChannel(uint8_t channel)
{
	/* ADC input select using the selected channel */
	ADCMCTL0 |= ADCINCH_1 + channel;  
	
	/* Sampling and conversion start */
    ADCCTL0 |= ADCENC | ADCSC;  
}


/* ADC interrupt service routine */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC_VECTOR))) ADC_ISR (void)
#else
#error Compiler not supported!
#endif
{
	/* Switch Case for various ADC Interrupt flags */
    switch(__even_in_range(ADCIV,ADCIV_ADCIFG))
    {
        case ADCIV_NONE:
            break;
        case ADCIV_ADCOVIFG:
            break;
        case ADCIV_ADCTOVIFG:
            break;
        case ADCIV_ADCHIIFG:
            break;
        case ADCIV_ADCLOIFG:
            break;
        case ADCIV_ADCINIFG:
            break;
        case ADCIV_ADCIFG:									 
            ADC_ProcessMeasurementFromISR(ADCMEM0);  		 // Function found in "adc.h" that needs to be called in interrupt to process measurement
            break;
        default:
            break;
    }  
}

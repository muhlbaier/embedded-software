#include "buzzer.h"
#include "hal_general.h"
#include "project_settings.h" 
#include <stdint.h>


 
void hal_Buzzer_Init(void){
	// Timer Config
    TA0CTL = TASSEL_1 + ID_0 + MC_1 + TACLR;    // SMCLK, up mode, clear TAR

    // Buzzer pin set to P1.2
    P1DIR |= BIT2;               				// P1.2 output
    P1SEL |= BIT2;               				// P1.2 options select
    TA0CCTL1 = OUTMOD_7;                        // CCR1 reset/set
    TA0CCR1 = 0;                                // CCR1 PWM duty cycle
}
 
void hal_Start_Buzzer(uint16_t frequency){
	if (frequency < 2048) {
        TA0CTL = TASSEL_1 + ID_0 + MC_1 + TACLR;    // ACLK, up mode, clear TAR
        TA0CCR0 = 32768 / frequency;
    } else {
        TA0CTL = TASSEL_2 + ID_0 + MC_1 + TACLR;    // SMCLK, up mode, clear TAR
        TA0CCR0 = FCPU / frequency;
    }
    TA0CCR1 = TA0CCR0 >> 1;
}

void hal_Stop_Buzzer(void){
	TA0CCR1 = 0; 
}

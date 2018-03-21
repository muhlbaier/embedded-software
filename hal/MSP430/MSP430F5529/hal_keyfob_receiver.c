#include "project_settings.h"
#include "hal_keyfob_receiver.h"

#ifndef KEYFOB_SIGNAL_PIN
#define KEYFOB_SIGNAL_PIN BIT2
#endif

void hal_Keyfob_Init(void) {
	// Init digital interrupt
    P1IE |= KEYFOB_SIGNAL_PIN ; 		// Interrupt on Input Pin P1.2
    P1IES |= KEYFOB_SIGNAL_PIN; 		// High to Low Edge

    // Init timer
    TA1CTL = TASSEL_2 + MC_2 + TACLR;	// SMCLK, contmode, clear TAR
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) PORT1_ISR (void)
#else
#error Compiler not supported!
#endif
{
	static uint16_t prev_count = 0;
	Keyfob_ProcessPulse(TA1R-prev_count);
	prev_count = TA1R;

	P1IES ^= KEYFOB_SIGNAL_PIN ; 		// Toggle Edge sensitivity
	P1IFG &= ~KEYFOB_SIGNAL_PIN; 		// Clear Interrupt Flag
}

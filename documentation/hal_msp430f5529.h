/**
 * @defgroup hal_msp430f5529 MSP430F5529 Documentation
 * @ingroup hal_doc
 *
 * All defines are optional (i.e. have default values) except for FCPU. Defines
 * should be located in system.h.
 *
 * @{
 */
 
 /**
 * @ingroup hal_clock
 * @defgroup hal_clock_msp430f5529 Configureation for MSP430F5529
 *
 * The default configuration for the MSP430F5529 is to use the internal DCO
 * to gererate a nominal clock speed of ~1.045MHz.
 *
 * Below is an example for setting the DCO to 25MHz (the max speed for this
 * MCU).
 *
@code
void SetVcoreUp (unsigned int level);
//...
//the following would typically be located at the top of main

// Increase Vcore setting to level3 to support fsystem=25MHz
// NOTE: Change core voltage one level at a time..
SetVcoreUp (0x01);
SetVcoreUp (0x02);
SetVcoreUp (0x03);

UCSCTL3 = SELREF_2;                       // Set DCO FLL reference = REFO
UCSCTL4 |= SELA_2;                        // Set ACLK = REFO

__bis_SR_register(SCG0);                  // Disable the FLL control loop
UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
UCSCTL1 = DCORSEL_7;                      // Select DCO range 50MHz operation
UCSCTL2 = FLLD_0 + 762;                   // Set DCO Multiplier for 25MHz
										// (N + 1) * FLLRef = Fdco
										// (762 + 1) * 32768 = 25MHz
										// Set FLL Div = fDCOCLK/2
__bic_SR_register(SCG0);                  // Enable the FLL control loop

// Worst-case settling time for the DCO when the DCO range bits have been
// changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
// UG for optimization.
// 32 x 32 x 25 MHz / 32,768 Hz ~ 780k MCLK cycles for DCO to settle
__delay_cycles(782000);

// Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
do
{
UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
										// Clear XT2,XT1,DCO fault flags
SFRIFG1 &= ~OFIFG;                      // Clear fault flags
}while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
//...
//somewhere else SetVcoreUp must be included
void SetVcoreUp (unsigned int level)
{
  // Open PMM registers for write
  PMMCTL0_H = PMMPW_H;
  // Set SVS/SVM high side new level
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
  // Set SVM low side to new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
  // Wait till SVM is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait till new level reached
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set SVS/SVM low side to new level
  SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
}
@endcode
*/

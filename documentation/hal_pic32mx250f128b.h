/**
 * @defgroup hal_pic32mx250f128b PIC32MX250F128B Documentation
 * @ingroup hal_doc
 *
 * All defines are optional (i.e. have default values) except for FCPU. Defines
 * should be located in system.h.
 *
 * @{
 */

/** @brief Timer to use for Timing Module @ref timing
 *
 * NOT REQUIRED
 *
 * Timer number to use for the Timing Module. Default is Timer 2. Possible
 * values are (1), 2-5
 *
 * @warning May not work on Timer 1 since Timer 1 functions different than the
 * other timers. Should be tested if Timer 1 is used and hal_timing.c may need
 * modification.
 */
#define TIMING_NUMBER 2

#define UART_INTERRUPT_PRIORITY 3 ///< default for UART interrupt priority (valid values are 1-7)

#define UART1_TX_PIN RPA0 ///< default for UART2_TX_PIN
#define UART1_RX_PIN RPA2 ///< default for UART2_TX_PIN
// UART2 is connected to the pins used on the Microstick II header
#define UART2_TX_PIN RPB10 ///< default for UART2_TX_PIN
#define UART2_RX_PIN RPB11 ///< default for UART2_RX_PIN

/**
 * @ingroup hal_clock
 * @defgroup hal_clock_pic32mx250f128b Configureation for PIC32MX250F128B
 *
 * Default clock configuration on the PIC32MX250F128B is done via configuration
 * bits. Configuration bits can be genrated using the MPLAB X tool:
 * Window -> PIC Memory Views -> Configuration bits.
 *
 * Only FRC and FRCPLL clock configurations will be discussed. Refer to the
 * device datasheet for further details.
 *
 * FNOSC sets the primary default clock source some possible settings are:
 * @code
 * #pragma config FNOSC = FRC              // Oscillator Selection Bits (Fast RC Osc without PLL)
 * or
 * #pragma config FNOSC = FRCPLL           // Oscillator Selection Bits (Fast RC Osc with PLL)
 * @endcode
 *
 * FRC will generate a clock speed of 8MHz. FRCPLL will use the PLL block to
 * generate a different clokc speed.
 *
 * When using PLL the FPLLIDIV, FPLLMUL, and FPLLODIV will determine the clock
 * speed output of the PLL. The equation is:
 * FCPU = ((8MHz / FPLLIDIV) * FPLLMUL) / FPLLODIV
 *
 * Since the input frequency to the PLL should be around 4MHz a FPLLIDIV of 2 is
 * appropriate. Thus to generate a 16MHz clock for example the following
 * settings could be used:
 *
 * @code
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_16         // PLL Multiplier (16x Multiplier)
#pragma config FPLLODIV = DIV_4         // System PLL Output Clock Divider (PLL Divide by 4)
 * @endcode
 */

/** @} */

#ifndef HAL_GENERAL_
#define HAL_GENERAL_
#include <xc.h>
#include <stdint.h>
// Nop() is defined by the C32 and XC32 compilers and is not needed here
// use XC32 enable and disable interrupt routines
#define DisableInterrupts() INTDisableInterrupts()
#define EnableInterrupts() INTCONSET = _INTCON_MVEC_MASK; __builtin_enable_interrupts()
#define BlockInterrupts() unsigned int intStatus; \
					      intStatus = __builtin_disable_interrupts()
#define RestoreInterrupts() if(intStatus&1) __builtin_enable_interrupts()
#define hal_Watchdog_Clear() (WDTCONSET = _WDTCON_WDTCLR_MASK)
#define hal_Watchdog_Enable() (WDTCONSET = _WDTCON_ON_MASK)

// copied here to relieve dependence on plib
#define IN_PIN_PPS1_RPD2	0
#define IN_PIN_PPS1_RPG8	1
#define IN_PIN_PPS1_RPF4	2
#define IN_PIN_PPS1_RPD10	3
#define IN_PIN_PPS1_RPF1	4
#define IN_PIN_PPS1_RPB9	5
#define IN_PIN_PPS1_RPB10	6
#define IN_PIN_PPS1_RPC14	7
#define IN_PIN_PPS1_RPB5	8
#define IN_PIN_PPS1_RPF2	15
#define IN_PIN_PPS2_RPD3	0
#define IN_PIN_PPS2_RPG7	1
#define IN_PIN_PPS2_RPF5	2
#define IN_PIN_PPS2_RPD11	3
#define IN_PIN_PPS2_RPF0	4
#define IN_PIN_PPS2_RPB1	5
#define IN_PIN_PPS2_RPE5	6
#define IN_PIN_PPS2_RPC13	7
#define IN_PIN_PPS2_RPB3	8
#define IN_PIN_PPS2_RPF2	14
#define IN_PIN_PPS3_RPD9	0
#define IN_PIN_PPS3_RPG6	1
#define IN_PIN_PPS3_RPB8	2
#define IN_PIN_PPS3_RPB15	3
#define IN_PIN_PPS3_RPD4	4
#define IN_PIN_PPS3_RPB0	5
#define IN_PIN_PPS3_RPE3	6
#define IN_PIN_PPS3_RPB7	7
#define IN_PIN_PPS3_RPB2	15
#define IN_PIN_PPS4_RPD1	0
#define IN_PIN_PPS4_RPG9	1
#define IN_PIN_PPS4_RPB14	2
#define IN_PIN_PPS4_RPD0	3
#define IN_PIN_PPS4_RPD8	4
#define IN_PIN_PPS4_RPB6	5
#define IN_PIN_PPS4_RPD5	6
#define IN_PIN_PPS4_RPB2	7
#define IN_PIN_PPS4_RPF2	11


typedef struct {
    uint8_t crap_bit:1;
} crap_t;
extern crap_t crap;
// helper macros for GPIO related stuff
#define SetPinAsDigital(pin) CAT2(_ANS, pin) = 0
#define SetPinAsAnalog(pin) CAT2(_ANS, pin) = 1
#define SetPinAsInput(pin) CAT2(_TRIS, pin) = 1
#define SetPinAsOutput(pin) CAT2(_TRIS, pin) = 0

// ANSEL bit defines
#define _ANSA2 crap.crap_bit
#define _ANSA3 crap.crap_bit
#define _ANSA4 crap.crap_bit
#define _ANSB0 ANSELBbits.ANSB0
#define _ANSB1 ANSELBbits.ANSB1
#define _ANSB2 ANSELBbits.ANSB2
#define _ANSB3 ANSELBbits.ANSB3
#define _ANSB4 ANSELBbits.ANSB4
#define _ANSB5 ANSELBbits.ANSB5
#define _ANSB6 ANSELBbits.ANSB6
#define _ANSB7 ANSELBbits.ANSB7
#define _ANSB8 ANSELBbits.ANSB8
#define _ANSB9 ANSELBbits.ANSB9
#define _ANSB10 ANSELBbits.ANSB10
#define _ANSB11 ANSELBbits.ANSB11
#define _ANSB12 ANSELBbits.ANSB12
#define _ANSB13 ANSELBbits.ANSB13
#define _ANSB14 ANSELBbits.ANSB14
#define _ANSB15 ANSELBbits.ANSB15
#define _ANSD1 ANSELDbits.ANSD1
#define _ANSD2 ANSELDbits.ANSD2
#define _ANSD3 ANSELDbits.ANSD3
#define _ANSE2 ANSELEbits.ANSE2
#define _ANSE4 ANSELEbits.ANSE4
#define _ANSE5 ANSELEbits.ANSE5
#define _ANSE6 ANSELEbits.ANSE6
#define _ANSE7 ANSELEbits.ANSE7
#define _ANSG6 ANSELGbits.ANSG6
#define _ANSG7 ANSELGbits.ANSG7
#define _ANSG8 ANSELGbits.ANSG8
#define _ANSG9 ANSELGbits.ANSG9


// TRIS, PORT, LAT bit defines in device header provided with compiler

#endif // HAL_GENERAL_

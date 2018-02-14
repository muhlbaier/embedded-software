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

// copied here to relieve dependence on plib
#define IN_PIN_PPS3_RPA2         0   /* Assign RPA2  as Input Pin */
#define IN_PIN_PPS3_RPB6         1   /* Assign RPB6  as Input Pin */
#define IN_PIN_PPS3_RPA4         2   /* Assign RPA4  as Input Pin */
#define IN_PIN_PPS3_RPB13        3   /* Assign RPB13 as Input Pin */
#define IN_PIN_PPS3_RPB2         4   /* Assign RPB2  as Input Pin */
#define IN_PIN_PPS3_RPC6         5   /* Assign RPC6  as Input Pin */
#define IN_PIN_PPS3_RPC1         6   /* Assign RPC1  as Input Pin */
#define IN_PIN_PPS3_RPC3         7   /* Assign RPC3  as Input Pin */
#define IN_PIN_PPS2_RPA1         0   /* Assign RPA1  as Input Pin */
#define IN_PIN_PPS2_RPB5         1   /* Assign RPB5  as Input Pin */
#define IN_PIN_PPS2_RPB1         2   /* Assign RPB1  as Input Pin */
#define IN_PIN_PPS2_RPB11        3   /* Assign RPB11 as Input Pin */
#define IN_PIN_PPS2_RPB8         4   /* Assign RPB8  as Input Pin */
#define IN_PIN_PPS2_RPA8         5   /* Assign RPA8  as Input Pin */
#define IN_PIN_PPS2_RPC8         6   /* Assign RPC8  as Input Pin */
#define IN_PIN_PPS2_RPA9         7   /* Assign RPA9  as Input Pin */

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
#define _ANSA0 ANSELAbits.ANSA0
#define _ANSA1 ANSELAbits.ANSA1
#define _ANSA2 crap.crap_bit
#define _ANSA3 crap.crap_bit
#define _ANSA4 crap.crap_bit
#define _ANSB0 ANSELBbits.ANSB0
#define _ANSB1 ANSELBbits.ANSB1
#define _ANSB2 ANSELBbits.ANSB2
#define _ANSB3 ANSELBbits.ANSB3
#define _ANSB4 crap.crap_bit
#define _ANSB5 crap.crap_bit
#define _ANSB7 crap.crap_bit
#define _ANSB8 crap.crap_bit
#define _ANSB9 crap.crap_bit
#define _ANSB10 crap.crap_bit
#define _ANSB11 crap.crap_bit
#define _ANSB13 ANSELBbits.ANSB13
#define _ANSB14 ANSELBbits.ANSB14
#define _ANSB15 ANSELBbits.ANSB15
// TRIS, PORT, LAT bit defines in device header provided with compiler

#endif // HAL_GENERAL_

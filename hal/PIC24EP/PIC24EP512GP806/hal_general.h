#ifndef HAL_GENERAL_
#define HAL_GENERAL_
#include <xc.h>
#include <stdint.h>
// Nop() is defined by the XC16 compiler and is not needed here
// use XC16 enable and disable interrupt routines
#define DisableInterrupts() SET_CPU_IPL(7)
#define EnableInterrupts() RESTORE_CPU_IPL(0)
#define BlockInterrupts() int current_ipl; SET_AND_SAVE_CPU_IPL(current_ipl, 7)
#define RestoreInterrupts() RESTORE_CPU_IPL(current_ipl)
#define hal_Watchdog_Clear() ClrWdt()
#define hal_Watchdog_Enable() #warning "implement this"

#define OUTPUT_PIN_TO_REG_NUM(pin) CAT2(OUTPUT_PIN_TO_REG_NUM_PIN, pin)
#define OUTPUT_PIN_TO_REG_NUM_PIN64 0
#define OUTPUT_PIN_TO_REG_NUM_PIN65 0
#define OUTPUT_PIN_TO_REG_NUM_PIN66 1
#define OUTPUT_PIN_TO_REG_NUM_PIN67 1
#define OUTPUT_PIN_TO_REG_NUM_PIN68 2
#define OUTPUT_PIN_TO_REG_NUM_PIN69 2
#define OUTPUT_PIN_TO_REG_NUM_PIN70 3
#define OUTPUT_PIN_TO_REG_NUM_PIN71 3
#define OUTPUT_PIN_TO_REG_NUM_PIN79 4
#define OUTPUT_PIN_TO_REG_NUM_PIN80 4
#define OUTPUT_PIN_TO_REG_NUM_PIN82 5
#define OUTPUT_PIN_TO_REG_NUM_PIN84 5
#define OUTPUT_PIN_TO_REG_NUM_PIN85 6
#define OUTPUT_PIN_TO_REG_NUM_PIN87 6
#define OUTPUT_PIN_TO_REG_NUM_PIN96 7
#define OUTPUT_PIN_TO_REG_NUM_PIN97 7
#define OUTPUT_PIN_TO_REG_NUM_PIN98 8
#define OUTPUT_PIN_TO_REG_NUM_PIN99 8
#define OUTPUT_PIN_TO_REG_NUM_PIN100 9
#define OUTPUT_PIN_TO_REG_NUM_PIN101 9
#define OUTPUT_PIN_TO_REG_NUM_PIN102 10
#define OUTPUT_PIN_TO_REG_NUM_PIN104 11
#define OUTPUT_PIN_TO_REG_NUM_PIN108 11
#define OUTPUT_PIN_TO_REG_NUM_PIN109 12
#define OUTPUT_PIN_TO_REG_NUM_PIN112 12
#define OUTPUT_PIN_TO_REG_NUM_PIN113 13
#define OUTPUT_PIN_TO_REG_NUM_PIN118 13
#define OUTPUT_PIN_TO_REG_NUM_PIN120 14
#define OUTPUT_PIN_TO_REG_NUM_PIN125 14
#define OUTPUT_PIN_TO_REG_NUM_PIN126 15
#define OUTPUT_PIN_TO_REG_NUM_PIN127 15

#define SetPPS_Out(pin) CAT5(RPOR, OUTPUT_PIN_TO_REG_NUM(pin), bits.RP, pin, R) 

#endif // HAL_GENERAL_

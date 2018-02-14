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

#define OUTPUT_PIN_TO_REG_NUM(pin) CAT2(OUTPUT_PIN_TO_REG_NUM_PIN, pin)
#define OUTPUT_PIN_TO_REG_NUM_PIN0 0
#define OUTPUT_PIN_TO_REG_NUM_PIN1 0
#define OUTPUT_PIN_TO_REG_NUM_PIN2 1
#define OUTPUT_PIN_TO_REG_NUM_PIN3 1
#define OUTPUT_PIN_TO_REG_NUM_PIN4 2
#define OUTPUT_PIN_TO_REG_NUM_PIN5 2
#define OUTPUT_PIN_TO_REG_NUM_PIN6 3
#define OUTPUT_PIN_TO_REG_NUM_PIN7 3
#define OUTPUT_PIN_TO_REG_NUM_PIN8 4
#define OUTPUT_PIN_TO_REG_NUM_PIN9 4
#define OUTPUT_PIN_TO_REG_NUM_PIN10 5
#define OUTPUT_PIN_TO_REG_NUM_PIN11 5
#define OUTPUT_PIN_TO_REG_NUM_PIN12 6
#define OUTPUT_PIN_TO_REG_NUM_PIN13 6
#define OUTPUT_PIN_TO_REG_NUM_PIN14 7
#define OUTPUT_PIN_TO_REG_NUM_PIN15 7
#define OUTPUT_PIN_TO_REG_NUM_PIN16 8
#define OUTPUT_PIN_TO_REG_NUM_PIN17 8
#define OUTPUT_PIN_TO_REG_NUM_PIN18 9
#define OUTPUT_PIN_TO_REG_NUM_PIN19 9
#define OUTPUT_PIN_TO_REG_NUM_PIN20 10
#define OUTPUT_PIN_TO_REG_NUM_PIN20 10
#define OUTPUT_PIN_TO_REG_NUM_PIN21 11
#define OUTPUT_PIN_TO_REG_NUM_PIN21 11
#define OUTPUT_PIN_TO_REG_NUM_PIN22 12
#define OUTPUT_PIN_TO_REG_NUM_PIN22 12
#endif // HAL_GENERAL_

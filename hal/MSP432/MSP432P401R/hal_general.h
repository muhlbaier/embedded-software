#ifndef _HAL_GENERAL_
#define _HAL_GENERAL_

#include <msp432.h>

#define Nop() __asm("  nop")

#define DisableInterrupts() __disable_interrupts()
#define EnableInterrupts() __enable_interrupts()

#define BlockInterrupts() DisableInterrupts() //NVIC handles masking individual interrupts
#define RestoreInterrupts() EnableInterrupts()//NVIC handles masking individual interrupts

#define Reset() PMMCTL0 |= PMMSWPOR

#endif // _HAL_GENERAL_

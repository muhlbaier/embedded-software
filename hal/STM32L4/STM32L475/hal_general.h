#ifndef _HAL_GENERAL_
#define _HAL_GENERAL_

#include <stdint.h>
#include <stdbool.h>
#include "stm32l4xx.h"

#define Nop() 	__asm("        nop\n");
#define DisableInterrupts() __disable_irq()
#define EnableInterrupts() __enable_irq()

#define BlockInterrupts() int prim; prim = __get_PRIMASK(); __disable_irq()
#define RestoreInterrupts() if(!prim) __enable_irq()

#endif // _HAL_GENERAL_

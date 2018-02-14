#ifndef _HAL_GENERAL_
#define _HAL_GENERAL_

#include <stdint.h>
#include <stdbool.h>
#include "chip.h"

#define Nop() 	__asm("        nop\n");
#define DisableInterrupts() __disable_irq()
#define EnableInterrupts() __enable_irq()

#define BlockInterrupts() __disable_irq()
#define RestoreInterrupts() __enable_irq()

#endif // _HAL_GENERAL_

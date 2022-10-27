#ifndef _HAL_GENERAL_
#define _HAL_GENERAL_

#include <stdint.h>
#include <stdbool.h>
#include "fsl_interrupt_manager.h"

#define Nop() 	__asm("        nop\n");
#define DisableInterrupts() __disable_irq()
#define EnableInterrupts() __enable_irq()

#define BlockInterrupts() INT_SYS_DisableIRQGlobal()
#define RestoreInterrupts() INT_SYS_EnableIRQGlobal()

#endif // _HAL_GENERAL_

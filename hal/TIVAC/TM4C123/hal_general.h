#ifndef _HAL_GENERAL_
#define _HAL_GENERAL_

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/interrupt.h"

#define Nop() 	__asm("        nop\n");
#define DisableInterrupts() IntMasterDisable()
#define EnableInterrupts() IntMasterEnable()
#warning "TODO - Make sure Tiva C block and restore makes sense"
#define BlockInterrupts() IntMasterDisable()
#define RestoreInterrupts() IntMasterEnable()

#endif // _HAL_GENERAL_

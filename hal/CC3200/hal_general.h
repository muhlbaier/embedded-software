#ifndef _HAL_GENERAL_
#define _HAL_GENERAL_

#include <stdint.h>
#include <stdbool.h>
#include "hw_types.h"
#include "driverlib/interrupt.h"
#include "hw_memmap.h"

#include "hal_pin_macros.h"

#define Nop() 	__asm("        nop\n");
#define DisableInterrupts() IntMasterDisable()
#define EnableInterrupts() IntMasterEnable()

#define BlockInterrupts() tBoolean intStatus = IntMasterDisable()

#define RestoreInterrupts() if(!intStatus) IntMasterEnable()

#endif // _HAL_GENERAL_

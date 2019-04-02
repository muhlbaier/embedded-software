#ifndef _HAL_GENERAL_
#define _HAL_GENERAL_

#include <msp430.h>
#include "project_settings.h"

#define Nop() _NOP()

#define DisableInterrupts() _DINT()
#define EnableInterrupts() _EINT()
#define BlockInterrupts() uint8_t intSta = (_get_SR_register() & GIE); DisableInterrupts()
#define RestoreInterrupts() if(intSta){EnableInterrupts();}

#define Reset() PMMCTL0 |= PMMSWPOR

#endif // _HAL_GENERAL_

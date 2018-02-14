#ifndef _HAL_GENERAL_
#define _HAL_GENERAL_
#include <xc.h>
// Nop() is defined by the C32 and XC32 compilers and is not needed here
// use XC32 enable and disable interrupt routines
#define DisableInterrupts() INTDisableInterrupts()
#define EnableInterrupts() INTCONSET = _INTCON_MVEC_MASK; INTEnableInterrupts()
#define BlockInterrupts() unsigned int intStatus; \
					      intStatus = INTDisableInterrupts()
#define RestoreInterrupts() INTRestoreInterrupts(intStatus)

#define Reset() SoftReset()
#define hal_Watchdog_Clear() (WDTCONSET = _WDTCON_WDTCLR_MASK)
#define hal_Watchdog_Enable() (WDTCONSET = _WDTCON_ON_MASK)
#endif // _HAL_GENERAL_

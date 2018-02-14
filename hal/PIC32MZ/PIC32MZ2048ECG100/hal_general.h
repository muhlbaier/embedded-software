#ifndef _HAL_GENERAL_
#define _HAL_GENERAL_
//#include "peripheral/peripheral.h"
#include "xc.h"
// add <harmony install dir>\Harmony\framework to your compiler include dirs if
// peripheral.h causes an issue

// Nop() is defined by the C32 and XC32 compilers and is not needed here
// use XC32 enable and disable interrupt routines
#define DisableInterrupts() asm volatile("di"); asm volatile("ehb")
#define EnableInterrupts() INTCONSET = _INTCON_MVEC_MASK; __builtin_mtc0(12,1,(__builtin_mfc0(12,1) | 0x8000)); asm volatile("ei")
//__builtin_mtc0(12,1,(__builtin_mfc0(12,1) | 0x8000)); __builtin_mtc0(12,0,(__builtin_mfc0(12,0) | 0x0001))
// the compiler has trouble if the above is not all one line.
//
 // mtc0 sets core registers, 12,1 is IntCtl reg, 15bit is CLREXL which makes IRET instruction work
//                            HAL_EnableInterruptsHelper() // 12,0 is Status reg, 0bit is interrupt enable
#define BlockInterrupts() //uint32_t interrupt_state; \
			//		      interrupt_state = __builtin_disable_interrupts();
#define RestoreInterrupts() //__builtin_mtc0(12,0,interrupt_state)

// helpers for this processor (from Harmony peripheral library)
#define _SFR_WRITE(reg,val)  ( *((SFR_TYPE *)(reg))  =  (val) )
#define _SFR_FIELD_WRITE(reg,mask,pos,val)  \
    (  *((SFR_TYPE *)(reg)) = \
        ( (*((SFR_TYPE *)(reg))) & ~(mask) ) | ( (mask)&((val)<<(pos)) )  )

#endif // _HAL_GENERAL_

/*
 * FreeModbus Libary: Atmel AT91SAM3S Demo Application
 * Copyright (C) 2010 Christian Walter <cwalter@embedded-solutions.at>
 *
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * IF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: portother.c,v 1.1 2010/06/05 09:57:48 wolti Exp $
 */


/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Modbus includes ----------------------------------*/
//#include <intrinsics.h>
#include <stdint.h>

/* ----------------------- Variables ----------------------------------------*/
static ULONG    ulNesting;
//static uint32_t xOldState;

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortEnterCritical( void )
{
    //uint32_t      xCurState;
    //xCurState = __get_interrupt_state(  );
    //__disable_irq();	// was changed on 12/6/2021 from disabling all interrupts to only those below the highest maskable priority
	// Disable any interrupts with a lower priority group than the highest maskable priority
	uint32_t pri_group_mask = 1 << NVIC_GetPriorityGrouping();
	uint32_t pri_bit_mask = 1 << (8 - __NVIC_PRIO_BITS);
	if(pri_group_mask < pri_bit_mask){
		__set_BASEPRI(pri_bit_mask);
	}else{
		__set_BASEPRI(pri_group_mask << 1);
	}
	
    if( ulNesting == 0 )
    {
        //xOldState = xCurState;
    }
    ulNesting++;
}

void
vMBPortExitCritical( void )
{
    ulNesting--;
    if( 0 == ulNesting )
    {
        //__enable_irq();	// was changed on 12/6/2021 from enabling all interrupts to only those below the highest maskable priority
		__set_BASEPRI(0U);	// Remove BASEPRI register masking
        //__set_interrupt_state( xOldState );
    }
}

void
vMBPortClose( void )
{
    extern void vMBPortSerialClose( void );
    extern void vMBPortTimerClose( void );
    vMBPortSerialClose(  );
    vMBPortTimerClose(  );
}

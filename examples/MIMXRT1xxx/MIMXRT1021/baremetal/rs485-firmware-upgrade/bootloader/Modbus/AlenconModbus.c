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
 * File: $Id: demo.c,v 1.2 2010/06/06 13:46:42 wolti Exp $
 */

/* ----------------------- AT91SAM3S includes -------------------------------*/
//#include <board.h>
//#include <usart/usart.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbrtu.h"
#include "AlenconModbus.h"
#include <string.h> // for memcpy

struct {
    uint16_t address;
    uint16_t * source;
}input_sources[MODBUS_MAX_INPUT_SOURCES];
uint16_t input_sources_n = 0;

struct holding_registers_s {
    uint16_t start_address;
    uint16_t end_address;
    uint16_t * holding_reg;
    void(*write_callback)(uint16_t *, uint16_t);
    void(*read_callback)(void);
}holding_registers[MODBUS_MAX_HOLDING_REGISTERS];
uint16_t holding_registers_n = 0;

const UCHAR     ucSlaveID[] = { 0x01, 0x00, 0x00 };
static uint8_t modbus_state = 0;
static uint8_t modbus_address = MB_DEFAULT_ADDRESS;
static uint32_t modbus_baud_rate = 115200;

/* ----------------------- Start implementation -----------------------------*/
void Modbus_Routine( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    eStatus = eStatus;
    
    switch(modbus_state) {
        case 0:
            //eMBInit( eMBMode eMode, UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
            if( MB_ENOERR != ( eStatus = eMBInit( MB_RTU, modbus_address, 1, modbus_baud_rate, MB_PAR_NONE) ) )
            {
                /* Can not initialize. Add error handling code here. */
            }else {
                modbus_state = 1;
            }
            break;
        case 1:      
            // eMBSetSlaveID( UCHAR ucSlaveID, BOOL xIsRunning, UCHAR const *pucAdditional, USHORT usAdditionalLen )
            // the 0x34 01 00 00 is arbitrary and should probably be replaced with the GUID. Note this is for responding to function code 17 only.
            if( MB_ENOERR != ( eStatus = eMBSetSlaveID( 0x34, TRUE, ucSlaveID, 3 ) ) )
            {
                /* Can not set slave id. Check arguments */
                modbus_state = 0;
            }else {
                modbus_state = 2;
            }
            break;
        case 2:
            if( MB_ENOERR != ( eStatus = eMBEnable(  ) ) )
            {
                /* Enable failed. */
                modbus_state = 0;
            }else {
                modbus_state = 3;
            }
            break;
        case 3:
            MB_RTU_RECEIVE_FSM();
            eStatus = eMBPoll(  );
            break;
        default:
            eStatus = eMBDisable(  );
            eStatus = eMBClose(  );                
            modbus_state = 0;
            break;
    }
    
    // these routines that are needed to service comms and the timer are abstracted so that we could potentially
    // use the same file for the SPOTCOM implementation as well (a compatibility that will likely never be needed)
    MB_PORT_SERVICE_TIMERS();
}

void AlenconModbus_Stop(void) {
	eMBDisable(  );
	eMBClose(  );
	modbus_state = 0;
}

void Modbus_SetBaudRate(uint32_t baud_rate) {
    if(baud_rate == 9600 || baud_rate == 19200 || baud_rate == 38400 || baud_rate == 57600 || baud_rate == 115200) {
        modbus_baud_rate = baud_rate;
    }
}

void Modbus_ChangeSlaveAddress(uint16_t address) {
    if(address >= MB_ADDRESS_MIN && address <= MB_ADDRESS_MAX) {
        if(address != modbus_address) {
            //eMBErrorCode eStatus = MB_ENOERR;
            if(modbus_state != 0) {
                /*eStatus = */eMBDisable(  );
                /*eStatus = */eMBClose(  );                
                modbus_state = 0;
            }
            modbus_address = address;
        }
    }
}

void Modbus_Reset(uint16_t address) {
    if(address >= MB_ADDRESS_MIN && address <= MB_ADDRESS_MAX)
    {
        eMBDisable(  );
        eMBClose(  );
        modbus_state = 0;
        modbus_address = address;
    }
}

uint8_t Modbus_GetSlaveAddress(void) {
    return modbus_address;
}

void Modbus_AddInputSource(uint16_t address, uint16_t * source) 
{
    if(input_sources_n < MODBUS_MAX_INPUT_SOURCES) {
        // convert to relative address
        if(address >= MODBUS_INPUT_BASE) {
            address -= MODBUS_INPUT_BASE;
        }
        input_sources[input_sources_n].address = address;
        input_sources[input_sources_n].source = source;
        input_sources_n++;
    }
}

void Modbus_AddHoldingRegister(uint16_t address, uint16_t * holding_reg, void(*write_callback)(uint16_t *, uint16_t), void(*read_callback)(void), uint16_t length_limit) 
{
    if(holding_registers_n < MODBUS_MAX_HOLDING_REGISTERS) {
        // convert to relative address
        if(address >= MODBUS_HOLDING_REGISTERS_BASE) {
            address -= MODBUS_HOLDING_REGISTERS_BASE;
        }
        holding_registers[holding_registers_n].start_address = address;
        holding_registers[holding_registers_n].holding_reg = holding_reg;
        holding_registers[holding_registers_n].write_callback = write_callback;
        holding_registers[holding_registers_n].read_callback = read_callback;
        holding_registers[holding_registers_n].end_address = address + length_limit - 1;
        holding_registers_n++;
    }
}

// freemodbus callback functions
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOREG;
    int             i;
    uint8_t * source;

    for(i = 0; i < input_sources_n; i++) {
        if(usAddress == input_sources[i].address) {
            eStatus = MB_ENOERR;
            source = (uint8_t*)input_sources[i].source;
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = *(source+1);
                *pucRegBuffer++ = *source;
                source +=2;
                usNRegs--;
            }
            break;
        }
    }
    
    return eStatus;
}

eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOREG;
    int             i, j;
    uint8_t * holding_reg;
    struct holding_registers_s * hr;
    uint16_t n_regs_this_block;

    for(i = 0; i < holding_registers_n; i++) {
        hr = &holding_registers[i];
        if(usAddress >= hr->start_address && usAddress <= hr->end_address) {
            eStatus = MB_ENOERR;
            // limit number of registers to make sure we don't go past the end of this array
            if(usNRegs > (hr->end_address + 1 - usAddress)) {
                n_regs_this_block = hr->end_address + 1 - usAddress;
            }else {
                n_regs_this_block = usNRegs;
            }
            usNRegs -= n_regs_this_block; // remaining registers after this block
            holding_reg = (uint8_t*)(hr->holding_reg + (usAddress - hr->start_address));
            switch(eMode)
            {
                case MB_REG_READ:
                    // call the read callback function if it exists
                    if(hr->read_callback) hr->read_callback();
                    while( n_regs_this_block > 0 )
                    {
                        *pucRegBuffer++ = *(holding_reg+1);
                        *pucRegBuffer++ = *holding_reg;
                        holding_reg +=2;
                        n_regs_this_block--;
                    }
                    break;
                case MB_REG_WRITE:
                    j = usAddress;
                    while( n_regs_this_block > 0 )
                    {
                        *(holding_reg+1) = *pucRegBuffer++;
                        *holding_reg = *pucRegBuffer++;
                        if(hr->write_callback) {
                            hr->write_callback((uint16_t*)holding_reg, j++);
                        }
                        holding_reg +=2;
                        n_regs_this_block--;
                    }
                    break;
                default:
                    eStatus = MB_EINVAL;
                    break;
            }
            if(usNRegs && eStatus == MB_ENOERR) {
                eStatus = eMBRegHoldingCB(pucRegBuffer, hr->end_address+1, usNRegs, eMode);
            }
            break;
        }
    }

    return eStatus;
}

eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}

void Modbus_LongToRegisters(uint32_t value, uint16_t * register_ptr) {
    *register_ptr++ = value >> 16;
    *register_ptr = (uint16_t)(value & 0xFFFF);
}

void Modbus_FloatToRegisters(float value, uint16_t * register_ptr) {
	uint16_t temp_reg[2];
	memcpy(temp_reg, &value, sizeof(float));
	*register_ptr++ = temp_reg[0];
	*register_ptr = temp_reg[1];
}

float Modbus_RegistersToFloat(uint16_t * register_ptr) {
    float value;
    memcpy(&value, register_ptr, sizeof(float));
    return value;
}

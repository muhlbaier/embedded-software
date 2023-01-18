#ifndef __LPUART_H_
#define __LPUART_H_


#include "mcuid.h"
#include "misc.h"



//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
//Note:
//-------------------------------------------------------------
//USART CLK = 120 MHz
//-------------------------------------------------------------
#define LPUART1_BAUDRATE_921600		0x00000082


#define USART_WAIT4TC_COUNT			40000


void initLPUART1(uint32_t baudrate);
void deinitLPUART1(void);
void putcharLPUART1(uint8_t data);
void printLPUART1(char * str, ... );
void sprintLPUART1(uint8_t * str);

void initLPUART2(uint32_t baudrate);
void putcharLPUART2(uint8_t data);

extern volatile uint32_t g_lpuart1_cnt;

#endif 

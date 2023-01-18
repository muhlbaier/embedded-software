#ifndef __DELAY_H_
#define __DELAY_H_

#include "MIMXRT1021.h"

#define SYSTIM_TIMEOUT							1
#define SYSTIM_KEEP_ALIVE						0
 
void 		delay_ms(uint32_t ms);				 													
void 		delay_us(uint32_t ms);				

void 		initSYSTIM(void);
uint32_t 	getSYSTIM(void);
uint8_t 	chk4TimeoutSYSTIM(uint32_t btime, uint32_t period);	
uint32_t 	getElapsedTimeSYSTIM(uint32_t t_beg);		


#endif 

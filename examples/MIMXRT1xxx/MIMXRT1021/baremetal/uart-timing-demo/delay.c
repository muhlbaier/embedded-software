#include "delay.h"

void delay_ms(uint32_t ms)
{// based on 24MHz external crystal
	GPT1->CR = (GPT_CR_SWR_MASK);
	__DMB();
	while(GPT1->CR & (GPT_CR_SWR_MASK));
	
	GPT1->PR = (11<<12)|(2000-1);
	GPT1->CR |= (GPT_CR_EN_24M_MASK)|GPT_CR_CLKSRC(5)|GPT_CR_EN_MASK|GPT_CR_FRR_MASK;
	
	while(GPT1->CNT <= ms);
	GPT1->CR = (GPT_CR_SWR_MASK);							 
}

void delay_us(uint32_t us)
{// based on 24MHz external crystal
	GPT1->CR = (GPT_CR_SWR_MASK);
	__DMB();
	while(GPT1->CR & (GPT_CR_SWR_MASK));
	
	GPT1->PR = (11<<12)|(2-1);
	GPT1->CR |= (GPT_CR_EN_24M_MASK)|GPT_CR_CLKSRC(5)|GPT_CR_EN_MASK|GPT_CR_FRR_MASK;
	
	while(GPT1->CNT < us);
	GPT1->CR = (GPT_CR_SWR_MASK);								
}

void initSYSTIM(void)
{// based on 24MHz external crystal		
	GPT2->CR = (GPT_CR_SWR_MASK);
	__DMB();
	while(GPT2->CR & (GPT_CR_SWR_MASK));
	
	GPT2->PR = (11<<12)|(2000-1);
	GPT2->CR |= (GPT_CR_EN_24M_MASK)|GPT_CR_CLKSRC(5)|GPT_CR_EN_MASK|GPT_CR_FRR_MASK;											
}

uint32_t getSYSTIM(void)
{
	return GPT2->CNT;
}

uint8_t chk4TimeoutSYSTIM(uint32_t btime, uint32_t period)
{
	uint32_t time = getSYSTIM();
	
	if(time >= btime)
	{
		if((time - btime) >= period)
			return (SYSTIM_TIMEOUT);
		else
			return (SYSTIM_KEEP_ALIVE);
	}
	else
	{
		uint32_t utmp32 = 0xFFFFFFFF - btime;
		if((time + utmp32) >= period)
			return (SYSTIM_TIMEOUT);
		else
			return (SYSTIM_KEEP_ALIVE);
	}
}

uint32_t getElapsedTimeSYSTIM(uint32_t t_beg)
{
	uint32_t time = getSYSTIM();
	t_beg = t_beg;
	
	if(time >= t_beg)
	{
		return (time - t_beg);
	}
	else
	{
		return (time + (0xFFFFFFFF - t_beg));
	}
}

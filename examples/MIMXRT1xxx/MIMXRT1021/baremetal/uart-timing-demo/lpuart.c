#include "lpuart.h"
#include "fsl_clock.h"

void initLPUART1(uint32_t baudrate)
{
	//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	// AD_B0_06 -> Tx
	// AD_B0_07 -> Rx
	// CLOCK 80MHz 
	//------------------------------------------------------------------
	
	{// GPIO pin init in ALT mode
		IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_06] = 0x00000002;
		IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_06] = 0x00000008;
		IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_07] = 0x00000002;
		IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_07] = 0x00000008;
	}	
	
	
	// reset peripheral
	LPUART1->GLOBAL = (LPUART_GLOBAL_RST_MASK);
	LPUART1->GLOBAL &= ~(LPUART_GLOBAL_RST_MASK);
	// disable input trigger
	LPUART1->PINCFG = 0x00000000;
	LPUART1->FIFO = 0;
	
	baudrate = (80000000/16)/baudrate;
	LPUART1->BAUD = baudrate|(15<<24);
	
	LPUART1->STAT = (LPUART_STAT_RXEDGIF_MASK | LPUART_STAT_IDLE_MASK | LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK |
                LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK);
	
	
	LPUART1->CTRL = (LPUART_CTRL_RE_MASK)|(LPUART_CTRL_TE_MASK);	
	
}


void initLPUART2(uint32_t baudrate)
{
	
	{// GPIO pin init in ALT mode
		IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_08] = 0x00000002;
		IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_08] = 0x00000008;
		IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_09] = 0x00000002;
		IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_09] = 0x00000008;
	}	
	
	
	baudrate = (80000000/(16))/(baudrate);
	CLOCK_EnableClock(kCLOCK_Lpuart2);
	// reset peripheral
	LPUART2->GLOBAL = (LPUART_GLOBAL_RST_MASK);
	LPUART2->GLOBAL &= ~(LPUART_GLOBAL_RST_MASK);
	// disable input trigger
	LPUART2->PINCFG = 0x00000000;
	
	LPUART2->FIFO = 0;
	// oversampling by 16, one stop bit
	LPUART2->BAUD = baudrate|(15<<24);
	
	LPUART2->STAT = (LPUART_STAT_RXEDGIF_MASK | LPUART_STAT_IDLE_MASK | LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK |
                LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK);
	
	LPUART2->CTRL = 0;
	
	LPUART2->CTRL |= (LPUART_CTRL_RE_MASK)|(LPUART_CTRL_TE_MASK);	
	__DMB();
	NVIC_EnableIRQ(LPUART2_IRQn);
	
	
	printLPUART1("LPUART2 INIT DONE\n");
}

volatile uint8_t * g_lpuart2_data = 0;
volatile uint8_t g_lpuart2_cnt = 0;
void txLPUART2(uint8_t * data)
{
	
	
	g_lpuart2_cnt = 0;
	g_lpuart2_data = data;
	LPUART2->DATA = data[g_lpuart2_cnt++];
	LPUART2->CTRL |= (LPUART_CTRL_TCIE_MASK);

}

volatile uint32_t g_lpuart1_cnt = 0;
volatile uint32_t g_lpuart1_stat;
//void LPUART2_IRQHandler(void) 
//{
	////NVIC_DisableIRQ(LPUART2_IRQn);
	////printLPUART1("LPUART2 IRQ %x\n",LPUART2->STAT);
	
	
	//g_lpuart1_stat = LPUART2->STAT;
	//g_lpuart1_cnt++;
	//if(g_lpuart2_data[g_lpuart2_cnt] != 0)
	//{
		//LPUART2->DATA = g_lpuart2_data[g_lpuart2_cnt++];
		//////NVIC_EnableIRQ(LPUART2_IRQn);
	//}
	//else
	//{
		////NVIC_DisableIRQ(LPUART2_IRQn);
		//LPUART2->CTRL &= ~(LPUART_CTRL_TCIE_MASK);
	//}

	//__DSB();
//}

void putcharLPUART2(uint8_t data)
{/// print one character to LPUART1
	uint32_t k;

	for(k=0; k<(USART_WAIT4TC_COUNT); k++)
	{
		if(LPUART2->STAT & LPUART_STAT_TDRE_MASK) 
		{
			LPUART2->DATA = data;
			
			break;
		}
	}												    
}


void deinitLPUART1(void)
{
	
}

void putcharLPUART1(uint8_t data)
{/// print one character to LPUART1
	uint32_t k;

	for(k=0; k<(USART_WAIT4TC_COUNT); k++)
	{
		if(LPUART1->STAT & LPUART_STAT_TDRE_MASK) 
		{
			LPUART1->DATA = data;
			
			break;
		}
	}												    
}

void printLPUART1(char *str, ... )
{ /// print text and up to 10 arguments!
    uint8_t rstr[40];													// 33 max -> 32 ASCII for 32 bits and NULL 
    uint16_t k = 0;
	uint16_t arg_type;
	uint32_t utmp32;
	uint32_t * p_uint32; 
	char * p_char;
	va_list vl;
	
	//va_start(vl, 10);													// always pass the last named parameter to va_start, for compatibility with older compilers
	va_start(vl, str);													// always pass the last named parameter to va_start, for compatibility with older compilers
	while(str[k] != 0x00)
	{
		if(str[k] == '%')
		{
			if(str[k+1] != 0x00)
			{
				switch(str[k+1])
				{
					case('b'):
					{// binary
						if(str[k+2] == 'b')
						{// byte
							utmp32 = va_arg(vl, int);
							arg_type = (PRINT_ARG_TYPE_BINARY_BYTE);
						}
						else if(str[k+2] == 'h')
						{// half word
							utmp32 = va_arg(vl, int);
							arg_type = (PRINT_ARG_TYPE_BINARY_HALFWORD);
						}
						else if(str[k+2] == 'w')
						{// word	
							utmp32 = va_arg(vl, uint32_t);
							arg_type = (PRINT_ARG_TYPE_BINARY_WORD);
						}
						else
						{// default word
							utmp32 = va_arg(vl, uint32_t);
							arg_type = (PRINT_ARG_TYPE_BINARY_WORD);
							k--;
						}
						
						k++;	
						p_uint32 = &utmp32;
						break;
					}
					case('d'):
					{// decimal
						if(str[k+2] == 'b')
						{// byte
							utmp32 = va_arg(vl, int);
							arg_type = (PRINT_ARG_TYPE_DECIMAL_BYTE);
						}
						else if(str[k+2] == 'h')
						{// half word
							utmp32 = va_arg(vl, int);
							arg_type = (PRINT_ARG_TYPE_DECIMAL_HALFWORD);
						}
						else if(str[k+2] == 'w')
						{// word	
							utmp32 = va_arg(vl, uint32_t);
							arg_type = (PRINT_ARG_TYPE_DECIMAL_WORD);
						}
						else
						{// default word
							utmp32 = va_arg(vl, uint32_t);
							arg_type = (PRINT_ARG_TYPE_DECIMAL_WORD);
							k--;
						}
						
						k++;	
						p_uint32 = &utmp32;
						break;
					}
					case('c'):
					{// character
						char tchar = va_arg(vl, int);	
						putcharLPUART1(tchar);
						arg_type = (PRINT_ARG_TYPE_CHARACTER);
						break;
					}
					case('s'):
					{// string 
						p_char = va_arg(vl, char *);	
						sprintLPUART1((uint8_t *)p_char);
						arg_type = (PRINT_ARG_TYPE_STRING);
						break;
					}
					case('f'):
					{// float
						uint64_t utmp64 = va_arg(vl, uint64_t);			// convert double to float representation IEEE 754
						uint32_t tmp1 = utmp64&0x00000000FFFFFFFF;
						tmp1 = tmp1>>29;
						utmp32 = utmp64>>32;
						utmp32 &= 0x07FFFFFF;
						utmp32 = utmp32<<3;
						utmp32 |= tmp1;
						if(utmp64 & 0x8000000000000000)
							utmp32 |= 0x80000000;
							
						if(utmp64 & 0x4000000000000000)
							utmp32 |= 0x40000000;
							
						p_uint32 = &utmp32;
						
						arg_type = (PRINT_ARG_TYPE_FLOAT);
						//arg_type = (PRINT_ARG_TYPE_HEXADECIMAL_WORD);
						//arg_type = (PRINT_ARG_TYPE_BINARY_WORD);
						break;
					}
					case('x'):
					{// hexadecimal 
						if(str[k+2] == 'b')
						{// byte
							utmp32 = (uint32_t)va_arg(vl, int);
							arg_type = (PRINT_ARG_TYPE_HEXADECIMAL_BYTE);
						}
						else if(str[k+2] == 'h')
						{// half word
							utmp32 = (uint32_t)va_arg(vl, int);
							arg_type = (PRINT_ARG_TYPE_HEXADECIMAL_HALFWORD);
						}
						else if(str[k+2] == 'w')
						{// word	
							utmp32 = va_arg(vl, uint32_t);
							arg_type = (PRINT_ARG_TYPE_HEXADECIMAL_WORD);
						}
						else
						{
							utmp32 = va_arg(vl, uint32_t);
							arg_type = (PRINT_ARG_TYPE_HEXADECIMAL_WORD);
							k--;
						}
						
						k++;
						p_uint32 = &utmp32;
						break;
					}
					default:
					{
						utmp32 = 0;
						p_uint32 = &utmp32;
						arg_type = (PRINT_ARG_TYPE_UNKNOWN);
						break;
					}
				}
					
				if(arg_type&(PRINT_ARG_TYPE_MASK_CHAR_STRING))	
				{
					getStr4NumMISC(arg_type, p_uint32, rstr);
					sprintLPUART1(rstr);	
				}
				k++;
			}
		}
		else
		{// not a '%' char -> print the char
			putcharLPUART1(str[k]);
			if (str[k] == '\n')
				putcharLPUART1('\r');
		}
		k++;
	}
	
	va_end(vl);
	return;
}

void sprintLPUART1(uint8_t * str)
{
	uint16_t k = 0;
	
	while (str[k] != '\0')
    {
        putcharLPUART1(str[k]);
        if (str[k] == '\n')
            putcharLPUART1('\r');
        k++;

        if (k == MAX_PRINT_STRING_SIZE)
            break;
    }
}



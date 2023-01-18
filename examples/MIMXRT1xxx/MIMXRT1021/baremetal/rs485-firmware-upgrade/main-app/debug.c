#include "debug.h"
#include "fwver.h"
#include "lpuart.h"

#define SYSTEM_DEBUG
#define LINUX_OS

#define printUART								printLPUART1
#define initUART								initLPUART1
#define putcharUART 							putcharLPUART1
#define sprintUART 								sprintLPUART1
#define UART_BAUDRATE							115200	


DEBUG_INFO g_DEBUG_INFO;

void initDEBUG(char * cpu_name, uint8_t cpu_name_color, char * app_name )
{
	uint8_t k;
	
	g_DEBUG_INFO.cpu_name_color = cpu_name_color;
	
	for(k=0;k<(DEBUG_CPU_NAME_SIZE);k++)
	{
		g_DEBUG_INFO.cpu_name[k] = cpu_name[k];
		if(cpu_name[k] == 0)
		{
			break;
		}
	}
	// ensure that cpu_name is NULL terminated
	if(k == (DEBUG_CPU_NAME_SIZE))
	{
		k--;
	}
	g_DEBUG_INFO.cpu_name[k] = 0;
	
	initUART(UART_BAUDRATE);
	printUART("\n\nwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
	printUART("w IRQDESIGN d.o.o. Tuzla Bosnia and Herzegovina\n");
	printUART("w----------------------------------------------------------------\n");
	printUART("w %s\nw\n",app_name);
	uint32_t fwver = FIRMWARE_VERSION;
	uint16_t hwd_p1 = (fwver>>28)&0x0000000F;
	uint16_t hwd_p2 = (fwver>>24)&0x0000000F;
	printUART("w hwver: %d.%d\n",hwd_p1,hwd_p2);
	printUART("w fwver: %d.%d.%d.\n",(fwver>>16)&0x000000FF,(fwver>>8)&0x000000FF,(fwver)&0x000000FF);
	printUART("w time:  %x\n",BUILD_TIME);
	uint32_t uuid[2];
	uint8_t * cid = (uint8_t *)&uuid[0];
	uuid[0] = *(uint32_t *)(DEVICE_ID_REG_ADDR);
	uuid[1] = *(uint32_t *)(DEVICE_ID_REG_ADDR + 0x10);
	printUART("w cpuid: ");
	for(k=0;k<(DEVICE_ID_REG_SIZE);k++)
	{
		printUART("%xb",cid[k]);
	}
	printUART("\n");
	printUART("wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
}

void printDEBUG(uint16_t sid, char * mod_name, char *str, ... )
{
#ifndef SYSTEM_DEBUG
	return;
#endif

	uint8_t rstr[40];													// 33 max -> 32 ASCII for 32 bits and NULL
	uint32_t k = 0;
	uint16_t arg_type;
	uint32_t utmp32;
	uint32_t * p_uint32;
	char * p_char;
	va_list vl;

#ifdef LINUX_OS

	if((sid & DAPPEND) == 0x0000)
	{
		#define LINUX_OS_DATETIME		
#ifdef LINUX_OS_DATETIME		
		// print SYSTIM
		rstr[0] = 0;
		
		uint32_t time = getSYSTIM();
		getStr4NumMISC(PRINT_ARG_TYPE_DECIMAL_WORD, &time, rstr);
		putcharUART('\e');
		putcharUART('[');
		putcharUART('3');
		putcharUART(DEBUG_COLOR_GREEN);
		putcharUART('m');
		putcharUART('[');
		
		k = 0;
		while(rstr[k] != 0)
		{
			putcharUART(rstr[k++]);
		}
		
		putcharUART(']');
#endif		
		
		if(g_DEBUG_INFO.cpu_name[0] != 0)
		{
			// print CPU NAME
			putcharUART('\e');
			putcharUART('[');
			putcharUART('3');
			putcharUART(g_DEBUG_INFO.cpu_name_color);
			putcharUART('m');

			for(k=0; k<16; k++)
			{
				putcharUART(g_DEBUG_INFO.cpu_name[k]);
			}
		}

		putcharUART('\e');
		putcharUART('[');
		putcharUART('3');
		putcharUART('5');
		putcharUART('m');
		putcharUART('~');
		
		
		// print the module
		k = 0;
		while(mod_name[k] != '\0')
		{
			putcharUART(mod_name[k++]);
		}

		putcharUART(':');
		putcharUART(' ');
		putcharUART('\e');
		putcharUART('[');
		putcharUART('0');
		putcharUART('m');


		if(sid & DERROR)
		{
			putcharUART('\e');
			putcharUART('[');
			putcharUART('3');
			putcharUART('1');
			putcharUART('m');
		}
		else if(sid & (DDEBUG))
		{
			putcharUART('\e');
			putcharUART('[');
			putcharUART('3');
			putcharUART('6');
			putcharUART('m');
		}
		else if(sid & (DWARNING))
		{
			putcharUART('\e');
			putcharUART('[');
			putcharUART('3');
			putcharUART('3');
			putcharUART('m');
		}
	}

#endif


	k = 0;
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
					{
						// binary
						if(str[k+2] == 'b')
						{
							// byte
							utmp32 = va_arg(vl, int);
							arg_type = (PRINT_ARG_TYPE_BINARY_BYTE);
						}
						else if(str[k+2] == 'h')
						{
							// half word
							utmp32 = va_arg(vl, int);
							arg_type = (PRINT_ARG_TYPE_BINARY_HALFWORD);
						}
						else if(str[k+2] == 'w')
						{
							// word
							utmp32 = va_arg(vl, uint32_t);
							arg_type = (PRINT_ARG_TYPE_BINARY_WORD);
						}
						else
						{
							// default word
							utmp32 = va_arg(vl, uint32_t);
							arg_type = (PRINT_ARG_TYPE_BINARY_WORD);
							k--;
						}

						k++;
						p_uint32 = &utmp32;
						break;
					}
					case('d'):
					{
						// decimal
						if(str[k+2] == 'b')
						{
							// byte
							utmp32 = va_arg(vl, int);
							arg_type = (PRINT_ARG_TYPE_DECIMAL_BYTE);
						}
						else if(str[k+2] == 'h')
						{
							// half word
							utmp32 = va_arg(vl, int);
							arg_type = (PRINT_ARG_TYPE_DECIMAL_HALFWORD);
						}
						else if(str[k+2] == 'w')
						{
							// word
							utmp32 = va_arg(vl, uint32_t);
							arg_type = (PRINT_ARG_TYPE_DECIMAL_WORD);
						}
						else
						{
							// default word
							utmp32 = va_arg(vl, uint32_t);
							arg_type = (PRINT_ARG_TYPE_DECIMAL_WORD);
							k--;
						}

						k++;
						p_uint32 = &utmp32;
						break;
					}
					case('c'):
					{
						// character
						char tchar = va_arg(vl, int);
						putcharUART(tchar);
						arg_type = (PRINT_ARG_TYPE_CHARACTER);
						break;
					}
					case('s'):
					{
						// string
						p_char = va_arg(vl, char *);
						sprintUART((uint8_t *)p_char);
						arg_type = (PRINT_ARG_TYPE_STRING);
						break;
					}
					case('f'):
					{
						// float
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
					{
						// hexadecimal
						if(str[k+2] == 'b')
						{
							// byte
							utmp32 = (uint32_t)va_arg(vl, int);
							arg_type = (PRINT_ARG_TYPE_HEXADECIMAL_BYTE);
						}
						else if(str[k+2] == 'h')
						{
							// half word
							utmp32 = (uint32_t)va_arg(vl, int);
							arg_type = (PRINT_ARG_TYPE_HEXADECIMAL_HALFWORD);
						}
						else if(str[k+2] == 'w')
						{
							// word
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
					sprintUART(rstr);
				}
				k++;
			}
		}
		else
		{
			// not a '%' char -> print the char
			putcharUART(str[k]);
			if (str[k] == '\n')
				putcharUART('\r');
		}
		k++;
	}

	va_end(vl);

#ifdef LINUX_OS
	if(sid & 0xC000)
	{
		putcharUART('\e');
		putcharUART('[');
		putcharUART('0');
		putcharUART('m');
	}
#endif
}

void NMI_Handler(void)
{
#ifdef SYSTEM_DEBUG	
	printUART("-> SYS: NMI_Handler\n");
#endif
	while(1);
}

void HardFault_Handler(void)
{
	__asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word HardFault_HandlerC    \n"
    );
	
}

void HardFault_HandlerC( uint32_t *pulFaultStackAddress )
{
/* These are volatile to try and prevent the compiler/linker optimising them
away as the variables never actually get used.  If the debugger won't show the
values of the variables, make them global my moving their declaration outside
of this function. */

#ifdef SYSTEM_DEBUG	
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr; /* Link register. */
	volatile uint32_t pc; /* Program counter. */
	volatile uint32_t psr;/* Program status register. */

	
    r0 = pulFaultStackAddress[0];
    r1 = pulFaultStackAddress[1];
    r2 = pulFaultStackAddress[2];
    r3 = pulFaultStackAddress[3];

    r12 = pulFaultStackAddress[4];
    lr = pulFaultStackAddress[5];
    pc = pulFaultStackAddress[6];
    psr = pulFaultStackAddress[7];
	
	printUART("\n\n-> ");
	uint8_t k;
	for(k=0;k<(DEBUG_CPU_NAME_SIZE);k++)
	{
		if(g_DEBUG_INFO.cpu_name[k] == 0)
		{
			break;
		}
		else
		{
			putcharUART(g_DEBUG_INFO.cpu_name[k]);
		}
	}
	printUART(": HardFault_Handler \n");
	
	printUART("r0\t[%x]\n",r0);
	printUART("r1\t[%x]\n",r1);
	printUART("r2\t[%x]\n",r2);
	printUART("r3\t[%x]\n",r3);
	printUART("r12\t[%x]\n",r12);
	printUART("lr\t[%x]\n",lr);
	printUART("pc\t[%x]\n",pc);
	printUART("psr\t[%x]\n\n",psr);
	
	printUART("bfar\t[%x]\n",(*((volatile unsigned long *)(0xE000ED38))));
	printUART("cfsr\t[%x]\n",(*((volatile unsigned long *)(0xE000ED28))));
	printUART("hfsr\t[%x]\n",(*((volatile unsigned long *)(0xE000ED2C))));
	printUART("dfsr\t[%x]\n",(*((volatile unsigned long *)(0xE000ED30))));
	printUART("afsr\t[%x]\n", (*((volatile unsigned long *)(0xE000ED3C))));

#endif	
    
    //NVIC_SystemReset();
    while(1);
}

void MemManage_Handler(void)
{
#ifdef SYSTEM_DEBUG	
	printUART("-> SYS: MemManage_Handler\n");
#endif
	while(1);	
}

void BusFault_Handler(void)
{
#ifdef SYSTEM_DEBUG	
	printUART("-> SYS: BusFault_Handler\n");
#endif
	while(1);	
}

void UsageFault_Handler(void)
{
#ifdef SYSTEM_DEBUG	
	printUART("-> SYS: UsageFault_Handler\n");
#endif
	while(1);
}

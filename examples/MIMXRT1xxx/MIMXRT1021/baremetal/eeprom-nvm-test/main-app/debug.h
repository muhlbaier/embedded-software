#ifndef __DEBUG_H__
#define __DEBUG_H__ 

#include "mcuid.h"
#include "delay.h"

#define debugMSG(type, ...)			printDEBUG(type, "MSG", __VA_ARGS__)


#define DNONE									0x0000
#define DAPPEND									0x1000
#define DDEBUG									0x2000
#define DWARNING								0x4000
#define DERROR									0x8000	
#define DNOTIFY									0x2000
#define DHEADER									0x2000  

typedef enum
{
	DEBUG_COLOR_BLACK = 0x30,
	DEBUG_COLOR_RED,
	DEBUG_COLOR_GREEN,
	DEBUG_COLOR_YELLOW,
	DEBUG_COLOR_BLUE,
	DEBUG_COLOR_MAGENTA,
	DEBUG_COLOR_CYAN,
	DEBUG_COLOR_WHITE,
	DEBUG_COLOR_COUNT,
} DEBUG_Text_Colors;


#define DEBUG_CPU_NAME_SIZE						16

typedef struct 
{
	uint8_t cpu_name_color;
	uint8_t cpu_name[DEBUG_CPU_NAME_SIZE];
	
} DEBUG_INFO;

void initDEBUG(char * cpu_name, uint8_t cpu_name_color, char * app);
void printDEBUG(uint16_t sid, char * mod_name, char *str, ... );

#endif 

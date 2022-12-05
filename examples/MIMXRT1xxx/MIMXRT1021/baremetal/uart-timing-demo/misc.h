#ifndef __MISC_H_
#define __MISC_H_

#include "mcuid.h"

#define MAX_PRINT_STRING_SIZE					1024

#define PRINT_ARG_TYPE_BINARY_BYTE				0x0001	
#define PRINT_ARG_TYPE_BINARY_HALFWORD			0x0002	
#define PRINT_ARG_TYPE_BINARY_WORD				0x0004			
#define PRINT_ARG_TYPE_DECIMAL_BYTE				0x0008		
#define PRINT_ARG_TYPE_DECIMAL_HALFWORD			0x0010		
#define PRINT_ARG_TYPE_DECIMAL_WORD				0x0020		
#define PRINT_ARG_TYPE_CHARACTER				0x0040			
#define PRINT_ARG_TYPE_STRING					0x0080			
#define PRINT_ARG_TYPE_FLOAT					0x0100			
#define PRINT_ARG_TYPE_HEXADECIMAL_BYTE			0x0200		
#define PRINT_ARG_TYPE_HEXADECIMAL_HALFWORD		0x0400		
#define PRINT_ARG_TYPE_HEXADECIMAL_WORD			0x0800
#define PRINT_ARG_TYPE_UNKNOWN					0x0000

#define PRINT_ARG_TYPE_MASK_CHAR_STRING			~((PRINT_ARG_TYPE_CHARACTER)|(PRINT_ARG_TYPE_STRING))


#define MISC_STR_LEN_DIFFERS					0x00
#define MISC_STR_DO_NOT_MATCH					0x01
#define MISC_STR_MACTH							0x02

void getDNumMISC(uint8_t * dnum, uint32_t num);
void putDNumMISC(uint8_t * dnum, uint16_t * m, uint8_t * r_str);
void getStr4NumMISC(uint16_t type, uint32_t  * num, uint8_t * r_str);
void getStrMISC(uint8_t * str, uint32_t  * num, uint8_t * r_str);
void cpyStrMISC(uint8_t * dst, uint8_t * src); 
void catStrMISC(uint8_t * dst, uint8_t * src);
uint16_t lenStrMISC(uint8_t * str);
uint8_t cmpStrMISC(uint8_t * spat, uint8_t * stest);
uint8_t cmpStrWithLenMISC(uint8_t * spat, uint16_t len_spat, uint8_t * stest, uint16_t len_stest);
void 	getStr4AddrMISC(uint8_t * str, uint32_t addr);
#endif 

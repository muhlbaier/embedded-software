/**
 * @defgroup buffer_printf printf Functionality for Buffer Module
 * @ingroup buffer
 * buffer_printf.h
 *
 *  Created on: Mar 11, 2014
 *      Author: Michael
 * 
 * @version 1.1 changed naming to match stdint and removed 2Buf from function names
 * @version 1.2 added %l for int32 and %b for 16-bit binary
 * @{
 */

#ifndef _BUFFER_PRINTF_H_
#define _BUFFER_PRINTF_H_

#include <stdarg.h>
#include <stdint.h>

#include "project_settings.h"

#ifndef USE_MODULE_BUFFER_PRINTF
#warning "USE_MODULE_BUFFER_PRINTF not defined in project_settings.h. Other modules won't be able to utilize this module."
#endif


#include "buffer.h"

/**
 *  @brief printf implementation to char buffer
 *  
 *  Currently supports the following replacement flags:
 *  - @c %c char, replaces flag with specified ASCII char
 *  - @c %b binary, replaces flag with Binary representation
 *    See PushBinary16()
 *  - @c %l long, replaces flag with Binary Representation
 *    See Push_int32()
 *  - @c %d signed 16 bit integer, replaces flag with specified int. 
 *    See Push_int16()
 *  - @c %e @c %f or @c %g float, replaces flag with specified float
 *    See PushFloat()
 *  - @c %s string, replaces flag with specified null terminated string
 *    See PushString()
 *  - @c %u unsigned 16 bit integer, replaces flag with specified unsigned int
 *    See Push_uint16()
 *  - @c %x 16 bit hex formated integer, replaces flag with 4 digit hex value. 
 *    Can accept length format input: @c %0Lx where L is between 1 and 8. The
 *    printf functions expect a 32 bit value only when L is between 5 and 8. For
 *    example @c %08x can be used to print a 32-bit number in hex format.
 *    See PushHex()
 *
 *  Example: 
 *  @code
 *  buffer_t tx;
 *  ...
 *  int16_t x = -1;
 *  char name[] = "Muhlbaier";
 *  Push_printf(&tx, "x = %d, hex - 0x%x, unsigned %u, name = %s);
 *  @endcode
 *  Would push to the buffer:
 *  "x = -1, hex - 0xFFFF, unsigned 65535, name = Muhlbaier"
 *  
 *  @param [in] buf Pointer to char buffer to print formatted string to
 *  @param [in] str Pointer to null terminated string with replacement flags
 *  @param [in] ... Variable argument list corresponding with replacement flags
 */
void Buffer_printf(buffer_t * buf, char * str, ...);

/**
 *  @brief vprintf implementation to char buffer
 *  
 *  Same as Push_printf() except with  a va_list pointer instead of an
 *  actual variable argument list. This allows other functions similar to
 *  Push_printf to be implemented.
 *
 *  For example:
 *  @code
 *  void LogStr(char * str, ...) {
 *     va_list vars;
 *     va_start(vars, str);
 *     // use Push_vprintf to log to SUBSYS_UART
 *     Push_vprintf(SUBSYS_UART, str, vars);
 *     va_end(vars);
 *  }
 *  @endcode
 *
 *  See Push_printf()
 *  
 *  @param [in] buf Pointer to char buffer to print formatted string to
 *  @param [in] str Pointer to null terminated string with replacement flags
 *  @param [in] vars Variable argument list corresponding with replacement flags
 */
void Buffer_vprintf(buffer_t * buf, char * str, va_list vars);

/** @brief Push 16 bit binary number to char buffer
 *
 * @param [in] buf Pointer to char buffer to print to
 * @param [in] x Unsigned 16-bit integer to convert to text in binary format
 */
void Buffer_Binary16(buffer_t * buf, uint16_t x);

/**
 *  @brief Push unsigned integer to char buffer
 *  
 *  @param [in] buf Pointer to char buffer to print unsigned int to
 *  @param [in] x Unsigned integer to convert to text
 */
void Buffer_uint16(buffer_t * buf, uint16_t x);

/**
 *  @brief Push integer to char buffer
 *  
 *  Note this function is dependent on Push_uint16()
 *  
 *  @param [in] buf Pointer to char buffer to print int to
 *  @param [in] x Integer to convert to text
 */
void Buffer_int16(buffer_t * buf, int16_t x);

/**
 *  @brief Push unsigned long integer to char buffer
 *  
 *  @param [in] buf Pointer to char buffer to print unsigned long to
 *  @param [in] x Unsigned integer to convert to text
 */
void Buffer_uint32(buffer_t * buf, uint32_t x);

/**
 *  @brief Push long to char buffer
 *  
 *  Note this function is dependent on Push_uint32()
 *  
 *  @param [in] buf Pointer to char buffer to print int to
 *  @param [in] x Long to convert to text
 */
void Buffer_int32(buffer_t * buf, int32_t x);

/**
 *  @brief Push char array (string) to char buffer
 *  
 *  @param [in] buf Pointer to char buffer to print string to
 *  @param [in] str Pointer to null terminated char array (e.g. string)
 */
void Buffer_Str(buffer_t * buf, char * str);

/**
 *  @brief Push 32 bit value to char buffer in hex format
 *  
 *  Will push four char's to the buffer, for example: A0F3
 *  
 *  @param [in] buf Pointer to char buffer to print hex formatted int to
 *  @param [in] x Integer to convert to hex
 *  @param [in] n number of hex characters to print (1-8)
 */
void Buffer_HexN(buffer_t * buf, uint32_t x, uint8_t n);
#define Buffer_Hex(buf, x) Buffer_HexN(buf, x, 4)

/**
 *  @brief Cheap implementation of float to char buffer
 *  
 *  Current implementation will format float as 0.000 by
 *  first printing out the integer portion of the float then
 *  multiplying the float by 1000 and subtracting the integer 
 *  portion x 1000 and printing that after the .
 *  
 *  @param [in] buf Pointer to char buffer to print float to
 *  @param [in] x Float value to convert to text
 */
void Buffer_Float(buffer_t * buf, float x);

///@}
#endif /* _BUFFER_PRINTF_H_ */

/*
 * buffer_printf.c
 *
 *  Created on: Mar 11, 2014
 *      Author: Michael
 */
#include <stdarg.h>

#include "buffer_printf.h"
#include "library.h"

void Buffer_printf(buffer_t * buf, char * str, ...) {
    // variable argument list type
    va_list vars;
    // initialize the variable argument list pointer by specifying the
    // input argument immediately preceding the variable list
    va_start(vars, str);
    Buffer_vprintf(buf, str, vars);
    va_end(vars);
}

void Buffer_vprintf(buffer_t * buf, char * str, va_list vars) {
    char escape = 0;
    uint8_t len = 0;
    while (*str != 0) {
        if (*str == '%' && !escape) {
            // check for length in the format of 0*
            if(*(str+1) == '0') {
                len = *(str+2) - '0';
                str += 2;
            }
            switch (*(str + 1)) {
            case 0:
                str++;
                continue;
            case 'c': // char
#if defined __XC32__ || defined __UVISION_VERSION || defined MSPGCC || defined __GNUC__
                Buffer_Enqueue(buf, va_arg(vars, int));
#else
                Buffer_Enqueue(buf, va_arg(vars, char));
#endif
                str += 2;
                continue;
            case 'd':
            case 'i':
                if(sizeof(int) == 4){
                    Buffer_int32(buf, va_arg(vars, int));
                } else {
                    Buffer_int16(buf, va_arg(vars, int));
                }
                str += 2;
                continue;
            case 'e':
            case 'f':
            case 'g': // scientific notation of float
                Buffer_Float(buf, va_arg(vars, double));
                str += 2;
                continue;
            case 'l':
                switch(*(str + 2)) {
                case 'd':
                case 'i':
                    Buffer_int32(buf, va_arg(vars, long));
                    str++;
                    break;
                case 'u':
                    Buffer_uint32(buf, va_arg(vars, long));
                    str++;
                    break;
                case 'x':
                    Buffer_HexN(buf, va_arg(vars, long), 8);
                    str++;
                    break;
                default:
                    Buffer_int32(buf, va_arg(vars, long));
                    break;
                }
                str += 2;
                continue;
            case 's': // string
                Buffer_Str(buf, va_arg(vars, char *));
                str += 2;
                continue;
            case 'u':
                // use va_arg to get the next variable of length int
                Buffer_uint16(buf, va_arg(vars, unsigned int));
                str += 2;
                continue;
            case 'x': // hex
                if(len>4) Buffer_HexN(buf, va_arg(vars, long), len);
                else if(len) Buffer_HexN(buf, va_arg(vars, int), len);
                else Buffer_Hex(buf, va_arg(vars, int));
                str += 2;
                continue;
            default:
                // if the character following the % was not "special"
                // then we will print the % and whatever follows it
                break;
            }
        }
        //escape = (*str == '\\') ? 1 : 0;
        //Buffer_Enqueue(buf,*str++);
        if (*str == '\\' && *(str + 1) == '%') {
            str++;
            escape = 1;
        } else {
            Buffer_Enqueue(buf, *str++);
            escape = 0;
        }
    }

}

void Buffer_uint16(buffer_t * buf, uint16_t x) {
    char num[6];
    unsigned int i = 0;
    do {
        num[i] = (x % 10) + '0';
        x /= 10;
        i++;
    } while (x);
    do {
        Buffer_Enqueue(buf, num[--i]);
    } while (i);
}

void Buffer_Binary16(buffer_t * buf, uint16_t x) {
    uint16_t i;
    for (i = 0; i < 16; i++) {
        Buffer_Enqueue(buf, (x & 0x0001) ? '1' : '0');
        x >>= 1;
    }
}

void Buffer_int16(buffer_t * buf, int16_t x) {
    if (x < 0) {
        Buffer_Enqueue(buf, '-');
        x = -x;
    }
    Buffer_uint16(buf, x);
}

void Buffer_int32(buffer_t * buf, int32_t x) {
    if (x < 0) {
        Buffer_Enqueue(buf, '-');
        x = -x;
    }
    Buffer_uint32(buf, x);
}

void Buffer_uint32(buffer_t * buf, uint32_t x) {
    char num[10];
    unsigned int i = 0;
    do {
        num[i] = (x % 10) + '0';
        x /= 10;
        i++;
    } while (x);
    do {
        Buffer_Enqueue(buf, num[--i]);
    } while (i);
}

void Buffer_Str(buffer_t * buf, char * str) {
    while(*str != 0) {
        Buffer_Enqueue(buf, *str++);
    }
}

void Buffer_HexN(buffer_t *buf, uint32_t x, uint8_t n) {
    char num[8];
    uint8_t i;
    union32_t y;
    uint8_t t;
    if(n>8) n = 8;
    y.phrase = x;
    for(i = 0; i < n; i += 2) {
        t = y.ub[i>>1].nibble.low;
        num[i] = (t >= 10) ? (t + 0x37) : (t + 0x30);
        t = y.ub[i>>1].nibble.high;
        num[i+1] = (t >= 10) ? (t + 0x37) : (t + 0x30);
    }
    do {
        Buffer_Enqueue(buf, num[--n]);
    } while (n);
}

void Buffer_Float(buffer_t * buf, float x) {
    int i;
    long l;
    // for now just print 0.000 format
    i = x;
    Buffer_int16(buf, i);
    l = (x * 1000);
    l = l - i * 1000;
    Buffer_Enqueue(buf, '.');
    if (l < 100) Buffer_Enqueue(buf, '0');
    if (l < 10) Buffer_Enqueue(buf, '0');
    if (l == 0) Buffer_Enqueue(buf, '0');
    else Buffer_uint16(buf, l & 0xFFFF);
}

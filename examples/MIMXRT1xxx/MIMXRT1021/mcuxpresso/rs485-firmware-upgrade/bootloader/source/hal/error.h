#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdint.h>

#define error_t int8_t

#define ENOSPC -1 // No space (Used for things like buffers or lists not having sufficient space for a new item)
#define ENORSRC -2 // No resource (Used for things like accessing an unavailable or non-existent UART channel)
#define EINVAL -3 // Invalid argument

#endif //_ERROR_H_

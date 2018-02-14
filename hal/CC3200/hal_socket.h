#ifndef _HAL_SOCKET_H_
#define _HAL_SOCKET_H_

#include <simplelink.h>

#define SET_SOCKET_NONBLOCKING(s)\
    sl_SetSockOpt(s, SOL_SOCKET, SL_SO_NONBLOCKING, &(uint32_t){1}, sizeof(uint32_t))

#endif //_HAL_SOCKET_H_

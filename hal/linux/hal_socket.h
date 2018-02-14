#ifndef _HAL_SOCKET_H_
#define _HAL_SOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define SET_SOCKET_NONBLOCKING(s) fcntl(s, F_SETFL, O_NONBLOCK)

#endif //_HAL_SOCKET_H_

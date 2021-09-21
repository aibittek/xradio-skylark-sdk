#ifndef _EIPLATFORM_H
#define _EIPLATFORM_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

#if defined(_WIN32)
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#define GET_ERROR() GetLastError()
#define vsnprintf _vsnprintf
#define snprintf _snprintf
typedef SOCKET sock_t;
#elif defined(__unix__)
#include <stdarg.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
typedef int sock_t;
#define SOCKET_ERROR -1
#define INVALID_SOCKET	(-1)
#define closesocket(x) close(x)
#define GET_ERROR() errno

#elif defined(__CONFIG_CHIP_XR872) && defined(__CONFIG_OS_FREERTOS)
#include "kernel/os/os.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "wlan/wlan.h"
#include "wlan/wlan_defs.h"
typedef int sock_t;
#define SOCKET_ERROR -1
#define INVALID_SOCKET	(-1)
#define GET_ERROR() errno

#else
#error "not support platform."
#endif

#define pvEIMalloc malloc
#define vEIFree free
#define pvEIRealloc realloc

#endif

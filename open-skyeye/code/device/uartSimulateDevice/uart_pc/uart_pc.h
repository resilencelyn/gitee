#ifndef __SKYEYE_UART_PC_H__
#define __SKYEYE_UART_PC_H__

#include "skyeye_log.h"
#include "skyeye_uart_ops.h"
#include <skyeye_serial.h>

#ifndef __MINGW32__
/* linux head file */
#include <poll.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
typedef char BOOL;
#else
#include <windows.h>
#include <direct.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

typedef struct receive_t{
	uint8_t* rec_buf;
	int      rec_head;
	int      rec_tail;
	int      rec_count;
	int      mutex;
}rec_t;


typedef struct skyeye_uart_pc{
	conf_object_t* obj;
	rec_t*         receive;
	const char*    obj_name;
	BOOL           attached;
#ifndef __MINGW32__
	int port_fd;
#else
	HANDLE port_fd;
#endif
	serial_t *serial;
	int SteupFlag;
}uart_pc_device;

#define MAX_REC_NUM 1024
#endif

#ifndef __SKYEYE_UART_TERM_H__
#define __SKYEYE_UART_TERM_H__

#include "skyeye_log.h"
#include "skyeye_uart_ops.h"

#ifndef __MINGW32__
/* linux head file */
#include <poll.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include <windows.h>
#include <direct.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

typedef struct receive_t
{
    uint8_t *rec_buf;
    int     rec_head;
    int     rec_tail;
    int     rec_count;
    int     mutex;
} rec_t;
#define MAX_DATA_CACHE 0x100000

typedef struct skyeye_uart_term
{
    conf_object_t * obj;
    rec_t *         receive;
    int             mod;
    bool_t            telnet_mode;
#ifndef __MINGW32__
    int             socket;
    pid_t           uart_console_pid;
#else
    SOCKET          socket;
    DWORD           uart_console_pid;
#endif
    bool_t            attached;
    struct
    {
        conf_object_t *obj;
        const char *port;
        const skyeye_serial_device_t *serial_dev;
    } serial_device;
    unsigned char   lastchar; // the last char outputed
    char            send_data_recoder[MAX_DATA_CACHE];
    uint32_t        send_count;
    uint16_t        socket_port;
    bool_t          open_client;
    char            *termString;
} uart_term_device;

#define MAX_STRING_LEN 0x10000
#define MAX_REC_NUM 1024
#define DBG_UART 1
#define DEBUG_UART(fmt, ...)                                     \
    do                                                           \
    {                                                            \
        if (DBG_UART)                                            \
        {                                                        \
            skyeye_log(Debug_log, __func__, fmt, ##__VA_ARGS__); \
        }                                                        \
    } while (0)

#ifndef __MINGW32__
#define term_write(fd, buf, len) \
    write((fd), (buf), (len))
#define term_read(fd, buf, len) \
    read((fd), (buf), (len))
#else
#define term_write(fd, buf, len) \
    send((fd), (buf), (len), 0)
#define term_read(fd, buf, len) \
    recv((fd), (buf), (len), 0)
#endif

#endif

/*
 * @file: uart_com.h
 * @brief: uart_com.h
 * @author: liubo
 * @version: 1.0
 * @date: 2016-08-25
 */

#ifndef __SKYEYE_UART_COM_H__
#define __SKYEYE_UART_COM_H__

#ifdef __MINGW32__
#include <windows.h>
#include <return_msg.h>
#endif

#define WIN_ASYNC_UART

#define MAX_FILE_NAME_LEN       128
#define ERROR_UART_NAME_LEN     5
#define ERROR_MSG               0
#define DEFAULT_BUFFER_SIZE     1024

/*
 * struct of rs232
 * set baudrate, bytesize, parity, stopbit
 */
typedef struct rs232_dev {
#ifdef __MINGW32__
    HANDLE hCom;

    #ifdef WIN_ASYNC_UART
        OVERLAPPED ov_read;
        OVERLAPPED ov_write;
    #endif /* WIN_ASYNC_UART */

#else
    int fd;
#endif /* __MINGW32__ */
    uint32_t baudrate;
    uint32_t bytesize;
    char parity;
    uint32_t stopbit;
}rs232_device;

/*
 * uart_com device struct type
 */
typedef struct skyeye_uart_com
{
    conf_object_t *obj;
    rs232_device *rs232;
    char filename[MAX_FILE_NAME_LEN];
    uint32_t BufferSize;
} uart_com_device;

#endif  /* __SKYEYE_UART_COM_H__ */

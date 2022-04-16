/*
 * @file: uart_com_serial_win.h
 * @brief: uart_com_serial_win.h
 * @author: liubo
 * @version: 1.0
 * @date: 2016-08-25
 */

#ifndef __UART_COM_SERIAL_WIN_H__
#define __UART_COM_SERIAL_WIN_H__

#include <string.h>

#ifdef __MINGW32__
#include <windows.h>

extern int set_opt(HANDLE hFile, int nSpeed, int nBits, char nEvent, int nStop, unsigned int buffersize);

#endif  /* __MINGW32__ */

#endif  /* __UART_COM_SERIAL_WIN_H__  */

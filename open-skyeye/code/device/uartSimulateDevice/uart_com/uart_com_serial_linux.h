/*
 * @file: uart_com_serial_linux.h
 * @brief: uart_com_serial_linux.h
 * @author:-liubo
 * @version: 1.0
 * @date: 2016-08-25
 */

#ifndef __UART_COM_SERIAL_LINUX_H__
#define __UART_COM_SERIAL_LINUX_H__

#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

extern int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop);
extern int open_port(const char *filename);

#endif

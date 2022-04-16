/*
 * © Copyright 2013 eJim Lee. All Rights Reserved.
 * File:    telnet.h
 * Email:   lyjforjob@gmail.com
 * Date:    2013-12-01
 */

#ifndef __TELNET_H__
#define __TELNET_H__

#include "uart_term.h"

#ifndef __MINGW32__
typedef int telnet_fd;
#else
typedef SOCKET telnet_fd;
#endif

int telnet_process(telnet_fd fd, void *str, int len);
int telnet_out_process(telnet_fd fd, void *str, int len, unsigned char *lastchar);

#endif

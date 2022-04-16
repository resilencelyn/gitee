/*
 * Copyright (C)
 * 2011 - Michael.Kang blackfin.kang@gmail.com
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/*
 * @file uart_term.c
 * @brief The implementation of skyeye uart term
 * @author David.Yu keweihk@gmail.com
 * @version 78.77
 * @date 2012-2-2
 */
/*
 * © Copyright 2013 eJim Lee. All Rights Reserved.
 * File:    uart_term.c
 * Email:   lyjforjob@gmail.com
 * Date:    2013-12-01
 * info:    change xterm and cmd to putty with telnet protocol
 */

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <simics/base_types.h>
#include "skyeye_thread.h"
#include "skyeye_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>

#include "uart_term.h"
#include "telnet.h"
#include "asnprintf.h"
#include "skyeye_log.h"
#include "skyeye_internal.h"

#ifdef __MINGW32__
#include <windows.h>
#endif
#ifndef SKYEYE_BIN
const char *default_bin_dir = "/opt/skyeye/bin/";
#else
char *default_bin_dir = SKYEYE_BIN;
#endif

#ifndef __MINGW32__
const char *uart_prog = "uart_instance";
#else
const char *uart_prog = "uart_instance.exe";
#define EADDRINUSE WSAEADDRINUSE
char *ReplaceAllChar(char *src, char oldChar, char newChar)
{
    char *head = src;
    while (*src != '\0')
    {
        if (*src == oldChar)
            *src = newChar;
        src++;
    }
    return head;
}
#endif

// putty is a telnet client
#ifndef __MINGW32__
const char *telnet_prog = "putty";
#else
#define EADDRINUSE WSAEADDRINUSE
const char *telnet_prog = "putty.exe";
#endif // __MINGW32__

int free_flag = 0;
static void update_serial_dev(uart_term_device *dev_uart);
static exception_t uart_term_raise(conf_object_t *object, int line_no)
{
    /* Get vic interrupt interface */
    general_signal_intf *vic_signal = (general_signal_intf *)SKY_get_iface(object, GENERAL_SIGNAL_INTF_NAME);
    vic_signal->raise_signal(vic_signal->conf_obj, line_no);

    return 0;
}

static exception_t uart_term_down(conf_object_t *object, int line_no)
{
    return 0;
}

void increase_output(conf_object_t *obj, char *data, int count)
{
    uart_term_device *dev = obj->obj;
    int i;
    for (i = 0; i < count; i++)
    {
        if (dev->send_count == MAX_DATA_CACHE)
        {
            dev->send_count = 0;
        }
        dev->send_data_recoder[dev->send_count++] = data[i];
    }
}
static size_t uart_term_read(conf_object_t *opaque, void *buf, size_t count)
{
    uart_term_device *dev = opaque->obj;
    rec_t *receive = dev->receive;
    int i = 0;
    char *rec_buf = buf;
    if (dev->attached && receive->mutex == 1)
    {
        /* move the charaters in buffer */
        while (i < count && (receive->rec_tail < receive->rec_count))
        {
            if (receive->rec_head >= receive->rec_tail)
            {
                /* No data */
                receive->rec_head = 0;
                receive->rec_tail = 0;
                //return Not_found_exp;
                break;
            }
            else
            {
                rec_buf[i++] = receive->rec_buf[receive->rec_head++];
                if (receive->rec_head == receive->rec_tail)
                {
                    /* data have been read*/
                    receive->rec_head = 0;
                    receive->rec_tail = 0;
                    //unlock the mutex, until reach the end of the buffer
                    receive->mutex = 0;
                }
            }
        }
        //DEBUG_UART("rec_buf %s\n", rec_buf);
    }
    else
    {
        //return Not_found_exp;
    }

    return i;
}

static exception_t _uart_term_write(uart_term_device *dev, void *buf, size_t count)
{
    static int wr_cnt = 0;
    int ret = -1;
    if (dev->telnet_mode)
    {
        ret = telnet_out_process(dev->socket, buf, count, &dev->lastchar);
    }
    if (ret < 0)
    {
        return Not_found_exp;
    }

    strncat(dev->termString, (char *)buf, 1);
    wr_cnt++;
    if (wr_cnt > MAX_STRING_LEN)
    {
        memset(dev->termString, 0, MAX_STRING_LEN);
        wr_cnt = 0;
    }
    ret = term_write(dev->socket, buf, count);
    if (ret < 0)
    {
        return Invarg_exp;
    }
    return No_exp;
}

static exception_t uart_term_write(conf_object_t *opaque, void *buf, size_t count)
{
    int try_count = 0;
    uart_term_device *dev = opaque->obj;
again:
    if (dev->attached)
    {
        increase_output(opaque, buf, count);
        return _uart_term_write(dev, buf, count);
    }
    else
    {
        /* wait about 1s for term attaching Ok */
        if (try_count++ > 1000)
        {
            //fprintf(stderr, "Terminal output timeout\n");
            //fprintf(stderr, "Output: ");
            size_t i;
            for (i = 0; i < count; i++)
            {
                char c = *((char *)buf + i);
                //fprintf(stderr, "'%c'{0x%hhx}", c, c);
            }
            //fprintf(stderr, "\n");
            return Invarg_exp;
        }
        usleep(1000);
        goto again;
    }
    return No_exp;
}

static void term_wait_string(conf_object_t *obj, const char *string)
{
    uart_term_device *dev = obj->obj;
    bool_t ret = False;

    while(free_flag == 0)
    {
        if (strstr(dev->termString, string) != NULL)
            break;
#ifdef __MINGW32__
        Sleep(5);
#else
        usleep(5);
#endif
    }
}

static void term_write_string(conf_object_t *obj, const char *string)
{
    uart_term_device *dev = obj->obj;
    rec_t *receive = dev->receive;

    while(*string != '\0' && free_flag == 0)
    {
        receive->rec_buf[receive->rec_tail] = *string;
        receive->rec_tail++;
        string++;
        update_serial_dev(dev);
#ifdef __MINGW32__
        Sleep(10);
#else
        usleep(10);
#endif
    }
}

static exception_t serial_write(conf_object_t *obj, uint32_t value)
{
    uart_term_device *dev = obj->obj;
    if (dev->attached)
    {
        return _uart_term_write(dev, &value, 1);
    }
    return Access_exp;
}

static exception_t serial_ready(conf_object_t *obj)
{
    uart_term_device *dev = obj->obj;
    int try_count = 0;

    while (!dev->attached)
    {
        if (try_count++ > 1000)
        {
            return Access_exp;
        }
        usleep(1000);
    }
    return No_exp;
}

#ifdef __MINGW32__
static DWORD create_term(uart_term_device *dev_uart, int port)
{
#else
static pid_t create_term(uart_term_device *dev_uart, int port)
{
#endif
    pid_t pid;

    char port_str[32];
    sprintf(port_str, "%d", port);

#ifdef __MINGW32__
    default_bin_dir = get_bin_env_path("BIN");
#endif

    // telnet can set a windows title
    char *hello_world = "SkyEye Terminal: ";
    int len = strlen(hello_world) + strlen(dev_uart->obj->objname) + 1;
    char *win_title = skyeye_mm(sizeof(char) * len);
    sprintf(win_title, "%s%s", hello_world, dev_uart->obj->objname);

    // link a xterm path
    len = strlen(default_bin_dir) + strlen(uart_prog) + 1;
    char *uart_instance_prog = skyeye_mm(sizeof(char) * len);
    sprintf(uart_instance_prog, "%s%s", default_bin_dir, uart_prog);

    dev_uart->telnet_mode = True;
#ifndef __MINGW32__
    switch (pid = fork())
    {
    case -1:
    {
        perror("The fork failed!");
        break;
    }
    case 0:
    {
        int ret = execlp("putty", "putty", "-title", win_title,
                         "-telnet", "-P", port_str, "127.0.0.1", (char *)NULL);
        // if arrive here, something wrong happend
        perror("exec putty");
        //fprintf(stderr, "SkyEye can't start a putty client and will start a xterm.\n");
        SKY_log_error(dev_uart->obj, 1, "SkyEye can't start a putty client and will start a xterm.\n");
        dev_uart->telnet_mode = False;
#ifdef DBG_XTERM
        ret = execlp("xterm", "xterm", "-hold", "-e", uart_instance_prog, dev_uart->obj->objname, port_str, (char *)NULL);
#else
        ret = execlp("xterm", "xterm", "-e", uart_instance_prog, dev_uart->obj->objname, port_str, (char *)NULL);
#endif
        perror("exec xterm");
        //fprintf(stderr, "SKYEYE Error: We need xterm to run the console of uart. "
        //		"Please check if you installed it correctly.\n");
        SKY_log_error(dev_uart->obj, 1, "SKYEYE Error: We need xterm to run the console of uart. "
                                        "Please check if you installed it correctly.\n");
        _exit(-1);
    }
    default:
        break;
    }
    skyeye_free(win_title);
    skyeye_free(uart_instance_prog);
    return pid;
#else

    char *telnet_cmdline = NULL;
    if (-1 == asnprintf(&telnet_cmdline, 256, "start -title \"%s\" -telnet -P %s 127.0.0.1", win_title, port_str))
    {
        skyeye_log(Error_log, __FUNCTION__, "telnet_cmdline: asnprintf() source string beyond the specified size");
    }

    STARTUPINFO startup_info;
    GetStartupInfo(&startup_info);
    PROCESS_INFORMATION process_info;
    ZeroMemory(&process_info, sizeof(process_info));
    ZeroMemory(&startup_info, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    startup_info.dwFlags = STARTF_USESHOWWINDOW;
    startup_info.wShowWindow = SW_SHOW;

    char *putty_path = NULL;
    if (-1 == asnprintf(&putty_path, MAX_PATH, "%s\\..\\..\\..\\%s", default_bin_dir, telnet_prog))
    {
        skyeye_log(Error_log, __FUNCTION__, "putty_path: asnprintf() source string beyond the specified size");
    }

    if (!CreateProcess(putty_path, telnet_cmdline, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info))
    {
        // if arrive here, somethong wrong happend
        SKYEYE_ERR("ERROR: system failed!");
        //fprintf(stderr, "Start a putty failed and will start a CMD");
        SKY_log_error(dev_uart->obj, 1, "Start a putty failed and will start a CMD");
        dev_uart->telnet_mode = False;
        char cmdline[2048] = "start ";
        ReplaceAllChar(uart_instance_prog, '/', '\\');
        strcat(cmdline, uart_instance_prog);
        strcat(cmdline, " ");
        strcat(cmdline, dev_uart->obj->objname);
        strcat(cmdline, " ");
        strcat(cmdline, port_str);
        if (system(cmdline) != 0)
        {
            SKYEYE_ERR("ERROR: system failed!");
            return -1;
        }
    }
    insane_free(putty_path);
    skyeye_free(win_title);
    skyeye_free(uart_instance_prog);
    return process_info.dwProcessId;
#endif // MINGW32
}

/**
 * @brief update_serial_dev write to serial device
 *
 * @param dev_uart self
 */
static void update_serial_dev(uart_term_device *dev_uart)
{
    const skyeye_serial_device_t *serial_intf = dev_uart->serial_device.serial_dev;
    if (!(serial_intf && serial_intf->write))
    {
        return;
    }

    conf_object_t *obj = dev_uart->serial_device.obj;
    exception_t (*write)(conf_object_t *, uint32_t) = serial_intf->write;
    exception_t (*receive_ready)(conf_object_t *) = serial_intf->receive_ready;

    rec_t *receive = dev_uart->receive;
    /* we need not mutex here */
    if (dev_uart->attached)
    {
        if (receive->rec_tail >= receive->rec_count)
        {
            /* maybe something wrong */
            /* rec_buf overflow */
            receive->rec_head = 0;
            receive->rec_tail = 0;
            return;
        }
        while (receive->rec_head < receive->rec_tail)
        {
            exception_t ret;
            uint32_t value = (uint32_t)receive->rec_buf[receive->rec_head++];
            if (write(obj, value) == No_exp)
            {
                /* Write OK */
                continue;
            }
            if (!receive_ready)
            {
                /* End update if write failed and no receive_ready callback */
                continue;
            }
            if (receive_ready(obj) != No_exp)
            {
                /* End update if ready failed */
                continue;
            }
            /* try to write again but not check return value */
            write(obj, value);
        }
        /* all data is writed */
        receive->rec_head = 0;
        receive->rec_tail = 0;
    }
}

static int uart_term_receive(uart_term_device *dev_uart)
{
    int res;
    rec_t *receive = dev_uart->receive;

#ifndef __MINGW32__
    struct pollfd fds;
#define POLL_TIMEOUT -1 /* wait forever ? FIXME ? */
    fds.fd = dev_uart->socket;
    fds.events = POLLIN | POLLPRI;
#if HOST_OS_SOLARIS9 /* { */
    fds.events |= POLLRDNORM | POLLRDBAND;
#endif /* } */
    fds.revents = 0;
    res = poll(&fds, 1, POLL_TIMEOUT);
#else  //__MINGW32__
    fd_set fdread;
    FD_ZERO(&fdread);
    FD_SET(dev_uart->socket, &fdread);
    select(0, &fdread, NULL, NULL, NULL);
#endif //__MINGW32__

#ifndef __MINGW32__
    if (fds.revents & POLLIN)
    {
#else
    if (FD_ISSET(dev_uart->socket, &fdread))
    {
#endif
        if (receive->rec_tail >= receive->rec_count)
        {
            //fprintf(stderr, "Overflow for uart link.\n");
            //SKY_log_error(dev_uart->obj, 3, "Overflow for uart link.\n");
        }
        else
        {
            res = term_read(dev_uart->socket,
                            receive->rec_buf + receive->rec_tail,
                            receive->rec_count - receive->rec_tail);
        }
        if (res == 0)
        {
            /* a read of 0 bytes is an EOF */
            dev_uart->attached = 0;
#ifndef __MINGW32__
            close(dev_uart->socket);
#else
            closesocket(dev_uart->socket);
            WSACleanup();
#endif
        }
        else if (res < 0)
        {
            /* FIXME!!! I think we need a mechanism that
			 * notice outside terminal have disconnected
			 */
            dev_uart->attached = 0;
        }
        else
        {
            if (dev_uart->telnet_mode)
            {
                res = telnet_process(dev_uart->socket,
                                     receive->rec_buf + receive->rec_head, res);
            }
            /* expand receive buf */
            receive->rec_tail += res;
            receive->mutex = 1;
            //DEBUG_UART("receive %s\n", receive->rec_buf);
            update_serial_dev(dev_uart);
        }
    }
    return 0;
}

static void *create_uart_console(uart_term_device *dev_uart)
{
    //return (void *)0;
    conf_object_t *uart = dev_uart->obj;
    struct hostent *hp;
    int on, length;
    struct sockaddr_in server, from;
    struct linger so_linger;
    char *froms;

#ifndef __MINGW32__
    int term_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (term_socket < 0)
        SKYEYE_ERR("opening stream socket");

    /* enable the reuse of this socket if this process dies */
    if (setsockopt(term_socket, SOL_SOCKET, SO_REUSEADDR, (uint8_t *)&on, sizeof(on)) < 0)
    {
        SKYEYE_ERR("turning on REUSEADDR");
    }
    if (setsockopt(term_socket, SOL_SOCKET, SO_REUSEPORT, (uint8_t *)&on, sizeof(on)) < 0)
    {
        SKYEYE_ERR("turning on REUSEPORT");
    }
    so_linger.l_onoff = 1;
    so_linger.l_linger = 0;
    if (setsockopt(term_socket, SOL_SOCKET, SO_LINGER, (uint8_t *)&so_linger, sizeof(so_linger)) < 0)
    {
        SKYEYE_ERR("set socket SO_LINGER");
    }
#else
    int nsize;
    int err;
    struct timeval tv;
    WSADATA wsaData;
    SOCKET term_socket = INVALID_SOCKET;
    BOOL bOptVal = FALSE;
    int bOptLen = sizeof(BOOL);

    /*
     * initiates use of the Winsock DLL by a process
     * shenoubang modified it 2012-12-4
     */
    err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        //SKY_log_error(uart, 1, "WSAStartup failed with error: %d\n", err);
        return (void *)1;
    }
    /* creates a socket that is bound to a specific transport service provider */
    term_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (term_socket == INVALID_SOCKET)
    {
        SKY_log_error(dev_uart->obj, 1, "socket function failed with error: %u\n", WSAGetLastError());
        WSACleanup();
        return (void *)1;
    }
    bOptVal = TRUE;
    err = setsockopt(term_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&bOptVal, bOptLen);
    if (err == SOCKET_ERROR)
    {
        //SKY_log_error(dev_uart->obj, 1, "setsockopt for RESUEADDR failed with error: %u\n", WSAGetLastError());
        return (void *)1;
    }
#endif
retry:
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(dev_uart->socket_port); /* bind to an OS selected local port */

#ifndef __MINGW32__
    if (bind(term_socket, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
#else
    if (bind(term_socket, (SOCKADDR *)&server, sizeof(server)) < 0)
    {
#endif
        switch (errno)
        {
        case EAGAIN:
            goto retry;
        case EADDRINUSE:
            //SKY_log_error(dev_uart->obj, 1, "Port is already in use\n");
            break;
        default:
            //SKY_log_error(dev_uart->obj, 1, "binding tcp stream socket");
            break;
        }
    }

    length = sizeof(server);
    if (getsockname(term_socket, (struct sockaddr *)&server, &length) == -1)
    {
        SKYEYE_ERR("getting socket name");
    }

    listen(term_socket, 1);
    /* Create the client xterm */
    if (dev_uart->open_client != 0)
    {
        dev_uart->uart_console_pid = create_term(dev_uart, ntohs(server.sin_port));
    }
    /* main loop */
    do
    {
        if (!dev_uart->attached)
        {
            //SKY_log_info(0, uart, 1, "Waiting for connection to %s: %d\n", dev_uart->obj->objname, ntohs(server.sin_port));
            length = sizeof(from);
            /* save the uart socket */
            dev_uart->socket = accept(term_socket, (struct sockaddr *)&from, (int *)&length);

            hp = gethostbyaddr((char *)&from.sin_addr, 4, AF_INET);
            if (hp == (struct hostent *)0)
            {
                froms = inet_ntoa(from.sin_addr);
                //SKY_log_info(0, uart, 1, "cant resolve hostname for %s\n", froms);
            }
            else
            {
                froms = hp->h_name;
            }
            dev_uart->attached = 1;
        }
        else
        {
            /* begin receive data. */
            uart_term_receive(dev_uart);
        } //if (fds.revents & POLLIN)
    } while (1);

    return (void *)0;
}

static const skyeye_serial_device_t serial_device = {
    .write = serial_write,
    .receive_ready = serial_ready,
};

static exception_t serial_device_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    uart_term_device *dev = obj->obj;
    dev->serial_device.port = port;
    dev->serial_device.obj = obj2;
    dev->serial_device.serial_dev = (skyeye_serial_device_t *)SKY_get_iface(dev->serial_device.obj, SERIAL_DEVICE_INTERFACE);
    if (dev->serial_device.serial_dev == NULL)
    {
        if (dev->serial_device.obj == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Get 'skyeye_serial_device_t' interface from NULL object fail.");
        }
        else
        {
            skyeye_log(Error_log, __FUNCTION__, "Get 'skyeye_serial_device_t' interface from %s fail.", obj2->objname);
        }
    }
    return No_exp;
}

static exception_t serial_device_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    uart_term_device *dev = obj->obj;
    *obj2 = dev->serial_device.obj;
    *port = NULL;
    return No_exp;
}

static attr_value_t get_attr_socket_port(void *arg, conf_object_t *conf_obj, attr_value_t *idx)
{
    uart_term_device *dev = conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(dev->socket_port);
    return value;
}
static exception_t set_attr_socket_port(void *arg, conf_object_t *conf_obj, attr_value_t *value, attr_value_t *idx)
{
    uart_term_device *dev = conf_obj->obj;
    dev->socket_port = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_open_client(void *arg, conf_object_t *conf_obj, attr_value_t *idx)
{
    uart_term_device *dev = conf_obj->obj;
    attr_value_t value = SKY_make_attr_boolean(dev->open_client);
    return value;
}
static exception_t set_attr_open_client(void *arg, conf_object_t *conf_obj, attr_value_t *value, attr_value_t *idx)
{
    uart_term_device *dev = conf_obj->obj;
    dev->open_client = SKY_attr_boolean(*value);
    return No_exp;
}

void uart_term_register_attribute(conf_class_t *clss)
{

    SKY_register_attribute(clss, "socket_port", get_attr_socket_port, NULL, set_attr_socket_port, NULL, SKY_Attr_Optional, "uinteger", "telnet socket port");
    SKY_register_attribute(clss, "open_client", get_attr_open_client, NULL, set_attr_open_client, NULL, SKY_Attr_Optional, "boolean", "open Putty telnet client");
}

static conf_object_t *new_uart_term(const char *obj_name)
{
    uart_term_device *dev_uart = skyeye_mm_zero(sizeof(uart_term_device));
    rec_t *receive = skyeye_mm_zero(sizeof(rec_t));
    dev_uart->obj = new_conf_object(obj_name, dev_uart);

    /* initial recevie */
    dev_uart->receive = receive;
    receive->rec_buf = skyeye_mm_zero(MAX_REC_NUM);
    receive->rec_head = 0;
    receive->rec_tail = 0;
    receive->mutex = 0;
    receive->rec_count = MAX_REC_NUM;

    /* Register io function to the object */
#if 0
    skyeye_uart_intf* uart_method = skyeye_mm_zero(sizeof(skyeye_uart_intf));
    uart_method->conf_obj = dev_uart->obj;
    uart_method->read = uart_term_read;
    uart_method->write = uart_term_write;
    SKY_register_interface(uart_method, obj_name, SKYEYE_UART_INTF);
#endif

    dev_uart->attached = 0;
    dev_uart->socket = -1;
    dev_uart->mod = 3;
    dev_uart->telnet_mode = True;

    dev_uart->send_count = 0;
    dev_uart->socket_port = 0;
    dev_uart->open_client = True;
    dev_uart->termString = skyeye_mm_zero(MAX_STRING_LEN);

    free_flag = 0;
#ifndef __MINGW32__
    usleep(1000);
#else
    Sleep(100);
#endif

    return dev_uart->obj;
}

exception_t config_uart_term(conf_object_t *dev)
{
    uart_term_device *dev_uart = dev->obj;
    pthread_t pthread_id;
    create_thread(create_uart_console, dev_uart, &pthread_id);
    return No_exp;
}

exception_t free_uart_term(conf_object_t *dev)
{
    uart_term_device *dev_uart = dev->obj;
    free_flag = 1;

#ifndef __MINGW32__
    usleep(100);
#else
    Sleep(100);
#endif
    /* Terminate telnet client*/
    if (dev_uart->open_client != 0)
    {
#ifndef __MINGW32__
        kill(dev_uart->uart_console_pid, SIGTERM);
#else
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dev_uart->uart_console_pid);
        if (hProcess == NULL)
            return Unknown_exp;
        TerminateProcess(hProcess, 0);
#endif
    }
    /* close telnet server socket*/
#ifndef __MINGW32__
    close(dev_uart->socket);
#else
    closesocket(dev_uart->socket);
#endif

    skyeye_free(dev_uart->termString);
    skyeye_free(dev_uart->receive->rec_buf);
    skyeye_free(dev_uart->receive);
    skyeye_free(dev_uart->obj);
    skyeye_free(dev_uart);

    return No_exp;
}

void init_uart_term()
{
    static skyeye_class_t class_data = {
        .class_name = "uart_term",
        .class_desc = "uart term",
        .new_instance = new_uart_term,
        .free_instance = free_uart_term,
        .config_instance = config_uart_term,
        .get_attr = NULL,
        .set_attr = NULL,
        .module_type = SKYML_OFFCHIP,
    };

    conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);
    static const skyeye_uart_intf uart_iface = {
        .read = uart_term_read,
        .write = uart_term_write,
        .wait_string = term_wait_string,
        .write_string = term_write_string
    };
    SKY_register_iface(clss, SKYEYE_UART_INTF, &uart_iface);

    static const skyeye_serial_device_t serial_device = {
        .write = serial_write,
        .receive_ready = NULL};
    SKY_register_iface(clss, SERIAL_DEVICE_INTERFACE, &serial_device);

    static const struct InterfaceDescription ifaces[] = {
        (struct InterfaceDescription){
            .name = SKYEYE_UART_INTF,
            .iface = &uart_iface,
        },
        (struct InterfaceDescription){
            .name = SERIAL_DEVICE_INTERFACE,
            .iface = &serial_device,
        }

    };
    static const struct ConnectDescription connects[] = {
        (struct ConnectDescription){
            .name = SERIAL_DEVICE_INTERFACE,
            .set = serial_device_set,
            .get = serial_device_get,
        }};
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    const char *const groups[] = {
        [0] = "Init",
        [1] = "Send",
        [2] = "Recv",
        [3] = NULL,
    };
    SKY_log_register_groups(clss, groups);
    uart_term_register_attribute(clss);
}

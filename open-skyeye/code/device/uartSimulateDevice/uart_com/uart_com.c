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
 * @file uart_com.c
 * @brief The implementation of skyeye uart to rs232
 * @author-
 * @version 1.0
 * @date 2016-08-25
 */

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_uart_ops.h>
#include "skyeye_thread.h"
#include "skyeye_dev_intf.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>
#include "uart_com.h"

#ifdef __MINGW32__
#define SKYEYE_UART_COM_POSIX_SUPPORTED 0
#else
#define SKYEYE_UART_COM_POSIX_SUPPORTED 1
#endif

#if SKYEYE_UART_COM_POSIX_SUPPORTED

#include "uart_com_serial_linux.h"

/*
 * This function open_file
 * 1 open serial, and return file descriptor
 * 2 configurate serial via fd ( return by open function )
 */
static void open_file(uart_com_device* dev){
    dev->rs232->fd = open_port(dev->filename);

    if(-1 == dev->rs232->fd){
        skyeye_log(Error_log, __FUNCTION__, "Can not open file: %s\n", dev->filename);
        return;
    }

    if(-1 == set_opt(dev->rs232->fd,      /* file descriptor */
                dev->rs232->baudrate,
                dev->rs232->bytesize,
                dev->rs232->parity,
                dev->rs232->stopbit))
    {
        skyeye_log(Error_log, __FUNCTION__, "Set file: %s ERROR!\n", dev->filename);
        if(-1 != dev->rs232->fd)
            close(dev->rs232->fd);
        return;
    }

    return;
}

static exception_t uart_com_read(conf_object_t *opaque, void* buf, size_t count){
    uart_com_device *dev = opaque->obj;
    if(-1 != dev->rs232->fd){
        read(dev->rs232->fd, buf, count);
    }else{
        skyeye_log(Error_log, __FUNCTION__, "No file point is opend!\n");
    }

    return No_exp;
}

static exception_t uart_com_write(conf_object_t *opaque, void* buf, size_t count){
    uart_com_device *dev = opaque->obj;

    if(-1 != dev->rs232->fd){
        write(dev->rs232->fd, buf, count);
    }else{
        skyeye_log(Error_log, __FUNCTION__, "No file point is opend!\n");
    }

    return No_exp;
}


#else  /* windows */

#include "uart_com_serial_win.h"

/*
 * This function open_file
 * 1 open serial, and return file descriptor
 * 2 configurate serial via fd ( return by open function )
 */
static void open_file(uart_com_device* dev){
#ifdef WIN_ASYNC_UART
    dev->rs232->hCom = CreateFile(dev->filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
#else
    dev->rs232->hCom = CreateFile(dev->filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
#endif /* WIN_ASYNC_UART */
    if(INVALID_HANDLE_VALUE == dev->rs232->hCom)
    {
        if (strlen(dev->filename) == ERROR_UART_NAME_LEN)
        {
            fprintf(stderr, "[uart_com] UART ID Error - Please Use  \"\\\\\\\\\\\\\\\\.\\\\\\\\+NUM\"  if COMID > 9");
        }
        else
        {
            fprintf(stderr, "[uart_com] %s Open Error - Please Select Other Uart\n", dev->filename);
        }
        return;
    }

    if(-1 == set_opt(dev->rs232->hCom,
                dev->rs232->baudrate,
                dev->rs232->bytesize,
                dev->rs232->parity,
                dev->rs232->stopbit,
                dev->BufferSize))
    {
        skyeye_log(Error_log, __FUNCTION__, "Set comm fail!\n");
        if(INVALID_HANDLE_VALUE != dev->rs232->hCom)
            CloseHandle(dev->rs232->hCom);
        return;
    }
#ifdef WIN_ASYNC_UART
    dev->rs232->ov_read.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    ZeroMemory(&dev->rs232->ov_write, sizeof(&dev->rs232->ov_write));
    if(dev->rs232->ov_write.hEvent != NULL)
    {
        ResetEvent(dev->rs232->ov_write.hEvent);
        dev->rs232->ov_write.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    }

    COMMTIMEOUTS CommTimeouts;
    GetCommTimeouts(dev->rs232->hCom, &CommTimeouts);
    CommTimeouts.ReadIntervalTimeout = 2;
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;
    CommTimeouts.ReadTotalTimeoutConstant = 0;
    CommTimeouts.WriteTotalTimeoutMultiplier = 0;
    CommTimeouts.WriteTotalTimeoutConstant = 0;
    SetCommTimeouts(dev->rs232->hCom, &CommTimeouts);
    SetCommMask(dev->rs232->hCom, EV_ERR | EV_RXCHAR);
#else
    COMMTIMEOUTS CommTimeouts;
    GetCommTimeouts(dev->rs232->hCom, &CommTimeouts);
    CommTimeouts.ReadIntervalTimeout = MAXDWORD;
    CommTimeouts.ReadTotalTimeoutMultiplier = 10;
    CommTimeouts.ReadTotalTimeoutConstant = 10;
    CommTimeouts.WriteTotalTimeoutMultiplier = 50;
    CommTimeouts.WriteTotalTimeoutConstant = 100;
    SetCommTimeouts(dev->rs232->hCom, &CommTimeouts);
#endif /* WIN_ASYNC_UART */
    return;
}

static exception_t uart_com_read(conf_object_t *opaque, void* buf, size_t count){
#ifdef WIN_ASYNC_UART
    uart_com_device *dev = opaque->obj;
    DWORD readsize = 0;
    BOOL Status = FALSE;
    DWORD Error;
    while(INVALID_HANDLE_VALUE != dev->rs232->hCom)
    {
        ClearCommError(dev->rs232->hCom, &Error, NULL);
        dev->rs232->ov_read.Offset = 0;
        Status = ReadFile(dev->rs232->hCom, buf, count, &readsize, &dev->rs232->ov_read);
        if(Status == FALSE && (GetLastError() == ERROR_IO_PENDING))
        {
            Status = GetOverlappedResult(dev->rs232->hCom, &dev->rs232->ov_read, &readsize, TRUE);
        }
        if((Status != FALSE) && (readsize > 0))
        {
            return No_exp;
        }
        else
        {
            PurgeComm(dev->rs232->hCom, PURGE_RXCLEAR | PURGE_RXABORT);
            return Unknown_exp;
        }
    }
#else
    uart_com_device *dev = opaque->obj;
    DWORD readsize = 0;

    if(INVALID_HANDLE_VALUE != dev->rs232->hCom){
        ReadFile(dev->rs232->hCom, buf, count, &readsize, NULL);
        if (readsize != 0)
            return No_exp;
    }else{
        skyeye_log(Error_log, __FUNCTION__, "No file point is opend!\n");
    }

    return Unknown_exp;
#endif /* WIN_ASYNC_UART */
}

static exception_t uart_com_write(conf_object_t *opaque, void* buf, size_t count){
#ifdef WIN_ASYNC_UART
    uart_com_device *dev = opaque->obj;
    DWORD writesize = 0;
    DWORD Error;
    if(ClearCommError(dev->rs232->hCom, &Error, NULL))
    {
        PurgeComm(dev->rs232->hCom, PURGE_TXABORT |PURGE_TXCLEAR);
    }
    if(!WriteFile(dev->rs232->hCom, buf, count, &writesize, &dev->rs232->ov_write))
    {
        if(!GetLastError() == ERROR_IO_PENDING)
        {
            while(!GetOverlappedResult(dev->rs232->hCom, &dev->rs232->ov_write, &writesize, FALSE))
            {
                if(!GetLastError() == ERROR_IO_INCOMPLETE)
                {
                    continue;
                }
                else
                {
                    ClearCommError(dev->rs232->hCom, &Error, NULL);
                    return Unknown_exp;
                }
            }
        }
    }
    return No_exp;
#else
    uart_com_device *dev = opaque->obj;
    DWORD writesize = 0;

    if(INVALID_HANDLE_VALUE != dev->rs232->hCom){
        WriteFile(dev->rs232->hCom, buf, count, &writesize, NULL);
    }else{
        skyeye_log(Error_log, __FUNCTION__, "No file point is opend!\n");
    }

    return No_exp;
#endif /* WIN_ASYNC_UART */
}

#endif  /* SKYEYE_UART_COM_POSIX_SUPPORT */


static attr_value_t get_attr_serialname(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
    uart_com_device* dev = conf_obj->obj;
    attr_value_t value = SKY_make_attr_string(dev->filename);
    return value;
}
static exception_t set_attr_serialname(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
    uart_com_device* dev = conf_obj->obj;
    const char *filename;
    filename = SKY_attr_string(*value);
    strncpy(dev->filename, filename, MAX_FILE_NAME_LEN);

    return No_exp;
}

static attr_value_t get_attr_baudrate(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
    uart_com_device* dev = conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(dev->rs232->baudrate);
    return value;
}
static exception_t set_attr_baudrate(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
    uart_com_device* dev = conf_obj->obj;
    dev->rs232->baudrate = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_bytesize(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
    uart_com_device* dev = conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(dev->rs232->bytesize);
    return value;
}
static exception_t set_attr_bytesize(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
    uart_com_device* dev = conf_obj->obj;
    dev->rs232->bytesize = SKY_attr_uinteger(*value);



    return No_exp;
}

static attr_value_t get_attr_parity(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
    uart_com_device* dev = conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(dev->rs232->parity);
    return value;
}
static exception_t set_attr_parity(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
    uart_com_device* dev = conf_obj->obj;

    const char* rs232_parity;
    rs232_parity = SKY_attr_string(*value);
    switch(rs232_parity[0]){
        case 'N':  //NOPARITY
            dev->rs232->parity = 'N';
            break;
        case 'O':  //ODDPARITY
            dev->rs232->parity = 'O';
            break;
        case 'E':  //EVENTPARITY
            dev->rs232->parity = 'E';
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Unknown parity! Please confirm!\n");
            break;
    }

    return No_exp;
}

static attr_value_t get_attr_stopbit(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
    uart_com_device* dev = conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(dev->rs232->stopbit);
    return value;
}
static exception_t set_attr_stopbit(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
    uart_com_device* dev = conf_obj->obj;
    dev->rs232->stopbit = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_buffersize(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
    uart_com_device* dev = conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(dev->BufferSize);
    return value;
}

static exception_t set_attr_buffersize(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
    uart_com_device* dev = conf_obj->obj;
    dev->BufferSize = SKY_attr_uinteger(*value);

    return No_exp;
}

void uart_com_register_attribute(conf_class_t* clss) {
    SKY_register_attribute(clss, "com_parity", get_attr_parity, NULL, set_attr_parity, NULL, SKY_Attr_Optional, "string", "rs232 parity");
    SKY_register_attribute(clss, "com_baudrate", get_attr_baudrate, NULL, set_attr_baudrate, NULL, SKY_Attr_Optional, "uinteger", "rs232 baudrate");
    SKY_register_attribute(clss, "com_serialname", get_attr_serialname, NULL, set_attr_serialname, NULL, SKY_Attr_Optional, "string", "rs232 serialname");
    SKY_register_attribute(clss, "com_stopbit", get_attr_stopbit, NULL, set_attr_stopbit, NULL, SKY_Attr_Optional, "uinteger", "rs232 stopbit");
    SKY_register_attribute(clss, "com_bytesize", get_attr_bytesize, NULL, set_attr_bytesize, NULL, SKY_Attr_Optional, "uinteger", "rs232 bytesize");
    SKY_register_attribute(clss, "com_buffersize", get_attr_buffersize, NULL, set_attr_buffersize, NULL, SKY_Attr_Optional, "uinteger", "rs232 buffersize");
}

static void init_filename(conf_object_t *obj)
{
    uart_com_device* dev = obj->obj;
    int i;
    for (i = 0; i < MAX_FILE_NAME_LEN; i++){
        dev->filename[i] = 0;
    }
    return;
}

static conf_object_t* new_uart_com(const char* obj_name){
    uart_com_device* dev = (uart_com_device*)skyeye_mm_zero(sizeof(uart_com_device));
    if(NULL == dev){
        skyeye_log(Error_log, __FUNCTION__, "Memory allocation fail! New device<uart_com_device> fail!\n");
        return NULL;
    }

    dev->rs232 = (rs232_device *)skyeye_mm_zero(sizeof(rs232_device));
    if(NULL == dev->rs232){
        skyeye_log(Error_log, __FUNCTION__, "Memory allocation fail! New device<rs232 device> fail!\n");
        return NULL;
    }

    dev->obj = new_conf_object(obj_name, dev);
    if(NULL == dev->obj){
        skyeye_log(Error_log, __FUNCTION__, "Memory allocation fail! New conf_object fail!\n");
        return NULL;
    }

    init_filename(dev->obj);

#if SKYEYE_UART_COM_POSIX_SUPPORTED
    dev->rs232->fd = -1;
#else
    dev->rs232->hCom = -1;
#endif

    return dev->obj;
}

static exception_t config_uart_com(conf_object_t *obj)
{
    uart_com_device* dev = obj->obj;
    dev->BufferSize = DEFAULT_BUFFER_SIZE;
    open_file(dev);

    return No_exp;
}

exception_t free_uart_com(conf_object_t* obj){
    uart_com_device* dev = obj->obj;

#if SKYEYE_UART_COM_POSIX_SUPPORTED
    if(-1 != dev->rs232->fd)
        close(dev->rs232->fd);
#else
    if(INVALID_HANDLE_VALUE != dev->rs232->hCom)
        CloseHandle(dev->rs232->hCom);
#endif

    return No_exp;
}

void init_uart_com(){
    static skyeye_class_t class_data = {
        .class_name = "uart_com",
        .class_desc = "uart_com",
        .new_instance = new_uart_com,
        .free_instance = free_uart_com,
        .config_instance = config_uart_com,
        .get_attr = NULL,
        .set_attr = NULL,
        .module_type = SKYML_OFFCHIP,
    };
    conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);

    static const uart_com_intf uart_com_iface = {
        .read = uart_com_read,
        .write = uart_com_write
    };
    SKY_register_iface(clss, UART_COM_INTF, &uart_com_iface);

    static const struct InterfaceDescription ifaces[] = {
        (struct InterfaceDescription) {
            .name = UART_COM_INTF,
            .iface = &uart_com_iface,
        }
    };

    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, CONNECTS_NULL);

    uart_com_register_attribute(clss);
    return;
}


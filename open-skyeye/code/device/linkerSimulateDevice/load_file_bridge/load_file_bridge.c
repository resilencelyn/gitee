/* Copyright (C)
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
/**
 * @file load_file_bridge.c
 */
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_iface.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_obj.h>
#include <skyeye_core.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <sim_control.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>

#ifndef __MINGW32__
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#else
#include <windows.h>
#include <winsock2.h>
#endif
#include <stdlib.h>

#define DEBUG
#include <skyeye_log.h>

#include "load_file_bridge.h"
#include "load_file_fifo.h"
#include <skyeye_system.h>

void send_data_callback(conf_object_t* obj)
{
    load_file_bridge *dev = obj->obj;
    unsigned char data;

    if(CheckCanReadNum(dev->send_fifo) == 0)
    {
        return;
    }
    RW_WRLOCK(dev->lock);
    ReadFIFO(dev->send_fifo, &data, 1);
    RW_UNLOCK(dev->lock);

    if (dev->uart_iface)
    {
        dev->uart_iface->send_data(dev->uart_obj, &data, 1);
    }
}

int start_load_file(conf_object_t* obj)
{
    load_file_bridge *dev = obj->obj;

#ifdef __MINGW32__
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA data;
    char connect_msg[] = {'#', 0, '@', 1, '$'};

    if(WSAStartup(sockVersion, &data) != 0)
    {
        return Invarg_exp;
    }

    dev->sv_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(dev->sv_socket == INVALID_SOCKET)
    {
        printf("In %s ,create socket failed!\n", __func__);
        return Invarg_exp;
    }

    struct sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(dev->server_port);
    serAddr.sin_addr.S_un.S_addr = inet_addr(dev->server_ip);
    if (connect(dev->sv_socket, (SOCKADDR *)&serAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        printf("In %s, connect load_file ui failed !\n", __func__);
        closesocket(dev->sv_socket);
        return Invarg_exp;
    }
    //给遥测界面发送握手命令
    send(dev->sv_socket, connect_msg, 5, 0);

    while(1)
    {
        if (SIM_is_running())
        {
            unsigned char recData[1024] = {0};
            int i;
            int ret = recv(dev->sv_socket, recData, 50, 0);

            if (ret > 0)
            {
                for(i = 0; i < ret; i++)
                {
                    RW_WRLOCK(dev->lock);
                    WriteFIFO(dev->send_fifo, &recData[i], 1);
                    RW_UNLOCK(dev->lock);
                }
            }
        }
        Sleep(5);
    }
#endif
}

static attr_value_t get_attr_server_ip(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    load_file_bridge *dev = conf_obj->obj;
    attr_value_t ip = SKY_make_attr_string(dev->server_ip);
    return ip;
}

static exception_t set_attr_server_ip(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
    load_file_bridge *dev = conf_obj->obj;
    dev->server_ip = skyeye_strdup(SKY_attr_string(*value));

    return No_exp;
}

static attr_value_t get_attr_server_port(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    load_file_bridge *dev = conf_obj->obj;
    attr_value_t server_port = SKY_make_attr_uinteger(dev->server_port);
    return server_port;
}

static exception_t set_attr_server_port(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    load_file_bridge *dev = conf_obj->obj;
    dev->server_port = SKY_attr_uinteger(*value);
    return No_exp;
}

static load_file_register_attribute(conf_class_t* clss){
    SKY_register_attribute(clss, "server_ip", get_attr_server_ip, NULL, set_attr_server_ip, NULL, SKY_Attr_Optional, "string", "server ip");
    SKY_register_attribute(clss, "server_port", get_attr_server_port, NULL, set_attr_server_port, NULL, SKY_Attr_Optional, "uinteger", "server port");
}

static exception_t send_file_data_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    load_file_bridge *dev = obj->obj;
    dev->uart_obj = obj2;
    dev->uart_iface = (send_file_data_intf*)SKY_get_iface(obj2, SEND_FILE_DATA_INTF);
    if (dev->uart_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'send_file_data_intf' interface from %s fail.", obj2->objname);
        return Not_found_exp;
    }
    return No_exp;
}

static exception_t send_file_data_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    load_file_bridge *dev = obj->obj;
    *obj2 = dev->uart_obj;
    *port = NULL;
    return No_exp;
}

static conf_object_t* new_load_file(const char* obj_name)
{
    load_file_bridge* dev = skyeye_mm_zero(sizeof(load_file_bridge));
    dev->obj = new_conf_object(obj_name, dev);
    dev->recvBuf = skyeye_mm_zero(MAX_DATA_LENGTH);
    dev->send_fifo = CreateFIFO(2048);
    RWLOCK_INIT(dev->lock);

    return dev->obj;
}

static exception_t free_load_file(conf_object_t* obj)
{
    load_file_bridge *dev = obj->obj;
    skyeye_free(dev->obj);
    skyeye_free(dev->recvBuf);
    skyeye_free(dev->send_fifo);
#ifdef __MINGW32__
    closesocket(dev->sv_socket);
    WSACleanup();
#endif
    skyeye_free(dev);

    return No_exp;
}

static exception_t config_load_file(conf_object_t* obj)
{
#ifdef __MINGW32__
    CreateThread(NULL, 0, (void *)start_load_file, obj, 0, NULL);
    system_register_timer_handler(obj, 100, (time_sched_t)send_data_callback, SCHED_US|SCHED_NORMAL);
#endif
}

void init_load_file()
{
    static skyeye_class_t class_data =
    {
        .class_name = "load_file_bridge",
        .class_desc = "load_file_bridge",
        .new_instance = new_load_file,
        .free_instance = free_load_file,
        .config_instance = config_load_file
    };
    conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

    static const struct InterfaceDescription ifaces[] =
    {
    };

    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription )
        {
            .name = SEND_FILE_DATA_INTF,
            .set = send_file_data_set,
            .get = send_file_data_get,
        },
    };

    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    load_file_register_attribute(clss);
}

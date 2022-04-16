/* Copyright (C) 
 * 2013 - Michael.Kang blackfin.kang@gmail.com
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
 * @file sparc_ctrl.c
 * @brief The interface for zlg USB CAN I device
 * @author Michael.Kang blackfin.kang@gmail.com
 * @version 7849
 * @date 2013-08-06
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

#include "sparc_ctrl_bridge.h"
#include <skyeye_system.h>

static exception_t send_power_flag(conf_object_t *opaque, uint32_t flag)
{
    sparc_ctrl_bridge *dev = opaque->obj;
    char buf[10] = {0};
    int size = 5;

    buf[0] = '#';
    buf[1] = 1;
    buf[2] = '@';
    buf[3] = flag;
    buf[4] = '$';

    send(dev->sv_socket, buf, size, 0);

    return No_exp;
}

int unpack_power_msg(char *buf, int flag)
{
    int size = 7;
    int i;

    buf[0] = '#';
    buf[1] = 1;
    buf[2] = '@';
    buf[size - 1] = '$';

    return size;
}

void clr_power_flag(conf_object_t* obj, int flag)
{
    sparc_ctrl_bridge *dev = obj->obj;
    unsigned char buf[1024] = {0};
    int size;
    size = unpack_power_msg(buf, flag);

    send(dev->sv_socket, buf, size, 0);
}

void unpack_ctrl_data(conf_object_t* obj, unsigned char *buf, int size)
{
    sparc_ctrl_bridge *dev = obj->obj;
    int cmd_type, cmd_ctrl;
    int i;

    if (buf[0] == '#' && buf[1] == 1 && buf[2] == '@' && buf[size - 1] == '$')
    {
        cmd_type = buf[3];
        cmd_ctrl = buf[4];

        sparc_ctrl_dev_list_t *list =  &(dev->device_list);
        conf_object_t *device;
        for(i = 0; i < list->device_num; i++)
        {
            device = list->device[i].obj;
            if (list->device[i].iface)
            {
                (list->device[i].iface)->send_ctrl_cmd(device, cmd_type, cmd_ctrl);
            }
        }
    }
}

int start_sparc_ctrl(conf_object_t* obj)
{
    sparc_ctrl_bridge *dev = obj->obj;

#ifdef __MINGW32__
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA data;
    char connect_msg[] = {'#', 0, '@', 2, '$'};

    if(WSAStartup(sockVersion, &data) != 0)
    {
        return Invarg_exp;
    }

    dev->sv_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(dev->sv_socket == INVALID_SOCKET)
    {
        printf("In %s ,create sparc_ctrl socket failed!\n", __func__);
        return Invarg_exp;
    }

    struct sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(dev->server_port);
    serAddr.sin_addr.S_un.S_addr = inet_addr(dev->server_ip);
    if (connect(dev->sv_socket, (SOCKADDR *)&serAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        printf("In %s, connect sparc_ctrl ui failed !\n", __func__);
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
                unpack_ctrl_data(obj, recData, ret);
            }
        }
        Sleep(5);
    }
#endif
}

static attr_value_t get_attr_server_ip(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    sparc_ctrl_bridge *dev = conf_obj->obj;
    attr_value_t ip = SKY_make_attr_string(dev->server_ip);
    return ip;
}

static exception_t set_attr_server_ip(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
    sparc_ctrl_bridge *dev = conf_obj->obj;
    dev->server_ip = skyeye_strdup(SKY_attr_string(*value));

    return No_exp;
}

static attr_value_t get_attr_server_port(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    sparc_ctrl_bridge *dev = conf_obj->obj;
    attr_value_t server_port = SKY_make_attr_uinteger(dev->server_port);
    return server_port;
}

static exception_t set_attr_server_port(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    sparc_ctrl_bridge *dev = conf_obj->obj;
    dev->server_port = SKY_attr_uinteger(*value);
    return No_exp;
}

static sparc_ctrl_register_attribute(conf_class_t* clss)
{
    SKY_register_attribute(clss, "server_ip", get_attr_server_ip, NULL, set_attr_server_ip, NULL, SKY_Attr_Optional, "string", "server ip");
    SKY_register_attribute(clss, "server_port", get_attr_server_port, NULL, set_attr_server_port, NULL, SKY_Attr_Optional, "uinteger", "server port");
}

static exception_t sparc_ctrl_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    sparc_ctrl_bridge *dev = obj->obj;
    sparc_ctrl_dev_list_t *list =  &(dev->device_list);
    int i;
    for(i = 0; i < list->device_num; i++)
    {
        if (list->device[i].obj == obj2)
        {
            return Unknown_exp;
        }
    }
    list->device[list->device_num].obj = obj2;
    list->device[list->device_num].iface = (sparc_ctrl_intf *)SKY_get_iface(obj2, SPARC_CTRL_INTF);
    if (list->device[list->device_num].iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Set sparc_ctrl_intf iface error\n");
    }
    list->device_num++;
    return No_exp;
}

static exception_t sparc_ctrl_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    sparc_ctrl_bridge *dev = obj->obj;
    *obj2 = NULL;
    *port = NULL;
    return No_exp;
}

static conf_object_t* new_sparc_ctrl(const char* obj_name)
{
    sparc_ctrl_bridge* dev = skyeye_mm_zero(sizeof(sparc_ctrl_bridge));
    dev->obj = new_conf_object(obj_name, dev);

    return dev->obj;
}

static exception_t free_sparc_ctrl(conf_object_t* obj)
{
    sparc_ctrl_bridge *dev = obj->obj;
    skyeye_free(dev->obj);
#ifdef __MINGW32__
    closesocket(dev->sv_socket);
    WSACleanup();
#endif
    skyeye_free(dev);

    return No_exp;
}

static exception_t config_sparc_ctrl(conf_object_t* obj)
{
#ifdef __MINGW32__
    CreateThread(NULL, 0, start_sparc_ctrl, obj, 0, NULL);
#endif

    return No_exp;
}

void init_sparc_ctrl()
{
    static skyeye_class_t class_data =
    {
        .class_name = "sparc_ctrl_bridge",
        .class_desc = "sparc_ctrl_bridge",
        .new_instance = new_sparc_ctrl,
        .free_instance = free_sparc_ctrl,
        .config_instance = config_sparc_ctrl,
        .get_attr = NULL,
        .set_attr = NULL
    };
    conf_class_t* clss = SKY_register_linker_class(class_data.class_name, &class_data);

    static const sparc_ctrl_intf sparc_ctrl_iface =
    {
        .send_power_flag = send_power_flag,
    };
    SKY_register_iface(clss, SPARC_CTRL_INTF, &sparc_ctrl_iface);
    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription) {
            .name = SPARC_CTRL_INTF, 
            .iface = &sparc_ctrl_iface,
        },
    };
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription ){
            .name = SPARC_CTRL_INTF,
            .set = sparc_ctrl_set,
            .get = sparc_ctrl_get,
        },
    };

    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    sparc_ctrl_register_attribute(clss);
}

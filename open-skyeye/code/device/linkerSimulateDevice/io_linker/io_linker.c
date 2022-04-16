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
 * @file io_linker.c
 * @brief 
 * @author zyuming zyumingfit@gmail.com
 * @date 2013-11-27
 */
#include <stdio.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_types.h>
#include <skyeye_mm.h>
#include <skyeye_class.h>
#include <skyeye_fifo.h>
#include <skyeye_interface.h>
#include <memory_space.h>
#include <skyeye_obj.h>
#include <skyeye_emif.h>
#include <skyeye_attribute.h>
#include <skyeye_core.h>
#include <skyeye_device.h>
#include "io_linker.h"
#include <skyeye_dev_bus_intf.h>

static int io_linker_recv_data(conf_object_t *opaque, conf_object_t* obj2, void* recv_buf, int length, int io_id)
{
    io_linker_t *dev = opaque->obj;
    int i;
    for(i = 0; i < dev->dev_cnt; i++)
    {
        if (obj2 != dev->device_list[i])
        {
            if (dev->device_iface_list[i] != NULL)
            {
                memcpy(recv_buf, dev->send_buf[i], length);
            }
        }
    }
    return 0;
}

static void io_linker_send_data(conf_object_t *opaque, conf_object_t* obj2, void *buf, int length, int io_id)
{
    io_linker_t *dev = (io_linker_t *)opaque->obj;
    int i;

    for(i = 0; i < dev->dev_cnt; i++)
    {
        if (obj2 != dev->device_list[i])
        {
            if (dev->device_iface_list[i] != NULL)
            {
                memcpy(dev->send_buf[i], buf, length);
                dev->device_iface_list[i]->receive_data(dev->device_list[i], buf, length, io_id);
            }
        }
    }
    return;
}

static conf_object_t* new_io_linker(const char* obj_name)
{
    int i;
    io_linker_t *io_linker = skyeye_mm_zero(sizeof(io_linker_t));
    io_linker->obj = new_conf_object(obj_name, io_linker);
    io_linker->recv_buf = (char *)skyeye_mm_zero(32*sizeof(int));
    io_linker->dev_cnt = 0;
    for (i = 0; i < DEV_MAX_COUNT; i++)
    {
        io_linker->send_buf[i] = (char *)skyeye_mm_zero(32*sizeof(int));
    }

    return io_linker->obj;
}

static exception_t reset_io_linker(conf_object_t* opaque, const char* args)
{
    io_linker_t *io_linker = opaque->obj;

    return No_exp; 
}

static exception_t free_io_linker(conf_object_t* opaque)
{
    io_linker_t *dev = opaque->obj;
    int i;
    for (i = 0; i < DEV_MAX_COUNT;i++)
        skyeye_free(dev->send_buf[i]);
    skyeye_free(dev->recv_buf);
    skyeye_free(dev->obj);
    skyeye_free(dev);
    return No_exp;
}

static exception_t io_dev_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    io_linker_t* dev = (io_linker_t*)(obj->obj);
    dev->device_list[dev->dev_cnt] = obj2;

    dev->device_iface_list[dev->dev_cnt] = (io_dev_intf*)SKY_get_iface(obj2, IO_DEV_INTF);
    if (dev->device_iface_list[dev->dev_cnt] == NULL)
    {
        if (dev->device_list[dev->dev_cnt] == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", IO_DEV_INTF);
        } else
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", IO_DEV_INTF, obj2->objname);

        return Not_found_exp;
    }
    dev->dev_cnt ++;

    return No_exp;
}

static exception_t io_dev_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    io_linker_t* dev = (io_linker_t*)(obj->obj);

    dev->dev_cnt --;
    *obj2 = dev->device_list[dev->dev_cnt];
    return No_exp;
}

void init_io_linker()
{
    static skyeye_class_t class_data =
    {
        .class_name = "io_linker",
        .class_desc = "io_linker",
        .new_instance = new_io_linker,
        .reset_instance = reset_io_linker,
        .free_instance = free_io_linker,
        .get_attr = NULL,
        .set_attr = NULL,
    };
    conf_class_t* class = SKY_register_linker_class(class_data.class_name, &class_data);
    static const io_linker_intf io_linker_iface =
    {
        .recv_data = io_linker_recv_data,
        .send_data = io_linker_send_data,
    };
    SKY_register_iface(class, IO_LINKER_INTF, &io_linker_iface);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = IO_LINKER_INTF,
            .iface = &io_linker_iface,
        },
    };
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription )
        {
            .name = IO_DEV_INTF,
            .set = io_dev_set,
            .get = io_dev_get,
        },
    };

    class_register_ifaces(class, ifaces);
    class_register_connects(class, connects);
}

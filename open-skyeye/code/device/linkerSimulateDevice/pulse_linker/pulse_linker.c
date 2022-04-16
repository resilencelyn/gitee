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
 * @file pulse_linker.c
 * @brief 
 * @author jiachao jiachao@digiproto.com
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
#include "pulse_linker.h"
#include <skyeye_dev_bus_intf.h>

static void pulse_linker_send_pulse(conf_object_t *opaque, conf_object_t* obj2, pulse_msg_t *msg, int io_num, int io_id)
{
    pulse_linker_t *dev = (pulse_linker_t *)opaque->obj;
    int i;

    for(i = 0; i < dev->dev_cnt; i++)
    {
        if (obj2 != dev->device_list[i])
        {
            if (dev->device_iface_list[i] != NULL)
            {
                dev->device_iface_list[i]->receive_pulse(dev->device_list[i], msg, io_num, io_id);
            }
        }
    }
    return;
}

static conf_object_t* new_pulse_linker(char* obj_name)
{
    int i;
    pulse_linker_t *pulse_linker = skyeye_mm_zero(sizeof(pulse_linker_t));
    pulse_linker->obj = new_conf_object(obj_name, pulse_linker);
    pulse_linker->recv_buf = (char *)skyeye_mm_zero(32*sizeof(int));
    pulse_linker->dev_cnt = 0;

    for (i = 0; i < DEV_MAX_COUNT;i++)
    {
        pulse_linker->send_buf[i] = (char *)skyeye_mm_zero(32*sizeof(int));
    }

    return pulse_linker->obj;
}

static exception_t reset_pulse_linker(conf_object_t* opaque, const char* args)
{
    pulse_linker_t *pulse_linker = opaque->obj;

    return No_exp;
}

static exception_t free_pulse_linker(conf_object_t* opaque)
{
    pulse_linker_t *dev = opaque->obj;
    int i;
    for (i = 0; i < DEV_MAX_COUNT;i++)
    {
        skyeye_free(dev->send_buf[i]);
    }

    skyeye_free(dev->recv_buf);
    skyeye_free(dev->obj);
    skyeye_free(dev);
    return No_exp;
}

static exception_t pulse_dev_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    pulse_linker_t* dev = (pulse_linker_t*)(obj->obj);
    dev->device_list[dev->dev_cnt] = obj2;

    dev->device_iface_list[dev->dev_cnt] = (pulse_dev_intf*)SKY_get_iface(obj2, PULSE_DEV_INTF);

    if (dev->device_iface_list[dev->dev_cnt] == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'pulse_dev_iface' interface from %s fail.", obj2->objname);
        return Not_found_exp;
    }
    dev->dev_cnt ++;
    return No_exp;

}

static exception_t pulse_dev_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    pulse_linker_t* dev = (pulse_linker_t*)(obj->obj);
    return No_exp;
}

void init_pulse_linker()
{
    static skyeye_class_t class_data =
    {
        .class_name = "pulse_linker",
        .class_desc = "pulse_linker",
        .new_instance = new_pulse_linker,
        .reset_instance = reset_pulse_linker,
        .free_instance = free_pulse_linker,
        .get_attr = NULL,
        .set_attr = NULL,
    };
    conf_class_t* class = SKY_register_linker_class(class_data.class_name, &class_data);

    static const pulse_linker_intf pulse_linker_iface =
    {
        .send_pulse = pulse_linker_send_pulse,
    };
    SKY_register_iface(class, PULSE_LINKER_INTF, &pulse_linker_iface);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = PULSE_LINKER_INTF,
            .iface = &pulse_linker_iface,
        },
    };

    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription ){
            .name = PULSE_DEV_INTF,
            .set = pulse_dev_set,
            .get = pulse_dev_get,
        },
    };

    class_register_ifaces(class, ifaces);
    class_register_connects(class, connects);
}

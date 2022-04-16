/* Copyright (C)
 * zyumingfit@gmail.com
 * This program is free software; you power redistribute it and/or
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
 * @file power_link.c
 * @brief The implementation of device
 * @author zyumingfit@gmail.com
 * @version 01.00
 */

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_iface.h>
#include <pthread.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>

#include "power_link.h"

static exception_t init_power_device_list(power_link_device_t *dev)
{
    dev->device_list.device_num = 0;
    return No_exp;
}

static conf_object_t* power_link_new(const char* obj_name)
{
    power_link_device_t* dev = skyeye_mm_zero(sizeof(power_link_device_t));
    dev->obj = new_conf_object(obj_name, dev);
    init_power_device_list(dev);

    return dev->obj;
}

static exception_t power_link_free(conf_object_t* dev)
{
    return No_exp;
}

static exception_t power_link_reset(conf_object_t* obj, const char *parameter)
{
    return No_exp;
}

static exception_t power_link_set_attr(conf_object_t* obj,
        const char* attr_name, attr_value_t* value)
{
    return No_exp;
}

static attr_value_t* power_link_get_attr(const char* attr_name,
        conf_object_t* obj)
{
    return NULL;
}

exception_t power_linker(conf_object_t *power_linker, conf_object_t *power_device, uint32_t power_flag)
{
    power_link_device_t *power_link = power_linker->obj;
    power_device_list_t *list =  &(power_link->device_list);
    conf_object_t *device;

    int i;
    for(i = 0; i < list->device_num; i++)
    {
        device = list->device[i].device_obj;
        if (device != power_device)
        {
            (list->device[i].iface)->power_mach(device, power_flag);
        }
    }
    return No_exp;
}

exception_t duty_linker(conf_object_t *duty_linker, conf_object_t *power_device, uint32_t duty){
    power_link_device_t *power_link = duty_linker->obj;
    power_device_list_t *list =  &(power_link->device_list);
    conf_object_t *device;

    int i;
    for(i = 0; i < list->device_num; i++)
    {
        device = list->device[i].device_obj;
        if (device != power_device)
        {
            (list->device[i].iface)->set_duty(device, duty);
        }
    }
    return No_exp;
}

int get_signal_linker(conf_object_t *duty_linker, conf_object_t *power_device)
{
    power_link_device_t *power_link = duty_linker->obj;
    power_device_list_t *list =  &(power_link->device_list);
    conf_object_t *device;
    int signal_flag = 0;

    int i;
    for(i = 0; i < list->device_num; i++)
    {
        device = list->device[i].device_obj;
        if (device != power_device)
        {
            signal_flag = (list->device[i].iface)->get_signal(device);
        }
    }
    return signal_flag;
}

static exception_t power_dev_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    power_link_device_t *power_link = obj->obj;
    power_device_list_t *list =  &(power_link->device_list);
    int i;
    for(i = 0; i < list->device_num; i++)
    {
        if (list->device[i].device_obj == obj2)
        {
            return Unknown_exp;
        }
    }
    list->device[list->device_num].device_obj = obj2;
    list->device[list->device_num].iface =  (power_dev_intf *)SKY_get_iface(obj2, POWER_DEV_INTF);
    list->device_num++;
    return No_exp;
}

static exception_t power_dev_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    power_link_device_t *power_link = obj->obj;
    *obj2 = NULL;
    *port = NULL;
    return No_exp;
}


void init_power_link()
{
    static skyeye_class_t class_data =
    {
        .class_name = "power_linker",
        .class_desc = "power_linker",
        .new_instance = power_link_new,
        .free_instance = power_link_free,
        .reset_instance = power_link_reset,
        .get_attr = power_link_get_attr,
        .set_attr = power_link_set_attr
    };
    conf_class_t *clss = SKY_register_linker_class(class_data.class_name, &class_data);

    static const power_linker_intf linker_iface =
    {
        .power_linker = power_linker,
        .duty_linker = duty_linker,
        .get_signal_linker = get_signal_linker,
    };
    SKY_register_iface(clss, POWER_LINKER_INTF, &linker_iface);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = POWER_LINKER_INTF,
            .iface = &linker_iface,
        },
    };
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription ){
            .name = POWER_DEV_INTF,
            .set = power_dev_set,
            .get = power_dev_get,
        },

    };

    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);
}

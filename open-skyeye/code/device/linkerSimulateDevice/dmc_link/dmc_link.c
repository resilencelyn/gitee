/* Copyright (C)
 * zyumingfit@gmail.com
 * This program is free software; you dmc redistribute it and/or
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
 * @file dmc_link.c
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

#include "dmc_link.h"

static exception_t init_dmc_device_list(dmc_link_device_t *dev)
{
    dev->device_list.device_num = 0;
    return No_exp;
}

static conf_object_t* dmc_link_new(const char* obj_name)
{
    dmc_link_device_t* dev = skyeye_mm_zero(sizeof(dmc_link_device_t));
    dev->obj = new_conf_object(obj_name, dev);
    init_dmc_device_list(dev);

    return dev->obj;
}

static exception_t dmc_link_free(conf_object_t* dev)
{
    skyeye_free(dev->obj);
    skyeye_free(dev);
    return No_exp;
}

static exception_t dmc_link_reset(conf_object_t* obj, const char *parameter)
{
    return No_exp;
}

int dmc_linker_send_data(conf_object_t *dmc_linker, conf_object_t *dmc_device, void *buf)
{
    dmc_link_device_t *dmc_link = dmc_linker->obj;
    dmc_device_list_t *list = &(dmc_link->device_list);
    conf_object_t *device;

    int i;
    for(i = 0; i < list->device_num; i++)
    {
        device = list->device[i].device_obj;
        if (device != dmc_device)
        {
            (list->device[i].iface)->receive_data(device, buf);
        }
    }
    return 0;
}

int dmc_linker_send_backup_data(conf_object_t *dmc_linker, conf_object_t *dmc_device, uint16_t* buf, int length)
{
    dmc_link_device_t *dmc_link = dmc_linker->obj;
    dmc_device_list_t *list =  &(dmc_link->device_list);
    conf_object_t *device;

    int i;
    for(i = 0; i < list->device_num; i++)
    {
        device = list->device[i].device_obj;
        if (device != dmc_device)
        {
            (list->device[i].iface)->send_backup_data(device, buf, length);
        }
    }
    return 0;
}

int dmc_linker_raise_signal(conf_object_t *dmc_linker, conf_object_t *dmc_device, int irq_number)
{
    dmc_link_device_t *dmc_link = dmc_linker->obj;
    dmc_device_list_t *list =  &(dmc_link->device_list);
    conf_object_t *device;

    int i;
    for(i = 0; i < list->device_num; i++)
    {
        device = list->device[i].device_obj;
        if (device != dmc_device)
        {
            (list->device[i].iface)->raise_signal(device, irq_number);
        }
    }
    return 0;
}

static exception_t dmc_dev_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    dmc_link_device_t *dmc_link = obj->obj;
    dmc_device_list_t *list =  &(dmc_link->device_list);
    int i;
    for(i = 0; i < list->device_num; i++)
    {
        if (list->device[i].device_obj == obj2)
        {
            return Unknown_exp;
        }
    }
    list->device[list->device_num].device_obj = obj2;
    list->device[list->device_num].iface =  (dmc_dev_intf *)SKY_get_iface(obj2, DMC_DEV_INTF);
    list->device_num++;
    return No_exp;
}

static exception_t dmc_dev_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    dmc_link_device_t *dmc_link = obj->obj;
    *obj2 = NULL;
    *port = NULL;
    return No_exp;
}

void init_dmc_link()
{
    static skyeye_class_t class_data =
    {
        .class_name = "dmc_linker",
        .class_desc = "dmc_linker",
        .new_instance = dmc_link_new,
        .free_instance = dmc_link_free,
        .reset_instance = dmc_link_reset
    };
    conf_class_t *clss = SKY_register_linker_class(class_data.class_name, &class_data);

    static const dmc_linker_intf linker_iface =
    {
        .send_data = dmc_linker_send_data,
        .raise_signal = dmc_linker_raise_signal,
        .send_linker_backup_data = dmc_linker_send_backup_data,
    };
    SKY_register_iface(clss, DMC_LINKER_INTF, &linker_iface);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = DMC_LINKER_INTF,
            .iface = &linker_iface,
        },
    };
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription )
        {
            .name = DMC_DEV_INTF,
            .set = dmc_dev_set,
            .get = dmc_dev_get,
        },

    };

    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);
}

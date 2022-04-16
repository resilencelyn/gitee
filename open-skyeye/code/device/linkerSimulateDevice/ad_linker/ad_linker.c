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
 * @file ad_linker.c
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
#include "ad_linker.h"
#include <skyeye_dev_bus_intf.h>

static void ad_linker_send_data(conf_object_t *opaque, conf_object_t* obj2, void *buf, int length)
{
    ad_linker_t *dev = (ad_linker_t *)opaque->obj;
    int i;

    for(i = 0; i < dev->dev_cnt;i++)
    {
        if (obj2 != dev->device_list[i])
        {
            if (dev->device_iface_list[i] != NULL)
            {
                dev->device_iface_list[i]->receive_data(dev->device_list[i], buf, length);
            }
        }
    }
    return;
}

static conf_object_t* new_ad_linker(char* obj_name)
{
    ad_linker_t *ad_linker = skyeye_mm_zero(sizeof(ad_linker_t));
    ad_linker->obj = new_conf_object(obj_name, ad_linker);
    ad_linker->dev_cnt = 0;
    ad_linker->get_dev_cnt = 0;

    return ad_linker->obj;
}
static exception_t reset_ad_linker(conf_object_t* opaque, const char* args)
{
    ad_linker_t *ad_linker = opaque->obj;

    return No_exp;
}

static exception_t free_ad_linker(conf_object_t* opaque)
{
    ad_linker_t *dev = opaque->obj;
    skyeye_free(dev->obj);
    skyeye_free(dev);
    return No_exp;
}
static exception_t ad_dev_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    ad_linker_t* dev = (ad_linker_t*)(obj->obj);
    dev->device_list[dev->dev_cnt] = obj2;

    dev->device_iface_list[dev->dev_cnt] = (ad_dev_intf*)SKY_get_iface(obj2, AD_DEV_INTF);
    if (dev->device_iface_list[dev->dev_cnt] == NULL)
    {
        if (obj2 == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Get 'ad_dev_iface' interface from NULL object fail.", obj2);
        } else
        {
            skyeye_log(Error_log, __FUNCTION__, "Get 'ad_dev_iface' interface from %s fail.", obj2->objname);
        }
        return Not_found_exp;
    }

    dev->dev_cnt ++;
    return No_exp;
}

static exception_t ad_dev_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    ad_linker_t* dev = (ad_linker_t*)(obj->obj);

    *obj2 = dev->device_list[dev->get_dev_cnt];
    dev->get_dev_cnt++;
    return No_exp;
}

void init_ad_linker()
{
    static skyeye_class_t class_data =
    {
        .class_name = "ad_linker",
        .class_desc = "ad_linker",
        .new_instance = new_ad_linker,
        .reset_instance = reset_ad_linker,
        .free_instance = free_ad_linker,
        .get_attr = NULL,
        .set_attr = NULL,
    };
    conf_class_t* class = SKY_register_linker_class(class_data.class_name, &class_data);
    static const ad_linker_intf ad_linker_iface =
    {
        .send_data = ad_linker_send_data,
    };
    SKY_register_iface(class, AD_LINKER_INTF, &ad_linker_iface);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = AD_LINKER_INTF,
            .iface = &ad_linker_iface,
        },
    };
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription )
        {
            .name = AD_DEV_INTF,
            .set = ad_dev_set,
            .get = ad_dev_get,
        },
    };

    class_register_ifaces(class, ifaces);
    class_register_connects(class, connects);
}

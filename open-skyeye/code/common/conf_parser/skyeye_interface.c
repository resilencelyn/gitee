/* Copyright (C) 
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

/**
* @file skyeye_interface.c
* @brief The implementation of interface
* @author Michael.Kang blackfin.kang@gmail.com
* @version 1.3.4
* @date 2011-08-15
*/

#include <string.h>
#include <stdio.h>
#include "skyeye_types.h"
#include "skyeye_interface.h"
#include "skyeye_obj.h"
//#define DEBUG
#include "skyeye_log.h"
#include "skyeye_class.h"

exception_t SKY_register_interface(void *intf_obj, const char *objname, const char *iface_name)
{
    char iface_objname[MAX_OBJNAME];

    if (strlen(objname) + strlen(iface_name) + 1 > MAX_OBJNAME)
    {
        return Invarg_exp;
    }
    get_strcat_objname(iface_objname, objname, iface_name);
    DBG("In %s, interface name=%s\n", __FUNCTION__, iface_objname);
    if (new_map_object(iface_objname, intf_obj, MapObject) != NULL)
    {
        return No_exp;
    } else
    {
        return Invarg_exp;
    }
}

void *SKY_get_interface(conf_object_t * obj, const char *iface_name)
{
    char iface_objname[MAX_OBJNAME];

    if (strlen(obj->objname) + strlen(iface_name) + 1 > MAX_OBJNAME)
    {
        return NULL;
    }
    get_strcat_objname(iface_objname, obj->objname, iface_name);
    DBG("In %s, obj->objname=%s, interface name=%s\n", __FUNCTION__, obj->objname, iface_objname);
    conf_object_t *intf_obj = get_conf_obj(iface_objname);

    return intf_obj->obj;
}

/*
 * from_intf:
 * to_intf:
 * iface_name:
 */
exception_t SKY_set_interface(void *from_obj, void *to_obj, const char *iface_name)
{
    skyeye_intf_t *from_intf = SKY_get_interface(from_obj, iface_name);
    skyeye_intf_t *to_intf = SKY_get_interface(to_obj, iface_name);

    if (from_intf->registered)
        to_intf->obj = from_intf->obj;
    else
    {
        printf("register interface %s fault!\n", iface_name);
        return Invarg_exp;
    }

    return No_exp;
}

void const *skyeye_find_iface(const struct InterfaceDescription *ids, const char *iface)
{
    const struct InterfaceDescription *id;

    for (id = ids; id->iface; id++)
    {
        if (!strcmp(id->iface, iface))
        {
            return id->iface;
        }
    }
    return NULL;
}

#if 0
void const *skyeye_get_interface(conf_object_t * obj, const char *iface_name)
{
    char iface_objname[MAX_OBJNAME];

    if (strlen(obj->class_name) + strlen(iface_name) + 1 > MAX_OBJNAME)
    {
        return NULL;
    }
    get_strcat_objname(iface_objname, obj->class_name, iface_name);
    DBG("In %s, obj->objname=%s, interface name=%s\n", __FUNCTION__, obj->objname, iface_objname);
    conf_object_t *intf_obj = get_conf_obj(iface_objname);

    return intf_obj->obj;
}
#endif
void const *skyeye_get_port_interface(conf_object_t * obj, const char *iface, const char *port)
{
    printf("not implemented\n");
    return NULL;
}

exception_t _skyeye_connect(conf_object_t * obj, conf_object_t * obj2, const char *con_name, const char *port,
                            int index)
{
    skyeye_class_t *class_data = obj->class_data;
    int count, i;
    const struct ConnectDescription *connects;

    count = class_data->connect_list.cnt;
    connects = class_data->connect_list.connects;

    for (i = 0; i < count; i++)
    {
        if (!strcmp(connects[i].name, con_name))
        {
            return connects[i].set(obj, obj2, port, index);
        }
    }
    skyeye_log(Error_log, __FUNCTION__, "there is no %s connect in %s.\n", con_name, obj->objname);
    return Not_found_exp;
}

exception_t skyeye_connect(conf_object_t * obj, conf_object_t * obj2, const char *name, uint32_t index)
{
    return _skyeye_connect(obj, obj2, name, NULL, index);
}

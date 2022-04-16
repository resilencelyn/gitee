/* * skyeye_iface.cpp:
 *
 * Copyright (C) 2014 Oubang Shen <shenoubang@gmail.com>
 * Skyeye Develop Group, for help please send mail to
 * <skyeye-developer@lists.gro.clinux.org>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <iostream>
#include <list>
#include <algorithm>

#include <stdio.h>
#include <string.h>

#include <skyeye_iface.h>
#include <skyeye_types.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_attribute.h>
#include <skyeye_attr.h>
#include <skyeye_mm.h>
#include <skyeye_log.h>

using namespace std;

char *iface_cache[256];
typedef struct iface
{
    const char *name;
    const char *port_name;
    interface_t *iface;
} iface_t;

typedef list < iface_t > INTERFACE_LIST;

typedef struct interface_list
{
    INTERFACE_LIST lst;
} interface_list_t;

void init_class_interface_list(conf_class_t * clss)
{
    interface_list_t *iface_list = new interface_list_t;

    clss->iface = iface_list;
    return;
}

bool operator ==(const iface_t & first, const iface_t & other)
{
    return (strcmp(first.name, other.name) ? 0 : 1);
}

INTERFACE_LIST::iterator find_class_interface(conf_class_t * clss, const char *name)
{
    iface_t tmp;

    tmp.name = name;
    INTERFACE_LIST::iterator it = std::find(clss->iface->lst.begin(), clss->iface->lst.end(), tmp);
    return it;
}

int SKY_unregister_iface(conf_class_t * clss, const char *name)
{
    INTERFACE_LIST::iterator it = clss->iface->lst.begin();
    while (it != clss->iface->lst.end())
    {
        if (!(strcmp(it->name, name)))
        {
            it = clss->iface->lst.erase(it);
        } else
        {
            it++;
        }
    }

    return 0;
}

int SKY_register_iface(conf_class_t * clss, const char *name, const void *iface)
{
    iface_t tmp;

    tmp.name = name;
    tmp.iface = (interface_t *) iface;
    clss->iface->lst.push_back(tmp);
    return 0;
}

void SKY_print_class_iface(conf_class_t * clss)
{
    INTERFACE_LIST::iterator it = clss->iface->lst.begin();
    while (it != clss->iface->lst.end())
    {
        cout << "class : " << clss->class_data.class_name << "\tinterface: " << it->name << endl;
        it++;
    }

    return;
}

int SKY_register_port_iface(conf_class_t * clss, const char *name, const void *iface, const char *port_name,
                            const char *desc)
{
    /* TODO: implement the port interface */
    return 0;
}

interface_t *SKY_get_class_iface(conf_class_t * clss, const char *name)
{
    interface_t *tmp = NULL;

    if (clss == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "get class iface failed! clss is NULL!");
        return tmp;
    }
    INTERFACE_LIST::iterator it = find_class_interface(clss, name);
    if (it != clss->iface->lst.end())
    {
        tmp = it->iface;
    } else
    {
        tmp = NULL;
    }
    return tmp;
}

static conf_class_t *get_obj_class(conf_object_t * obj)
{
    conf_class_t *clss = (conf_class_t *) (obj->clss);

    return clss;
}

interface_t *SKY_get_iface(conf_object_t * obj, const char *name)
{
    interface_t *tmp = NULL;

    if (obj != NULL)
    {
        conf_class_t *clss = get_obj_class(obj);

        tmp = SKY_get_class_iface(clss, name);
    }

    return tmp;
}

interface_t *SKY_get_port_iface(conf_object_t * obj, const char *name, const char *portname)
{
    /* TODO: implement the content about port interface */
    return NULL;
}

interface_t *SKY_get_class_port_iface(conf_class_t * clss, const char *name, const char *portname)
{
    /* TODO: implement the content about port interface */
    return NULL;
}

exception_t SKY_connect(conf_object_t * dest, conf_object_t * sr, const char *name)
{
    attr_value_t value = SKY_make_attr_object(sr);

    return SKY_set_attribute(dest, name, &value);
}

char **SKY_get_ifacename_list(conf_class_t * clss)
{
    INTERFACE_LIST::iterator it;;
    unsigned i = 0;

    if (clss->iface == NULL)
        goto Ret;
    for (it = clss->iface->lst.begin(); it != clss->iface->lst.end(); it++)
    {
        iface_cache[i] = (char *) ((*it).name);
        i++;
    }
  Ret:
    iface_cache[i] = NULL;
    return iface_cache;
}

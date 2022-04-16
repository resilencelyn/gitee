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
* @file skyeye_class.c
* @brief the class register.
* @author Michael.Kang blackfin.kang@gmail.com
* @version 0.1
* @date 2011-08-12
*/
#include "skyeye_obj.h"
#include "skyeye_device.h"
#include "skyeye_class.h"
#include "skyeye_log.h"
#include "skyeye_mm.h"
#include "skyeye_attribute.h"
#include "skyeye_iface.h"
#include "skyeye_types.h"
#include "skyeye_list.h"

#include <stdio.h>

SKY_list *class_list = NULL;
SKY_list *obj_list = NULL;
char *class_cache[1024];
static void init_class_list(void);
static void init_obj_list(void);
static exception_t put_class_node(void *cls);
static exception_t put_obj_node(void *obj);

static conf_class_t *init_class(conf_class_t * cc)
{
    init_log_groups(cc);
    init_class_attribute_list(cc);
    init_class_interface_list(cc);
    /* for more */
    return cc;
}

conf_class_t *SKY_register_class(const char *name, class_data_t * skyeye_class)
{
    conf_class_t *cc = skyeye_mm_zero(sizeof (conf_class_t));

    cc->class_data = *skyeye_class;
    new_map_object(name, cc, ClassObject);
    if (class_list == NULL)
        init_class_list();
    put_class_node(cc);
    return init_class(cc);
}

conf_class_t *SKY_register_cpu_class(const char *name, class_data_t * skyeye_class)
{
    skyeye_class->class_type = "cpu";
    return SKY_register_class(name, skyeye_class);
}

conf_class_t *SKY_register_device_class(const char *name, class_data_t * skyeye_class)
{
    skyeye_class->class_type = "device";
    return SKY_register_class(name, skyeye_class);
}

conf_class_t *SKY_register_soc_class(const char *name, class_data_t * skyeye_class)
{
    skyeye_class->class_type = "mach";
    return SKY_register_class(name, skyeye_class);
}

conf_class_t *SKY_register_linker_class(const char *name, class_data_t * skyeye_class)
{
    skyeye_class->class_type = "linker";
    return SKY_register_class(name, skyeye_class);
}

conf_class_t *SKY_get_object_class(conf_object_t * obj)
{
    assert(obj != NULL);
    conf_class_t *clss = (conf_class_t *) (obj->clss);

    return clss;
}

conf_object_t *SKY_alloc_conf_object(const char *objname, const char *class_name)
{
    conf_object_t *obj = get_conf_obj(class_name);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not find the object %s\n", class_name);
        return NULL;

    }
    skyeye_class_t *class_data = obj->obj;

    if (class_data == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not find the class %s\n", class_name);
        return NULL;
    }
    conf_object_t *instance = class_data->new_instance(objname);

    if (instance)
    {
        instance->class_data = class_data;
    }

    instance->class_name = skyeye_strdup(class_name);
    return instance;
}

conf_object_t *pre_conf_obj(const char *objname, const char *class_name)
{
    conf_object_t *instance = SKY_alloc_conf_object(objname, class_name);

    if (obj_list == NULL)
    {
        init_obj_list();
    }
    if (instance)
    {
        put_obj_node(instance);
    }
    return instance;
}

exception_t config_conf_obj(conf_object_t * obj)
{
    conf_object_t *class_obj = get_conf_obj(obj->class_name);
    skyeye_class_t *class_data = class_obj->obj;

    if (class_data->config_instance)
        return class_data->config_instance(obj);
    return Not_found_exp;
}

exception_t reset_conf_obj(conf_object_t * obj)
{
    conf_object_t *class_obj = get_conf_obj(obj->class_name);
    skyeye_class_t *class_data = class_obj->obj;

    if (class_data->reset_instance)
        return class_data->reset_instance(obj, NULL);

    /* can't find the class 's reset function */
    return Not_found_exp;
}

void free_conf_obj(conf_object_t * obj)
{
    conf_object_t *class_obj = get_conf_obj(obj->class_name);
    skyeye_class_t *class_data = class_obj->obj;

    if (class_data->free_instance)
    {
        delete_conf_obj(obj->objname);
        class_data->free_instance(obj);
    }
    return;
}

static void init_obj_list(void)
{
    obj_list = SKY_create_list();
}

static exception_t put_obj_node(void *obj)
{
    if (obj_list)
        SKY_list_push_back(obj_list, obj);
    return No_exp;
}

int delete_obj(void *node)
{
    conf_object_t *obj = (conf_object_t *) node;

    free_conf_obj(obj);
    return 0;
}

exception_t SKY_delete_all_obj(void)
{
    if (obj_list)
    {
        SKY_list_traveral(obj_list, delete_obj);
        SKY_list_clear(obj_list);
    }
    return No_exp;
}

int print_obj(void *node)
{
    conf_object_t *obj = (conf_object_t *) node;

    printf("%s\n", obj->objname);
    return 0;
}

void SKY_print_obj_list()
{
    if (obj_list)
    {
        SKY_list_traveral(obj_list, print_obj);
    }
    return;
}

static void init_class_list(void)
{
    class_list = SKY_create_list();
}

uint32_t SKY_get_obj_count()
{
    return SKY_list_count(obj_list);
}

conf_object_t *SKY_get_obj_index(int index)
{
    return SKY_list_get_node(obj_list, index);
}

exception_t put_class_node(void *cls)
{
    SKY_list_push_back(class_list, cls);
    return No_exp;
}

char **SKY_get_classname_list()
{
    unsigned class_num = SKY_list_count(class_list), i;

    for (i = 0; i < class_num; i++)
    {
        conf_class_t *cc = SKY_list_get_node(class_list, i);

        class_cache[i] = cc->class_data.class_name;
    }
    class_cache[i] = NULL;
    return class_cache;
}

int print(void *node)
{
    conf_class_t *cc = (conf_class_t *) node;

    printf("%s\n", cc->class_data.class_name);
    return 0;
}

void SKY_print_class_list()
{
    SKY_list_traveral(class_list, print);
    return;
}

conf_class_t *SKY_get_class(const char *name)
{
    unsigned class_num = SKY_list_count(class_list), i;
    char *class_name = NULL;

    for (i = 0; i < class_num; i++)
    {
        conf_class_t *cc = (conf_class_t *) SKY_list_get_node(class_list, i);

        class_name = cc->class_data.class_name;
        if (!strcmp(name, class_name))
            return cc;
    }
    return NULL;
}

exception_t _class_register_ifaces(conf_class_t * clss, const struct InterfaceDescription * ifaces, uint32_t count)
{
    char *name = clss->class_data.class_name;

    clss->class_data.iface_list.ifaces = ifaces;
    clss->class_data.iface_list.cnt = count;
    return No_exp;
}

exception_t _class_register_connects(conf_class_t * clss, const struct ConnectDescription * connects, uint32_t count)
{
    clss->class_data.connect_list.connects = connects;
    clss->class_data.connect_list.cnt = count;
    return No_exp;
}

char **class_get_iface_list(conf_class_t * clss)
{
    static char *iface_names[256];
    uint32_t count, i;
    const struct InterfaceDescription *ifaces = clss->class_data.iface_list.ifaces;

    count = clss->class_data.iface_list.cnt;
    for (i = 0; i < count; i++)
    {
        iface_names[i] = ifaces[i].name;
    }
    iface_names[i] = NULL;
    return iface_names;
}

char **class_get_connect_list(conf_class_t * clss)
{
    static char *connects_names[256];
    uint32_t count, i;
    const struct ConnectDescription *connects = clss->class_data.connect_list.connects;

    count = clss->class_data.connect_list.cnt;
    for (i = 0; i < count; i++)
    {
        connects_names[i] = connects[i].name;
    }
    connects_names[i] = NULL;
    return connects_names;
}

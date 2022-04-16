/*
 * skyeye_attribute.h:
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

#include <skyeye_attribute.h>
#include <skyeye_types.h>
#include <skyeye_mm.h>

const char *attr_cache[256];
const char *attr_info[5];
char attr_string_tmp[9];
using namespace std;
typedef list < attribute_t > ATTRIBUTE_LIST;

typedef struct attribute_list
{
    ATTRIBUTE_LIST obj;
    ATTRIBUTE_LIST clss;
} attribute_list_t;

void init_class_attribute_list(conf_class_t * clss)
{
    attribute_list_t *attr_list = new attribute_list_t;

    clss->attr = attr_list;
    return;
}

bool operator ==(const attribute_t & first, const attribute_t & other)
{
    return (strcmp(first.name, other.name) ? 0 : 1);
}

static ATTRIBUTE_LIST::iterator find_class_attribute(conf_class_t * clss, const char *name)
{
    attribute_t tmp;

    tmp.name = name;
    ATTRIBUTE_LIST::iterator it = std::find(clss->attr->clss.begin(), clss->attr->clss.end(), tmp);
    return it;
}

static ATTRIBUTE_LIST::iterator find_obj_attribute(conf_class_t * clss, const char *name)
{
    attribute_t tmp;

    tmp.name = name;
    ATTRIBUTE_LIST::iterator it = std::find(clss->attr->obj.begin(), clss->attr->obj.end(), tmp);
    return it;
}

ATTRIBUTE_LIST::iterator find_attribute(conf_class_t * clss, const char *name)
{
    attribute_t tmp;

    tmp.name = name;
    ATTRIBUTE_LIST::iterator it = std::find(clss->attr->obj.begin(), clss->attr->obj.end(), tmp);
    if (it == clss->attr->obj.end())
    {
        it = std::find(clss->attr->clss.begin(), clss->attr->clss.end(), tmp);
    }
    return it;
}

int SKY_class_has_attribute(conf_class_t * clss, const char *attr_name)
{
    int finded = 0;

    ATTRIBUTE_LIST::iterator it = find_attribute(clss, attr_name);
    if ((it != clss->attr->obj.end()) || (it != clss->attr->clss.end()))
    {
        finded = 1;
    } else
    {
        finded = 0;
    }
    return finded;
}

int SKY_print_attribute(conf_class_t * clss)
{
    ATTRIBUTE_LIST::iterator it = clss->attr->obj.begin();
    while (it != clss->attr->obj.end())
    {
        cout << "attribute name:" << it->name << "\ttype:" << it->type << endl;
        it++;
    }
    it = clss->attr->clss.begin();
    while (it != clss->attr->clss.end())
    {
        cout << "attribute name:" << it->name << "\ttype:" << it->type << endl;
        it++;
    }

    return 0;
}

exception_t SKY_change_attribute_attribute(conf_class_t * clss, const char *name, attr_attr_t attr)
{
    ATTRIBUTE_LIST::iterator it = find_attribute(clss, name);
    if (it != clss->attr->obj.end() || it != clss->attr->clss.end())
    {
        it->attr = attr;
    } else
    {
        /* TODO: change it with skyeye log */
        fprintf(stderr, "The class '%s' do not have the attribue '%s'\n", clss->class_data.class_name, name);
        return Not_found_exp;
    }
    return No_exp;
}

int SKY_register_attribute(conf_class_t * clss, const char *name, get_attr_t get_attr, void *get_args,
                           set_attr_t set_attr, void *set_args, attr_attr_t attr, const char *type, const char *desc)
{
    attribute_t tmp;

    tmp.name = name;
    tmp.get = get_attr;
    tmp.get_args = get_args;
    tmp.set = set_attr;
    tmp.set_args = set_args;
    tmp.attr = attr;
    tmp.type = type;
    tmp.desc = desc;
    clss->attr->obj.push_back(tmp);

    return 0;
}

int SKY_register_class_attribute(conf_class_t * clss, const char *name, get_class_attr_t get_attr, void *get_args,
                                 set_class_attr_t set_attr, void *set_args, attr_attr_t attr, const char *type,
                                 const char *desc)
{
    attribute_t tmp;

    tmp.name = name;
    tmp.class_get = get_attr;
    tmp.get_args = get_args;
    tmp.class_set = set_attr;
    tmp.set_args = set_args;
    tmp.attr = attr;
    tmp.type = type;
    tmp.desc = desc;
    clss->attr->clss.push_back(tmp);

    return 0;
}

static conf_class_t *get_obj_class(conf_object_t * obj)
{
    conf_class_t *clss = (conf_class_t *) (obj->clss);

    return clss;
}

exception_t SKY_set_attribute(conf_object_t * obj, const char *name, attr_value_t * value)
{
    conf_class_t *clss = get_obj_class(obj);

    ATTRIBUTE_LIST::iterator it = find_obj_attribute(clss, name);
    exception_t status = No_exp;

    if (it != clss->attr->obj.end())
    {
        status = it->set(NULL, obj, value, NULL);
    } else
    {
        status = Not_found_exp;
    }

    return status;
}

exception_t SKY_set_attribute_idx(conf_object_t * obj, const char *name, attr_value_t * index, attr_value_t * value)
{
    conf_class_t *clss = get_obj_class(obj);

    ATTRIBUTE_LIST::iterator it = find_obj_attribute(clss, name);
    exception_t status = No_exp;

    if (it != clss->attr->obj.end())
    {
        status = it->set(NULL, obj, value, index);
    } else
    {
        status = Not_found_exp;
    }
    return status;
}

attr_value_t SKY_get_attribute(conf_object_t * obj, const char *name)
{
    conf_class_t *clss = get_obj_class(obj);
    attr_value_t tmp;

    ATTRIBUTE_LIST::iterator it = find_obj_attribute(clss, name);
    if (it != clss->attr->obj.end())
    {
        tmp = it->get(NULL, obj, NULL);
    } else
    {
        tmp.type = Val_Invalid;
    }
    return tmp;
}

attr_value_t SKY_get_attribute_idx(conf_object_t * obj, const char *name, attr_value_t * index)
{
    conf_class_t *clss = get_obj_class(obj);
    attr_value_t tmp;

    ATTRIBUTE_LIST::iterator it = find_obj_attribute(clss, name);
    if (it != clss->attr->obj.end())
    {
        tmp = it->get(NULL, obj, index);
    } else
    {
        tmp.type = Val_Invalid;
    }
    return tmp;
}

exception_t SKY_set_class_attribute(conf_class_t * clss, const char *name, attr_value_t * value)
{
    ATTRIBUTE_LIST::iterator it = find_class_attribute(clss, name);
    exception_t status;

    if (it != clss->attr->clss.end())
    {
        status = it->class_set(NULL, clss, value, NULL);
    } else
    {
        status = Not_found_exp;
    }
    return status;
}

exception_t SKY_set_class_attribute_idx(conf_class_t * clss, const char *name, attr_value_t * index,
                                        attr_value_t * value)
{
    exception_t status;

    ATTRIBUTE_LIST::iterator it = find_class_attribute(clss, name);
    if (it != clss->attr->clss.end())
    {
        status = it->class_set(NULL, clss, value, index);
    } else
    {
        status = Not_found_exp;
    }
    return status;
}

attr_value_t SKY_get_class_attribute(conf_class_t * clss, const char *name)
{
    attr_value_t tmp;

    ATTRIBUTE_LIST::iterator it = find_class_attribute(clss, name);
    if (it != clss->attr->clss.end())
    {
        tmp = it->class_get(NULL, clss, NULL);
    } else
    {
        tmp.type = Val_Invalid;
    }
    return tmp;
}

attr_value_t SKY_get_class_attribute_idx(conf_class_t * clss, const char *name, attr_value_t * index)
{
    attr_value_t tmp;

    ATTRIBUTE_LIST::iterator it = find_class_attribute(clss, name);
    if (it != clss->attr->clss.end())
    {
        tmp = it->class_get(NULL, clss, index);
    } else
    {
        tmp.type = Val_Invalid;
    }
    return tmp;
}

const char **get_attributes(conf_object_t * obj)
{
    int i = 0;

    ATTRIBUTE_LIST::iterator it;

    conf_class_t *clss = get_obj_class(obj);
    const char **list = new const char *[clss->attr->obj.size() + 1];

    for (i = 0, it = clss->attr->obj.begin(); it != clss->attr->obj.end(); it++)
    {
        list[i++] = it->name;
    }
    list[i] = NULL;
    return list;
}

const char **get_class_attributes(conf_class_t * clss)
{
    int i = 0;

    ATTRIBUTE_LIST::iterator it;
    for (i = 0, it = clss->attr->obj.begin(); it != clss->attr->obj.end(); it++)
    {
        attr_cache[i++] = it->name;
    }
    attr_cache[i] = NULL;
    return attr_cache;
}

const char **get_class_attrinfo(conf_class_t * clss, const char *attrname)
{
    ATTRIBUTE_LIST::iterator it;
    int i;

    for (it = clss->attr->obj.begin(); it != clss->attr->obj.end(); it++)
    {
        if (!strcmp(it->name, attrname))
        {
            attr_info[0] = it->name;
            attr_info[1] = it->type;
            attr_info[2] = it->desc;
            //save uint32 to char[9]
            for (i = 0; i < 8; i++)
            {
                attr_string_tmp[i] = (((it->attr) >> (i * 4)) & (0x0000000F)) + '0';
            }
            attr_string_tmp[8] = 0;
            attr_info[3] = attr_string_tmp;
        }
    }
    attr_info[4] = NULL;
    return attr_info;
}

exception_t SKY_has_attribute(conf_object_t * obj, char *name)
{
    conf_class_t *clss = get_obj_class(obj);
    attr_value_t tmp;

    ATTRIBUTE_LIST::iterator it = find_obj_attribute(clss, name);
    if (it != clss->attr->obj.end())
    {
        return No_exp;
    }
    return Not_found_exp;
}

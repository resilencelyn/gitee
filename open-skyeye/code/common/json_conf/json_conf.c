/*
 * © Copyright 2014 eJim Lee. All Rights Reserved.
 * File:    json_conf.c
 * Email:   lyjforjob@gmail.com
 * Date:    2014-02-11
 */

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include "json_conf.h"
#include "json_base.h"
#include "json_parser.h"

static int strcmp_l(const char *left, const char *right)
{
    int t, l, r;

    assert(left && right);
    do
    {
        l = tolower(*left++);
        r = tolower(*right++);
        t = l - r;
    } while (l && t == 0);
    return t;
}

static obj_base *pick_obj_base(struct obj_desc *obj)
{
    obj_base *intf = NULL;

    for (intf = base_list; intf->name; intf++)
    {
        if (strcmp_l(obj->base, intf->name) == 0)
        {
            return intf;
        }
    }
    return NULL;
}

static int obj_create(struct obj_desc *obj, void *extra)
{
    obj_base *intf = pick_obj_base(obj);

    if (!intf)
    {
        fprintf(stderr, "your object \"%s\" have a undefined base \"%s\"\n", obj->name, obj->base);
        return -1;
    }
    return intf->create(obj, extra);
}

int objlist_create(struct obj_desc *objlist, void *extra)
{
    struct obj_desc *obj;
    int ret;

    for (obj = objlist; obj; obj = obj->next)
    {
        if ((ret = obj_create(obj, extra)) < 0)
            return ret;
    }
    return 0;
}

static int obj_set(struct obj_desc *obj, void *extra)
{
    obj_base *intf = pick_obj_base(obj);

    if (!intf)
    {
        fprintf(stderr, "your object \"%s\" have a undefined base \"%s\"\n", obj->name, obj->base);
        return -1;
    }
    return intf->set(obj, extra);
}

int objlist_set(struct obj_desc *objlist, void *extra)
{
    struct obj_desc *obj;
    int ret;

    for (obj = objlist; obj; obj = obj->next)
    {
        if ((ret = obj_set(obj, extra)) < 0)
            return ret;
    }
    return 0;
}

static int obj_conf_create(struct obj_desc *obj, void *extra)
{
    obj_base *intf = pick_obj_base(obj);
    int ret;

    if (!intf)
    {
        fprintf(stderr, "your object \"%s\" have a undefined base \"%s\"\n", obj->name, obj->base);
        return -1;
    }
    if ((ret = intf->create(obj, extra)) < 0)
        return ret;
    return 0;
}

static int obj_conf_set(struct obj_desc *obj, void *extra)
{
    obj_base *intf = pick_obj_base(obj);
    int ret;

    if (!intf)
    {
        fprintf(stderr, "your object \"%s\" have a undefined base \"%s\"\n", obj->name, obj->base);
        return -1;
    }
    if ((ret = intf->set(obj, extra)) < 0)
        return ret;
    return 0;
}

static int objlist_conf(struct obj_desc *objlist, void *extra)
{
    struct obj_desc *obj;
    int ret;

    for (obj = objlist; obj; obj = obj->next)
    {
        printf("CREATE NAME:%s\n", obj->name);
        if ((ret = obj_conf_create(obj, extra)) < 0)
            return ret;
    }
    for (obj = objlist; obj; obj = obj->next)
    {
        printf("SET NAME:%s\n", obj->name);
        if ((ret = obj_conf_set(obj, extra)) < 0)
            printf("Return:%s\n", obj->name);
        return ret;
    }
    return 0;
}

int json_conf(const char *conf, void *extra)
{
    struct obj_desc *objlist;
    int ret;

    if (conf == NULL)
    {
        fprintf(stderr, "conf path is empty.\n");
        return -1;
    }

    objlist = parse_file(conf);
    if (!objlist)
    {
        return -1;
    }
    //look_objlist(objlist, 0);
    ret = objlist_conf(objlist, extra);

    obj_desc_list_destroy(objlist);

    if (ret)
        return ret;
    return 0;
}

#if 0
int main(int argc, const char *argv[])
{
struct mach_t mach = memset(malloc(sizeof (struct; json_conf("device.json", NULL); return 0;}
#endif

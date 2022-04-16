/*
 * © Copyright 2014 eJim Lee. All Rights Reserved.
 * File:    json_base.h
 * Email:   lyjforjob@gmail.com
 * Date:    2014-02-11
 */

#ifndef __JSON_BASE_H__
#define __JSON_BASE_H__

#include "json_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* a list of base */
#define BASE_LIST(f) \
	f(mach) \
	f(cpu) \
	f(device)\
	f(linker)

    typedef int (*base_intf_cb) (struct obj_desc * obj, void *extra);
    typedef struct obj_base
    {
        const char *name;
        base_intf_cb create;
        base_intf_cb set;
    } obj_base;

    extern obj_base *base_list;

#ifdef __cplusplus
}
#endif

#endif

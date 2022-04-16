/*
 * © Copyright 2014 eJim Lee. All Rights Reserved.
 * File:    json_base.c
 * Email:   lyjforjob@gmail.com
 * Date:    2014-02-11
 */

#include <stdio.h>
#include "json_base.h"
#include "json_attr.h"
#include "json_conf.h"
#include "skyeye_mach.h"
#include "skyeye_class.h"
#include "skyeye_arch.h"
#include "skyeye_obj.h"
#include "skyeye_iface.h"
#include "skyeye_config.h"
#include "skyeye_options.h"
#include "skyeye_internal.h"
#include "skyeye_log.h"

/* NOTE: you should add new base info into base list on json_base.h
 * and add create, set function on this file
 */

#if 0
#define DBG(fmt, ...) fprintf(stdout, fmt, ##__VA_ARGS__)
#else
#define DBG(fmt, ...)
#endif

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#define check_attr_type_failed(attr, type) \
	eprintf("cpu attribute \"%s\" require a %s type.\n", attr, type);

#define DEAL_ATTR_SPEC(base) \
	struct attr_spec_intf *intf; \
	for(intf = base##_attr_spec_list; intf->key; intf++) { \
		if(strcmp(attr->key, intf->key) == 0) { \
			if(strcmp(attr->type, intf->type) != 0) { \
				check_attr_type_failed(intf->key, intf->type); \
				return -1; \
			} \
			intf->set(obj, attr, extra); \
			break; \
		} \
	} \
	if(intf->key == NULL) { \
		intf->set(obj, attr, extra); \
	}

#define BASE_SET(base) \
	struct attr_desc *attr; \
	for(attr = obj->attrs; attr; attr = attr->next) { \
		DBG("%s: %s attr(%s, %s, %s)\n", __func__, obj->name, \
				attr->key, attr->type, attr->val); \
		DEAL_ATTR_SPEC(base) \
	} \
	return objlist_set(obj->subobj, extra);

static int add_map_check(struct obj_desc *obj, void *extra)
{
    DBG("%s: %s\n", __func__, obj->name);
    struct attr_desc **attr_addr_len = extra;
    struct attr_desc *tmp, *list;
    int ADDRESSFLAG = 0, LENGTHFLAG = 0;

    list = obj->attrs;
    for (tmp = list; tmp != NULL; tmp = tmp->next)
    {
        if (!strncmp(tmp->key, "address", strlen("address")))
        {
            attr_addr_len[0] = tmp;
            ADDRESSFLAG = 1;
        } else if (!strncmp(tmp->key, "length", strlen("length")))
        {
            attr_addr_len[1] = tmp;
            LENGTHFLAG = 1;
        }
    }
    if (ADDRESSFLAG && LENGTHFLAG)
    {
        return 1;
    } else if (ADDRESSFLAG && LENGTHFLAG == 0)
    {
        skyeye_log(Error_log, __FUNCTION__, "Missing 'length' attribute\n");
        exit(0);
    } else if (ADDRESSFLAG == 0 && LENGTHFLAG)
    {
        skyeye_log(Error_log, __FUNCTION__, "Missing 'address' attribute\n");
        exit(0);
    }
    return 0;
}

static int add_map_func(struct obj_desc *obj, void *extra)
{

}
static int mach_create(struct obj_desc *obj, void *extra)
{
    DBG("%s: %s\n", __func__, obj->name);
    printf("machine create\n");
    skyeye_config_t *config = get_current_config();
    struct attr_desc *tmp, *list;
    int base;

    list = obj->attrs;
    for (tmp = list; tmp != NULL; tmp = tmp->next)
    {
        if (!strncmp(tmp->key, "brate", strlen("brate")))
        {
            if (!strncmp(tmp->val, "0x", 2))
            {
                base = 16;
            } else
            {
                base = 10;
            }
            config->cpu_rate = strtol(tmp->val, NULL, base);
        }
    }
    if (config->machs.soc_num == 0)
    {
        config->serial_count = 0;
    }
    conf_object_t *conf_obj = pre_conf_obj(obj->name, obj->cls);

    config->mach_obj = conf_obj;
    register_soc(conf_obj);

    return objlist_create(obj->subobj, obj);
}

static int mach_set(struct obj_desc *obj, void *extra)
{
BASE_SET(mach)}

static int cpu_create(struct obj_desc *obj, void *extra)
{
    DBG("%s: %s\n", __func__, obj->name);
    struct attr_desc *tmp, *list;
    int i;
    skyeye_config_t *config = get_current_config();

    list = obj->attrs;
    /*
     * for(tmp = list; tmp != NULL; tmp = list->next){
     * if(!strncmp(tmp->key, "type", strlen("type"))){
     * for (i = 0; i < (sizeof (arm_cpus) / sizeof (cpu_config_t)); i++) 
     * if (!strncmp(tmp->val, arm_cpus[i].cpu_name, strlen(arm_cpus[i].cpu_name))){
     * config->p_arm_cpu = &arm_cpus[i];
     * break;
     * }
     * break;
     * }
     * }
     * register_arch_option("arm");
     */
    conf_object_t *cpu = pre_conf_obj(obj->name, obj->cls);
    exception_t ret = reset_conf_obj(cpu);
    struct obj_desc *mach = extra;

    printf("machine name:%s\n", mach->name);
    conf_object_t *mach_obj = get_conf_obj(mach->name);

    //skyeye_machine_intf *inface = (skyeye_machine_intf *)SKY_get_interface(mach_obj, MACHINE_INTF_NAME);
    skyeye_machine_intf *inface = (skyeye_machine_intf *) SKY_get_iface(mach_obj, MACHINE_INTF_NAME);
    addr_space_t *space = (addr_space_t *) SKY_get_iface(cpu, ADDR_SPACE_INTF_NAME);

    inface->set_space(mach_obj, space);
    inface->set_core(mach_obj, cpu);
    return objlist_create(obj->subobj, extra);
}

static int cpu_set(struct obj_desc *obj, void *extra)
{
BASE_SET(cpu)}

static int device_create(struct obj_desc *obj, void *extra)
{
    DBG("%s: %s\n", __func__, obj->name);
    printf("device create\n");
    struct obj_desc *mach = extra;
    struct attr_desc *attr_addr_len[2];
    struct attr_desc *tmp, *list;
    int ret;
    conf_object_t *mach_obj = get_conf_obj(mach->name);
    conf_object_t *device = pre_conf_obj(obj->name, obj->cls);

    register_device_to_soc(mach_obj, device);
    reset_conf_obj(device);

    ret = add_map_check(obj, attr_addr_len);
    if (ret)
    {
        memory_space_intf *io_memory = (memory_space_intf *) SKY_get_iface(device, MEMORY_SPACE_INTF_NAME);
        skyeye_machine_intf *inface = (skyeye_machine_intf *) SKY_get_iface(mach_obj, MACHINE_INTF_NAME);
        addr_space_t *space = inface->get_space(mach_obj);

        ret =
            SKY_add_map(space, device, strtol(attr_addr_len[0]->val, NULL, 16), strtol(attr_addr_len[1]->val, NULL, 16),
                        0x0, io_memory, 1, 1);
    }
    return objlist_create(obj->subobj, extra);
}

static int device_set(struct obj_desc *obj, void *extra)
{
BASE_SET(device)}

static int linker_create(struct obj_desc *obj, void *extra)
{
    printf("linker create\n");
    conf_object_t *device = pre_conf_obj(obj->name, obj->cls);

    return objlist_create(obj->subobj, extra);
}

static int linker_set(struct obj_desc *obj, void *extra)
{

}

/* create list of base info about forms such as creating
 * and setting for object
 */
#define BASE_LIST_DEF(base) \
	{ \
		.name = #base, \
		.create = base##_create, \
		.set = base##_set, \
	},
static obj_base base_list_in[] = {
    BASE_LIST(BASE_LIST_DEF) {
                              .name = NULL,
                              .create = NULL,
                              .set = NULL,
                              },
};
obj_base *base_list = base_list_in;

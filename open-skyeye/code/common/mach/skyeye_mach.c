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
* @file skyeye_mach.c
* @brief the machine module of SkyEye
* @author Michael.Kang blackfin.kang@gmail.com
* @version 
* @date 2011-04-30
*/

#include <stdlib.h>
#include "skyeye_mm.h"
#include "skyeye_mach.h"
#include "skyeye_options.h"
#include "skyeye_config.h"
#include "skyeye_log.h"
#include "skyeye_class.h"
#include "skyeye_types.h"

/**
* @brief the supported machine list
*/
static machine_config_t *mach_list;

/**
* @brief the handler of machine option
*
* @param this_option
* @param num_params
* @param params[]
*
* @return 
*/
int do_mach_option(skyeye_option_t * this_option, int num_params, const char *params[])
{
    int ret;
    machine_config_t *mach = get_mach(params[0]);
    skyeye_config_t *config = get_current_config();

    if (mach != NULL)
    {
        config->mach = mach;
        skyeye_log(Info_log, __FUNCTION__, "mach info: name %s, mach_init addr %p\n", config->mach->machine_name,
                   config->mach->mach_init);
        ret = 0;
    } else
    {
        sprintf(config->config_mach, "%s", params[0]);
        //conf_object_t * conf_obj = pre_conf_obj("am335x_mach", params[0]);
        //config->mach_obj = conf_obj;
        //if(conf_obj == NULL){
        //        SKYEYE_ERR ("Error: Unknown mach name \"%s\"\n", params[0]);
        //      ret = -1;
        //}
    }
    return ret;
}

/**
* @brief initialization of machine module
*/
void init_mach()
{
    mach_list = NULL;
    register_option("mach", do_mach_option, "machine option");
}

/**
* @brief register a machine to the simulator
*
* @param mach_name
* @param mach_init
*/
void register_mach(const char *mach_name, mach_init_t mach_init)
{
    machine_config_t *mach;

    mach = skyeye_mm_zero(sizeof (machine_config_t));
    mach->machine_name = skyeye_strdup(mach_name);
    mach->mach_init = mach_init;
    mach->next = mach_list;
    mach_list = mach;
    //skyeye_log(Debug_log, __FUNCTION__, "regiser mach %s successfully.\n", mach->machine_name);
}

/**
* @brief get a machine struct by its name
*
* @param mach_name
*
* @return 
*/
machine_config_t *get_mach(const char *mach_name)
{
    machine_config_t *node;

    node = mach_list;
    while (node)
    {
        //if(!strncmp(node->machine_name, mach_name, strlen(node->machine_name))){
        if (!strcmp(node->machine_name, mach_name))
        {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

#if 0
machine_config_t *get_mach(skyeye_config_t * config)
{
    return config->mach;
}
#endif

/**
* @brief get the whole machine list
*
* @return 
*/
machine_config_t *get_mach_list()
{
    return mach_list;
}

/**
* @brief get the current running machine 
*
* @return 
*/
machine_config_t *get_current_mach()
{
    skyeye_config_t *config = get_current_config();

    return config->mach;
}

/**************************NEW ARCHITECTURE MACHINE**************************************/

exception_t free_all_soc(void)
{
    int i;
    skyeye_config_t *config = get_current_config();
    mach_conf_t *machs = &(config->machs);

    for (i = 0; i < machs->soc_num; i++)
    {
        skyeye_free(machs->soc[i]);
    }
    machs->soc = NULL;
    skyeye_free(machs->soc);
    machs->soc_num = 0;
    return No_exp;
}

exception_t register_soc(conf_object_t * soc)
{
    skyeye_config_t *config = get_current_config();
    mach_conf_t *machs = &(config->machs);

    machs->soc = skyeye_realloc(machs->soc, sizeof (soc_t *) * (++machs->soc_num));
    *(machs->soc + machs->soc_num - 1) = skyeye_mm(sizeof (soc_t));
    memset(*(machs->soc + machs->soc_num - 1), 0, sizeof (soc_t));
    machs->soc[machs->soc_num - 1]->mach = soc;
    return No_exp;
}

exception_t register_soc_elfname(conf_object_t * soc, char *elfname)
{
    int i;
    skyeye_config_t *config = get_current_config();
    mach_conf_t *machs = &(config->machs);
    int filename_len = strlen(elfname) + 1;

    if (filename_len >= MAX_ELF_FILENAME_LEN)
        return Invarg_exp;
    for (i = 0; i < machs->soc_num; i++)
    {
        if (soc == machs->soc[i]->mach)
        {
            machs->soc[i]->elfname = skyeye_strdup(elfname);
        }
    }
    return No_exp;
}

char *get_soc_elfname(conf_object_t * soc)
{
    int i;
    skyeye_config_t *config = get_current_config();
    mach_conf_t *machs = &(config->machs);

    for (i = 0; i < machs->soc_num; i++)
    {
        if (soc == machs->soc[i]->mach)
        {
            return machs->soc[i]->elfname;
        }
    }
    return NULL;
}

exception_t register_device_to_soc(conf_object_t * soc, conf_object_t * device)
{
    int i, num;
    skyeye_config_t *config = get_current_config();
    mach_conf_t *machs = &(config->machs);
    conf_object_t **devices;

    for (i = 0; i < machs->soc_num; i++)
    {
        if (soc == machs->soc[i]->mach)
        {
            machs->soc[i]->devices =
                skyeye_realloc(machs->soc[i]->devices, sizeof (conf_object_t *) * (++machs->soc[i]->devices_num));
            devices = machs->soc[i]->devices;
            num = machs->soc[i]->devices_num;
            devices[num - 1] = device;
        }
    }
    return No_exp;
}

void print_socs(void)
{
    int i, j, soc_num, dev_num;
    skyeye_config_t *config = get_current_config();

    soc_num = config->machs.soc_num;
    soc_t *soc;
    conf_object_t **devices;

    for (i = 0; i < soc_num; i++)
    {
        soc = config->machs.soc[i];
        devices = soc->devices;
        dev_num = soc->devices_num;
        for (j = 0; j < dev_num; j++)
        {
            printf("%s\n", devices[j]->objname);
        }
    }
}
int get_soc_num(void)
{
    int soc_num;
    skyeye_config_t *config = get_current_config();

    soc_num = config->machs.soc_num;
    return soc_num;
}

char *get_soc_num_name_by_id(int id)
{
    skyeye_config_t *config = get_current_config();
    mach_conf_t *machs = &(config->machs);

    return machs->soc[id]->mach->objname;
}

int mach_set_space(common_machine_attr_t * mach_attr, addr_space_t * space)
{
    mach_attr->space = space;
    return 0;
}

addr_space_t *mach_get_space(common_machine_attr_t * mach_attr)
{
    return mach_attr->space;
}

int mach_set_core(common_machine_attr_t * mach_attr, conf_object_t * core)
{
    mach_attr->core[mach_attr->core_num++] = core;
    if (mach_attr->core_num >= MAX_CORE_NUM)
    {
        skyeye_log(Error_log, __FUNCTION__, "This MACHINE supports up to 8-cores\n");
    }
    return 0;
}

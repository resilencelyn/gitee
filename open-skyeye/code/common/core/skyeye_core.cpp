/*
 * skyeye_arch.c -  all architecture definition for skyeye
 * Copyright (C) 2003 Skyeye Develop Group
 * for help please send mail to <skyeye-developer@lists.sf.linuxforum.net>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */
/*
 * 12/16/2006   Michael.Kang  <blackfin.kang@gmail.com>
 */

#include "skyeye_core.h"
#include "skyeye_core_intf.h"
#include "skyeye_config.h"
#include "skyeye_options.h"
#include "skyeye_mm.h"
#include "skyeye_log.h"
#include "skyeye_iface.h"
#include "skyeye_attr.h"
#include "skyeye_exec.h"
#include "skyeye_thread.h"
#include <stdlib.h>

#include <list>
using namespace std;

list < conf_object_t * >core_list;

conf_object_t *SIM_current_processor()
{
    /* Casting a conf_obj_t to ARM_CPU_State type */
    pthread_t id = pthread_self();

    /* Judge if we are running in paralell or sequenial */
    if (thread_exist(id))
    {
        conf_object_t *core = get_current_exec_priv(id);

        return core;
    } else
        return NULL;
}

conf_object_t *SIM_get_processor(int proc_no)
{
    return NULL;
}

generic_address_t SIM_get_current_pc()
{
    conf_object_t *core = SIM_current_processor();
    skyeye_reg_intf *reg_intf = (skyeye_reg_intf *) SKY_get_iface(core, SKYEYE_REG_INTF);
    int pc_idx = reg_intf->get_regid_by_name(core, "PC_REG");

    return reg_intf->get_regvalue_by_id(core, pc_idx);
}

generic_address_t SIM_get_pc(conf_object_t * conf_obj)
{
    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(conf_obj, CORE_INFO_INTF_NAME);

    return core_info->get_pc(conf_obj);
}

void SIM_set_pc(conf_object_t * conf_obj, generic_address_t pc, generic_address_t code_start,
                generic_address_t code_end)
{
    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(conf_obj, CORE_INFO_INTF_NAME);

    core_info->set_pc(conf_obj, pc);

    if (core_info->set_textsection_info)
        core_info->set_textsection_info(conf_obj, code_start, code_end);

    return;
}

void SIM_add_processor(conf_object_t * cpu)
{
    core_list.push_back(cpu);
    return;
}

/*
 * attr_value_t SIM_get_all_processors(){
 * return ;
 * }
 */

conf_object_t *SIM_current_processor(int index)
{
    return NULL;
}

int SIM_number_processors()
{
    return core_list.size();
}

int SIM_step_count()
{
    return 0;
}

void SIM_reset_prcessor(conf_object_t * cpu, int hard_reset)
{
    return;
}

skyeye_reg_intf *SIM_get_current_reg_intf()
{
    conf_object_t *core = SIM_current_processor();

    return (skyeye_reg_intf *) SKY_get_iface(core, SKYEYE_REG_INTF);
}

uint64_t SIM_get_current_steps()
{
    conf_object_t *core = SIM_current_processor();
    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    return core_info->get_steps(core_info->conf_obj);
}

uint64_t skyeye_get_core_steps(conf_object_t * core)
{
    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    return core_info->get_steps(core);
}

align_t SIM_get_current_align()
{
    conf_object_t *core = SIM_current_processor();
    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    return core_info->get_alignment(core);
}

endian_t SIM_get_current_endianess()
{
    /* FIXME, need to get current endian */
#if 0
    conf_object_t *core = SIM_current_processor();
    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    return core_info->get_endian(core);
    return Little_endian;
#endif
    conf_object_t *core = SIM_current_processor();
    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    return core_info->get_endian(core);
}

core_running_mode_t SIM_get_core_mode(conf_object_t * conf_obj)
{
    core_running_mode_t mode = RUN_MODE_NUM;

    if (!SKY_has_attribute(conf_obj, "mode"))
    {
        attr_value_t attr = SKY_get_attribute(conf_obj, "mode");

        mode = (core_running_mode_t) SKY_attr_uinteger(attr);
    }
    return mode;
}

void SIM_set_core_mode(conf_object_t * conf_obj, core_running_mode_t mode)
{
    attr_value_t value;

    if (!SKY_has_attribute(conf_obj, "mode"))
    {
        value = SKY_make_attr_uinteger(mode);
        SKY_set_attribute(conf_obj, "mode", &value);
    }
    return;
}

int SIM_get_core_debug_mode(conf_object_t * conf_obj)
{
    int mode = -1;

    if (!SKY_has_attribute(conf_obj, "debug_mode"))
    {
        attr_value_t attr = SKY_get_attribute(conf_obj, "debug_mode");

        mode = (core_running_mode_t) SKY_attr_uinteger(attr);
    }
    return mode;
}

void SIM_set_core_debug_mode(conf_object_t * conf_obj)
{
    attr_value_t value;

    if (!SKY_has_attribute(conf_obj, "debug_mode"))
    {
        value = SKY_make_attr_uinteger(1);
        SKY_set_attribute(conf_obj, "debug_mode", &value);
    }
    return;
}

/*
 * skyeye2gdb.c - necessary definition for skyeye debugger
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
 * 12/04/2005   ksh  <blackfin.kang@gmail.com>
 * */

//#include "armdefs.h"

#include "skyeye_types.h"
#include "skyeye_ram.h"
#include "sim_control.h"
#include "skyeye_arch.h"
#include "breakpoint.h"
#include <stdlib.h>
#include <stdint.h>
#include "skyeye_addr_space.h"
#include "skyeye_iface.h"
#include "skyeye_obj.h"
#include "skyeye_core_intf.h"
#include "skyeye_pref.h"
#include "skyeye2gdb.h"
int frommem(endian_t endian, unsigned char *memory)
{
    if (endian == Big_endian)
    {
        return (memory[0] << 24) | (memory[1] << 16) | (memory[2] << 8) | (memory[3] << 0);
    } else
    {
        return (memory[3] << 24) | (memory[2] << 16) | (memory[1] << 8) | (memory[0] << 0);
    }
}

void tomem(endian_t endian, unsigned char *memory, int val)
{
    if (endian == Big_endian)
    {
        memory[0] = val >> 24;
        memory[1] = val >> 16;
        memory[2] = val >> 8;
        memory[3] = val >> 0;
    } else
    {
        memory[3] = val >> 24;
        memory[2] = val >> 16;
        memory[1] = val >> 8;
        memory[0] = val >> 0;
    }
}

#if 0
ARMword ARMul_Debug(ARMul_State * state, ARMword pc ATTRIBUTE_UNUSED, ARMword instr ATTRIBUTE_UNUSED)
{
    state->Emulate = STOP;
    stop_simulator = 1;
    return 1;
}
#endif

int sim_write(conf_object_t * core, generic_address_t addr, unsigned char *buffer, int size, int byte_width)
{
    int ret;

    memory_space_intf *sim_ms = SKY_get_iface(core, MEMORY_SPACE_INTF_NAME);

    if (sim_ms)
    {
        int i = 0;

        for (; i < size; i++)
        {
            ret = sim_ms->write(core, addr + i, buffer + i * byte_width, byte_width);

        }
    }
#if 0
    addr_space_t *space = get_space_from_attr(core);

    if (space != NULL)
    {
        ret = space->memory_space->write(space->obj, addr, buffer, size);
    }
#endif
#if 0
    generic_arch_t *arch_instance = get_arch_instance(config->arch->arch_name);

    for (i = 0; i < size; i++)
    {
        if (arch_instance->mmu_write != NULL)
            fault = arch_instance->mmu_write(8, addr + i, buffer[i]);
        else
            mem_write(8, addr + i, buffer[i]);
        if (fault)
            return -1;
    }
#endif
    return size;
}

int sim_read(conf_object_t * core, generic_address_t addr, unsigned char *buffer, int size, int byte_width)
{
    int ret;

    memory_space_intf *sim_ms = SKY_get_iface(core, MEMORY_SPACE_INTF_NAME);

    if (sim_ms)
    {
        int i = 0;

        for (; i < size; i++)
        {
            //printf("in %s, size/8=%d, size=%d\n", __FUNCTION__, size/8, size);
            ret = sim_ms->read(core, addr + i, buffer + i * byte_width, byte_width);
        }
    }
#if 0                                   //du
    addr_space_t *space = get_space_from_attr(core);

    if (space != NULL)
    {
        int i = 0;

        for (; i < size; i++)
        {
            //printf("in %s, size/8=%d, size=%d\n", __FUNCTION__, size/8, size);
            ret = space->memory_space->read(space->obj, addr + i, buffer + i, 1);
        }
    }
#endif

#if 0
    generic_arch_t *arch_instance = get_arch_instance(config->arch->arch_name);

    for (i = 0; i < size; i++)
    {
        if (arch_instance->mmu_read != NULL)
            fault = arch_instance->mmu_read(8, addr + i, &v);
        else
            fault = mem_read(8, addr + i, &v);
        if (fault)
            return -1;
        buffer[i] = v;
    }
#endif
    return size * byte_width;
}

void gdbserver_cont(conf_object_t * conf_obj)
{
#if 0
    if (!strcmp(skyeye_config.arch->arch_name, "arm"))
    {
        //chy 2006-04-12
        state->NextInstr = RESUME;      /* treat as PC change */
        state->Reg[15] = ARMul_DoProg(state);
    } else
        sim_resume(0);
#endif
    SIM_continue(0);
    while (SIM_is_running())
    {
        remote_interrupt(conf_obj);
    }
    return;
}

void gdbserver_step(conf_object_t * core)
{
    skyeye_core_stepi(core, 1);
    while (SIM_is_running())
        ;
}

signal_status_t sim_get_signal_status(conf_object_t * core)
{
    signal_status_t ret;
    core_signal_intf *core_signal = SKY_get_iface(core, CORE_SIGNAL_INTF_NAME);

    if (core_signal)
    {
        if (core_signal->get_signal_status)
        {
            ret = core_signal->get_signal_status(core);
        }
    }
    return ret;
}

int sim_ice_breakpoint_remove(conf_object_t * core, generic_address_t addr)
{
    int ret;

#if 0
    addr_space_t *space = get_space_from_attr(core);
    breakpoint_mgt_t *break_manager = &space->bp_manager;

    ret = remove_bp_by_addr(break_manager, addr);
#endif
    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    ret = iface->delete_bp(core, addr);
    return ret;
}

int sim_ice_breakpoint_insert(conf_object_t * core, generic_address_t addr)
{
    int ret;
    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

#if 0
    addr_space_t *space = get_space_from_attr(core);

    breakpoint_mgt_t *break_manager = &space->bp_manager;

    ret = create_a_breakpoint(break_manager, addr);
#endif
    ret = iface->insert_bp(core, addr);
    return ret;
}

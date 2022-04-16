/*
 *      PearPC
 *      ppc_cpu.cc
 *
 *      Copyright (C) 2003, 2004 Sebastian Biallas (sb@biallas.net)
 *      Portions Copyright (C) 2004 Apple Computer, Inc.
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License version 2 as
 *      published by the Free Software Foundation.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "ppc_cpu.h"
#include "ppc_dec.h"
#include "ppc_fpu.h"
#include "ppc_exc.h"
#include "ppc_mmu.h"
#include "ppc_tools.h"
#include "tracers.h"
#include "portable/portable.h"
#include <skyeye_disassemble.h>

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
//#include "dyncom/ppc_dyncom_run.h"

//#include "io/graphic/gcard.h"

static bool_t gSinglestep = False;

//uint32 gBreakpoint2 = 0x11b3acf4;
uint32 gBreakpoint3 = 0xc016ee74 & 0;
uint32 gBreakpoint = 0x11b3acf4 & 0;
uint32 gBreakpoint2 = 0xc017a4f4 & 0;

bool_t activate = False;
static inline void ppc_debug_hook(e500_core_t *current_core)
{
    if (current_core->pc == gBreakpoint)
    {
        gSinglestep = True;
        //              SINGLESTEP("breakpoint 1");
    }
    if (current_core->pc == gBreakpoint2)
    {
        //SINGLESTEP("breakpoint 2");
    }
    //      if (current_core->pc == gBreakpoint3 && current_core->gpr[5]==0x100004ec) {
    /*      if (current_core->pc == gBreakpoint3) {
     * activate = True;
     * SINGLESTEP("breakpoint 3");
     * } */
    /*
     * if (gSinglestep) {
     * gDebugger->enter();
     * } */
}

/*sys_mutex exception_mutex; */

void ppc_cpu_atomic_raise_ext_exception(e500_core_t *current_core)
{
    /*sys_lock_mutex(exception_mutex); */
    current_core->ext_exception = True;
    current_core->exception_pending = True;
    /*sys_unlock_mutex(exception_mutex); */
}

void ppc_cpu_atomic_cancel_ext_exception(e500_core_t *current_core)
{
    /* sys_lock_mutex(exception_mutex); */
    current_core->ext_exception = False;
    if (!current_core->dec_exception)
        current_core->exception_pending = False;
    /*sys_unlock_mutex(exception_mutex); */
}

void ppc_cpu_atomic_raise_dec_exception(e500_core_t *current_core)
{
    /*sys_lock_mutex(exception_mutex); */
    current_core->dec_exception = True;
    current_core->exception_pending = True;
    /*sys_unlock_mutex(exception_mutex); */
}

uint64 ppc_get_clock_frequency(int cpu)
{
    return PPC_CLOCK_FREQUENCY;
}

uint64 ppc_get_bus_frequency(int cpu)
{
    return PPC_BUS_FREQUENCY;
}

uint64 ppc_get_timebase_frequency(int cpu)
{
    return PPC_TIMEBASE_FREQUENCY;
}

void ppc_machine_check_exception()
{
    PPC_CPU_ERR("machine check exception\n");
}

uint32 ppc_cpu_get_gpr(e500_core_t *current_core, int cpu, int i)
{
    return current_core->gpr[i];
}

void ppc_cpu_set_gpr(e500_core_t *current_core, int cpu, int i, uint32 newvalue)
{
    current_core->gpr[i] = newvalue;
}

void ppc_cpu_set_msr(e500_core_t *current_core, int cpu, uint32 newvalue)
{
    current_core->msr = newvalue;
}

void ppc_cpu_set_pc(e500_core_t *current_core, int cpu, uint32 newvalue)
{
    current_core->pc = newvalue;
    current_core->npc = current_core->pc + 4;
}

uint32 ppc_cpu_get_pc(e500_core_t *current_core, int cpu)
{
    return current_core->pc;
}

uint32 ppc_cpu_get_pvr(e500_core_t * core)
{
    return core->pvr;
}

void ppc_cpu_map_framebuffer(e500_core_t *current_core, uint32 pa, uint32 ea)
{
    // use BAT for framebuffer
    current_core->dbatu[0] = ea | (7 << 2) | 0x3;
    current_core->dbat_bl17[0] = ~(BATU_BL(current_core->dbatu[0]) << 17);
    current_core->dbatl[0] = pa;
}

void ppc_set_singlestep_v(bool_t v, const char *file, int line, const char *format, ...)
{
    va_list arg;

    va_start(arg, format);
    ht_fprintf(stdout, "singlestep %s from %s:%d, info: ", v ? "set" : "cleared", file, line);
    ht_vfprintf(stdout, format, arg);
    ht_fprintf(stdout, "\n");
    va_end(arg);
    gSinglestep = v;
}

void ppc_set_singlestep_nonverbose(bool_t v)
{
    gSinglestep = v;
}

uint32_t ppc_get_address_width(conf_object_t * obj)
{
    return 4;
}

char *ppc_disassemble(conf_object_t * obj, generic_address_t addr)
{
    e500_core_t *core = (e500_core_t *) obj->obj;

    return skyeye_disassemble(obj, addr);
}

void ppc_set_memory_watch_status(conf_object_t * obj, uint32_t addr, int status)
{
    e500_core_t *core = (e500_core_t *) obj->obj;

    if (core->mode != 0)
        //set_dyncom_memory_watch_status(core, addr, status);
    return;
}

int ppc_get_cpu_exception_num(conf_object_t * obj)
{
    e500_core_t *core = (e500_core_t *) obj->obj;

    return sizeof (core->ivor) / sizeof (core->ivor[0]);
}

void ppc_set_cpu_exception_by_id(conf_object_t * obj, int exception_id)
{
    e500_core_t *core = (e500_core_t *) obj->obj;

    if (core->arch_type == E200_FAMILY)
    {
        e500_ppc_exception(core, exception_id, 0, core->pc);
    } else
    {
        if ((exception_id << 8) == PPC_EXC_MACHINE_CHECK)
        {
            //Enable Machine Check Interrupt
            core->msr |= MSR_ME;
        }
        e600_ppc_exception(core, exception_id << 8, 0, core->pc);
    }
    return;
}

void clear_msr_special_bits(e500_core_t *current_core)
{
    current_core->msr &= ~MSR_WE;
    current_core->msr &= ~MSR_EE;
    current_core->msr &= ~MSR_PR;
    current_core->msr &= ~MSR_IS;
    current_core->msr &= ~MSR_DS;
    current_core->msr &= ~MSR_FP;
    current_core->msr &= ~MSR_FE0;
    current_core->msr &= ~MSR_FE1;
}

char *ppc_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id)
{
    e500_core_t *core = (e500_core_t *) obj->obj;

    strcpy(core->exception_name, "IVOR");

    assert(exception_id < (sizeof (core->ivor) / sizeof (core->ivor[0])));
    sprintf(core->exception_name, "%s%d", core->exception_name, exception_id);
    return core->exception_name;
}

#define CPU_KEY_PVR	"cpu_pvr"

//#include "configparser.h"
#define MPC8560_DPRAM_SIZE 0xC000
#define MPC8560_IRAM_SIZE 0x8000

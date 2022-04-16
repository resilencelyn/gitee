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
* @file ftc662_core.c
* @brief The arm core class
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-21
*/

#include "armdefs.h"
#include "armcpu.h"
#include "armemu.h"
#include "arm_regformat.h"
#include "vfp/vfp.h"
#include "arch64_vfp.h"
//#include <dyncom/defines.h>

#include <skyeye_mm.h>
#include <skyeye_class.h>
#include <skyeye_core_intf.h>
#include <skyeye_dev_intf.h>
#include <skyeye_iface.h>
#include <skyeye_attribute.h>
#include <memory_space.h>
#include <sim_control.h>
#include <skyeye_callback.h>
#include <breakpoint.h>
#include <skyeye_disassemble.h>
#include "arch64_memory.h"
//#include "arm_dyncom_interface.h"

typedef struct ftc662_core
{
    conf_object_t *obj;
    ARMul_State *state;
    generic_address_t load_addr;
    uint32_t mask;
    breakpoint_mgt_t bp_manager;
    core_running_mode_t mode;
    struct
    {
        int num;
        conf_object_t *obj;
        general_signal_intf *intf;
    } signal;
    uint32_t isWake;
} ftc662_core_t;

exception_t ftc662_memory_read(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;

    arch64_mem_read(state, addr, AccType_NORMAL, buf, NULL, count);
    return No_exp;
}

exception_t ftc662_memory_write(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;

    arch64_mem_write(state, addr, AccType_NORMAL, *(uint32_t *) buf, count);
    return No_exp;
}

static exception_t memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;
    ARMul_State *state = core->state;

    state->memory_space = obj2;
    state->memory_space_iface = (memory_space_intf *) SKY_get_iface(state->memory_space, MEMORY_SPACE_INTF_NAME);

    if (state->memory_space_iface == NULL)
        skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' interface from %s fail.", obj2->objname);
    return No_exp;
}

static exception_t memory_space_get(conf_object_t * obj, conf_object_t ** obj2, const char **port, int index)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;

    *obj2 = core->state->memory_space;
    *port = NULL;
    return No_exp;
}

static exception_t gic_interface_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;
    ARMul_State *state = core->state;

    state->gic = obj2;
    state->gic_iface = (gic_interface_intf *) SKY_get_iface(state->gic, GIC_INTERFACE_INTF);

    if (state->gic_iface == NULL)
        skyeye_log(Warning_log, __FUNCTION__, "Get 'gic_iface' interface from %s fail.", obj2->objname);
    return No_exp;
}

static exception_t gic_interface_get(conf_object_t * obj, conf_object_t ** obj2, const char **port, int index)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;

    *obj2 = core->state->gic;
    *port = NULL;
    return No_exp;
}

static exception_t event_signal_interface_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;
    ARMul_State *state = core->state;

    state->event = obj2;
    state->event_iface = (multcore_event_signal_intf *) SKY_get_iface(state->event, MULTCORE_EVENT_SIGNAL_INTF);
    if (state->event_iface == NULL)
        skyeye_log(Warning_log, __FUNCTION__, "Get 'event_iface' interface from %s fail.", obj2->objname);

    return No_exp;
}

static exception_t event_signal_interface_get(conf_object_t * obj, conf_object_t ** obj2, const char **port, int index)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;
    *obj2 = core->state->event;
    *port = NULL;
    return No_exp;
}


static exception_t ftc662_event_signal_set(conf_object_t *obj, size_t sev, int cpu_id)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;
    ARMul_State *state = core->state;
    if(cpu_id == 0 || cpu_id == state->cpuID)
    {
        core->isWake = (uint32_t)sev;
    }
    return No_exp;
}

static exception_t ftc662_event_signal_get(conf_object_t *obj, size_t *sev, int cpu_id)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;
    ARMul_State *state = core->state;
    if(cpu_id == state->cpuID)
    {
        *sev = (size_t)core->isWake;
    }
    return No_exp;
}

static uint32 ftc662_arm_get_pc(conf_object_t * opaque)
{
    ftc662_core_t *core = (ftc662_core_t *) opaque->obj;
    uint32 pc = arm_get_pc(core->state);

    return pc;
}

static uint32 ftc662_get_regnum(conf_object_t * opaque)
{
    ftc662_core_t *core = (ftc662_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    if (state->isAArch64)
        return arm_arch64_get_regnum(state);
    else
        return arm_get_regnum(state);
}

static uint32_t ftc662_get_regid_by_name(conf_object_t * opaque, char *name)
{
    ftc662_core_t *core = (ftc662_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    if (state->isAArch64)
        return arm_arch64_get_regid_by_name(state, name);
    else
        return arm_get_regid_by_name(state, name);
}

static char *ftc662_get_regname_by_id(conf_object_t * opaque, int id)
{
    ftc662_core_t *core = (ftc662_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    if (state->isAArch64)
        return arm_arch64_get_regname_by_id(state, id);
    else
        return arm_get_regname_by_id(state, id);
}

static exception_t ftc662_set_regval_by_id(conf_object_t *opaque, uint64 value, int id)
{
    ftc662_core_t *core = (ftc662_core_t *)opaque->obj;
    ARMul_State *state = core->state;

    if (state->isAArch64)
    {
        // FIXME: Need to separate the 32 and 64 bit arch interface
         return arm_arch64_set_regvalue_by_id(state, value, id);
        //skyeye_log(Error_log, __FUNCTION__, "Setting 64 bit register value is not supported yet.");
    } else
    {
        return arm_set_regvalue_by_id(state, value, id);
    }
}

static uint64 ftc662_get_regval_by_id(conf_object_t * opaque, int id)
{
    ftc662_core_t *core = (ftc662_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    if (state->isAArch64)
        return arm_arch64_get_regvalue_by_id(state, id);
    else
        return arm_get_regvalue_by_id(state, id);
}

static exception_t ftc662_signal(conf_object_t * opaque, interrupt_signal_t * signal)
{
    ftc662_core_t *core = (ftc662_core_t *) opaque->obj;
    ARMul_State *state = core->state;
    arm_signal_t *arm_signal = &signal->arm_signal;

    if (arm_signal->irq != Prev_level)
        state->NirqSig = arm_signal->irq;

    if (arm_signal->firq != Prev_level)
        state->NfiqSig = arm_signal->firq;

    /*use reset signal to trigger SError Exception */
    if (arm_signal->reset != Prev_level)
        state->NresetSig = arm_signal->reset;

    return No_exp;
}

void check_timer_interrupt(ftc662_core_t * core)
{
    ARMul_State *state = core->state;

    state->system_reg.CNTVCT_EL0++;
    state->system_reg.CNTPCT_EL0++;

    //Virtual Timer
    if (state->system_reg.CNTV_CTL_EL0.ENABLE)
    {
        if (state->system_reg.CNTVCT_EL0 == state->system_reg.CNTV_TVAL_EL0)
        {
            state->system_reg.CNTV_CTL_EL0.ISTATUS = 1;
            if (state->system_reg.CNTV_CTL_EL0.IMASK == 0)
            {
                core->signal.num = 27;
                //general timer interrupt
                if (core->signal.intf)
                    core->signal.intf->raise_signal(core->signal.obj, core->signal.num);
            }
        }
    }
    //Physics Timer
    if (state->system_reg.CNTP_CTL_EL0.ENABLE)
    {
        /* When CNTP_CTL_EL0.ENABLE is 1, the timer condition is met when (CNTPCT_EL0 - CNTP_CVAL_EL0)
         * is greater than or equal to zero
         */
        if (state->system_reg.CNTPCT_EL0 == state->system_reg.CNTP_CVAL_EL0)
        {
            state->system_reg.CNTP_CTL_EL0.ISTATUS = 1;
            if (state->system_reg.CNTP_CTL_EL0.IMASK == 0)
            {
                //nCNTP NS IRQ Number
                core->signal.num = 30;
                //general timer interrupt
                if (core->signal.intf)
                    core->signal.intf->raise_signal(core->signal.obj, core->signal.num);
            }
        }
    }
}

uint32_t ftc662_run(conf_object_t * conf_obj)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;

    uint64_t steps = state->NumInstrs;

    if(!core->isWake)
    {
        return;
    }
    if (state->isAArch64)
    {
        SKY_pc_hap_occurred(NULL, conf_obj, state->A64_PC);
    }

    if (state->mode == DYNCOM_RUN || state->mode == HYBRID_RUN)
    {
        extern void arm_arch64_dyncom_step_once(arm_core_t * core);

        if (state->isAArch64)
        {
            // arm_arch64_dyncom_step_once(state);
        }
        else
        {
            // arm_dyncom_step_once(state);
        }
    } else if (state->mode == INTERPRET_RUN || state->mode == DYNCOM_RUN_SINGLE_STEP)
    {
        if (state->isAArch64)
            arm_arch64_step_once(state);
        else
            arm_step_once(state);
    } else
    {
        /* invalid mode */
        skyeye_log(Error_log, __FUNCTION__, "Unknown run mode:%d. Set mode to INTERPRET_RUN!\n", state->mode);
        state->mode = INTERPRET_RUN;
        return;
    }

    //breakpoint check
    generic_address_t pc = state->realPC;
    check_breakpoint(&(core->bp_manager), pc, state->NumInstrs);
    if (state->stop_step == 0 && state->mode == DYNCOM_RUN_SINGLE_STEP)
    {
        /* we reach the stop steps */
        state->mode = DYNCOM_RUN;
    }

    //check stepi
    if (state->stop_step != 0 && state->NumInstrs != 0 && state->stop_step == state->NumInstrs)
    {
        SIM_stop(NULL);
        state->stop_step = 0;
    }

    check_timer_interrupt(core);
    return state->NumInstrs - steps;
}

void ftc662_set_pc(conf_object_t * conf_obj, generic_address_t pc)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;

    arm_set_pc(core->state, pc);
}

void ftc662_stepi(conf_object_t * obj, uint64_t steps)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;

    core->state->stop_step = core->state->NumInstrs + steps;
    return;
}

uint64_t ftc662_get_steps(conf_object_t * obj)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;

    return core->state->NumInstrs;
}

void ftc662_stepi_check(conf_object_t * obj)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;
    ARMul_State *state = core->state;

    if (state->stop_step != 0 && state->NumInstrs != 0 && state->stop_step == state->NumInstrs)
    {
        SIM_stop(NULL);
        state->stop_step = 0;
    }

    return;
}

static int check_breakpoint_callback(conf_object_t * obj)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;
    ARMul_State *state = core->state;
    generic_address_t pc = arm_get_pc(state);

    check_breakpoint(&(core->bp_manager), pc, state->NumInstrs);
    return 0;
}

void SystemRegister_Init(ARMul_State * state)
{
    int i;

    state->PSTATE.D = 0;
    state->PSTATE.A = 0;
    state->PSTATE.I = 0;
    state->PSTATE.F = 0;
    state->PSTATE.SS = 0;
    state->PSTATE.IL = 0;
    state->PSTATE.EL = 3;

    //state->stack_address = 0xf0000000;

    for (i = 0; i < 31; i++)
    {
        state->R[i].X = 0;
    }

    state->NirqSig = HIGH;
    state->NfiqSig = HIGH;
    state->NresetSig = HIGH;
}

static conf_object_t *new_ftc662_core(char *obj_name)
{
    ftc662_core_t *core = skyeye_mm_zero(sizeof (ftc662_core_t));

    core->obj = new_conf_object(obj_name, core);
    ARMul_State *state = skyeye_mm_zero(sizeof (ARMul_State));

    ARMul_NewState(state);
    ARMul_SelectProcessor(state, ARM_v4_Prop | ARM_v5_Prop | ARM_v5e_Prop | ARM_v6_Prop | ARM_v7_Prop | ARM_v8_Prop);

    state->isAArch64 = 1;
    /* Init cpu register */
    SystemRegister_Init(state);
    state->NextInstr = 0;
    state->Emulate = 3;
    core->state = state;
    state->cpu_obj = core->obj;
    core->isWake = 1;
    //register_callback(ftc662_stepi_check, Step_callback);
    //register_callback(check_breakpoint_callback, Step_callback);

    return core->obj;
}

static cpu_config_t ftc662_cpu_info = { "armv8", "ftc662", 0x413Fc662, 0x0000fff0, NONCACHE };

static exception_t config_ftc662_core(conf_object_t * obj)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;
    ARMul_State *state = (ARMul_State *) core->state;

    if (!state->isAArch64)
    {
        ARMul_EmulateInit();
        state->cpu = &ftc662_cpu_info;
        ARMul_Reset(state);

        state->abort_model = 0;
        state->NextInstr = 0;
        state->Emulate = 3;

        state->Reg[0] = 0;
        state->Reg[1] = 0x2c;
        state->Reg[2] = 0x85000000;
        state->system_reg.MIDR_EL1 = 0x414fc090;
        state->system_reg.ID_MMFR0_EL1.all = 0x10033;

        //state->system_reg.TCR_EL1.T0SZ = 32;

        state->lateabtSig = LOW;
        state->bigendSig = LOW;
        addr_space_t *space = skyeye_mm_zero(sizeof (addr_space_t));

        state->space.conf_obj = core->obj;

        state->space.read = ftc662_memory_read;
        state->space.write = ftc662_memory_write;

        // arm_dyncom_init(state);
    } else
    {
        extern void arm_arch64_dyncom_init(arm_core_t * core);

        // arm_arch64_dyncom_init(state);
    }
    return No_exp;
}

static exception_t free_ftc662_core(conf_object_t * obj)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;

    skyeye_free(core->state);
    skyeye_free(obj->objname);
    skyeye_free(obj->obj);
    skyeye_free(obj);

    return No_exp;
}

static char *ftc662_disassemble(conf_object_t * obj, generic_address_t addr)
{
    return skyeye_disassemble(obj, addr);
}

char *ftc662_get_architecture(conf_object_t * conf_obj)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;

    if (core->state->isAArch64)
        return "aarch64";
    else
        return "arm";
}

static int ftc662_insert_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    int ret;

    ret = create_a_breakpoint(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

static int ftc662_delete_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    int ret;

    ret = remove_bp_by_addr(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

static int ftc662_get_bp_numbers(conf_object_t * conf_obj)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;

    return get_bp_numbers(&core->bp_manager);
}

static uint32_t ftc662_get_bp_addr_by_id(conf_object_t * conf_obj, int id)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);

    if (bp)
        return bp->address;
    return 0;
}

static int ftc662_get_trigger(conf_object_t * conf_obj)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;

    return get_breakpoint_hit_id(&core->bp_manager);
}

static bool_t ftc662_check_bp_trigger(conf_object_t * conf_obj)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    int ret;

    ret = check_breakpoint_hit_flag(&core->bp_manager);
    if (ret == 0)
        return True;
    return False;
}

static int ftc662_clear_bp_trigger(conf_object_t * conf_obj)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;

    clear_breakpoint_hit_flag(&core->bp_manager);
    return 0;
}

uint32_t ftc662_get_address_width(conf_object_t * obj)
{
    return 4;
}

int ftc662_get_cpu_exception_num(conf_object_t * obj)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;
    ARMul_State *state = core->state;

    return ARMErrorV / 4;
}

void ftc662_set_cpu_exception_by_id(conf_object_t * obj, int exception_id)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;
    ARMul_State *state = core->state;

    if (!state->isAArch64 && (exception_id < ARMErrorV / 4))
        ARMul_Abort(state, exception_id * 4);

    return;
}

extern const char *arm_exception_name[];
char *ftc662_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;
    ARMul_State *state = core->state;

    assert(exception_id < (ARMErrorV / 4));

    return arm_exception_name[exception_id];
}

static void ftc662_register_interface(conf_class_t * clss)
{
    static const core_exec_intf core_exec = {
        .run = ftc662_run,
        .stop = arm_stop,
        .stepi = ftc662_stepi,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const core_signal_intf core_signal = {
        .signal = ftc662_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    static const core_info_intf core_info = {
        .set_pc = ftc662_set_pc,
        .get_pc = ftc662_arm_get_pc,
        .get_steps = ftc662_get_steps,
        .get_endian = arm_get_endian,
        .get_alignment = arm_get_alignment,
        .reset = arm_reset,
        .disassemble = ftc662_disassemble,
        .get_architecture = ftc662_get_architecture,
        .get_address_width = ftc662_get_address_width,
        .get_cpu_exception_num = ftc662_get_cpu_exception_num,
        .set_cpu_exception_by_id = ftc662_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = ftc662_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

    static const skyeye_reg64_intf core_reg64 = {
        .get_regvalue_by_id = ftc662_get_regval_by_id,
        .get_regname_by_id = ftc662_get_regname_by_id,
        .set_regvalue_by_id = ftc662_set_regval_by_id,
        .get_regid_by_name = ftc662_get_regid_by_name,
        .get_regnum = ftc662_get_regnum
    };
    SKY_register_iface(clss, SKYEYE_REG64_INTF, &core_reg64);

    static const skyeye_reg_intf core_reg32 = {
        .get_regvalue_by_id = ftc662_get_regval_by_id,
        .get_regname_by_id = ftc662_get_regname_by_id,
        .set_regvalue_by_id = ftc662_set_regval_by_id,
        .get_regid_by_name = ftc662_get_regid_by_name,
        .get_regnum = ftc662_get_regnum
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &core_reg32);

    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = ftc662_insert_bp,
        .delete_bp = ftc662_delete_bp,
        .get_bp_numbers = ftc662_get_bp_numbers,
        .get_bp_addr_by_id = ftc662_get_bp_addr_by_id,
        .get_bp_trigger = ftc662_get_trigger,
        .check_bp_trigger = ftc662_check_bp_trigger,
        .clear_bp_trigger = ftc662_clear_bp_trigger,
    };
    SKY_register_iface(clss, CORE_BREAKPOINT_OP_INTF_NAME, &bp_op);

    static const memory_space_intf memory_space = {
        .read = ftc662_memory_read,
        .write = ftc662_memory_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    static const multcore_event_signal_intf event = {
        .set = ftc662_event_signal_set,
        .get = ftc662_event_signal_get
    };
    SKY_register_iface(clss, MULTCORE_EVENT_SIGNAL_INTF, &event);

    static const struct InterfaceDescription ifaces[] = {
        (struct InterfaceDescription) {
            .name = CORE_INFO_INTF_NAME,
            .iface = &core_info,
        },
        (struct InterfaceDescription) {
            .name = SKYEYE_REG64_INTF,
            .iface = &core_reg64,
        },
        (struct InterfaceDescription) {
            .name = SKYEYE_REG_INTF,
            .iface = &core_reg32,
        },
        (struct InterfaceDescription) {
            .name = CORE_EXEC_INTF_NAME,
            .iface = &core_exec,
        },
        (struct InterfaceDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &memory_space,
        },
        (struct InterfaceDescription) {
            .name = CORE_BREAKPOINT_OP_INTF_NAME,
            .iface = &bp_op,
        },
        (struct InterfaceDescription) {
            .name = CORE_SIGNAL_INTF_NAME,
            .iface = &core_signal,
        },
        (struct InterfaceDescription) {
            .name = MULTCORE_EVENT_SIGNAL_INTF,
            .iface = &event,
        },
    };

    class_register_ifaces(clss, ifaces);
    return;
}

static attr_value_t get_attr_load_mask(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->mask);

    return value;
}

static exception_t set_attr_load_mask(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;

    core->mask = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_load_addr(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->load_addr);

    return value;
}

static exception_t set_attr_load_addr(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;

    core->load_addr = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_mode(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->mode);

    return value;
}

static exception_t set_attr_mode(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;

    state->mode = core->mode = SKY_attr_uinteger(*value);

    return No_exp;
}

static attr_value_t get_gic_interface_base_addr(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    attr_value_t value = SKY_make_attr_uinteger(state->gic_interface_base_addr);

    return value;
}

static exception_t set_gic_interface_base_addr(void *arg, conf_object_t * conf_obj, attr_value_t * value,
                                               attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;

    state->gic_interface_base_addr = SKY_attr_uinteger(*value);

    return No_exp;
}

static attr_value_t get_gic_interface_length(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    attr_value_t value = SKY_make_attr_uinteger(state->gic_interface_length);

    return value;
}

static exception_t set_gic_interface_length(void *arg, conf_object_t * conf_obj, attr_value_t * value,
                                            attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;

    state->gic_interface_length = SKY_attr_uinteger(*value);

    return No_exp;
}

static attr_value_t get_attr_cpuID(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    attr_value_t value = SKY_make_attr_uinteger(state->cpuID);

    return value;
}

static exception_t set_attr_cpuID(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;

    state->cpuID = SKY_attr_uinteger(*value);
    state->system_reg.MPIDR_EL1 = state->cpuID;
    
    return No_exp;
}

static attr_value_t get_attr_isWake(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->isWake);
    
    return value;
}

static exception_t set_attr_isWake(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;

    core->isWake = SKY_attr_uinteger(*value);
    
    return No_exp;
}

static attr_value_t get_attr_setAArch32(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    attr_value_t value = SKY_make_attr_uinteger(state->isAArch64);

    return value;
}

static exception_t set_attr_setAArch32(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    ftc662_core_t *core = (ftc662_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    ARMword AArch32Flag = SKY_attr_uinteger(*value);

    state->isAArch64 = AArch32Flag ? 0 : 1;
    return No_exp;
}

static void ftc662_register_attribute(conf_class_t * clss)
{
    SKY_register_attribute(clss, "load_mask", get_attr_load_mask, NULL, set_attr_load_mask, NULL, SKY_Attr_Optional,
                           "uinteger", "load mask of processor core");
    SKY_register_attribute(clss, "load_addr", get_attr_load_addr, NULL, set_attr_load_addr, NULL, SKY_Attr_Optional,
                           "uinteger", "load addr space of processor core");
    SKY_register_attribute(clss, "mode", get_attr_mode, NULL, set_attr_mode, NULL, SKY_Attr_Optional, "uinteger",
                           "the different mode for cpu instruction");
    SKY_register_attribute(clss, "gic_interface_base_addr", get_gic_interface_base_addr, NULL,
                           set_gic_interface_base_addr, NULL, SKY_Attr_Optional, "uinteger",
                           "set gic interface base addr");
    SKY_register_attribute(clss, "gic_interface_length", get_gic_interface_length, NULL, set_gic_interface_length, NULL,
                           SKY_Attr_Optional, "uinteger", "set gic cpu interface offset length");
    SKY_register_attribute(clss, "cpuID", get_attr_cpuID, NULL, set_attr_cpuID, NULL, SKY_Attr_Optional, "uinteger",
                           "set current core cpuID");
    SKY_register_attribute(clss, "setAArch32", get_attr_setAArch32, NULL, set_attr_setAArch32, NULL, SKY_Attr_Optional,
                           "uinteger", "set current core aarch32 system");
    SKY_register_attribute(clss, "isWake", get_attr_isWake, NULL, set_attr_isWake, NULL, SKY_Attr_Optional,
                           "uinteger", "set current core wake flag");
}

static exception_t signal_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;

    core->signal.obj = obj2;
    core->signal.intf = (general_signal_intf *) SKY_get_iface(core->signal.obj, GENERAL_SIGNAL_INTF_NAME);
    if (core->signal.intf == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from %s fail.", obj2->objname);
    }

    return No_exp;
}

static exception_t signal_get(conf_object_t * obj, conf_object_t ** obj2, char **port, int index)
{
    ftc662_core_t *core = (ftc662_core_t *) obj->obj;

    *obj2 = core->signal.obj;
    *port = NULL;
    return No_exp;
}

void init_ftc662_core()
{
    static skyeye_class_t class_data = {
        .class_name = "ftc662_core",
        .class_desc = "ftc662_core",
        .new_instance = new_ftc662_core,
        .free_instance = free_ftc662_core,
        .config_instance = config_ftc662_core,
        .get_attr = NULL,
        .set_attr = NULL
    };
    conf_class_t *class = SKY_register_cpu_class(class_data.class_name, &class_data);

    ftc662_register_interface(class);

    static const struct ConnectDescription connects[] = {
        (struct ConnectDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = memory_space_set,
            .get = memory_space_get,
        },
        (struct ConnectDescription) {
            .name = GIC_INTERFACE_INTF,
            .set = gic_interface_set,
            .get = gic_interface_get,
        },
        (struct ConnectDescription) {
            .name = MULTCORE_EVENT_SIGNAL_INTF,
            .set = event_signal_interface_set,
            .get = event_signal_interface_get,
        },
        (struct ConnectDescription) {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set = signal_set,
            .get = signal_get,
        }
    };

    class_register_connects(class, connects);
    ftc662_register_attribute(class);
}

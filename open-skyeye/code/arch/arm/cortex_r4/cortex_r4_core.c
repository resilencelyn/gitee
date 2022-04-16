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
 * @file cortex_r4_core.c
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
//#include <dyncom/defines.h>
//#include "arm_dyncom_interface.h"

#include <skyeye_mm.h>
#include <skyeye_class.h>
#include <skyeye_core_intf.h>
#include <skyeye_iface.h>
#include <skyeye_attribute.h>
#include <memory_space.h>
#include <sim_control.h>
#include <skyeye_callback.h>
#include <breakpoint.h>
#include <skyeye_iface.h>
#include <skyeye_disassemble.h>

typedef struct cortex_r4_core
{
    conf_object_t *obj;
    ARMul_State *state;
    generic_address_t load_addr;
    uint32_t mask;
    breakpoint_mgt_t bp_manager;
    core_running_mode_t mode;
} cortex_r4_core_t;
void cortex_r4_stepi_check(conf_object_t * obj);

exception_t cortex_r4_memory_read(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    int i;

    for (i = 0; i < count; i++)
    {
        state->mmu.ops.read_byte(state, addr + i, buf + i);
    }
    return No_exp;
}

exception_t cortex_r4_memory_write(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    int i;

    for (i = 0; i < count; i++)
    {
        state->mmu.ops.write_byte(state, addr + i, *((char *) buf + i));
    }
    return No_exp;
}

static exception_t memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) obj->obj;
    ARMul_State *state = core->state;

    state->memory_space = obj2;
    state->memory_space_iface = (memory_space_intf *) SKY_get_iface(state->memory_space, MEMORY_SPACE_INTF_NAME);

    if (state->memory_space_iface == NULL)
        skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' interface from %s fail.", obj2->objname);
    return No_exp;
}

static exception_t memory_space_get(conf_object_t * obj, conf_object_t ** obj2, const char **port, int index)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) obj->obj;

    *obj2 = core->state->memory_space;
    *port = NULL;
    return No_exp;
}

static uint32 cortex_r4_arm_get_pc(conf_object_t * opaque)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) opaque->obj;
    uint32 pc = arm_get_pc(core->state);

    return pc;
}

static uint32 cortex_r4_get_regnum(conf_object_t * opaque)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_get_regnum(state);
}

static uint32_t cortex_r4_get_regid_by_name(conf_object_t * opaque, char *name)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_get_regid_by_name(state, name);
}

static char *cortex_r4_get_regname_by_id(conf_object_t * opaque, int id)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_get_regname_by_id(state, id);
}

static exception_t cortex_r4_set_regval_by_id(conf_object_t * opaque, uint32 value, int id)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    if (id == R13_IRQ)
        state->Reg_irq[0] = value;
    if (id == R14_IRQ)
        state->Reg_irq[1] = value;
    if (id == R13_SVC)
        state->Reg_svc[0] = value;
    if (id == R14_SVC)
        state->Reg_svc[1] = value;
    if (id == R13_ABORT)
        state->Reg_abort[0] = value;
    if (id == R14_ABORT)
        state->Reg_abort[1] = value;
    if (id == CP15_FAULT_STATUS)
        state->mmu.fault_status = value;
    if (id == CPSR_REG)
    {
        state->Cpsr = value;
        state->NFlag = (value >> 31) & 0x1;
        state->ZFlag = (value >> 30) & 0x1;
        state->CFlag = (value >> 29) & 0x1;
        state->VFlag = (value >> 28) & 0x1;
        state->TFlag = (value >> 5) & 0x1;
        switch (value & MODEBITS)
        {
            case 0x10:
                state->Mode = USER32MODE;
                break;
            case 0x12:
                state->Mode = IRQ32MODE;
                break;
            case 0x13:
                state->Mode = SVC32MODE;
                break;
        };
    } else
    {
        state->Reg[id] = value;
    }
    return No_exp;
}

static uint32 cortex_r4_get_regval_by_id(conf_object_t * opaque, int id)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    if (id == 0xFF)
        return state->CurrInstr;
    if (id == R13_IRQ)
        return state->Reg_irq[0];
    if (id == R14_IRQ)
        return state->Reg_irq[1];
    if (id == R13_SVC)
        return state->Reg_svc[0];
    if (id == R14_SVC)
        return state->Reg_svc[1];
    if (id == R13_ABORT)
        return state->Reg_abort[0];
    if (id == R14_ABORT)
        return state->Reg_abort[1];
    if (id == CP15_FAULT_ADDRESS)
        return state->mmu.fault_address;
    if (id == CP15_FAULT_STATUS)
        return state->mmu.fault_status;
    if (id == CPSR_REG)
    {
        ARMword cpsr = state->Cpsr & 0x0FFFFFDF;

        cpsr |= (state->NFlag & 0x1) << 31;
        cpsr |= (state->ZFlag & 0x1) << 30;
        cpsr |= (state->CFlag & 0x1) << 29;
        cpsr |= (state->VFlag & 0x1) << 28;
        cpsr |= (state->TFlag & 0x1) << 5;
        return cpsr;
    } else
    {
        if (id != 15)
            return state->Reg[id];
        else
            return state->realPC;
    }
}

static exception_t cortex_r4_signal(conf_object_t * opaque, interrupt_signal_t * signal)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    arm_signal_t *arm_signal = &signal->arm_signal;

    if (arm_signal->irq != Prev_level)
        state->NirqSig = arm_signal->irq;
    if (arm_signal->firq != Prev_level)
        state->NfiqSig = arm_signal->firq;

    /* reset signal in arm dyf add when move sa1100 to soc dir  2010.9.21 */
    if (arm_signal->reset != Prev_level)
        state->NresetSig = arm_signal->reset;
    return No_exp;
}

uint32_t cortex_r4_run(conf_object_t * conf_obj)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    uint32_t steps = state->NumInstrs;

    if (state->mode == DYNCOM_RUN || state->mode == HYBRID_RUN)
    {
        // arm_dyncom_step_once(state);
    } else if (state->mode == INTERPRET_RUN || state->mode == DYNCOM_RUN_SINGLE_STEP)
    {
        arm_step_once(state);
    } else
    {
        /* invalid mode */
        return;
    }
    if (state->stop_step == 0 && state->mode == DYNCOM_RUN_SINGLE_STEP)
    {
        /* we reach the stop steps */
        state->mode = DYNCOM_RUN;
    }

    //breakpoint check
    generic_address_t pc = state->realPC;
    check_breakpoint(&(core->bp_manager), pc, state->NumInstrs);

    //check stepi
    if (state->stop_step != 0 && state->NumInstrs != 0 && state->stop_step == state->NumInstrs)
    {
        SIM_stop(NULL);
        state->stop_step = 0;
    }

    return state->NumInstrs - steps;
}

void cortex_r4_set_pc(conf_object_t * conf_obj, generic_address_t pc)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;

    arm_set_pc(core->state, pc);
}

void cortex_r4_stepi(conf_object_t * obj, uint64_t steps)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) obj->obj;

    core->state->stop_step = core->state->NumInstrs + steps;
    return;
}

uint64_t cortex_r4_get_steps(conf_object_t * obj)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) obj->obj;

    return core->state->NumInstrs;
}

endian_t cortex_r4_get_endian(conf_object_t * conf_obj)
{
    return Little_endian;
}

void cortex_r4_stepi_check(conf_object_t * obj)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) obj->obj;
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
    cortex_r4_core_t *core = (cortex_r4_core_t *) obj->obj;
    ARMul_State *state = core->state;
    generic_address_t pc = arm_get_pc(state);

    check_breakpoint(&(core->bp_manager), pc - 8, state->NumInstrs);
    return 0;
}

static cpu_config_t cortex_r4_cpu_info = { "armv7", "cortex_r4", 0x413fc140, 0x0000fff0, NONCACHE };

static conf_object_t *new_cortex_r4_core(char *obj_name)
{
    cortex_r4_core_t *core = skyeye_mm_zero(sizeof (cortex_r4_core_t));

    core->obj = new_conf_object(obj_name, core);
    ARMul_EmulateInit();
    ARMul_State *state = skyeye_mm_zero(sizeof (ARMul_State));

    ARMul_NewState(state);
    state->abort_model = 0;
    state->cpu = &cortex_r4_cpu_info;
    state->bigendSig = LOW;

    ARMul_SelectProcessor(state, ARM_v4_Prop | ARM_v5_Prop | ARM_v5e_Prop | ARM_v6_Prop | ARM_v7_Prop);

    state->lateabtSig = LOW;
    mmu_init(state);
    /* reset the core to initial state */
    ARMul_Reset(state);
    state->NextInstr = 0;
    state->Emulate = 3;
    core->state = state;
    state->cpu_obj = core->obj;

#if FAST_MEMORY
    mmap_ram(0xc0000000, 0x10000000);
#endif
    // arm_dyncom_init(state);
    VFPInit(core);

    //register_callback(cortex_r4_stepi_check, Step_callback);
    //register_callback(check_breakpoint_callback, Step_callback);

    return core->obj;
}

static exception_t free_cortex_r4_core(conf_object_t * obj)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) obj->obj;

    ARMul_FreeState(core->state);
    skyeye_free(core->state);
    skyeye_free(obj->objname);
    skyeye_free(obj->obj);
    skyeye_free(obj);

    return No_exp;
}

static char *cortex_r4_disassemble(conf_object_t * obj, generic_address_t addr)
{
    return skyeye_disassemble(obj, addr);
}

char *cortex_r4_get_architecture(conf_object_t * conf_obj)
{
    return "arm";
}

static int cortex_r4_insert_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    int ret;

    ret = create_a_breakpoint(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

static int cortex_r4_delete_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    int ret;

    ret = remove_bp_by_addr(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

static int cortex_r4_get_bp_numbers(conf_object_t * conf_obj)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;

    return get_bp_numbers(&core->bp_manager);
}

static uint32_t cortex_r4_get_bp_addr_by_id(conf_object_t * conf_obj, int id)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);

    if (bp)
        return bp->address;
    return 0;
}

static int cortex_r4_get_trigger(conf_object_t * conf_obj)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;

    return get_breakpoint_hit_id(&core->bp_manager);
}

static bool_t cortex_r4_check_bp_trigger(conf_object_t * conf_obj)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    int ret;

    ret = check_breakpoint_hit_flag(&core->bp_manager);
    if (ret == 0)
        return True;
    return False;
}

static int cortex_r4_clear_bp_trigger(conf_object_t * conf_obj)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;

    clear_breakpoint_hit_flag(&core->bp_manager);
    return 0;
}

uint32_t cortex_r4_get_address_width(conf_object_t * obj)
{
    return 4;
}

int cortex_r4_get_cpu_exception_num(conf_object_t * obj)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) obj->obj;
    ARMul_State *state = core->state;

    return ARMErrorV / 4;
}

void cortex_r4_set_cpu_exception_by_id(conf_object_t * obj, int exception_id)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) obj->obj;
    ARMul_State *state = core->state;

    if (exception_id < ARMErrorV / 4)
        ARMul_Abort(state, exception_id * 4);

    return;
}

extern const char *arm_exception_name[];
char *cortex_r4_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) obj->obj;
    ARMul_State *state = core->state;

    assert(exception_id < (ARMErrorV / 4));

    return arm_exception_name[exception_id];
}

static void cortex_r4_register_interface(conf_class_t * clss)
{
    static const core_exec_intf core_exec = {
        .run = cortex_r4_run,
        .stop = arm_stop,
        .stepi = cortex_r4_stepi,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const core_signal_intf core_signal = {
        .signal = cortex_r4_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    static const core_info_intf core_info = {
        .set_pc = cortex_r4_set_pc,
        .get_pc = cortex_r4_arm_get_pc,
        .get_steps = cortex_r4_get_steps,
        .get_endian = cortex_r4_get_endian,
        .get_alignment = arm_get_alignment,
        .reset = arm_reset,
        .disassemble = cortex_r4_disassemble,
        .get_architecture = cortex_r4_get_architecture,
        .get_address_width = cortex_r4_get_address_width,
        .get_cpu_exception_num = cortex_r4_get_cpu_exception_num,
        .set_cpu_exception_by_id = cortex_r4_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = cortex_r4_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

    static const skyeye_reg_intf core_reg = {
        .get_regvalue_by_id = cortex_r4_get_regval_by_id,
        .get_regname_by_id = cortex_r4_get_regname_by_id,
        .set_regvalue_by_id = cortex_r4_set_regval_by_id,
        .get_regid_by_name = cortex_r4_get_regid_by_name,
        .get_regnum = cortex_r4_get_regnum
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &core_reg);
    static const memory_space_intf memory_space = {
        .read = cortex_r4_memory_read,
        .write = cortex_r4_memory_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = cortex_r4_insert_bp,
        .delete_bp = cortex_r4_delete_bp,
        .get_bp_numbers = cortex_r4_get_bp_numbers,
        .get_bp_addr_by_id = cortex_r4_get_bp_addr_by_id,
        .get_bp_trigger = cortex_r4_get_trigger,
        .check_bp_trigger = cortex_r4_check_bp_trigger,
        .clear_bp_trigger = cortex_r4_clear_bp_trigger,
    };
    SKY_register_iface(clss, CORE_BREAKPOINT_OP_INTF_NAME, &bp_op);

    static const struct InterfaceDescription ifaces[] = {
        (struct InterfaceDescription) {
            .name = CORE_INFO_INTF_NAME,
            .iface = &core_info,
        },
        (struct InterfaceDescription) {
            .name = SKYEYE_REG_INTF,
            .iface = &core_reg,
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
        }
    };
    class_register_ifaces(clss, ifaces);
    return;
}

#define REG_NUM 16
static attr_value_t get_attr_all_registers(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    attr_value_t value = SKY_alloc_attr_list(REG_NUM);
    attr_value_t item;
    uint32_t i;

    for (i = 0; i < REG_NUM; i++)
    {
        uint32_t data = core->state->Reg[i];

        item = SKY_make_attr_uinteger(data);
        SKY_attr_list_set_item(&value, i, item);
    }
    return value;
}
static exception_t set_attr_all_registers(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    uint32_t i = 0, data = 0;
    attr_value_t item;

    for (i = 0; i < REG_NUM; i++)
    {
        item = SKY_attr_list_item(*value, i);
        data = SKY_attr_uinteger(item);
        core->state->Reg[i] = data;
    }
    return No_exp;
}
static attr_value_t get_attr_cpsr(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->state->Cpsr);

    return value;
}
static exception_t set_attr_cpsr(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;

    core->state->Cpsr = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_Mode(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->state->Mode);

    return value;
}

static exception_t set_attr_Mode(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;

    core->state->Mode = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_load_mask(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->mask);

    return value;
}

static exception_t set_attr_load_mask(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;

    core->mask = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_load_addr(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->load_addr);

    return value;
}

static exception_t set_attr_load_addr(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;

    core->load_addr = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_mode(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->mode);

    return value;
}

static exception_t set_attr_mode(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_r4_core_t *core = (cortex_r4_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;

    state->mode = core->mode = SKY_attr_uinteger(*value);

    return No_exp;
}

static void cortex_r4_register_attribute(conf_class_t * clss)
{
    SKY_register_attribute(clss, "load_mask", get_attr_load_mask, NULL, set_attr_load_mask, NULL, SKY_Attr_Optional,
                           "uinteger", "load mask of processor core");
    SKY_register_attribute(clss, "load_addr", get_attr_load_addr, NULL, set_attr_load_addr, NULL, SKY_Attr_Optional,
                           "uinteger", "load addr space of processor core");
    SKY_register_attribute(clss, "Reg", get_attr_all_registers, NULL, set_attr_all_registers, NULL, SKY_Attr_Optional,
                           "uinteger", "all registers of processor core");
    SKY_register_attribute(clss, "Cpsr", get_attr_cpsr, NULL, set_attr_cpsr, NULL, SKY_Attr_Optional, "uinteger",
                           "Cpsr register of processor core");
    SKY_register_attribute(clss, "work_mode", get_attr_Mode, NULL, set_attr_Mode, NULL, SKY_Attr_Optional, "uinteger",
                           "work mode of processor core");
    SKY_register_attribute(clss, "mode", get_attr_mode, NULL, set_attr_mode, NULL, SKY_Attr_Optional, "uinteger",
                           "the different mode for cpu instruction");
}

void init_cortex_r4_core()
{
    static skyeye_class_t class_data = {
        .class_name = "cortex_r4_core",
        .class_desc = "cortex_r4_core",
        .new_instance = new_cortex_r4_core,
        .free_instance = free_cortex_r4_core,
        .get_attr = NULL,
        .set_attr = NULL
    };
    conf_class_t *class = SKY_register_cpu_class(class_data.class_name, &class_data);

    cortex_r4_register_attribute(class);
    cortex_r4_register_interface(class);

    static const struct ConnectDescription connects[] = {
        (struct ConnectDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = memory_space_set,
            .get = memory_space_get,
        }
    };

    class_register_connects(class, connects);
}

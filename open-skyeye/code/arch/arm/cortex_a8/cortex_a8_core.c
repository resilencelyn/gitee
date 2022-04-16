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
* @file cortex_a8_core.c
* @brief The arm core class
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-21
*/

#include "armdefs.h"
#include "armcpu.h"
#include "armemu.h"
#include "arm_regformat.h"

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

#define CP15(idx)       (idx - CP15_BASE)
typedef struct cortex_a8_core
{
    conf_object_t *obj;
    ARMul_State *state;
    generic_address_t load_addr;
    uint32_t mask;
    memory_space_intf *space;
    conf_object_t *memory_space;
    breakpoint_mgt_t bp_manager;
} cortex_a8_core_t;

exception_t cortex_a8_memory_read(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
    memory_space_intf *iface = core->space;

    return iface->read(core->memory_space, addr, buf, count);
}

exception_t cortex_a8_memory_write(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
    memory_space_intf *iface = core->space;

    return iface->write(core->memory_space, addr, buf, count);
}

static exception_t memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) obj->obj;

    core->memory_space = obj2;
    core->space = (memory_space_intf *) SKY_get_iface(core->memory_space, MEMORY_SPACE_INTF_NAME);
    if (core->space == NULL)
        skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' interface from %s fail.", obj2->objname);
    return No_exp;
}

static exception_t memory_space_get(conf_object_t * obj, conf_object_t ** obj2, const char **port, int index)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) obj->obj;

    *obj2 = core->memory_space;
    *port = NULL;
    return No_exp;
}

static uint32 cortex_a8_arm_get_pc(conf_object_t * opaque)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) opaque->obj;
    uint32 pc = arm_get_pc(core->state);

    return pc;
}

static uint32 cortex_a8_get_regnum(conf_object_t * opaque)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_get_regnum(state);
}

static char *cortex_a8_disassemble(conf_object_t * obj, generic_address_t addr)
{
    return skyeye_disassemble(obj, addr);
}

static uint32_t cortex_a8_get_regid_by_name(conf_object_t * opaque, char *name)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_get_regid_by_name(state, name);
}

static char *cortex_a8_get_regname_by_id(conf_object_t * opaque, int id)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_get_regname_by_id(state, id);
}

static exception_t cortex_a8_set_regval_by_id(conf_object_t * opaque, uint32 value, int id)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_set_regvalue_by_id(state, value, id);
}

static uint32 cortex_a8_get_regval_by_id(conf_object_t * opaque, int id)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_get_regvalue_by_id(state, id);
}

static exception_t cortex_a8_signal(conf_object_t * opaque, interrupt_signal_t * signal)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) opaque->obj;
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

uint32_t cortex_a8_run(conf_object_t * conf_obj)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;

    uint32_t steps = state->NumInstrs;

    arm_step_once(core->state);

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

void cortex_a8_set_pc(conf_object_t * conf_obj, generic_address_t pc)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;

    arm_set_pc(core->state, pc);
}

void cortex_a8_stepi(conf_object_t * obj, uint64_t steps)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) obj->obj;

    core->state->stop_step = core->state->NumInstrs + steps;
    return;
}

uint64_t cortex_a8_get_steps(conf_object_t * obj)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) obj->obj;

    return core->state->NumInstrs;
}

void cortex_a8_stepi_check(conf_object_t * obj)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) obj->obj;
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
    cortex_a8_core_t *core = (cortex_a8_core_t *) obj->obj;
    ARMul_State *state = core->state;
    generic_address_t pc = arm_get_pc(state);

    check_breakpoint(&(core->bp_manager), pc - 8, state->NumInstrs);
    return 0;
}

static cpu_config_t cortex_a8_cpu_info = { "armv7", "cortex_a8", 0x413Fc082, 0x0000fff0, NONCACHE };
static conf_object_t *new_cortex_a8_core(char *obj_name)
{
    cortex_a8_core_t *core = skyeye_mm_zero(sizeof (cortex_a8_core_t));

    core->obj = new_conf_object(obj_name, core);
    ARMul_EmulateInit();
    ARMul_State *state = skyeye_mm_zero(sizeof (ARMul_State));

    ARMul_NewState(state);
    state->abort_model = 0;
    state->cpu = &cortex_a8_cpu_info;
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
    state->Reg[0] = 0;
    state->Reg[1] = 0x2c;
    state->Reg[2] = 0x85000000;
    addr_space_t *space = skyeye_mm_zero(sizeof (addr_space_t));

    initialize_addr_space(space);

    core->space = skyeye_mm_zero(sizeof (memory_space_intf));
    core->space->read = space->memory_space->read;
    core->space->write = space->memory_space->write;
    state->space.conf_obj = core->obj;

    state->space.read = cortex_a8_memory_read;
    state->space.write = cortex_a8_memory_write;

    //register_callback(cortex_a8_stepi_check, Step_callback);
    //register_callback(check_breakpoint_callback, Step_callback);

    skyeye_free(space->memory_space);
    skyeye_free(space);

    return core->obj;
}

static exception_t free_cortex_a8_core(conf_object_t * obj)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) obj->obj;

    ARMul_FreeState(core->state);
    skyeye_free(core->state);
    skyeye_free(obj->objname);
    skyeye_free(obj->obj);
    skyeye_free(obj);

    return No_exp;
}

static int cortex_a8_insert_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
    int ret;

    ret = create_a_breakpoint(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

static int cortex_a8_delete_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
    int ret;

    ret = remove_bp_by_addr(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

static int cortex_a8_get_bp_numbers(conf_object_t * conf_obj)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;

    return get_bp_numbers(&core->bp_manager);
}

static uint32_t cortex_a8_get_bp_addr_by_id(conf_object_t * conf_obj, int id)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
    breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);

    if (bp)
        return bp->address;
    return 0;
}

static int cortex_a8_get_trigger(conf_object_t * conf_obj)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;

    return get_breakpoint_hit_id(&core->bp_manager);
}

static bool_t cortex_a8_check_bp_trigger(conf_object_t * conf_obj)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
    int ret;

    ret = check_breakpoint_hit_flag(&core->bp_manager);
    if (ret == 0)
        return True;
    return False;
}

static int cortex_a8_clear_bp_trigger(conf_object_t * conf_obj)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;

    clear_breakpoint_hit_flag(&core->bp_manager);
    return 0;
}

uint32_t cortex_a8_get_address_width(conf_object_t * obj)
{
    return 4;
}

int cortex_a8_get_cpu_exception_num(conf_object_t * obj)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) obj->obj;
    ARMul_State *state = core->state;

    return ARMErrorV / 4;
}

void cortex_a8_set_cpu_exception_by_id(conf_object_t * obj, int exception_id)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) obj->obj;
    ARMul_State *state = core->state;

    if (exception_id < ARMErrorV / 4)
        ARMul_Abort(state, exception_id * 4);

    return;
}

extern const char *arm_exception_name[];
char *cortex_a8_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) obj->obj;
    ARMul_State *state = core->state;

    assert(exception_id < (ARMErrorV / 4));

    return arm_exception_name[exception_id];
}

static void cortex_a8_register_interface(conf_class_t * clss)
{
    static const core_exec_intf core_exec = {
        .run = cortex_a8_run,
        .stop = arm_stop,
        .stepi = cortex_a8_stepi,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const core_signal_intf core_signal = {
        .signal = cortex_a8_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    static const core_info_intf core_info = {
        .set_pc = cortex_a8_set_pc,
        .get_pc = cortex_a8_arm_get_pc,
        .get_steps = cortex_a8_get_steps,
        .get_endian = arm_get_endian,
        .get_alignment = arm_get_alignment,
        .get_architecture = arm_get_architecture,
        .disassemble = cortex_a8_disassemble,
        .reset = arm_reset,
        .get_address_width = cortex_a8_get_address_width,
        .get_cpu_exception_num = cortex_a8_get_cpu_exception_num,
        .set_cpu_exception_by_id = cortex_a8_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = cortex_a8_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

    static const skyeye_reg_intf core_reg = {
        .get_regvalue_by_id = cortex_a8_get_regval_by_id,
        .get_regname_by_id = cortex_a8_get_regname_by_id,
        .set_regvalue_by_id = cortex_a8_set_regval_by_id,
        .get_regid_by_name = cortex_a8_get_regid_by_name,
        .get_regnum = cortex_a8_get_regnum
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &core_reg);
    static const memory_space_intf memory_space = {
        .read = cortex_a8_memory_read,
        .write = cortex_a8_memory_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = cortex_a8_insert_bp,
        .delete_bp = cortex_a8_delete_bp,
        .get_bp_numbers = cortex_a8_get_bp_numbers,
        .get_bp_addr_by_id = cortex_a8_get_bp_addr_by_id,
        .get_bp_trigger = cortex_a8_get_trigger,
        .check_bp_trigger = cortex_a8_check_bp_trigger,
        .clear_bp_trigger = cortex_a8_clear_bp_trigger,
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

static attr_value_t get_attr_load_mask(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->mask);

    return value;
}

static exception_t set_attr_load_mask(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;

    core->mask = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_load_addr(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->load_addr);

    return value;
}

static exception_t set_attr_load_addr(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;

    core->load_addr = SKY_attr_uinteger(*value);
    return No_exp;
}

#define REG_NUM 16
static attr_value_t get_attr_all_registers(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
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
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
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
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->state->Cpsr);

    return value;
}

static exception_t set_attr_cpsr(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;

    core->state->Cpsr = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_NumInstrs(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->state->NumInstrs);

    return value;
}

static exception_t set_attr_NumInstrs(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;

    core->state->NumInstrs = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_Mode(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->state->Mode);

    return value;
}

static exception_t set_attr_Mode(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_a8_core_t *core = (cortex_a8_core_t *) conf_obj->obj;

    core->state->Mode = SKY_attr_uinteger(*value);
    return No_exp;
}

static void cortex_a8_register_attribute(conf_class_t * clss)
{
    SKY_register_attribute(clss, "load_mask", get_attr_load_mask, NULL, set_attr_load_mask, NULL, SKY_Attr_Optional,
                           "uinteger", "load mask of processor core");
    SKY_register_attribute(clss, "load_addr", get_attr_load_addr, NULL, set_attr_load_addr, NULL, SKY_Attr_Optional,
                           "uinteger", "load addr space of processor core");
    SKY_register_attribute(clss, "Reg", get_attr_all_registers, NULL, set_attr_all_registers, NULL, SKY_Attr_Optional,
                           "uinteger", "all registers of processor core");
    SKY_register_attribute(clss, "Cpsr", get_attr_cpsr, NULL, set_attr_cpsr, NULL, SKY_Attr_Optional, "uinteger",
                           "Cpsr register of processor core");
    SKY_register_attribute(clss, "NumInstrs", get_attr_NumInstrs, NULL, set_attr_NumInstrs, NULL, SKY_Attr_Optional,
                           "uinteger", "NumInstrs of processor core");
    SKY_register_attribute(clss, "work_mode", get_attr_Mode, NULL, set_attr_Mode, NULL, SKY_Attr_Optional, "uinteger",
                           "work mode of processor core");
}

void init_cortex_a8_core()
{
    static skyeye_class_t class_data = {
        .class_name = "cortex_a8_core",
        .class_desc = "cortex_a8_core",
        .new_instance = new_cortex_a8_core,
        .free_instance = free_cortex_a8_core,
        .get_attr = NULL,
        .set_attr = NULL
    };
    conf_class_t *class = SKY_register_cpu_class(class_data.class_name, &class_data);

    cortex_a8_register_attribute(class);
    cortex_a8_register_interface(class);

    static const struct ConnectDescription connects[] = {
        (struct ConnectDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = memory_space_set,
            .get = memory_space_get,
        }
    };

    class_register_connects(class, connects);
}

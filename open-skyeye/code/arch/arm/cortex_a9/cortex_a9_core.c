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
* @file cortex_a9_core.c
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
typedef struct cortex_a9_core
{
    conf_object_t *obj;
    ARMul_State *state;
    memory_space_intf *space;
    conf_object_t *memory_space;
    breakpoint_mgt_t bp_manager;
    uint32_t freq_hz;
} cortex_a9_core_t;

#if 0
static void cortex_a9_set_pc(conf_object_t * opaque, generic_address_t pc)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    state->Reg[15] = pc;
    return;
}
#endif

static void cortex_a9_step_once(conf_object_t * opaque)
{
    //ARMul_DoInstr(state);
    cortex_a9_core_t *core = (cortex_a9_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    if (state->space.conf_obj == NULL)
    {
        state->space.conf_obj = core->space->conf_obj;
        state->space.read = core->space->read;
        state->space.write = core->space->write;
    }

    state->step++;
    state->cycle++;
    state->EndCondition = 0;
    state->stop_simulator = 0;
    state->NextInstr = RESUME;          /* treat as PC change */
    state->last_pc = state->Reg[15];
    //core->space->read(core->space->conf_obj, state->Reg[15], state->last_instr, 4);
    //state->Reg[15] = ARMul_DoProg(state);
    state->Reg[15] = ARMul_DoInstr(state);
    //core->space->read(core->space->conf_obj, state->Reg[15], state->CurrInstr, 4);
    state->Cpsr = (state->Cpsr & 0x0fffffdf) |
        (state->NFlag << 31) | (state->ZFlag << 30) | (state->CFlag << 29) | (state->VFlag << 28) | (state->TFlag << 5);

    FLUSHPIPE;
}

static uint32 cortex_a9_arm_get_pc(conf_object_t * opaque)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) opaque->obj;
    uint32 pc = arm_get_pc(core->state);

    return pc;
}

static uint32 cortex_a9_get_regnum(conf_object_t * opaque)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_get_regnum(state);
}

static exception_t cortex_a9_get_regid_by_name(conf_object_t * opaque, char *name)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_get_regid_by_name(state, name);
}

static char *cortex_a9_get_regname_by_id(conf_object_t * opaque, int id)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_get_regname_by_id(state, id);
}

static exception_t cortex_a9_set_regval_by_id(conf_object_t * opaque, uint32 value, int id)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_set_regvalue_by_id(state, value, id);
}

/**
* @brief
*
* @param id
*
* @return
*/
static uint32 cortex_a9_get_regval_by_id(conf_object_t * opaque, int id)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_get_regvalue_by_id(state, id);
}

fault_t cortex_a9_read_byte(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    return NO_FAULT;
}

fault_t cortex_a9_read_halfword(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    return NO_FAULT;
}

fault_t cortex_a9_read_word(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    return NO_FAULT;
}

static fault_t cortex_a9_write_byte(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return NO_FAULT;
}

static fault_t cortex_a9_write_halfword(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return NO_FAULT;
}

static fault_t cortex_a9_write_word(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return NO_FAULT;
}

static exception_t cortex_a9_signal(conf_object_t * opaque, interrupt_signal_t * signal)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) opaque->obj;
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

uint32_t cortex_a9_run(conf_object_t * conf_obj)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    uint32_t steps = state->NumInstrs;

    arm_step_once(state);

    //breakpoint check
    generic_address_t pc = state->realPC;
    check_breakpoint(&(state->bp_manager), pc, state->NumInstrs);

    //check stepi
    if (state->stop_step != 0 && state->NumInstrs != 0 && state->stop_step == state->NumInstrs)
    {
        SIM_stop(NULL);
        state->stop_step = 0;
    }

    return state->NumInstrs - steps;
}

void cortex_a9_set_pc(conf_object_t * conf_obj, generic_address_t pc)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;

    arm_set_pc(core->state, pc);
}

void cortex_a9_stepi(conf_object_t * obj, uint64_t steps)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) obj->obj;

    core->state->stop_step = core->state->NumInstrs + steps;
    return;
}

uint64_t cortex_a9_get_steps(conf_object_t * obj)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) obj->obj;

    return core->state->NumInstrs;
}

static int cortex_a9_insert_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;
    int ret;

    ret = create_a_breakpoint(&(core->state->bp_manager), addr);
    if (ret != 0)
        return ret;
    return 0;
}
static int cortex_a9_delete_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;
    int ret;

    ret = remove_bp_by_addr(&(core->state->bp_manager), addr);
    if (ret != 0)
        return ret;
    return 0;
}
static int cortex_a9_get_bp_numbers(conf_object_t * conf_obj)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;

    return get_bp_numbers(&(core->state->bp_manager));
}

static uint32_t cortex_a9_get_bp_addr_by_id(conf_object_t * conf_obj, int id)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;
    breakpoint_t *bp = get_bp_by_id(&(core->state->bp_manager), id);

    if (bp)
        return bp->address;
    return 0;
}

static int cortex_a9_get_trigger(conf_object_t * conf_obj)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;

    return get_breakpoint_hit_id(&(core->state->bp_manager));
}

static bool_t cortex_a9_check_bp_trigger(conf_object_t * conf_obj)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;
    int ret;

    ret = check_breakpoint_hit_flag(&(core->state->bp_manager));
    if (ret == 0)
        return True;
    return False;
}

static int cortex_a9_clear_bp_trigger(conf_object_t * conf_obj)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;

    clear_breakpoint_hit_flag(&(core->state->bp_manager));
    return 0;
}

exception_t cortex_a9_memory_read(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;
    memory_space_intf *iface = core->space;

    return iface->read(core->memory_space, addr, buf, count);
}

exception_t cortex_a9_memory_write(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;
    memory_space_intf *iface = core->space;

    return iface->write(core->memory_space, addr, buf, count);
}

/**
* @brief The initialization for arm core instance
*
* @return
*/
const static cpu_config_t cortex_a9_cpu_info = { "armv7", "cortex_a9", 0x413Fc090, 0x0000fff0, NONCACHE };
static conf_object_t *new_cortex_a9_core(char *obj_name)
{
    cortex_a9_core_t *core = skyeye_mm_zero(sizeof (cortex_a9_core_t));

    core->obj = new_conf_object(obj_name, core);
    ARMul_EmulateInit();
    ARMul_State *state = skyeye_mm_zero(sizeof (ARMul_State));

    ARMul_NewState(state);
    state->abort_model = 0;
    state->cpu = &cortex_a9_cpu_info;
    state->bigendSig = LOW;

    ARMul_SelectProcessor(state, ARM_v4_Prop | ARM_v5_Prop | ARM_v5e_Prop | ARM_v6_Prop | ARM_v7_Prop);

    //ARMul_SelectProcessor (state, ARM_v6_Prop);
    state->lateabtSig = LOW;
    mmu_init(state);
    /* reset the core to initial state */
    ARMul_Reset(state);
    state->NextInstr = 0;
    state->Emulate = 3;
    core->state = state;
    state->cpu_obj = core->obj;
    //addr_space_t* space = new_addr_space("cortex_a9_space");
    addr_space_t *space = skyeye_mm_zero(sizeof (addr_space_t));

    //init_addr_space(obj_name, space);
    initialize_addr_space(space);
    //SKY_register_interface(space, obj_name, ADDR_SPACE_INTF_NAME);
    core->space = skyeye_mm_zero(sizeof (memory_space_intf));
    state->space.conf_obj = core->obj;
    state->space.read = space->memory_space->read;
    state->space.write = space->memory_space->write;

    state->space.read = cortex_a9_memory_read;
    state->space.write = cortex_a9_memory_write;

    init_breakpoint_manager(&(state->bp_manager));

    return core->obj;
}

static void free_cortex_a9_core(conf_object_t * dev)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) dev->obj;

    ARMul_FreeState(core->state);
    skyeye_free(core->state);
    skyeye_free(dev->objname);
    skyeye_free(dev->obj);
    skyeye_free(dev);

    return No_exp;
}

char *cortex_a9_get_architecture(conf_object_t * conf_obj)
{
    return "arm";
}

static char *cortex_a9_disassemble(conf_object_t * obj, generic_address_t addr)
{
    return skyeye_disassemble(obj, addr);
}

uint32_t cortex_a9_get_address_width(conf_object_t * obj)
{
    return 4;
}

int cortex_a9_get_cpu_exception_num(conf_object_t * obj)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) obj->obj;
    ARMul_State *state = core->state;

    return ARMErrorV / 4;
}

void cortex_a9_set_cpu_exception_by_id(conf_object_t * obj, int exception_id)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) obj->obj;
    ARMul_State *state = core->state;

    if (exception_id < ARMErrorV / 4)
        ARMul_Abort(state, exception_id * 4);

    return;
}

extern const char *arm_exception_name[];
char *cortex_a9_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) obj->obj;
    ARMul_State *state = core->state;

    assert(exception_id < (ARMErrorV / 4));

    return arm_exception_name[exception_id];
}

static void cortex_a9_register_interface(conf_class_t * clss)
{
    static const core_exec_intf core_exec = {
        .run = cortex_a9_run,
        .stop = arm_stop,
        .stepi = cortex_a9_stepi,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const core_signal_intf core_signal = {
        .signal = cortex_a9_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    static const core_info_intf core_info = {
        .set_pc = cortex_a9_set_pc,
        .get_pc = cortex_a9_arm_get_pc,
        .get_steps = cortex_a9_get_steps,
        .get_endian = arm_get_endian,
        .get_alignment = arm_get_alignment,
        .reset = arm_reset,
        .get_architecture = cortex_a9_get_architecture,
        .disassemble = cortex_a9_disassemble,
        .get_address_width = cortex_a9_get_address_width,
        .get_cpu_exception_num = cortex_a9_get_cpu_exception_num,
        .set_cpu_exception_by_id = cortex_a9_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = cortex_a9_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

    static const skyeye_reg_intf core_reg = {
        .get_regvalue_by_id = cortex_a9_get_regval_by_id,
        .get_regname_by_id = cortex_a9_get_regname_by_id,
        .set_regvalue_by_id = cortex_a9_set_regval_by_id,
        .get_regid_by_name = cortex_a9_get_regid_by_name,
        .get_regnum = cortex_a9_get_regnum
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &core_reg);
    static const memory_space_intf memory_space = {
        .read = cortex_a9_memory_read,
        .write = cortex_a9_memory_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = cortex_a9_insert_bp,
        .delete_bp = cortex_a9_delete_bp,
        .get_bp_numbers = cortex_a9_get_bp_numbers,
        .get_bp_addr_by_id = cortex_a9_get_bp_addr_by_id,
        .get_bp_trigger = cortex_a9_get_trigger,
        .check_bp_trigger = cortex_a9_check_bp_trigger,
        .clear_bp_trigger = cortex_a9_clear_bp_trigger,
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
static attr_value_t get_attr_addr_space(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_object(core->space);

    return value;
}
static exception_t set_attr_addr_space(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;

    core->space = SKY_attr_object(*value);
    return No_exp;
}
static attr_value_t get_attr_cpuID(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    attr_value_t value = SKY_make_attr_uinteger(state->cpuID);

    return value;
}
static exception_t set_attr_cpuID(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;

    state->cpuID = SKY_attr_uinteger(*value);
    state->system_reg.MPIDR_EL1 = state->cpuID;

    return No_exp;
}
static attr_value_t get_attr_freq_hz(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->freq_hz);

    return value;
}
static exception_t set_attr_freq_hz(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) conf_obj->obj;

    core->freq_hz = SKY_attr_uinteger(*value);
    return No_exp;
}
static void cortex_a9_register_attribute(conf_class_t * clss)
{
    SKY_register_attribute(clss, "cpuID", get_attr_cpuID, NULL, set_attr_cpuID, NULL, SKY_Attr_Optional, "uinteger",
                           "set current core cpuID");
    SKY_register_attribute(clss, "freq_hz", get_attr_freq_hz, NULL, set_attr_freq_hz, NULL, SKY_Attr_Optional,
                           "uinteger", "");
}
static exception_t memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) obj->obj;

    core->memory_space = obj2;
    core->space = (memory_space_intf *) SKY_get_iface(core->memory_space, MEMORY_SPACE_INTF_NAME);
    if (core->space == NULL)
        skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' interface from %s fail.", obj2->objname);
    return No_exp;
}
static exception_t memory_space_get(conf_object_t * obj, conf_object_t ** obj2, const char **port, int index)
{
    cortex_a9_core_t *core = (cortex_a9_core_t *) obj->obj;

    *obj2 = core->memory_space;
    *port = NULL;
    return No_exp;
}

void init_cortex_a9_core()
{
    static skyeye_class_t class_data = {
        .class_name = "cortex_a9_core",
        .class_desc = "cortex_a9_core",
        .new_instance = new_cortex_a9_core,
        .free_instance = free_cortex_a9_core,
        .get_attr = NULL,
        .set_attr = NULL
    };
    conf_class_t *class = SKY_register_cpu_class(class_data.class_name, &class_data);

    cortex_a9_register_interface(class);

    static const struct ConnectDescription connects[] = {
        (struct ConnectDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = memory_space_set,
            .get = memory_space_get,
        }
    };

    class_register_connects(class, connects);
    cortex_a9_register_attribute(class);
}

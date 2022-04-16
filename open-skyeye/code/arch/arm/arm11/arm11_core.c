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
* @file arm11_core.c
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
#include <sim_control.h>
#include <skyeye_callback.h>
#include <skyeye_core_intf.h>
#include <skyeye_iface.h>
#include <skyeye_interface.h>
#include <memory_space.h>
#include <skyeye_disassemble.h>
#define CP15(idx)       (idx - CP15_BASE)
typedef struct arm11_core
{
    conf_object_t *obj;
    ARMul_State *state;
    generic_address_t load_addr;
    uint32_t mask;
    breakpoint_mgt_t bp_manager;
    core_running_mode_t mode;
} arm11_core_t;
exception_t arm11_memory_read(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count);

exception_t arm11_memory_write(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count);

static void arm11_set_pc(conf_object_t * opaque, generic_address_t pc)
{
    arm11_core_t *core = (arm11_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    arm_set_pc(core->state, pc);
    return;
}

generic_address_t arm11_get_pc(conf_object_t * opaque, generic_address_t pc)
{
    arm11_core_t *core = (arm11_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return core->state->realPC;
}

uint32_t arm11_run(conf_object_t * conf_obj)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    uint32_t steps = state->NumInstrs;

    SKY_pc_hap_occurred(NULL, conf_obj, state->Reg[15]);
    if (state->mode == INTERPRET_RUN)
    {
        arm_step_once(state);
    } else
    {
        /* invalid mode */
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

    return state->NumInstrs - steps;
}

void arm11_stepi(conf_object_t * obj, uint64_t steps)
{
    arm11_core_t *core = (arm11_core_t *) obj->obj;

    core->state->stop_step = core->state->NumInstrs + steps;
    return;
}

uint64_t arm11_get_steps(conf_object_t * obj)
{
    arm11_core_t *core = (arm11_core_t *) obj->obj;

    return core->state->NumInstrs;
}

void arm11_stepi_check(conf_object_t * obj)
{
    arm11_core_t *core = (arm11_core_t *) obj->obj;
    ARMul_State *state = core->state;

    if (state->stop_step != 0 && state->NumInstrs != 0 && state->stop_step == state->NumInstrs)
    {
        SIM_stop(NULL);
        state->stop_step = 0;
    }
    return;
}

int arm11_check_breakpoint_callback(conf_object_t * obj)
{
    arm11_core_t *core = (arm11_core_t *) obj->obj;
    ARMul_State *state = core->state;
    generic_address_t pc = arm_get_pc(state);

    check_breakpoint(&(core->bp_manager), pc - 8, state->NumInstrs);
    return 0;
}
static exception_t arm11_set_regval_by_id(conf_object_t * opaque, uint32 value, int id)
{
    arm11_core_t *core = (arm11_core_t *) opaque->obj;
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
        if (id != 15)
            return state->Reg[id];
        else
            return state->realPC;
    }
    return No_exp;
}

/**
* @brief 
*
* @param id
*
* @return 
*/
static uint32 arm11_get_regval_by_id(conf_object_t * opaque, int id)
{
    arm11_core_t *core = (arm11_core_t *) opaque->obj;
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
        return state->Reg[id];
}

fault_t arm11_read_byte(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    //mem_read(short size, int offset, uint32_t * value){
    return NO_FAULT;
}

fault_t arm11_read_halfword(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    return NO_FAULT;
}

fault_t arm11_read_word(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    return NO_FAULT;
}

static fault_t arm11_write_byte(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return NO_FAULT;
}

static fault_t arm11_write_halfword(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return NO_FAULT;
}

static fault_t arm11_write_word(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return NO_FAULT;
}
static exception_t arm_signal(conf_object_t * opaque, interrupt_signal_t * signal)
{
    arm11_core_t *core = (arm11_core_t *) opaque->obj;
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

static uint32_t arm11_is_io_space(generic_address_t phys_addr)
{
    if (((phys_addr >= 0x71200000) && (phys_addr < 0x71200000 + 0x280)) ||
        ((phys_addr >= 0x71300000) && (phys_addr < 0x71300000 + 0x280)) ||
        ((phys_addr >= 0x7f006000) && (phys_addr < 0x7f006000 + 0x280)) ||
        ((phys_addr >= 0x7f005000) && (phys_addr < 0x7f005000 + 0x3c)) ||
        ((phys_addr >= 0x7e00f000) && (phys_addr < 0x7e00f000 + 0x1000)) ||
        ((phys_addr >= 0x7f008000) && (phys_addr < 0x7f008000 + 0x1000)) ||
        ((phys_addr >= 0x7e002000) && (phys_addr < 0x7e002000 + 0x1000)) ||
        ((phys_addr >= 0x7e005000) && (phys_addr < 0x7e005000 + 0x1000)) ||
        ((phys_addr >= 0x7e00b000) && (phys_addr < 0x7e00b000 + 0x1000)) ||
        ((phys_addr >= 0x7f00b000) && (phys_addr < 0x7f00b000 + 0x1000)) ||
        ((phys_addr >= 0x74300000) && (phys_addr < 0x74300000 + 0x1000)) ||
        ((phys_addr >= 0x7f001000) && (phys_addr < 0x7f001000 + 0x1000)) ||
        ((phys_addr >= 0x78000000) && (phys_addr < 0x78000000 + 0x1000)))
        return 1;
    else
        return 0;
}

/**
* @brief The initialization for arm core instance
*
* @return 
*/
const static cpu_config_t arm11_cpu_info = { "armv6", "arm11", 0x0007b000, 0x0007f000, NONCACHE };
static conf_object_t *new_arm11_core(char *obj_name)
{
    arm11_core_t *core = skyeye_mm_zero(sizeof (arm11_core_t));

    core->obj = new_conf_object(obj_name, core);
    ARMul_EmulateInit();
    ARMul_State *state = skyeye_mm_zero(sizeof (ARMul_State));

    ARMul_NewState(state);
    state->abort_model = 0;
    state->cpu = &arm11_cpu_info;
    state->bigendSig = LOW;

    ARMul_SelectProcessor(state, ARM_v6_Prop | ARM_v5_Prop | ARM_v5e_Prop);
    state->lateabtSig = LOW;
    mmu_init(state);
    /* reset the core to initial state */
    ARMul_Reset(state);
    state->NextInstr = 0;
    state->Emulate = 3;
    core->state = state;
    state->cpu_obj = core->obj;
    state->Reg[0] = 0;
    state->Reg[1] = 1626;
    state->Reg[2] = 0x50000100;

    state->arm11_memory_iface = skyeye_mm_zero(sizeof (memory_space_intf));
    state->arm11_memory_iface->read = arm11_memory_read;
    state->arm11_memory_iface->write = arm11_memory_write;

#if FAST_MEMORY
    mmap_ram(0x50000000, 0x40000000);
    mmap_ram(0x40300000, 0x10000);
#endif

#if (FAST_MEMORY || FAST_MEM32)
    state->is_io_space = arm11_is_io_space;
#endif
    VFPInit(core);
    //register_callback(arm11_stepi_check, Step_callback);
    //register_callback(arm11_check_breakpoint_callback, Step_callback);
    return core->obj;
}

static void free_arm11_core(conf_object_t * dev)
{
    arm11_core_t *core = (arm11_core_t *) dev->obj;

    skyeye_free(core->obj);
    skyeye_free(core->state->arm11_memory_iface);
    skyeye_free(core->state);
    skyeye_free(core);
    return;
}

static attr_value_t get_attr_load_mask(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->mask);

    return value;
}
static exception_t set_attr_load_mask(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;

    core->mask = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_load_addr(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->load_addr);

    return value;
}
static exception_t set_attr_load_addr(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;

    core->load_addr = SKY_attr_uinteger(*value);
    return No_exp;
}
static attr_value_t get_attr_mode(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->mode);

    return value;
}
static exception_t set_attr_mode(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;

    state->mode = core->mode = SKY_attr_uinteger(*value);

    printf("state->mode=%d\n", state->mode);
    return No_exp;
}

static void arm11_register_attribute(conf_class_t * clss)
{
    SKY_register_attribute(clss, "load_mask", get_attr_load_mask, NULL, set_attr_load_mask, NULL, SKY_Attr_Optional,
                           "uinteger", "load mask of processor core");
    SKY_register_attribute(clss, "load_addr", get_attr_load_addr, NULL, set_attr_load_addr, NULL, SKY_Attr_Optional,
                           "uinteger", "load addr space of processor core");
    SKY_register_attribute(clss, "mode", get_attr_mode, NULL, set_attr_mode, NULL, SKY_Attr_Optional, "uinteger",
                           "the different mode for cpu instruction");
    return;
}

static char *arm11_disassemble(conf_object_t * obj, generic_address_t addr)
{
    return skyeye_disassemble(obj, addr);
}

char *arm11_get_architecture(conf_object_t * conf_obj)
{
    return "arm";
}

exception_t arm11_memory_read(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    int i;

    for (i = 0; i < count; i++)
    {
        state->mmu.ops.read_byte(state, addr + i, buf + i);
    }
    return No_exp;
}

exception_t arm11_memory_write(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    int i;

    for (i = 0; i < count; i++)
    {
        state->mmu.ops.write_byte(state, addr + i, *((char *) buf + i));
    }
    return No_exp;
}

static int arm11_insert_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;
    int ret;

    ret = create_a_breakpoint(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

static int arm11_delete_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;
    int ret;

    ret = remove_bp_by_addr(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

static int arm11_get_bp_numbers(conf_object_t * conf_obj)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;

    return get_bp_numbers(&core->bp_manager);
}

static uint32_t arm11_get_bp_addr_by_id(conf_object_t * conf_obj, int id)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;
    breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);

    if (bp)
        return bp->address;
    return 0;
}

static int arm11_get_trigger(conf_object_t * conf_obj)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;

    return get_breakpoint_hit_id(&core->bp_manager);
}

static bool_t arm11_check_bp_trigger(conf_object_t * conf_obj)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;
    int ret;

    ret = check_breakpoint_hit_flag(&core->bp_manager);
    if (ret == 0)
        return True;
    return False;
}

static int arm11_clear_bp_trigger(conf_object_t * conf_obj)
{
    arm11_core_t *core = (arm11_core_t *) conf_obj->obj;

    clear_breakpoint_hit_flag(&core->bp_manager);
    return 0;
}

static exception_t memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    arm11_core_t *core = (arm11_core_t *) obj->obj;
    ARMul_State *state = core->state;

    state->memory_space = obj2;
    state->memory_space_iface = (memory_space_intf *) SKY_get_iface(state->memory_space, MEMORY_SPACE_INTF_NAME);

    if (state->memory_space_iface == NULL)
        skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' interface from %s fail.", obj2->objname);
    return No_exp;
}

static exception_t memory_space_get(conf_object_t * obj, conf_object_t ** obj2, const char **port, int index)
{
    arm11_core_t *core = (arm11_core_t *) obj->obj;

    *obj2 = core->state->memory_space;
    *port = NULL;
    return No_exp;
}

uint32_t arm11_get_address_width(conf_object_t * obj)
{
    return 4;
}

int arm11_get_cpu_exception_num(conf_object_t * obj)
{
    arm11_core_t *core = (arm11_core_t *) obj->obj;
    ARMul_State *state = core->state;

    return ARMErrorV / 4;
}

void arm11_set_cpu_exception_by_id(conf_object_t * obj, int exception_id)
{
    arm11_core_t *core = (arm11_core_t *) obj->obj;
    ARMul_State *state = core->state;

    if (exception_id < ARMErrorV / 4)
        ARMul_Abort(state, exception_id * 4);

    return;
}

extern const char *arm_exception_name[];
char *arm11_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id)
{
    arm11_core_t *core = (arm11_core_t *) obj->obj;
    ARMul_State *state = core->state;

    assert(exception_id < (ARMErrorV / 4));

    return arm_exception_name[exception_id];
}

void init_arm11_core()
{
    static skyeye_class_t class_data = {
        .class_name = "arm11_core",
        .class_desc = "arm11_core",
        .new_instance = new_arm11_core,
        .free_instance = free_arm11_core,
        .get_attr = NULL,
        .set_attr = NULL
    };

    conf_class_t *clss = SKY_register_cpu_class(class_data.class_name, &class_data);

    static const core_info_intf info = {
        .set_pc = arm11_set_pc,
        .get_pc = arm11_get_pc,
        .get_steps = arm11_get_steps,
        .get_endian = arm_get_endian,
        .get_alignment = arm_get_alignment,
        .reset = arm_reset,
        .disassemble = arm11_disassemble,
        .get_architecture = arm11_get_architecture,
        .get_address_width = arm11_get_address_width,
        .get_cpu_exception_num = arm11_get_cpu_exception_num,
        .set_cpu_exception_by_id = arm11_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = arm11_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &info);

    static const skyeye_reg_intf reg = {
        .get_regvalue_by_id = arm11_get_regval_by_id,
        .get_regname_by_id = arm_get_regname_by_id,
        .set_regvalue_by_id = arm11_set_regval_by_id,
        .get_regid_by_name = arm_get_regid_by_name,
        .get_regnum = arm_get_regnum
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg);

    static const core_exec_intf core_exec = {
        .run = arm11_run,
        .stop = arm_stop,
        .stepi = arm11_stepi,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const core_signal_intf core_signal = {
        .signal = arm_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);
    static const memory_space_intf memory_space = {
        .read = arm11_memory_read,
        .write = arm11_memory_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = arm11_insert_bp,
        .delete_bp = arm11_delete_bp,
        .get_bp_numbers = arm11_get_bp_numbers,
        .get_bp_addr_by_id = arm11_get_bp_addr_by_id,
        .get_bp_trigger = arm11_get_trigger,
        .check_bp_trigger = arm11_check_bp_trigger,
        .clear_bp_trigger = arm11_clear_bp_trigger,
    };
    SKY_register_iface(clss, CORE_BREAKPOINT_OP_INTF_NAME, &bp_op);

    static const struct InterfaceDescription ifaces[] = {
        (struct InterfaceDescription) {
            .name = CORE_INFO_INTF_NAME,
            .iface = &info,
        },
        (struct InterfaceDescription) {
            .name = SKYEYE_REG_INTF,
            .iface = &reg,
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
    static const struct ConnectDescription connects[] = {
        (struct ConnectDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = memory_space_set,
            .get = memory_space_get,
        }
    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    arm11_register_attribute(clss);
    return;
}

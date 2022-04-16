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
* @file cortex_m3_core.c
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
#include <sim_control.h>
#include <skyeye_callback.h>
#include <skyeye_core_intf.h>
#include <skyeye_iface.h>
#include <skyeye_interface.h>
#include <memory_space.h>
#include <skyeye_disassemble.h>
#define CP15(idx)       (idx - CP15_BASE)

typedef enum cortex_m3_vector_table
{
    MSP_Init_Value = 0,
    Reset_Vector,
    NMI_Vector,
    HardFault_Vector,
    MemManage_Vector,
    BusFault_Vector,
    UsageFault_Vector,
    SecureFault_Vector,
    RES0,
    RES1,
    RES2,
    SVC_Vector,
    Debug_Monitor,
    RES3,
    PendSV_Vector,
    Systick_Vector,
}vector_table_t;

typedef struct cortex_m3_core
{
    conf_object_t *obj;
    ARMul_State *state;
    generic_address_t load_addr;
    uint32_t mask;
    breakpoint_mgt_t bp_manager;
    int freq_hz;
} cortex_m3_core_t;

exception_t cortex_m3_memory_read(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count);

exception_t cortex_m3_memory_write(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count);

static void cortex_m3_set_pc(conf_object_t * opaque, generic_address_t pc)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    arm_set_pc(core->state, pc);
    return;
}

generic_address_t cortex_m3_get_pc(conf_object_t * opaque)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) opaque->obj;
    generic_address_t pc = arm_get_pc(core->state);

    return pc;
}

static void systick_do_cycle(ARMul_State *state)
{
    //SysTick enable
    if (state->scb_reg.SYSTICK_CS & 1)
    {
        if (state->scb_reg.SYSTICK_CUR > 0)
        {
            state->scb_reg.SYSTICK_CUR --;
        }
    }

    if (state->scb_reg.SYSTICK_CUR == 0)
    {
        //SysTick underflow interrupt enable
        if (state->scb_reg.SYSTICK_CS & 2)
        {
            state->M3_exception = Systick_Vector;
        }

        //COUNTFlag set to 1 when SYSTICK_CUR to 0
        state->scb_reg.SYSTICK_CS = 1 << 16;

        state->scb_reg.SYSTICK_CUR = state->scb_reg.SYSTICK_RL;
    }
}

int cortex_m3_run(conf_object_t * conf_obj)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    uint32_t steps = state->NumInstrs;

    generic_address_t pc = state->realPC;

    if (state->realPC > 0xfffffff0)
    {
        //return from exception
        int i;
        for (i = 0; i < 4; i++)
        {
            state->memory_space_iface->read(state->memory_space, state->Reg[13], &(state->Reg[i]), 4);
            state->Reg[13] += 4;
        }
        state->memory_space_iface->read(state->memory_space, state->Reg[13], &(state->Reg[12]), 4);
        state->Reg[13] += 4;
        state->memory_space_iface->read(state->memory_space, state->Reg[13], &(state->Reg[14]), 4);
        state->Reg[13] += 4;
        state->memory_space_iface->read(state->memory_space, state->Reg[13], &(pc), 4);
        state->Reg[13] += 4;
        state->memory_space_iface->read(state->memory_space, state->Reg[13], &(state->Cpsr), 4);
        state->Reg[13] += 4;
        state->Reg[15] = pc & (~1);
        FLUSHPIPE;
    }
    arm_step_once(state);

    systick_do_cycle(state);
    //breakpoint check
    check_breakpoint(&(core->bp_manager), pc, state->NumInstrs);

    //check stepi
    if (state->stop_step != 0 && state->NumInstrs != 0 && state->stop_step == state->NumInstrs)
    {
        SIM_stop(NULL);
        state->stop_step = 0;
    }

    return state->NumInstrs - steps;
}

static void cortex_m3_step_once(conf_object_t * opaque)
{
    //ARMul_DoInstr(state);
    cortex_m3_core_t *core = (cortex_m3_core_t *) opaque->obj;
    ARMul_State *state = core->state;

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

void cortex_m3_stepi(conf_object_t * obj, uint64_t steps)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) obj->obj;

    core->state->stop_step = core->state->NumInstrs + steps;
    return;
}

uint64_t cortex_m3_get_steps(conf_object_t * obj)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) obj->obj;

    return core->state->NumInstrs;
}

void cortex_m3_stepi_check(conf_object_t * obj)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) obj->obj;
    ARMul_State *state = core->state;

    if (state->stop_step != 0 && state->NumInstrs != 0 && state->stop_step == state->NumInstrs)
    {
        SIM_stop(NULL);
        state->stop_step = 0;
    }
    return;
}

int cortex_m3_check_breakpoint_callback(conf_object_t * obj)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) obj->obj;
    ARMul_State *state = core->state;
    generic_address_t pc = arm_get_pc(state);

    check_breakpoint(&(core->bp_manager), pc - 8, state->NumInstrs);
    return 0;
}
static exception_t cortex_m3_set_regval_by_id(conf_object_t * opaque, uint32 value, int id)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_set_regvalue_by_id(state, value, id);
    /*
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
        state->Reg[id] = value;
    return No_exp;
    */
}

/**
* @brief
*
* @param id
*
* @return
*/
static uint32 cortex_m3_get_regval_by_id(conf_object_t * opaque, int id)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    return arm_get_regvalue_by_id(state, id);
    /*
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
    */
}

fault_t cortex_m3_read_byte(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    //mem_read(short size, int offset, uint32_t * value){
    return NO_FAULT;
}

fault_t cortex_m3_read_halfword(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    return NO_FAULT;
}

fault_t cortex_m3_read_word(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    return NO_FAULT;
}

static fault_t cortex_m3_write_byte(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return NO_FAULT;
}

static fault_t cortex_m3_write_halfword(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return NO_FAULT;
}

static fault_t cortex_m3_write_word(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return NO_FAULT;
}
static exception_t arm_signal(conf_object_t * opaque, interrupt_signal_t * signal)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) opaque->obj;
    ARMul_State *state = core->state;

    arm_signal_t *arm_signal = &signal->arm_signal;

    //外部中断从中断向量表第16号开始
    state->M3_exception = arm_signal->m0_irq + 16;

    return No_exp;
}

/**
* @brief The initialization for arm core instance
*
* @return
*/
const static cpu_config_t cortex_m3_cpu_info = { "armv6", "cortex_m3", 0x0007b000, 0x0007f000, NONCACHE };
static conf_object_t *new_cortex_m3_core(char *obj_name)
{
    cortex_m3_core_t *core = skyeye_mm_zero(sizeof (cortex_m3_core_t));

    core->obj = new_conf_object(obj_name, core);
    ARMul_EmulateInit();
    ARMul_State *state = skyeye_mm_zero(sizeof (ARMul_State));

    ARMul_NewState(state);
    state->abort_model = 0;
    state->cpu = &cortex_m3_cpu_info;
    state->bigendSig = LOW;

    ARMul_SelectProcessor(state, ARM_v6_Prop | ARM_v5_Prop | ARM_v5e_Prop);
    state->lateabtSig = LOW;
    mmu_init(state);
    /* reset the core to initial state */
    ARMul_Reset(state);
    state->NextInstr = 0;
    state->Emulate = 3;
    core->state = state;
    core->state->scb_reg.VTOR = 0x11000000;

    state->cpu_obj = core->obj;

    state->arm11_memory_iface = skyeye_mm_zero(sizeof (memory_space_intf));
    state->arm11_memory_iface->read = cortex_m3_memory_read;
    state->arm11_memory_iface->write = cortex_m3_memory_write;

    //register_callback(cortex_m3_stepi_check, Step_callback);
    //register_callback(cortex_m3_check_breakpoint_callback, Step_callback);
    return core->obj;
}

static void free_cortex_m3_core(conf_object_t * dev)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) dev->obj;

    skyeye_free(core->state->arm11_memory_iface);
    skyeye_free(core->state);
    skyeye_free(core);
}

static attr_value_t get_attr_load_mask(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->mask);

    return value;
}
static exception_t set_attr_load_mask(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;

    core->mask = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_load_addr(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->load_addr);

    return value;
}

static exception_t set_attr_load_addr(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;

    core->load_addr = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_freq_hz(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->freq_hz);

    return value;
}

static exception_t set_attr_freq_hz(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;

    core->freq_hz = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_base_vector(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(core->state->scb_reg.VTOR);

    return value;
}

static exception_t set_attr_base_vector(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;

    core->state->scb_reg.VTOR = SKY_attr_uinteger(*value);
    return No_exp;
}

static void cortex_m3_register_attribute(conf_class_t * clss)
{
    SKY_register_attribute(clss, "load_mask", get_attr_load_mask, NULL, set_attr_load_mask, NULL, SKY_Attr_Optional,
                           "uinteger", "load mask of processor core");
    SKY_register_attribute(clss, "load_addr", get_attr_load_addr, NULL, set_attr_load_addr, NULL, SKY_Attr_Optional,
                           "uinteger", "load addr space of processor core");
    SKY_register_attribute(clss, "freq_hz", get_attr_freq_hz, NULL, set_attr_freq_hz, NULL, SKY_Attr_Optional,
                           "uinteger", "set frequency for processor core");
    SKY_register_attribute(clss, "vector_base_addr", get_attr_base_vector, NULL, set_attr_base_vector, NULL, SKY_Attr_Optional,
                           "uinteger", "set vector base addr for processor core");
    return;
}

static char *cortex_m3_disassemble(conf_object_t * obj, generic_address_t addr)
{
    return skyeye_disassemble(obj, addr);
}

char *cortex_m3_get_architecture(conf_object_t * conf_obj)
{
    return "arm";
}

exception_t cortex_m3_memory_read(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    memory_space_intf *iface = state->memory_space_iface;

    return iface->read(state->memory_space, addr, buf, count);
}

exception_t cortex_m3_memory_write(conf_object_t * conf_obj, generic_address_t addr, void *buf, size_t count)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;
    ARMul_State *state = core->state;
    memory_space_intf *iface = state->memory_space_iface;

    return iface->write(state->memory_space, addr, buf, count);
}

static int cortex_m3_insert_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;
    int ret;

    ret = create_a_breakpoint(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

static int cortex_m3_delete_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;
    int ret;

    ret = remove_bp_by_addr(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

static int cortex_m3_get_bp_numbers(conf_object_t * conf_obj)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;

    return get_bp_numbers(&core->bp_manager);
}

static uint32_t cortex_m3_get_bp_addr_by_id(conf_object_t * conf_obj, int id)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;
    breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);

    if (bp)
        return bp->address;
    return 0;
}

static int cortex_m3_get_trigger(conf_object_t * conf_obj)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;

    return get_breakpoint_hit_id(&core->bp_manager);
}

static bool_t cortex_m3_check_bp_trigger(conf_object_t * conf_obj)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;
    int ret;

    ret = check_breakpoint_hit_flag(&core->bp_manager);
    if (ret == 0)
        return True;
    return False;
}

static int cortex_m3_clear_bp_trigger(conf_object_t * conf_obj)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) conf_obj->obj;

    clear_breakpoint_hit_flag(&core->bp_manager);
    return 0;
}

static exception_t memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) obj->obj;
    ARMul_State *state = core->state;

    state->memory_space = obj2;
    state->memory_space_iface = (memory_space_intf *) SKY_get_iface(state->memory_space, MEMORY_SPACE_INTF_NAME);

    if (state->memory_space_iface == NULL)
        skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' interface from %s fail.", obj2->objname);
    return No_exp;
}

static exception_t memory_space_get(conf_object_t * obj, conf_object_t ** obj2, const char **port, int index)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) obj->obj;

    *obj2 = core->state->memory_space;
    *port = NULL;
    return No_exp;
}

uint32_t cortex_m3_get_address_width(conf_object_t * obj)
{
    return 4;
}

int cortex_m3_get_cpu_exception_num(conf_object_t * obj)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) obj->obj;
    ARMul_State *state = core->state;

    return ARMErrorV / 4;
}

void cortex_m3_set_cpu_exception_by_id(conf_object_t * obj, int exception_id)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) obj->obj;
    ARMul_State *state = core->state;

    if (exception_id < ARMErrorV / 4)
        ARMul_Abort(state, exception_id * 4);

    return;
}

extern const char *arm_exception_name[];
char *cortex_m3_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id)
{
    cortex_m3_core_t *core = (cortex_m3_core_t *) obj->obj;
    ARMul_State *state = core->state;

    assert(exception_id < (ARMErrorV / 4));

    return arm_exception_name[exception_id];
}

void init_cortex_m3_core()
{
    static skyeye_class_t class_data = {
        .class_name = "cortex_m3",
        .class_desc = "cortex_m3",
        .new_instance = new_cortex_m3_core,
        .free_instance = free_cortex_m3_core,
        .get_attr = NULL,
        .set_attr = NULL
    };

    conf_class_t *clss = SKY_register_cpu_class(class_data.class_name, &class_data);

    static const core_info_intf info = {
        .set_pc = cortex_m3_set_pc,
        .get_pc = cortex_m3_get_pc,
        .get_steps = cortex_m3_get_steps,
        .get_endian = arm_get_endian,
        .get_alignment = arm_get_alignment,
        .reset = arm_reset,
        .disassemble = cortex_m3_disassemble,
        .get_architecture = cortex_m3_get_architecture,
        .get_address_width = cortex_m3_get_address_width,
        .get_cpu_exception_num = cortex_m3_get_cpu_exception_num,
        .set_cpu_exception_by_id = cortex_m3_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = cortex_m3_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &info);

    static const skyeye_reg_intf reg = {
        .get_regvalue_by_id = cortex_m3_get_regval_by_id,
        .get_regname_by_id = arm_get_regname_by_id,
        .set_regvalue_by_id = cortex_m3_set_regval_by_id,
        .get_regid_by_name = arm_get_regid_by_name,
        .get_regnum = arm_get_regnum
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg);

    static const core_exec_intf core_exec = {
        .run = cortex_m3_run,
        .stop = arm_stop,
        .stepi = cortex_m3_stepi,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const core_signal_intf core_signal = {
        .signal = arm_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);
    static const memory_space_intf memory_space = {
        .read = cortex_m3_memory_read,
        .write = cortex_m3_memory_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = cortex_m3_insert_bp,
        .delete_bp = cortex_m3_delete_bp,
        .get_bp_numbers = cortex_m3_get_bp_numbers,
        .get_bp_addr_by_id = cortex_m3_get_bp_addr_by_id,
        .get_bp_trigger = cortex_m3_get_trigger,
        .check_bp_trigger = cortex_m3_check_bp_trigger,
        .clear_bp_trigger = cortex_m3_clear_bp_trigger,
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

    cortex_m3_register_attribute(clss);
    return;
}

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <skyeye_types.h>
#include <skyeye_class.h>
#include <skyeye_core_intf.h>
#include <skyeye_int_register.h>
#include <memory_space.h>
#include <skyeye_iface.h>
#include <skyeye_attribute.h>
#include <skyeye_mm.h>
#include <emul.h>
#include "mipsdef.h"
#include "mips_regformat.h"
#include "mips_cpu.h"
#include "skyeye_disassemble.h"
#include <skyeye_pref.h>
#include <skyeye_arch.h>
#include "skyeye_log.h"
#include "sim_control.h"

/**
 * @brief inital icache
 *
 * @param mstate core state
 */

/**
* @brief ls132r read mem interface
*
* @param pa	physic addr
* @param data  data pointer
* @param len	data length
*/
exception_t ls132r_mem_read(conf_object_t * obj, generic_address_t pa, void *data, size_t len)
{
    mips_core_t *core = (mips_core_t *) obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    return iface->read(core->memory_space, pa, data, len);
}

/**
* @brief ls132r write memory interface
*
* @param pa	physic addr
* @param data	data pointer
* @param len	data length
*/
exception_t ls132r_mem_write(conf_object_t * obj, generic_address_t pa, void *data, size_t len)
{
    mips_core_t *core = (mips_core_t *) obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    return iface->write(core->memory_space, pa, data, len);
}

uint64_t ls132r_run(conf_object_t * conf_obj)
{
    mips_core_t *mstate = (mips_core_t *) conf_obj->obj;
    uint64_t steps = mstate->insn_num;

    ls132r_step_once(conf_obj);

    if (mstate->debug_mode)
    {
        //stop check
        if (mstate->stop_step != 0 && mstate->insn_num != 0 && mstate->stop_step <= mstate->insn_num)
        {
            SIM_stop(NULL);
            mstate->stop_step = 0;
        }
        //check breakpoint
        check_breakpoint(&(mstate->bp_manager), mstate->pc, mstate->insn_num);
    }

    return (mstate->insn_num - steps);
}

conf_object_t *new_ls132r_core(const char *obj_name)
{
    mips_core_t *core = skyeye_mm_zero(sizeof (mips_core_t));

    core->obj = new_conf_object(obj_name, core);

    MIPS_CPU_State *cpu = skyeye_mm_zero(sizeof (MIPS_CPU_State));

    cpu->core_num = 1;
    cpu->core = core;
    core->cpu = (conf_object_t *) cpu;

    /*clfei: add the initialization of PRId and Config */
    core->cp0[PRId] = 0x00004200;
    core->cp0[Config] = 0x00030270;
    set_bit(core->cp0[SR], SR_CU0);
    set_bit(core->mode, 2);

    core->warm = 0;
    core->conf.ec = 4;

    //No interrupt
    core->irq_pending = 0;
    core->cp0[SR] = 0x40004;
    core->active = 1;                   /* set cpu suspend */

    core->mode = 0;
    core->debug_mode = 1;
    core->freq_hz = 8000000;
    core->mmu_flag = NO_MMU;

    return core->obj;
}

exception_t free_ls132r_core(conf_object_t * obj)
{
    mips_core_t *core = obj->obj;

    skyeye_free(core->core_memory_iface);
    skyeye_free(core->cpu);
    skyeye_free(core->obj);
    skyeye_free(core);

    return 0;
}

exception_t config_ls132r_core(conf_object_t * obj)
{
    mips_core_t *core = obj->obj;

    core->core_memory_iface = skyeye_mm_zero(sizeof (memory_space_intf));

    core->core_memory_iface->read = ls132r_mem_read;
    core->core_memory_iface->write = ls132r_mem_write;
    core->process_exception = ls132r_process_exception;

    return No_exp;
}

static exception_t reset_ls132r_core(conf_object_t * conf_obj, const char *parameter)
{
    mips_core_t *mstate = (mips_core_t *) conf_obj->obj;

    mstate->cp0[SR] = set_bit(mstate->cp0[SR], SR_BEV);
    mstate->isa_mode = 0;
    mstate->pc = ls132r_vector_base + reset_vector;
    mstate->pipeline = nothing_special;

    return No_exp;
}

int ls132r_signal(conf_object_t * conf_obj, mips_signal_t * signal)
{
    mips_core_t *mstate = conf_obj->obj;
    int ipNum = signal->HARD_IP;

    mstate->cp0[Cause] |= 1 << ipNum;
    mstate->irq_pending = 1;

    return 0;
}

void init_ls132r(void)
{
    /*Processor class registration */
    static skyeye_class_t class_data = {
        .class_name = "ls132r_core",
        .class_desc = "ls132r_core",
        .new_instance = new_ls132r_core,
        .free_instance = free_ls132r_core,
        .config_instance = config_ls132r_core,
        .reset_instance = reset_ls132r_core,
        .get_attr = NULL,
        .set_attr = NULL
    };
    conf_class_t *clss = SKY_register_cpu_class(class_data.class_name, &class_data);

    /*Processor information interface */
    static const core_info_intf core_info = {
        .set_pc = mips_set_pc,
        .get_pc = mips_get_pc,
        .get_steps = mips_get_step,
        .get_endian = mips_get_endian,
        .get_alignment = mips_get_alignment,
        .set_textsection_info = mips_set_textsection_info,
        .get_architecture = mips_get_architecture,
        .disassemble = mips_disassemble,
        .get_address_width = mips_get_address_width
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

    /*register interface */
    static const skyeye_reg_intf reg = {
        .get_regvalue_by_id = mips_get_regval_by_id,
        .get_regname_by_id = mips_get_regname_by_id,
        .set_regvalue_by_id = mips_set_regval_by_id,
        .get_regid_by_name = mips_get_regid_by_name,
        .get_regnum = mips_get_regnum
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg);

    /*Processor operating interface */
    static const core_exec_intf core_exec = {
        .run = ls132r_run,
        .stop = mips_stop,
        .stepi = mips_stepi,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    /*interrupt interface */
    static const core_signal_intf core_signal = {
        .signal = ls132r_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    /*memory_space interface */
    static const memory_space_intf memory_space = {
        .read = ls132r_mem_read,
        .write = ls132r_mem_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    /*breakpoint interface */
    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = mips_insert_bp,
        .delete_bp = mips_delete_bp,
        .get_bp_numbers = mips_get_bp_numbers,
        .get_bp_addr_by_id = mips_get_bp_addr_by_id,
        .get_bp_trigger = mips_get_trigger,
        .check_bp_trigger = mips_check_bp_trigger,
        .clear_bp_trigger = mips_clear_bp_trigger,
    };
    SKY_register_iface(clss, CORE_BREAKPOINT_OP_INTF_NAME, &bp_op);

    static const struct InterfaceDescription ifaces[] = {
        (struct InterfaceDescription) {
            .name = CORE_EXEC_INTF_NAME,
            .iface = &core_exec,
        },
        (struct InterfaceDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &memory_space,
        },
        (struct InterfaceDescription) {
            .name = CORE_SIGNAL_INTF_NAME,
            .iface = &core_signal,
        },
        (struct InterfaceDescription) {
            .name = CORE_BREAKPOINT_OP_INTF_NAME,
            .iface = &bp_op,
        },
        (struct InterfaceDescription) {
            .name = SKYEYE_REG_INTF,
            .iface = &reg,
        },
        (struct InterfaceDescription) {
            .name = CORE_INFO_INTF_NAME,
            .iface = &core_info,
        }
    };
    static const struct ConnectDescription connects[] = {
        (struct ConnectDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = mips_memory_space_set,
            .get = mips_memory_space_get,
        }
    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);
    mips_register_attribute(clss);

}

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

/**
* @brief inital icache
*
* @param mstate core state
*/
#include "skyeye_uart_ops.h"
//extern MIPS_State* mstate;

static void init_icache(mips_core_t * mstate)
{
    int i;

    for (i = 0; i < Icache_log2_sets; i++)
    {
        Icache_lru_init(mstate->icache.set[i].Icache_lru);
    }
}

/**
* @brief inital dcache
*
* @param mstate core state
*/
static void init_dcache(mips_core_t * mstate)
{
    int i;

    for (i = 0; i < Dcache_log2_sets; i++)
    {
        Dcache_lru_init(mstate->dcache.set[i].Dcache_lru);
    }
}

/**
* @brief inital tlb
*
* @param mstate	core state
*/
static void init_tlb(mips_core_t * mstate)
{
    int i;

    for (i = 0; i < tlb_map_size + 1; i++)
    {
        mstate->tlb_map[i] = NULL;
    }
}

/**
* @brief  initial a cpu core
*
* @param mstate core state
* @param core_id core id
*/
bool mips_core_init(mips_core_t * mstate, int core_id)
{
    set_bit(mstate->mode, 2);
    /*      mstate = (MIPS_State* )malloc(sizeof(MIPS_State)); */
    if (!mstate)
    {
        fprintf(stderr, "malloc error!\n");
        skyeye_exit(-1);
    }

    mstate->warm = 0;
    mstate->conf.ec = 4;                //I don't know what should it be.

    //No interrupt
    mstate->irq_pending = 0;

    mstate->cp0[SR] = 0x40004;

    init_icache(mstate);
    init_dcache(mstate);
    init_tlb(mstate);

    return true;
}

conf_object_t *new_mips_core(char *obj_name)
{
    mips_core_t *core = skyeye_mm_zero(sizeof (mips_core_t));

    core->obj = new_conf_object(obj_name, core);

    core->mode = 0;
    core->debug_mode = 0;
#if 1                                   /*old feamework */
    MIPS_CPU_State *cpu = skyeye_mm_zero(sizeof (MIPS_CPU_State));

    cpu->core_num = 1;
    cpu->core = core;
    core->cpu = cpu;
#endif
    return core->obj;
}

conf_object_t *new_gs32eb1_core(const char *obj_name)
{
    conf_object_t *obj = new_mips_core(obj_name);
    mips_core_t *core = (mips_core_t *) obj->obj;

    /*clfei: add the initialization of PRId and Config */
    core->cp0[PRId] = 0x00004200;
    core->cp0[Config] = 0x00030270;
    set_bit(core->cp0[SR], SR_CU0);

    mips_core_init(core, 1);
    core->active = 1;                   /* set cpu suspend */
    core->mmu_flag = SUPPORT_MMU;       //support MMU and cache

    return obj;
}

exception_t free_gs32eb1_core(conf_object_t * obj)
{
    mips_core_t *core = obj->obj;

    skyeye_free(core->core_memory_iface);
    skyeye_free(core->cpu);
    skyeye_free(core->obj);
    skyeye_free(core);

    return 0;
}

exception_t config_gs32eb1_core(conf_object_t * obj)
{
    mips_core_t *core = obj->obj;

    core->core_memory_iface = skyeye_mm_zero(sizeof (memory_space_intf));

    core->core_memory_iface->read = mips_mem_read;
    core->core_memory_iface->write = mips_mem_write;
    core->process_exception = gs32eb1_process_exception;
    //if(core->mode !=0)
    //mips_dyncom_init(core);

    return 0;
}

static void reset_gs32eb1_core(conf_object_t * conf_obj)
{
    mips_core_t *mstate = (mips_core_t *) conf_obj->obj;

    mstate->cp0[SR] = set_bit(mstate->cp0[SR], SR_BEV);
    mstate->pc = reset_vector_base + reset_vector;
    mstate->pipeline = nothing_special;

}

void init_gs32eb1(void)
{
    /*Processor class registration */
    static skyeye_class_t class_data = {
        .class_name = "gs32eb1_core",
        .class_desc = "gs32eb1_core",
        .new_instance = new_gs32eb1_core,
        .free_instance = free_gs32eb1_core,
        .config_instance = config_gs32eb1_core,
        .reset_instance = reset_gs32eb1_core,
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
        .run = gs32eb1_run,
        .stop = mips_stop,
        .stepi = mips_stepi,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    /*interrupt interface */
    static const core_signal_intf core_signal = {
        .signal = gs32eb1_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    /*memory_space interface */
    static const memory_space_intf memory_space = {
        .read = mips_mem_read,
        .write = mips_mem_write
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

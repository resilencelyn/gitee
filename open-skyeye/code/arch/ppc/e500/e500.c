/*
 * e500.c:
 *
 * Copyright (C) 2014 Oubang Shen <shenoubang@gmail.com>
 * Skyeye Develop Group, for help please send mail to
 * <skyeye-developer@lists.gro.clinux.org>
 *
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <skyeye_types.h>
#include <skyeye_class.h>
#include <skyeye_core_intf.h>
#include <skyeye_int_register.h>
#include <skyeye_mm.h>
#include <skyeye_iface.h>
#include <skyeye_log.h>
#include <ppc_mmu.h>
#include <ppc_cpu.h>
#include <ppc_exc.h>
#include <ppc_dec.h>

/* head file for bd_t */
#include <ppc_boot.h>

/* ---------------boot for linux -------------------- */
static size_t load_data(e500_core_t * core, char *src, size_t len, size_t start_addr)
{
    size_t addr = start_addr, cnt = 0;

    for (cnt = 0; cnt < len; cnt++)
    {
        core->memory_space_iface->write(core->memory_space, addr++, &src[cnt], 1);
    }

    return cnt;
}
static void set_bootcmd(e500_core_t * core)
{
    const int bd_start = 8 * 1024 * 1024;
    const int initrd_start = 32 * 1024 * 1024, initrd_size = 2 * 1024 * 1024;
    bd_t t;

    memset(&t, '\0', sizeof (t));
    t.bi_immr_base = (0xe0000000);
    t.bi_busfreq = (100 * 1024 * 1024);
    t.bi_intfreq = (500 * 1024 * 1024);
    t.bi_baudrate = (9600);
    t.bi_memsize = (64 * 1024 * 1024);

    load_data(core, (char *) &t, sizeof (t), bd_start);

    core->gpr[3] = bd_start;

    /*
     *   r4 - Starting address of the init RAM disk
     *   r5 - Ending address of the init RAM disk
     */
    core->gpr[4] = initrd_start;
    core->gpr[5] = initrd_start + initrd_size;

    char *bootcmd = "root=/dev/ram0 console=ttyCPM0 mem=64M";
    const int bootcmd_start = 9 * 1024 * 1024;

    /* load bootcmd string to bootcmd_start address */
    load_data(core, bootcmd, (strlen(bootcmd) + 1), bootcmd_start);

    core->gpr[6] = bootcmd_start;
    core->gpr[7] = bootcmd_start + strlen(bootcmd) + 1;
}
static void setup_boot_map(e500_core_t * core)
{
    /* setup initial tlb map for linux, that should be done by bootloader */
    ppc_tlb_entry_t *entry = &core->mmu.l2_tlb1_vsp[0];

    entry->v = 1;                       /* entry is valid */
    entry->ts = 0;                      /* address space 0 */
    entry->tid = 0;                     /* TID value for shared(global) page */
    //entry->epn = 0xC0000; /* Virtual address of DDR ram in address space*/
    entry->epn = 0x00000;               /* Virtual address of DDR ram in address space */
    entry->rpn = 0x0;                   /* Physical address of DDR ram in address space */
    entry->size = 0x7;                  /* 16M byte page size */
    /* usxrw should be initialized to 010101 */
    entry->usxrw |= 0x15;               /* Full supervisor mode access allowed */
    entry->usxrw &= 0x15;               /* No user mode access allowed */
    entry->wimge = 0x8;                 /* Caching-inhibited, non-coherent,big-endian */
    entry->x = 0;                       /* Reserved system attributes */
    entry->u = 0;                       /* User attribute bits */
    entry->iprot = 1;                   /* Page is protected from invalidation */
}

/* -------------------------------------------------- */
static void e500_dec_io_do_cycle(e500_core_t * core)
{
    core->tbl++;
    /* if tbl overflow, we increase tbh */
    if (core->tbl == 0)
        core->tbu++;
    /* trigger interrupt when dec value from 1 to 0 */
    if (core->dec > 0)
        core->dec--;

    /* if decrementer eqauls zero */
    if((core->tcr & DEC_TIMER_ENABLE) != 0)
    {
        if ((core->dec == 0) && (core->msr & MSR_EE))
        {
            /* trigger timer interrupt */
            ppc_exception(core, DEC, 0, core->pc);
            core->dec = 150000;
        }
    }

    return;
}

static uint32_t e500_get_ccsr_base(uint32_t reg)
{
    return ((reg >> 8) & 0xfff) << 20;
}

static conf_object_t *new_e500_core(char *obj_name)
{
    conf_object_t *obj = new_ppc_core(obj_name);
    e500_core_t *core = (e500_core_t *) obj->obj;

    core->effective_to_physical = e500_effective_to_physical;
    core->ppc_exception = e500_ppc_exception;
    core->dec_io_do_cycle = e500_dec_io_do_cycle;
    core->get_ccsr_base = e500_get_ccsr_base;

    core->pvr = 0x80200010;             /* PVR for e500 */

    /* set default ccsr addr: 4G - 9M or 0xff70_0000, (ccsr >> 8 & 0xFFF) << 20 == 0xff70_0000 */
    core->cpu->ccsr = 0x000ff700;
#if 0
    /* open bootpage translation, c0000 is boot page = first elf entry >> 12 */
    core->cpu->bptr = 0x80000000 | 0xc0000;
#endif
    //core->ccsr_size                       = 0x00100000;   /* ccsr_size: 1MB */
    //T2080 ccsr_size = 2MB
    core->ccsr_size = 0x00200000;       /* ccsr_size: 2MB */

    e500_mmu_init(&core->mmu);

    /* ---------------boot for linux -------------------- */
    //already put set boot cmd in soc module, it not necessary */
    //set_bootcmd(core);
    setup_boot_map(core);
    core->isWake = 1;
    /* -------------------------------------------------- */

    return obj;
}
static void free_e500(conf_object_t * obj)
{
    free_ppc_core(obj);
    return;
}
static exception_t reset_instance(conf_object_t * obj, const char *parameter)
{
}
static exception_t raise_signal(conf_object_t * obj, interrupt_signal_t * signal)
{
    e500_core_t *core = (e500_core_t *) obj->obj;

    // if(core->msr & 0x8000) //EE == 1 
    {
        core->ipi_flag = 1;
        core->hardware_vector_mode = 0;
    }
}
void init_e500(void)
{
    static skyeye_class_t class_data = {
        .class_name = "e500_core",
        .class_desc = "powerpc e500 core",
        .new_instance = new_e500_core,
        .free_instance = free_e500,
        .config_instance = config_ppc_core,
        .reset_instance = reset_instance,
        .get_attr = NULL,
        .set_attr = NULL
    };
    conf_class_t *clss = SKY_register_cpu_class(class_data.class_name, &class_data);

    static const core_exec_intf core_exec = {
        .run = ppc_run,
        /* FIXME */
        .stop = NULL,
        .stepi = ppc_stepi,
        .power = ppc_power
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const memory_space_intf memory_space = {
        .read = ppc_space_read,
        .write = ppc_space_write,
        .get_page = ppc_get_page
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    static const core_info_intf core_info = {
        .set_pc = ppc_set_pc,
        .get_pc = ppc_get_pc,
        .get_steps = ppc_get_step,
        .get_endian = ppc_get_endian,
        .get_alignment = ppc_get_alignment,
        .set_textsection_info = ppc_set_textsection_info,
        .get_architecture = ppc_get_architecture,
        .disassemble = ppc_disassemble,
        /* FIXME */
        .reset = NULL,
        .get_address_width = ppc_get_address_width,
        .set_memory_watch_status = ppc_set_memory_watch_status,
        .get_cpu_exception_num = ppc_get_cpu_exception_num,
        .set_cpu_exception_by_id = ppc_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = ppc_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

    static const skyeye_reg_intf reg = {
        .get_regvalue_by_id = ppc_get_regval_by_id,
        .get_regname_by_id = ppc_get_regname_by_id,
        .set_regvalue_by_id = ppc_set_regval_by_id,
        .get_regid_by_name = ppc_get_regid_by_name,
        .get_regnum = ppc_get_regnum
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg);

    static const skyeye_reg64_intf reg64 = {
        .get_regvalue_by_id = ppc_get_reg64val_by_id,
        .get_regname_by_id = ppc_get_regname_by_id,
        .set_regvalue_by_id = ppc_set_reg64val_by_id,
        .get_regid_by_name = ppc_get_regid_by_name,
        .get_regnum = ppc_get_regnum
    };
    SKY_register_iface(clss, SKYEYE_REG64_INTF, &reg64);

    static const core_signal_intf core_signal = {
        .signal = raise_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = ppc_insert_bp,
        .delete_bp = ppc_delete_bp,
        .get_bp_numbers = ppc_get_bp_numbers,
        .get_bp_addr_by_id = ppc_get_bp_addr_by_id,
        .get_bp_trigger = ppc_get_trigger,
        .check_bp_trigger = ppc_check_bp_trigger,
        .clear_bp_trigger = ppc_clear_bp_trigger,
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
            .name = SKYEYE_REG64_INTF,
            .iface = &reg64,
        }
    };

    static const struct ConnectDescription connects[] = {
        (struct ConnectDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = ppc_memory_space_set,
            .get = ppc_memory_space_get,
        }
    };

    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    ppc_register_attribute(clss);
}

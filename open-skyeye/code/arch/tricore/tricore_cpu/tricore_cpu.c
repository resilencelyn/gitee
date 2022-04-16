/*************************************************************************
	> TriCore simulation for SkyEye: implement tricore_cpu iface
	> File Name    : tricore_cpu.c
	> Author       : jiachao
	> Mail         : jiachao@digiproto.com
	> Created Time : 2021/10/26 09:42:09
************************************************************************/

/**********************************************
 * Start implement tricore core_info_intf iface *
 **********************************************/
#include "cpu.h"
#include "helper.h"

static char *regnames_a[] =
{
    "a0"  , "a1"  , "a2"  , "a3" , "a4"  , "a5" ,
    "a6"  , "a7"  , "a8"  , "a9" , "sp" , "a11" ,
    "a12" , "a13" , "a14" , "a15",
};

static char *regnames_d[] =
{
    "d0"  , "d1"  , "d2"  , "d3" , "d4"  , "d5"  ,
    "d6"  , "d7"  , "d8"  , "d9" , "d10" , "d11" ,
    "d12" , "d13" , "d14" , "d15",
};

static char *regnames_csfr[] =
{
    "pcxi"  , "psw"  , "pc"  , "syscon" , "cpu_id"  , "core_id",
};

typedef enum
{
    PCXI = 32,
    PSW,
    PC,
    SYSCON,
    CPU_ID,
    CORE_ID,
    MAX_REG_NUM
}TriCore_Reg_ID;

static void tricore_set_pc(conf_object_t* opaque, generic_address_t addr)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    core->PC = addr & (~1);

    return;
}

static generic_address_t tricore_get_pc(conf_object_t* opaque)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    return core->PC;
}

static uint64_t tricore_get_steps(conf_object_t* opaque)
{
    uint64_t steps = 0;
    return steps;
}

static endian_t tricore_get_endian(conf_object_t* opaque)
{
    return Little_endian;
}

static align_t tricore_get_alignment(conf_object_t* opaque)
{
    return Align;
}

static char* tricore_get_architecture(conf_object_t* opaque)
{
    return "TriCore";
}

static void tricore_reset(conf_object_t* opaque)
{
}

static char* tricore_disassemble(conf_object_t* opaque, generic_address_t addr)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    return skyeye_disassemble(opaque, addr);
}

static uint32_t tricore_get_address_width(conf_object_t* opaque)
{
    return 4;
}

static void tricore_get_cpu_run_status(conf_object_t* opaque, uint32_t* cpu_freq, uint64_t* cpu_insn_num)
{

}

static void tricore_set_memory_watch_status(conf_object_t* opaque, uint32_t addr, int status)
{

}

static int tricore_get_cpu_exception_num(conf_object_t* opaque)
{

}

static void tricore_set_cpu_exception_by_id(conf_object_t* opaque, int exception_id)
{

}

static char* tricore_get_cpu_exception_name_by_id(conf_object_t* opaque, int exception_id)
{

}
/********************************************
 * End implement tricore core_info_intf iface *
 ********************************************/

/***********************************************
 * Start implement tricore skyeye_reg_intf iface *
 ***********************************************/

static uint32_t tricore_get_regvalue_by_id(conf_object_t* opaque, uint32_t id)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    uint32_t regValue = 0;

    if (id < 16)
    {
        return core->gpr_a[id];
    }

    if (id >= 16 && id < 32)
    {
        return core->gpr_d[id];
    }

    switch (id)
    {
        case PCXI:
            regValue = core->PCXI;
            break;
        case PSW:
            regValue = core->PSW;
            break;
        case PC:
            regValue = core->PC;
            break;
        case SYSCON:
            regValue = core->SYSCON;
            break;
        case CPU_ID:
            regValue = core->CPU_ID;
            break;
        case CORE_ID:
            regValue = core->CORE_ID;
            break;
        default:
            printf("In %s, Not Define RegisterID:%d\n", __func__, id);
            break;
    }

    return regValue;
}

static char* tricore_get_regname_by_id(conf_object_t* opaque, uint32_t id)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    if (id < 16)
    {
        return regnames_a[id];
    }

    if (id >= 16 && id < 32)
    {
        return regnames_d[id - 16];
    }

    if (id >= PCXI && id <= CORE_ID)
    {
        return regnames_csfr[id - PCXI];
    }
    printf("In %s, Not Define RegisterID:%d\n", __func__, id);
    return "undef_reg";
}

static exception_t tricore_set_regvalue_by_id(conf_object_t* opaque, uint32_t value, uint32_t id)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    if (id < 16)
    {
        core->gpr_a[id] = value;
        return No_exp;
    }

    if (id >= 16 && id < 32)
    {
        core->gpr_d[id - 16] = value;
        return No_exp;
    }

    switch (id)
    {
        case PCXI:
            core->PCXI = value;
            break;
        case PSW:
            core->PSW = value;
            break;
        case PC:
            core->PC = value;
            break;
        case SYSCON:
            core->SYSCON = value;
            break;
        case CPU_ID:
            core->CPU_ID = value;
            break;
        case CORE_ID:
            core->CORE_ID = value;
            break;
        default:
            printf("In %s, Not Define RegisterID:%d\n", __func__, id);
            break;
    }

    return No_exp;
}

static uint32_t tricore_get_regid_by_name(conf_object_t* opaque, char* name)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    uint32_t regnum = MAX_REG_NUM;
    int i;

    for (i = 0; i < regnum; i++)
    {
        if (i < 16)
        {
            if (strcmp(name, regnames_a[i]) == 0)
                return i;
        } else if (i >= 16 && i < 32)
        {
            if (strcmp(name, regnames_d[i-16]) == 0)
                return i;
        } else
        {
            if (strcmp(name, regnames_csfr[i-PCXI]) == 0)
                return i;
        }
    }

    return regnum;
}

static uint32_t tricore_get_regnum(conf_object_t* opaque)
{
    return MAX_REG_NUM;
}
/*********************************************
 * End implement tricore skyeye_reg_intf iface *
 *********************************************/

/**********************************************
 * Start implement tricore core_exec_intf iface *
 **********************************************/

static uint32_t tricore_cpu_run(conf_object_t* opaque)
{
    uint32_t step = tricore_step_once(opaque);

    return step;
}

static void tricore_stepi(conf_object_t* opaque, uint64_t steps)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    core->stop_step = core->insn_num + 1;
}
/********************************************
 * End implement tricore core_exec_intf iface *
 ********************************************/

/************************************************
 * Start implement tricore memory_space_intf iface *
 ************************************************/
exception_t tricore_space_read(conf_object_t* opaque, generic_address_t addr, void *buf, size_t count)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    memory_space_intf *iface = core->memory_space.iface;

    return iface->read(core->memory_space.obj, addr, buf, count);
}

exception_t tricore_space_write(conf_object_t* opaque, generic_address_t addr, void *buf, size_t count)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    memory_space_intf *iface = core->memory_space.iface;

    return iface->write(core->memory_space.obj, addr, buf, count);
}
/**********************************************
 * End implement tricore memory_space_intf iface *
 **********************************************/

/************************************************
 * Start implement tricore core_signal_intf iface *
 ************************************************/
static exception_t tricore_signal(conf_object_t* opaque, interrupt_signal_t* signal)
{

}
/**********************************************
 * End implement tricore core_signal_intf iface *
 **********************************************/

/*******************************************************
 * Start implement tricore core_breakpoint_op_intf iface *
 *******************************************************/
static int tricore_insert_bp(conf_object_t* opaque, generic_address_t addr)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    int ret;

    ret = create_a_breakpoint(&core->bp_manager, addr);

    return ret;
}

static int tricore_delete_bp(conf_object_t* opaque, generic_address_t addr)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    int ret;

    ret = remove_bp_by_addr(&core->bp_manager, addr);

    return ret;
}

static uint32_t tricore_get_bp_numbers(conf_object_t* opaque)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    return get_bp_numbers(&core->bp_manager);
}

static uint32_t tricore_get_bp_addr_by_id(conf_object_t* opaque, int id)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);

    if (bp)
    {
        return bp->address;
    }

    return 0;
}

static int tricore_get_bp_trigger(conf_object_t* opaque)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    return get_breakpoint_hit_id(&core->bp_manager);
}

static bool_t tricore_check_bp_trigger(conf_object_t* opaque)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    int ret;

    ret = check_breakpoint_hit_flag(&core->bp_manager);
    if (ret == 0)
    {
        return True;
    } else
    {
        return False;
    }
}

static int tricore_clear_bp_trigger(conf_object_t* opaque)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    clear_breakpoint_hit_flag(&core->bp_manager);

    return 0;
}
/*****************************************************
 * End implement tricore core_breakpoint_op_intf iface *
 *****************************************************/

static attr_value_t get_attr_freq_hz(void *arg, conf_object_t* opaque, attr_value_t* idx)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    attr_value_t freq_hz = SKY_make_attr_uinteger(core->freq_hz);

    return freq_hz;
}

static exception_t set_attr_freq_hz(void *arg, conf_object_t* opaque, attr_value_t* value, attr_value_t* idx)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    core->freq_hz = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_dbgEnable(void *arg, conf_object_t* opaque, attr_value_t* idx)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    attr_value_t dbgEnable = SKY_make_attr_uinteger(core->dbgEnable);

    return dbgEnable;
}

static exception_t set_attr_dbgEnable(void *arg, conf_object_t* opaque, attr_value_t* value, attr_value_t* idx)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    core->dbgEnable = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_dump_filename(void *arg, conf_object_t* opaque, attr_value_t* idx)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    attr_value_t dbg_filename = SKY_make_attr_string(core->dbg_filename);

    return dbg_filename;
}

static exception_t set_attr_dump_filename(void *arg, conf_object_t* opaque, attr_value_t* value, attr_value_t* idx)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    core->dbg_filename = skyeye_strdup(SKY_attr_string(*value));

    core->dbg_fp = fopen(core->dbg_filename, "w+");

    if (core->dbg_fp == NULL)
    {
        printf("In %s, Open Filename:%s Failed!\n", __func__, core->dbg_filename);
    }
    return No_exp;
}

static void class_register_attribute(conf_class_t* clss)
{
    SKY_register_attribute(clss, "freq_hz", get_attr_freq_hz, NULL, set_attr_freq_hz, NULL,
            SKY_Attr_Optional, "uinteger", "Set tricore cpu freq_hz");
    SKY_register_attribute(clss, "dbgEnable", get_attr_dbgEnable, NULL, set_attr_dbgEnable, NULL,
            SKY_Attr_Optional, "uinteger", "enable tricore cpu dump registers info to file");
    SKY_register_attribute(clss, "dump_filename", get_attr_dump_filename, NULL, set_attr_dump_filename, NULL,
            SKY_Attr_Optional, "string", "Set tricore cpu dump filename");
}

exception_t memory_space_set(conf_object_t* opaque, conf_object_t* obj2, const char* port, int index)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    core->memory_space.obj = obj2;
    if (core->memory_space.obj == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "tricore Core Get 'memory_space' object Fail");
        return Not_found_exp;
    }
    core->memory_space.iface = (memory_space_intf *) SKY_get_iface(core->memory_space.obj, MEMORY_SPACE_INTF_NAME);

    if (core->memory_space.iface == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "tricore Core Get 'memory_space' interface from %s Fail", obj2->objname);
        return Not_found_exp;
    }
    return No_exp;
}

exception_t memory_space_get(conf_object_t* opaque, conf_object_t** obj2, char** port, int index)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    *obj2 = core->memory_space.obj;
    *port = NULL;
    return No_exp;
}

/**********************************************
 * Start implement tricore skyeye_class_t iface *
 **********************************************/

static conf_object_t *new_tricore_cpu(const char* obj_name)
{
    tricore_cpu_t *core = skyeye_mm_zero(sizeof (tricore_cpu_t));
    conf_object_t *obj = new_conf_object(obj_name, core);

    core->obj = obj;
    core->freq_hz = 133000000;
    core->dbgEnable = 0;

    return core->obj;
}

static exception_t free_tricore_cpu(conf_object_t* opaque)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    if (core->dbg_fp)
    {
        fclose(core->dbg_fp);
    }
    skyeye_free(core->obj);
    skyeye_free(core);

    return No_exp;
}

static exception_t reset_tricore_cpu(conf_object_t* opaque, const char* parameter)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    /* Reset Regs to Default Value */
    core->PSW = 0xb80;
    fpu_set_state(core);
    return No_exp;
}

static exception_t config_tricore_cpu(conf_object_t* opaque)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    if (core->dbg_fp == NULL)
    {
        core->dbg_fp = fopen("./dump_cpu_registers", "w+");
    }
    core->dump_state = tricore_cpu_dump_state;
    return No_exp;
}

/********************************************
 * End implement tricore skyeye_class_t iface *
 ********************************************/

void init_tricore_cpu()
{
    static skyeye_class_t class_data =
    {
        .class_name = "tricore_cpu",
        .class_desc = "Infineon AURIX TriCore CPU",
        .new_instance = new_tricore_cpu,
        .free_instance = free_tricore_cpu,
        .reset_instance = reset_tricore_cpu,
        .config_instance = config_tricore_cpu,
        .get_attr = NULL,
        .set_attr = NULL
    };

    conf_class_t *clss = SKY_register_cpu_class(class_data.class_name, &class_data);

    static const core_info_intf core_info =
    {
        .set_pc = tricore_set_pc,
        .get_pc = tricore_get_pc,
        .get_steps = tricore_get_steps,
        .get_endian = tricore_get_endian,
        .get_alignment = tricore_get_alignment,
        .get_address_width = tricore_get_address_width,
        .disassemble = tricore_disassemble,
        .get_architecture = tricore_get_architecture,
        .reset = tricore_reset,
        .get_cpu_run_status = tricore_get_cpu_run_status,
        .set_memory_watch_status = tricore_set_memory_watch_status,
        .get_cpu_exception_num = tricore_get_cpu_exception_num,
        .set_cpu_exception_by_id = tricore_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = tricore_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

    static const skyeye_reg_intf reg =
    {
        .get_regvalue_by_id = tricore_get_regvalue_by_id,
        .get_regname_by_id = tricore_get_regname_by_id,
        .set_regvalue_by_id = tricore_set_regvalue_by_id,
        .get_regid_by_name = tricore_get_regid_by_name,
        .get_regnum = tricore_get_regnum,
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg);

    static const core_exec_intf core_exec =
    {
        .run = tricore_cpu_run,
        .stepi = tricore_stepi,
        .stop = NULL,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const core_signal_intf core_signal =
    {
        .signal = tricore_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    static const core_breakpoint_op_intf bp_op =
    {
        .insert_bp = tricore_insert_bp,
        .delete_bp = tricore_delete_bp,
        .get_bp_numbers = tricore_get_bp_numbers,
        .get_bp_addr_by_id = tricore_get_bp_addr_by_id,
        .get_bp_trigger = tricore_get_bp_trigger,
        .check_bp_trigger = tricore_check_bp_trigger,
        .clear_bp_trigger = tricore_clear_bp_trigger,
    };
    SKY_register_iface(clss, CORE_BREAKPOINT_OP_INTF_NAME, &bp_op);

    static const memory_space_intf memory_space =
    {
        .read = tricore_space_read,
        .write = tricore_space_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription) {
            .name = CORE_INFO_INTF_NAME,
            .iface = &core_info,
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
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = memory_space_set,
            .get = memory_space_get,
        }
    };

    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    class_register_attribute(clss);
}

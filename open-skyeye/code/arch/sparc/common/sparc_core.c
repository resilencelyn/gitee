/*
 * sparc_core.c:
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
#include <memory_space.h>
#include <skyeye_attribute.h>
#include "skyeye_mm.h"
#include "skyeye_iface.h"
#include "sparc.h"
#include "iu.h"
#include "sparc_arch_interface.h"
#include "sim_control.h"
#include "skyeye_callback.h"
#include "skyeye_disassemble.h"

extern iu_config_t *iu;
trap_handle_t *traps;

/*
 *
 * ===  FUNCTION  ======================================================================
 *         Name:  sparc_set_pc
 *  Description:  This function sets the Program Counter
 * =====================================================================================
 */
void sparc_set_pc(conf_object_t * conf_obj, generic_address_t addr)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;

    iu->iu_set_pc(conf_obj, addr);
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  sparc_get_pc
 *  Description:  This function returns the Program Counter
 * =====================================================================================
 */
generic_address_t sparc_get_pc(conf_object_t * conf_obj)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;

    return (iu->iu_get_pc(conf_obj));
}

exception_t sparc_signal(conf_object_t * conf_obj, interrupt_signal_t * signal)
{
    int trap_no;
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj;

    sparc_signal_t *sparc_signal = &signal->sparc_signal;

    trap_no = sparc_signal->irq_no;
    return traps->irq_signal(sparc_state, trap_no);
}

void sparc_stepi(conf_object_t * conf_obj, uint64_t steps)
{

    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;

    sparc_state->stop_step = sparc_state->steps + steps;
    return;
}

void sparc_power(conf_object_t * conf_obj, int run)
{

    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;

    sparc_state->run_flag = run;

    sparc_state->pc = sparc_state->reset_pc;
    sparc_state->npc = sparc_state->reset_npc;
    return;
}

void sparc_stepi_check(conf_object_t * conf_obj)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;

    if (sparc_state->stop_step == sparc_state->steps && sparc_state->stop_step != 0 && sparc_state->steps != 0)
    {
        SIM_stop(NULL);
        sparc_state->stop_step = 0;

    }
    return;
}

int sparc_breakpoint_check(conf_object_t * conf_obj)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;
    uint32_t pc = sparc_state->pc;

    check_breakpoint(&(sparc_state->bp_manager), pc, sparc_state->steps);
    return 0;
}

exception_t sparc_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{

    sparc_state_t *sparc_state = (sparc_state_t *) obj->obj;
    memory_space_intf *iface = sparc_state->memory_space_iface;

    return iface->read(sparc_state->memory_space, addr, buf, count);
}

exception_t sparc_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{
    sparc_state_t *sparc_state = (sparc_state_t *) obj->obj;
    memory_space_intf *iface = sparc_state->memory_space_iface;

    return iface->write(sparc_state->memory_space, addr, buf, count);
}

get_page_t *sparc_get_page(conf_object_t * obj, generic_address_t addr)
{
    sparc_state_t *sparc_state = (sparc_state_t *) obj->obj;
    memory_space_intf *iface = sparc_state->memory_space_iface;

    return iface->get_page(sparc_state->memory_space, addr);
}

void init_tbl_cache(conf_object_t * obj)
{
    sparc_state_t *sparc_state = (sparc_state_t *) obj->obj;
    int i;
    for (i = 0; i < SPARC_TLB_SIZE; i++)
    {
        memset(&sparc_state->tlb_cache[i], 0, sizeof(sparc_tlb_item_t));
    }
}

conf_object_t *new_sparc(const char *obj_name)
{
    sparc_state_t *core = skyeye_mm_zero(sizeof (sparc_state_t));
    conf_object_t *obj = new_conf_object(obj_name, core);

    core->obj = obj;
    core->core_obj = obj;

    core->debug_mode = 0;
    core->mode = INTERPRET_RUN;
    core->freq_hz = 25000000;
    core->wake_flag = 0;
    core->run_flag = 1;
    core->asr_regs[17] = 0x7; // Indicates N_WINDOW - 1
    core->leon_version = 0;
    sparc_init_state(obj);

    init_tbl_cache(obj);
    return core->obj;
}

exception_t free_sparc(conf_object_t * obj)
{
    sparc_state_t *sparc_state = (sparc_state_t *) obj->obj;

    if (sparc_state->mode == HYBRID_RUN || sparc_state->mode == DYNCOM_RUN)
    {
        // sparc_dyncom_fini(sparc_state);
    }

    skyeye_free(sparc_state->sparc_memory_iface);
    skyeye_free(sparc_state->obj);
    skyeye_free(sparc_state);

    return No_exp;
}

exception_t config_sparc(conf_object_t * obj)
{
    sparc_state_t *core = (sparc_state_t *) obj->obj;

    core->psr &= 0xF0FFFFFF;
    switch (core->leon_version)
    {
        case 4:
            core->psr |= 0x04000000;
            break;
        case 3:
            core->psr |= 0x03000000;
            break;
        case 2:
        default:
            core->psr |= 0x02000000;
            break;
    }

    init_breakpoint_manager(&core->bp_manager);
#ifdef LLVM_EXIST
    if (core->mode != 0)
    {
        sparc_dyncom_init(core);
    }
#endif

    core->sparc_memory_iface = skyeye_mm_zero(sizeof (memory_space_intf));
    core->sparc_memory_iface->read = sparc_space_read;
    core->sparc_memory_iface->write = sparc_space_write;
    core->sparc_memory_iface->get_page = sparc_get_page;
    return No_exp;
}

exception_t reset_sparc(conf_object_t * conf_obj, const char *parameter)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;

    return No_exp;
}

char *sparc_get_architecture(conf_object_t * obj)
{
    return "sparc";
}

int sparc_insert_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    sparc_state_t *core = (sparc_state_t *) conf_obj->obj;
    int ret;

    ret = create_a_breakpoint(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    /*
     * if(core->mode == DYNCOM_RUN){
     * insert_dyncom_bp(core, addr);
     * }
     */
    return 0;

}

int sparc_delete_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    sparc_state_t *core = (sparc_state_t *) conf_obj->obj;
    int ret;

    ret = remove_bp_by_addr(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    /*
     * if(core->mode == DYNCOM_RUN){
     * delete_dyncom_bp(core, addr);
     * }
     */
    return 0;
}

uint32_t sparc_get_bp_numbers(conf_object_t * conf_obj)
{
    sparc_state_t *core = (sparc_state_t *) conf_obj->obj;

    return get_bp_numbers(&core->bp_manager);
}

uint32_t sparc_get_bp_addr_by_id(conf_object_t * conf_obj, int id)
{
    sparc_state_t *core = (sparc_state_t *) conf_obj->obj;
    breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);

    if (bp)
        return bp->address;
    return 0;
}

int sparc_get_trigger(conf_object_t * conf_obj)
{
    sparc_state_t *core = (sparc_state_t *) conf_obj->obj;

    return get_breakpoint_hit_id(&core->bp_manager);
}

bool_t sparc_check_bp_trigger(conf_object_t * conf_obj)
{
    sparc_state_t *core = (sparc_state_t *) conf_obj->obj;
    int ret;

    ret = check_breakpoint_hit_flag(&core->bp_manager);
    if (ret == 0)
        return True;
    return False;
}

int sparc_clear_bp_trigger(conf_object_t * conf_obj)
{
    sparc_state_t *core = (sparc_state_t *) conf_obj->obj;

    clear_breakpoint_hit_flag(&core->bp_manager);
    return 0;
}

uint64_t sparc_get_steps(conf_object_t * conf_obj)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;

    return sparc_state->steps;
}

uint32_t sparc_get_regvalue_by_id(conf_object_t * conf_obj, uint32_t id)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;

    if (id < 32)
        return REG(id);
    else if (id < 64)
        return FPREG(id - 32);
    switch (id - 64)
    {
        case 0:
            return YREG;
        case 1:
            return PSRREG;
        case 2:
            return WIMREG;
        case 3:
            return TBRREG;
        case 4:
            return PCREG;
        case 5:
            return NPCREG;
        case 6:
            return FPSRREG;
        case 7:
            return CPSRREG;
        case 8:
            return CWP;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Have not register id %d\n", id);
            return -1;
    }
    return 0;
}

exception_t sparc_set_regvalue_by_id(conf_object_t * conf_obj, uint32 value, uint32 id)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;

    if (id < 32)
        REG(id) = value;
    else if (id < 64)
        FPREG(id - 32) = value;
    else
    {
        switch (id - 64)
        {
            case 0:
                YREG = value;
                break;
            case 1:
                PSRREG = value;
                break;
            case 2:
                WIMREG = value;
                break;
            case 3:
                TBRREG = value;
                break;
            case 4:
                PCREG = value;
                NPCREG = value + 4;
                break;
            case 5:
                NPCREG = value;
                break;
            case 6:
                FPSRREG = value;
                break;
            case 7:
                CPSRREG = value;
                break;
            default:
                skyeye_log(Error_log, __FUNCTION__, "Have not register id %d\n", id);
                return Dll_open_exp;
        }
    }
    return No_exp;
}

uint32_t sparc_core_run(conf_object_t * conf_obj)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;
    uint32_t steps = sparc_state->steps;

    if (sparc_state->wake_flag)
    {
        if (sparc_state->run_flag == 0)
            return;
    }

    SKY_pc_hap_occurred(NULL, conf_obj, sparc_state->pc);
    if (sparc_state->mode == DYNCOM_RUN || sparc_state->mode == HYBRID_RUN)
    {
        //sparc_dyncom_step_once(conf_obj);
    } else if (sparc_state->mode == INTERPRET_RUN || sparc_state->mode == DYNCOM_RUN_SINGLE_STEP)
    {
        sparc_run(conf_obj);
    } else
    {
        /* invalid mode */
        return;
    }
    sparc_stepi_check(conf_obj);
    sparc_breakpoint_check(conf_obj);
    if (sparc_state->stop_step == 0 && sparc_state->mode == DYNCOM_RUN_SINGLE_STEP)
    {
        /* we reach the stop steps */
        sparc_state->mode = DYNCOM_RUN;
        //printf("in %s, change mode to %d\n", __FUNCTION__, sparc_state->mode);
    }
    return sparc_state->steps - steps;
}

uint32 sparc_get_regid_by_name(conf_object_t * conf_obj, char *name)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;
    int i = 0;
    int reg_num = sparc_get_regnum(conf_obj);
    char *reg_name;

    for (i = 0; i < reg_num + 1; i++)
    {
        reg_name = sparc_get_regname_by_id(conf_obj, i);
        if (strcmp(name, reg_name) == 0)
            return i;
    }
    skyeye_log(Error_log, __FUNCTION__, "Can't find reg name %s\n", name);
    return -1;
}

char *sparc_disassemble(conf_object_t * obj, generic_address_t addr)
{
    return skyeye_disassemble(obj, addr);
}

uint32_t *sparc_get_reg_window(conf_object_t * obj)
{
    sparc_state_t *sparc_state = (sparc_state_t *) obj->obj;

    return sparc_state->regbase;
}

DEF_UINTEGER_ATTR_FUNC(sparc_state_t, y);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, psr);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, wim);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, tbr);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, pc);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, npc);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, cpsr);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, fpsr);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, ctrl);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, ctp);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, ctxt);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, fsr);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, far_reg);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, ncwp);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, trap);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, trap_impl);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, trap_arg0);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, trap_arg1);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, trap_arg2);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, trap_delay);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, cwp);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, pipeline);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, sparc_exp);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, irq_pending);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, irq_mask);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, steps);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, mode);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, debug_mode);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, freq_hz);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, wake_flag);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, run_flag);
DEF_UINTEGER_ATTR_FUNC(sparc_state_t, leon_version);
DEF_LIST_UINTEGER_ATTR_FUNC(sparc_state_t, global, 8);
DEF_LIST_UINTEGER_ATTR_FUNC(sparc_state_t, regw, 24);
DEF_LIST_UINTEGER_ATTR_FUNC(sparc_state_t, regbase, MAX_NWINDOWS * 16 + 8);
DEF_LIST_UINTEGER_ATTR_FUNC(sparc_state_t, fp_regs, 32);
DEF_LIST_UINTEGER_ATTR_FUNC(sparc_state_t, asr_regs, 32);

attr_value_t get_attr_int_regw_index(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    sparc_state_t *core = (sparc_state_t *) (conf_obj->obj);
    uint32_t index = (uint32_t) ((core->regwptr[2] - core->regbase) / 16);
    attr_value_t int_number = SKY_make_attr_uinteger(index);

    return int_number;
}

exception_t set_attr_int_regw_index(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    sparc_state_t *core = (sparc_state_t *) (conf_obj->obj);

    core->regw_index = SKY_attr_uinteger(*value);
    core->regwptr[1] = &core->regw[0];
    core->regwptr[2] = &core->regbase[core->regw_index * 16];
    return No_exp;
}

attr_value_t get_attr_cpu_id(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    sparc_state_t *core = (sparc_state_t *) (conf_obj->obj);
    attr_value_t cpu_id = SKY_make_attr_uinteger(core->cpu_id);

    return cpu_id;
}

exception_t set_attr_cpu_id(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    sparc_state_t *core = (sparc_state_t *) (conf_obj->obj);

    core->cpu_id = SKY_attr_uinteger(*value);
    core->asr_regs[17] |= core->cpu_id << 28;
    return No_exp;
}

void sparc_register_attribute(conf_class_t * clss)
{
    DEF_DISABLE_SHOW_ATTR(y, uinteger, "sparc core y register");
    DEF_DISABLE_SHOW_ATTR(psr, uinteger, "sparc core psr register");
    DEF_DISABLE_SHOW_ATTR(wim, uinteger, "sparc core wim register");
    DEF_DISABLE_SHOW_ATTR(tbr, uinteger, "sparc core tbr register");
    DEF_ATTR(pc, uinteger, "sparc core pc register");
    DEF_DISABLE_SHOW_ATTR(npc, uinteger, "sparc core npc register");
    DEF_DISABLE_SHOW_ATTR(cpsr, uinteger, "sparc core cpsr register");
    DEF_DISABLE_SHOW_ATTR(fpsr, uinteger, "sparc core fpsr register");
    DEF_DISABLE_SHOW_ATTR(ctrl, uinteger, "sparc core ctrl register");
    DEF_DISABLE_SHOW_ATTR(ctp, uinteger, "sparc core ctp register");
    DEF_DISABLE_SHOW_ATTR(ctxt, uinteger, "sparc core ctxt");
    DEF_DISABLE_SHOW_ATTR(fsr, uinteger, "sparc core fsr register");
    DEF_DISABLE_SHOW_ATTR(far_reg, uinteger, "sparc core far_reg");
    DEF_DISABLE_SHOW_ATTR(ncwp, uinteger, "sparc core ncwp");
    DEF_DISABLE_SHOW_ATTR(trap, uinteger, "sparc core trap");
    DEF_DISABLE_SHOW_ATTR(trap_impl, uinteger, "sparc core trap_impl");
    DEF_DISABLE_SHOW_ATTR(trap_arg0, uinteger, "sparc core trap_arg0");
    DEF_DISABLE_SHOW_ATTR(trap_arg1, uinteger, "sparc core trap_arg1");
    DEF_DISABLE_SHOW_ATTR(trap_arg2, uinteger, "sparc core trap_arg2");
    DEF_DISABLE_SHOW_ATTR(trap_delay, uinteger, "sparc core trap_delay");
    DEF_DISABLE_SHOW_ATTR(cwp, uinteger, "sparc core cwp");
    DEF_DISABLE_SHOW_ATTR(pipeline, uinteger, "sparc core pipeline");
    DEF_DISABLE_SHOW_ATTR(sparc_exp, uinteger, "sparc core sparc_exp");
    DEF_DISABLE_SHOW_ATTR(irq_pending, uinteger, "sparc core irq_pending");
    DEF_DISABLE_SHOW_ATTR(irq_mask, uinteger, "sparc core irq_mask");
    DEF_DISABLE_SHOW_ATTR(steps, uinteger, "sparc core steps");
    DEF_DISABLE_SHOW_ATTR(global, list, "sparc core global");

    DEF_DISABLE_SHOW_ATTR(regw, list, "sparc core regw");
    DEF_DISABLE_SHOW_ATTR(regbase, list, "sparc core regbase");
    DEF_DISABLE_SHOW_ATTR(fp_regs, list, "sparc core fp_regs");
    DEF_DISABLE_SHOW_ATTR(asr_regs, list, "sparc core asr_regs");
    DEF_ATTR(mode, uinteger, "sparc core mode");
    DEF_ATTR(debug_mode, uinteger, "sparc core debug_mode");
    DEF_ATTR(freq_hz, uinteger, "sparc core freq_hz");
    DEF_ATTR(wake_flag, uinteger, "sparc core wake flag");
    DEF_ATTR(run_flag, uinteger, "sparc core run flag");
    DEF_ATTR(leon_version, uinteger, "LEON IP Core version");

    SKY_register_attribute(clss, "regw_index", get_attr_int_regw_index, NULL,
                           set_attr_int_regw_index, NULL, SKY_Attr_Optional|SKY_Attr_Type_disable_show,
                           "uinteger", "which window should regwptr pointed");
    SKY_register_attribute(clss, "cpu_id", get_attr_cpu_id, NULL,
                           set_attr_cpu_id, NULL, SKY_Attr_Optional,
                           "uinteger", "set sparc core cpu id");


    return;
}

exception_t memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    sparc_state_t *sparc_state = (sparc_state_t *) obj->obj;

    sparc_state->memory_space = obj2;
    if (sparc_state->memory_space == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "set 'memory space' iface Error");
    }
    sparc_state->memory_space_iface =
        (memory_space_intf *) SKY_get_iface(sparc_state->memory_space, MEMORY_SPACE_INTF_NAME);
    if (sparc_state->memory_space_iface == NULL)
        skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' interface from %s fail.",
                   sparc_state->memory_space->objname);
    return No_exp;
}

exception_t memory_space_get(conf_object_t * obj, conf_object_t ** obj2, char **port, int index)
{
    sparc_state_t *sparc_state = (sparc_state_t *) obj->obj;

    *obj2 = sparc_state->memory_space;
    *port = NULL;
    return No_exp;
}

uint32_t sparc_get_address_width(conf_object_t * obj)
{
    return 4;
}

int sparc_get_cpu_exception_num(conf_object_t * obj)
{
    sparc_state_t *core = (sparc_state_t *) obj->obj;

    return UPDATE_CWP;
}

extern trap_handle_t *traps;
void sparc_set_cpu_exception_by_id(conf_object_t * obj, int exception_id)
{
    sparc_state_t *sparc_state = (sparc_state_t *) obj->obj;

    if (exception_id < UPDATE_CWP)
    {
        set_bit(PSRREG, PSR_ET);
        sparc_state->trap = exception_id;
        traps->trigger(obj, sparc_state->trap);
        sparc_state->trap = TRAP_IMPL_MAX;
    }

    return;
}

char *sparc_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id)
{
    sparc_state_t *core = (sparc_state_t *) obj->obj;

    strcpy(core->exception_name, "TRAP");

    assert(exception_id < UPDATE_CWP);
    sprintf(core->exception_name, "%s%d", core->exception_name, exception_id);
    return core->exception_name;
}

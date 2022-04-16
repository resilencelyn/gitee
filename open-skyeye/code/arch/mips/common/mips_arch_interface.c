#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "cache.h"
#include "cpu.h"
#include "emul.h"
#include "instr.h"
#include "inttypes.h"
#include "mips_regformat.h"
#include "mips_cpu.h"
#include "types.h"

#include "skyeye_disassemble.h"
#include <skyeye_class.h>
#include <skyeye_core_intf.h>
#include <skyeye_int_register.h>
#include <skyeye_signal.h>
#include <memory_space.h>
#include <skyeye_iface.h>
#include <skyeye_attribute.h>
#include "skyeye_types.h"
#include "skyeye_config.h"
#include "skyeye_callback.h"
#include "skyeye_core.h"
#include "skyeye.h"
#include "skyeye_exec.h"
#include "skyeye_mm.h"
#include "skyeye_cell.h"
#include "skyeye_ram.h"
#include <sim_control.h>
#include <skyeye_arch.h>
#include "skyeye_options.h"
#include "skyeye_pref.h"
#include "bank_defs.h"

extern FILE *skyeye_logfd;

/* Interrupt Controller */
typedef struct int_ctrl_s
{
    uint32_t cfg0rd;
    uint32_t cfg0set;
    uint32_t cfg0clr;
    uint32_t cfg1rd;
    uint32_t cfg1set;
    uint32_t cfg1clr;
    uint32_t cfg2rd;
    uint32_t cfg2set;
    uint32_t cfg2clr;
    uint32_t req0int;
    uint32_t srcrd;
    uint32_t srcset;
    uint32_t srcclr;
    uint32_t req1int;
    uint32_t assignrd;
    uint32_t assignset;
    uint32_t assignclr;
    uint32_t wakerd;
    uint32_t wakeset;
    uint32_t wakeclr;
    uint32_t maskrd;
    uint32_t maskset;
    uint32_t maskclr;
    uint32_t risingrd;
    uint32_t risingclr;
    uint32_t fallingrd;
    uint32_t fallingclr;
    uint32_t testbit;
} int_ctrl_t;

typedef struct gs32eb1_io_s
{
    int_ctrl_t int_ctrl[2];
} gs32eb1_io_t;

static gs32eb1_io_t io;

void mips_set_pc(conf_object_t * conf_obj, generic_address_t addr)
{
    MIPS_CPU_State *cpu = get_current_cpu();
    int i;

#if 0
    for (i = 0; i < cpu->core_num; i++)
        cpu->core[i].pc = addr;
#endif
    cpu->core[0].pc = addr;
}

UInt32 mips_get_pc()
{
    MIPS_CPU_State *cpu = get_current_cpu();

    return cpu->core[0].pc;
}

uint64_t mips_get_step(conf_object_t * conf_obj)
{
    mips_core_t *core = conf_obj->obj;

    return core->insn_num;
}

endian_t mips_get_endian(conf_object_t * obj)
{

    return Little_endian;
}

align_t mips_get_alignment(conf_object_t * obj)
{
    return UnAlign;
}

uint32_t mips_get_address_width(conf_object_t * obj)
{
    return 4;
}

char *mips_disassemble(conf_object_t * obj, generic_address_t addr)
{
    mips_core_t *core = (mips_core_t *) obj->obj;

    return skyeye_disassemble(obj, addr);
}

char *mips_get_architecture(conf_object_t * obj)
{
    mips_core_t *core = (mips_core_t *) obj->obj;

    return "mips";
}

void mips_set_textsection_info(conf_object_t * obj, generic_address_t code_start_addr, generic_address_t code_end_addr)
{
    mips_core_t *core = obj->obj;

    core->code_start_addr = code_start_addr;
    core->code_end_addr = code_end_addr;
    printf("%x %x for text section\n", code_start_addr, code_end_addr);

}

uint32 mips_get_regval_by_id(conf_object_t * conf_obj, uint32_t id)
{
    MIPS_CPU_State *cpu = get_current_cpu();

    if (id == PC)
        return cpu->core[0].pc;
    return cpu->core[0].gpr[id];
}

char *mips_get_regname_by_id(conf_object_t * conf_obj, uint32_t id)
{
    return mips_regstr[id];
}

exception_t mips_set_regval_by_id(conf_object_t * conf_obj, uint32_t value, uint32_t id)
{
    MIPS_CPU_State *cpu = get_current_cpu();

    if (id == PC)
    {
        cpu->core[0].pc = value;
    }
    else
    {
        cpu->core[0].gpr[id] = value;
    }
    return No_exp;
}

UInt32 mips_get_regid_by_name(conf_object_t * conf_obj, char *name)
{
    return 0;
}

int mips_get_regnum()
{
    return 32;
}

/**
* @brief  gs32eb1 Trigger a irq and set
*
* @param mstate
*/
void gs32eb1_trigger_irq(mips_core_t * mstate)
{
    VA epc;

    //Get the content of the cause register
    UInt32 cause = mstate->cp0[Cause];

    //When the instruction is in the delay slot, we have to delay an instruction
    if (!branch_delay_slot(mstate))
        epc = mstate->pc;
    else
    {
        epc = mstate->pc - 4;
        cause = set_bit(cause, Cause_BD);
    }
    mstate->cp0[Cause] = cause;
    mstate->cp0[EPC] = epc;

    //Change the pointer pc to deal with the interrupt handler
    if (bit(mstate->cp0[SR], SR_BEV))
    {
        mstate->pc = 0xbfc00380;
    } else
    {
        mstate->pc = 0x80000180;
    }

    mstate->pipeline = nothing_special;
}

/**
* @brief mips read mem interface
*
* @param pa	physic addr
* @param data  data pointer
* @param len	data length
*/
exception_t mips_mem_read(conf_object_t * obj, UInt32 pa, void *data, int len)
{
    mips_core_t *core = (mips_core_t *) obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    /* if pa is located at kseg0 */
    if (pa >= 0x80000000 && pa < 0xA0000000)
        pa = pa & ~0x80000000;
    /* if pa is located at kseg1 */
    if (pa >= 0xA0000000 && pa < 0xC0000000)
        pa = pa & ~0xE0000000;

    return iface->read(core->memory_space, pa, data, len);
}

/**
* @brief mips write memory interface
*
* @param pa	physic addr
* @param data	data pointer
* @param len	data length
*/
exception_t mips_mem_write(conf_object_t * obj, UInt32 pa, void *data, int len)
{
    mips_core_t *core = (mips_core_t *) obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    /* if pa is located at kseg0 */
    if (pa >= 0x80000000 && pa < 0xA0000000)
        pa = pa & ~0x80000000;
    /* if pa is located at kseg1 */
    if (pa >= 0xA0000000 && pa < 0xC0000000)
        pa = pa & ~0xE0000000;

    return iface->write(core->memory_space, pa, data, len);
}

/**
* @brief cpu stop interface.
*
* @param running_core	core state
*/
void mips_stop(conf_object_t * running_core)
{
    mips_core_t *core = (mips_core_t *) get_cast_conf_obj(running_core, "mips_core_t");
}

void check_cpu_timer_interrupt(mips_core_t * mstate)
{
    /* if timer int is not mask and counter value is equal to compare value */
    mstate->cp0[Count]++;
    if (mstate->cp0[Count] == mstate->cp0[Compare])
    {
        /* update counter value in cp0 */
        mstate->cp0[Count] = 0;

        /* if interrupt is enabled? IP1 为CPU 定时器中断 */
        if ((mstate->cp0[SR] & (1 << SR_IEC)) && (mstate->cp0[SR] & (1 << SR_IM1)))
        {
            if (!(mstate->cp0[Cause] & (1 << Cause_IP1)))
            {
                //fprintf(stderr, "counter=0x%x,pc=0x%x\n", mstate->cp0[Count], mstate->pc);
                /* Set counter interrupt bit in IP section of Cause register */
                mstate->cp0[Cause] |= 1 << Cause_IP1;
                mstate->cp0[Cause] |= 1 << Cause_TI;    /* for release 2 */
                /* Set ExcCode to zero in Cause register */
                process_exception(mstate, EXC_Int, reset_vector);
            }
        }
    }
    return;
}

/**
* @brief  ls132r Trigger a irq and set
*
* @param mstate
*/
void ls132r_trigger_irq(mips_core_t * mstate)
{
    UInt32 cause = mstate->cp0[Cause];
    VA epc;

    //When the instruction is in the delay slot, we have to delay an instruction
    if (!branch_delay_slot(mstate))
        epc = mstate->pc;
    else
    {
        epc = mstate->prev_pc;
        cause = set_bit(cause, Cause_BD);
    }
    mstate->cp0[Cause] = cause;
    if (mstate->isa_mode)
        epc = set_bit(epc, 0);
    mstate->cp0[EPC] = epc;

    //Change the pointer pc to deal with the interrupt handler
    if (bit(mstate->cp0[SR], SR_BEV))
    {
        mstate->isa_mode = 0;
        mstate->pc = 0xbf000380;
    } else
    {
        printf("[ERROR]:In %s (line:%d) Can't Get Interrupt Entry Address!\n", __func__, __LINE__);
        //mstate->pc += 4;
    }

    mstate->pipeline = nothing_special;
    mstate->irq_pending = 0;
}

void ls132r_step_once(conf_object_t * conf_obj)
{
    mips_core_t *mstate = (mips_core_t *) conf_obj->obj;
    int next_state, instr;

    /* if active is 0, the core is suspend */
    if (!mstate->active)
        return;

    /* Check for interrupts. In real hardware, these have a priority lower
     * than all exceptions, but simulating this effect is too hard to be
     * worth the effort (interrupts and resets are not meant to be
     * delivered accurately anyway.)
     */
    mstate->cycle++;
    if (mstate->irq_pending)
    {
        if (mstate->cp0[SR] & (1 << SR_IEC) && (mstate->cp0[SR] & 1 << SR_IM7))
        {
            ls132r_trigger_irq(mstate);
        }
    }

    SKY_pc_hap_occurred(NULL, conf_obj, mstate->pc);
    int inst_size = mstate->isa_mode ? 2 : 4;
    VA prev_pc = mstate->pc;

    mstate->core_memory_iface->read(conf_obj, mstate->pc, &instr, 4);
    //printf("pc : 0x%x | instr : 0x%x | isa_mode : 0x%x\n", mstate->pc, instr, mstate->isa_mode);
    if (mstate->isa_mode)
        next_state = decode16(mstate, instr);
    else
        next_state = decode(mstate, instr);

    switch (mstate->pipeline)
    {
        case nothing_special:
            mstate->pc += inst_size;
            break;
        case branch_delay:
            mstate->isa_mode = bit(mstate->branch_target, 0);
            mstate->pc = clear_bit(mstate->branch_target, 0);
            break;
        case instr_addr_error:
            process_address_error(mstate, instr_fetch, mstate->branch_target);
        case branch_nodelay:           /* For syscall and TLB exp, we donot like to add pc */
            mstate->insn_num++;
            // save previous instruction address which used for branch in delay slot.
            mstate->prev_pc = prev_pc;
            mstate->pipeline = nothing_special;
            return;                     /* do nothing */
    }
    mstate->pipeline = next_state;
    mstate->insn_num++;
    // save previous instruction address which used for branch in delay slot.
    mstate->prev_pc = prev_pc;
    check_cpu_timer_interrupt(mstate);
}

/**
* @brief cpu exec one step
*
* @param running_core core state
*/
void gs32eb1_step_once(conf_object_t * conf_obj)
{
    mips_core_t *mstate = (mips_core_t *) conf_obj->obj;

    mstate->gpr[0] = 0;

    /* if active is 0, the core is suspend */
    if (!mstate->active)
        return;
    /* Check for interrupts. In real hardware, these have a priority lower
     * than all exceptions, but simulating this effect is too hard to be
     * worth the effort (interrupts and resets are not meant to be
     * delivered accurately anyway.)
     */
    if (mstate->irq_pending)
    {
        gs32eb1_trigger_irq(mstate);
    }

    /* Look up the ITLB. It's not clear from the manuals whether the ITLB
     * stores the ASIDs or not. I assume it does. ITLB has the same size
     * as in the real hardware, mapping two 4KB pages.  Because decoding a
     * MIPS64 virtual address is far from trivial, ITLB and DTLB actually
     * improve the simulator's performance: something I cannot say about
     * caches and JTLB.
     */

    PA pa;                              //Shi yang 2006-08-18
    VA va;
    Instr instr;
    int next_state;

    va = mstate->pc;
    mstate->cycle++;

    if (translate_vaddr(mstate, va, instr_fetch, &pa) == TLB_SUCC)
    {
        SKY_pc_hap_occurred(NULL, conf_obj, mstate->pc);
        mips_mem_read(conf_obj, pa, &instr, 4);
        //printf("pa = %x, pc = %x instr = %x\n", pa, mstate->pc, instr);
        next_state = decode(mstate, instr);
        //skyeye_exit(-1);
    } else
    {
        //fprintf(stderr, "Exception when get instruction!\n");
    }

    /* NOTE: mstate->pipeline is also possibely set in decode function */
    if (skyeye_logfd)
        //fprintf(skyeye_logfd, "KSDBG:instr=0x%x,pa=0x%x, va=0x%x, sp=0x%x, ra=0x%x,s1=0x%x, v0=0x%x\n", instr, pa, va, mstate->gpr[29], mstate->gpr[31],mstate->gpr[17], mstate->gpr[2]);
        fprintf(skyeye_logfd,
                "KSDBG:instr=0x%x,pa=0x%x, va=0x%x, a0=0x%x, k1=0x%x, t0=0x%x, ra=0x%x, s4=0x%x, gp=0x%x\n", instr, pa,
                va, mstate->gpr[4], mstate->gpr[27], mstate->gpr[8], mstate->gpr[31], mstate->gpr[20], mstate->gpr[28]);
    //fprintf(skyeye_logfd, "KSDBG:instr=0x%x,pa=0x%x, va=0x%x,v0=0x%x,t0=0x%x\n", instr, pa, va, mstate->gpr[2], mstate->gpr[8]);

    switch (mstate->pipeline)
    {
        case nothing_special:
            mstate->pc += 4;
            break;
        case branch_delay:
            mstate->pc = mstate->branch_target;
            break;
        case instr_addr_error:
            process_address_error(mstate, instr_fetch, mstate->branch_target);
        case branch_nodelay:           /* For syscall and TLB exp, we donot like to add pc */
            mstate->pipeline = nothing_special;
            return;                     /* do nothing */
    }
    mstate->pipeline = next_state;
    mstate->insn_num++;
    /* if timer int is not mask and counter value is equal to compare value */
    if (mstate->cp0[Count]++ >= mstate->cp0[Compare])
    {
        /* update counter value in cp0 */
        mstate->cp0[Count] = 0;

        /* if interrupt is enabled? */
        if ((mstate->cp0[SR] & (1 << SR_IEC)) && (mstate->cp0[SR] & 1 << SR_IM7))
        {
            if (!(mstate->cp0[Cause] & 1 << Cause_IP7) && (!(mstate->cp0[SR] & 0x2)))
            {
                //fprintf(stderr, "counter=0x%x,pc=0x%x\n", mstate->cp0[Count], mstate->pc);
                /* Set counter interrupt bit in IP section of Cause register */
                mstate->cp0[Cause] |= 1 << Cause_IP7;
                mstate->cp0[Cause] |= 1 << Cause_TI;    /* for release 2 */
                /* Set ExcCode to zero in Cause register */
                process_exception(mstate, EXC_Int, common_vector);
            }
        }
    }
}

uint64_t gs32eb1_run(conf_object_t * conf_obj)
{
    mips_core_t *mstate = (mips_core_t *) conf_obj->obj;
    uint64_t steps = mstate->insn_num;

    //if(mstate->mode == DYNCOM_RUN)
    //mips_dyncom_step_once(conf_obj);
    //else if(mstate->mode == HYBRID_RUN){
    //mips_hybrid_run(conf_obj);
    //}
    //else if(mstate->mode == INTERPRET_RUN){
    gs32eb1_step_once(conf_obj);
    //}
    //else{
    /* invalid mode */
    //      return;
    //}

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

void mips_stepi(conf_object_t * obj, uint64_t steps)
{
    mips_core_t *core = obj->obj;

    core->stop_step = core->insn_num + steps;
}

int gs32eb1_signal(conf_object_t * conf_obj, interrupt_signal_t * signal)
{
    mips_core_t *mstate = conf_obj->obj;

    if (io.int_ctrl[0].req0int & 0x1)
    {
        //printf("hardware int happened!\n");
        mstate->cp0[Cause] |= 1 << Cause_IP2;
        process_exception(mstate, EXC_Int, common_vector);
    } else if (!(mstate->cp0[Cause] & (1 << Cause_IP4)))
    {
        mstate->int_flag = 1;
    }
    return 0;
}

/*
 * Implement processor breakpoint management interface
 */
int mips_insert_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    mips_core_t *core = conf_obj->obj;
    int ret;

    ret = create_a_breakpoint(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

int mips_delete_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    mips_core_t *core = conf_obj->obj;
    int ret;

    ret = remove_bp_by_addr(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

uint32_t mips_get_bp_numbers(conf_object_t * conf_obj)
{
    mips_core_t *core = conf_obj->obj;

    return get_bp_numbers(&core->bp_manager);
}

uint32_t mips_get_bp_addr_by_id(conf_object_t * conf_obj, int id)
{
    mips_core_t *core = conf_obj->obj;
    breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);

    if (bp)
        return bp->address;
    return 0;
}

int mips_get_trigger(conf_object_t * conf_obj)
{
    mips_core_t *core = conf_obj->obj;

    return get_breakpoint_hit_id(&core->bp_manager);
}

bool_t mips_check_bp_trigger(conf_object_t * conf_obj)
{
    mips_core_t *core = conf_obj->obj;
    int ret;

    ret = check_breakpoint_hit_flag(&core->bp_manager);
    if (ret == 0)
        return True;
    return False;
}

int mips_clear_bp_trigger(conf_object_t * conf_obj)
{
    mips_core_t *core = conf_obj->obj;

    clear_breakpoint_hit_flag(&core->bp_manager);
    return 0;
}

exception_t mips_memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    mips_core_t *core = (mips_core_t *) obj->obj;

    core->memory_space = obj2;

    core->memory_space_iface = (memory_space_intf *) SKY_get_iface(core->memory_space, MEMORY_SPACE_INTF_NAME);
    if (core->memory_space_iface == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' " "interface from %s fail.", obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

exception_t mips_memory_space_get(conf_object_t * obj, conf_object_t ** obj2, char **port, int index)
{

    mips_core_t *core = (mips_core_t *) obj->obj;

    *obj2 = core->memory_space;
    *port = NULL;
    return No_exp;
}

DEF_UINTEGER_ATTR_FUNC(mips_core_t, mode);
DEF_UINTEGER_ATTR_FUNC(mips_core_t, debug_mode);
DEF_UINTEGER_ATTR_FUNC(mips_core_t, freq_hz);
void mips_register_attribute(conf_class_t * clss)
{

    DEF_ATTR(mode, uinteger, NULL);
    DEF_ATTR(debug_mode, uinteger, NULL);
    DEF_ATTR(freq_hz, uinteger, NULL);
    return;
}

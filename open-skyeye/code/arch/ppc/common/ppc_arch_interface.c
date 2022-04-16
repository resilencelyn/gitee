/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/*
 * 12/06/2007   Michael.Kang  <blackfin.kang@gmail.com>
 */
#include <string.h>

#include "ppc_cpu.h"
#include "ppc_mmu.h"
#include "ppc_exc.h"
#include "ppc_boot.h"
#include "ppc_io.h"
#include "types.h"
#include "tracers.h"
#include "ppc_irq.h"
#include "ppc_dec.h"
#include "ppc_regformat.h"
#include "bank_defs.h"

#include <skyeye_class.h>
#include <skyeye_core_intf.h>
#include <skyeye_int_register.h>
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
//#include "dyncom/ppc_dyncom_run.h"

#ifdef __CYGWIN__
#include <sys/time.h>
#endif

#ifdef DBG
#undef DBG
//#define DBG(fmt, ...) do { fprintf(stderr, fmt, ## __VA_ARGS__); } while (0)
#define DBG(fmt, ...)
#endif

const char *ppc_regstr[] =
{
    "R0",
    "R1",
    "R2",
    "R3",
    "R4",
    "R5",
    "R6",
    "R7",
    "R8",
    "R9",
    "R10",
    "R11",
    "R12",
    "R13",
    "R14",
    "R15",
    "R16",
    "R17",
    "R18",
    "R19",
    "R20",
    "R21",
    "R22",
    "R23",
    "R24",
    "R25",
    "R26",
    "R27",
    "R28",
    "R29",
    "R30",
    "R31",
    "F0",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "F25",
    "F26",
    "F27",
    "F28",
    "F29",
    "F30",
    "F31",
    "PC",
    "MSR",
    "CR",
    "LR",
    "CTR",
    "XER",
    "FPSCR",
    "NPC",
    "DEC",
    NULL
};

#define QUEUE_LEN 100000
typedef struct
{
    unsigned int qu[QUEUE_LEN];
    int len;
    int head;
    int count;
    int full;
} skyeye_queue;

skyeye_queue *skyeye_queue_init()
{
    skyeye_queue *queue = malloc(sizeof (skyeye_queue));

    if (!queue)
        return queue;
    queue->head = -1;
    queue->count = 0;
    queue->len = QUEUE_LEN;
    queue->full = 0;
    return queue;
}

int skyeye_queue_enter(skyeye_queue * queue, unsigned int item)
{
    if (queue->head == (queue->len - 1))
    {
        queue->head = -1;
    }
    if (!queue->full)
        queue->count++;

    queue->qu[++queue->head] = item;

    if (queue->head == (queue->len - 1))
        queue->full = 1;

}

int skyeye_queue_counter(skyeye_queue * queue)
{
    return queue->count;
}

int skyeye_queue_display(skyeye_queue * queue)
{
    int i;

    if ((!queue->full) || (queue->full && (queue->head == (queue->len - 1))))
    {
        for (i = 0; i < queue->count; i++)
            printf("PC:%x\n", queue->qu[i]);
    } else
    {
        i = queue->head + 1;
        for (; i < queue->len; i++)
            printf("PC:%x\n", queue->qu[i]);
        for (i = 0; i <= queue->head; i++)
            printf("PC:%x\n", queue->qu[i]);
    }
    return 0;
}

int ppc_divisor = 0;

extern void check_ext_int(e500_core_t * core);
static void debug_log(e500_core_t * core);

void ppc_stepi(conf_object_t * obj, uint64_t steps)
{
    e500_core_t *core = obj->obj;

    pthread_rwlock_wrlock(&(core->check_stepping_rwlock));

    core->stop_step = core->insn_num + steps;

    pthread_rwlock_unlock(&(core->check_stepping_rwlock));
}

void check_trace_exception(e500_core_t * core)
{
    if (core->pc == core->siar)
    {
        /*Trace Interrupt is Taken */
        ppc_exception(core, PPC_EXC_TRACE2, 0, 0);
        core->trace_exception = False;
        core->mode = core->old_mode;
    }
    return;
}

void check_exception_over(e500_core_t * core)
{
    if (core->pc == core->srr[0] && core->exception_status)
    {
        core->exception_status = 0;
    }
}

// insn_cache: set value
inline void insert_insn_tlb(e500_core_t *core, insn_cache_t *insn_cache, uint32_t pc, uint32_t insn)
{
    insn_cache->pc = pc;
    insn_cache->insn = insn;
    insn_cache->opc_func = get_opc_function(core, insn);
}

// insn_cache: create
bool dyncom_alloc_memory(e500_core_t *core, int index)
{
    core->insn_cache[index] = skyeye_mm_align(0x1000, sizeof(insn_cache_t) * PAGE_SIZE);

    if (core->insn_cache[index] == NULL)
    {
        printf("In %s, Line %d: Malloc memory error!\n", __func__, __LINE__);
        return false;
    }

    assert(core->index_count < PAGE_NUM);

    core->index_array[core->index_count] = index;
    core->index_count++;

    return true;
}

// insn_cache: check exist
inline bool is_in_insn_tlb(e500_core_t *core, uint32_t addr)
{
    int index = HASH_PAGE(addr);
    int offset = HASH_OFFSET(addr);
    assert(index < PAGE_NUM);
    assert(offset < PAGE_SIZE);

    if (core->insn_cache[index] != NULL)
    {
        if (core->insn_cache[index][offset].pc == addr && addr != 0)
        {
            return true;
        }
    }
    return false;
}

// insn_cache: get
insn_cache_t *get_insn_cache(conf_object_t *running_core, uint32_t addr)
{
    e500_core_t *core = running_core->obj;

    int index = HASH_PAGE(addr);
    int offset = HASH_OFFSET(addr);
    assert(index < PAGE_NUM);
    assert(offset < PAGE_SIZE);

    if (core->insn_cache[index] == NULL)
    {
        if (!dyncom_alloc_memory(core, index))
        {
            insn_cache_t *fake;
            fake->insn = 0;
            return fake;
        }
    }

    insn_cache_t *insn_cache = &core->insn_cache[index][offset];

    if (!is_in_insn_tlb(core, addr))
    {
        uint32_t insn = 0;
        core->core_memory_iface->read(running_core, addr, &insn, 4);
        insert_insn_tlb(core, insn_cache, addr, insn);
    }

    return insn_cache;
}

// insn_cache: update
bool do_check_selfmodify(conf_object_t *running_core, uint32_t addr)
{
    addr -= (addr % 4); // addr aligned by 4 bytes
    e500_core_t *core = running_core->obj;

    if (is_in_insn_tlb(core, addr)) //if write addr is selfmodify pc, rewrite insn to insn_cache
    {
        insn_cache_t *insn_cache = get_insn_cache(running_core, addr);
        uint32_t insn = 0;
        core->core_memory_iface->read(running_core, addr, &insn, 4);
        insert_insn_tlb(core, insn_cache, insn_cache->pc, insn);

        if (core->mode != 0) //flush selfModify bb if mode is hybrid mode or dyncom mode
        {

            //hybrid_selfmodify_flush(core, addr);
        }
        return true;
    }
    return false;
}

uint8_t CurrentInsnBytes;
void ppc_current_insn_width(uint8_t *width_bytes)
{
    if((CurrentInsnBytes == 2 )|| (CurrentInsnBytes == 4))
    {
        *width_bytes = CurrentInsnBytes;
    } else
    {
        *width_bytes = 0;
    }
}

void ppc_step_once(conf_object_t * running_core)
{
    e500_core_t *core = running_core->obj;
    uint32 real_addr;
    uint32 instr = 0;
    uint8_t InsnBitsNum = 0;

    /* Check the second core and boot flags */
    //if (core->pir) {
    //if (!(cpu->eebpcr & 0x2000000))
    //      return;
    //}
    /* sometimes, core->npc will be changed by another core */
    if ((core->ipi_flag == 1) && (core->msr & 0x8000))
    {                                   // MSR[EE] == 1
        if (core->arch_type == E200_FAMILY)
        {
            ppc_exception(core, EXT_INT, -1, 0);
        } else
        {
            ppc_exception(core, PPC_EXC_EXT_INT, 0, 0);
        }


        core->ipi_flag = 0;
        core->pc = core->npc;
        core->npc = core->pc + 4;
    }

    switch (ppc_effective_to_physical(core, core->pc, PPC_MMU_CODE, &real_addr))
    {
        case PPC_MMU_OK:
            break;
            /* we had TLB miss and need to jump to its handler */
        case PPC_MMU_EXC:
            goto exec_npc;
        case PPC_MMU_FATAL:
            /* TLB miss */
            fprintf(stderr, "TLB missed at 0x%x\n", core->pc);
            skyeye_exit(-1);
        default:
            /* TLB miss */
            fprintf(stderr, "Something wrong during address translation at 0x%x\n", core->pc);
            skyeye_exit(-1);
    };

    insn_cache_t *insn_cache = get_insn_cache(running_core, core->pc);

    if (core->VLE_Enable == 0)
    {
        if (insn_cache->insn != 0)
        {
            core->current_opc = insn_cache->insn;
            insn_cache->opc_func(core);
        } else
        {
            core->core_memory_iface->read(running_core, real_addr, &instr, 4);
            core->current_opc = instr;
            ppc_exec_opc(core);
        }
    } else
    {
        if (insn_cache->insn != 0)
        {
            core->current_opc = insn_cache->insn;

            InsnBitsNum = InsnBitsNumJudge(core->current_opc);
            if (InsnBitsNum == 16)
            {
                core->npc = core->pc + 2;
                CurrentInsnBytes = 2;
            } else
            {
                core->npc = core->pc + 4;
                CurrentInsnBytes = 4;
            }
            insn_cache->opc_func(core);
       } else
       {
            core->core_memory_iface->read(running_core, real_addr, &instr, 4);
            core->current_opc = instr;
            if (InsnBitsNum == 16)
            {
                core->npc = core->pc + 2;
                CurrentInsnBytes = 2;
            } else
            {
                core->npc = core->pc + 4;
                CurrentInsnBytes = 4;
            }
            ppc_exec_vle_opc(core);
       }
    }

    //DBG("in %s, core->pc=0x%x, r0=0x%x, instr=0x%x\n", __FUNCTION__, core->pc, core->gpr[0], instr);
    //printf("in func: %s, core->pc:0x%x, core->npc:0x%x, core->current_opc:0x%x, line:%d\n", __func__, //core->pc, core->npc, core->current_opc, __LINE__ );

    //some dubug printf
    /*
     * int i;
     * printf("########PC:0x%x,Instr:%x\n", core->pc, instr);
     * printf("\t[CTR]:0x%-8x\n", core->ctr);
     * printf("\t");
     * for(i = 0; i < 32; i++){
     * printf("[R%-2d]:%-8x", i, core->gpr[i]);
     * if((i + 1) % 8 == 0)
     * printf("\n\t");
     * }
     * printf("\n");
     */
    /*
     * static skyeye_queue *queue = NULL;
     * if(!queue)
     * queue = skyeye_queue_init();
     * if(queue){
     * skyeye_queue_enter(queue, core->pc);
     * if(core->pc == 0xfff00104){
     * skyeye_queue_display(queue);
     * exit(0);
     * }
     * }else{
     * printf("init queue fail!\n");
     * }
     */
    core->insn_num++;
    if (core->trace_exception)
        check_trace_exception(core);

  exec_npc:
    core->dec_io_do_cycle(core);

    if (core->npc == 0)
    {
        fprintf(stderr, "Next PC is 0, Current PC = 0x%x\n", core->pc);
        skyeye_log(Error_log, __FUNCTION__, "Next PC is 0, Current PC = 0x%x\n", core->pc);
        //exit(-1);
    }

    if (core->VLE_Enable == 0)
    {
        core->pc = core->npc;
        core->npc = core->pc + 4;
    } else
    {
        core->pc = core->npc;
    }

    check_exception_over(core);
    return;

}

void ppc_power(conf_object_t * conf_obj, uint32_t wake_flag)
{

    e500_core_t *core = conf_obj->obj;

    core->isWake = wake_flag;

    return;
}

uint64_t ppc_run(conf_object_t * running_core)
{
    e500_core_t *core = running_core->obj;
    uint64_t steps = core->insn_num;

    if (!core->isWake)
    {
        return 0;
    }
    SKY_pc_hap_occurred(NULL, running_core, core->pc);

    if (core->mode == DYNCOM_RUN)
    {
        //ppc_dyncom_step_once(running_core);
    }
    else if (core->mode == HYBRID_RUN)
    {
        //ppc_hybrid_run(running_core);
    } else if (core->mode == INTERPRET_RUN)
    {
        ppc_step_once(running_core);
    } else
    {
        /* invalid mode */
        return;
    }
    //stop check
    pthread_rwlock_wrlock(&(core->check_stepping_rwlock));
    if (core->stop_step != 0 && core->insn_num != 0 && core->stop_step <= core->insn_num)
    {
        SIM_stop(NULL);
        core->stop_step = 0;
    }
    pthread_rwlock_unlock(&(core->check_stepping_rwlock));

    //check breakpoint
    check_breakpoint(&(core->bp_manager), core->pc, core->insn_num);

    return (core->insn_num - steps);
}

void ppc_set_textsection_info(conf_object_t * obj, generic_address_t code_start_addr, generic_address_t code_end_addr)
{
    e500_core_t *core = obj->obj;

    core->code_start_addr = code_start_addr;
    core->code_end_addr = code_end_addr;
    printf("%x %x for text section\n", code_start_addr, code_end_addr);

    return;
}

void ppc_set_pc(conf_object_t * conf_obj, generic_address_t pc)
{
    e500_core_t *core = conf_obj->obj;

    if (core->VLE_Enable == 0)
    {
        core->pc = pc;
        core->npc = core->pc + 4;
    } else
    {
        core->pc = pc;
        core->npc = core->pc;
    }
    return;
    /* Fixme, for e500 core, the first instruction should be executed at 0xFFFFFFFC */
    //gCPU.pc = 0xFFFFFFFC;
}

generic_address_t ppc_get_pc(conf_object_t * conf_obj)
{
    e500_core_t *core = conf_obj->obj;

    return core->pc;
}

uint64 ppc_get_step(conf_object_t * conf_obj)
{
    e500_core_t *core = conf_obj->obj;

    return core->insn_num;
}

uint32 ppc_get_regnum(conf_object_t * conf_obj)
{
    return PPC_MAX_REGNUM;
}

char *ppc_get_regname_by_id(conf_object_t * conf_obj, int id)
{
    return ppc_regstr[id];
    //return NULL;
}

exception_t ppc_set_regval_by_id(conf_object_t * conf_obj, uint32 value, int id)
{
    /* we return the reg value of core 0 by default */
    e500_core_t *core = conf_obj->obj;

    if (id >= 0 && id < 32)
    {
        core->gpr[id] = value;
        return No_exp;
    }

    if (id >= 32 && id < 64)
    {
        core->fpr[id] = value;
        return No_exp;
    }

    switch (id)
    {
        case PC_REG:
            core->pc = value;
            break;
        case MSR:
            core->msr = value;
            break;
        case CR_REG:
            core->cr = value;
            break;
        case LR_REG:
            core->lr = value;
            break;
        case CTR_REG:
            core->ctr = value;
            break;
        case XER_REG:
            core->xer = value;
            break;
        case FPSCR:
            core->fpscr = value;
            break;
        case NPC_REG:
            core->npc = value;
            break;
        case DEC_REG:
            core->dec = value;
            break;
        default:
            /* can not find any corrsponding register */
            return Not_found_exp;
    }
    return No_exp;
}

exception_t ppc_set_reg64val_by_id(conf_object_t * conf_obj, uint64 value, int id)
{
    /* we return the reg value of core 0 by default */
    e500_core_t *core = conf_obj->obj;

    if (id >= 0 && id < 32)
    {
        core->gpr[id] = (uint32)(value & 0xFFFFFFFF);
        core->gprh[id] = (uint32)((value >> 32) & 0xFFFFFFFF);
        return No_exp;
    }

    if (id >= 32 && id < 64)
    {
        core->fpr[id] = value;
        return No_exp;
    }

    switch (id)
    {
        case PC_REG:
            core->pc = (uint32)value;
            break;
        case MSR:
            core->msr = (uint32)value;
            break;
        case CR_REG:
            core->cr = (uint32)value;
            break;
        case LR_REG:
            core->lr = (uint32)value;
            break;
        case CTR_REG:
            core->ctr = (uint32)value;
            break;
        case XER_REG:
            core->xer = (uint32)value;
            break;
        case FPSCR:
            core->fpscr = (uint32)value;
            break;
        case NPC_REG:
            core->npc = (uint32)value;
            break;
        case DEC_REG:
            core->dec = (uint32)value;
            break;
        default:
            /* can not find any corrsponding register */
            return Not_found_exp;
    }
    return No_exp;
}


uint32 ppc_get_regid_by_name(conf_object_t * conf_obj, char *name)
{
    int i;

    for (i = 0; i < PPC_MAX_REGNUM; i++)
    {
        if (strcmp(name, ppc_regstr[i]) == 0)
            return i;
    }
    return 0;
}

uint32 ppc_get_regval_by_id(conf_object_t * conf_obj, int id)
{
    /* we return the reg value of core 0 by default */
    e500_core_t *core = conf_obj->obj;

    if (id >= 0 && id < 32)
        return core->gpr[id];
    if (id >= 32 && id < 64)
    {
        return core->fpr[id];
    }
    switch (id)
    {
        case PC_REG:
            return core->pc;
        case MSR:
            return core->msr;
        case CR_REG:
            return core->cr;
        case LR_REG:
            return core->lr;
        case CTR_REG:
            return core->ctr;
        case XER_REG:
            return core->xer;
        case FPSCR:
            return core->fpscr;
        case NPC_REG:
            return core->npc;
        case DEC_REG:
            return core->dec;
        default:
            /* can not find any corrsponding register */
            return 0;
    }
}

uint64 ppc_get_reg64val_by_id(conf_object_t * conf_obj, int id)
{
    /* we return the reg value of core 0 by default */
    e500_core_t *core = conf_obj->obj;

    if (id >= 0 && id < 32)
    {
        return ((uint64)core->gprh[id] << 32) | ((uint64)core->gpr[id]);
    }
    if (id >= 32 && id < 64)
    {
        return core->fpr[id];
    }
    switch (id)
    {
        case PC_REG:
            return (uint64)core->pc;
        case MSR:
            return (uint64)core->msr;
        case CR_REG:
            return (uint64)core->cr;
        case LR_REG:
            return (uint64)core->lr;
        case CTR_REG:
            return (uint64)core->ctr;
        case XER_REG:
            return (uint64)core->xer;
        case FPSCR:
            return (uint64)core->fpscr;
        case NPC_REG:
            return (uint64)core->npc;
        case DEC_REG:
            return (uint64)core->dec;
        default:
            /* can not find any corrsponding register */
            return 0;
    }
}


endian_t ppc_get_endian(conf_object_t * obj)
{

    return Big_endian;
}

align_t ppc_get_alignment(conf_object_t * obj)
{
    return UnAlign;
}

char *ppc_get_architecture(conf_object_t * obj)
{
    return "powerpc";
}

void print_ppc_arg(e500_core_t *current_core, FILE * log)
{
    if (log)
        fprintf(log, "r3=0x%x,r4=0x%x,r5=0x%x\n", current_core->gpr[3], current_core->gpr[4], current_core->gpr[5]);
}

static void debug_log(e500_core_t * core)
{
    static uint32_t dbg_start = 0xc0000000;
    static uint32_t dbg_end = 0xfff83254;
    static int flag = 1;

    /*
     * if(core->pc == dbg_start)
     * flag = 0;
     * if (core->pc > dbg_start)
     * flag = 0;
     */
    /*
     * if(flag)
     * printf("In %s,pc=0x%x\n", __FUNCTION__, core->pc);
     */
    if (flag)
    {
        //if(core->pc >= 0xC0000000)
        printf("DBG:before r4=0x%x,r3=0x%x,r5=0x%x,pc=0x%x, npc=0x%x, &npc=0x%x, pir=0x%x\n",
               core->gpr[4], core->gpr[3], core->gpr[5], core->pc, core->npc, &core->npc, core->pir);
        if (core->pir == 1)
            printf("DBG:before r4=0x%x,r3=0x%x,r5=0x%x,pc=0x%x, npc=0x%x, &npc=0x%x, pir=0x%x\n",
                   core->gpr[4], core->gpr[3], core->gpr[5], core->pc, core->npc, &core->npc, core->pir);
    }
    if (flag)
    {
        /*
         * if(core->pc >= 0xC0000000)
         * if(core->pir)
         * printf("DBG 0:pc=0x%x, npc=0x%x, &npc=0x%x, pir=0x%x\n",
         * core->pc, core->npc, &core->npc, core->pir);
         */
    }
    /*if (skyeye_config.log.logon >= 1 && !core->pir)
     * skyeye_log(core->pc);
     */
}

int ppc_insert_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    e500_core_t *core = conf_obj->obj;
    int ret;

    ret = create_a_breakpoint(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

int ppc_delete_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    e500_core_t *core = conf_obj->obj;
    int ret;

    ret = remove_bp_by_addr(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

uint32_t ppc_get_bp_numbers(conf_object_t * conf_obj)
{
    e500_core_t *core = conf_obj->obj;

    return get_bp_numbers(&core->bp_manager);
}

uint32_t ppc_get_bp_addr_by_id(conf_object_t * conf_obj, int id)
{
    e500_core_t *core = conf_obj->obj;
    breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);

    if (bp)
        return bp->address;
    return 0;
}

int ppc_get_trigger(conf_object_t * conf_obj)
{
    e500_core_t *core = conf_obj->obj;

    return get_breakpoint_hit_id(&core->bp_manager);
}

bool_t ppc_check_bp_trigger(conf_object_t * conf_obj)
{
    e500_core_t *core = conf_obj->obj;
    int ret;

    ret = check_breakpoint_hit_flag(&core->bp_manager);
    if (ret == 0)
        return True;
    return False;
}

int ppc_clear_bp_trigger(conf_object_t * conf_obj)
{
    e500_core_t *core = conf_obj->obj;

    clear_breakpoint_hit_flag(&core->bp_manager);
    return 0;
}

conf_object_t *new_ppc_core(char *obj_name)
{
    e500_core_t *core = skyeye_mm_zero(sizeof (e500_core_t));

    core->obj = new_conf_object(obj_name, core);

    core->mode = 0;
    core->debug_mode = 0;
    core->freq_hz = 200000000;

#if 1                                   /* FIXME, compatible with old framework */

    PPC_CPU_State *cpu = skyeye_mm_zero(sizeof (PPC_CPU_State));

    cpu->core_num = 1;
    cpu->core = core;
    cpu->boot_core_id = 0;
    core->cpu = cpu;
#endif
    if (pthread_rwlock_init(&(core->check_stepping_rwlock), NULL))
    {
        fprintf(stderr, "can not initilize the rwlock\n");
    }

    return core->obj;
}

void config_ppc_core(conf_object_t * obj)
{
    e500_core_t *core = obj->obj;

    ppc_dec_init(core);

    core->core_memory_iface = skyeye_mm_zero(sizeof (memory_space_intf));
    core->core_memory_iface->read = ppc_space_read;
    core->core_memory_iface->write = ppc_space_write;
    core->core_memory_iface->get_page = ppc_get_page;

    core->insn_cache = skyeye_mm_zero(sizeof (insn_cache_t *) * PAGE_NUM);
    core->index_count = 0;

    //if (core->mode != 0)
        //ppc_dyncom_init(core);

    core->old_mode = core->mode;

    if (!strcmp(obj->class_name, "e500_core") || !strcmp(obj->class_name, "e200_core"))
    {
        core->arch_type = E200_FAMILY;
    } else if (!strcmp(obj->class_name, "e600_core"))
    {
        core->arch_type = E600_CORE;
    } else if (!strcmp(obj->class_name, "e300_core"))
    {
        core->arch_type = E300_CORE;
    } else
    {
        core->arch_type = UNDEFINE_CORE;
    }

    return;
}

void free_ppc_core(conf_object_t * dev)
{
    e500_core_t *core = dev->obj;

    //if (core->mode != 0)
    //{
        //ppc_dyncom_fini(core);
    //}

    int i, index;

    for (i = 0; i < core->index_count; i++)
    {
        index = core->index_array[i];
        skyeye_align_free(core->insn_cache[index]);
    }
    skyeye_free(core->insn_cache);

    skyeye_free(core->core_memory_iface);
    skyeye_free(core->cpu);
    skyeye_free(core->obj);
    skyeye_free(core);

    return;
}

static attr_value_t get_attr_ccsr(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{

    e500_core_t *core = (e500_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_object(core->cpu->ccsr);

    return value;
}
static exception_t set_attr_ccsr(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{

    e500_core_t *core = (e500_core_t *) conf_obj->obj;

    core->cpu->ccsr = SKY_attr_uinteger(*value);
    return No_exp;
}

DEF_UINTEGER_ATTR_FUNC(e500_core_t, load_addr);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, load_mask);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, msr);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, ipr);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, ipi_flag);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, pir);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, freq_hz);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, pvr);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, mode);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, debug_mode);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, svr);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, ccsr_size);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, isWake);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, speSupport);
DEF_UINTEGER_ATTR_FUNC(e500_core_t, VLE_Enable);

void ppc_register_attribute(conf_class_t * clss)
{

    SKY_register_attribute(clss, "ccsr", get_attr_ccsr, NULL, set_attr_ccsr, NULL, SKY_Attr_Optional, "uinteger",
                           "ccsr register of cpu");
    DEF_ATTR(load_addr, uinteger, "load addr space of processor core");
    DEF_ATTR(load_mask, uinteger, "load mask of processor core");
    DEF_ATTR(msr, uinteger, NULL);
    DEF_ATTR(ipr, uinteger, NULL);
    DEF_ATTR(ipi_flag, uinteger, NULL);
    DEF_ATTR(pir, uinteger, NULL);
    DEF_ATTR(freq_hz, uinteger, NULL);
    DEF_ATTR(pvr, uinteger, NULL);
    DEF_ATTR(mode, uinteger, NULL);
    DEF_ATTR(debug_mode, uinteger, NULL);
    DEF_ATTR(svr, uinteger, NULL);
    DEF_ATTR(ccsr_size, uinteger, NULL);
    DEF_ATTR(isWake, uinteger, NULL);
    DEF_ATTR(speSupport, uinteger, NULL);
    DEF_ATTR(VLE_Enable, uinteger, NULL);

    return;
}

exception_t ppc_memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    e500_core_t *core = obj->obj;

    switch (index)
    {
        case 1:
            core->memory_space = obj2;
            if (core->memory_space == NULL)
            {
                skyeye_log(Warning_log, __FUNCTION__, "Set 'memory space' attribute Error");
                return Not_found_exp;
            }
            core->memory_space_iface = (memory_space_intf *) SKY_get_iface(core->memory_space, MEMORY_SPACE_INTF_NAME);
            if (core->memory_space_iface == NULL)
            {
                skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' " "interface from %s fail.", obj2->objname);
                return Not_found_exp;
            }
            break;
        case 2:
            core->memory_ccsr = obj2;
            if (core->memory_ccsr == NULL)
            {
                skyeye_log(Warning_log, __FUNCTION__, "Set 'memory space' attribute Error");
                return Not_found_exp;
            }
            core->memory_ccsr_iface = (memory_space_intf *) SKY_get_iface(core->memory_ccsr, MEMORY_SPACE_INTF_NAME);
            if (core->memory_ccsr_iface == NULL)
            {
                skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' interface from %s fail.", obj2->objname);
                return Not_found_exp;
            }
            break;
        default:
            skyeye_log(Warning_log, __FUNCTION__, "connect index error(%d)", index);
            return Not_found_exp;
    }

    return No_exp;
}

exception_t ppc_memory_space_get(conf_object_t * obj, conf_object_t ** obj2, char **port, int index)
{

    e500_core_t *core = obj->obj;

    *obj2 = core->memory_space;
    *port = NULL;
    return No_exp;
}

exception_t ppc_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{

    e500_core_t *core = obj->obj;
    PPC_CPU_State *cpu = (PPC_CPU_State *) core->cpu;

    //r = ppc_effective_to_physical(current_core, addr, PPC_MMU_READ, &p);
    if (in_ccsr_range(core, addr))
    {
        return core->memory_ccsr_iface->read(core->memory_ccsr, (addr - core->get_ccsr_base(cpu->ccsr)), buf, count);
    } else
    {
        memory_space_intf *iface = core->memory_space_iface;

        return iface->read(core->memory_space, addr, buf, count);
    }
}

exception_t ppc_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{

    e500_core_t *core = obj->obj;
    PPC_CPU_State *cpu = (PPC_CPU_State *) core->cpu;

    if (in_ccsr_range(core, addr))
    {
        return core->memory_ccsr_iface->write(core->memory_ccsr, (addr - core->get_ccsr_base(cpu->ccsr)), buf, count);
    } else
    {
        exception_t retval = core->memory_space_iface->write(core->memory_space, addr, buf, count);
        do_check_selfmodify(obj, addr);
        return retval;
    }
}

get_page_t *ppc_get_page(conf_object_t * obj, generic_address_t addr)
{
    e500_core_t *core = obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    return iface->get_page(core->memory_space, addr);
}

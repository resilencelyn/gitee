/*
 * =====================================================================================
 *
 *       Filename:  iu.h
 *
 *    Description:  SPARC Integuer Unit
 *
 *        Version:  1.0
 *        Created:  22/04/08 09:57:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

#ifndef _IU_H_
#define _IU_H_

#include "skyeye_types.h"
#include "skyeye_addr_space.h"
#include "bits.h"

#ifndef __SPARC_H__
#error "arch/sparc/common/sparc.h header file must be included"
#endif
#ifndef _TRAPS_H_
#error "arch/sparc/common/traps.h header file must be included"
#endif
#include <skyeye_addr_space.h>
#include <skyeye_core.h>

/** Number of slied windows */
#define N_WINDOWS       8
#define MAX_NWINDOWS    32

/** Processor State Register (PSR) fields   */
enum
{
    PSR_impl_last = 31,
    PSR_impl_first = 28,
    PSR_ver_last = 27,
    PSR_ver_first = 24,
    PSR_icc_N = 23,
    PSR_icc_Z = 22,
    PSR_icc_V = 21,
    PSR_icc_C = 20,
    PSR_reserved_last = 19,
    PSR_reserved_first = 14,
    PSR_EC = 13,
    PSR_EF = 12,
    PSR_PIL_last = 11,
    PSR_PIL_first = 8,
    PSR_S = 7,
    PSR_PS = 6,
    PSR_ET = 5,
    PSR_CWP_last = 4,
    PSR_CWP_first = 0,
};

enum
{
    TBR_tba_last = 31,
    TBR_tba_first = 12,
    TBR_tt_last = 11,
    TBR_tt_first = 4,
};

/** window register offsets  */
enum
{ OUT_REG_OFF = 0, LOCAL_REG_OFF = 8, IN_REG_OFF = 16, };
typedef struct insn_cache
{
    uint32 insn;
    int rd;
    int rs1;
    int rs2;
    int imm;
} insn_cache_t;

typedef struct sparc_tlb_item
{
    uintptr_t hva;                      /* host virtual address */
    generic_address_t gpa;              /* guest physical address */
} sparc_tlb_item_t;

typedef struct
{
    uint32_t cache_ctrl_reg;
    uint32_t icache_cfg_reg;
    uint32_t dcache_cfg_reg;
}l1cache_reg_t;

typedef struct _sparc_state
{
    conf_object_t *obj;
    /*  Register set    */
    /* In dyncom mode, a register window is compounded of global + regw
     * and load from/restore to regbase depending on cwp when sliding window
     */
    uint32 global[8];
    uint32 regw[24];
    uint32 regbase[MAX_NWINDOWS * 16 + 8];
    uint32 *regwptr[3];
    uint32 regw_index;

    uint32 y;                           // multiply/divide register
    uint32 psr;                         // PSR register
    uint32 wim;                         // window invalid register
    uint32 tbr;                         // Trap base register
    uint32 pc;                          // Program counter
    uint32 npc;                         // Next program counter
    uint32 cpsr;                        // coprocesor status register
    uint32 fpsr;                        // Floating point status register
    uint32 fp_regs[32];                 // FPU registers

    /* mmu for dyncom */
    uint32 ctrl;
    uint32 ctp;
    uint32 ctxt;
    uint32 fsr;
    uint32 far_reg;

    /* about implement */
    uint32 ncwp;
    /* trap: include exception & window sliding
     * window sliding will start from 0x100
     */
    uint32 trap;
    /*
     * trap_impl: just irq trap
     * under dynamic mode, the irq is not been handled right on time,
     * it waits until entering the trap block.
     */
    uint32 trap_impl;
    uint32 trap_arg0;
    uint32 trap_arg1;
    uint32 trap_arg2;
    uint32 trap_delay;
    uint32 irq_income;
    uint32 asr_regs[32];                 // ASR registers
    uint32 is_irq;

    uint8 cwp;                          // Current window pointer

    /*  CPU state   */
    uint32 pipeline;                    // instruction in the delay-slot

    sparc_exp_t sparc_exp;

    uint32 irq_pending;                 // IRQ pending flag
    uint32 irq_mask;
    uint64 cycle_counter;               // processor cycle counter
    uint64 steps;                       // how manys instructions executed by processor
    core_running_mode_t mode;
    int debug_mode;
    conf_object_t *dyncom_cpu;
    addr_space_t *bus_space;
    conf_object_t *core_obj;            /* the object of sparc core class */
    insn_cache_t cache[1024];
    uint64_t stop_step;

    struct general_signal_s
    {
        conf_object_t *obj;
        general_signal_iface_t *iface;
    } general_signal;
    memory_space_intf *memory_space_iface;
    conf_object_t *memory_space;
    memory_space_intf *sparc_memory_iface;
    breakpoint_mgt_t bp_manager;
    uint32_t freq_hz;
    int wake_flag;
    int run_flag;
    int cpu_id;
    int leon_version;
    uint32_t reset_pc;
    uint32_t reset_npc;

#define SPARC_TLB_SIZE 0x100000
    sparc_tlb_item_t tlb_cache[SPARC_TLB_SIZE];
    char exception_name[100];
    l1cache_reg_t l1cache_reg;
} sparc_state_t;

/** Defines to deal with the TBR register   */
#define TBR_BASE_MASK   0xFFFFF000
#define TBR_TT_MASK     0x00000FF0

/** 
 * This is the sparc_state variable, which maintains the SPARC CPU status
 * information
 * */
//extern sparc_state_t sparc_state;

#define REG(x)  sparc_state->regwptr[(x > 7)][x - 8*(x > 7)]
#define FPREG(x)  sparc_state->fp_regs[x]
#define YREG  sparc_state->y
#define PSRREG  sparc_state->psr
#define WIMREG  sparc_state->wim
#define TBRREG  sparc_state->tbr
#define PCREG  sparc_state->pc
#define NPCREG  sparc_state->npc
#define FPSRREG  sparc_state->fpsr
#define CPSRREG  sparc_state->cpsr
#define CWP     (bits(sparc_state->psr, PSR_CWP_last, PSR_CWP_first))

/*-----------------------------------------------------------------------------
 *  PUBLIC INTERFACE
 *-----------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif
    uint32 iu_sub_cwp(sparc_state_t * sparc_state);
    uint32 iu_add_cwp(sparc_state_t * sparc_state);
    int init_sparc_iu(sparc_state_t * sparc_state);
    void iu_set_cwp(sparc_state_t * sparc_state, int new_cwp);
    trap_handle_t *get_sparc_trap(void);

#ifdef __cplusplus
}
#endif
int sign_ext(int, int);
extern trap_handle_t *traps;

//#define sign_ext22(x)   sign_ext(x, 22)
//#define sign_ext13(x)   sign_ext(x, 13)
#define psr_get_s()		bit(sparc_state->psr, PSR_S)
#define psr_get_carry()         bit(sparc_state->psr, PSR_icc_C)
#define psr_set_carry()         set_bit(sparc_state->psr, PSR_icc_C)
#define psr_clear_carry()       clear_bit(sparc_state->psr, PSR_icc_C)

#define psr_get_overflow()         bit(sparc_state->psr, PSR_icc_V)
#define psr_set_overflow()      set_bit(sparc_state->psr, PSR_icc_V)
#define psr_clear_overflow()    clear_bit(sparc_state->psr, PSR_icc_V)

#define psr_get_zero()         bit(sparc_state->psr, PSR_icc_Z)
#define psr_set_zero()          set_bit(sparc_state->psr, PSR_icc_Z)
#define psr_clear_zero()        clear_bit(sparc_state->psr, PSR_icc_Z)

#define psr_get_neg()         bit(sparc_state->psr, PSR_icc_N)
#define psr_set_neg()           set_bit(sparc_state->psr, PSR_icc_N)
#define psr_clear_neg()         clear_bit(sparc_state->psr, PSR_icc_N)

void sparc_set_pc(conf_object_t * opaque, generic_address_t addr);
generic_address_t sparc_get_pc(conf_object_t * opaque);
uint64_t sparc_get_steps(conf_object_t * obj);
endian_t sparc_get_endian(conf_object_t * obj);
align_t sparc_get_alignment(conf_object_t * obj);
void sparc_reset(conf_object_t * opaque);
void sparc_run(conf_object_t * obj);

 /**
 *  \class sparc_return_t
 *  \brief  This structure defines all the possible error codes
 */
typedef enum _sparc_return_t
{
    SPARC_SUCCESS = 0,
    SPARC_ERROR = -1,

    SPARC_ISA_INIT_ERR = -100,
    SPARC_ISA_NEW_I_ERR = -101,

    SPARC_EMU_BAD_TT = -200,

} sparc_return_t;

#endif

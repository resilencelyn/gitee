#include <skyeye_interface.h>
#include <skyeye_class.h>
#include "armdefs.h"
#include "armcpu.h"
#include "armemu.h"
#include "arm_regformat.h"
#include "arm_export_reg.h"

#include "skyeye_internal.h"
#include "skyeye_checkpoint.h"
#include "skyeye_arch.h"
#include "skyeye_options.h"
#include "skyeye_types.h"
#include "skyeye_signal.h"
#include "skyeye_mm.h"
#include "skyeye_cell.h"
#include "bank_defs.h"
#include "skyeye_log.h"
#include "mmu/tlb.h"
#include "mmu/cache.h"
#include "sim_control.h"
#include "arch64_decode.h"

char *arm_regstr[] = {
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
    "SP",
    "LR",
    "R15",
    "Cpsr",
    "Spsr",
    "MMU_Fault_Address",
    "MMU_CTRL",
    "MMU_TTBR0",
    "MMU_TTBR1",
    "MMU_TTBC",
    "PHYS_PC",
    "R13_USR",
    "R14_USR",
    "R13_SVC",
    "R14_SVC",
    "R13_ABORT",
    "R14_ABORT",
    "R13_UNDEF",
    "R14_UNDEF",
    "R13_IRQ",
    "R14_IRQ",
    "R8_FIRQ",
    "R9_FIRQ",
    "R10_FIRQ",
    "R11_FIRQ",
    "R12_FIRQ",
    "R13_FIRQ",
    "R14_FIRQ",
    "SPSR_INVALID1",
    "SPSR_INVALID2",
    "SPSR_SVC",
    "SPSR_ABORT",
    "SPSR_UNDEF",
    "SPSR_IRQ",
    "SPSR_FIRQ"
};

char *arm_arch64_regstr[] = {
    "X0",
    "X1",
    "X2",
    "X3",
    "X4",
    "X5",
    "X6",
    "X7",
    "X8",
    "X9",
    "X10",
    "X11",
    "X12",
    "X13",
    "X14",
    "X15",
    "X16",
    "X17",
    "X18",
    "X19",
    "X20",
    "X21",
    "X22",
    "X23",
    "X24",
    "X25",
    "X26",
    "X27",
    "X28",
    "X29",
    "X30",
    "ZR",
    "A64_SP",
    "A64_PC",
};

const char *arm_exception_name[] =
{
    "Reset",
    "UndefinedInstr",
    "SWI",
    "RrefetchAbort",
    "DataAbort",
    "AddrException",
    "IRQ",
    "FIQ",
    NULL
};

void arm_run(conf_object_t * conf_obj)
{
    arm_step_once(NULL);
}

void arm_stop(conf_object_t * conf_obj)
{
}

extern ARMword ARMul_Execute_One_Insn(ARMul_State * state);
uint32_t arm_step_once(ARMul_State * state)
{
    uint64_t cur_steps = state->NumInstrs;

    if (!state->step)
    {
        state->NextInstr = RESUME; /* treat as PC change */
    }

    state->step++;

    ARMul_Execute_One_Insn(state);
    return state->NumInstrs - cur_steps;
}

uint32_t arm_arch64_step_once(ARMul_State * state)
{
    uint32_t cur_steps = state->NumInstrs;

    arch64_execute_insn(state);

    return state->NumInstrs - cur_steps;
}

void arm_set_pc(ARMul_State * state, generic_address_t pc)
{
    if (state->is_v8 && state->isAArch64)
    {
        state->A64_PC = pc;
    } else
    {
        if (pc & 1)
        {
            /*Thumb bit*/
            state->TFlag = 1;
        }
        state->Reg[15] = pc & (~3);
        state->realPC = pc & (~3);
    }
    return;
}

generic_address_t arm_get_pc(ARMul_State * state)
{
    generic_address_t pc;

    if (state->is_v8 && state->isAArch64)
        pc = state->A64_PC;
    else
        pc = state->realPC;
    return pc;
}

uint64_t arm_get_steps(conf_object_t * conf_obj)
{
    return 0;
}

endian_t arm_get_endian(conf_object_t * conf_obj)
{
    return Little_endian;
}

align_t arm_get_alignment(conf_object_t * conf_obj)
{
    return Align;
}

void arm_reset(conf_object_t * conf_obj)
{
    return;
}

char *arm_get_architecture(conf_object_t * obj)
{
    return "arm";
}

static uint64 arm_get_step()
{
    ARMul_State *state = get_current_core();
    uint64 step = state->NumInstrs;

    return step;
}

char *arm_get_regname_by_id(ARMul_State * state, uint32 id)
{
    return arm_export_regname[id];
}

uint32 arm_get_regid_by_name(ARMul_State * state, char *name)
{
    uint32_t regnum = EX_REG_NUM;
    int i;

    for (i = 0; i < regnum; i++)
    {
        if (strcmp(name, arm_export_regname[i]) == 0)
            return i;
    }
    return regnum;
}

uint32 arm_get_regvalue_by_id(ARMul_State * state, uint32 id)
{
    uint32 ret_data = 0;
    if (id < EX_R15)
    {
        return state->Reg[id];
    }

    switch (id)
    {
        case EX_R15:
            ret_data = state->realPC;
            break;
        case EX_CPSR_REG:
            ret_data = state->Cpsr;
            break;
        case EX_SPSR_REG:
            ret_data = state->Spsr_copy;
            break;
        case EX_MMU_Fault_Address:
            ret_data = state->mmu.fault_address;
            break;
        case EX_MMU_CTRL:
            ret_data = state->mmu.control;
            break;
        case EX_MMU_TTBR0:
            ret_data = state->mmu.translation_table_base0;
            break;
        case EX_MMU_TTBR1:
            ret_data = state->mmu.translation_table_base1;
            break;
        case EX_MMU_TTBC:
            ret_data = state->mmu.translation_table_ctrl;
            break;
        default:
            printf("In %s, Line:%d. Not define register id:%d\n", __func__, __LINE__, id);
            break;
    }
    return ret_data;
}

/**
* @brief Only return the number of regular register
*
* @return the number of regular register
*/
uint32 arm_get_regnum(ARMul_State * state)
{
    return EX_REG_NUM;
}

exception_t arm_set_regvalue_by_id(ARMul_State * state, uint32 value, int id)
{
    if (id < EX_R15)
    {
        state->Reg[id] = value;
        return No_exp;
    }

    switch (id)
    {
        case EX_R15:
            state->realPC = value;
            break;
        case EX_CPSR_REG:
            state->Cpsr = value;
            break;
        case EX_SPSR_REG:
            state->Spsr_copy = value;
            break;
        case EX_MMU_Fault_Address:
            state->mmu.fault_address = value;
            break;
        case EX_MMU_CTRL:
            state->mmu.control = value;
            break;
        case EX_MMU_TTBR0:
            state->mmu.translation_table_base0 = value;
            break;
        case EX_MMU_TTBR1:
            state->mmu.translation_table_base1 = value;
            break;
        case EX_MMU_TTBC:
            state->mmu.translation_table_ctrl = value;
            break;
        default:
            printf("In %s, Line:%d. Not define register id:%d\n", __func__, __LINE__, id);
            break;
    }
    return No_exp;
}

/**
* @brief Only return the number of AArch64 regular register
*
*/
uint32 arm_arch64_get_regnum(ARMul_State * state)
{
    return MAX_ARCH64_REG_NUM;
}

exception_t arm_arch64_set_regvalue_by_id(ARMul_State * state, uint64 value, uint32 id)
{
    if (id == ZR)
        state->ZR = value;
    else if (id == A64_SP)
        state->A64_SP = value;
    else if (id == A64_PC)
        state->A64_PC = value;
    else
        state->R[id].X = value;
    return No_exp;
}

char *arm_arch64_get_regname_by_id(ARMul_State * state, uint32 id)
{
    assert(id < MAX_ARCH64_REG_NUM);
    return arm_arch64_regstr[id];
}

uint32 arm_arch64_get_regid_by_name(ARMul_State * state, char *name)
{
    int i;

    for (i = 0; i < MAX_ARCH64_REG_NUM; i++)
    {
        if (strcmp(name, arm_arch64_regstr[i]) == 0)
            return i;
    }
    return 0;
}

uint64 arm_arch64_get_regvalue_by_id(ARMul_State * state, uint32 id)
{
    if (id == ZR)
        return state->ZR;
    else if (id == A64_SP)
        return state->A64_SP;
    else if (id == A64_PC)
        return state->A64_PC;
    else
    {
        return state->R[id].X;
    }
}

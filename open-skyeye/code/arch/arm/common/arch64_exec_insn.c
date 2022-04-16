/*  armv8_exec_insn.c -- ARMv8 instruction excute:  ARM8 Instruction Emulator.
 * Copyright (C) 1994 Advanced RISC Machines Ltd.
 * Modifications to add arch. v4 support by <jsmith@cygnus.com>.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "arm_regformat.h"
#include "armdefs.h"
#include "armemu.h"
#include "armos.h"

#include "skyeye_callback.h"
#include "skyeye_bus.h"
#include "sim_control.h"
#include "skyeye_pref.h"
#include "skyeye.h"
#include "arch64_decode.h"
#include "arch64_exception.h"
#include "arch64_vfp.h"
#include "skyeye_types.h"
#include <skyeye_swapendian.h>
#include <math.h>
#include <assert.h>
#include "arch64_helper.h"
#include "arch64_memory.h"

void print_pclog(ARMul_State * state)
{
    printf("####PC = 0x%llx SP = 0x%llx instr = %x N = %d C = %d Z = %d V = %d\n", state->A64_PC, state->A64_SP,
           state->instr, state->PSTATE.N, state->PSTATE.C, state->PSTATE.Z, state->PSTATE.V);
    {
        int i;

        for (i = 0; i < 31; i++)
        {
            printf("X[%d] = %08llx\t", i, state->R[i].X);
            if (i > 0 && ((i + 1) % 4) == 0)
                printf("\n");
        }
        printf("\n");

        for (i = 0; i < 32; i++)
        {
            printf("V[%d] = %08llx|%08llx\t", i, state->V[i].dword.d[0], state->V[i].dword.d[1]);
            if (i > 0 && ((i + 1) % 4) == 0)
                printf("\n");
        }
        printf("\n");
    }
}

static arch64_opc_function arch64_opc_table_main[16] = {
    &arm_opc_reserved,                  //0000 Reserved
    &arm_opc_unallocated,               //0001 Unallocated
    &arm_opc_sve,                       //0010 SVE instructions
    &arm_opc_unallocated,               //0011 Unallocated
    &arm_opc_load_and_store,            //0100 Loads and Stores
    &arm_opc_data_process_register,     //0101 Data Processing--Register
    &arm_opc_load_and_store,            //0110 Loads and Stores
    &arm_opc_data_process_fp_and_simd,  //0111 Data Processing--Scalar Floating-Point and Advanced SIMD
    &arm_opc_data_process_immediate,    //1000 Data Processing--Immediate
    &arm_opc_data_process_immediate,    //1001 Data Processing--Immediate
    &arm_opc_branch_exception_system,   //1010 Branch,Exception Generating and System instructions
    &arm_opc_branch_exception_system,   //1011 Branch,Exception Generating and System instructions
    &arm_opc_load_and_store,            //1100 Loads and Stores
    &arm_opc_data_process_register,     //1101 Data Processing--Register
    &arm_opc_load_and_store,            //1110 Loads and Stores
    &arm_opc_data_process_fp_and_simd,  //1111 Data Processing--Scalar Floating-Point and Advanced SIMD
};

void arch64_exec_opc(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword op = BITS(25, 28);

    arch64_opc_table_main[op] (state);
}

void arch64_execute_insn(ARMul_State * state)
{
    ARMdword pc = state->A64_PC;
    ARMword instr = 0;

    if (NO_FAULT != mem_read_word(state, (int) pc, &instr))
    {
        skyeye_log(Error_log, __FUNCTION__, "Read instr error at pc = %llx\n", pc);
    }
    //decode instructions
    state->instr = instr;
#if DEBUG
    print_pclog(state);
#endif

    arch64_exec_opc(state);
    state->NumInstrs++;

    /* check breakpoint. chech if we need to run some callback functions at this time */
    exec_callback(Step_callback, state->cpu_obj);
}

/*
 * This utility function is for doing register extension with an
 * optional shift. You will likely want to pass a temporary for the
 * destination register. See DecodeRegExtend() in the ARM ARM.
 */
static void ext_and_shift_reg(ARMdword * dst, ARMdword * src, int option, ARMword shift)
{
    int extsize = option & 0x3;
    bool_t is_signed = option >> 2;
    int64_t result;

    if (is_signed)
    {
        switch (extsize)
        {
            case 0:
                result = *src << 56;
                *dst = result >> 56;
                break;
            case 1:
                result = *src << 48;
                *dst = result >> 48;
                break;
            case 2:
                result = *src << 32;
                *dst = result >> 32;
                break;
            case 3:
                *dst = *src;
                break;
            default:
                break;
        }
    } else
    {
        switch (extsize)
        {
            case 0:
                *dst = *src & 0xFF;
                break;
            case 1:
                *dst = *src & 0xFFFF;
                break;
            case 2:
                *dst = *src & 0xFFFFFFFF;
                break;
            case 3:
                *dst = *src;
                break;
            default:
                break;
        }
    }

    if (shift)
    {
        *dst <<= shift;
    }
}

/* Shift a  src by immediate, put result in dst.
 * The shift amount must be in range (this should always be true as the
 * relevant instructions will UNDEF on bad shift immediates).
 */
static void shift_reg_imm(ARMdword * dst, ARMdword * src, int sf, enum a64_shift_type shift_type, ARMword shift_i)
{
    assert(shift_i < (sf ? 64 : 32));
    ARMdword result;

    if (shift_i == 0)
    {
        *dst = *src;
    } else
    {
        ARMdword shift_const = (ARMdword) shift_i;

        switch (shift_type)
        {
            case A64_SHIFT_TYPE_LSL:
                *dst = *src << shift_const;
                break;
            case A64_SHIFT_TYPE_LSR:
                *dst = *src >> shift_const;
                break;
            case A64_SHIFT_TYPE_ASR:
                if (!sf)
                {
                    result = *src & BIT32_MAX_VALUE;
                    if (result >> 31)
                    {
                        BIT32_SHIFT(result, shift_const);
                    }
                } else
                {
                    result = *src;
                    if (result >> 63)
                    {
                        BIT64_SHIFT(result, shift_const);
                    }
                }
                result |= (*src >> shift_const);
                *dst = result;
                break;
            case A64_SHIFT_TYPE_ROR:
                if (!sf)
                {
                    result = ROR32((ARMword) (*src), shift_const) & BIT32_MAX_VALUE;
                } else
                {
                    result = ROR64(*src, shift_const);
                }
                *dst = result;
                break;
            default:
                break;
        }
    }
}

static inline ARMdword A64_AddWithCarry(ARMul_State * state, ARMdword x, ARMdword y, ARMword carry_in, ARMword setFlags)
{
    ARMdword result;
    uint64_t unsigned_sum = (uint64_t) x + (uint64_t) y + carry_in;
    int64_t signed_sum = (int64_t) x + (int64_t) y + carry_in;

    result = signed_sum;
    if (setFlags)
    {
        //set PSATE N Z C V
        state->PSTATE.N = result >> 63;

        if (result == 0)
            setPStateZF;
        else
            clearPStateZF;

        if (((int64_t) x < 0 || (int64_t) y < 0) && (signed_sum > 0))
            setPStateCF;
        else
            clearPStateCF;

        if ((int64_t) result == signed_sum)
            clearPStateVF;
        else
            setPStateVF;
    }

    return result;
}

static inline ARMword A32_AddWithCarry(ARMul_State * state, ARMword x, ARMword y, ARMword carry_in, ARMword setFlags)
{
    ARMword result;
    uint64_t unsigned_sum = (uint64_t) x + (uint64_t) y + carry_in;
    int32_t signed_sum = (int32_t) x + (int32_t) y + carry_in;

    result = (uint32_t) unsigned_sum;
    if (setFlags)
    {
        //set PSATE N Z C V
        state->PSTATE.N = (result >> 31) & 1;

        if (result == 0)
            setPStateZF;
        else
            clearPStateZF;

        if (result == unsigned_sum)
            clearPStateCF;
        else
            setPStateCF;

        if ((int32_t) result == signed_sum)
            clearPStateVF;
        else
            setPStateVF;
    }

    return result;
}

const SysReg_Decode_Opc SysRegTable[] = {
    {"CurrentEL", 3, 0, 4, 2, 2},
    {"DAIF", 3, 3, 4, 2, 1},
    {"DIT", 3, 3, 4, 2, 3},
    {"NZCV", 3, 3, 4, 2, 0},
    {"PAN", 3, 0, 4, 2, 3},
    {"SPSel", 3, 0, 4, 2, 0},
    {"SSBS", 3, 3, 4, 2, 6},
    {"TCO", 3, 3, 4, 2, 7},
    {"UAO", 3, 0, 4, 2, 4},
    {"ELR_EL1", 3, 0, 4, 0, 1},
    {"ELR_EL2", 3, 4, 4, 0, 1},
    {"ELR_EL3", 3, 6, 4, 0, 1},
    {"FPCR", 3, 4, 3, 0, 4},
    {"FPSR", 3, 4, 3, 1, 4},
    {"SP_EL0", 3, 0, 4, 1, 0},
    {"SP_EL1", 3, 4, 4, 1, 0},
    {"SP_EL2", 3, 6, 4, 1, 0},
    //{"SP_EL3", x, x, x, x, x},
    //{"SPSR_abt", 3, 4, 4, 3, 1},
    {"SPSR_EL1", 3, 0, 4, 0, 0},
    //{"A32_SPSR_EL2", x, x, x, x, x},
    {"SPSR_EL2", 3, 4, 4, 0, 0},
    //{"A32_SPSR_EL3", 3, 5, 4, 0, 0},
    {"SPSR_EL3", 3, 6, 4, 0, 0},
    //{"SPSR_fiq", 3, 4, 4, 3, 3},
    //{"SPSR_irq", 3, 4, 4, 3, 0},
    //{"SPSR_und", 3, 4, 4, 3, 2},
    //System registers
    {"ACTLR_EL1", 3, 0, 1, 0, 1},
    {"ACTLR_EL2", 3, 4, 1, 0, 1},
    {"ACTLR_EL3", 3, 6, 1, 0, 1},
    {"AFSR0_EL1", 3, 0, 5, 1, 0},
    {"AFSR0_EL2", 3, 4, 5, 1, 0},
    {"AFSR0_EL3", 3, 6, 5, 1, 0},
    {"AFSR1_EL1", 3, 0, 5, 1, 1},
    {"AFSR1_EL2", 3, 4, 5, 1, 1},
    {"AFSR1_EL3", 3, 6, 5, 1, 1},
    {"AIDR_EL1", 3, 1, 0, 0, 7},
    {"AMAIR_EL1", 3, 0, 10, 3, 0},
    {"AMAIR_EL2", 3, 4, 10, 3, 0},
    {"AMAIR_EL3", 3, 6, 10, 3, 0},
    {"APDAKeyHi_EL1", 3, 0, 2, 2, 1},
    {"APDAKeyLo_EL1", 3, 0, 2, 2, 0},
    {"APDBKeyHi_EL1", 3, 0, 2, 2, 3},
    {"APDBKeyLo_EL1", 3, 0, 2, 2, 2},
    {"APGAKeyHi_EL1", 3, 0, 2, 3, 1},
    {"APGAKeyLo_EL1", 3, 0, 2, 3, 0},
    {"APIAKeyHi_EL1", 3, 0, 2, 1, 1},
    {"APIAKeyLo_EL1", 3, 0, 2, 1, 0},
    {"APIBKeyHi_EL1", 3, 0, 2, 1, 3},
    {"APIBKeyLo_EL1", 3, 0, 2, 1, 2},
    {"CCSIDR2_EL1", 3, 1, 0, 0, 2},
    {"CCSIDR_EL1", 3, 1, 0, 0, 0},
    {"CLIDR_EL1", 3, 1, 0, 0, 1},
    {"CONTEXTIDR_EL1", 3, 0, 13, 0, 1},
    {"CONTEXTIDR_EL2", 3, 4, 13, 0, 1},
    {"CPACR_EL1", 3, 0, 1, 0, 2},
    {"CPTR_EL2", 3, 4, 1, 1, 2},
    {"CPTR_EL3", 3, 6, 1, 1, 2},
    {"CSSELR_EL1", 3, 2, 0, 0, 0},
    {"CTR_EL0", 3, 3, 0, 0, 1},
    {"DACR32_EL2", 3, 4, 3, 0, 0},
    {"DCZID_EL0", 3, 3, 0, 0, 7},
    {"ESR_EL1", 3, 0, 5, 2, 0},
    {"ESR_EL2", 3, 4, 5, 2, 0},
    {"ESR_EL3", 3, 6, 5, 2, 0},
    {"FAR_EL1", 3, 0, 6, 0, 0},
    {"FAR_EL2", 3, 4, 6, 0, 0},
    {"FAR_EL3", 3, 6, 6, 0, 0},
    {"FPEXC32_EL2", 3, 4, 5, 3, 0},
    {"GCR_EL1", 3, 0, 1, 0, 6},
    {"GMID_EL1", 0, 3, 1, 4, 0},
    {"HACR_EL2", 3, 4, 1, 1, 7},
    {"HCR_EL2", 3, 4, 1, 1, 0},
    {"HPFAR_EL2", 3, 4, 6, 0, 4},
    {"HSTR_EL2", 3, 4, 1, 1, 3},
    {"ID_AA64AFR0_EL1", 3, 0, 0, 5, 4},
    {"ID_AA64AFR1_EL1", 3, 0, 0, 5, 5},
    {"ID_AA64DFR0_EL1", 3, 0, 0, 5, 0},
    {"ID_AA64FFR1_EL1", 3, 0, 0, 5, 1},
    {"ID_AA64ISAR0_EL1", 3, 0, 0, 6, 0},
    {"ID_AA64ISAR1_EL1", 3, 0, 0, 6, 1},
    {"ID_AA64MMFR0_EL1", 3, 0, 0, 7, 0},
    {"ID_AA64MMFR1_EL1", 3, 0, 0, 7, 1},
    {"ID_AA64MMFR2_EL1", 3, 0, 0, 7, 2},
    {"ID_AA64PFR0_EL1", 3, 0, 0, 4, 0},
    {"ID_AA64PFR1_EL1", 3, 0, 0, 4, 1},
    //ID registers
    {"ID_AFR0_EL1", 3, 0, 0, 1, 3},
    {"ID_DFR0_EL1", 3, 0, 0, 1, 2},
    {"ID_ISAR0_EL1", 3, 0, 0, 2, 0},
    {"ID_ISAR1_EL1", 3, 0, 0, 2, 1},
    {"ID_ISAR2_EL1", 3, 0, 0, 2, 2},
    {"ID_ISAR3_EL1", 3, 0, 0, 2, 3},
    {"ID_ISAR4_EL1", 3, 0, 0, 2, 4},
    {"ID_ISAR5_EL1", 3, 0, 0, 2, 5},
    {"ID_ISAR6_EL1", 3, 0, 0, 2, 7},
    {"ID_MMFR0_EL1", 3, 0, 0, 1, 4},
    {"ID_MMFR1_EL1", 3, 0, 0, 1, 5},
    {"ID_MMFR2_EL1", 3, 0, 0, 1, 6},
    {"ID_MMFR3_EL1", 3, 0, 0, 1, 7},
    {"ID_MMFR4_EL1", 3, 0, 0, 2, 6},
    {"ID_PFR0_EL1", 3, 0, 0, 1, 0},
    {"ID_PFR1_EL1", 3, 0, 0, 1, 1},
    {"ID_PFR2_EL1", 3, 0, 0, 3, 4},
    {"IFSR32_EL2", 3, 4, 5, 0, 1},
    {"ISR_EL1", 3, 0, 12, 1, 0},
    {"LORC_EL1", 3, 0, 10, 4, 3},
    {"LOREA_EL1", 3, 0, 10, 4, 1},
    {"LORID_EL1", 3, 0, 10, 4, 7},
    {"LORN_EL1", 3, 0, 10, 4, 2},
    {"LORSA_EL1", 3, 0, 10, 4, 0},
    {"MAIR_EL1", 3, 0, 10, 2, 0},
    {"MAIR_EL2", 3, 0, 10, 2, 0},
    {"MAIR_EL3", 3, 6, 10, 2, 0},
    {"MIDR_EL1", 3, 0, 0, 0, 0},
    {"MPIDR_EL1", 3, 0, 0, 0, 5},
    {"MVFR0_EL1", 3, 0, 0, 3, 0},
    {"MVFR1_EL1", 3, 0, 0, 3, 1},
    {"MVFR2_EL1", 3, 0, 0, 3, 2},
    {"PAR_EL1", 3, 0, 7, 4, 0},
    {"REVIDR_EL1", 3, 0, 0, 0, 6},
    {"RGSR_EL1", 3, 0, 1, 0, 5},
    {"RMR_EL1", 3, 0, 12, 0, 2},
    {"RMR_EL2", 3, 4, 12, 0, 2},
    {"RMR_EL3", 3, 6, 12, 0, 2},
    {"RNDR", 3, 3, 2, 4, 0},
    {"RNDRRS", 3, 3, 2, 4, 1},
    {"RVBAR_EL1", 3, 0, 12, 0, 1},
    {"RVBAR_EL2", 3, 4, 12, 0, 1},
    {"SCR_EL3", 3, 6, 1, 1, 0},
    {"SCTLR_EL1", 3, 0, 1, 0, 0},
    {"SCTLR_EL2", 3, 4, 1, 0, 0},
    {"SCTLR_EL3", 3, 6, 1, 0, 0},
    {"SCXTNUM_EL0", 3, 3, 13, 0, 7},
    {"SCXTNUM_EL1", 3, 0, 13, 0, 7},
    {"SCXTNUM_EL2", 3, 4, 13, 0, 7},
    {"SCXTNUM_EL3", 3, 6, 13, 0, 7},
    {"TCR_EL1", 3, 0, 2, 0, 2},
    {"TCR_EL2", 3, 4, 2, 0, 2},
    {"TCR_EL3", 3, 6, 2, 0, 2},
    {"TFSRE0_EL1", 3, 0, 5, 6, 1},
    {"TFSR_EL1", 3, 0, 5, 6, 0},
    {"TFSR_EL2", 3, 4, 5, 6, 0},
    {"TFSR_EL3", 3, 6, 5, 6, 0},
    {"TPIDR_EL0", 3, 3, 13, 0, 2},
    {"TPIDR_EL1", 3, 0, 13, 0, 4},
    {"TPIDR_EL2", 3, 4, 13, 0, 2},
    {"TPIDR_EL3", 3, 6, 13, 0, 2},
    {"TPIDRRO_EL0", 3, 3, 13, 0, 3},
    {"TTBR0_EL1", 3, 0, 2, 0, 0},
    {"TTBR0_EL2", 3, 4, 2, 0, 0},
    {"TTBR0_EL3", 3, 6, 2, 0, 0},
    {"TTBR1_EL1", 3, 0, 2, 0, 1},
    {"TTBR1_EL2", 3, 4, 2, 0, 1},
    {"VBAR_EL1", 3, 0, 12, 0, 0},
    {"VBAR_EL2", 3, 4, 12, 0, 0},
    {"VBAR_EL3", 3, 6, 12, 0, 0},
    {"VMPIDR_EL2", 3, 4, 0, 0, 5},
    {"VNCR_EL2", 3, 4, 2, 2, 0},
    {"VPIDR_EL2", 3, 4, 0, 0, 0},
    {"VSTCR_EL2", 3, 4, 2, 6, 2},
    {"VSTTBR_EL2", 3, 4, 2, 6, 0},
    {"VTCR_EL2", 3, 4, 2, 1, 2},
    {"VTTBR_EL2", 3, 4, 2, 1, 0},
    //{"DLR_EL0", 3, 3, 4, 5, 1},
    {"S3_C15_C1", 3, 1, 0xf, 1, 0},
    {"MDSCR_EL1", 2, 0, 0, 2, 2},
    {"CNTFRQ_EL0", 3, 3, 0xe, 0, 0},
    {"CNTKCTL_EL1", 3, 0, 0xe, 1, 0},
    {"CNTP_CTL_EL0", 3, 3, 0xe, 2, 1},
    {"CNTP_CVAL_EL0", 3, 3, 0xe, 2, 2},
    {"CNTP_TVAL_EL0", 3, 3, 0xe, 2, 0},
    {"CNTPCT_EL0", 3, 3, 0xe, 0, 1},
    {"CNTV_CTL_EL0", 3, 3, 0xe, 3, 1},
    {"CNTV_CVAL_EL0", 3, 3, 0xe, 3, 2},
    {"CNTV_TVAL_EL0", 3, 3, 0xe, 3, 0},
    {"CNTVCT_EL0", 3, 3, 0xe, 0, 2},
};

static void arm_opc_reserved(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void arm_opc_unallocated(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void arm_opc_sve(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void arm_opc_load_and_store(ARMul_State * state)
{
    ARMword instr = state->instr;

    ARMword op0, op1, op2, op3, op4;

    ARM_OPC_GET_LD_ST_OPC(op0, op1, op2, op3, op4);

    if ((op0 == 0 || op0 == 4) && op1 == 1)
    {
        if ((op2 == 0 && op3 == 0) || (op2 == 1 && (op3 & 0x20) == 0))
        {
            //SIMD load/store multiple structures
            //SIMD load/store multiple structures (post-indexed)
            ldst_multiple_struct(state);
        } else if (op2 == 2 && (op3 & 0x1f) == 0 || op2 == 3)
        {
            //SIMD load/store single structures
            //SIMD load/store single structures (post-indexed)
            ldst_single_struct(state);
        } else
        {
            UNALLOCATED_INSTR;
            goto OutBranch;
        }

        goto OutBranch;
    }

    if (op0 == 0xd && op1 == 0 && (op2 & 0x2) && (op3 & 0x20))
    {
        //Load/store memory tags
        load_store_memory_tags_instr_proc(state);
        goto OutBranch;
    }

    if ((op0 & 0x3) == 0 && op1 == 0 && (op2 & 0x2) == 0)
    {
        //Load/store exclusive
        load_store_exclusive_instr_proc(state);
        goto OutBranch;
    }

    if ((op0 & 0x3) == 1 && op1 == 0 && (op2 & 0x2) && op4 == 0 && (op3 & 0x20) == 0)
    {
        //LDAPR/STLR (unscaled immediate)
        ldapr_stlr(state);
        goto OutBranch;
    }

    if ((op0 & 0x3) == 1 && (op2 & 0x2) == 0)
    {
        //Load register (literal)
        load_register_literal(state);
        goto OutBranch;
    }

    if ((op0 & 0x3) == 2)
    {
        switch (op2)
        {
            case 0:
                //load/store no-allocate pair(offset)
                load_store_no_allocate_pair_offset(state);
                break;
            case 1:
            case 2:
            case 3:
                //load/store register pair(post-indexed)
                //load/store register pair(offset)
                //load/store register pair(pre-indexed)
                load_store_register_pair(state);
                break;
            default:
                break;
        }
        goto OutBranch;
    }

    if ((op0 & 0x3) == 3 && (op2 & 0x2) == 0)
    {
        if ((op3 & 0x20) == 0)
        {
            switch (op4)
            {
                case 0:
                    //Load/store register (unscaled immediate)
                    load_store_register_unscaled_imm(state);
                    break;
                case 1:
                case 3:
                    //Load/store register (immediate post-indexed)
                    //Load/store register (immediate pre-indexed)
                    load_store_register_imm9(state);
                    break;
                case 2:
                    //Load/store register (unprivileged)
                    load_store_register_unpriv(state);
                    break;
                default:
                    break;
            }
        } else
        {
            if (op4 == 0)
            {
                //Atomic memory operations
                atomic_mem(state);
            } else if (op4 == 2)
            {
                //Load/store register (register offset)
                load_store_register_offset(state);
            } else
            {
                //Load/store register (pac)
                load_store_register_pac(state);
            }
        }

        goto OutBranch;
    }

    if ((op0 & 0x3) == 3 && (op2 & 0x2))
    {
        //Load/store register (unsigned immediate)
        load_store_register_uimm(state);
        goto OutBranch;
    }

    UNALLOCATED_INSTR;
  OutBranch:
    state->A64_PC += 4;
    return;
}

static void arm_opc_data_process_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword op0, op1, op2, op3;

    ARM_OPC_GET_DATA_PROC_REG_OPC(op0, op1, op2, op3);
    if (op0 == 0 && op1 == 1 && op2 == 6)
    {
        //Data-processing (2 source)
        data_process_register_two_src(state);
    } else if (op0 == 1 && op1 == 1 && op2 == 6)
    {
        //Data-processing (1 source)
        data_process_register_one_src(state);
    } else if (op1 == 0 && op2 <= 7)
    {
        //Logical(shifted register)
        logical_shift_register(state);
    } else if (op1 == 0 && (op2 > 7))
    {
        if ((op2 % 2) == 0)
            //Add/subract(shift register)
            add_sub_shift_reg(state);
        else
            //Add/subract(extended register)
            add_sub_extend_reg(state);
    } else if (op1 == 1)
    {
        if (op2 == 0 && op3 == 0)
        {
            //Add/subtract(with carry)
            add_sub_with_carry(state);
        } else if (op2 == 0 && (op3 % 2) == 1)
        {
            //Rotate right into flags
            rotate_right(state);
        } else if (op2 == 0 && (op3 % 2) == 0 && op3 != 0)
        {
            //Evaluate into flags
            evaluate(state);
        } else if (op2 == 4)
        {
            //Conditional select
            cond_select(state);
        } else if (op2 > 7)
        {
            //Data-processing (3 source)
            data_process_register_three_src(state);
        } else if (op2 == 2)
        {
            if ((op3 >> 1 & 0x1) == 0)
            {
                //Conditional compare(register)
                cond_cmp_register(state);
            } else
            {
                //Conditional compare(immediate)
                cond_cmp_imm(state);
            }
        }
    } else
    {
        UNALLOCATED_INSTR;
        return;
    }

    state->A64_PC += 4;
    return;
}

static void data_process_register_two_src(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, S, opcode;

    ARM_OPC_GET_DATA_PROC_SRC_OPC(sf, S, opcode);
    if (sf == 0)
    {
        switch (opcode)
        {
            case 2:
                //UDIV 32-bit variant
                udiv32(state);
                break;
            case 3:
                //SDIV 32-bit variant
                sdiv32(state);
                break;
            case 8:
                //LSLV 32-bit variant
                lslv32(state);
                break;
            case 9:
                //LSRV 32-bit variant
                lsrv32(state);
                break;
            case 10:
                //ASRV 32-bit variant
                asrv32(state);
                break;
            case 11:
                //RORV 32-bit variant
                rorv32(state);
                break;
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x14:
            case 0x15:
            case 0x16:
                //CRC32B CRC32H, CRC32W,CRC32X-CRC32B variant
                //CRC32B CRC32H, CRC32W,CRC32X-CRC32H variant
                //CRC32B CRC32H, CRC32W,CRC32X-CRC32W variant
                //CRC32CB CRC32CH, CRC32CW,CRC32CX-CRC32CB variant
                //CRC32CB CRC32CH, CRC32CW,CRC32CX-CRC32CH variant
                //CRC32CB CRC32CH, CRC32CW,CRC32CX-CRC32CW variant
                crc32_32(state);
                break;
            default:
                break;
        }
    } else
    {
        if (S == 1)
        {
            //SUBPS
            NOT_IMPLEMENT;
            return;
        }

        switch (opcode)
        {
            case 0:
                //SUBP
                subp(state);
                break;
            case 2:
                //UDIV-64-bit variant
                udiv64(state);
                break;
            case 3:
                //SDIV-64-bit variant
                sdiv64(state);
                break;
            case 4:
                //IRG
                irg(state);
                break;
            case 5:
                //GMI
                gmi(state);
                break;
            case 8:
                //LSLV 64-bit variant
                lslv64(state);
                break;
            case 9:
                //LSRV 64-bit variant
                lsrv64(state);
                break;
            case 10:
                //ASRV 64-bit variant
                asrv64(state);
                break;
            case 11:
                //RORV 64-bit variant
                rorv64(state);
                break;
            case 12:
                //PACGA
                pacga(state);
                break;
            case 0x13:
            case 0x17:
                //CRC32B CRC32H, CRC32W,CRC32X-CRC32X variant
                //CRC32CB CRC32CH, CRC32CW,CRC32CX-CRC32CX variant
                crc32_64(state);
                break;
        }
    }
}

/* Data-processing (1 source)
 *   31  30  29  28             21 20     16 15    10 9    5 4    0
 * +----+---+---+-----------------+---------+--------+------+------+
 * | sf | 1 | S | 1 1 0 1 0 1 1 0 | opcode2 | opcode |  Rn  |  Rd  |
 * +----+---+---+-----------------+---------+--------+------+------+
 */
static void data_process_register_one_src(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf_S, opcode2, opcode, Rn;

    ARM_OPC_GET_ONE_SRC(sf_S, opcode2, opcode, Rn) if (opcode2 == 0)
    {
        switch (opcode)
        {
            case 0:
                //RBIT instruction
                if (sf_S == 0 || sf_S == 2)
                    rbit(state);
                else
                    UNALLOCATED_INSTR;
                break;
            case 1:
                //REV16 instruction
                if (sf_S == 0 || sf_S == 2)
                    rev16(state);
                else
                    UNALLOCATED_INSTR;
                break;
            case 2:
            case 3:
                //REV instruction
                if (sf_S == 0 || sf_S == 2)
                    rev(state);
                else
                    UNALLOCATED_INSTR;
                break;
            case 4:
                //CLZ instruction
                if (sf_S == 0 || sf_S == 2)
                    clz(state);
                else
                    UNALLOCATED_INSTR;
                break;
            case 5:
                //CLS instruction
                if (sf_S == 0 || sf_S == 2)
                    cls(state);
                else
                    UNALLOCATED_INSTR;
                break;
            default:
                UNALLOCATED_INSTR;
                break;
        }
        return;
    }

    if (opcode2 == 1)
    {
        switch (opcode)
        {
            case 0:
                pacia(state);
                break;
            case 1:
                pacib(state);
                break;
            case 2:
                pacda(state);
                break;
            case 3:
                pacdb(state);
                break;
            case 4:
                autia(state);
                break;
            case 5:
                autib(state);
                break;
            case 6:
                autda(state);
                break;
            case 7:
                autdb(state);
                break;
            case 8:
                if (Rn == 0x1F)
                    pacia(state);
                else
                    UNALLOCATED_INSTR;
                break;
            case 9:
                if (Rn == 0x1F)
                    pacib(state);
                else
                    UNALLOCATED_INSTR;
                break;
            case 10:
                if (Rn == 0x1F)
                    pacda(state);
                else
                    UNALLOCATED_INSTR;
                break;
            case 11:
                if (Rn == 0x1F)
                    pacdb(state);
                else
                    UNALLOCATED_INSTR;
                break;
            case 12:
                if (Rn == 0x1F)
                    autia(state);
                else
                    UNALLOCATED_INSTR;
                break;
            case 13:
                if (Rn == 0x1F)
                    autib(state);
                else
                    UNALLOCATED_INSTR;
                break;
            case 14:
                if (Rn == 0x1F)
                    autda(state);
                else
                    UNALLOCATED_INSTR;
                break;
            case 15:
                if (Rn == 0x1F)
                    autdb(state);
                else
                    UNALLOCATED_INSTR;
                break;
            case 0x10:
                if (Rn == 0x1F)
                    xpaci(state);
                else
                    UNALLOCATED_INSTR;
                break;
            case 0x11:
                if (Rn == 0x1F)
                    xpacd(state);
                else
                    UNALLOCATED_INSTR;
                break;
            default:
                UNALLOCATED_INSTR;
                break;
        }

        return;
    }

    UNALLOCATED_INSTR;
    return;
}

/* Logical (shifted register)
 *   31  30 29 28       24 23   22 21  20  16 15    10 9    5 4    0
 * +----+-----+-----------+-------+---+------+--------+------+------+
 * | sf | opc | 0 1 0 1 0 | shift | N |  Rm  |  imm6  |  Rn  |  Rd  |
 * +----+-----+-----------+-------+---+------+--------+------+------+
 */
static void logical_shift_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, opc, N, opcode;

    ARM_OPC_GET_LOGICAL_SHIFT_REG_OPC(sf, opc, N, opcode);
    switch (opcode)
    {
        case 0:
            and_shift_register(state);
            break;
        case 1:
            bic_shift_register(state);
            break;
        case 2:
            orr_shift_register(state);
            break;
        case 3:
            orn_shift_register(state);
            break;
        case 4:
            eor_shift_register(state);
            break;
        case 5:
            eon_shift_register(state);
            break;
        case 6:
            ands_shift_register(state);
            break;
        case 7:
            bics_shift_register(state);
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

/*
 * Add/subtract (shifted register)
 *
 *  31 30 29 28       24 23 22 21 20   16 15     10 9    5 4    0
 * +--+--+--+-----------+-----+--+-------+---------+------+------+
 * |sf|op| S| 0 1 0 1 1 |shift| 0|  Rm   |  imm6   |  Rn  |  Rd  |
 * +--+--+--+-----------+-----+--+-------+---------+------+------+
 *
 *    sf: 0 -> 32bit, 1 -> 64bit
 *    op: 0 -> add  , 1 -> sub
 *     S: 1 -> set flags
 * shift: 00 -> LSL, 01 -> LSR, 10 -> ASR, 11 -> RESERVED
 *  imm6: Shift amount to apply to Rm before the add/sub
 */
static void add_sub_shift_reg(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, op_S;

    ARM_OPC_GET_ARITH_SHIFT_REG_OPC(sf, op_S);

    switch (op_S)
    {
        case 0:
            add_shift_register(state);
            break;
        case 1:
            adds_shift_register(state);
            break;
        case 2:
            sub_shift_register(state);
            break;
        case 3:
            subs_shift_register(state);
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

/*
 * Add/subtract (extended register)
 *
 *  31|30|29|28       24|23 22|21|20   16|15  13|12  10|9  5|4  0|
 * +--+--+--+-----------+-----+--+-------+------+------+----+----+
 * |sf|op| S| 0 1 0 1 1 | opt | 1|  Rm   |option| imm3 | Rn | Rd |
 * +--+--+--+-----------+-----+--+-------+------+------+----+----+
 *
 *  sf: 0 -> 32bit, 1 -> 64bit
 *  op: 0 -> add  , 1 -> sub
 *   S: 1 -> set flags
 * opt: 00
 * option: extension type (see DecodeRegExtend)
 * imm3: optional shift to Rm
 *
 * Rd = Rn + LSL(extend(Rm), amount)
 */
static void add_sub_extend_reg(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, op_S, opt;

    ARM_OPC_GET_ARITH_EXTEND_REG_OPC(sf, op_S, opt);

    if (opt == 0)
    {
        switch (op_S)
        {
            case 0:
                add_extend_register(state);
                break;
            case 1:
                adds_extend_register(state);
                break;
            case 2:
                sub_extend_register(state);
                break;
            case 3:
                subs_extend_register(state);
                break;
            default:
                UNALLOCATED_INSTR;
                break;
        }
    } else
    {
        UNALLOCATED_INSTR;
    }
}

static void add_sub_with_carry(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, op_S;

    ARM_OPC_GET_ARITH_SHIFT_REG_OPC(sf, op_S);
    switch (op_S)
    {
        case 0:
            adc(state);
            break;
        case 1:
            adcs(state);
            break;
        case 2:
            sbc(state);
            break;
        case 3:
            sbcs(state);
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

static void rotate_right(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, op_S, o2;

    ARM_OPC_GET_ROTATE_OPC(sf, op_S, o2);
    if (sf == 1 && op_S == 1 && o2 == 0)
        rmif(state);
    else
        UNALLOCATED_INSTR;
}

static void evaluate(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, op_S, opcode2, sz, o3, mask;

    ARM_OPC_GET_EVALUATE_OPC(sf, op_S, opcode2, sz, o3, mask)
        if (sf == 0 && op_S == 1 && opcode2 == 0 && o3 == 0 && mask == 0xd)
    {
        if (sz == 0)
        {
            //SETF8
            setf8(state);
        } else
        {
            //SETF16
            setf16(state);
        }
    } else
        UNALLOCATED_INSTR;
}

/* Conditional select
 *   31   30  29  28             21 20  16 15  12 11 10 9    5 4    0
 * +----+----+---+-----------------+------+------+-----+------+------+
 * | sf | op | S | 1 1 0 1 0 1 0 0 |  Rm  | cond | op2 |  Rn  |  Rd  |
 * +----+----+---+-----------------+------+------+-----+------+------+
 */
static void cond_select(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, op, S, op2, opcode;

    ARM_OPC_GET_COND_SELECT_OPC(sf, op, S, op2, opcode);

    if (opcode == 0)
    {
        csel(state);
    } else if (opcode == 1)
    {
        csinc(state);
    } else if (opcode == 8)
    {
        csinv(state);
    } else if (opcode == 9)
    {
        csneg(state);
    } else
    {
        UNALLOCATED_INSTR;
    }
}

static void data_process_register_three_src(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, op54, op31, o0;

    ARM_OPC_GET_THREE_SRC_OPC(sf, op54, op31, o0);

    if (op54 == 0)
    {
        if (op31 == 0)
        {
            if (o0 == 0)
                madd(state);
            else
                msub(state);
        } else if (op31 == 1)
        {
            if (o0 == 0)
                smaddl(state);
            else
                smsubl(state);
        } else if (op31 == 2 && o0 == 0)
        {
            smulh(state);
        } else if (op31 == 5 && o0 == 0)
        {
            umaddl(state);
        } else if (op31 == 5 && o0 == 1)
        {
            umsubl(state);
        } else if (op31 == 6 && o0 == 0)
        {
            umulh(state);
        } else
        {
            UNALLOCATED_INSTR;
        }
    } else
    {
        UNALLOCATED_INSTR;
    }
}

static void cond_cmp_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, op, S, o2, o3, opcode;

    ARM_OPC_GET_COND_CMP_OPC(sf, op, S, o2, o3);
    opcode = op << 3 | S << 2 | o2 << 1 | o3;

    if (opcode == 4)
    {
        ccmn_register(state);
    } else if (opcode == 12)
    {
        ccmp_register(state);
    } else
    {
        UNALLOCATED_INSTR;
    }
}

static void cond_cmp_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, op, S, o2, o3, opcode;

    ARM_OPC_GET_COND_CMP_OPC(sf, op, S, o2, o3);
    opcode = op << 3 | S << 2 | o2 << 1 | o3;

    if (opcode == 4)
    {
        ccmn_imm(state);
    } else if (opcode == 12)
    {
        ccmp_imm(state);
    } else
    {
        UNALLOCATED_INSTR;
    }
}

/* FP-specific subcases of table C3-6 (SIMD and FP data processing)
 *   31  30  29 28     25 24                          0
 * +---+---+---+---------+-----------------------------+
 * |   | 0 |   | 1 1 1 1 |                             |
 * +---+---+---+---------+-----------------------------+
 */
static void arm_opc_data_process_fp_and_simd(ARMul_State * state)
{
    ARMword instr = state->instr;

    DEBUG_INSN;
    if (BITS(28, 28) == 1 && BITS(30, 30) == 0)
    {
        disas_data_proc_fp(state);
    } else
    {
        /* SIMD, including crypto */
        disas_data_proc_simd(state);
    }

    state->A64_PC += 4;
}

static void arm_opc_data_process_immediate(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword op0 = BITS(23, 25);

    switch (op0)
    {
        case 0:
        case 1:
            //PC-rel. addressing
            pc_rel_adr_instr_proc(state);
            break;
        case 2:
            //Add/subtract(immediate)
            add_sub_imm_instr_proc(state);
            break;
        case 3:
            //Add/subtract(immediate, with tags)
            add_sub_imm_tag_instr_proc(state);
            break;
        case 4:
            //logical(immediate)
            logical_imm_instr_proc(state);
            break;
        case 5:
            //Move wide(immediate)
            mov_imm_instr_proc(state);
            break;
        case 6:
            //Bitfield
            bitfield_instr_proc(state);
            break;
        case 7:
            //Extract
            extract_instr_proc(state);
            break;
        default:
            printf("In %s. op0:0x%x not implement!\n", __func__, op0);
            break;
    }

    state->A64_PC += INSN_SIZE;
}

static void arm_opc_branch_exception_system(ARMul_State * state)
{
    ARMword op0, op1, op2;
    ARMword instr = state->instr;

    op0 = BITS(29, 31);
    op1 = BITS(12, 25);
    op2 = BITS(0, 4);

    //Unconditional branch (immediate)
    if (op0 == 0 || op0 == 4)
    {
        arm_opc_uncond_branch_imm(state);
        return;
    }
    //Compare and branch (immediate) or Test and branch (immediate)
    if (op0 == 1 || op0 == 5)
    {
        ARMword tmp_op = BITS(25, 25);

        if (tmp_op == 0)
            //Compare and branch (immediate)
            arm_opc_cmp_and_branch_imm(state);
        else
            //Test and branch (immediate)
            arm_opc_test_and_branch_imm(state);

        return;
    }
    //Conditional branch (immediate)
    if (op0 == 2)
    {
        arm_opc_cond_branch_imm(state);
        return;
    }

    if (op0 == 6)
    {
        ARMword tmp_op = BITS(24, 25);

        if (tmp_op == 0)
        {
            //Exception generation
            arm_opc_exception_generation(state);
            return;
        } else if (tmp_op == 1)
        {
            if (BITS(12, 25) == HINTS_OPC)
            {
                //Hints
                arm_opc_hints(state);
                return;
            }

            if (BITS(12, 25) == BARRIERS_OPC)
            {
                //Barriers
                arm_opc_barriers(state);
                return;
            }

            if (BITS(12, 15) == 0x4 && BITS(19, 25) == 0x20)
            {
                //PSTATE
                arm_opc_pstate(state);
                return;
            }

            if (BITS(22, 25) == 0x4 && BITS(19, 20) == 0x1)
            {
                arm_opc_system_instr(state);
                state->A64_PC += 4;
                return;
            }

            if (BITS(22, 25) == 0x4 && BITS(19, 20) > 0x1)
            {
                arm_opc_system_register_move(state);
                state->A64_PC += 4;
                return;
            }

        } else
        {
            arm_opc_uncond_branch_register(state);
            return;
        }
    }
}

static void arm_opc_uncond_branch_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword op, imm26;
    ARMdword offset;

    ARM_OPC_GET_IMM26(op, imm26);
    offset = SignExtend64(imm26, 26);

    if (op == 1)
    {
        //BL instruction
        WRITE_X_REG(30, state->A64_PC + 4);
    }
    //To branch
    state->A64_PC += offset * 4;
    DEBUG_INSN;
}

static void arm_opc_cmp_and_branch_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, op;

    ARM_OPC_GET_CMP_BRANCH_OPC(sf, op);

    if (op == 0)
    {
        //CBZ-64-bit or 32-bit variant
        cbz(state);
    } else
    {
        //CBNZ-64-bit or 32-bit variant
        cbnz(state);
    }
}

static void arm_opc_test_and_branch_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword op = BITS(24, 24);

    if (op == 0)
    {
        //TBZ instruction
        tbz(state);
    } else
    {
        //TBNZ instruction
        tbnz(state);
    }
}

static void arm_opc_cond_branch_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword o1, imm19, o0, cond;
    ARMdword offset;
    bool_t cond_ret;

    ARM_OPC_GET_COND_BRANCH(o1, imm19, o0, cond);
    if (o1 == 0 && o0 == 0)
    {
        //B.cond instruction
        offset = SignExtend64(imm19, 19) * 4;
    } else
    {
        //Unallocated
        UNALLOCATED_INSTR;
        return;
    }

    cond_ret = ConditionHolds(state, cond);

    if (cond_ret)
    {
        //branch to PC + offset
        state->A64_PC += offset;
    } else
        state->A64_PC += 4;
    DEBUG_INSN;
}

static void arm_opc_exception_generation(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword opc, op2, LL;

    ARM_OPC_GET_EXC_OPC(opc, op2, LL);

    if (opc == 0 && op2 == 0)
    {
        if (LL == 1)
        {
            //SVC instruction
            svc(state);
        } else if (LL == 2)
        {
            UNALLOCATED_INSTR;
            //HVC instruction
            hvc(state);
        } else if (LL == 3)
        {
            //SMC instruction
            smc(state);
        } else
        {
            UNALLOCATED_INSTR;
        }
    } else if (opc == 1 && op2 == 0 && LL == 0)
    {
        //BRK instruction
        ibrk(state);
    } else if (opc == 2 && op2 == 0 && LL == 0)
    {
        //HLT instruction
        hlt(state);
    } else if (opc == 5)
    {
        if (LL == 1)
        {
            //DCPS1 instruction
            dcps1(state);
        } else if (LL == 2)
        {
            //DCPS2 instruction
            dcps2(state);
        } else if (LL == 3)
        {
            //DCPS3 instruction
            dcps3(state);
        } else
        {
            UNALLOCATED_INSTR;
        }
    } else
    {
        UNALLOCATED_INSTR;
    }
}

static void arm_opc_hints(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword CRm, op2;

    ARM_OPC_GET_HINT_OPC(CRm, op2);

    if (CRm == 0)
    {
        switch (op2)
        {
            case 0:
                //NOP instruction
                nop(state);
                break;
            case 1:
                //YIELD instruction
                yield(state);
                break;
            case 2:
                //WFE instruction
                wfe(state);
                break;
            case 3:
                //WFI instruction
                wfi(state);
                break;
            case 4:
                //SEV instruction
                sev(state);
                break;
            case 5:
                //SEVL instruction
                sevl(state);
                break;
            case 7:
                //XPACD,XPACI,XPACLRI instruction
                xpac_instr_proc(state);
                break;
            default:
                UNALLOCATED_INSTR;
                break;
        }
    } else if (CRm == 1)
    {
        switch (op2)
        {
            case 0:
                pacia1716_instr_proc(state);
                break;
            case 2:
                pacib1716_instr_proc(state);
                break;
            case 4:
                autia1716_instr_proc(state);
                break;
            case 6:
                autib1716_instr_proc(state);
                break;
            default:
                UNALLOCATED_INSTR;
                break;
        }
    } else if (CRm == 2)
    {
        switch (op2)
        {
            case 0:
                //ESB instruction
                esb(state);
                break;
            case 1:
                //PSB CSYNC instruction
                psb_csync(state);
                break;
            case 2:
                //TSB CSYNC instruction
                tsb_csync(state);
                break;
            case 4:
                //CSDB instruction
                csdb(state);
                break;
            default:
                UNALLOCATED_INSTR;
                break;
        }
    } else if (CRm == 3)
    {
        switch (op2)
        {
            case 0:
                paciaz_instr_proc(state);
                break;
            case 1:
                paciasp_instr_proc(state);
                break;
            case 2:
                pacibz_instr_proc(state);
                break;
            case 3:
                pacibsp_instr_proc(state);
                break;
            case 4:
                autiaz_instr_proc(state);
                break;
            case 5:
                autiasp_instr_proc(state);
                break;
            case 6:
                autibz_instr_proc(state);
                break;
            case 7:
                autibsp_instr_proc(state);
                break;
            default:
                UNALLOCATED_INSTR;
                break;
        }
    } else if (CRm == 4)
    {
        //BTI instruction
        bti(state);
    } else
    {
        UNALLOCATED_INSTR;
    }
    state->A64_PC += 4;
}

static void arm_opc_barriers(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword CRm, op2, Rt;

    ARM_OPC_GET_BARRIERS_OPC(CRm, op2, Rt);

    if (Rt == 0x1f)
    {
        switch (op2)
        {
            case 2:
                //CLREX instruction
                clrex(state);
                break;
            case 5:
                //DMB instruction
                dmb(state);
                break;
            case 6:
                //ISB instruction
                isb(state);
                break;
            case 7:
                //SB instruction
                sb(state);
                break;
            case 4:
                if (CRm != 0 || CRm != 4)
                {
                    dsb(state);
                } else if (CRm == 0)
                {
                    ssbb(state);
                } else if (CRm == 4)
                {
                    pssbb(state);
                } else
                {
                    UNALLOCATED_INSTR;
                }
                break;
            default:
                UNALLOCATED_INSTR;
                break;
        }
    } else
    {
        UNALLOCATED_INSTR;
    }
    state->A64_PC += 4;
}

static void arm_opc_pstate(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword op1, op2, Rt;

    ARM_OPC_GET_PSTATE_OPC(op1, op2, Rt);
    if (Rt == 0x1f)
    {
        if (op1 == 0)
        {
            if (op2 == 0)
            {
                //CFINV instruction
                cfinv(state);
            } else if (op2 == 1)
            {
                //XAFLAG instruction
                xaflag(state);
            } else if (op2 == 2)
            {
                //AXFLAG instruction
                axflag(state);
            } else
            {
                //MSR immediate
                msr_imm(state);
            }
        } else
        {
            //MSR immediate
            msr_imm(state);
        }
    } else
    {
        UNALLOCATED_INSTR;
    }

    state->A64_PC += 4;
}

static void arm_opc_system_instr(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword L, op1, CRn, CRm, op2, Rt;

    ARM_OPC_GET_SYSTEM_OPC(L, op1, CRn, CRm, op2, Rt);

    if (L)
    {
        //SYSL instruction
        NOT_IMPLEMENT;
    } else
    {
        //SYS instruction
        if (CRn == 7)
        {
            //IC instruction Cache
            ic(state);
        } else
        {
            NOT_IMPLEMENT;
        }
    }
}

static void arm_opc_system_register_move(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword L, op0, op1, op2, CRn, CRm, Rt;
    ARMword regIndex;
    ARMdword *regValue;
    SystemRegister_t *SysReg = &state->system_reg;

    ARM_OPC_GET_SYSTEM_ACCESS(L, op0, op1, CRn, CRm, op2, Rt);
    //get system register index
    regIndex = GetSystemRegIndex(op0, op1, CRn, CRm, op2);
    regValue = (ARMdword *) SysReg + regIndex;

    if (regIndex == -1)
    {
        printf("In %s. line:%d Not Found SystemRegister PC = 0x%llx Instr = 0x%x!\n", __func__, __LINE__,
               state->A64_PC, instr);
        return;
    } else if (regIndex < System_MAX_REG)
    {
        updatePEState(state, L, regIndex, Rt);
    } else
    {
        if (L == 0)
        {
            //MSR instruction
            SysRegWriteValue(Rt, regValue);
        } else
        {
            //MRS instruction
            SysRegReadValue(Rt, regValue);
        }
    }
    return;
}

static void arm_opc_uncond_branch_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword opc, op2, op3, Rn, op4;

    ARM_OPC_GET_UNCODE_BRANCH_REG_OPC(opc, op2, op3, Rn, op4);
    if (op2 == 0x1f)
    {
        if (op3 == 0 && op4 == 0)
        {
            if (opc == 0)
            {
                //BR instruction
                br(state);
                return;
            } else if (opc == 1)
            {
                //BLR instruction
                blr(state);
                return;
            } else if (opc == 2)
            {
                //RET instruction
                ret(state);
                return;
            } else if (opc == 4)
            {
                if (Rn == 0x1f)
                {
                    //ERET instruction
                    eret(state);
                    return;
                }
            } else if (opc == 5)
            {
                if (Rn == 0x1f)
                {
                    //DRPS instruction
                    drps(state);
                    return;
                }
            }
        } else if (op3 == 2 && op4 == 0x1f)
        {
            if (opc == 0)
            {
                //BRAA,BRAAZ,BRAB, BRABZ-Key A, zero modifer variant
                bra_keyA(state);
                return;
            } else if (opc == 1)
            {
                //BLRAA,BLRAAZ, BLRAB, BLRABZ-Key A, zero modifer variant
                blra_keyA(state);
                return;
            } else if (opc == 2)
            {
                if (Rn == 0x1f)
                {
                    //RETAA,RETAB-RETAA-variant
                    retaa(state);
                    return;
                }
            } else if (opc == 4)
            {
                if (Rn == 0x1f)
                {
                    //ERETAA,ERETAB-RETAA-variant
                    eretaa(state);
                    return;
                }
            }
        } else if (op3 == 3 && op4 == 0x1f)
        {
            if (opc == 0)
            {
                //BRAA,BRAAZ,BRAB, BRABZ-Key A, zero modifer variant
                bra_keyB(state);
                return;
            } else if (opc == 1)
            {
                //BLRAA,BLRAAZ, BLRAB, BLRABZ-Key B, zero modifer variant
                blra_keyB(state);
                return;
            } else if (opc == 2)
            {
                if (Rn == 0x1f)
                {
                    //RETAA,RETAB-RETAB-variant
                    retab(state);
                    return;
                }
            } else if (opc == 4)
            {
                if (Rn == 0x1f)
                {
                    //ERETAA,ERETAB-RETAB-variant
                    eretab(state);
                    return;
                }
            }
        } else if (opc == 0x8 && op3 == 2)
        {
            //BRAA,BRAAZ,BRAB,BRABZ-KeyA
            braa_keyA(state);
        } else if (opc == 0x8 && op3 == 3)
        {
            //BRAA,BRAAZ,BRAB,BRABZ-KeyB
            braa_keyB(state);
        } else if (opc == 0x9 && op3 == 2)
        {
            //BLRAA,BLRAAZ,BLRAB,BRABZ-KeyA
            blraa_keyA(state);
        } else if (opc == 0x9 && op3 == 3)
        {
            //BLRAA,BLRAAZ,BLRAB,BLRABZ-KeyB
            blraa_keyB(state);
        } else
        {
            UNALLOCATED_INSTR;
            return;
        }
    } else
    {
        UNALLOCATED_INSTR;
    }
}

/* PC-rel. addressing
 *   31  30   29 28       24 23                5 4    0
 * +----+-------+-----------+-------------------+------+
 * | op | immlo | 1 0 0 0 0 |       immhi       |  Rd  |
 * +----+-------+-----------+-------------------+------+
 */
static void pc_rel_adr_instr_proc(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword op, immlo, immhi, Rd;
    ARMdword imm, base;

    ARM_OPC_GET_DATA_ADR(op, immlo, immhi, Rd);

    /* SignExtend(immhi:immlo) -> imm */
    imm = sextract64(instr, 5, 19);
    imm = imm << 2 | immlo;
    base = state->A64_PC;

    if (op)
    {
        //ADRP instruction
        base &= 0xFFFFF000;
        imm <<= 12;
    }

    WRITE_X_REG(Rd, base + imm);
    DEBUG_INSN;

    return;
}

/*
 * Add/subtract (immediate)
 *
 *  31 30 29 28       24 23 22 21         10 9   5 4   0
 * +--+--+--+-----------+-----+-------------+-----+-----+
 * |sf|op| S| 1 0 0 0 1 |shift|    imm12    |  Rn | Rd  |
 * +--+--+--+-----------+-----+-------------+-----+-----+
 *
 *    sf: 0 -> 32bit, 1 -> 64bit
 *    op: 0 -> add  , 1 -> sub
 *     S: 1 -> set flags
 * shift: 00 -> LSL imm by 0, 01 -> LSL imm by 12
 */
static void add_sub_imm_instr_proc(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf_opS;

    ARM_OPC_GET_SF_OPS(sf_opS);

    switch (sf_opS)
    {
        case 0:
            //ADD(immediate) 32-bit variant
            add32_imm(state);
            break;
        case 1:
            //ADDS(immediate) 32-bit variant
            adds32_imm(state);
            break;
        case 2:
            //SUB(immediate) 32-bit variant
            sub32_imm(state);
            break;
        case 3:
            //SUBS(immediate) 32-bit variant
            subs32_imm(state);
            break;
        case 4:
            //ADD(immediate) 64-bit variant
            add64_imm(state);
            break;
        case 5:
            //ADDS(immediate) 64-bit variant
            adds64_imm(state);
            break;
        case 6:
            //SUB(immediate) 64-bit variant
            sub64_imm(state);
            break;
        case 7:
            //SUBS(immediate) 64-bit variant
            subs64_imm(state);
            break;
        default:
            printf("In %s. line:%d sf_opS = 0x%x Decode Fields Error!\n", __func__, __LINE__, sf_opS);
            break;
    }
}

/*
 * Add/subtract (immediate, with tags)
 *
 *  31 30 29 28        23 22 21      16 15 14 13  10 9   5 4   0
 * +--+--+--+------------+-----+-------------+-----+------------+
 * |sf|op| S| 1 0 0 0 1 1|o2|  uimm6  |  op3 | uimm4| Rn  | Rd |
 * +--+--+--+-----------+-----+-------------+-----+-------------+
 *
 */
static void add_sub_imm_tag_instr_proc(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf_opS;

    ARM_OPC_GET_SF_OPS(sf_opS);

    if (sf_opS == 4)
    {
        //ADDG instruction
        addg(state);
    } else if (sf_opS == 6)
    {
        //SUBG instruction
        subg(state);
    } else
    {
        UNALLOCATED_INSTR;
        //Unallocated
    }
}

/* Logical (immediate)
 *   31  30 29 28         23 22  21  16 15  10 9    5 4    0
 * +----+-----+-------------+---+------+------+------+------+
 * | sf | opc | 1 0 0 1 0 0 | N | immr | imms |  Rn  |  Rd  |
 * +----+-----+-------------+---+------+------+------+------+
 */
static void logical_imm_instr_proc(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf_opc;

    ARM_OPC_GET_SF_OPS(sf_opc);
    switch (sf_opc)
    {
        case 0:
            and32_imm(state);
            break;
        case 1:
            orr32_imm(state);
            break;
        case 2:
            eor32_imm(state);
            break;
        case 3:
            ands32_imm(state);
            break;
        case 4:
            and64_imm(state);
            break;
        case 5:
            orr64_imm(state);
            break;
        case 6:
            eor64_imm(state);
            break;
        case 7:
            ands64_imm(state);
            break;
        default:
            INSTR_DECODE_ERR;
            break;
    }
}

/*
 * Move wide (immediate)
 *
 *  31 30 29 28         23 22 21 20             5 4    0
 * +--+-----+-------------+-----+----------------+------+
 * |sf| opc | 1 0 0 1 0 1 |  hw |  imm16         |  Rd  |
 * +--+-----+-------------+-----+----------------+------+
 *
 * sf: 0 -> 32 bit, 1 -> 64 bit
 * opc: 00 -> N, 10 -> Z, 11 -> K
 * hw: shift/16 (0,16, and sf only 32, 48)
 */
static void mov_imm_instr_proc(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf_opc;

    ARM_OPC_GET_SF_OPS(sf_opc);

    switch (sf_opc)
    {
        case 0:
        case 4:
            //MOVN 32-bit or 64-bitvariant
            movn_imm(state);
            break;
        case 2:
        case 6:
            //MOVZ 32-bit or 64-bitvariant
            movz_imm(state);
            break;
        case 3:
        case 7:
            //MOVK 32-bit or 64-bit variant
            movk_imm(state);
            break;
        default:
            //Unallocated
            UNALLOCATED_INSTR;
            break;
    }
}

/* Bitfield
 *   31  30 29 28         23 22  21  16 15  10 9    5 4    0
 * +----+-----+-------------+---+------+------+------+------+
 * | sf | opc | 1 0 0 1 1 0 | N | immr | imms |  Rn  |  Rd  |
 * +----+-----+-------------+---+------+------+------+------+
 */
static void bitfield_instr_proc(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf_opc;

    ARM_OPC_GET_SF_OPS(sf_opc);
    bfm(state);
}

/* Extract
 *   31  30  29 28         23 22   21  20  16 15    10 9    5 4    0
 * +----+------+-------------+---+----+------+--------+------+------+
 * | sf | op21 | 1 0 0 1 1 1 | N | o0 |  Rm  |  imms  |  Rn  |  Rd  |
 * +----+------+-------------+---+----+------+--------+------+------+
 */
static void extract_instr_proc(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf_opc, N, o0, imms;

    ARM_OPC_GET_EXTRACT_OPC(sf_opc, N, o0, imms);

    if (sf_opc == 0 && N == 0 && o0 == 0 && (imms >> 5) == 0)
    {
        //EXTR 32-bit instruction
        extr32(state);
    } else if (sf_opc == 4 && N == 1 && o0 == 0)
    {
        //EXTR 64-bit instruction
        extr64(state);
    } else
    {
        UNALLOCATED_INSTR;
    }
}

bool_t ConditionHolds(ARMul_State * state, int cond)
{
    ARMword op = cond >> 1;
    ARMword op1 = cond & 1;
    bool_t result = False;

    switch (op)
    {
        case 0:
            result = state->PSTATE.Z == 1;
            break;
        case 1:
            result = state->PSTATE.C == 1;
            break;
        case 2:
            result = state->PSTATE.N == 1;
            break;
        case 3:
            result = state->PSTATE.V == 1;
            break;
        case 4:
            result = (state->PSTATE.C == 1 && state->PSTATE.Z == 0);
            break;
        case 5:
            result = state->PSTATE.N == state->PSTATE.V;
            break;
        case 6:
            result = (state->PSTATE.N == state->PSTATE.V && state->PSTATE.Z == 0);
            break;
        case 7:
            result = True;
            break;
        default:
            printf("In %s. line:%d cond = 0x%x Error!\n", __func__, __LINE__, cond);
            break;
    }

    if (op1 && cond != 0xF)
        result = !result;

    return result;
}

ARMword GetSystemRegIndex(int op0, int op1, int CRn, int CRm, int op2)
{
    SysReg_Decode_Opc SysReg;
    ARMdword SysRegData;
    ARMword index = -1;

    SysReg.content[0] = op0;
    SysReg.content[1] = op1;
    SysReg.content[2] = CRn;
    SysReg.content[3] = CRm;
    SysReg.content[4] = op2;

    if (decode_SysRegField(&SysReg, &index) == DECODE_OK)
    {
        return index;
    } else
    {
        printf("In %s, op0 = %x op1 = %x CRn = %x CRm = %x op2 = %x\n", __func__, op0, op1, CRn, CRm, op2);
        return -1;
    }
}

ARMword decode_SysRegField(SysReg_Decode_Opc * SysReg, int *index)
{
    int i;
    int n = 5;
    int base = 0;

    for (i = 0; i < sizeof (SysRegTable) / sizeof (SysReg_Decode_Opc); i++)
    {
        n = 5;
        base = 0;
        while (n)
        {
            if (SysReg->content[base] != SysRegTable[i].content[base])
                break;
            base++;
            n--;
        }

        if (n == 0)
        {
            SysReg->SysRegName = SysRegTable[i].SysRegName;
            *index = i;
            return DECODE_OK;
        }
    }

    return DECODE_ERR;
}

static void subp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void irg(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void gmi(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void addg(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword uimm6 = BITS(16, 21);
    ARMword uimm4 = BITS(10, 13);
    ARMword rn = BITS(5, 9);
    ARMword rd = BITS(0, 4);
    ARMdword offset, operand1, result;
    ARMword start_tag, exclude, rtag;

    offset = (ARMdword) uimm6 << LOG2_TAG_GRANULE;
    operand1 = READ_X_REG(rn);
    start_tag = GetTagFromAddress(operand1);
    exclude = state->system_reg.GCR_EL1 & 0xffff;

    if (AllocationTagAccessIsEnable(state))
        rtag = ChooseNonExcludedTag(start_tag, uimm4, exclude);
    else
        rtag = 0;

    result = A64_AddWithCarry(state, operand1, offset, 0, 0);

    result = AddressWithAllocationTag(state, result, rtag);
    WRITE_X_REG(rd, result);
    DEBUG_INSN;
}

static void subg(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword uimm6 = BITS(16, 21);
    ARMword uimm4 = BITS(10, 13);
    ARMword rn = BITS(5, 9);
    ARMword rd = BITS(0, 4);
    ARMdword offset, operand1, result;
    ARMword start_tag, exclude, rtag;

    offset = (ARMdword) uimm6 << LOG2_TAG_GRANULE;
    operand1 = READ_X_REG(rn);
    start_tag = GetTagFromAddress(operand1);
    exclude = state->system_reg.GCR_EL1 & 0xffff;

    if (AllocationTagAccessIsEnable(state))
        rtag = ChooseNonExcludedTag(start_tag, uimm4, exclude);
    else
        rtag = 0;

    offset = ~offset;
    result = A64_AddWithCarry(state, operand1, offset, 1, 0);

    result = AddressWithAllocationTag(state, result, rtag);
    WRITE_X_REG(rd, result);
    DEBUG_INSN;
}

static void and32_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword N, immr, imms, Rn, Rd;
    ARMdword imm;
    ARMword operand1, result;

    ARM_OPC_GET_AND_IMM_OPC(N, immr, imms, Rn, Rd);

    if (N != 0)
    {
        UNALLOCATED_INSTR;
        return;
    }

    if (!DecodeBitMasks(&imm, N, imms, immr))
    {
        UNALLOCATED_INSTR;
        return;
    }
    imm &= BIT32_MAX_VALUE;

    READ_W_REG(Rn, operand1);
    result = operand1 & imm;

    WRITE_W_REG(Rd, result);
    DEBUG_INSN;
}

static void orr32_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword N, immr, imms, Rn, Rd;
    ARMdword imm;
    ARMword operand1, result;

    ARM_OPC_GET_AND_IMM_OPC(N, immr, imms, Rn, Rd);

    if (N != 0)
    {
        UNALLOCATED_INSTR;
        return;
    }

    if (!DecodeBitMasks(&imm, N, imms, immr))
    {
        UNALLOCATED_INSTR;
        return;
    }
    imm &= BIT32_MAX_VALUE;

    READ_W_REG(Rn, operand1);
    result = operand1 | imm;

    WRITE_W_REG(Rd, result);
    WRITE_X_HIGH_REG(Rd, 0);
    DEBUG_INSN;
}

static void eor32_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword N, immr, imms, Rn, Rd;
    ARMdword imm;
    ARMword operand1, result;

    ARM_OPC_GET_AND_IMM_OPC(N, immr, imms, Rn, Rd);

    if (N != 0)
    {
        UNALLOCATED_INSTR;
        return;
    }

    if (!DecodeBitMasks(&imm, N, imms, immr))
    {
        UNALLOCATED_INSTR;
        return;
    }
    imm &= BIT32_MAX_VALUE;

    READ_W_REG(Rn, operand1);
    result = operand1 ^ imm;

    WRITE_W_REG(Rd, result);
    WRITE_X_HIGH_REG(Rd, 0);
    DEBUG_INSN;
}

static void ands32_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword N, immr, imms, Rn, Rd;
    ARMdword imm;
    ARMword operand1, result;

    ARM_OPC_GET_AND_IMM_OPC(N, immr, imms, Rn, Rd);

    if (N != 0)
    {
        UNALLOCATED_INSTR;
        return;
    }

    if (!DecodeBitMasks(&imm, N, imms, immr))
    {
        UNALLOCATED_INSTR;
        return;
    }
    imm &= BIT32_MAX_VALUE;

    READ_W_REG(Rn, operand1);
    result = operand1 & imm;

    WRITE_W_REG(Rd, result);

    /*Set NZCV as for a logical operation.NZ as per result, CV cleared. */
    if (result == 0)
        setPStateZF;
    else
        clearPStateZF;

    if (result >> 31 & 0x1)
        setPStateNF;
    else
        clearPStateNF;

    clearPStateCF;
    clearPStateVF;
    DEBUG_INSN;
}

static void and64_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword N, immr, imms, Rn, Rd;
    ARMdword imm, operand1, result;

    ARM_OPC_GET_AND_IMM_OPC(N, immr, imms, Rn, Rd);
    if (!DecodeBitMasks(&imm, N, imms, immr))
    {
        UNALLOCATED_INSTR;
        return;
    }
    operand1 = READ_X_REG(Rn);
    result = operand1 & imm;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void orr64_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword N, immr, imms, Rn, Rd;
    ARMdword imm, operand1, result;

    ARM_OPC_GET_AND_IMM_OPC(N, immr, imms, Rn, Rd);
    if (!DecodeBitMasks(&imm, N, imms, immr))
    {
        UNALLOCATED_INSTR;
        return;
    }
    operand1 = READ_X_REG(Rn);
    result = operand1 | imm;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void eor64_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword N, immr, imms, Rn, Rd;
    ARMdword imm, operand1, result;

    ARM_OPC_GET_AND_IMM_OPC(N, immr, imms, Rn, Rd);
    if (!DecodeBitMasks(&imm, N, imms, immr))
    {
        UNALLOCATED_INSTR;
        return;
    }
    operand1 = READ_X_REG(Rn);
    result = operand1 ^ imm;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void ands64_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword N, immr, imms, Rn, Rd;
    ARMdword imm, operand1, result;

    ARM_OPC_GET_AND_IMM_OPC(N, immr, imms, Rn, Rd);
    if (!DecodeBitMasks(&imm, N, imms, immr))
    {
        UNALLOCATED_INSTR;
        return;
    }
    operand1 = READ_X_REG(Rn);
    result = operand1 & imm;

    WRITE_X_REG(Rd, result);
    /*Set NZCV as for a logical operation.NZ as per result, CV cleared. */
    if (result == 0)
        setPStateZF;
    else
        clearPStateZF;

    if (result >> 63 & 0x1)
        setPStateNF;
    else
        clearPStateNF;

    clearPStateCF;
    clearPStateVF;
    DEBUG_INSN;
}

static void bfm(ARMul_State * state)
{
    ARMword instr = state->instr;
    int sf, N, opc, immr, imms, Rn, Rd, bitsize, pos, len;
    ARMdword rd_val = 0, rn_val = 0;

    ARM_OPC_GET_BFM_OPC(sf, opc, N, immr, imms, Rn, Rd);
    bitsize = sf ? 64 : 32;

    if (sf != N || immr >= bitsize || imms >= bitsize || opc > 2)
    {
        UNALLOCATED_INSTR;
        return;
    }

    if (!sf)
    {
        rn_val = (ARMword) READ_X_REG(Rn);
    } else
        rn_val = READ_X_REG(Rn);

    /* Recognize simple(r) extractions.  */
    if (imms >= immr)
    {
        /* Wd<s-r:0> = Wn<s:r> */
        len = (imms - immr) + 1;
        if (opc == 0)
        {                               /* SBFM: ASR, SBFX, SXTB, SXTH, SXTW */
            rd_val = gen_sextract64(rn_val, immr, len);
            goto done;
        } else if (opc == 2)
        {                               /* UBFM: UBFX, LSR, UXTB, UXTH */
            rd_val = gen_extract64(rn_val, immr, len);
            WRITE_X_REG(Rd, rd_val);
            return;
        }
        /* opc == 1, BXFIL fall through to deposit */
        rn_val = gen_extract64(rn_val, immr, len);
        pos = 0;
    } else
    {
        /* Handle the ri > si case with a deposit
         * Wd<32+s-r,32-r> = Wn<s:0>
         */
        len = imms + 1;
        pos = (bitsize - immr) & (bitsize - 1);
    }

    if (opc == 0 && len < immr)
    {
        /* SBFM: sign extend the destination field from len to fill
         * the balance of the word.  Let the deposit below insert all
         * of those sign bits.  */
        rn_val = gen_sextract64(rn_val, 0, len);
        len = immr;
    }

    if (opc == 1)
    {                                   /* BFM, BXFIL */
        rd_val = gen_deposit64(rd_val, rn_val, pos, len);
    } else
    {
        /* SBFM or UBFM: We start with zero, and we haven't modified
         * any bits outside bitsize, therefore the zero-extension
         * below is unneeded.  */
        rd_val = gen_deposit_z64(rn_val, pos, len);
    }

  done:
    if (!sf)
        rd_val &= 0xffffffff;

    WRITE_X_REG(Rd, rd_val);
}

static void extr32(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void extr64(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void svc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void hvc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void smc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ibrk(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void hlt(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void dcps1(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void dcps2(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void dcps3(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void nop(ARMul_State * state)
{
}

static void yield(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void wfe(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void wfi(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void sev(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void sevl(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void xpac_instr_proc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void pacia1716_instr_proc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void pacib1716_instr_proc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void autia1716_instr_proc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void autib1716_instr_proc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void esb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void psb_csync(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void tsb_csync(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void csdb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void paciaz_instr_proc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void paciasp_instr_proc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void pacibz_instr_proc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void pacibsp_instr_proc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void autiaz_instr_proc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void autiasp_instr_proc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void autibz_instr_proc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void autibsp_instr_proc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void bti(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void cfinv(ARMul_State * state)
{
    state->PSTATE.C = ~state->PSTATE.C;
}

static void xaflag(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void axflag(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void msr_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword op1, CRm, op2, field;

    op1 = BITS(16, 18);
    op2 = BITS(5, 7);
    CRm = BITS(8, 13);

    if (op1 == 0)
    {
        switch (op2)
        {
            case 3:
                field = PSTATEField_UAO;
                state->PSTATE.UAO = CRm & 1;
                break;
            case 4:
                field = PSTATEField_PAN;
                state->PSTATE.PAN = CRm & 1;
                break;
            case 5:
                field = PSTATEField_SP;
                state->PSTATE.SP = CRm & 1;
                break;
            default:
                NOT_IMPLEMENT;
                break;
        }
    } else if (op1 == 3)
    {
        switch (op2)
        {
            case 1:
                field = PSTATEField_SSBS;
                state->PSTATE.SSBS = CRm & 1;
                break;
            case 2:
                field = PSTATEField_DIT;
                state->PSTATE.DIT = CRm & 1;
                break;
            case 4:
                field = PSTATEField_TCO;
                state->PSTATE.TCO = CRm & 1;
                break;
            case 6:
                field = PSTATEField_DAIFSet;
                state->PSTATE.D |= ((CRm >> 3) & 0x1);
                state->PSTATE.A |= ((CRm >> 2) & 0x1);
                state->PSTATE.I |= ((CRm >> 1) & 0x1);
                state->PSTATE.F |= ((CRm >> 0) & 0x1);
                break;
            case 7:
                field = PSTATEField_DAIFClr;
                state->PSTATE.D &= ~((CRm >> 3) & 0x1);
                state->PSTATE.A &= ~((CRm >> 2) & 0x1);
                state->PSTATE.I &= ~((CRm >> 1) & 0x1);
                state->PSTATE.F &= ~((CRm >> 0) & 0x1);
                break;
            default:
                NOT_IMPLEMENT;
                break;
        }
    } else
    {
        NOT_IMPLEMENT;
    }

    if (state->PSTATE.EL == EL0 && (field == PSTATEField_DAIFClr || field == PSTATEField_DAIFSet))
    {
        if ((state->system_reg.SCTLR_EL1.all & EL1_UMA) == 0)
            // Exception_SystemRegisterTrap
            systemAccessTrap(state, EL1, 0x18, state->A64_PC);
    }
}

static void clrex(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void dmb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void isb(ARMul_State * state)
{
    /* We need to break the TB after this insn to execute
     * * a self-modified code correctly and also to take
     * * any pending interrupts immediately
     * */
    //NOT_IMPLEMENT;
    check_interrupt(state);
}

static void sb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void dsb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ssbb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void pssbb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void br(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rn;
    ARMdword target;

    Rn = BITS(5, 9);

    target = READ_X_REG(Rn);

    state->A64_PC = target;
    DEBUG_INSN;
}

static void blr(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rn;
    ARMdword target;

    Rn = BITS(5, 9);

    target = READ_X_REG(Rn);
    WRITE_X_REG(30, state->A64_PC + 4); //X[30] = pc + 4;

    state->A64_PC = target;
    DEBUG_INSN;
}

static void ret(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rn;
    ARMdword target;

    Rn = BITS(5, 9);

    target = READ_X_REG(Rn);

    state->A64_PC = target;
    DEBUG_INSN;
}

static void eret(ARMul_State * state)
{
    ARMdword target;
    ARMword spsr;

    if (state->PSTATE.EL == EL0)
    {
        UNDEFINED_INSTR;
    }
    checkForERetTrap(state, False, True);

    target = getELR(state, state->PSTATE.EL);
    spsr = getSPSR(state);

    ExceptionReturn(state, target, spsr);
}

static void drps(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void bra_keyA(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void blra_keyA(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void retaa(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void eretaa(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void bra_keyB(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void blra_keyB(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void retab(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void eretab(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void braa_keyA(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void braa_keyB(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void blraa_keyA(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void blraa_keyB(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void cbz(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rt;
    ARMdword imm19, offset, operand1;

    sf = BITS(31, 31);
    imm19 = BITS(5, 23);
    Rt = BITS(0, 4);

    offset = SignExtend64(imm19, 19) * 4;
    operand1 = READ_X_REG(Rt);

    if (!sf)
        operand1 &= BIT32_MAX_VALUE;

    if (operand1 == 0)
        state->A64_PC += offset;
    else
        state->A64_PC += 4;

    DEBUG_INSN;
    //NOT_IMPLEMENT;
}

static void cbnz(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rt;
    ARMdword imm19, offset, operand1;

    sf = BITS(31, 31);
    imm19 = BITS(5, 23);
    Rt = BITS(0, 4);

    offset = SignExtend64(imm19, 19) * 4;
    operand1 = READ_X_REG(Rt);

    if (!sf)
        operand1 &= BIT32_MAX_VALUE;

    if (operand1 != 0)
        state->A64_PC += offset;
    else
        state->A64_PC += 4;
    DEBUG_INSN;
    //NOT_IMPLEMENT;
}

static void tbz(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword b5, b40, Rt, bit_pos, imm14;
    ARMdword offset, operand1;

    ARM_OPC_GET_TBZ_OPC(b5, b40, imm14, Rt);
    bit_pos = (b5 << 5) | b40;
    offset = sextract64(instr, 5, 14) * 4;

    operand1 = READ_X_REG(Rt);

    if (!b5)
        operand1 &= BIT32_MAX_VALUE;

    if ((operand1 & (1 << bit_pos)) == 0)
        state->A64_PC += offset;
    else
        state->A64_PC += 4;
    DEBUG_INSN;
}

static void tbnz(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword b5, b40, Rt, bit_pos, imm14;
    ARMdword offset, operand1;

    ARM_OPC_GET_TBZ_OPC(b5, b40, imm14, Rt);
    bit_pos = (b5 << 5) | b40;
    offset = sextract64(instr, 5, 14) * 4;

    operand1 = READ_X_REG(Rt);

    if (!b5)
        operand1 &= BIT32_MAX_VALUE;

    if (operand1 & (1 << bit_pos))
    {
        state->A64_PC += offset;
    } else
        state->A64_PC += 4;

    DEBUG_INSN;
}

/* AdvSIMD load/store multiple structures
 *
 *  31  30  29           23 22  21         16 15    12 11  10 9    5 4    0
 * +---+---+---------------+---+-------------+--------+------+------+------+
 * | 0 | Q | 0 0 1 1 0 0 0 | L | 0 0 0 0 0 0 | opcode | size |  Rn  |  Rt  |
 * +---+---+---------------+---+-------------+--------+------+------+------+
 *
 * AdvSIMD load/store multiple structures (post-indexed)
 *
 *  31  30  29           23 22  21  20     16 15    12 11  10 9    5 4    0
 * +---+---+---------------+---+---+---------+--------+------+------+------+
 * | 0 | Q | 0 0 1 1 0 0 1 | L | 0 |   Rm    | opcode | size |  Rn  |  Rt  |
 * +---+---+---------------+---+---+---------+--------+------+------+------+
 *
 * Rt: first (or only) SIMD&FP register to be transferred
 * Rn: base address or SP
 * Rm (post-index only): post-index register (when !31) or size dependent #imm
 */
static void ldst_multiple_struct(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMdword addr, rn_val;
    int rt = BITS(0, 4);
    int rn = BITS(5, 9);
    int size = BITS(10, 11);
    int opcode = BITS(12, 15);
    bool_t is_store = !BITS(22, 22);
    bool_t is_postidx = BITS(23, 23);
    bool_t is_q = BITS(30, 30);

    int ebytes = 1 << size;
    int elements = (is_q ? 128 : 64) / (8 << size);
    int rpt;                            /* num iterations */
    int selem;                          /* structure elements */
    int r;

    if (BITS(31, 31) || BITS(21, 21))
    {
        UNALLOCATED_INSTR;
        return;
    }

    /* From the shared decode logic */
    switch (opcode)
    {
        case 0x0:
            rpt = 1;
            selem = 4;
            break;
        case 0x2:
            rpt = 4;
            selem = 1;
            break;
        case 0x4:
            rpt = 1;
            selem = 3;
            break;
        case 0x6:
            rpt = 3;
            selem = 1;
            break;
        case 0x7:
            rpt = 1;
            selem = 1;
            break;
        case 0x8:
            rpt = 1;
            selem = 2;
            break;
        case 0xa:
            rpt = 2;
            selem = 1;
            break;
        default:
            UNALLOCATED_INSTR;
            return;
    }

    if (size == 3 && !is_q && selem != 1)
    {
        /* reserved */
        UNALLOCATED_INSTR;
        return;
    }

    if (rn == 31)
    {
        check_sp_alignment(state);
    }

    rn_val = READ_X_REG(rn);
    addr = rn_val;

    for (r = 0; r < rpt; r++)
    {
        int e;

        for (e = 0; e < elements; e++)
        {
            int tt = (rt + r) % 32;
            int xs;

            for (xs = 0; xs < selem; xs++)
            {
                if (is_store)
                {
                    do_vec_st(state, tt, e, addr, size);
                } else
                {
                    do_vec_ld(state, tt, e, addr, size);
                    /* For non-quad operations, setting a slice of the low
                     * 64 bits of the register clears the high 64 bits
                     */
                    if (e == 0 && (r == 0 || xs == selem - 1))
                    {
                        clear_vec_high(state, is_q, tt, size);
                    }
                }
                addr += ebytes;
                tt = (tt + 1) % 32;
            }
        }
    }

    if (is_postidx)
    {
        int rm = BITS(16, 20);

        if (rm == 31)
        {
            WRITE_X_REG(rn, addr);
        } else
        {
            rn_val += READ_X_REG(rm);
            WRITE_X_REG(rn, rn_val);
        }
    }
}

/* AdvSIMD load/store single structure
 *
 *  31  30  29           23 22 21 20       16 15 13 12  11  10 9    5 4    0
 * +---+---+---------------+-----+-----------+-----+---+------+------+------+
 * | 0 | Q | 0 0 1 1 0 1 0 | L R | 0 0 0 0 0 | opc | S | size |  Rn  |  Rt  |
 * +---+---+---------------+-----+-----------+-----+---+------+------+------+
 *
 * AdvSIMD load/store single structure (post-indexed)
 *
 *  31  30  29           23 22 21 20       16 15 13 12  11  10 9    5 4    0
 * +---+---+---------------+-----+-----------+-----+---+------+------+------+
 * | 0 | Q | 0 0 1 1 0 1 1 | L R |     Rm    | opc | S | size |  Rn  |  Rt  |
 * +---+---+---------------+-----+-----------+-----+---+------+------+------+
 *
 * Rt: first (or only) SIMD&FP register to be transferred
 * Rn: base address or SP
 * Rm (post-index only): post-index register (when !31) or size dependent #imm
 * index = encoded in Q:S:size dependent on size
 *
 * lane_size = encoded in R, opc
 * transfer width = encoded in opc, S, size
 */
static void ldst_single_struct(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMdword addr, rn_val;
    int rt = BITS(0, 4);
    int rn = BITS(5, 9);
    int size = BITS(10, 11);
    int S = BITS(12, 12);
    int opc = BITS(13, 15);
    int R = BITS(21, 21);
    bool_t is_load = BITS(22, 22);
    bool_t is_postidx = BITS(23, 23);
    bool_t is_q = BITS(30, 30);

    int scale = extract32(opc, 1, 2);
    int selem = (extract32(opc, 0, 1) << 1 | R) + 1;
    bool_t replicate = False;
    int index = is_q << 3 | S << 2 | size;
    int ebytes, xs;

    switch (scale)
    {
        case 3:
            if (!is_load || S)
            {
                UNALLOCATED_INSTR;
                return;
            }
            scale = size;
            replicate = True;
            break;
        case 0:
            break;
        case 1:
            if (extract32(size, 0, 1))
            {
                UNALLOCATED_INSTR;
                return;
            }
            index >>= 1;
            break;
        case 2:
            if (extract32(size, 1, 1))
            {
                UNALLOCATED_INSTR;
                return;
            }
            if (!extract32(size, 0, 1))
            {
                index >>= 2;
            } else
            {
                if (S)
                {
                    UNALLOCATED_INSTR;
                    return;
                }
                index >>= 3;
                scale = 3;
            }
            break;
        default:
            NOT_IMPLEMENT;
    }

    ebytes = 1 << scale;

    if (rn == 31)
    {
        check_sp_alignment(state);
    }

    rn_val = READ_X_REG(rn);
    addr = rn_val;

    for (xs = 0; xs < selem; xs++)
    {
        if (replicate)
        {
            /* Load and replicate to all elements */
            uint64_t mulconst, tmp_val;
            uint32_t lo32, hi32;

            //mem_read_dword(state, addr, &tmp_val);
            if (NO_FAULT == arch64_mem_read(state, addr, AccType_NORMAL, &lo32, &hi32, 8))
            {
                tmp_val = lo32 | ((ARMdword) hi32 << 32);
            }

            switch (scale)
            {
                case 0:
                    mulconst = 0x0101010101010101ULL;
                    break;
                case 1:
                    mulconst = 0x0001000100010001ULL;
                    break;
                case 2:
                    mulconst = 0x0000000100000001ULL;
                    break;
                case 3:
                    mulconst = 0;
                    break;
                default:
                    NOT_IMPLEMENT;
            }
            if (mulconst)
            {
                tmp_val *= mulconst;
            }
            state->V[rt].dword.d[0] = tmp_val;
            if (is_q)
            {
                state->V[rt].dword.d[1] = tmp_val;
            }
            clear_vec_high(state, is_q, rt, scale);
        } else
        {
            /* Load/store one element per register */
            if (is_load)
            {
                do_vec_ld(state, rt, index, addr, scale);
            } else
            {
                do_vec_st(state, rt, index, addr, scale);
            }
        }
        addr += ebytes;
        rt = (rt + 1) % 32;
    }

    if (is_postidx)
    {
        int rm = extract32(instr, 16, 5);

        if (rm == 31)
        {
            WRITE_X_REG(rn, addr);
        } else
        {
            rn_val += READ_X_REG(rm);
            WRITE_X_REG(rn, rn_val);
        }
    }
}

static void load_store_memory_tags_instr_proc(ARMul_State * state)
{
    //ARMv8.5
    ARMword instr = state->instr;
    ARMword opc, imm9, op2;

    ARM_OPC_GET_MEM_TAG_OPC(opc, imm9, op2);

    switch (opc)
    {
        case 0:
            if (op2 == 0 && imm9 == 0)
            {
                //STZGM instruction
                stzgm(state);
            } else if (op2 > 0)
            {
                //STG instruction
                stg(state);
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        case 1:
            if (op2 == 0)
            {
                //LDG instruction
                ldg(state);
            } else if (op2 > 0)
            {
                //STZG instruction
                stzg(state);
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        case 2:
            if (op2 == 0 && imm9 == 0)
            {
                //STGM instruction
                stgm(state);
            } else if (op2 > 0)
            {
                //ST2G instruction
                st2g(state);
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        case 3:
            if (op2 == 0 && imm9 == 0)
            {
                //LDGM instruction
                ldgm(state);
            } else if (op2 > 0)
            {
                //STZ2G instruction
                stz2g(state);
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

/* Load/store exclusive
 *
 *  31 30 29         24  23  22   21  20  16  15  14   10 9    5 4    0
 * +-----+-------------+----+---+----+------+----+-------+------+------+
 * | sz  | 0 0 1 0 0 0 | o2 | L | o1 |  Rs  | o0 |  Rt2  |  Rn  | Rt   |
 * +-----+-------------+----+---+----+------+----+-------+------+------+
 *
 *  sz: 00 -> 8 bit, 01 -> 16 bit, 10 -> 32 bit, 11 -> 64 bit
 *   L: 0 -> store, 1 -> load
 *  o2: 0 -> exclusive, 1 -> not
 *  o1: 0 -> single register, 1 -> register pair
 *  o0: 1 -> load-acquire/store-release, 0 -> not
 */
static void load_store_exclusive_instr_proc(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, o2_L_o1, o0, Rt2, opcode;

    ARM_OPC_GET_EXCLUSIVE_OPC(size, o2_L_o1, o0, Rt2);
    opcode = o2_L_o1 << 1 | o0;

    switch (opcode)
    {
        case 0:
            if (size == 0)
            {
                //STXRB instruction
                stxrb(state);
            } else if (size == 1)
            {
                //STXRH instruction
                stxrh(state);
            } else if (size == 2 || size == 3)
            {
                //STXR instruction
                stxr(state);
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        case 1:
            if (size == 0)
            {
                //STLXRB instruction
                stlxrb(state);
            } else if (size == 1)
            {
                //STLXRH instruction
                stlxrh(state);
            } else if (size == 2 || size == 3)
            {
                //SLTXR instruction
                stlxr(state);
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        case 2:
        case 3:
        case 6:
        case 7:
            if (size == 0 || size == 1)
            {
                if (Rt2 == 0x1F)
                {
                    //CASP 32-bit or 64-bit
                    casp(state);
                } else
                {
                    UNALLOCATED_INSTR;
                }
            } else if (size == 2 || size == 3)
            {
                if (opcode == 2)
                {
                    //STXP instruction
                    stxp(state);
                } else if (opcode == 3)
                {
                    //STLXP instruction
                    stlxp(state);

                } else if (opcode == 6)
                {
                    //LDXP instruction
                    ldxp(state);
                } else if (opcode == 7)
                {
                    //LDAXP instruction
                    ldaxp(state);
                } else
                {
                    UNALLOCATED_INSTR;
                }
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        case 4:
            if (size == 0)
            {
                //LDXRB instruction
                ldxrb(state);
            } else if (size == 1)
            {
                //LDXRH instruction
                ldxrh(state);
            } else if (size == 2 || size == 3)
            {
                //LDXR instruction
                ldxr(state);
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        case 5:
            if (size == 0)
            {
                //LDAXRB instruction
                ldaxrb(state);
            } else if (size == 1)
            {
                //LDAXRH instruction
                ldaxrh(state);
            } else if (size == 2 || size == 3)
            {
                //LDAXR instruction
                ldaxr(state);
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        case 8:
            if (size == 0)
            {
                //STLLRB instruction
                stllrb(state);
            } else if (size == 1)
            {
                //STLLRH instruction
                stllrh(state);
            } else if (size == 2 || size == 3)
            {
                //STLLR instruction
                stllr(state);
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        case 9:
            if (size == 0)
            {
                //STLRB instruction
                stlrb(state);
            } else if (size == 1)
            {
                //STLRH instruction
                stlrh(state);
            } else if (size == 2 || size == 3)
            {
                //STLR instruction
                stlr(state);
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        case 10:
        case 11:
        case 14:
        case 15:
            if (size == 0)
            {
                if (Rt2 == 0x1F)
                {
                    //CASB CASAB, CASALB, CASLB instruction
                    casb(state);
                } else
                {
                    UNALLOCATED_INSTR;
                }
            } else if (size == 1)
            {
                if (Rt2 == 0x1F)
                {
                    //CASH CASAH, CASALH, CASLH instruction
                    cash(state);
                } else
                {
                    UNALLOCATED_INSTR;
                }
            } else if (size == 2 || size == 3)
            {
                if (Rt2 == 0x1F)
                {
                    //CAS CASA, CASAL, CASL instruction
                    cas(state);
                } else
                {
                    UNALLOCATED_INSTR;
                }
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        case 12:
            if (size == 0)
            {
                //LDLARB instruction
                ldlarb(state);
            } else if (size == 1)
            {
                //LDLARH instruction
                ldlarh(state);
            } else if (size == 2 || size == 3)
            {
                //LDLAR instruction
                ldlar(state);
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        case 13:
            if (size == 0)
            {
                //LDARB instruction
                ldarb(state);
            } else if (size == 1)
            {
                //LDARH instruction
                ldarh(state);
            } else if (size == 2 || size == 3)
            {
                //LDAR instruction
                ldar(state);
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

static void ldapr_stlr(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, opc, opcode;

    ARM_OPC_GET_LDAPR_STLR_OPC(size, opc, opcode);
    switch (opcode)
    {
        case 0:
            //STLURB instruction
            stlurb(state);
            break;
        case 1:
            //LDAPURB instruction
            ldapurb(state);
            break;
        case 2:
        case 3:
            //LDAPURSB instruction
            ldapursb(state);
            break;
        case 4:
            //STLURH instruction
            stlurh(state);
            break;
        case 5:
            //LDAPURH instruction
            ldapurh(state);
            break;
        case 6:
        case 7:
            //LDAPURSH instruction
            ldapursh(state);
            break;
        case 8:
        case 12:
            //STLUR instruction
            stlur(state);
            break;
        case 9:
        case 13:
            //LDAPUR instruction
            ldapur(state);
            break;
        case 10:
            //LDAPURSW instruction
            ldapursw(state);
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

static void load_register_literal(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword opc, V, opcode;

    ARM_OPC_GET_LDAD_LITTERAL_OPC(opc, V, opcode);

    switch (opcode)
    {
        case 0:
        case 2:
            //LDR (literal) 32-bit or 64-bit
            ldr_literal(state);
            break;
        case 1:
        case 3:
        case 5:
            //LDR (literal, SIMD&FP) 32-bit or 64-bit or 128-bit
            ldr_literal_simd_fp(state);
            break;
        case 4:
            //LDRSW instruction
            ldrsw_literal(state);
            break;
        case 6:
            //PRFM instruction
            prfm_literal(state);
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

static void load_store_no_allocate_pair_offset(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword opc, V, L, opcode;

    ARM_OPC_GET_PAIR_POST_INDEX_OPC(opc, V, L, opcode);
    switch (opcode)
    {
        case 0:
        case 8:
            //STNP instruction
            stnp(state);
            break;
        case 1:
        case 9:
            //LDNP instruction
            ldnp(state);
            break;
        case 2:
        case 6:
        case 10:
            //STNP (SIMD&FP) 32-bit or 64-bit or 128-bit
            stnp_simd_fp(state);
            break;
        case 3:
        case 7:
        case 11:
            //LDNP (SIMD&FP) instruction
            ldnp_simd_fp(state);
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

static void load_store_register_pair(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword opc, V, L, opcode;

    ARM_OPC_GET_PAIR_POST_INDEX_OPC(opc, V, L, opcode);
    switch (opcode)
    {
        case 0:
        case 8:
            //STP instruction
            stp(state);
            break;
        case 1:
        case 9:
            //LDP instruction
            ldp(state);
            break;
        case 2:
        case 6:
        case 10:
            //STP (SIMD&FP) 32-bit or 64-bit or 128-bit
            stp_simd_fp(state);
            break;
        case 3:
        case 7:
        case 11:
            //LDNP (SIMD&FP) instruction
            ldp_simd_fp(state);
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

static void load_store_register_unscaled_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, V, opc, opcode;

    ARM_OPC_GET_UNSCALED_IMM_OPC(size, V, opc, opcode);
    switch (opcode)
    {
        case 0x0:
        case 0x8:
        case 0x10:
        case 0x18:
            //STURB STURH, STUR instruction
            stur(state);
            break;
        case 0x1:
        case 0x9:
        case 0x11:
        case 0x19:
            //LDURB LDURH LDUR instruction
            ldur(state);
            break;
        case 0x2:
        case 0x3:
        case 0xA:
        case 0xB:
        case 0x12:
            //LDURSB LDURSH LDURSW instruction
            ldurs(state);
            break;
        case 0x4:
        case 0x6:
        case 0xC:
        case 0x14:
        case 0x1C:
            //STUR(SIMD&FP) instruction
            stur_simd_fp(state);
            break;
        case 0x5:
        case 0x7:
        case 0xD:
        case 0x15:
        case 0x1D:
            //LDUR(SIMD&FP) instruction
            ldur_simd_fp(state);
            break;
        case 0x1A:
            //PRFUM instruction
            prfum(state);
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

/*
 * Load/store (immediate post-indexed)
 * Load/store (immediate pre-indexed)
 * Load/store (unscaled immediate)
 *
 * 31 30 29   27  26 25 24 23 22 21  20    12 11 10 9    5 4    0
 * +----+-------+---+-----+-----+---+--------+-----+------+------+
 * |size| 1 1 1 | V | 0 0 | opc | 0 |  imm9  | idx |  Rn  |  Rt  |
 * +----+-------+---+-----+-----+---+--------+-----+------+------+
 *
 * idx = 01 -> post-indexed, 11 pre-indexed, 00 unscaled imm. (no writeback)
 10 -> unprivileged
 * V = 0 -> non-vector
 * size: 00 -> 8 bit, 01 -> 16 bit, 10 -> 32 bit, 11 -> 64bit
 * opc: 00 -> store, 01 -> loadu, 10 -> loads 64, 11 -> loads 32
 */
static void load_store_register_imm9(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, V, opc, opcode;

    ARM_OPC_GET_UNSCALED_IMM_OPC(size, V, opc, opcode);
    switch (opcode)
    {
        case 0x0:
        case 0x8:
        case 0x10:
        case 0x18:
            //STRB STRH STR instruction
            str_imm(state);
            break;
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x9:
        case 0xA:
        case 0xB:
        case 0x11:
        case 0x12:
        case 0x19:
            //LDRB LDRH LDR LDRSB LDRSH LDRSW instruction
            ldr_imm(state);
            break;
        case 0x4:
        case 0x6:
        case 0xC:
        case 0x14:
        case 0x1C:
            //STR(SIMD&FP) instruction
            str_imm_simd_fp(state);
            break;
        case 0x5:
        case 0x7:
        case 0xD:
        case 0x15:
        case 0x1D:
            //LDR(SIMD&FP) instruction
            ldr_imm_simd_fp(state);
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

static void load_store_register_unpriv(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, V, opc, opcode;

    ARM_OPC_GET_UNSCALED_IMM_OPC(size, V, opc, opcode);
    switch (opcode)
    {
        case 0:
            //STTRB instruction
            sttrb(state);
            break;
        case 1:
            //LDTRB instruction
            ldtrb(state);
            break;
        case 2:
        case 3:
            //LDTRSB instruction
            ldtrsb(state);
            break;
        case 8:
            //STRH instruction
            sttrh(state);
            break;
        case 9:
            //LDTRRH instruction
            ldtrh(state);
            break;
        case 10:
        case 11:
            //LDTRSH instruction
            ldtrsh(state);
            break;
        case 0x10:
        case 0x18:
            //STTR instruction
            sttr(state);
            break;
        case 0x11:
        case 0x19:
            //LDTR instruction
            ldtr(state);
            break;
        case 0x12:
            //LDTRSW instruction
            ldtrsw(state);
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

static void atomic_mem(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, V, A, R, o3, opc, opcode;

    ARM_OPC_GET_ATOMIC_MEM_OPC(size, V, A, R, o3, opc, opcode);

    switch (opcode)
    {
        case 0:
        case 2:
        case 4:
        case 6:
            ld_opc(state, opc);
            break;
        case 1:
        case 3:
        case 5:
        case 7:
            if (opcode == 5 && opc == 4)
            {
                //LDAPRB instruction
                ldapr(state);
            } else if (opc == 0)
            {
                //SWPB, SWPAB, SWPALB, SWPLB-SWPB variant instruction
                swp(state);
            } else
            {
                UNALLOCATED_INSTR;
            }
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
    return;
}

static void ld_opc(ARMul_State * state, int opc)
{
    ARMword instr = state->instr;

    switch (opc)
    {
        case 0:
            ldadd(state);
            break;
        case 1:
            ldclr(state);
            break;
        case 2:
            ldeor(state);
            break;
        case 3:
            ldset(state);
            break;
        case 4:
            ldsmax(state);
            break;
        case 5:
            ldsmin(state);
            break;
        case 6:
            ldumax(state);
            break;
        case 7:
            ldumin(state);
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

static void load_store_register_offset(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, V, opc, opcode;

    ARM_OPC_GET_UNSCALED_IMM_OPC(size, V, opc, opcode);
    switch (opcode)
    {
        case 0x0:
        case 0x8:
        case 0x10:
        case 0x18:
            //STRB STRH STR instruction
            str_register(state);
            break;
        case 0x1:
        case 0x9:
        case 0x11:
        case 0x19:
        case 0x2:
        case 0x3:
        case 0xA:
        case 0xB:
        case 0x12:
            //LDRB LDRH LDR instruction
            //LDRSB LDRSH LDRS instruction
            ldr_register(state);
            break;
        case 0x4:
        case 0x6:
        case 0xC:
        case 0x14:
        case 0x1C:
            //STR(SIMD&FP) instruction
            str_register_simd_fp(state);
            break;
        case 0x5:
        case 0x7:
        case 0xD:
        case 0x15:
        case 0x1D:
            //LDR(SIMD&FP) instruction
            ldr_register_simd_fp(state);
            break;
        case 0x1A:
            //PRFM instruction
            prfm_register(state);
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

static void load_store_register_pac(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, V, M, W, opcode;

    ARM_OPC_GET_PAC_OPC(size, V, M, W, opcode);
    if (size == 3)
    {
        if (opcode < 4)
        {
            //LDRAA, LDRAB-Key A, KeyB
            ldra(state);
        } else
        {
            UNALLOCATED_INSTR;
        }
    } else
    {
        UNALLOCATED_INSTR;
    }
}

static void load_store_register_uimm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, V, opc, opcode;

    ARM_OPC_GET_UNSCALED_IMM_OPC(size, V, opc, opcode);
    switch (opcode)
    {
        case 0x0:
        case 0x8:
        case 0x10:
        case 0x18:
            //STRB STRH STR instruction
            str_imm(state);
            break;
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x9:
        case 0xA:
        case 0xB:
        case 0x11:
        case 0x12:
        case 0x19:
            //LDRB LDRSB LDRH LDRSH LDR LDRSW instruction
            ldr_imm(state);
            break;
        case 0x4:
        case 0x6:
        case 0xC:
        case 0x14:
        case 0x1C:
            //STR(SIMD&FP) instruction
            str_imm_simd_fp(state);
            break;
        case 0x5:
        case 0x7:
        case 0xD:
        case 0x15:
        case 0x1D:
            //LDR(SIMD&FP) instruction
            ldr_imm_simd_fp(state);
            break;
        case 0x1A:
            //PRFM instruction
            prfm_imm(state);
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

static void stzgm(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stg(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldg(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stzg(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stgm(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void st2g(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldgm(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stz2g(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stxrb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stxrh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stxr(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stlxrb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stlxrh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stlxr(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void casp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stxp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stlxp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldxp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldaxp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldxrb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldxrh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldxr(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldaxrb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldaxrh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldaxr(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stllrb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stllrh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stllr(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stlrb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stlrh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stlr(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void casb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void cash(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void cas(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldlarb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldlarh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldlar(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldarb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldarh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldar(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stlurb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldapurb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldapursb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stlurh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldapurh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldapursh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stlur(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldapur(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldapursw(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldr_literal(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword opc, imm19, Rt;
    ARMdword offset, address;

    ARM_OPC_GET_LDR_LITERAL_OPC(opc, imm19, Rt);
    offset = SignExtend64(imm19, 19) * 4;
    address = state->A64_PC + offset;

    if (opc == 1)
    {
        //64-bit
        ARMdword result;
        ARMword lo32, hi32;

        //if (NO_FAULT == mem_read_dword(state, address, &result))
        if (NO_FAULT == arch64_mem_read(state, address, AccType_NORMAL, &lo32, &hi32, 8))
        {
            result = lo32 | ((ARMdword) hi32 << 32);
            WRITE_X_REG(Rt, result);
        }
    } else
    {
        //32-bit
        ARMword result;

        if (NO_FAULT == arch64_mem_read(state, address, AccType_NORMAL, &result, NULL, 4))
        {
            WRITE_W_REG(Rt, result);
            WRITE_X_HIGH_REG(Rt, 0);
        }
    }
    DEBUG_INSN;
}

static void ldr_literal_simd_fp(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword opc, imm19, Rt, size;
    ARMdword offset, address;
    bit128 data;

    ARM_OPC_GET_LDR_LITERAL_OPC(opc, imm19, Rt);
    offset = SignExtend64(imm19, 19) * 4;
    address = state->A64_PC + offset;
    size = 2 + opc;

    data = do_fp_ld_mem(state, address, size);

    SET_VFP_DATA(size, Rt, data);

    DEBUG_INSN;
}

static void ldrsw_literal(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void prfm_literal(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stnp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldnp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stnp_simd_fp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldnp_simd_fp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void stp(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword opc, Rt2, Rn, Rt, idx;
    ARMdword imm7, offset, data1, data2;
    int64_t addr;
    bool_t writeback, post_index, is_signed;
    int size;

    ARM_OPC_GET_STP_OPC(opc, imm7, Rt2, Rn, Rt);
    idx = BITS(23, 24);
    is_signed = opc & 1;

    if (is_signed)
    {
        UNALLOCATED_INSTR;
        return;
    }

    switch (idx)
    {
        case 0:
        case 2:
            post_index = False;
            writeback = False;
            break;
        case 1:
            post_index = True;
            writeback = True;
            break;
        case 3:
            post_index = False;
            writeback = True;
            break;
        default:
            break;
    }

    size = 2 + (opc >> 1);
    imm7 = SignExtend64(imm7, 7);
    offset = imm7 << size;

    addr = READ_X_REG(Rn);

    if (!post_index)
        addr += offset;

    if (Rt != 31)
    {
        data1 = READ_X_REG(Rt);
    } else
        data1 = 0;

    if (Rt2 != 31)
    {
        data2 = READ_X_REG(Rt2);
    } else
        data2 = 0;

    if (opc == 0)
    {
        //32-bit
        data1 &= BIT32_MAX_VALUE;
        data2 &= BIT32_MAX_VALUE;
    }
    bool_t iss_sf = ldst_compute_iss_sf(size, is_signed, opc);

    do_st_mem(state, addr, data1, size, True, Rt, iss_sf, False);
    addr += 1 << size;
    do_st_mem(state, addr, data2, size, True, Rt, iss_sf, False);

    if (writeback)
    {
        if (post_index)
            addr += offset - (1 << size);
        else
            addr -= (1 << size);

        WRITE_X_REG(Rn, addr);
    }
    DEBUG_INSN;
}

static void ldp(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword opc, Rt2, Rn, Rt, idx;
    ARMdword imm7, offset, addr;
    ARMdword data1 = 0, data2 = 0;
    bool_t writeback, post_index, is_signed;
    int size;

    ARM_OPC_GET_STP_OPC(opc, imm7, Rt2, Rn, Rt);
    idx = BITS(23, 24);
    is_signed = opc & 1;

    if (is_signed)
    {
        UNALLOCATED_INSTR;
        return;
    }

    switch (idx)
    {
        case 0:
        case 2:
            post_index = False;
            writeback = False;
            break;
        case 1:
            post_index = True;
            writeback = True;
            break;
        case 3:
            post_index = False;
            writeback = True;
            break;
        default:
            break;
    }

    size = 2 + (opc >> 1);
    //offset = SignExtend64(imm7, 7) << size;
    offset = imm7 << size;

    addr = READ_X_REG(Rn);

    if (!post_index)
        addr += offset;

    bool_t iss_sf = ldst_compute_iss_sf(size, is_signed, opc);

    data1 = do_ld_mem(state, addr, size, is_signed, False, True, Rt, iss_sf, False);
    WRITE_X_REG(Rt, data1);
    addr += 1 << size;
    data2 = do_ld_mem(state, addr, size, is_signed, False, True, Rt, iss_sf, False);
    WRITE_X_REG(Rt2, data2);

    if (writeback)
    {
        if (post_index)
            addr += offset - (1 << size);
        else
            addr -= (1 << size);

        WRITE_X_REG(Rn, addr);
    }
    DEBUG_INSN;
}

static void stp_simd_fp(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword opc, Rt2, Rn, Rt, idx, size;
    ARMdword imm7, offset;
    int64_t addr;
    bool_t writeback, post_index;

    ARM_OPC_GET_STP_OPC(opc, imm7, Rt2, Rn, Rt);
    idx = BITS(23, 24);

    switch (idx)
    {
        case 0:
        case 2:
            post_index = False;
            writeback = False;
            break;
        case 1:
            post_index = True;
            writeback = True;
            break;
        case 3:
            post_index = False;
            writeback = True;
            break;
        default:
            break;
    }

    size = 2 + opc;
    imm7 = SignExtend64(imm7, 7);
    offset = imm7 << size;

    addr = READ_X_REG(Rn);

    if (!post_index)
        addr += offset;

    do_fp_st_mem(state, addr, Rt, size);
    addr += 1 << size;
    do_fp_st_mem(state, addr, Rt2, size);

    if (writeback)
    {
        if (post_index)
            addr += offset - (1 << size);
        else
            addr -= (1 << size);

        WRITE_X_REG(Rn, addr);
    }
    DEBUG_INSN;
}

static void ldp_simd_fp(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword opc, Rt2, Rn, Rt, idx, size;
    ARMdword imm7, offset;
    bit128 src_rt, src_rt2;
    int64_t addr;
    bool_t writeback, post_index;

    ARM_OPC_GET_STP_OPC(opc, imm7, Rt2, Rn, Rt);
    idx = BITS(23, 24);

    switch (idx)
    {
        case 0:
        case 2:
            post_index = False;
            writeback = False;
            break;
        case 1:
            post_index = True;
            writeback = True;
            break;
        case 3:
            post_index = False;
            writeback = True;
            break;
        default:
            break;
    }

    size = 2 + opc;
    imm7 = SignExtend64(imm7, 7);
    offset = imm7 << size;

    addr = READ_X_REG(Rn);

    if (!post_index)
        addr += offset;

    src_rt = do_fp_ld_mem(state, addr, size);
    addr += 1 << size;
    src_rt2 = do_fp_ld_mem(state, addr, size);

    SET_VFP_DATA(size, Rt, src_rt);
    SET_VFP_DATA(size, Rt2, src_rt2);
    if (writeback)
    {
        if (post_index)
            addr += offset - (1 << size);
        else
            addr -= (1 << size);

        WRITE_X_REG(Rn, addr);
    }
    DEBUG_INSN;
}

static void stur(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, opc, imm9, Rn, Rt;
    ARMdword address, offset, data;

    ARM_OPC_GET_LD_ST_IMM_OPC(size, opc, imm9, Rn, Rt);
    offset = SignExtend64(imm9, 9);

    address = READ_X_REG(Rn);

    address += offset;
    data = READ_X_REG(Rt);
    mem_st_i64(state, address, data, size);

    DEBUG_INSN;
}

static void ldur(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, opc, imm9, Rn, Rt;
    ARMdword address, offset, data;

    ARM_OPC_GET_LD_ST_IMM_OPC(size, opc, imm9, Rn, Rt);
    offset = SignExtend64(imm9, 9);

    address = READ_X_REG(Rn);

    address += offset;
    mem_ld_i64(state, address, &data, size);

    WRITE_X_REG(Rt, data);
    DEBUG_INSN;
}

static void ldurs(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, opc, imm9, Rn, Rt;
    ARMdword address, offset, data;
    bool_t is_signed;

    ARM_OPC_GET_LD_ST_IMM_OPC(size, opc, imm9, Rn, Rt);
    offset = SignExtend64(imm9, 9);
    is_signed = (opc >> 1) & 0x1;
    address = READ_X_REG(Rn);

    address += offset;
    bool_t iss_sf = ldst_compute_iss_sf(size, is_signed, opc);

    data = do_ld_mem(state, address, size, is_signed, False, True, Rt, iss_sf, False);

    WRITE_X_REG(Rt, data);
    DEBUG_INSN;
}

static void stur_simd_fp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldur_simd_fp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void prfum(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void str_imm_simd_fp(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMdword offset, address, data;

    ARMword imm12 = BITS(10, 21);
    ARMword imm9 = BITS(12, 20);
    ARMword rt = BITS(0, 4);
    ARMword rn = BITS(5, 9);
    ARMword opc = BITS(22, 23);
    ARMword size = BITS(30, 31);
    bool_t is_vector = BITS(26, 26);
    bool_t isImm12 = BITS(24, 24);

    bool_t is_signed = False;
    bool_t is_extended = False;

    if (is_vector)
    {
        size |= (opc & 2) << 1;
        if (size > 4)
        {
            UNALLOCATED_INSTR;
            return;
        }
    } else
    {
        if (size == 3 && opc == 2)
        {
            /* PRFM - prefetch */
            return;
        }
        if (opc == 3 && size > 1)
        {
            UNALLOCATED_INSTR;
            return;
        }
        is_signed = opc >> 1;
        is_extended = (size < 3) && (opc & 1);
    }

    if (rn == 31)
    {
        UNALLOCATED_INSTR;
    }

    address = READ_X_REG(rn);

    if (isImm12)
        offset = imm12 << size;
    else
        offset = imm9;

    address += offset;

    if (is_vector)
    {
        do_fp_st_mem(state, address, rt, size);
    } else
    {
        data = READ_X_REG(rt);
        bool_t iss_sf = ldst_compute_iss_sf(size, is_signed, opc);

        do_st_mem(state, address, data, size, True, rt, iss_sf, False);
    }
}

static void ldr_imm_simd_fp(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMdword offset, address;

    ARMword imm12 = BITS(10, 21);
    ARMword imm9 = BITS(12, 20);
    ARMword rt = BITS(0, 4);
    ARMword rn = BITS(5, 9);
    ARMword opc = BITS(22, 23);
    ARMword size = BITS(30, 31);
    bool_t is_vector = BITS(26, 26);
    bool_t isImm12 = BITS(24, 24);

    bool_t is_signed = False;
    bool_t is_extended = False;
    bit128 data;

    memset(&data, 0, sizeof (bit128));

    if (is_vector)
    {
        size |= (opc & 2) << 1;
        if (size > 4)
        {
            UNALLOCATED_INSTR;
            return;
        }
    } else
    {
        if (size == 3 && opc == 2)
        {
            /* PRFM - prefetch */
            return;
        }
        if (opc == 3 && size > 1)
        {
            UNALLOCATED_INSTR;
            return;
        }
        is_signed = opc >> 1;
        is_extended = (size < 3) && (opc & 1);
    }

    if (rn == 31)
    {
        UNALLOCATED_INSTR;
    }

    address = READ_X_REG(rn);

    if (isImm12)
        offset = imm12 << size;
    else
        offset = imm9;

    address += offset;

    if (is_vector)
    {
        data = do_fp_ld_mem(state, address, size);
    } else
    {
        bool_t iss_sf = ldst_compute_iss_sf(size, is_signed, opc);

        data.lo64 = do_ld_mem(state, address, size, is_signed, False, True, rt, iss_sf, False);
    }
    SET_VFP_DATA(size, rt, data);
    DEBUG_INSN;
}

static void ldrh_imm(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void str_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, idx, opc, imm9, imm12, Rn, Rt, is_signed;
    ARMdword offset, address, data;
    bool_t post_index = False;
    bool_t writeback = False;
    bool_t isImm12 = False;

    ARM_OPC_GET_LD_ST_IMM_OPC(size, opc, imm9, Rn, Rt);
    is_signed = (opc >> 1) & 0x1;
    idx = BITS(10, 11);
    imm12 = BITS(10, 21);
    isImm12 = BITS(24, 24);

    switch (idx)
    {
        case 0:
        case 2:
            post_index = False;
            writeback = False;
            break;
        case 1:
            post_index = True;
            writeback = True;
            break;
        case 3:
            post_index = False;
            writeback = True;
            break;
        default:
            break;
    }

    if (isImm12)
    {
        offset = imm12 << size;
        post_index = False;
        writeback = False;
    } else
        offset = imm9;

    address = READ_X_REG(Rn);

    if (!post_index)
        address += offset;

    if (Rt != 31)
    {
        data = READ_X_REG(Rt);
    } else
        data = 0;

    bool_t iss_sf = ldst_compute_iss_sf(size, is_signed, opc);

    do_st_mem(state, address, data, size, True, Rt, iss_sf, False);

    if (writeback)
    {
        if (post_index)
            address += offset;

        WRITE_X_REG(Rn, address);
    }
    DEBUG_INSN;
}

static void ldr_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword idx, isImm12;
    ARMword size, opc, imm9, imm12, Rn, Rt;
    ARMdword offset, address, data;
    bool_t is_signed, is_extended;
    bool_t post_index = False;
    bool_t writeback = False;

    ARM_OPC_GET_LD_ST_IMM_OPC(size, opc, imm9, Rn, Rt);
    is_signed = (opc >> 1) & 0x1;
    is_extended = (size < 3) && (opc & 0x1);
    idx = BITS(10, 11);
    imm12 = BITS(10, 21);
    isImm12 = BITS(24, 24);

    switch (idx)
    {
        case 0:
        case 2:
            post_index = False;
            writeback = False;
            break;
        case 1:
            post_index = True;
            writeback = True;
            break;
        case 3:
            post_index = False;
            writeback = True;
            break;
        default:
            break;
    }

    if (isImm12)
    {
        offset = imm12 << size;
        post_index = False;
        writeback = False;
    } else
        offset = imm9;

    address = READ_X_REG(Rn);

    if (!post_index)
        address += offset;

    bool_t iss_sf = ldst_compute_iss_sf(size, is_signed, opc);

    data = do_ld_mem(state, address, size, is_signed, is_extended, True, Rt, iss_sf, False);
    WRITE_X_REG(Rt, data);

    if (writeback)
    {
        if (post_index)
            address += offset;

        WRITE_X_REG(Rn, address);
    }
    DEBUG_INSN;
}

static void sttrb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldtrb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldtrsb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void sttrh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldtrh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldtrsh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void sttr(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldtr(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldtrsw(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldapr(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void swp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldadd(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldclr(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldeor(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldset(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldsmax(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldsmin(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldumax(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldumin(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void str_register_simd_fp(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, Rm, option, S, Rn, Rt;
    ARMdword result, operand1, src, offset, address, data;

    bool_t is_signed = False;
    bool_t is_extend = True;
    bool_t is_vector = BITS(26, 26);

    ARM_OPC_GET_STR_OPC(size, Rm, option, S, Rn, Rt);

    if ((option >> 1 & 1) == 0)
    {
        UNDEFINED_INSTR;
    }

    src = READ_X_REG(Rm);
    if ((option & 1) == 0)
        src &= BIT32_MAX_VALUE;
    ext_and_shift_reg(&offset, &src, option, S ? size : 0);     //offset = Extend(X[m])

    address = READ_X_REG(Rn);           //address = X[n] or SP
    address += offset;

    if (is_vector)
    {
        do_fp_st_mem(state, address, Rt, size);
    } else
    {
        data = READ_X_REG(Rt);
        bool_t iss_sf = ldst_compute_iss_sf(size, is_signed, option);

        do_st_mem(state, address, data, size, True, Rt, iss_sf, False);
    }
    DEBUG_INSN;
}

static void ldr_register_simd_fp(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, Rm, option, S, Rn, Rt;
    ARMdword result, operand1, src, offset, address;
    bit128 data;

    bool_t is_signed = False;
    bool_t is_extend = True;
    bool_t is_vector = BITS(26, 26);

    memset(&data, 0, sizeof (bit128));

    ARM_OPC_GET_STR_OPC(size, Rm, option, S, Rn, Rt);

    if ((option >> 1 & 1) == 0)
    {
        UNDEFINED_INSTR;
    }

    src = READ_X_REG(Rm);
    if ((option & 1) == 0)
        src &= BIT32_MAX_VALUE;
    ext_and_shift_reg(&offset, &src, option, S ? size : 0);     //offset = Extend(X[m])

    address = READ_X_REG(Rn);           //address = X[n] or SP
    address += offset;

    if (is_vector)
    {
        data = do_fp_ld_mem(state, address, size);
    } else
    {
        bool_t iss_sf = ldst_compute_iss_sf(size, is_signed, option);

        data.lo64 = do_ld_mem(state, address, size, is_signed, False, True, Rt, iss_sf, False);
    }
    SET_VFP_DATA(size, Rt, data);
    DEBUG_INSN;
}

static void str_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, opc, Rm, option, S, Rn, Rt;
    ARMdword result, operand1, src, offset, address;
    ARMdword data;
    bool_t is_signed = False;

    ARM_OPC_GET_STR_OPC(size, Rm, option, S, Rn, Rt);

    src = READ_X_REG(Rm);
    if ((option & 1) == 0)
        src &= BIT32_MAX_VALUE;
    ext_and_shift_reg(&offset, &src, option, S ? size : 0);     //offset = Extend(X[m])

    address = READ_X_REG(Rn);           //address = X[n] or SP
    address += offset;

    if (Rt != 31)
    {
        data = READ_X_REG(Rt);          //data = X[t]
    } else
        data = 0;                       //data = 0

    bool_t iss_sf = ldst_compute_iss_sf(size, is_signed, option);

    do_st_mem(state, address, data, size, True, Rt, iss_sf, False);
    DEBUG_INSN;
}

static void ldr_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword size, Rm, option, S, Rn, Rt;
    ARMdword result, operand1, src, offset, address;
    ARMdword data;
    bool_t is_signed = False;
    bool_t is_extend = True;

    ARM_OPC_GET_STR_OPC(size, Rm, option, S, Rn, Rt);

    if ((option >> 1 & 1) == 0)
    {
        UNDEFINED_INSTR;
    }

    src = READ_X_REG(Rm);
    if ((option & 1) == 0)
        src &= BIT32_MAX_VALUE;
    ext_and_shift_reg(&offset, &src, option, S ? size : 0);     //offset = Extend(X[m])

    address = READ_X_REG(Rn);           //address = X[n] or SP
    address += offset;

    bool_t iss_sf = ldst_compute_iss_sf(size, is_signed, option);

    data = do_ld_mem(state, address, size, is_signed, is_extend, True, Rt, iss_sf, False);
    WRITE_X_REG(Rt, data);              //X[t] = data
    DEBUG_INSN;
}

static void prfm_register(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void ldra(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void prfm_imm(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void rbit(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rn, Rd;
    ARMdword operand;

    ARM_OPC_GET_ONE_SRC_OPC(sf, Rn, Rd);

    if (!sf)
    {
        operand = (uint32_t) READ_X_REG(Rn);
        operand = revbit32((uint32_t) operand);
    } else
    {
        operand = READ_X_REG(Rn);
        operand = revbit64(operand);
    }

    WRITE_X_REG(Rd, operand);
    DEBUG_INSN;
}

static void rev16(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rn, Rd;
    ARMdword operand, mask, tmp_val, result;

    ARM_OPC_GET_ONE_SRC_OPC(sf, Rn, Rd);
    if (!sf)
    {
        operand = (Rn == 31) ? 0 : (uint32_t) READ_X_REG(Rn);
        mask = 0x00ff00ff;
    } else
    {
        operand = (Rn == 31) ? 0 : READ_X_REG(Rn);
        mask = 0x00ff00ff00ff00ffull;
    }

    tmp_val = operand >> 8;
    result = operand & mask;
    tmp_val &= mask;
    result <<= 8;

    result |= tmp_val;
    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void rev(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rn, Rd;
    ARMdword operand, mask, tmp_val, result;

    ARM_OPC_GET_ONE_SRC_OPC(sf, Rn, Rd);
    if (!sf)
    {
        operand = (Rn == 31) ? 0 : (uint32_t) READ_X_REG(Rn);
        result = bswap_word((uint32_t) operand);
    } else
    {
        operand = (Rn == 31) ? 0 : READ_X_REG(Rn);
        tmp_val = operand & 0xffffffff;
        result = bswap_word((uint32_t) tmp_val);
        tmp_val = operand >> 32;
        tmp_val = bswap_word((uint32_t) tmp_val);
        result |= (tmp_val << 32);
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void clz(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rn, Rd, result;
    ARMdword operand;

    ARM_OPC_GET_ONE_SRC_OPC(sf, Rn, Rd);

    if (!sf)
    {
        operand = (Rn == 31) ? 0 : (uint32_t) READ_X_REG(Rn);
        result = clz32((uint32_t) operand);
    } else
    {
        operand = (Rn == 31) ? 0 : READ_X_REG(Rn);
        result = clz64(operand);
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void cls(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rn, Rd, result;
    ARMdword operand;

    ARM_OPC_GET_ONE_SRC_OPC(sf, Rn, Rd);

    if (!sf)
    {
        operand = (Rn == 31) ? 0 : (uint32_t) READ_X_REG(Rn);
        result = cls32((uint32_t) operand);
    } else
    {
        operand = (Rn == 31) ? 0 : READ_X_REG(Rn);
        result = cls64(operand);
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void pacia(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void pacib(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void pacda(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void pacdb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void autia(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void autib(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void autda(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void autdb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void xpaci(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void xpacd(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void and_shift_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, shift_type, Rm, imm6, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_SHIFT_REG_OPC(sf, shift_type, Rm, imm6, Rn, Rd);

    if (!sf && (imm6 >> 5))
    {
        UNDEFINED_INSTR;
    }

    if (Rm != 31)
    {
        src = READ_X_REG(Rm);
    } else
        src = 0;

    shift_reg_imm(&operand2, &src, sf, shift_type, imm6);       //operand2 = shiftReg(Rm, shift_type, imm6);
    if (Rn == 31)
    {
        operand1 = 0;
    } else
        operand1 = READ_X_REG(Rn);
    result = operand1 & operand2;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void bic_shift_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, shift_type, Rm, imm6, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_SHIFT_REG_OPC(sf, shift_type, Rm, imm6, Rn, Rd);

    if (!sf && (imm6 >> 5))
    {
        UNDEFINED_INSTR;
    }

    if (Rm != 31)
    {
        src = READ_X_REG(Rm);
    } else
        src = 0;

    shift_reg_imm(&operand2, &src, sf, shift_type, imm6);       //operand2 = shiftReg(Rm, shift_type, imm6);
    operand2 = ~operand2;               //not(operand2)

    if (Rn == 31)
    {
        operand1 = 0;
    } else
        operand1 = READ_X_REG(Rn);
    result = operand1 & operand2;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void orr_shift_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, shift_type, Rm, imm6, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_SHIFT_REG_OPC(sf, shift_type, Rm, imm6, Rn, Rd);

    if (!sf && (imm6 >> 5))
    {
        UNDEFINED_INSTR;
    }

    if (shift_type == 0 && imm6 == 0 && Rn == 0x1F)
    {
        //MOV instruction
        result = READ_X_REG(Rm);        //result = X[Rm];

        if (!sf)
            result &= BIT32_MAX_VALUE;

        WRITE_X_REG(Rd, result);        //X[Rd] = result
        return;
    }

    if (Rm != 31)
    {
        src = READ_X_REG(Rm);
    } else
        src = 0;
    shift_reg_imm(&operand2, &src, sf, shift_type, imm6);       //operand2 = shiftReg(Rm, shift_type, imm6);
    if (Rn == 31)
    {
        operand1 = 0;
    } else
        operand1 = READ_X_REG(Rn);

    result = operand1 | operand2;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void orn_shift_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, shift_type, Rm, imm6, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_SHIFT_REG_OPC(sf, shift_type, Rm, imm6, Rn, Rd);

    if (!sf && (imm6 >> 5))
    {
        UNDEFINED_INSTR;
    }

    if (shift_type == 0 && imm6 == 0 && Rn == 0x1F)
    {
        //MVN instruction
        result = READ_X_REG(Rm);        //result = X[Rm];
        result = ~result;

        if (!sf)
            result &= BIT32_MAX_VALUE;

        WRITE_X_REG(Rd, result);        //X[Rd] = result
        return;
    }

    if (Rm != 31)
    {
        src = READ_X_REG(Rm);           ////src = X[Rm]
    } else
        src = 0;
    shift_reg_imm(&operand2, &src, sf, shift_type, imm6);       //operand2 = shiftReg(Rm, shift_type, imm6);

    operand2 = ~operand2;               //not(operand2)
    if (Rn == 31)
    {
        operand1 = 0;
    } else
        operand1 = READ_X_REG(Rn);

    result = operand1 | operand2;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void eor_shift_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, shift_type, Rm, imm6, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_SHIFT_REG_OPC(sf, shift_type, Rm, imm6, Rn, Rd);

    if (!sf && (imm6 >> 5))
    {
        UNDEFINED_INSTR;
    }

    if (Rm != 31)
    {
        src = READ_X_REG(Rm);
    } else
        src = 0;

    shift_reg_imm(&operand2, &src, sf, shift_type, imm6);       //operand2 = shiftReg(Rm, shift_type, imm6);
    if (Rn == 31)
    {
        operand1 = 0;
    } else
        operand1 = READ_X_REG(Rn);
    result = operand1 ^ operand2;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void eon_shift_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, shift_type, Rm, imm6, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_SHIFT_REG_OPC(sf, shift_type, Rm, imm6, Rn, Rd);

    if (!sf && (imm6 >> 5))
    {
        UNDEFINED_INSTR;
    }

    if (Rm != 31)
    {
        src = READ_X_REG(Rm);           //src = X[Rm]
    } else
        src = 0;
    shift_reg_imm(&operand2, &src, sf, shift_type, imm6);       //operand2 = shiftReg(Rm, shift_type, imm6);

    operand2 = ~operand2;               //not(operand2)
    if (Rn == 31)
    {
        operand1 = 0;
    } else
        operand1 = READ_X_REG(Rn);

    result = operand1 ^ operand2;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void ands_shift_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, shift_type, Rm, imm6, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_SHIFT_REG_OPC(sf, shift_type, Rm, imm6, Rn, Rd);

    if (!sf && (imm6 >> 5))
    {
        UNDEFINED_INSTR;
    }

    if (Rm != 31)
    {
        src = READ_X_REG(Rm);           //src = X[Rm]
    } else
        src = 0;

    shift_reg_imm(&operand2, &src, sf, shift_type, imm6);       //operand2 = shiftReg(Rm, shift_type, imm6);

    if (Rn == 31)
    {
        operand1 = 0;
    } else
        operand1 = READ_X_REG(Rn);

    result = operand1 & operand2;

    if (Rd != 31)
        WRITE_X_REG(Rd, result);

    /*Set NZCV as for a logical operation.NZ as per result, CV cleared. */
    if (result == 0)
        setPStateZF;
    else
        clearPStateZF;

    if (!sf)
    {
        if (result >> 31 & 0x1)
            setPStateNF;
        else
            clearPStateNF;
    } else
    {
        if (result >> 63 & 0x1)
            setPStateNF;
        else
            clearPStateNF;
    }
    clearPStateCF;
    clearPStateVF;
    DEBUG_INSN;
}

static void bics_shift_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, shift_type, Rm, imm6, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_SHIFT_REG_OPC(sf, shift_type, Rm, imm6, Rn, Rd);

    if (!sf && (imm6 >> 5))
    {
        UNDEFINED_INSTR;
    }

    if (Rm != 31)
    {
        src = READ_X_REG(Rm);           //src = X[Rm]
    } else
        src = 0;
    shift_reg_imm(&operand2, &src, sf, shift_type, imm6);       //operand2 = shiftReg(Rm, shift_type, imm6);
    operand2 = ~operand2;               //not(operand2)

    if (Rn == 31)
    {
        operand1 = 0;
    } else
        operand1 = READ_X_REG(Rn);

    result = operand1 & operand2;

    WRITE_X_REG(Rd, result);

    /*Set NZCV as for a logical operation.NZ as per result, CV cleared. */
    if (result == 0)
        setPStateZF;
    else
        clearPStateZF;

    if (!sf)
    {
        if (result >> 31 & 0x1)
            setPStateNF;
        else
            clearPStateNF;
    } else
    {
        if (result >> 63 & 0x1)
            setPStateNF;
        else
            clearPStateNF;
    }
    clearPStateCF;
    clearPStateVF;
    DEBUG_INSN;
}

static void add_shift_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, shift_type, Rm, imm6, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_SHIFT_REG_OPC(sf, shift_type, Rm, imm6, Rn, Rd);

    if ((!sf && (imm6 >> 5)) || shift_type == 3)
    {
        UNDEFINED_INSTR;
    }

    if (Rm != 31)
    {
        src = READ_X_REG(Rm);           //src = X[Rm]
    } else
        src = 0;
    shift_reg_imm(&operand2, &src, sf, shift_type, imm6);       //operand2 = shiftReg(Rm, shift_type, imm6);

    if (Rn == 31)
    {
        operand1 = 0;
    } else
        operand1 = READ_X_REG(Rn);

    if (!sf)
    {
        result = A32_AddWithCarry(state, (ARMword) operand1, (ARMword) operand2, 0, 0);
    } else
    {
        result = A64_AddWithCarry(state, operand1, operand2, 0, 0);
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void adds_shift_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, shift_type, Rm, imm6, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_SHIFT_REG_OPC(sf, shift_type, Rm, imm6, Rn, Rd);

    if ((!sf && (imm6 >> 5)) || shift_type == 3)
    {
        UNDEFINED_INSTR;
    }

    if (Rm != 31)
    {
        src = READ_X_REG(Rm);           //src = X[Rm]
    } else
        src = 0;
    shift_reg_imm(&operand2, &src, sf, shift_type, imm6);       //operand2 = shiftReg(Rm, shift_type, imm6);

    if (Rn == 31)
    {
        operand1 = 0;
    } else
        operand1 = READ_X_REG(Rn);

    if (!sf)
    {
        result = A32_AddWithCarry(state, (ARMword) operand1, (ARMword) operand2, 0, 1); //set nvzc flag
    } else
    {
        result = A64_AddWithCarry(state, operand1, operand2, 0, 1);     //set nvzc flag
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void sub_shift_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, shift_type, Rm, imm6, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_SHIFT_REG_OPC(sf, shift_type, Rm, imm6, Rn, Rd);

    if ((!sf && (imm6 >> 5)) || shift_type == 3)
    {
        UNDEFINED_INSTR;
    }

    if (Rm != 31)
    {
        src = READ_X_REG(Rm);
    } else
        src = 0;

    shift_reg_imm(&operand2, &src, sf, shift_type, imm6);       //operand2 = shiftReg(Rm, shift_type, imm6);
    operand2 = ~operand2;

    if (Rn != 31)
    {
        operand1 = READ_X_REG(Rn);      //operand1 = X[n]
    } else
        operand1 = 0;

    if (!sf)
    {
        result = A32_AddWithCarry(state, (ARMword) operand1, (ARMword) operand2, 1, 0);
    } else
    {
        result = A64_AddWithCarry(state, operand1, operand2, 1, 0);
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void subs_shift_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, shift_type, Rm, imm6, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_SHIFT_REG_OPC(sf, shift_type, Rm, imm6, Rn, Rd);

    if ((!sf && (imm6 >> 5)) || shift_type == 3)
    {
        UNDEFINED_INSTR;
    }

    if (Rm != 31)
    {
        src = READ_X_REG(Rm);
    } else
        src = 0;

    shift_reg_imm(&operand2, &src, sf, shift_type, imm6);       //operand2 = shiftReg(Rm, shift_type, imm6);
    operand2 = ~operand2;

    if (Rn == 31)
    {
        operand1 = 0;
    } else
        operand1 = READ_X_REG(Rn);

    if (!sf)
    {
        result = A32_AddWithCarry(state, (ARMword) operand1, (ARMword) operand2, 1, 1); //set nvzc flag
    } else
    {
        result = A64_AddWithCarry(state, operand1, operand2, 1, 1);     //set nvzc flag
    }

    //Rd = 31, CMP instruction
    if (Rd != 31)
    {
        WRITE_X_REG(Rd, result);
    }
    DEBUG_INSN;
}

static void add_extend_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, option, imm3, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_EXTEND_REG_OPC(sf, Rm, option, imm3, Rn, Rd);

    if (imm3 > 4)
    {
        UNDEFINED_INSTR;
    }

    src = READ_X_REG(Rm);
    ext_and_shift_reg(&operand2, &src, option, imm3);   //ExtendReg(Rm, extend_typem shift);

    operand1 = READ_X_REG(Rn);          //operand1 = Rn

    if (!sf)
    {
        result = A32_AddWithCarry(state, (ARMword) operand1, (ARMword) operand2, 0, 0); //not set nvzc flag
    } else
    {
        result = A64_AddWithCarry(state, operand1, operand2, 0, 0);     //not set nvzc flag
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void adds_extend_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, option, imm3, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_EXTEND_REG_OPC(sf, Rm, option, imm3, Rn, Rd);

    if (imm3 > 4)
    {
        UNDEFINED_INSTR;
    }

    src = READ_X_REG(Rm);
    ext_and_shift_reg(&operand2, &src, option, imm3);   //ExtendReg(Rm, extend_typem shift);

    operand1 = READ_X_REG(Rn);          //operand1 = Rn

    if (!sf)
    {
        result = A32_AddWithCarry(state, (ARMword) operand1, (ARMword) operand2, 0, 1); //set nvzc flag
    } else
    {
        result = A64_AddWithCarry(state, operand1, operand2, 0, 1);     //set nvzc flag
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void sub_extend_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, option, imm3, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_EXTEND_REG_OPC(sf, Rm, option, imm3, Rn, Rd);

    if (imm3 > 4)
    {
        UNDEFINED_INSTR;
    }

    src = READ_X_REG(Rm);
    ext_and_shift_reg(&operand2, &src, option, imm3);   //ExtendReg(Rm, extend_typem shift);
    operand2 = ~operand2;

    operand1 = READ_X_REG(Rn);          //operand1 = Rn

    if (!sf)
    {
        result = A32_AddWithCarry(state, (ARMword) operand1, (ARMword) operand2, 1, 0); //not set nvzc flag
    } else
    {
        result = A64_AddWithCarry(state, operand1, operand2, 1, 0);     //not set nvzc flag
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void subs_extend_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, option, imm3, Rn, Rd;
    ARMdword result, operand1, src, operand2;

    ARM_OPC_GET_EXTEND_REG_OPC(sf, Rm, option, imm3, Rn, Rd);

    if (imm3 > 4)
    {
        UNDEFINED_INSTR;
    }

    if (Rm != 31)
    {
        src = READ_X_REG(Rm);
    } else
        src = 0;

    ext_and_shift_reg(&operand2, &src, option, imm3);   //ExtendReg(Rm, extend_typem shift);
    operand2 = ~operand2;

    if (Rn != 31)
    {
        operand1 = READ_X_REG(Rn);      //operand1 = Rn
    } else
        operand1 = 0;

    if (!sf)
    {
        result = A32_AddWithCarry(state, (ARMword) operand1, (ARMword) operand2, 1, 1); //set nvzc flag
    } else
    {
        result = A64_AddWithCarry(state, operand1, operand2, 1, 1);     //set nvzc flag
    }

    if (Rd != 31)
        WRITE_X_REG(Rd, result);

    DEBUG_INSN;
}

static void adc(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, Rn, Rd;
    ARMdword result, operand1, operand2;

    ARM_OPC_GET_SRC_DST_OPC(sf, Rm, Rn, Rd);

    operand2 = READ_X_REG(Rm);          //operand2 = Rm
    operand1 = READ_X_REG(Rn);          //operand1 = Rn

    if (!sf)
    {
        result = A32_AddWithCarry(state, (ARMword) operand1, (ARMword) operand2, state->PSTATE.C, 0);   //not set nvzc flag
    } else
    {
        result = A64_AddWithCarry(state, operand1, operand2, state->PSTATE.C, 0);       //not set nvzc flag
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void adcs(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, Rn, Rd;
    ARMdword result, operand1, operand2;

    ARM_OPC_GET_SRC_DST_OPC(sf, Rm, Rn, Rd);

    operand2 = READ_X_REG(Rm);          //operand2 = Rm
    operand1 = READ_X_REG(Rn);          //operand1 = Rn

    if (!sf)
    {
        result = A32_AddWithCarry(state, (ARMword) operand1, (ARMword) operand2, state->PSTATE.C, 1);   //set nvzc flag
    } else
    {
        result = A64_AddWithCarry(state, operand1, operand2, state->PSTATE.C, 1);       //set nvzc flag
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void sbc(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, Rn, Rd;
    ARMdword result, operand1, operand2;

    ARM_OPC_GET_SRC_DST_OPC(sf, Rm, Rn, Rd);

    operand2 = READ_X_REG(Rm);          //operand2 = Rm
    operand1 = READ_X_REG(Rn);          //operand1 = Rn

    operand2 = ~operand2;

    if (!sf)
    {
        result = A32_AddWithCarry(state, (ARMword) operand1, (ARMword) operand2, state->PSTATE.C, 0);   //not set nvzc flag
    } else
    {
        result = A64_AddWithCarry(state, operand1, operand2, state->PSTATE.C, 0);       //not set nvzc flag
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void sbcs(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, Rn, Rd;
    ARMdword result, operand1, operand2;

    ARM_OPC_GET_SRC_DST_OPC(sf, Rm, Rn, Rd);

    operand2 = READ_X_REG(Rm);          //operand2 = Rm
    operand1 = READ_X_REG(Rn);          //operand1 = Rn

    operand2 = ~operand2;

    if (!sf)
    {
        result = A32_AddWithCarry(state, (ARMword) operand1, (ARMword) operand2, state->PSTATE.C, 1);   //set nvzc flag
    } else
    {
        result = A64_AddWithCarry(state, operand1, operand2, state->PSTATE.C, 1);       //set nvzc flag
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void rmif(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMdword rn_val;
    ARMword tmp_val;

    ARMword imm6 = BITS(15, 20);
    ARMword Rn = BITS(5, 9);
    ARMword mask = BITS(0, 3);

    rn_val = READ_X_REG(Rn);
    tmp_val = (rn_val >> imm6) & 0xf;

    if (mask & 0x1)
        state->PSTATE.V = tmp_val & 0x1;
    if (mask & 0x2)
        state->PSTATE.C = (tmp_val >> 1) & 0x1;
    if (mask & 0x4)
        state->PSTATE.Z = (tmp_val >> 2) & 0x1;
    if (mask & 0x8)
        state->PSTATE.N = (tmp_val >> 3) & 0x1;

    DEBUG_INSN;
}

static void setf8(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rn, rn_val;

    Rn = BITS(5, 9);

    READ_W_REG(Rn, rn_val);
    state->PSTATE.N = (rn_val >> 7) & 0x1;
    state->PSTATE.Z = (rn_val == 0) ? 1 : 0;
    state->PSTATE.V = ((rn_val >> 8) & 0x1) ^ ((rn_val >> 7) & 0x1);

    DEBUG_INSN;
}

static void setf16(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rn, rn_val;

    Rn = BITS(5, 9);

    READ_W_REG(Rn, rn_val);
    state->PSTATE.N = (rn_val >> 15) & 0x1;
    state->PSTATE.Z = (rn_val == 0) ? 1 : 0;
    state->PSTATE.V = ((rn_val >> 16) & 0x1) ^ ((rn_val >> 15) & 0x1);

    DEBUG_INSN;
}

static void ccmn_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, cond, Rn, nzcv;
    bool_t cond_ret;

    ARM_OPC_GET_CCMP_OPC(sf, Rm, cond, Rn, nzcv);

    cond_ret = ConditionHolds(state, cond);

    if (sf)
    {
        ARMdword operand1 = READ_X_REG(Rn);
        ARMdword operand2 = READ_X_REG(Rm);

        if (cond_ret)
            A64_AddWithCarry(state, operand1, operand2, 0, 1);
        else
        {
            state->PSTATE.N = nzcv & 0x1;
            state->PSTATE.Z = (nzcv >> 1) & 0x1;
            state->PSTATE.C = (nzcv >> 2) & 0x1;
            state->PSTATE.V = (nzcv >> 3) & 0x1;
        }
    } else
    {
        ARMword operand1, operand2;

        READ_W_REG(Rn, operand1);
        READ_W_REG(Rm, operand2);
        if (cond_ret)
            A32_AddWithCarry(state, operand1, operand2, 0, 1);
        else
        {
            state->PSTATE.N = nzcv & 0x1;
            state->PSTATE.Z = (nzcv >> 1) & 0x1;
            state->PSTATE.C = (nzcv >> 2) & 0x1;
            state->PSTATE.V = (nzcv >> 3) & 0x1;
        }
    }
    DEBUG_INSN;
}

static void ccmp_register(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, cond, Rn, nzcv;
    bool_t cond_ret;

    ARM_OPC_GET_CCMP_OPC(sf, Rm, cond, Rn, nzcv);

    cond_ret = ConditionHolds(state, cond);

    if (sf)
    {
        ARMdword operand1 = READ_X_REG(Rn);
        ARMdword operand2 = READ_X_REG(Rm);

        operand2 = ~operand2;
        if (cond_ret)
            A64_AddWithCarry(state, operand1, operand2, 1, 1);
        else
        {
            state->PSTATE.N = nzcv & 0x1;
            state->PSTATE.Z = (nzcv >> 1) & 0x1;
            state->PSTATE.C = (nzcv >> 2) & 0x1;
            state->PSTATE.V = (nzcv >> 3) & 0x1;
        }
    } else
    {
        ARMword operand1, operand2;

        READ_W_REG(Rn, operand1);
        READ_W_REG(Rm, operand2);
        operand2 = ~operand2;
        if (cond_ret)
            A32_AddWithCarry(state, operand1, operand2, 1, 1);
        else
        {
            state->PSTATE.N = nzcv & 0x1;
            state->PSTATE.Z = (nzcv >> 1) & 0x1;
            state->PSTATE.C = (nzcv >> 2) & 0x1;
            state->PSTATE.V = (nzcv >> 3) & 0x1;
        }
    }
    DEBUG_INSN;
}

static void ccmn_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, imm5, cond, Rn, nzcv;
    bool_t cond_ret;

    ARM_OPC_GET_CCMP_OPC(sf, imm5, cond, Rn, nzcv);

    cond_ret = ConditionHolds(state, cond);

    if (sf)
    {
        ARMdword imm = (uint64_t) imm5;
        ARMdword operand = READ_X_REG(Rn);

        if (cond_ret)
            A64_AddWithCarry(state, operand, imm, 0, 1);
        else
        {
            state->PSTATE.N = nzcv & 0x1;
            state->PSTATE.Z = (nzcv >> 1) & 0x1;
            state->PSTATE.C = (nzcv >> 2) & 0x1;
            state->PSTATE.V = (nzcv >> 3) & 0x1;
        }
    } else
    {
        ARMword imm = (uint32_t) imm5;
        ARMword operand;

        READ_W_REG(Rn, operand);
        if (cond_ret)
            A32_AddWithCarry(state, operand, imm, 0, 1);
        else
        {
            state->PSTATE.N = nzcv & 0x1;
            state->PSTATE.Z = (nzcv >> 1) & 0x1;
            state->PSTATE.C = (nzcv >> 2) & 0x1;
            state->PSTATE.V = (nzcv >> 3) & 0x1;
        }
    }
    DEBUG_INSN;
}

static void ccmp_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, imm5, cond, Rn, nzcv;
    bool_t cond_ret;

    ARM_OPC_GET_CCMP_OPC(sf, imm5, cond, Rn, nzcv);

    cond_ret = ConditionHolds(state, cond);

    if (sf)
    {
        ARMdword imm = (uint64_t) imm5;
        ARMdword operand = READ_X_REG(Rn);

        imm = ~imm;
        if (cond_ret)
            A64_AddWithCarry(state, operand, imm, 1, 1);
        else
        {
            state->PSTATE.N = nzcv & 0x1;
            state->PSTATE.Z = (nzcv >> 1) & 0x1;
            state->PSTATE.C = (nzcv >> 2) & 0x1;
            state->PSTATE.V = (nzcv >> 3) & 0x1;
        }
    } else
    {
        ARMword imm = (uint32_t) imm5;
        ARMword operand;

        READ_W_REG(Rn, operand);
        imm = ~imm;
        if (cond_ret)
            A32_AddWithCarry(state, operand, imm, 1, 1);
        else
        {
            state->PSTATE.N = nzcv & 0x1;
            state->PSTATE.Z = (nzcv >> 1) & 0x1;
            state->PSTATE.C = (nzcv >> 2) & 0x1;
            state->PSTATE.V = (nzcv >> 3) & 0x1;
        }
    }
    DEBUG_INSN;
}

static void csel(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, cond, Rn, Rd;
    ARMdword operand1, operand2, result;
    bool_t cond_ret;

    ARM_OPC_GET_CSEL_OPC(sf, Rm, cond, Rn, Rd);

    if (Rn == 31)
        operand1 = 0;
    else
        operand1 = READ_X_REG(Rn);

    if (Rm == 31)
        operand2 = 0;
    else
        operand2 = READ_X_REG(Rm);

    if (!sf)
    {
        operand1 &= 0xffffffff;
        operand2 &= 0xffffffff;
    }

    cond_ret = ConditionHolds(state, cond);

    if (cond_ret)
        result = operand1;
    else
        result = operand2;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void csinc(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, cond, Rn, Rd;
    ARMdword operand1, operand2, result;
    bool_t cond_ret;

    ARM_OPC_GET_CSEL_OPC(sf, Rm, cond, Rn, Rd);

    if (Rn == 31)
        operand1 = 0;
    else
        operand1 = READ_X_REG(Rn);

    if (Rm == 31)
        operand2 = 0;
    else
        operand2 = READ_X_REG(Rm);

    if (!sf)
    {
        operand1 &= 0xffffffff;
        operand2 &= 0xffffffff;
    }

    cond_ret = ConditionHolds(state, cond);

    if (cond_ret)
        result = operand1;
    else
        result = operand2 + 1;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void csinv(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, cond, Rn, Rd;
    ARMdword operand1, operand2, result;
    bool_t cond_ret;

    ARM_OPC_GET_CSEL_OPC(sf, Rm, cond, Rn, Rd);

    if (Rn == 31)
        operand1 = 0;
    else
        operand1 = READ_X_REG(Rn);

    if (Rm == 31)
        operand2 = 0;
    else
        operand2 = READ_X_REG(Rm);

    if (!sf)
    {
        operand1 &= 0xffffffff;
        operand2 &= 0xffffffff;
    }

    cond_ret = ConditionHolds(state, cond);

    if (cond_ret)
        result = operand1;
    else
        result = ~operand2;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void csneg(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, cond, Rn, Rd;
    ARMdword operand1, operand2, result;
    bool_t cond_ret;

    ARM_OPC_GET_CSEL_OPC(sf, Rm, cond, Rn, Rd);

    if (Rn == 31)
        operand1 = 0;
    else
        operand1 = READ_X_REG(Rn);

    if (Rm == 31)
        operand2 = 0;
    else
        operand2 = READ_X_REG(Rm);

    if (!sf)
    {
        operand1 &= 0xffffffff;
        operand2 &= 0xffffffff;
    }

    cond_ret = ConditionHolds(state, cond);

    if (cond_ret)
        result = operand1;
    else
        result = ~operand2 + 1;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void madd(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, Ra, Rn, Rd;
    ARMdword src1, src2, src3, result;

    ARM_OPC_GET_MUL_OPC(sf, Rm, Ra, Rn, Rd);
    src1 = READ_X_REG(Rn);
    src2 = READ_X_REG(Rm);

    if (Ra == 31)
        src3 = 0;
    else
        src3 = READ_X_REG(Ra);

    if (sf)
    {
        result = src3 + (src1 * src2);
        WRITE_X_REG(Rd, result);
    } else
    {
        result = (uint32_t) src3 + ((uint32_t) src1 * (uint32_t) src2);
        WRITE_X_REG(Rd, result);
    }
    DEBUG_INSN;
}

static void msub(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, Ra, Rn, Rd;
    ARMdword src1, src2, src3, result;

    ARM_OPC_GET_MUL_OPC(sf, Rm, Ra, Rn, Rd);
    src1 = READ_X_REG(Rn);
    src2 = READ_X_REG(Rm);

    if (Ra == 31)
        src3 = 0;
    else
        src3 = READ_X_REG(Ra);

    if (sf)
    {
        result = src3 - (src1 * src2);
        WRITE_X_REG(Rd, result);
    } else
    {
        result = (uint32_t) src3 - ((uint32_t) src1 * (uint32_t) src2);
        WRITE_X_REG(Rd, result);
    }
    DEBUG_INSN;
}

static void smaddl(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, Ra, Rn, Rd;
    ARMword src1, src2;
    ARMdword src3, result;

    ARM_OPC_GET_MUL_OPC(sf, Rm, Ra, Rn, Rd);
    src1 = READ_X_REG(Rn);
    src2 = READ_X_REG(Rm);

    if (Ra == 31)
        src3 = 0;
    else
        src3 = READ_X_REG(Ra);

    result = (int64_t) src3 + ((int32_t) src1 * (int32_t) src2);
    WRITE_X_REG(Rd, result);

    DEBUG_INSN;
}

static void smsubl(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, Ra, Rn, Rd;
    ARMword src1, src2;
    ARMdword src3, result;

    ARM_OPC_GET_MUL_OPC(sf, Rm, Ra, Rn, Rd);
    src1 = READ_X_REG(Rn);
    src2 = READ_X_REG(Rm);

    if (Ra == 31)
        src3 = 0;
    else
        src3 = READ_X_REG(Ra);

    result = (int64_t) src3 - ((int32_t) src1 * (int32_t) src2);
    WRITE_X_REG(Rd, result);

    DEBUG_INSN;
}

static void smulh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void umaddl(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, Ra, Rn, Rd;
    ARMword src1, src2;
    ARMdword src3, result;

    ARM_OPC_GET_MUL_OPC(sf, Rm, Ra, Rn, Rd);
    src1 = READ_X_REG(Rn);
    src2 = READ_X_REG(Rm);

    if (Ra != 31)
    {
        src3 = READ_X_REG(Ra);
    } else
        src3 = 0;

    result = (uint64_t) src3 + ((uint32_t) src1 * (uint32_t) src2);
    WRITE_X_REG(Rd, result);

    DEBUG_INSN;
}

static void umsubl(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, Rm, Ra, Rn, Rd;
    ARMword src1, src2;
    ARMdword src3, result;

    ARM_OPC_GET_MUL_OPC(sf, Rm, Ra, Rn, Rd);
    src1 = READ_X_REG(Rn);
    src2 = READ_X_REG(Rm);

    if (Ra != 31)
    {
        src3 = READ_X_REG(Ra);
    } else
        src3 = 0;

    result = (uint64_t) src3 - ((uint32_t) src1 * (uint32_t) src2);
    WRITE_X_REG(Rd, result);

    DEBUG_INSN;
}

static void umulh(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void udiv32(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd;
    ARMword operand1, operand2, result;

    ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd);

    READ_W_REG(Rn, operand1);
    READ_W_REG(Rm, operand2);

    if (operand2 == 0)
        result = 0;
    else
        result = operand1 / operand2;

    WRITE_W_REG(Rd, result);
    DEBUG_INSN;
}

static void sdiv32(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd;
    int operand1, operand2, result;

    ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd);

    READ_W_REG(Rn, operand1);
    READ_W_REG(Rm, operand2);

    if (operand2 == 0)
        result = 0;
    else
        result = operand1 / operand2;

    WRITE_W_REG(Rd, result);
    DEBUG_INSN;
}

static void lslv32(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd;
    ARMword operand1, operand2, result, amount;

    ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd);
    READ_W_REG(Rn, operand1);
    READ_W_REG(Rm, operand2);

    amount = operand2 % 32;
    result = operand1 << amount;

    WRITE_W_REG(Rd, result);
    DEBUG_INSN;
}

//Âß¼­ÓÒÒÆ
static void lsrv32(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd;
    ARMword operand1, operand2, result, amount;

    ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd);
    READ_W_REG(Rn, operand1);
    READ_W_REG(Rm, operand2);

    amount = operand2 % 32;
    result = operand1 >> amount;

    WRITE_W_REG(Rd, result);
    DEBUG_INSN;
}

//ËãÊõÓÒÒÆ
static void asrv32(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd;
    ARMword operand1, operand2, result, amount;

    ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd);
    READ_W_REG(Rn, operand1);
    READ_W_REG(Rm, operand2);

    amount = operand2 % 32;
    if ((operand1 >> 31) & 0x1)
    {
        BIT32_SHIFT(result, amount);
        result |= (operand1 >> amount);
    } else
        result = operand1 >> amount;

    WRITE_W_REG(Rd, result);
    DEBUG_INSN;
}

static void rorv32(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd;
    ARMword operand1, operand2, result, amount;

    ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd);
    READ_W_REG(Rn, operand1);
    READ_W_REG(Rm, operand2);

    amount = operand2 % 32;
    result = ROR32(operand1, amount);

    WRITE_W_REG(Rd, result);
    DEBUG_INSN;
}

static void crc32_32(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd, sz, C;
    ARMword size, acc, val, result;
    ARMword poly;
    ARMdword tmpacc = 0;
    ARMdword tmpval = 0;

    ARM_OPC_GET_CRC_OPC(Rm, C, sz, Rn, Rd);

    if (C == 0)
        poly = POLY_VAL;
    else
        poly = POLYC_VAL;

    READ_W_REG(Rn, acc);                //acc = X[n]
    READ_W_REG(Rm, val);                //val = X[m]

    if (sz >= 3)
    {
        //UNDEFINED
        return;
    }

    size = 8 << sz;

    if (size == 8)
        val = val & 0xFF;
    else if (size == 16)
        val = val & 0xFFFF;

    tmpacc = (uint64_t) revbit32(acc) << size;  // acc:size
    tmpval = (uint64_t) revbit32(val) << 32;    //val:32

    result = Poly32Mod2(tmpacc ^ tmpval, poly, size + 32);
    result = revbit32(result);

    WRITE_W_REG(Rd, result);
    DEBUG_INSN;
}

static void udiv64(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd;
    ARMdword operand1, operand2;
    ARMword result;

    ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd);

    operand1 = READ_X_REG(Rn);
    operand2 = READ_X_REG(Rm);

    if (operand2 == 0)
        result = 0;
    else
        result = operand1 / operand2;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void sdiv64(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd;
    int64_t operand1, operand2;
    int result;

    ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd);

    operand1 = READ_X_REG(Rn);
    operand2 = READ_X_REG(Rm);

    if (operand2 == 0)
        result = 0;
    else
        result = operand1 / operand2;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void lslv64(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd;
    ARMdword operand1, operand2, result;
    ARMword amount;

    ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd);
    operand1 = READ_X_REG(Rn);
    operand2 = READ_X_REG(Rm);

    amount = operand2 % 64;
    result = operand1 << amount;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void lsrv64(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd;
    ARMdword operand1, operand2, result;
    ARMword amount;

    ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd);
    operand1 = READ_X_REG(Rn);
    operand2 = READ_X_REG(Rm);

    amount = operand2 % 64;
    result = operand1 >> amount;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void asrv64(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd;
    ARMdword operand1, operand2, result;
    ARMword amount;

    ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd);
    operand1 = READ_X_REG(Rn);
    operand2 = READ_X_REG(Rm);

    amount = operand2 % 64;
    if ((operand1 >> 63) & 0x1)
    {
        BIT64_SHIFT(result, amount);
        result |= (operand1 >> amount);
    } else
        result = operand1 >> amount;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void rorv64(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd;
    ARMdword operand1, operand2, result;
    ARMword amount;

    ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd);
    operand1 = READ_X_REG(Rn);
    operand2 = READ_X_REG(Rm);

    amount = operand2 % 64;
    result = ROR64(operand1, amount);

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void pacga(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd;
    ARMdword operand1, operand2, result;

    ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd);
    operand1 = READ_X_REG(Rm);
    operand2 = READ_X_REG(Rn);

    addPACGA(state, operand1, operand2);
    DEBUG_INSN;
}

static void crc32_64(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword Rm, Rn, Rd, sz, C;
    ARMword size, acc;
    ARMdword val, result;
    ARMword poly;;
    ARMdword tmpacc = 0;
    ARMdword tmpval = 0;

    ARM_OPC_GET_CRC_OPC(Rm, C, sz, Rn, Rd);
    if (C == 0)
        poly = POLY_VAL;
    else
        poly = POLYC_VAL;

    READ_W_REG(Rn, acc);                //acc = X[n]
    val = READ_X_REG(Rm);               //val = X[m]

    if (sz < 3)
    {
        //UNDEFINED
        return;
    }

    size = 8 << sz;

    tmpacc = (uint64_t) revbit32(acc) << 32;    // acc:size
    tmpval = revbit64(val) << 32;       //val:32

    result = Poly32Mod2(tmpacc ^ tmpval, poly, size);
    result = revbit64(result);

    WRITE_X_REG(Rd, result);
}

static void add32_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sh, Rn, Rd;
    ARMword imm12, operand1, result;

    ARM_OPC_GET_DATA_IMM12(sh, imm12, Rn, Rd);

    if (sh == 1)
        imm12 <<= 12;

    READ_W_REG(Rn, operand1);

    result = A32_AddWithCarry(state, operand1, imm12, 0, 0);
    WRITE_W_REG(Rd, result);
    WRITE_X_HIGH_REG(Rd, 0);
    DEBUG_INSN;
}

static void adds32_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sh, Rn, Rd;
    ARMword imm12, operand1, result;

    ARM_OPC_GET_DATA_IMM12(sh, imm12, Rn, Rd);

    if (sh == 1)
        imm12 <<= 12;

    READ_W_REG(Rn, operand1);

    result = A32_AddWithCarry(state, operand1, imm12, 0, 1);
    WRITE_W_REG(Rd, result);
    WRITE_X_HIGH_REG(Rd, 0);
    DEBUG_INSN;
}

static void add64_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sh, Rn, Rd;
    ARMdword imm12, operand1, result;

    ARM_OPC_GET_DATA_IMM12(sh, imm12, Rn, Rd);

    if (sh == 1)
        imm12 <<= 12;

    operand1 = READ_X_REG(Rn);

    result = A64_AddWithCarry(state, operand1, imm12, 0, 0);
    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void adds64_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sh, Rn, Rd;
    ARMdword imm12, operand1, result;

    ARM_OPC_GET_DATA_IMM12(sh, imm12, Rn, Rd);

    if (sh == 1)
        imm12 <<= 12;

    operand1 = READ_X_REG(Rn);

    result = A64_AddWithCarry(state, operand1, imm12, 0, 1);
    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void sub32_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sh, Rn, Rd;
    ARMword imm12, operand1, operand2, result;

    ARM_OPC_GET_DATA_IMM12(sh, imm12, Rn, Rd);

    if (sh == 1)
        imm12 <<= 12;

    READ_W_REG(Rn, operand1);
    operand2 = (ARMword) (imm12 ^ BIT32_MAX_VALUE);

    result = A32_AddWithCarry(state, operand1, operand2, 1, 0);

    WRITE_W_REG(Rd, result);
    WRITE_X_HIGH_REG(Rd, 0);

    DEBUG_INSN;
}

static void subs32_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sh, Rn, Rd;
    ARMword imm12, operand1, operand2, result;

    ARM_OPC_GET_DATA_IMM12(sh, imm12, Rn, Rd);

    if (sh == 1)
        imm12 <<= 12;

    READ_W_REG(Rn, operand1);
    operand2 = (ARMword) (imm12 ^ BIT32_MAX_VALUE);

    result = A32_AddWithCarry(state, operand1, operand2, 1, 1);

    if (Rd != 0x1f)
    {
        WRITE_W_REG(Rd, result);
        WRITE_X_HIGH_REG(Rd, 0);
    }
    DEBUG_INSN;
}

static void sub64_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sh, Rn, Rd;
    ARMdword imm12, operand1, operand2, result;

    ARM_OPC_GET_DATA_IMM12(sh, imm12, Rn, Rd);

    if (sh == 1)
        imm12 <<= 12;

    operand1 = READ_X_REG(Rn);
    operand2 = (ARMdword) (imm12 ^ BIT64_MAX_VALUE);

    result = A64_AddWithCarry(state, operand1, operand2, 1, 0);

    WRITE_X_REG(Rd, result);

    DEBUG_INSN;
}

static void subs64_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sh, Rn, Rd;
    ARMdword imm12, operand1, operand2, result;

    ARM_OPC_GET_DATA_IMM12(sh, imm12, Rn, Rd);
    if (sh == 1)
        imm12 <<= 12;

    operand1 = READ_X_REG(Rn);
    operand2 = (ARMdword) (imm12 ^ BIT64_MAX_VALUE);

    result = A64_AddWithCarry(state, operand1, operand2, 1, 1);
    //Rd = 31 Alias CMP instruction
    if (Rd != 0x1f)
        WRITE_X_REG(Rd, result);

    DEBUG_INSN;
}

static void movn_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, hw, Rd, shift;
    ARMdword imm16;
    ARMdword result = 0;

    ARM_OPC_GET_MOV_IMM16(hw, imm16, Rd);
    sf = BITS(31, 31);
    if (hw >> 1)
    {
        //UNDEFINED
        UNDEFINED_INSTR;
    }

    shift = 16 * hw;
    result = imm16 << shift;
    result = ~result;                   //not(result)
    if (!sf)
        result &= BIT32_MAX_VALUE;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void movk_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, hw, Rd, shift;
    ARMdword imm16;
    ARMdword result;

    ARM_OPC_GET_MOV_IMM16(hw, imm16, Rd);
    sf = BITS(31, 31);

    if (hw >> 1)
    {
        UNDEFINED_INSTR;
    }

    result = READ_X_REG(Rd);
    shift = 16 * hw;

    if (!sf)
    {
        if (shift == 0)
            result = imm16 | (result & 0xFFFF0000);
        else
            result = (imm16 << shift) | (result & 0xFFFF);
    } else
    {
        result = GetShiftValue(result, shift);
        result = result | imm16 << shift;
    }

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void movz_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword sf, hw, Rd, shift;
    ARMdword imm16;
    ARMdword result = 0;

    ARM_OPC_GET_MOV_IMM16(hw, imm16, Rd);
    sf = BITS(31, 31);
    if (hw >> 1 && sf == 0)
    {
        //UNDEFINED
        UNDEFINED_INSTR;
    }

    shift = 16 * hw;
    result = imm16 << shift;
    if (!sf)
        result &= BIT32_MAX_VALUE;

    WRITE_X_REG(Rd, result);
    DEBUG_INSN;
}

static void ic(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword L, op1, CRn, CRm, op2, Rt;

    ARM_OPC_GET_SYSTEM_OPC(L, op1, CRn, CRm, op2, Rt);
    if (op1 == 0 && CRm == 1 && op2 == 0)
    {
        //IALLUIS
        if (state->PSTATE.EL == EL1)
        {
            if (EL2Enabled(state) && !ELUSingAArch32(EL2) &&
                (state->system_reg.HCR_EL2.TPU || state->system_reg.HCR_EL2.TICAB))
                systemAccessTrap(state, EL2, 0x18, state->A64_PC);
        }
    } else if (op1 == 0 && CRm == 5 && op2 == 0)
    {
        //IALLU
    } else if (op1 == 3 && CRm == 5 && op2 == 1)
    {
        //IVAU
    } else
    {
        NOT_IMPLEMENT;
    }

    DEBUG_INSN;
}

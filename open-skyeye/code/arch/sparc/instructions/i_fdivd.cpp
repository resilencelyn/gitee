/*
 * =====================================================================================
 *
 *       Filename:  i_fdivd.c
 *
 *    Description:  Implementation of the FDIVD instruction
 *
 *        Version:  1.0
 *        Created:  12/11/08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David.Yu keweihk@gmail.com
 *        Company:  Tsinghua
 *
 * =====================================================================================
 */

/*  Format (3)
 * 
 * 31-30  29-25   24-19     18-14         13-5              4-0
 * +----+-------+--------+------------------------------------------+
 * | 10 |   rd  | 110100 |   rs1   |      opf           |    rs2    |
 * +----+-------+--------+------------------------------------------+
 * 
 * op              = 10
 * rd              = 00000
 * opf          = 001001110
 * 
 * 31-30  29-25   24-19     18-14       13-5             4-0
 * +----+-------+--------+------------------------------------------+
 * | 10 | 00000 | 110100 |   rs1   |     001001110      |   rs2  |
 * +----+-------+--------+------------------------------------------+
 * 
 */
#include "skyeye_fp.h"
#include "common/bits.h"
#include "common/sparc.h"
#include "common/traps.h"
#include "common/fpu.h"
#include "common/iu.h"
#include "i_utils.h"

#include "sparc_instr.h"
#include <stdio.h>

static int execute(sparc_state_t * sparc_state, uint32 instr);
static int disassemble(uint32 instr, void *state);

#define FDIVD_CYCLES    1
#define FDIVD_CODE_MASK 0x81a009c0
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x2

#define OPF_OFF_first     5
#define OPF_OFF_last      13
#define OPF         0x4e

#define RD_OFF_first      25
#define RD_OFF_last       29

#define RS1_OFF_first     14
#define RS1_OFF_last      18

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define FMT_OFF_first	19
#define FMT_OFF_last	24
#define FMT		0x34



sparc_instruction_t i_fdivd = { " i_fdivd",
    execute,
    disassemble,
    FDIVD_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    int rd, rs1, rs2, fmt;

    fmt = bits(instr, FMT_OFF_last, FMT_OFF_first);
    rd = bits(instr, RD_OFF_last, RD_OFF_first);
    rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
    rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);
    /*
     * va_* are used to save the registers' value
     */
    d_precision va_rd, va_rs1, va_rs2;
    int cexc = 0;

    /*  Check whether the FPU is enabled or not */
    if (!bit(PSRREG, PSR_EF))
    {
        traps->signal(sparc_state->obj, FP_DISABLED);
        fsr_set_ftt(unimplemented_FPop);
        return (FDIVD_CYCLES);
    }
#if 0
    if ((rs1 & 1) || (rs2 & 1))
    {
        traps->signal(sparc_state->obj, FP_EXCEPTION);
        fsr_set_ftt(invalid_fp_register);
        return (FDIVD_CYCLES);
    }
#endif
    va_rs1.data = fpu_get_double(sparc_state, rs1);
    va_rs2.data = fpu_get_double(sparc_state, rs2);

    double x, y, z;

    x = va_rs1.value;
    y = va_rs2.value;
    if (y == 0)
    {
        if (x == 0)
        {
            /* 0 / 0 = nan */
            cexc |= IEEE_FNV;
#ifdef __WIN32__
            va_rd.data = 0x7fffe00000000000ULL;
#else
            va_rd.data = 0x7fffe00000000000UL;
#endif
        } else
        {
            /* num / 0 = inf */
            cexc |= IEEE_FDZ;
#ifdef __WIN32__
            va_rd.data = 0x7ff0000000000000ULL;
#else
            va_rd.data = 0x7ff0000000000000UL;
#endif
        }
    } else
    {
        /* We have 2 ops to div */
        va_rd.value = z = x / y;
        if (z > D_max.value)
        {
            /* overflow and inexact */
            cexc |= (IEEE_FOF | IEEE_FDX);
        } else if (z < D_min.value)
        {
            /* underflow and inexact */
            cexc |= (IEEE_FUF | IEEE_FDX);
        } else if (z == va_rd.value)
            print_inst_RS_RS_RD("fdivd", rs1, rs2, rd);
    }

    fpu_put_double(sparc_state, va_rd.data, rd);
    if ((ieee_754_exception(sparc_state, cexc)) == 1)
        return (FDIVD_CYCLES);

    PCREG = NPCREG;
    NPCREG += 4;

    fsr_clear_ftt();
    // Everyting takes some time
    return FDIVD_CYCLES;

}

static int disassemble(uint32 instr, void *state)
{
    int opf = 0, op;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    opf = bits(instr, OPF_OFF_last, OPF_OFF_first);
    int rd, rs1, rs2, fmt;

    fmt = bits(instr, FMT_OFF_last, FMT_OFF_first);
    rd = bits(instr, RD_OFF_last, RD_OFF_first);
    rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
    rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);

    if ((instr & FDIVD_CODE_MASK) && (op == OP) && (opf == OPF) && (fmt == FMT))
    {
        rd = bits(instr, RD_OFF_last, RD_OFF_first);
        rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
        rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);

        return 1;
    }
    return 0;
}


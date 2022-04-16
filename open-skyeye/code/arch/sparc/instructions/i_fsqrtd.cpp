/*
 * =====================================================================================
 *
 *       Filename:  i_fsqrtd.c
 *
 *    Description:  Implementation of the FSQRTD instruction
 *
 *        Version:  1.0
 *        Created:  16/04/08 18:09:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shenoubang shenoubang@gmail.com
 *        Company:  TsingHua
 *
 * =====================================================================================
 */

/*  Format (3)
 *
 31-30  29-25     24-19        18-14      13-5       4-0
 +----+-------+-----------+------------+---------+-----------+
 | op |   rd  |    op3    |     rs1    |   opf   |    rs2    |
 +----+-------+-----------+------------+---------+-----------+
 op              = 10
 op3                            = 110100
 rs1                            = 00000
 opf                            = 000101010
 
 31-30  29-25     24-19        18-14         13-5           4-0
 +----+-------+-----------+------------+---------------+-----------+
 | op |   rd  |  110100   |    00000   |   000101010   |    rs2    |
 +----+-------+-----------+------------+---------------+-----------+
 */
#include "skyeye_fp.h"
#include "bits.h"
#include "sparc.h"
#include "traps.h"
#include "fpu.h"
#include "iu.h"
#include "i_utils.h"

#include "sparc_instr.h"
#include <stdio.h>
#include <math.h>

static int execute(sparc_state_t * sparc_state, uint32 instr);
static int disassemble(uint32 instr, void *state);

#define FSQRTD_CYCLES    1
#define FSQRTD_CODE_MASK 0x81A00540
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x2

#define OPF_OFF_first    5
#define OPF_OFF_last     13
#define OPF         	0x2A

#define RD_OFF_first	25
#define RD_OFF_last		29

#define RS2_OFF_first	0
#define RS2_OFF_last	4

#define FMT_OFF_first	19
#define FMT_OFF_last	24
#define FMT		0x34

sparc_instruction_t i_fsqrtd = { " i_fsqrtd",
    execute,
    disassemble,
    FSQRTD_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    int rd, rs2, fmt;

    fmt = bits(instr, FMT_OFF_last, FMT_OFF_first);
    rd = bits(instr, RD_OFF_last, RD_OFF_first);
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
        return (FSQRTD_CYCLES);
    }
    if ((rd & 1) || (rs2 & 1))
    {
        traps->signal(sparc_state->obj, FP_EXCEPTION);
        fsr_set_ftt(invalid_fp_register);
        return (FSQRTD_CYCLES);
    }
    va_rs2.data = fpu_get_double(sparc_state, rs2);

    double y, z;

    y = va_rs2.value;

    /* We have 2 ops to div */
    va_rd.value = z = sqrt(y);
    if (va_rd.value != z)
    {
        cexc |= IEEE_FDX;
    }
    if (dwNAN(va_rd.data))
    {
        cexc |= IEEE_FNV;
    }
    //printf("fsqrtd f%d, f%d------------------\n", rs2, rd);
    //printf("%g(0x%lx) * %g(0x%lx) = %g(0x%lx)\n",va_rs1.value, va_rs1.data, va_rs1.value, va_rs1.data, va_rd.value, va_rd.data);

    fpu_put_double(sparc_state, va_rd.data, rd);
    if ((ieee_754_exception(sparc_state, cexc)) == 1)
        return (FSQRTD_CYCLES);

    PCREG = NPCREG;
    NPCREG += 4;

    fsr_clear_ftt();
    // Everyting takes some time
    return FSQRTD_CYCLES;
}

static int disassemble(uint32 instr, void *state)
{
    int opf = 0, op;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    opf = bits(instr, OPF_OFF_last, OPF_OFF_first);
    int rd, rs2, fmt;

    fmt = bits(instr, FMT_OFF_last, FMT_OFF_first);
    rd = bits(instr, RD_OFF_last, RD_OFF_first);
    rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);

    if ((instr & FSQRTD_CODE_MASK) && (op == OP) && (opf == OPF) && (fmt == FMT))
    {
        rd = bits(instr, RD_OFF_last, RD_OFF_first);
        rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);

        return 1;
    }
    return 0;
}


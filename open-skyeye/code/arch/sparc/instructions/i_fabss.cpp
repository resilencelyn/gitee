/*
 * =====================================================================================
 *
 *       Filename:  i_fabss.c
 *
 *    Description:  Implementation of the FADDD instruction
 *
 *        Version:  1.0
 *        Created:  12/11/08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shenoubang shenoubang@gmail.com
 *        Company:  Tsinghua
 *
 * =====================================================================================
 */

/*  Format (3)
 * 
 * 31-30  29-25   24-19     18-14         13-5              4-0
 * +----+-------+--------+------------------------------------------+
 * | 10 |   rd  | 110100 |   ___   |      opf           |    rs2    |
 * +----+-------+--------+------------------------------------------+
 * 
 * op      = 10
 * opf          = 000001001
 * 
 * 31-30  29-25   24-19     18-14       13-5             4-0
 * +----+-------+--------+------------------------------------------+
 * | 10 |   rd  | 110100 |   ___   |     000001001      |   rs2  |
 * +----+-------+--------+------------------------------------------+
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

#define FABSS_CYCLES    1
#define FABSS_CODE_MASK 0X81A00120
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x2

#define OPF_OFF_first     5
#define OPF_OFF_last      13
#define OPF         0x09

#define RD_OFF_first      25
#define RD_OFF_last       29

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define FMT_OFF_first	19
#define FMT_OFF_last	24
#define FMT		0x34


sparc_instruction_t i_fabss = { " i_fabss",
    execute,
    disassemble,
    FABSS_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    int rd, rs2, fmt;

    rd = bits(instr, RD_OFF_last, RD_OFF_first);
    rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);
    fmt = bits(instr, FMT_OFF_last, FMT_OFF_first);

    /*
     * va_* are used to save the registers' value
     */
    s_precision va_rd, va_rs2;
    int cexc = 0;

    /*  Check whether the FPU is enabled or not */
    if (!bit(PSRREG, PSR_EF))
    {
        traps->signal(sparc_state->obj, FP_DISABLED);
        fsr_set_ftt(unimplemented_FPop);
        return (FABSS_CYCLES);
    }

    va_rs2.data = fpu_get_single(sparc_state, rs2);

    /* We have 2 ops to div */
    if (va_rs2.value < 0)
        va_rd.value = va_rs2.value * (-1);
    else
        va_rd.value = va_rs2.value;

    fpu_put_single(sparc_state, va_rd.data, rd);
    if ((ieee_754_exception(sparc_state, cexc)) == 1)
        return (FABSS_CYCLES);

    PCREG = NPCREG;
    NPCREG += 4;

    fsr_clear_ftt();
    // Everyting takes some time
    return FABSS_CYCLES;

}

static int disassemble(uint32 instr, void *state)
{
    int opf = 0, op;

    int rd, rs2, fmt;

    fmt = bits(instr, FMT_OFF_last, FMT_OFF_first);

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    opf = bits(instr, OPF_OFF_last, OPF_OFF_first);

    if (((instr & FABSS_CODE_MASK) == FABSS_CODE_MASK) && (op == OP) && (opf == OPF) && (fmt == FMT))
    {
        rd = bits(instr, RD_OFF_last, RD_OFF_first);
        rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);

        return 1;
    }
    return 0;
}
/*
 * =====================================================================================
 *
 *       Filename:  i_fcmped.c
 *
 *    Description:  Implementation of the FCMPD instruction
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
 * | 10 |  abz  | 110101 |   rs1   |      opf           |    rs2    |
 * +----+-------+--------+------------------------------------------+
 * 
 * op              = 10
 * rd              = 00000
 * opf                          = 001010110
 * 
 * 31-30  29-25   24-19     18-14       13-5             4-0
 * +----+-------+--------+------------------------------------------+
 * | 10 | 00000 | 110100 |   rs1   |     001010110      |   rs2  |
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
#include "i_futils.h"

#include "sparc_instr.h"
#include <stdio.h>

static int execute(sparc_state_t * sparc_state, uint32 instr);
static int disassemble(uint32 instr, void *state);

#define FCMPED_CYCLES    1
#define FCMPED_CODE_MASK 0x81A80AC0
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x2

#define OPF_OFF_first    5
#define OPF_OFF_last     13
#define OPF         0x56

#define RS1_OFF_first     14
#define RS1_OFF_last      18

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define FMT_OFF_first	19
#define FMT_OFF_last	24
#define FMT		0x35



sparc_instruction_t i_fcmped = { " i_fcmped",
    execute,
    disassemble,
    FCMPED_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    /*
     * va_* are used to save the registers' value
     */
    int rs1, rs2;

    rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
    rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);
    d_precision va_rs1, va_rs2;
    double result = 0;

    /*  Check whether the FPU is enabled or not */
    if (!bit(PSRREG, PSR_EF))
    {
        traps->signal(sparc_state->obj, FP_DISABLED);
        fsr_set_ftt(unimplemented_FPop);
        return (FCMPED_CYCLES);
    }
#if 0
    if ((rs1 & 1) || (rs2 & 1))
    {
        traps->signal(sparc_state->obj, FP_EXCEPTION);
        fsr_set_ftt(invalid_fp_register);
        return (FCMPED_CYCLES);
    }
#endif

    int cexc = 0;

    va_rs1.data = fpu_get_double(sparc_state, rs1);
    va_rs2.data = fpu_get_double(sparc_state, rs2);
#if 0
    if (dwNAN(va_rs1.data) || dwNAN(va_rs2.data))
    {
        printf("pc:%x, error 2\n", PCREG);
        cexc |= IEEE_FNV;
    }
#endif

    if ((ieee_754_exception(sparc_state, cexc)) == 1)
        return (FCMPED_CYCLES);

    //result = (double)((double)va_rs1.data - (double)va_rs2.data); 
    result = (va_rs1.value - va_rs2.value);

    /*双精度最小值为2(-1074)即4.9E-324 */
    if (result < 4.9E-324 && result > -4.9E-324)
        result = 0;

    if (result == 0)
    {
        fsr_set_fcc(0);
    } else if (result < 0)
    {
        fsr_set_fcc(1);
    } else if (result > 0)
    {
        fsr_set_fcc(2);
    } else
    {
        fsr_set_fcc(3);
    }

    PCREG = NPCREG;
    NPCREG += 4;

    fsr_clear_ftt();
    // Everyting takes some time
    return FCMPED_CYCLES;
}

static int disassemble(uint32 instr, void *state)
{
    int opf = 0, op, fmt;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    opf = bits(instr, OPF_OFF_last, OPF_OFF_first);
    fmt = bits(instr, FMT_OFF_last, FMT_OFF_first);

    if (((instr & FCMPED_CODE_MASK) == FCMPED_CODE_MASK) && (op == OP) && (opf == OPF) && (fmt = FMT))
    {
        int rs1, rs2;

        rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
        rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);

        return 1;
    }
    return 0;
}
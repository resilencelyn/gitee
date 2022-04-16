/*
 * =====================================================================================
 *
 *       Filename:  i_fcmpes.c
 *
 *    Description:  Implementation of the FCMPS instruction
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
 * | 10 |  abz  | 110101 |   rs1   |      opf           |    rs2    |
 * +----+-------+--------+------------------------------------------+
 * 
 * op              = 10
 * rd              = 00000
 * opf          = 001010001
 * 
 * 31-30  29-25   24-19     18-14       13-5             4-0
 * +----+-------+--------+------------------------------------------+
 * | 10 | 00000 | 110100 |   rs1   |     001010101      |   rs2  |
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

#define FCMPS_CYCLES    1
#define FCMPS_CODE_MASK 0x81a80aa0
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x2

#define OPF_OFF_first    5
#define OPF_OFF_last     13
#define OPF         0x55

#define RS1_OFF_first     14
#define RS1_OFF_last      18

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define FMT_OFF_first	19
#define FMT_OFF_last	24
#define FMT		0x35


sparc_instruction_t i_fcmpes = { " i_fcmpes",
    execute,
    disassemble,
    FCMPS_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    int rs1, rs2;

    rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
    rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);
    /*
     * va_* are used to save the registers' value
     */
    s_precision va_rs1, va_rs2;
    float result = 0;

    /*  Check whether the FPU is enabled or not */
    if (!bit(PSRREG, PSR_EF))
    {
        traps->signal(sparc_state->obj, FP_DISABLED);
        fsr_set_ftt(unimplemented_FPop);
        return (FCMPS_CYCLES);
    }
#if 0
    if ((rs1 & 2) || (rs2 & 2))
    {
        printf("line :%d\n", __LINE__);
        traps->signal(sparc_state->obj, FP_EXCEPTION);
        fsr_set_ftt(invalid_fp_register);
        return (FCMPS_CYCLES);
    }
#endif
    va_rs1.data = fpu_get_single(sparc_state, rs1);
    va_rs2.data = fpu_get_single(sparc_state, rs2);
    result = va_rs1.value - va_rs2.value;

    /*单精度最小值为(1*(e-22)*(e-127)) = 2(-149)即1.4E-45 */
    if (result < 1.4E-45 && result > -1.4E-45)
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

    print_fpinst_RS_RS("fcmpes", rs1, rs2);
    PCREG = NPCREG;
    NPCREG += 4;

    fsr_clear_ftt();
    // Everyting takes some time
    return FCMPS_CYCLES;
}

static int disassemble(uint32 instr, void *state)
{
    int opf = 0, op, fmt;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    opf = bits(instr, OPF_OFF_last, OPF_OFF_first);
    fmt = bits(instr, FMT_OFF_last, FMT_OFF_first);

    if (((instr & FCMPS_CODE_MASK) == FCMPS_CODE_MASK) && (op == OP) && (opf == OPF) && (fmt = FMT))
    {
        int rs1, rs2;

        rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
        rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);

        return 1;
    }
    return 0;
}

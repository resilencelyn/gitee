/*
 * =====================================================================================
 *
 *       Filename:  i_fdtoi.c
 *
 *    Description:  Implementation of the FSTOD instruction
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
 * | 10 |   rd  | 110100 |   NBZ   |      opf           |    rs2    |
 * +----+-------+--------+------------------------------------------+
 * 
 * op      = 10
 * opf          = 011010010
 * 
 * 31-30  29-25   24-19     18-14       13-5             4-0
 * +----+-------+--------+------------------------------------------+
 * | 10 |   rd  | 110100 |   NBZ   |     011010010      |   rs2  |
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

#define FDTOI_CYCLES    1
#define FDTOI_CODE_MASK	0X81A01A40
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x2

#define OPF_OFF_first     5
#define OPF_OFF_last      13
#define OPF         0xd2

#define RD_OFF_first      25
#define RD_OFF_last       29

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define FMT_OFF_first	19
#define FMT_OFF_last	24
#define FMT		0x34


sparc_instruction_t i_fdtoi = { " i_fdtoi",
    execute,
    disassemble,
    FDTOI_CODE_MASK,
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
    d_precision va_rs2;
    s_precision va_rd;
    int cexc = 0;

    /*  Check whether the FPU is enabled or not */
    if (!bit(PSRREG, PSR_EF))
    {
        traps->signal(sparc_state->obj, FP_DISABLED);
        fsr_set_ftt(unimplemented_FPop);
        return (FDTOI_CYCLES);
    }
#if 0
    if (rs2 & 1)
    {
        traps->signal(sparc_state->obj, FP_EXCEPTION);
        fsr_set_ftt(invalid_fp_register);
        return (FDTOI_CYCLES);
    }
#endif
    va_rs2.data = fpu_get_double(sparc_state, rs2);

    va_rd.data = (int) (va_rs2.value);

#if 0                                   //fix me fo larg positive argument or large negative argument
    /* check the value overflow or underflow */
    if (va_rd.value < va_rs2.value)
    {
        cexc |= IEEE_FOF | IEEE_FDX;
    }                                   //overflow
    else if (va_rd.value > va_rs2.value)
    {
        cexc |= IEEE_FUF | IEEE_FDX;
    }                                   // underlow
    if (swNAN(va_rd.data))
    {
        printf("2\n");
        cexc |= IEEE_FNV;
    }
#endif

    fpu_put_single(sparc_state, va_rd.data, rd);
    if ((ieee_754_exception(sparc_state, cexc)) == 1)
    {
        return (FDTOI_CYCLES);
    }
    //printf("fdtoi f%d, f%d---------------\n", rd, rs2);
    PCREG = NPCREG;
    NPCREG += 4;

    fsr_clear_ftt();
    // Everyting takes some time
    return FDTOI_CYCLES;

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

    if ((instr & FDTOI_CODE_MASK) && (op == OP) && (opf == OPF) && (fmt == FMT))
    {
        rd = bits(instr, RD_OFF_last, RD_OFF_first);
        rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);

        return 1;
    }
    return 0;
}
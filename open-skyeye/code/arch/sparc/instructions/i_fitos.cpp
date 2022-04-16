/*
 * =====================================================================================
 *
 *       Filename:  i_fitos.c
 *
 *    Description:  Implementation of the FSTOS instruction
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
 * opf          = 011000100
 * 
 * 31-30  29-25   24-19     18-14       13-5             4-0
 * +----+-------+--------+------------------------------------------+
 * | 10 |   rd  | 110100 |   NBZ   |     011000100      |   rs2  |
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

#define FITOS_CYCLES    1
#define FITOS_CODE_MASK	0X81A01880
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x2

#define OPF_OFF_first     5
#define OPF_OFF_last      13
#define OPF         0xC4

#define RD_OFF_first      25
#define RD_OFF_last       29

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define FMT_OFF_first	19
#define FMT_OFF_last	24
#define FMT		0x34


sparc_instruction_t i_fitos = { " i_fitos",
    execute,
    disassemble,
    FITOS_CODE_MASK,
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
    int32_t va_rs2;
    s_precision va_rd;
    int cexc = 0;

    /*  Check whether the FPU is enabled or not */
    if (!bit(PSRREG, PSR_EF))
    {
        traps->signal(sparc_state->obj, FP_DISABLED);
        fsr_set_ftt(unimplemented_FPop);
        return (FITOS_CYCLES);
    }
    /*
     * if(rd & 1){
     * traps->signal(sparc_state->obj, FP_EXCEPTION);
     * fsr_set_ftt(invalid_fp_register);
     * return(FITOS_CYCLES);
     * }
     */
    va_rs2 = fpu_get_single(sparc_state, rs2);
    va_rd.value = (float) va_rs2;
    fpu_put_single(sparc_state, va_rd.data, rd);

    //printf("fitos f%d, f%d---------------\n",  rs2, rd);
    PCREG = NPCREG;
    NPCREG += 4;

    //fsr_clear_ftt();
    // Everyting takes some time
    return FITOS_CYCLES;

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

    if ((instr & FITOS_CODE_MASK) && (op == OP) && (opf == OPF) && (fmt == FMT))
    {
        rd = bits(instr, RD_OFF_last, RD_OFF_first);
        rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);

        return 1;
    }
    return 0;
}
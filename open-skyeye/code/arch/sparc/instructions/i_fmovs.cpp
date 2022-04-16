/*
 * =====================================================================================
 *
 *       Filename:  i_fmovs.c
 *
 *    Description:  Implementation of the FMOVS instruction
 *
 *        Version:  1.0
 *        Created:  16/04/08 18:09:22
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
 31-30  29-25     24-19        18-14      13-5       4-0
 +----+-------+-----------+------------+---------+-----------+
 | op |   rd  |    op3    |     rs1    |   opf   |    rs2    |
 +----+-------+-----------+------------+---------+-----------+
 op              = 10
 op3                            = 110100
 rs1                            = 00000
 opf                            = 000000001
 
 31-30  29-25     24-19        18-14         13-5           4-0
 +----+-------+-----------+------------+---------------+-----------+
 | op |   rd  |  110100   |    00000   |   000000001   |    rs2    |
 +----+-------+-----------+------------+---------------+-----------+
 */
#include "common/bits.h"
#include "common/sparc.h"
#include "common/traps.h"
#include "common/iu.h"
#include "common/fpu.h"
#include "i_utils.h"

#include "sparc_instr.h"
#include <stdio.h>

static int execute(sparc_state_t * sparc_state, uint32 instr);
static int disassemble(uint32 instr, void *state);

#define FMOVS_CYCLES    1
#define FMOVS_CODE_MASK 0x81A00020
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x2

#define OPF_OFF_first    5
#define OPF_OFF_last     13
#define OPF         	0x1

#define RD_OFF_first	25
#define RD_OFF_last		29

#define RS2_OFF_first	0
#define RS2_OFF_last	4



sparc_instruction_t i_fmovs = { " i_fmovs",
    execute,
    disassemble,
    FMOVS_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    int rs2, rd;

    rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);
    rd = bits(instr, RD_OFF_last, RD_OFF_first);
    /*
     * va_* are used to save the registers' value
     */
    s_precision va_rs2, va_rd;

    /*  Check whether the FPU is enabled or not */
    if (!bit(PSRREG, PSR_EF))
    {
        traps->signal(sparc_state->obj, FP_DISABLED);
        fsr_set_ftt(unimplemented_FPop);
        return (FMOVS_CYCLES);
    }
    va_rs2.data = fpu_get_single(sparc_state, rs2);
    fpu_put_single(sparc_state, va_rs2.data, rd);
    //fpu_put_single(sparc_state, va_rd.data, rd);

    print_fpinst_RS_RS("fmovs", rs2, rd);
    //      va_rd.data = fpu_get_single(sparc_state, rd);
    PCREG = NPCREG;
    NPCREG += 4;

    return FMOVS_CYCLES;

}

static int disassemble(uint32 instr, void *state)
{
    int opf = 0, op;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    opf = bits(instr, OPF_OFF_last, OPF_OFF_first);

    if ((instr & FMOVS_CODE_MASK) && (op == OP) && (opf == OPF))
    {
        int rs2, rd;

        rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);
        rd = bits(instr, RD_OFF_last, RD_OFF_first);

        return 1;
    }
    return 0;

}
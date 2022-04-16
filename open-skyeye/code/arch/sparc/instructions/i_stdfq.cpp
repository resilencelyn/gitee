/*
 * =====================================================================================
 *
 *       Filename:  i_stdfq.c
 *
 *    Description:  Implementation of the STDF instruction
 *
 *        Version:  1.0
 *        Created:  16/04/08 18:09:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

/*  Format (3)
 * 
 * 31-30  29-25   24-19     18-14    13            12-0
 * +----+-------+--------+------------------------------------------+
 * | op |   rd  |  op3   |   rs1   | i=1 |         simm13           |
 * +----+-------+--------+------------------------------------------+
 * 
 * op              = 11
 * rd              = 0000
 * op3             = 100110
 * 
 * 31-30  29-25   24-19     18-14    13        12-5          4-0
 * +----+-------+--------+------------------------------------------+
 * | 11 | 00000 | 100110 |   rs1   | 1/0 |      simm12/rs2          |
 * +----+-------+--------+------------------------------------------+
 * 
 */
#include "common/bits.h"
#include "common/sparc.h"
#include "common/traps.h"
#include "common/iu.h"
#include "common/memory.h"
#include "common/debug.h"
#include "i_utils.h"

#include "sparc_instr.h"
#include <stdio.h>

static int execute(sparc_state_t * sparc_state, uint32 instr);
static int disassemble(uint32 instr, void *state);

#define STDFQ_CYCLES    1
#define STDF_CODE_MASK 0xc1300000
#define PRIVILEDGE  1

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP               0x3

#define OP3_OFF_first    19
#define OP3_OFF_last     24
#define OP3              0x26

#define RS1_OFF_first     14
#define RS1_OFF_last      18

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define I_OFF       13

#define SIMM13_OFF_first  0
#define SIMM13_OFF_last   12


sparc_instruction_t i_stdfq = { " i_stdfq",
    execute,
    disassemble,
    STDF_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    exception_t status;
    uint32 addr = 0;
    int rs1, rs2, imm;

    rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
    int i = bit(instr, I_OFF);

    if (i)
    {
        imm = bits(instr, SIMM13_OFF_last, SIMM13_OFF_first);
        rs2 = -1;
    } else
    {
        rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);
        imm = -1;
    }

    /*  If the EF field of the PSR register is 0, a store floating-point
     *  instruction causes an fp_disabled_trap
     */
    if (!bit(PSRREG, PSR_EF))
    {
        traps->signal(sparc_state->obj, FP_DISABLED);
        return (STDFQ_CYCLES);
    }
    // Not implemented here, just set FSR.ftt = 4 and trapped
    FPSRREG &= ~0x0001C000;             // clear bits[16:14], which is ftt
    FPSRREG |= 0x00010000;              // set FSR.ftt = 4
    traps->signal(sparc_state->obj, FP_EXCEPTION);
    return (STDFQ_CYCLES);

    PCREG = NPCREG;
    NPCREG += 4;

    // Everyting takes some time
    return (STDFQ_CYCLES);
}

static int disassemble(uint32 instr, void *state)
{
    int op3 = 0, op;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    op3 = bits(instr, OP3_OFF_last, OP3_OFF_first);

    if ((instr & STDF_CODE_MASK) && (op == OP) && (op3 == OP3))
    {
        int rs1, rs2, imm;

        rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
        int i = bit(instr, I_OFF);

        if (i)
        {
            imm = bits(instr, SIMM13_OFF_last, SIMM13_OFF_first);
            rs2 = -1;
        } else
        {
            rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);
            imm = -1;
        }

        return 1;
    }
    return 0;
}

/*
 * =====================================================================================
 *
 *       Filename:  i_sdiv.c
 *
 *    Description:  Implementation of the UDIV instruction
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
 * 31-30  29-25   24-19     18-14    13        12-5          4-0
 * +----+-------+--------+------------------------------------------+
 * | op |   rd  |  op3   |   rs1   | i=0 |   non-used   |    rs2    |
 * +----+-------+--------+------------------------------------------+
 * 
 * 31-30  29-25   24-19     18-14    13            12-0
 * +----+-------+--------+------------------------------------------+
 * | op |   rd  |  op3   |   rs1   | i=1 |         simm13           |
 * +----+-------+--------+------------------------------------------+
 * 
 * op              = 10
 * rd              = 00000
 * op3             = 001110
 * 
 * 31-30  29-25   24-19     18-14    13        12-5          4-0
 * +----+-------+--------+------------------------------------------+
 * | 10 | 00000 | 001110 |   rs1   | 1/0 |      simm13/rs2          |
 * +----+-------+--------+------------------------------------------+
 * 
 */
#include "common/bits.h"
#include "common/sparc.h"
#include "common/traps.h"
#include "common/iu.h"
#include "i_utils.h"

#include "sparc_instr.h"
#include <stdio.h>

static int execute(sparc_state_t * sparc_state, uint32 instr);
static int disassemble(uint32 instr, void *state);

#define SDIV_CYCLES    1
#define SDIV_CODE_MASK 0x80780000
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x2

#define OP3_OFF_first     19
#define OP3_OFF_last      24
#define OP3         0x0f

#define RD_OFF_first      25
#define RD_OFF_last       29

#define RS1_OFF_first     14
#define RS1_OFF_last      18

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define I_OFF       13

#define SIMM13_OFF_first  0
#define SIMM13_OFF_last   12


sparc_instruction_t i_sdiv = { " i_sdiv",
    execute,
    disassemble,
    SDIV_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    int rd, rs1, rs2, imm;

    rd = bits(instr, RD_OFF_last, RD_OFF_first);
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

    int64 z, dividend = 0;

    dividend = YREG;
    dividend = (dividend << 32) | REG(rs1);

    if (imm < 0)
    {
        if (REG(rs2) == 0)
        {
            traps->signal(sparc_state->obj, DIV_BY_ZERO);
            return SDIV_CYCLES;
        } else
        {
            z = dividend / REG(rs2);
            REG(rd) = z;
            print_inst_RS_RS_RD("sdiv", rs1, rs2, rd);
        }
    } else
    {
        if (imm == 0)
        {
            traps->signal(sparc_state->obj, DIV_BY_ZERO);
            return SDIV_CYCLES;
        } else
        {
            z = (dividend / sign_ext13(imm));
            REG(rd) = z;
            print_inst_RS_IM13_RD("sdiv", rs1, sign_ext13(imm), rd);
        }
    }
    /*
     * If overflow occurs, Value returned
     * in rd: 0x7fffffff or 0x80000000
     */
    if ((z >> 31) > 0)
        REG(rd) = 0x7fffffff;
    else if ((z >> 31) < 0)
        REG(rd) = 0x80000000;

    PCREG = NPCREG;
    NPCREG += 4;

    // Everyting takes some time
    return SDIV_CYCLES;

}

static int disassemble(uint32 instr, void *state)
{
    int op3 = 0, op;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    op3 = bits(instr, OP3_OFF_last, OP3_OFF_first);

    if ((instr & SDIV_CODE_MASK) && (op == OP) && (op3 == OP3))
    {
        int rd, rs1, rs2, imm;

        rd = bits(instr, RD_OFF_last, RD_OFF_first);
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

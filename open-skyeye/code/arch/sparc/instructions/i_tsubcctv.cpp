/*
 * =====================================================================================
 *
 *       Filename:  i_tsubcctv.c
 *
 *    Description:  Implementation of the TSUBCCTV instruction
 *
 *              Author:  jiachao@digiproto.com
 *              Company:  Tsinghua skyeye team
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
 * rd              = 0000
 * op3             = 100011
 * 
 * 31-30  29-25   24-19     18-14    13        12-5          4-0
 * +----+-------+--------+------------------------------------------+
 * | 10 | 00000 | 100011 |   rs1   | 1/0 |      simm12/rs2          |
 * +----+-------+--------+------------------------------------------+
 * 
 */
#include "i_utils.h"
#include "common/types.h"
#include "common/bits.h"
#include "common/sparc.h"
#include "common/traps.h"
#include "common/iu.h"

#include "sparc_instr.h"
#include <stdio.h>

static int execute(sparc_state_t * sparc_state, uint32 instr);
static int disassemble(uint32 instr, void *state);

#define TSUBCCTV_CYCLES    1
#define TSUBCCTV_CODE_MASK 0x81200000
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x2

#define OP3_OFF_first     19
#define OP3_OFF_last      24
#define OP3         0x23

#define RD_OFF_first      25
#define RD_OFF_last       29

#define RS1_OFF_first     14
#define RS1_OFF_last      18

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define I_OFF       13

#define SIMM13_OFF_first  0
#define SIMM13_OFF_last   12


sparc_instruction_t i_tsubcctv = { " i_tsubcctv",
    execute,
    disassemble,
    TSUBCCTV_CODE_MASK,
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

    uint32 x, y, z, tmp;
    char Sm, Dm, Rm;

    tmp = REG(rd);
    x = REG(rs1);
    if (imm < 0)
    {
        y = REG(rs2);
        z = REG(rd) = REG(rs1) - REG(rs2);
        print_inst_RS_RS_RD("tsubcctv", rs1, rs2, rd);
    } else
    {
        y = sign_ext13(imm);
        z = REG(rd) = REG(rs1) - sign_ext13(imm);
        print_inst_RS_IM13_RD("tsubcctv", rs1, sign_ext13(imm), rd);
    }

    Sm = bit(x, 31);
    Dm = bit(y, 31);
    Rm = bit(z, 31);
    /*
     * A tag_overflow occurs if bit 1 or bit 0 of either operand is nonzero, or if the
     * addition generates an arithmetic overflow
     */
    if (x & 0x3 || y & 0x3)
    {
        traps->signal(sparc_state->obj, TAG_OVERFLOW);
        REG(rd) = tmp;
        return TSUBCCTV_CYCLES;
    }
    if ((Sm != Dm) && (Sm != Rm))
    {
        traps->signal(sparc_state->obj, TAG_OVERFLOW);
        REG(rd) = tmp;
        return TSUBCCTV_CYCLES;
    }

    clear_icc(sparc_state);
    /*  CARRY condition  */
    if ((!Sm && Dm) || (Rm && (Dm || !Sm)))
    {
        psr_set_carry();
    }

    /*  ZERO condition  */
    if (z == 0)
    {
        psr_set_zero();
    }
    /*  NEGATIVE condition  */
    if (bit(z, 31))
    {
        psr_set_neg();
    }

    PCREG = NPCREG;
    NPCREG += 4;

    // Everyting takes some time
    return TSUBCCTV_CYCLES;
}

static int disassemble(uint32 instr, void *state)
{
    int op3 = 0, op;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    op3 = bits(instr, OP3_OFF_last, OP3_OFF_first);

    if ((instr & TSUBCCTV_CODE_MASK) && (op == OP) && (op3 == OP3))
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

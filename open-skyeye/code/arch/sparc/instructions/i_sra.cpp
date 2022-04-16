/*
 * =====================================================================================
 *
 *       Filename:  i_sra.c
 *
 *    Description:  Implementation of the arithmetic right shift.
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
 * 31-30  29-25   24-19     18-14    13        12-5          4-0
 * +----+-------+--------+------------------------------------------+
 * | op |   rd  |  op3   |   rs1   | i=0 |   non-used   |    rs2    |
 * +----+-------+--------+------------------------------------------+
 * 
 * 31-30  29-25   24-19     18-14    13        12-5          4-0
 * +----+-------+--------+------------------------------------------+
 * | op |   rd  |  op3   |   rs1   | i=1 |   non-used   |  shcnt    |
 * +----+-------+--------+------------------------------------------+
 * 
 * op              = 00
 * rd              = 0000
 * op3             = 100111
 * 
 * 31-30  29-25   24-19     18-14    13        12-5          4-0
 * +----+-------+--------+------------------------------------------+
 * | 10 | 00000 | 100111 |   rs1   |  i  |   non-used   | rs2/shcnt |
 * +----+-------+--------+------------------------------------------+
 * 
 */
#include "common/bits.h"
#include "common/sparc.h"
#include "common/traps.h"
#include "common/iu.h"

#include "sparc_instr.h"
#include <stdio.h>

static int execute(sparc_state_t * sparc_state, uint32 instr);
static int disassemble(uint32 instr, void *state);

#define SRA_CYCLES    1
#define SRA_CODE_MASK   0x81380000
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x2

#define OP3_OFF_first     19
#define OP3_OFF_last      24
#define OP3         0x27

#define RD_OFF_first      25
#define RD_OFF_last       29

#define RS1_OFF_first     14
#define RS1_OFF_last      18

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define I_OFF       13

#define SHCNT_OFF_last    4
#define SHCNT_OFF_first   0


sparc_instruction_t i_sra = { " i_sra",
    execute,
    disassemble,
    SRA_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int32_t Lmask(int32_t a)
{
    int count;

    count = a & 0x1f;
    if (!count)
        return 0;
    return ((unsigned) 0x80000000 >> count - 1) - 1 ^ 0xffffffff;
}

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    int s = 0;
    uint32_t x = 0;
    int rd, rs1, rs2;
    char imm;
    int shift;
    int result;

    rd = bits(instr, RD_OFF_last, RD_OFF_first);
    rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
    int i = bit(instr, I_OFF);

    if (rd == 0)
        goto out;

    if (i)
    {
        imm = bits(instr, SHCNT_OFF_last, SHCNT_OFF_first);
        rs2 = -1;
    } else
    {
        rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);
        imm = -1;
    }

    if (imm < 0)
    {
        shift = REG(rs2) & 0x1f;
        print_inst_RS_RS_RD("sra", rs1, rs2, rd);
    } else
    {
        shift = imm;
        print_inst_RS_IM13_RD("sra", rs1, imm, rd);
    }

    result = REG(rs1) >> shift;
    if ((REG(rs1) >> 31) & 0x1)
    {
        result |= Lmask(shift);
    } else
    {
        result &= ~Lmask(shift);
    }

    REG(rd) = result;

    /*  The host platform does logical shift, so we need to manually patch the
     *  higer bits  */
    //   x = REG(rd);
    //      if (s < 32)
    //REG(rd) = sign_extend_uint32(x, 32 - s);
    //      printf("REG rd :%x, s:%d\n", REG(rd), s);
    //    uint32 x = pstate->regs[cwp].r[rd];
    //    pstate->regs[cwp].r[rd] = sign_extend_uint32(x, 32 - s);

  out:
    PCREG = NPCREG;
    NPCREG += 4;

    // Everyting takes some time
    return (SRA_CYCLES);
}

static int disassemble(uint32 instr, void *state)
{
    int op3 = 0, op;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    op3 = bits(instr, OP3_OFF_last, OP3_OFF_first);

    if ((instr & SRA_CODE_MASK) && (op == OP) && (op3 == OP3))
    {
        int rd, rs1, rs2;
        char imm;

        rd = bits(instr, RD_OFF_last, RD_OFF_first);
        rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
        int i = bit(instr, I_OFF);

        if (i)
        {
            imm = bits(instr, SHCNT_OFF_last, SHCNT_OFF_first);
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

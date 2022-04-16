/*
 * =====================================================================================
 *
 *       Filename:  i_ldf.c
 *
 *    Description:  Implementation of the LDF instruction
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
 * op3             = 100000
 * 
 * 31-30  29-25   24-19     18-14    13        12-5          4-0
 * +----+-------+--------+------------------------------------------+
 * | 11 | 00000 | 100000 |   rs1   | 1/0 |      simm12/rs2          |
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

#define LDF_CYCLES    1
#define LDF_CODE_MASK 0xc1000000
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x3

#define OP3_OFF_first     19
#define OP3_OFF_last      24
#define OP3         0x20

#define RD_OFF_first      25
#define RD_OFF_last       29

#define RS1_OFF_first     14
#define RS1_OFF_last      18

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define I_OFF       13

#define SIMM13_OFF_first  0
#define SIMM13_OFF_last   12


sparc_instruction_t i_ldf = { " i_ldf",
    execute,
    disassemble,
    LDF_CODE_MASK,
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

    int status;
    uint32 addr;

    /*  Check whether the FPU is enabled or not */
    if (!bit(PSRREG, PSR_EF))
    {
        traps->signal(sparc_state->obj, FP_DISABLED);
        return (LDF_CYCLES);
    }

    if (imm < 0)
    {
        addr = REG(rs1) + REG(rs2);
        DBG("ldf [ reg[%d] + reg[%d] ], f[%d]\n", rs1, rs2, rd);
        //        print_inst_LS_RS_IM13_RD("ldd", rs1, sign_ext13(imm), rd);
    } else
    {
        addr = REG(rs1) + sign_ext13(imm);
        DBG("ldf [ reg[%d] + %d ], f[%d]\n", rs1, sign_ext13(imm), rd);
        //        print_inst_LS_RS_IM13_RD("ldd", rs1, sign_ext13(imm), rd);
    }

    /*  Check the word alignment. The address MUST be WORD aligned. That means
     *  that the last 2 bits of the address must be zero    */
    if (addr & 0x3)
    {
        traps->signal(sparc_state->obj, MEM_ADDR_NOT_ALIGNED);
        return (LDF_CYCLES);
    }

    uint32 value = 0;
    conf_object_t *conf_obj = sparc_state->obj;

    status = sparc_memory_read_word32(conf_obj, &value, addr);
    FPREG(rd) = value;

    if (status != No_exp)
    {
        traps->signal(conf_obj, get_sparc_exp(sparc_state));
        return (LDF_CYCLES);
    }

    PCREG = NPCREG;
    NPCREG += 4;

    // Everyting takes some time
    return (LDF_CYCLES);
}

static int disassemble(uint32 instr, void *state)
{
    int op3 = 0, op;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    op3 = bits(instr, OP3_OFF_last, OP3_OFF_first);

    if ((instr & LDF_CODE_MASK) && (op == OP) && (op3 == OP3))
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

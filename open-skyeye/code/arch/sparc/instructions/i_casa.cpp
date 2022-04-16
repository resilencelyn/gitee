/*
 * =====================================================================================
 *
 *       Filename:  i_casa.c
 *
 *    Description:  Implementation of the CASA(sparc V9) instruction
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
 * 31-30  29-25   24-19     18-14    13            12-5      4-0
 * +----+-------+--------+------------------------------------------+
 * | op |   rd  |  op3   |   rs1   | i=1 |         imm_asi  |  rs2  |
 * +----+-------+--------+------------------------------------------+
 * 
 * op              = 11
 * rd              = 0000
 * op3             = 111100
 * 
 * 31-30  29-25   24-19     18-14    13        12-5          4-0
 * +----+-------+--------+------------------------------------------+
 * | 11 | 00000 | 000000 |   rs1   | 1/0 |      simm12/rs2          |
 * +----+-------+--------+------------------------------------------+
 * 
 */
#include "common/bits.h"
#include "common/sparc.h"
#include "common/traps.h"
#include "common/iu.h"
#include "common/memory.h"
#include "i_utils.h"
#include "sparc_instr.h"

#include <stdio.h>

static int execute(sparc_state_t * sparc_state, uint32 instr);
static int disassemble(uint32 instr, void *state);

#define CASA_CYCLES    1
#define CASA_CODE_MASK 0xc0000000
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x3

#define OP3_OFF_first     19
#define OP3_OFF_last      24
#define OP3         0x3c

#define RD_OFF_first      25
#define RD_OFF_last       29

#define RS1_OFF_first     14
#define RS1_OFF_last      18

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define I_OFF       13

#define IMM_ASI_OFF_first  5
#define IMM_ASI_OFF_last   12



sparc_instruction_t i_casa = { " i_casa",
    execute,
    disassemble,
    CASA_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    int rd, rs1, rs2, imm_asi, addr, rs2_value;
    uint32 value = 0;
    exception_t status;
    conf_object_t *conf_obj = sparc_state->obj;

    rd = bits(instr, RD_OFF_last, RD_OFF_first);
    rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
    rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);
    int i = bit(instr, I_OFF);

    /*Compare and Swap Word from Alternate space*/
    if (i == 0)
    {
        imm_asi = bits(instr, IMM_ASI_OFF_last, IMM_ASI_OFF_first);
    }

    rs2_value = REG(rs2);
    addr = REG(rs1);
    /*  Check the word alignment. The address MUST be WORD aligned. That means
     *  that the last 2 bits of the address must be zero    */
    if (addr & 0x3)
    {
        traps->signal(sparc_state->obj, MEM_ADDR_NOT_ALIGNED);
        return (CASA_CYCLES);
    }

    status = sparc_memory_read_word32(conf_obj, &value, addr);

    if (status != No_exp)
    {
        traps->signal(conf_obj, get_sparc_exp(sparc_state));
        return (CASA_CYCLES);
    }

    /*compares the low-order 32 bits of r[rs2] with
     * a word in memory pointed to by the word address in r[rs1]*/
    if (value == rs2_value)
    {
        //swap
        status = sparc_memory_store_word32(conf_obj, addr, REG(rd));
        if (status != No_exp)
        {
            traps->signal(conf_obj, get_sparc_exp(sparc_state));
            return (CASA_CYCLES);
        }
        REG(rd) = value;
    } else
    {
        REG(rd) = value;
    }

    PCREG = NPCREG;
    NPCREG += 4;

    // Everyting takes some time
    return (CASA_CYCLES);
}

static int disassemble(uint32 instr, void *state)
{
    int op3 = 0, op;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    op3 = bits(instr, OP3_OFF_last, OP3_OFF_first);

    if ((instr & CASA_CODE_MASK) && (op == OP) && (op3 == OP3))
    {
        return 1;
    }

    return 0;
}

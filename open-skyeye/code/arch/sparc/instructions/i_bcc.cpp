/*
 * =====================================================================================
 *
 *       Filename:  i_bcc.c
 *
 *    Description:  Implementation of the BLEU instruction
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

/*  Format (2)
 * 
 * 31-30  29   28-25    24-22       21-0
 * +----+----+--------+------------------------------------------+
 * | op | a  |  cond  |   010   |          disp22                |
 * +----+----+--------+------------------------------------------+
 * 
 * op              = 00
 * cond            = 1101
 * 
 * 31-30  29   28-25    24-22       21-0
 * +----+----+--------+------------------------------------------+
 * | op | a  |  1101  |   010   |          disp22                |
 * +----+----+--------+------------------------------------------+
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

#define BCC_CYCLES    1
#define BCC_CODE_MASK 0x1a800000
#define PRIVILEDGE  0

#define OP_MASK    0xc0000000
#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x0

#define COND_OFF_first     25
#define COND_OFF_last      28
#define COND         0xd

#define A_OFF       29

#define DISP22_OFF_first  0
#define DISP22_OFF_last   21

#define FMT_OFF_first	22
#define FMT_OFF_last	24
#define FMT		0x2



sparc_instruction_t i_bcc = { " i_bcc",
    execute,
    disassemble,
    BCC_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    int cond, disp22, annul, op;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    annul = bit(instr, A_OFF);
    cond = bits(instr, COND_OFF_last, COND_OFF_first);
    disp22 = bits(instr, DISP22_OFF_last, DISP22_OFF_first);

    int32 addr = (disp22 << 10) >> 8;   /* sign extend  */
    uint32 c;

    c = psr_get_carry();

    /*  Branch if not C */
    if (c)
    {
        if (annul)
        {
            /*  the delay slot is annulled  */
            PCREG = NPCREG + 4;
            NPCREG += 8;
        } else
        {
            /*  the delay slot is executed  */
            PCREG = NPCREG;
            NPCREG += 4;
        }
    } else
    {
        addr += PCREG;
        PCREG = NPCREG;
        NPCREG = addr;
    }

    //    DBG("bcc 0x%x\n", addr);
    print_inst_BICC("bcc", addr);

    // Everyting takes some time
    return (BCC_CYCLES);

}

static int disassemble(uint32 instr, void *state)
{

    int cond, disp22, annul, op, fmt;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    annul = bit(instr, A_OFF);
    cond = bits(instr, COND_OFF_last, COND_OFF_first);
    disp22 = bits(instr, DISP22_OFF_last, DISP22_OFF_first);
    fmt = bits(instr, FMT_OFF_last, FMT_OFF_first);

    if (((instr & BCC_CODE_MASK) == BCC_CODE_MASK) && (op == OP) && (cond == COND) && (fmt == FMT))
    {
        disp22 = bits(instr, DISP22_OFF_last, DISP22_OFF_first);
        return 1;
    }
    return 0;
}


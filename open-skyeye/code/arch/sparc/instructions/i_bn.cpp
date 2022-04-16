/*
 * =====================================================================================
 *
 *       Filename:  i_bn.c
 *
 *    Description:  Implementation of the BN instruction
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
 * cond            = 0000
 * 
 * 31-30  29   28-25    24-22       21-0
 * +----+----+--------+------------------------------------------+
 * | op | a  |  0000  |   010   |          disp22                |
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

#define BN_CYCLES    1
#define BN_CODE_MASK 0x0080000
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x0

#define COND_OFF_first     25
#define COND_OFF_last      28
#define COND         0x0

#define A_OFF       29

#define DISP22_MASK 0x003fffff
#define DISP22_OFF  0

#define FMT_OFF_first	22
#define FMT_OFF_last	24
#define FMT		0x2


sparc_instruction_t i_bn = { " i_bn",
    execute,
    disassemble,
    BN_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    int cond, disp22, annul, op;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    annul = bit(instr, A_OFF);
    cond = bits(instr, COND_OFF_last, COND_OFF_first);
    disp22 = (instr & DISP22_MASK) >> DISP22_OFF;

    int32 addr = (disp22 << 10) >> 8;   /*  sign extend */

    if (annul)
    {
        /*  the delay slot is annulled  */
        PCREG = NPCREG + 4;
        NPCREG += 8;

        //        DBG("bn,a 0x%x\n", addr);
        print_inst_BICC("bn,a", addr);

    } else
    {
        //        DBG("bn 0x%x\n", addr);
        print_inst_BICC("bn", addr);
        PCREG = NPCREG;
        NPCREG += 4;
    }

    // Everyting takes some time
    return (BN_CYCLES);

}

static int disassemble(uint32 instr, void *state)
{
    int cond, disp22, annul, op, fmt;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    annul = bit(instr, A_OFF);
    cond = bits(instr, COND_OFF_last, COND_OFF_first);
    disp22 = (instr & DISP22_MASK) >> DISP22_OFF;
    fmt = bits(instr, FMT_OFF_last, FMT_OFF_first);

    if (((instr & BN_CODE_MASK) == BN_CODE_MASK) && (op == OP) && (cond == COND) && (fmt == FMT))
    {
        disp22 = (instr & DISP22_MASK) >> DISP22_OFF;
        return 1;
    }
    return 0;
}

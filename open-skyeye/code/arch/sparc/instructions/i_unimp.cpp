/*
 * =====================================================================================
 *
 *       Filename:  i_unimp.c
 *
 *    Description:  
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
 * 31-30  29-25   24-22
 * +----+-------+-------+---------------------------------------------------+
 * | op |   rd  |  op2  |              imm22/disp22                         |
 * +----+-------+-------+---------------------------------------------------+
 * 
 * op              = 00
 * rd              = 0000
 * op2             = 000
 * imm22/disp22    = 0
 * 
 * +----+-------+-------+---------------------------------------------------+
 * | 00 | 0000  |  000  |           --------- 0 --------                    |
 * +----+-------+-------+---------------------------------------------------+
 * 
 */
#include "common/sparc.h"
#include "common/traps.h"
#include "common/debug.h"
#include "common/iu.h"
#include "i_utils.h"

#include "sparc_instr.h"
#include <stdio.h>

static int execute(sparc_state_t * sparc_state, uint32 instr);
static int disassemble(uint32 instr, void *state);

#define UNIMP_CYCLES    1
#define UNIMP_CODE_MASK 0x2000000
#define PRIVILEDGE  0

#define OP  0x0

sparc_instruction_t i_unimp = { " i_unimp",
    execute,
    disassemble,
    UNIMP_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    DBG("unimp\n");

    traps->signal(sparc_state->obj, ILLEGAL_INSTR);

    // Everyting takes some time
    return UNIMP_CYCLES;
}

static int disassemble(uint32 instr, void *state)
{
    if ((instr | UNIMP_CODE_MASK) == UNIMP_CODE_MASK)
        return 1;
    else
        return 0;
}

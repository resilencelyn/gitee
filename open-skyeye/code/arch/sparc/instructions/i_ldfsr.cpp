/*
 * =====================================================================================
 *
 *       Filename:  i_ldfsr.c
 *
 *    Description:  Implementation of the LDFSR instruction
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
 * op3             = 100001
 * 
 * 31-30  29-25   24-19     18-14    13        12-5          4-0
 * +----+-------+--------+------------------------------------------+
 * | 11 | 00000 | 100001 |   rs1   | 1/0 |      simm12/rs2          |
 * +----+-------+--------+------------------------------------------+
 * 
 */
#include "common/bits.h"
#include "common/sparc.h"
#include "common/traps.h"
#include "common/iu.h"
#include "common/fpu.h"
#include "common/memory.h"
#include "common/debug.h"
#include "i_utils.h"

#include "sparc_instr.h"
#include <stdio.h>

static int execute(sparc_state_t * sparc_state, uint32 instr);
static int disassemble(uint32 instr, void *state);

#define LDFSR_CYCLES    1
#define LDFSR_CODE_MASK 0xc1010000
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x3

#define OP3_OFF_first     19
#define OP3_OFF_last      24
#define OP3         0x21

#define RD_OFF_first      25
#define RD_OFF_last       29

#define RS1_OFF_first     14
#define RS1_OFF_last      18

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define I_OFF       13

#define SIMM13_OFF_first  0
#define SIMM13_OFF_last   12

sparc_instruction_t i_ldfsr = { " i_ldfsr",
    execute,
    disassemble,
    LDFSR_CODE_MASK,
    PRIVILEDGE,
    OP
};

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    int status;
    uint32 addr;
    uint32 mask = 0xffffffff;
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

    /*  Check whether the FPU is enabled or not */
    if (!bit(PSRREG, PSR_EF))
    {
        traps->signal(sparc_state->obj, FP_DISABLED);
        return (LDFSR_CYCLES);
    }

    if (imm < 0)
    {
        addr = REG(rs1) + REG(rs2);
        DBG("ldfsr [ reg[%d] + reg[%d] ], f[%d]\n", rs1, rs2, rd);
    } else
    {
        addr = REG(rs1) + sign_ext13(imm);
        DBG("ldfsr [ reg[%d] + %d ], f[%d]\n", rs1, sign_ext13(imm), rd);
    }

    /*  Check the word alignment. The address MUST be WORD aligned. That means
     *  that the last 2 bits of the address must be zero    */
    if (addr & 0x3)
    {
        traps->signal(sparc_state->obj, MEM_ADDR_NOT_ALIGNED);
        return (LDFSR_CYCLES);
    }

    uint32 value = 0;
    conf_object_t *conf_obj = sparc_state->obj;

    status = sparc_memory_read_word32(conf_obj, &value, addr);
    /*
     * We can't set the follow bits of fsr
     * If you want to correct the definition of fpu,
     * you need correct the bits that are cleared"
     */
    clear_bits(mask, FSR_res_last, FSR_res_first);
    clear_bits(mask, FSR_ver_last, FSR_ver_first);
    clear_bits(mask, FSR_ftt_last, FSR_ftt_first);
    clear_bit(mask, FSR_qne);
    clear_bit(mask, FSR_NS);
    clear_bits(mask, 29, 28);
    clear_bit(mask, 12);

    value &= mask;
    FPSRREG &= ~mask;

    FPSRREG |= value;

    if (status != No_exp)
    {
        traps->signal(sparc_state->obj, DATA_ACCESS_EXCEPTION);
        return (LDFSR_CYCLES);
    }

    PCREG = NPCREG;
    NPCREG += 4;

    // Everyting takes some time
    return (LDFSR_CYCLES);
}

static int disassemble(uint32 instr, void *state)
{
    int op3 = 0, op;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    op3 = bits(instr, OP3_OFF_last, OP3_OFF_first);

    if ((instr & LDFSR_CODE_MASK) && (op == OP) && (op3 == OP3))
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

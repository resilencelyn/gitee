/*
 * =====================================================================================
 *
 *       Filename:  i_lduha.c
 *
 *    Description:  Implementation of the LDA instruction
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
 * op3             = 010000
 *
 * 31-30  29-25   24-19     18-14    13        12-5          4-0
 * +----+-------+--------+------------------------------------------+
 * | 11 | 00000 | 010000 |   rs1   | 1/0 |      simm12/rs2          |
 * +----+-------+--------+------------------------------------------+
 *
 */
#include "common/bits.h"
#include "common/sparc.h"
#include "common/traps.h"
#include "common/debug.h"
#include "common/iu.h"
#include "i_utils.h"
#include "common/memory.h"
#include "sparc_instr.h"

#include <stdio.h>

static int execute(sparc_state_t * sparc_state, uint32 instr);
static int disassemble(uint32 instr, void *state);

#define LDUHA_CYCLES    1
#define LDUHA_CODE_MASK 0xc0900000
#define PRIVILEDGE  1

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x3

#define OP3_OFF_first     19
#define OP3_OFF_last      24
#define OP3         0x12

#define RD_OFF_first      25
#define RD_OFF_last       29

#define RS1_OFF_first     14
#define RS1_OFF_last      18

#define RS2_OFF_first     0
#define RS2_OFF_last      4

#define I_OFF       13

#define SIMM13_OFF_first  0
#define SIMM13_OFF_last   12

#define ASI_OFF_first     5
#define ASI_OFF_last      12


sparc_instruction_t i_lduha = { " i_lduha",
    execute,
    disassemble,
    LDUHA_CODE_MASK,
    PRIVILEDGE,
    OP
};

uint32_t l1cache_reg_read1(sparc_state_t *sparc_state, uint32_t addr)
{
    uint32_t value = 0;
    switch (addr)
    {
        case 0x0:
            value = sparc_state->l1cache_reg.cache_ctrl_reg;
            break;
        case 0x8:
            value = sparc_state->l1cache_reg.icache_cfg_reg;
            break;
        case 0xc:
            value = sparc_state->l1cache_reg.dcache_cfg_reg;
            break;
        default:
            SPARC_LOG(2, "l1cache reg offset:%x not define!\n", addr);
            break;
    }
    return value;
}

uint32_t asi_read_halfword_access_process(sparc_state_t * sparc_state, int asi, uint32_t addr)
{
    conf_object_t *conf_obj = sparc_state->obj;
    uint32_t value = 0;

    switch (asi)
    {
        case 0x1:
            sparc_memory_read_word16(conf_obj, &value, addr);
            break;
        case 0x2:
            //l1cache reg read
            value = l1cache_reg_read1(sparc_state, addr);
            break;
        case 0x3:
            SPARC_LOG(2, "MMU flush/probe\n");
            break;
        case 0x5:
            sparc_memory_read_word16(conf_obj, &value, addr);
            break;
        case 0x6:
            sparc_memory_read_word16(conf_obj, &value, addr);
            break;
        case 0xc:
            SPARC_LOG(2, "instruction catch tag\n");
            break;
        case 0xd:
            SPARC_LOG(2, "instruction catch data\n");
            break;
        case 0xe:
            SPARC_LOG(2, "data catch tag\n");
            break;
        case 0xf:
            SPARC_LOG(2, "data catch data\n");
            break;
        case 0x10:
            SPARC_LOG(2, "Flush page\n");
            break;
        case 0x13:
            SPARC_LOG(2, "Flush context\n");
            break;
        case 0x18:
            sparc_memory_read_word16(conf_obj, &value, addr);
            break;
        case 0x19:
            sparc_read_mmu_regs(conf_obj, &value, addr);
            break;
        case 0x1c:
            //mmu bypass
            {
                memory_space_intf *iface = sparc_state->sparc_memory_iface;
                iface->read(conf_obj, addr, &value, 2);
                value &= 0xffff;
                //printf("lduha at PC = %x read addr = %x value = %x\n", PCREG, addr, value);
            }
            break;
        case 0x20:
            SPARC_LOG(2, "MMU bypass\n");
            break;
        default:
            SPARC_LOG(2, "ASI other operations.ASI = 0x%x at PC = 0x%x\n", asi, sparc_state->pc);
            break;
    }

    return value;
}

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    int status;
    uint32_t addr;
    uint32_t value;
    int rd, rs1, rs2, imm, asi;

    rd = bits(instr, RD_OFF_last, RD_OFF_first);
    rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
    int i = bit(instr, I_OFF);

    if (i)
    {
        traps->signal(sparc_state->obj, ILLEGAL_INSTR);
    } else
    {
        rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);
        asi = bits(instr, ASI_OFF_last, ASI_OFF_first);
        imm = -1;
    }

    if (imm < 0)
        addr = REG(rs1) + REG(rs2);
    else
        addr = REG(rs1) + (int) sign_ext13(imm);

    /*  Check the word alignment. The address MUST be WORD aligned. That means
     *  that the last 2 bits of the address must be zero    */
    if (addr & 0x1)
    {
        traps->signal(sparc_state->obj, MEM_ADDR_NOT_ALIGNED);
        return (LDUHA_CYCLES);
    }
    //    DBG("st reg[%d], [ 0x%x ]\n", rd, addr);
    print_inst_RD_IMM("lduha", rd, addr);
    conf_object_t *conf_obj = sparc_state->obj;

    value = asi_read_halfword_access_process(sparc_state, asi, addr);
    /*
     * if( status < 0 )
     * {
     * traps->signal(DATA_STORE_ERROR);
     * return(LDA_CYCLES);
     * }
     */
    REG(rd) = value;

    PCREG = NPCREG;
    NPCREG += 4;

    // Everyting takes some time
    return (LDUHA_CYCLES);
}

static int disassemble(uint32 instr, void *state)
{
    int op3 = 0, op, i;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    op3 = bits(instr, OP3_OFF_last, OP3_OFF_first);

    if ((instr & LDUHA_CODE_MASK) && (op == OP) && (op3 == OP3))
    {
        op3 = bits(instr, OP3_OFF_last, OP3_OFF_first);
        if (op3 != OP3)
            return 0;

        int rd, rs1, rs2, imm, asi;

        rd = bits(instr, RD_OFF_last, RD_OFF_first);
        rs1 = bits(instr, RS1_OFF_last, RS1_OFF_first);
        i = bit(instr, I_OFF);

        if (i)
        {
            // traps->signal(state->obj, ILLEGAL_INSTR);
        } else
        {
            rs2 = bits(instr, RS2_OFF_last, RS2_OFF_first);
            asi = bits(instr, ASI_OFF_last, ASI_OFF_first);
            imm = -1;
        }

        return 1;
    }
    return 0;
}

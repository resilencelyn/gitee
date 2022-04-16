/*
 * =====================================================================================
 *
 *       Filename:  i_sta.c
 *
 *    Description:  Implementation of the STA instruction
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
 * op3             = 010100
 * 
 * 31-30  29-25   24-19     18-14    13        12-5          4-0
 * +----+-------+--------+------------------------------------------+
 * | 11 | 00000 | 010100 |   rs1   | 1/0 |      simm12/rs2          |
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

#define STA_CYCLES    1
#define STA_CODE_MASK 0xc0a00000
#define PRIVILEDGE  0

#define OP_OFF_first     30
#define OP_OFF_last      31
#define OP         0x3

#define OP3_OFF_first     19
#define OP3_OFF_last      24
#define OP3         0x14

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

sparc_instruction_t i_sta = { " i_sta",
    execute,
    disassemble,
    STA_CODE_MASK,
    PRIVILEDGE,
    OP
};

void l1cache_reg_write(sparc_state_t *sparc_state, uint32_t value, uint32_t addr)
{
    switch (addr)
    {
        case 0x0:
            sparc_state->l1cache_reg.cache_ctrl_reg = value;
            break;
        case 0x8:
            sparc_state->l1cache_reg.icache_cfg_reg = value;
            break;
        case 0xc:
            sparc_state->l1cache_reg.dcache_cfg_reg = value;
            break;
        default:
            SPARC_LOG(2, "l1cache reg offset:%x not define!\n", addr);
            break;
    }
}

void asi_write_access_process(sparc_state_t * sparc_state, int asi, uint32_t addr, uint32_t data)
{
    conf_object_t *conf_obj = sparc_state->obj;
    switch (asi)
    {
        case 0x1:
            sparc_memory_store_word32(conf_obj, addr, data);
            break;
        case 0x2:
            //l1cache reg write
            l1cache_reg_write(sparc_state, data, addr);
            break;
        case 0x3:
            printf("MMU flush/probe\n");
            break;
        case 0x5:
            sparc_memory_store_word32(conf_obj, addr, data);
            break;
        case 0x6:
            sparc_memory_store_word32(conf_obj, addr, data);
            break;
        case 0xc:
            printf("instruction catch tag\n");
            break;
        case 0xd:
            printf("instruction catch data\n");
            break;
        case 0xe:
            printf("data catch tag\n");
            break;
        case 0xf:
            printf("data catch data\n");
            break;
        case 0x10:
            // printf("Flush page\n");
            break;
        case 0x13:
            printf("Flush context\n");
            break;
        case 0x18:
            sparc_memory_store_word32(conf_obj, addr, data);
            break;
        case 0x19:
            sparc_write_mmu_regs(conf_obj, data, addr);
            break;
        case 0x1c:
            //mmu bypass
            {
                memory_space_intf *iface = sparc_state->sparc_memory_iface;
                iface->write(conf_obj, addr, &data, 4);
            }
            break;
        case 0x20:
            printf("MMU bypass\n");
            break;
        default:
            // SPARC_LOG(2, "ASI other operations.ASI = 0x%x at PC = 0x%x\n", asi, sparc_state->pc);
            break;
    }
}

static int execute(sparc_state_t * sparc_state, uint32 instr)
{
    int status;
    uint32 addr;
    int rd, rs1, rs2, imm, asi;

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
        asi = bits(instr, ASI_OFF_last, ASI_OFF_first);
        imm = -1;
    }

    if (imm < 0)
        addr = REG(rs1) + REG(rs2);
    else
        addr = REG(rs1) + (int) sign_ext13(imm);

    /*  Check the word alignment. The address MUSTA be WORD aligned. That means
     *  that the last 2 bits of the address must be zero    */
    if (addr & 0x3)
    {
        traps->signal(sparc_state->obj, MEM_ADDR_NOT_ALIGNED);
        return (STA_CYCLES);
    }
    //    DBG("st reg[%d], [ 0x%x ]\n", rd, addr);
    print_inst_RD_IMM("sta", rd, addr);
    conf_object_t *conf_obj = sparc_state->obj;

    asi_write_access_process(sparc_state, asi, addr, REG(rd));
    /*
     * if( status < 0 )
     * {
     * traps->signal(DATA_STORE_ERROR);
     * return(STA_CYCLES);
     * }
     */

    PCREG = NPCREG;
    NPCREG += 4;

    // Everyting takes some time
    return (STA_CYCLES);
}

static int disassemble(uint32 instr, void *state)
{
    int op3 = 0, op, i;

    op = bits(instr, OP_OFF_last, OP_OFF_first);
    op3 = bits(instr, OP3_OFF_last, OP3_OFF_first);

    if ((instr & STA_CODE_MASK) && (op == OP) && (op3 == OP3))
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
            imm = bits(instr, SIMM13_OFF_last, SIMM13_OFF_first);
            rs2 = -1;
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

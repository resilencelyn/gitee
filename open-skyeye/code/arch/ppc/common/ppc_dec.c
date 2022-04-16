/*
 *      PearPC
 *      ppc_dec.cc
 *
 *      Copyright (C) 2003, 2004 Sebastian Biallas (sb@biallas.net)
 *      Portions Copyright (C) 2004 Daniel Foesch (dfoesch@cs.nmsu.edu)
 *      Portions Copyright (C) 2004 Apple Computer, Inc.
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License version 2 as
 *      published by the Free Software Foundation.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
 * #include "cstring"
 *
 * #include "system/types.h"
 * #include "cpu/debug.h"
 * #include "cpu/cpu.h"
 */

#include <stdio.h>
#include <string.h>

#include "types.h"
#include "tracers.h"
#include "ppc_alu.h"
#include "ppc_cpu.h"
#include "ppc_dec.h"
#include "ppc_exc.h"
#include "ppc_fpu.h"
#include "ppc_vec.h"
#include "ppc_mmu.h"
#include "ppc_opc.h"
#include "ppc_spe.h"
#include "skyeye.h"
#include "ppc_dec_vle.h"
#include "ppc_opc_vle.h"
#include "ppc_alu_vle.h"
#include "ppc_mmu_vle.h"

static void ppc_opc_group_v(e500_core_t *current_core);

static void ppc_opc_invalid(e500_core_t *current_core)
{
    /* FIXME by Michael.Kang
     * if (current_core->pc == gPromOSIEntry && current_core->current_opc == PROM_MAGIC_OPCODE) {
     * call_prom_osi();
     * return;
     * } */
    if (current_core->current_opc == 0x00333301)
    {
        // memset(r3, r4, r5)
        uint32 dest = current_core->gpr[3];
        uint32 c = current_core->gpr[4];
        uint32 size = current_core->gpr[5];

        if (dest & 0xfff)
        {
            byte *dst;

            ppc_direct_effective_memory_handle(current_core, dest, dst);
            uint32 a = 4096 - (dest & 0xfff);

            memset(dst, c, a);
            size -= a;
            dest += a;
        }
        while (size >= 4096)
        {
            byte *dst;

            ppc_direct_effective_memory_handle(current_core, dest, dst);
            memset(dst, c, 4096);
            dest += 4096;
            size -= 4096;
        }
        if (size)
        {
            byte *dst;

            ppc_direct_effective_memory_handle(current_core, dest, dst);
            memset(dst, c, size);
        }
        current_core->pc = current_core->npc;
        return;
    }
    if (current_core->current_opc == 0x00333302)
    {
        // memcpy
        uint32 dest = current_core->gpr[3];
        uint32 src = current_core->gpr[4];
        uint32 size = current_core->gpr[5];
        byte *d, *s;

        ppc_direct_effective_memory_handle(current_core, dest, d);
        ppc_direct_effective_memory_handle(current_core, src, s);
        while (size--)
        {
            if (!(dest & 0xfff))
                ppc_direct_effective_memory_handle(current_core, dest, d);
            if (!(src & 0xfff))
                ppc_direct_effective_memory_handle(current_core, src, s);
            *d = *s;
            src++;
            dest++;
            d++;
            s++;
        }
        current_core->pc = current_core->npc;
        return;
    }

    if ((current_core->current_opc & 0xff) == 0xce)
    {
        /*lvx instruction */
        ppc_opc_lvx(current_core);
        return;
    }

    if (current_core->current_opc == 0x10000644)
    {
        /*mtvscr instruction */
        ppc_opc_mtvscr;
        return;
    }

    if ((current_core->current_opc & 0x7ff) == 0x339)
    {
        /*evstwwe */
        ppc_opc_evstwwe(current_core);
        return;
    }

    if ((current_core->current_opc & 0x7ff) == 0x216)
    {
        /*evxor */
        ppc_opc_evxor(current_core);
        return;
    }

    if ((current_core->current_opc & 0x7ff) == 0x558)
    {
        /*evxor */
        ppc_opc_evmwumiaa(current_core);
        return;
    }

    fprintf(stderr, "Current CoreName = %s PC = %x instr = %x\n", current_core->obj->objname, current_core->pc, current_core->current_opc);
    fprintf(stderr, "[PPC/DEC] Bad opcode: %08x (%u:%u)\n",
            current_core->current_opc, PPC_OPC_MAIN(current_core->current_opc), PPC_OPC_EXT(current_core->current_opc));

    fprintf(stderr, "pc=0x%x\n", current_core->pc);
    skyeye_exit(-1);
    //SINGLESTEP("unknown instruction\n");
}

/********************************VLE**********************************************/
typedef enum
{
    RETURN_FUNC_PTR = 0,
    EXCUTE_FUNC = 1
}VleSeInstrFuncAnalyExcuteType;

const uint8_t VLE_SeInsn_prefix4bit[11] = {0x0, 0x2, 0x4, 0x6, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE};
ppc_opc_function ppc_vle_opc_table_group2[1015];

uint8_t InsnBitsNumJudge(uint32_t instr)
{
    uint8_t CurrentInsnPrefix4bit,i;
    uint8_t Ret;

    CurrentInsnPrefix4bit = (uint8_t)(instr >> 28);
    for(i=0; i<11; i++)
    {
        if(CurrentInsnPrefix4bit == VLE_SeInsn_prefix4bit[i])
        {
            return 16;
        }
    }

    return 32;
}

/*vle instruction:0-5 opcode is 6*/
static void ppc_vle_opc_group_f1(e500_core_t *current_core)
{
    uint32 ext;

    ext = bits(current_core->current_opc,19,16);
    if(ext == 8)
    {
        ppc_vle_opc_addi(current_core);
        return;
    }

    ext = PPC_VLE_OPC_EXT(current_core->current_opc);
    switch(ext & 0x1f)
    {
        case 0:
            {
                uint32 ext2 = PPC_VLE_OPC_EXT2(current_core->current_opc);
                switch(ext2)
                {
                    case 0:
                        ppc_vle_opc_lbzu(current_core);
                        break;
                    case 1:
                        ppc_vle_opc_lhzu(current_core);
                        break;
                    case 2:
                        ppc_vle_opc_lwzu(current_core);
                        break;
                    case 3:
                        ppc_vle_opc_lhau(current_core);
                        break;
                    case 4:
                        ppc_vle_opc_stbu(current_core);
                        break;
                    case 5:
                        ppc_vle_opc_sthu(current_core);
                        break;
                    case 6:
                        ppc_vle_opc_stwu(current_core);
                        break;
                }

                return;
            }
        case 1:
            {
                uint32 ext2 = PPC_VLE_OPC_EXT2(current_core->current_opc);
                switch(ext2)
                {
                    case 0:
                        ppc_vle_opc_lmw(current_core);
                        break;
                    case 1:
                        ppc_vle_opc_stmw(current_core);
                        break;
                }

                return;
            }
        case 9:
            ppc_vle_opc_addic(current_core);
            return;
        case 12:
            ppc_vle_opc_andi(current_core);
            return;
        case 20:
            ppc_vle_opc_mulli(current_core);
            return;
        case 21:
            if((bits(current_core->current_opc, 8, 6) == 0)
                    && ((ext & 0x1f) == 21))
            {
                ppc_vle_opc_cmpi(current_core);
                return;
            }

            if((bits(current_core->current_opc, 8, 6) == 1)
                    && ((ext & 0x1f) == 21))
            {
                ppc_vle_opc_cmpli(current_core);
                return;
            }

            return;
        case 22:
        case 23:
            ppc_vle_opc_subfic(current_core);
            return;
        case 26:
        case 27:
            ppc_vle_opc_ori(current_core);
            return;
        case 28:
        case 29:
            ppc_vle_opc_xori(current_core);
            return;
        default:
            printf("pc : 0x%x | isntr : 0x%x\n",current_core->pc,current_core->current_opc);
            printf("[error]instruction not implement\n");
            skyeye_exit(-1);
    }
}

static void ppc_vle_opc_group_f2(e500_core_t *current_core)
{

    if(! PPC_VLE_OPC_BIT16(current_core->current_opc))
    {
        ppc_vle_opc_li(current_core);
        return;
    }

    uint32 ext = PPC_VLE_OPC_EXT(current_core->current_opc);
    switch (ext & 0x1f) {
        case 17:
            ppc_vle_opc_add2i(current_core);
            return;
        case 18:
            ppc_vle_opc_add2is(current_core);
            return;
        case 19:
            ppc_vle_opc_cmp16i(current_core);
            return;
        case 20:
            ppc_vle_opc_mull2i(current_core);
            return;
        case 21:
            ppc_vle_opc_cmpl16i(current_core);
            return;
        case 22:
            ppc_vle_opc_cmph16i(current_core);
            return;
        case 23:
            ppc_vle_opc_cmphl16i(current_core);
            return;
        case 24:
            ppc_vle_opc_or2i(current_core);
            return;
        case 25:
            ppc_vle_opc_and2i(current_core);
            return;
        case 26:
            ppc_vle_opc_or2is(current_core);
            return;
        case 28:
            ppc_vle_opc_lis(current_core);
            return;
        case 29:
            ppc_vle_opc_and2is(current_core);
            return;
        default:
            printf("pc : 0x%x | isntr : 0x%x\n",current_core->pc,current_core->current_opc);
            printf("[error]instruction not implement\n");
            skyeye_exit(-1);
    }
}

static void ppc_vle_opc_group_f3(e500_core_t *current_core)
{
    uint32 ext = bits(current_core->current_opc, 30, 21);
    switch(ext & 0x3ff)
    {
        case 14:
            ppc_vle_opc_cmph(current_core);
            return;
        case 16:
            ppc_vle_opc_mcrf(current_core);
            return;
        case 33:
            ppc_vle_opc_crnor(current_core);
            return;
        case 46:
            ppc_vle_opc_cmphl(current_core);
            return;
        case 56:
            ppc_vle_opc_slwi(current_core);
            return;
        case 129:
            ppc_vle_opc_crandc(current_core);
            return;
        case 193:
            ppc_vle_opc_crxor(current_core);
            return;
        case 225:
            ppc_vle_opc_crnand(current_core);
            return;
        case 257:
            ppc_vle_opc_crand(current_core);
            return;
        case 280:
            ppc_vle_opc_rlw(current_core);
            return;
        case 289:
            ppc_vle_opc_creqv(current_core);
            return;
        case 312:
            ppc_vle_opc_rlwi(current_core);
            return;
        case 417:
            ppc_vle_opc_crorc(current_core);
            return;
        case 449:
            ppc_vle_opc_cror(current_core);
            return;
        case 568:
            ppc_vle_opc_srwi(current_core);
            return;
        default:
            printf("pc : 0x%x | isntr : 0x%x\n",current_core->pc,current_core->current_opc);
            printf("[error]instruction not implement\n");
            skyeye_exit(-1);
    }
}

static void ppc_vle_opc_group_b(e500_core_t *current_core)
{
    uint32 ext = bit(current_core->current_opc, 6);
    switch(ext & 0x1)
    {
        case 0:
            ppc_vle_opc_b(current_core);
            return;
        case 1:
            ppc_vle_opc_bc(current_core);
            return;
    }
}

static void ppc_vle_opc_group_rlwi(e500_core_t *current_core)
{
    uint32 ext = bit(current_core->current_opc, 31);
    switch(ext & 0x1)
    {
        case 0:
            ppc_vle_opc_rlwimi(current_core);
            return;
        case 1:
            ppc_vle_opc_rlwinm(current_core);
            return;
    }
}

// main opcode 31
static void ppc_vle_opc_init_group2(e500_core_t * core)
{
    uint i;

    for (i = 0; i < (sizeof ppc_vle_opc_table_group2 / sizeof ppc_vle_opc_table_group2[0]); i++)
    {
        ppc_vle_opc_table_group2[i] = ppc_opc_invalid;
    }
    ppc_vle_opc_table_group2[0] = ppc_opc_cmp;
    ppc_vle_opc_table_group2[4] = ppc_opc_tw;
    ppc_vle_opc_table_group2[8] = ppc_opc_subfcx;   //+
    ppc_vle_opc_table_group2[10] = ppc_opc_addcx;   //+
    ppc_vle_opc_table_group2[11] = ppc_opc_mulhwux;
    ppc_vle_opc_table_group2[15] = ppc_opc_isel;
    ppc_vle_opc_table_group2[19] = ppc_opc_mfcr;
    ppc_vle_opc_table_group2[20] = ppc_opc_lwarx;
    ppc_vle_opc_table_group2[23] = ppc_opc_lwzx;
    ppc_vle_opc_table_group2[24] = ppc_opc_slwx;
    ppc_vle_opc_table_group2[26] = ppc_opc_cntlzwx;
    ppc_vle_opc_table_group2[28] = ppc_opc_andx;
    ppc_vle_opc_table_group2[32] = ppc_opc_cmpl;
    ppc_vle_opc_table_group2[40] = ppc_opc_subfx;
    ppc_vle_opc_table_group2[47] = ppc_opc_iselgt;
    ppc_vle_opc_table_group2[54] = ppc_opc_dcbst;
    ppc_vle_opc_table_group2[55] = ppc_opc_lwzux;
    ppc_vle_opc_table_group2[60] = ppc_opc_andcx;
    ppc_vle_opc_table_group2[75] = ppc_opc_mulhwx;
    ppc_vle_opc_table_group2[79] = ppc_opc_iseleq;
    ppc_vle_opc_table_group2[83] = ppc_opc_mfmsr;
    ppc_vle_opc_table_group2[86] = ppc_opc_dcbf;
    ppc_vle_opc_table_group2[87] = ppc_opc_lbzx;
    ppc_vle_opc_table_group2[104] = ppc_opc_negx;
    ppc_vle_opc_table_group2[119] = ppc_opc_lbzux;
    ppc_vle_opc_table_group2[124] = ppc_opc_norx;
    ppc_vle_opc_table_group2[131] = ppc_opc_wrtee;
    ppc_vle_opc_table_group2[136] = ppc_opc_subfex; //+
    ppc_vle_opc_table_group2[138] = ppc_opc_addex;  //+
    ppc_vle_opc_table_group2[143] = ppc_opc_isel;
    ppc_vle_opc_table_group2[144] = ppc_opc_mtcrf;
    ppc_vle_opc_table_group2[146] = ppc_opc_mtmsr;
    ppc_vle_opc_table_group2[150] = ppc_opc_stwcx_;
    ppc_vle_opc_table_group2[151] = ppc_opc_stwx;
    ppc_vle_opc_table_group2[163] = ppc_opc_wrteei;
    ppc_vle_opc_table_group2[166] = ppc_opc_dcbtls;
    ppc_vle_opc_table_group2[183] = ppc_opc_stwux;
    ppc_vle_opc_table_group2[200] = ppc_opc_subfzex;        //+
    ppc_vle_opc_table_group2[202] = ppc_opc_addzex; //+
    ppc_vle_opc_table_group2[207] = ppc_opc_isel;
    ppc_vle_opc_table_group2[210] = ppc_opc_mtsr;
    ppc_vle_opc_table_group2[215] = ppc_opc_stbx;
    ppc_vle_opc_table_group2[232] = ppc_opc_subfmex;        //+
    ppc_vle_opc_table_group2[234] = ppc_opc_addmex;
    ppc_vle_opc_table_group2[235] = ppc_opc_mullwx; //+
    ppc_vle_opc_table_group2[242] = ppc_opc_mtsrin;
    ppc_vle_opc_table_group2[246] = ppc_opc_dcbtst;
    ppc_vle_opc_table_group2[247] = ppc_opc_stbux;
    ppc_vle_opc_table_group2[266] = ppc_opc_addx;   //+
    ppc_vle_opc_table_group2[278] = ppc_opc_dcbt;
    ppc_vle_opc_table_group2[279] = ppc_opc_lhzx;
    ppc_vle_opc_table_group2[284] = ppc_opc_eqvx;
    ppc_vle_opc_table_group2[306] = ppc_opc_tlbie;
    ppc_vle_opc_table_group2[310] = ppc_opc_eciwx;
    ppc_vle_opc_table_group2[311] = ppc_opc_lhzux;
    ppc_vle_opc_table_group2[316] = ppc_opc_xorx;
    ppc_vle_opc_table_group2[339] = ppc_opc_mfspr;
    ppc_vle_opc_table_group2[343] = ppc_opc_lhax;
    ppc_vle_opc_table_group2[335] = ppc_opc_isel;
    ppc_vle_opc_table_group2[370] = ppc_opc_tlbia;
    ppc_vle_opc_table_group2[371] = ppc_opc_mfspr;
    ppc_vle_opc_table_group2[375] = ppc_opc_lhaux;
    ppc_vle_opc_table_group2[390] = ppc_opc_dcbi;
    ppc_vle_opc_table_group2[407] = ppc_opc_sthx;
    ppc_vle_opc_table_group2[412] = ppc_opc_orcx;
    ppc_vle_opc_table_group2[438] = ppc_opc_ecowx;
    ppc_vle_opc_table_group2[439] = ppc_opc_sthux;
    ppc_vle_opc_table_group2[444] = ppc_opc_orx;
    ppc_vle_opc_table_group2[459] = ppc_opc_divwux; //+
    ppc_vle_opc_table_group2[463] = ppc_opc_isel;
    ppc_vle_opc_table_group2[467] = ppc_opc_mtspr;
    ppc_vle_opc_table_group2[470] = ppc_opc_dcbi;
    ppc_vle_opc_table_group2[476] = ppc_opc_nandx;
    ppc_vle_opc_table_group2[491] = ppc_opc_divwx;  //+
    ppc_vle_opc_table_group2[512] = ppc_opc_mcrxr;
    ppc_vle_opc_table_group2[520] = ppc_opc_subfcox;
    ppc_vle_opc_table_group2[522] = ppc_opc_addcox;
    ppc_vle_opc_table_group2[527] = ppc_opc_isel;
    ppc_vle_opc_table_group2[533] = ppc_opc_lswx;
    ppc_vle_opc_table_group2[534] = ppc_opc_lwbrx;
    ppc_vle_opc_table_group2[535] = ppc_opc_lfsx;
    ppc_vle_opc_table_group2[536] = ppc_opc_srwx;
    ppc_vle_opc_table_group2[552] = ppc_opc_subfox;
    ppc_vle_opc_table_group2[566] = ppc_opc_tlbsync;
    ppc_vle_opc_table_group2[567] = ppc_opc_lfsux;
    ppc_vle_opc_table_group2[591] = ppc_opc_isel;
    ppc_vle_opc_table_group2[595] = ppc_opc_mfsr;
    ppc_vle_opc_table_group2[597] = ppc_opc_lswi;
    ppc_vle_opc_table_group2[598] = ppc_opc_sync;
    ppc_vle_opc_table_group2[599] = ppc_opc_lfdx;
    ppc_vle_opc_table_group2[616] = ppc_opc_negox;
    ppc_vle_opc_table_group2[631] = ppc_opc_lfdux;
    ppc_vle_opc_table_group2[648] = ppc_opc_subfeox;
    ppc_vle_opc_table_group2[650] = ppc_opc_addeox;
    ppc_vle_opc_table_group2[659] = ppc_opc_mfsrin;
    ppc_vle_opc_table_group2[661] = ppc_opc_stswx;
    ppc_vle_opc_table_group2[662] = ppc_opc_stwbrx;
    ppc_vle_opc_table_group2[663] = ppc_opc_stfsx;
    ppc_vle_opc_table_group2[695] = ppc_opc_stfsux;
    ppc_vle_opc_table_group2[712] = ppc_opc_subfzeox;
    ppc_vle_opc_table_group2[714] = ppc_opc_addzeox;
    ppc_vle_opc_table_group2[725] = ppc_opc_stswi;
    ppc_vle_opc_table_group2[727] = ppc_opc_stfdx;
    ppc_vle_opc_table_group2[744] = ppc_opc_subfzeox;
    ppc_vle_opc_table_group2[746] = ppc_opc_addmeox;
    ppc_vle_opc_table_group2[747] = ppc_opc_mullwox;
    ppc_vle_opc_table_group2[758] = ppc_opc_dcba;
    ppc_vle_opc_table_group2[759] = ppc_opc_stfdux;
    ppc_vle_opc_table_group2[778] = ppc_opc_addox; //+
    ppc_vle_opc_table_group2[783] = ppc_opc_isel;
    ppc_vle_opc_table_group2[786] = ppc_opc_tlbivax;        /* TLB invalidated virtual address indexed */
    ppc_vle_opc_table_group2[790] = ppc_opc_lhbrx;
    ppc_vle_opc_table_group2[792] = ppc_opc_srawx;
    ppc_vle_opc_table_group2[815] = ppc_opc_isel;
    ppc_vle_opc_table_group2[824] = ppc_opc_srawix;
    ppc_vle_opc_table_group2[847] = ppc_opc_isel;
    ppc_vle_opc_table_group2[854] = ppc_opc_eieio;
    ppc_vle_opc_table_group2[911] = ppc_opc_isel;
    ppc_vle_opc_table_group2[914] = ppc_opc_tlbsx;
    ppc_vle_opc_table_group2[918] = ppc_opc_sthbrx;
    ppc_vle_opc_table_group2[922] = ppc_opc_extshx;
    ppc_vle_opc_table_group2[946] = ppc_opc_tlbrehi;
    ppc_vle_opc_table_group2[954] = ppc_opc_extsbx;
    ppc_vle_opc_table_group2[971] = ppc_opc_divwuox;
    ppc_vle_opc_table_group2[975] = ppc_opc_isel;
    ppc_vle_opc_table_group2[943] = ppc_opc_isel;
    ppc_vle_opc_table_group2[978] = ppc_opc_tlbwe;  /* TLB write entry */
    ppc_vle_opc_table_group2[982] = ppc_opc_icbi;
    ppc_vle_opc_table_group2[983] = ppc_opc_stfiwx;
    ppc_vle_opc_table_group2[1003] = ppc_opc_divwox;
    ppc_vle_opc_table_group2[1014] = ppc_opc_dcbz;
    ppc_vle_opc_table_group2[822] = ppc_opc_dss;    /*Temporarily modify */

    /* vle */
    ppc_vle_opc_table_group2[14] = ppc_vle_opc_cmph;
    ppc_vle_opc_table_group2[16] = ppc_vle_opc_mcrf;
    ppc_vle_opc_table_group2[33] = ppc_vle_opc_crnor;
    ppc_vle_opc_table_group2[46] = ppc_vle_opc_cmphl;
    ppc_vle_opc_table_group2[56] = ppc_vle_opc_slwi;
    ppc_vle_opc_table_group2[129] = ppc_vle_opc_crandc;
    ppc_vle_opc_table_group2[193] = ppc_vle_opc_crxor;
    ppc_vle_opc_table_group2[225] = ppc_vle_opc_crnand;
    ppc_vle_opc_table_group2[257] = ppc_vle_opc_crand;
    ppc_vle_opc_table_group2[280] = ppc_vle_opc_rlw;
    ppc_vle_opc_table_group2[289] = ppc_vle_opc_creqv;
    ppc_vle_opc_table_group2[312] = ppc_vle_opc_rlwi;
    ppc_vle_opc_table_group2[417] = ppc_vle_opc_crorc;
    ppc_vle_opc_table_group2[449] = ppc_vle_opc_cror;
    ppc_vle_opc_table_group2[568] = ppc_vle_opc_srwi;


    if ((ppc_cpu_get_pvr(core) & 0xffff0000) == 0x000c0000)
    {
        /* Added for Altivec support */
        ppc_vle_opc_table_group2[6] = ppc_opc_lvsl;
        ppc_vle_opc_table_group2[7] = ppc_opc_lvebx;
        ppc_vle_opc_table_group2[38] = ppc_opc_lvsr;
        ppc_vle_opc_table_group2[39] = ppc_opc_lvehx;
        ppc_vle_opc_table_group2[71] = ppc_opc_lvewx;
        ppc_vle_opc_table_group2[103] = ppc_opc_lvx;
        ppc_vle_opc_table_group2[135] = ppc_opc_stvebx;
        ppc_vle_opc_table_group2[167] = ppc_opc_stvehx;
        ppc_vle_opc_table_group2[199] = ppc_opc_stvewx;
        ppc_vle_opc_table_group2[231] = ppc_opc_stvx;
        ppc_vle_opc_table_group2[342] = ppc_opc_dst;
        ppc_vle_opc_table_group2[359] = ppc_opc_lvxl;
        ppc_vle_opc_table_group2[374] = ppc_opc_dstst;
        ppc_vle_opc_table_group2[487] = ppc_opc_stvxl;
        ppc_vle_opc_table_group2[822] = ppc_opc_dss;
    }
};

/*  main vle opcode 31 */
inline static void ppc_vle_opc_group_2(e500_core_t *current_core)
{
    uint32 ext = PPC_OPC_EXT(current_core->current_opc);

    if (ext >= (sizeof ppc_vle_opc_table_group2 / sizeof ppc_vle_opc_table_group2[0]))
    {
        ppc_opc_invalid(current_core);
    }
    ppc_vle_opc_table_group2[ext] (current_core);
}

/* se table */
static ppc_opc_function ppc_vle_se_opc_form_c_0_table[16] =
{
    &ppc_vle_se_opc_illegal,      //0
    &ppc_vle_se_opc_isync,        //1
    &ppc_vle_se_opc_sc,           //2
    &ppc_opc_invalid,             //3
    &ppc_vle_se_opc_blr,          //4
    &ppc_opc_invalid,             //5
    &ppc_vle_se_opc_bctr,         //6
    &ppc_vle_se_opc_bctr,         //7
    &ppc_vle_se_opc_rfi,          //8
    &ppc_vle_se_opc_rfci,         //9
    &ppc_vle_se_opc_rfdi,         //10
    &ppc_vle_se_opc_rfmci,        //11
    &ppc_vle_se_opc_rfgi,         //12
    &ppc_opc_invalid,             //13
    &ppc_opc_invalid,             //14
    &ppc_opc_invalid              //15
};

static ppc_opc_function ppc_vle_se_opc_form_r_0_table[16] =
{
    &ppc_opc_invalid,            //0
    &ppc_opc_invalid,            //1
    &ppc_vle_se_opc_not,         //2
    &ppc_vle_se_opc_neg,         //3
    &ppc_opc_invalid,            //4
    &ppc_opc_invalid,            //5
    &ppc_opc_invalid,            //6
    &ppc_opc_invalid,            //7
    &ppc_vle_se_opc_mflr,        //8
    &ppc_vle_se_opc_mtlr,        //9
    &ppc_vle_se_opc_mfctr,      //10
    &ppc_vle_se_opc_mtctr,      //11
    &ppc_vle_se_opc_extzb,      //12
    &ppc_vle_se_opc_extsb,      //13
    &ppc_vle_se_opc_extzh,      //14
    &ppc_vle_se_opc_extsh,      //15
};

static ppc_opc_function ppc_vle_se_opc_form_rr_0_table[16] =
{
    &ppc_opc_invalid,           //0
    &ppc_vle_se_opc_mr,         //1
    &ppc_vle_se_opc_mtar,       //2
    &ppc_vle_se_opc_mfar,       //3
    &ppc_vle_se_opc_add,        //4
    &ppc_vle_se_opc_mullw,      //5
    &ppc_vle_se_opc_sub,        //6
    &ppc_vle_se_opc_subf,       //7
    &ppc_opc_invalid,           //8
    &ppc_opc_invalid,           //9
    &ppc_opc_invalid,          //10
    &ppc_opc_invalid,          //11
    &ppc_vle_se_opc_cmp,       //12
    &ppc_vle_se_opc_cmpl,      //13
    &ppc_vle_se_opc_cmph,      //14
    &ppc_vle_se_opc_cmphl,     //15
};

static ppc_opc_function ppc_vle_se_opc_form_oim5_im5_2_table[16] =
{

    &ppc_vle_se_opc_addi,      //0
    &ppc_vle_se_opc_addi,      //1
    &ppc_vle_se_opc_cmpli,     //2
    &ppc_vle_se_opc_cmpli,     //3
    &ppc_vle_se_opc_subi,      //4
    &ppc_opc_invalid,          //5
    &ppc_opc_invalid,          //6
    &ppc_opc_invalid,          //7
    &ppc_opc_invalid,          //8
    &ppc_opc_invalid,          //9
    &ppc_vle_se_opc_cmpi,      //10
    &ppc_opc_invalid,          //11
    &ppc_vle_se_opc_bmaski,    //12
    &ppc_opc_invalid,          //13
    &ppc_vle_se_opc_andi,      //14
    &ppc_opc_invalid,          //15

};

static ppc_opc_function ppc_vle_se_opc_form_rr_4_table[16] =
{
    &ppc_vle_se_opc_srw,       //0
    &ppc_vle_se_opc_sraw,      //1
    &ppc_vle_se_opc_slw,       //2
    &ppc_opc_invalid,          //3
    &ppc_vle_se_opc_or,        //4
    &ppc_vle_se_opc_andc,      //5
    &ppc_vle_se_opc_and,       //6
    &ppc_opc_invalid,          //7
    &ppc_vle_se_opc_li,        //8
    &ppc_vle_se_opc_li,        //9
    &ppc_opc_invalid,          //10
    &ppc_vle_se_opc_li,        //11
    &ppc_vle_se_opc_li,        //12
    &ppc_vle_se_opc_li,        //13
    &ppc_vle_se_opc_li,        //14
    &ppc_opc_invalid           //15
};

static ppc_opc_function ppc_vle_se_opc_form_im5_6_table[16] =
{
    &ppc_vle_se_opc_bclri,     //0
    &ppc_vle_se_opc_bclri,     //1
    &ppc_vle_se_opc_bgeni,     //2
    &ppc_opc_invalid,          //3
    &ppc_vle_se_opc_bseti,     //4
    &ppc_vle_se_opc_bseti,     //5
    &ppc_vle_se_opc_btsti,     //6
    &ppc_vle_se_opc_btsti,     //7
    &ppc_vle_se_opc_srwi,      //8
    &ppc_vle_se_opc_srwi,      //9
    &ppc_vle_se_opc_srawi,    //10
    &ppc_opc_invalid,         //11
    &ppc_vle_se_opc_slwi,     //12
    &ppc_opc_invalid,         //13
    &ppc_opc_invalid,         //14
    &ppc_opc_invalid          //15
};

static ppc_opc_function ppc_vle_se_opc_form_sd4_table[16] =
{
    &ppc_opc_invalid,          //0
    &ppc_opc_invalid,          //1
    &ppc_opc_invalid,          //2
    &ppc_opc_invalid,          //3
    &ppc_opc_invalid,          //4
    &ppc_opc_invalid,          //5
    &ppc_opc_invalid,          //6
    &ppc_opc_invalid,          //7
    &ppc_vle_se_opc_lbz,       //8
    &ppc_vle_se_opc_stb,       //9
    &ppc_vle_opc_se_lhz,      //10
    &ppc_vle_se_opc_sth,      //11
    &ppc_vle_se_opc_lwz,      //12
    &ppc_vle_se_opc_stw,      //13
    &ppc_opc_invalid,         //14
    &ppc_opc_invalid,         //15
};

static ppc_opc_function ppc_vle_se_opc_form_bd8_table[3] =
{

    &ppc_vle_se_opc_bc,       //0
    &ppc_vle_se_opc_b,        //1
    &ppc_opc_invalid,         //2
};

static ppc_opc_function ppc_vle_opc_table_main[32] =
{
    &ppc_opc_invalid,           //0
    &ppc_opc_invalid,           //1
    &ppc_opc_invalid,           //2
    &ppc_opc_invalid,           //3
    &ppc_opc_group_v,           //4
    &ppc_opc_invalid,           //5
    &ppc_vle_opc_group_f1,      //6
    &ppc_vle_opc_add16i,        //7
    &ppc_opc_invalid,           //8
    &ppc_opc_invalid,           //9
    &ppc_opc_invalid,           //10
    &ppc_opc_invalid,           //11
    &ppc_vle_opc_lbz,           //12
    &ppc_vle_opc_stb,           //13
    &ppc_vle_opc_lha,           //14
    &ppc_opc_invalid,           //15
    &ppc_opc_invalid,           //16
    &ppc_opc_invalid,           //17
    &ppc_opc_invalid,           //18
    &ppc_opc_invalid,           //19
    &ppc_vle_opc_lwz,           //20
    &ppc_vle_opc_stw,           //21
    &ppc_vle_opc_lhz,           //22
    &ppc_vle_opc_sth,           //23
    &ppc_opc_invalid,           //24
    &ppc_opc_invalid,           //25
    &ppc_opc_invalid,           //26
    &ppc_opc_invalid,           //27
    &ppc_vle_opc_group_f2,      //28
    &ppc_vle_opc_group_rlwi,    //29
    &ppc_vle_opc_group_b,       //30
    &ppc_vle_opc_group_2,       //31
};

ppc_opc_function ppc_vle_se_instr_analysis_excute(e500_core_t *core, uint32_t instr, uint8_t excute_type)
{
    uint32_t InsnBranchCode_I;
    uint32_t InsnBranchCode_II;
    uint32_t  VleSeInsnArrayIndex;
    uint32_t CurrInsnPrefix4bit;

    CurrInsnPrefix4bit = (instr >> 28) & 0xF;

    switch (CurrInsnPrefix4bit)
    {
        /* Form C_0,R_0,RR_0 */
        case 0x0:
            InsnBranchCode_I = (instr >> 24) & 0xFF;
            if(InsnBranchCode_I == 0x00)
            {
                InsnBranchCode_II = (instr >> 20) & 0xF;
                if(InsnBranchCode_II == 0)
                {
                    /* Form C_0 */
                    VleSeInsnArrayIndex = (instr >> 16) & 0xFFFF;

                    if(excute_type == EXCUTE_FUNC)
                    {
                        ppc_vle_se_opc_form_c_0_table[VleSeInsnArrayIndex](core);
                    }
                    else
                    {
                        return ppc_vle_se_opc_form_c_0_table[VleSeInsnArrayIndex];
                    }
                } else
                {
                    /* Form R_0 */
                    VleSeInsnArrayIndex = InsnBranchCode_II;

                    if(excute_type == EXCUTE_FUNC)
                    {
                        ppc_vle_se_opc_form_r_0_table[VleSeInsnArrayIndex](core);
                    }
                    else
                    {
                        return ppc_vle_se_opc_form_r_0_table[VleSeInsnArrayIndex];
                    }
                }
            } else
            {
                /* Form RR_0 */
                VleSeInsnArrayIndex = InsnBranchCode_I;

                if(excute_type == EXCUTE_FUNC)
                {
                    ppc_vle_se_opc_form_rr_0_table[VleSeInsnArrayIndex](core);
                }
                else
                {
                    return ppc_vle_se_opc_form_rr_0_table[VleSeInsnArrayIndex];
                }
            }

            //printf("Func: %s, VleSeInsnArrayIndex:0x%x\n", __func__, VleSeInsnArrayIndex);

            break;
            /* Form OIM5, IM5 */
        case 0x2:
            InsnBranchCode_I = (instr >> 24) & 0xF;
            VleSeInsnArrayIndex = InsnBranchCode_I;

            if(excute_type == EXCUTE_FUNC)
            {
                ppc_vle_se_opc_form_oim5_im5_2_table[VleSeInsnArrayIndex](core);
            }
            else
            {
                return ppc_vle_se_opc_form_oim5_im5_2_table[VleSeInsnArrayIndex];
            }

            break;
            /*Form RR_4 */
        case 0x4:
            InsnBranchCode_I = (instr >> 24) & 0xF;
            VleSeInsnArrayIndex = InsnBranchCode_I;

            if(excute_type == EXCUTE_FUNC)
            {
                ppc_vle_se_opc_form_rr_4_table[VleSeInsnArrayIndex](core);
            }
            else
            {
                return ppc_vle_se_opc_form_rr_4_table[VleSeInsnArrayIndex];
            }

            break;
            /* form_im5_6 */
        case 0x6:
            InsnBranchCode_I = (instr >> 24) & 0xF;
            VleSeInsnArrayIndex = InsnBranchCode_I;

            if(excute_type == EXCUTE_FUNC)
            {
                ppc_vle_se_opc_form_im5_6_table[VleSeInsnArrayIndex](core);
            }
            else
            {
                return ppc_vle_se_opc_form_im5_6_table[VleSeInsnArrayIndex];
            }

            break;
            /* sid4*/
        case 0x8:
        case 0x9:
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xD:
            VleSeInsnArrayIndex = CurrInsnPrefix4bit;

            if(excute_type == EXCUTE_FUNC)
            {
                ppc_vle_se_opc_form_sd4_table[VleSeInsnArrayIndex](core);
            }
            else
            {
                return ppc_vle_se_opc_form_sd4_table[VleSeInsnArrayIndex];
            }

            break;
            /* BD8 */
        case 0xE:
            InsnBranchCode_I = (instr >> 24) & 0xF;

            if(excute_type == EXCUTE_FUNC)
            {
                if(InsnBranchCode_I & 0x8)
                {
                    ppc_vle_se_opc_b();

                } else
                {
                    ppc_vle_se_opc_bc();
                }
            }
            else
            {
                if(InsnBranchCode_I & 0x8)
                {
                    return ppc_vle_se_opc_form_bd8_table[1];
                } else
                {
                    return ppc_vle_se_opc_form_bd8_table[0];
                }

            }
            break;

        default:

            if(excute_type == EXCUTE_FUNC)
            {
                ppc_opc_invalid(core);
            }
            else
            {
                return ppc_vle_se_opc_form_bd8_table[2];//ppc_opc_invalid
            }
            break;
    }

    return NULL;
}

void FASTCALL ppc_exec_vle_opc(e500_core_t *core)
{
    uint32_t InsnBranchCode_I;
    uint32_t InsnBranchCode_II;
    uint32_t  VleSeInsnArrayIndex;
    uint32_t vle_mainopc;
    uint32_t InsnBitsNum = 0;

    InsnBitsNum = InsnBitsNumJudge(core->current_opc);

    if(InsnBitsNum == 16)
    {
        /*SE instruction */
        ppc_vle_se_instr_analysis_excute(core, core->current_opc, EXCUTE_FUNC);
    } else
    {
        vle_mainopc = PPC_VLE_SE_OPC_MAIN(core->current_opc);
        ppc_vle_opc_table_main[vle_mainopc] (core);
    }

}
//////////////////////////////////////////////////////////////////////////////

// main opcode 19
static void ppc_opc_group_1(e500_core_t *current_core)
{
    uint32 ext = PPC_OPC_EXT(current_core->current_opc);

    //printf("DBG:in %s,before exec pc=0x%x,opc=0x%x,ext=0x%x\n", __FUNCTION__, current_core->pc, current_core->current_opc, ext);
    if (ext & 1)
    {
        // crxxx
        if (ext <= 225)
        {
            switch (ext)
            {
                case 33:
                    ppc_opc_crnor(current_core);
                    return;
                case 129:
                    ppc_opc_crandc(current_core);
                    return;
                case 193:
                    ppc_opc_crxor(current_core);
                    return;
                case 225:
                    ppc_opc_crnand(current_core);
                    return;
            }
        } else
        {
            switch (ext)
            {
                case 257:
                    ppc_opc_crand(current_core);
                    return;
                case 289:
                    ppc_opc_creqv(current_core);
                    return;
                case 417:
                    ppc_opc_crorc(current_core);
                    return;
                case 449:
                    ppc_opc_cror(current_core);
                    return;
            }
        }
    } else if (ext & (1 << 9))
    {
        // bcctrx
        if (ext == 528)
        {
            ppc_opc_bcctrx(current_core);
            return;
        }
    } else
    {
        switch (ext)
        {
            case 16:
                ppc_opc_bclrx(current_core);
                return;
            case 0:
                ppc_opc_mcrf(current_core);
                return;
            case 50:
                ppc_opc_rfi(current_core);
                return;
            case 150:
                ppc_opc_isync(current_core);
                return;
        }
    }
    ppc_opc_invalid(current_core);
}

// main opcode 4
ppc_opc_function ppc_opc_table_group_efx_evx[0x7FF];

static void ppc_opc_init_group_efx_evx(e500_core_t * core)
{
    uint32_t i;

    for (i = 0; i < (sizeof ppc_opc_table_group_efx_evx / sizeof ppc_opc_table_group_efx_evx[0]); i++)
    {
        ppc_opc_table_group_efx_evx[i] = ppc_opc_invalid;
    }

    ppc_opc_table_group_efx_evx[512] = ppc_opc_evaddw;
    ppc_opc_table_group_efx_evx[514] = ppc_opc_evaddiw;
    ppc_opc_table_group_efx_evx[516] = ppc_opc_evsubfw;
    ppc_opc_table_group_efx_evx[518] = ppc_opc_evsubifw;
    ppc_opc_table_group_efx_evx[520] = ppc_opc_evabs;
    ppc_opc_table_group_efx_evx[521] = ppc_opc_evneg;
    ppc_opc_table_group_efx_evx[522] = ppc_opc_evextsb;
    ppc_opc_table_group_efx_evx[523] = ppc_opc_evextsh;
    ppc_opc_table_group_efx_evx[524] = ppc_opc_evrndw;
    ppc_opc_table_group_efx_evx[525] = ppc_opc_evcntlzw;
    ppc_opc_table_group_efx_evx[526] = ppc_opc_evcntlsw;
    ppc_opc_table_group_efx_evx[527] = ppc_opc_brinc;
    ppc_opc_table_group_efx_evx[529] = ppc_opc_evand;
    ppc_opc_table_group_efx_evx[530] = ppc_opc_evandc;
    ppc_opc_table_group_efx_evx[534] = ppc_opc_evxor;
    ppc_opc_table_group_efx_evx[535] = ppc_opc_evor;
    ppc_opc_table_group_efx_evx[536] = ppc_opc_evnor;
    ppc_opc_table_group_efx_evx[537] = ppc_opc_eveqv;
    ppc_opc_table_group_efx_evx[539] = ppc_opc_evorc;
    ppc_opc_table_group_efx_evx[542] = ppc_opc_evnand;
    ppc_opc_table_group_efx_evx[544] = ppc_opc_evsrwu;
    ppc_opc_table_group_efx_evx[545] = ppc_opc_evsrws;
    ppc_opc_table_group_efx_evx[546] = ppc_opc_evsrwiu;
    ppc_opc_table_group_efx_evx[547] = ppc_opc_evsrwis;
    ppc_opc_table_group_efx_evx[548] = ppc_opc_evslw;
    ppc_opc_table_group_efx_evx[550] = ppc_opc_evslwi;
    ppc_opc_table_group_efx_evx[552] = ppc_opc_evrlw;
    ppc_opc_table_group_efx_evx[553] = ppc_opc_evsplati;
    ppc_opc_table_group_efx_evx[554] = ppc_opc_evrlwi;
    ppc_opc_table_group_efx_evx[555] = ppc_opc_evsplatfi;
    ppc_opc_table_group_efx_evx[556] = ppc_opc_evmergehi;
    ppc_opc_table_group_efx_evx[557] = ppc_opc_evmergelo;
    ppc_opc_table_group_efx_evx[558] = ppc_opc_evmergehilo;
    ppc_opc_table_group_efx_evx[559] = ppc_opc_evmergelohi;
    ppc_opc_table_group_efx_evx[560] = ppc_opc_evcmpgtu;
    ppc_opc_table_group_efx_evx[561] = ppc_opc_evcmpgts;
    ppc_opc_table_group_efx_evx[562] = ppc_opc_evcmpltu;
    ppc_opc_table_group_efx_evx[563] = ppc_opc_evcmplts;
    ppc_opc_table_group_efx_evx[564] = ppc_opc_evcmpeq;
    ppc_opc_table_group_efx_evx[632] = ppc_opc_evsel;
    ppc_opc_table_group_efx_evx[633] = ppc_opc_evsel;
    ppc_opc_table_group_efx_evx[634] = ppc_opc_evsel;
    ppc_opc_table_group_efx_evx[635] = ppc_opc_evsel;
    ppc_opc_table_group_efx_evx[636] = ppc_opc_evsel;
    ppc_opc_table_group_efx_evx[637] = ppc_opc_evsel;
    ppc_opc_table_group_efx_evx[638] = ppc_opc_evsel;
    ppc_opc_table_group_efx_evx[639] = ppc_opc_evsel;
    ppc_opc_table_group_efx_evx[640] = ppc_opc_evfsadd;
    ppc_opc_table_group_efx_evx[641] = ppc_opc_evfssub;
    ppc_opc_table_group_efx_evx[644] = ppc_opc_evfsabs;
    ppc_opc_table_group_efx_evx[645] = ppc_opc_evfsnabs;
    ppc_opc_table_group_efx_evx[646] = ppc_opc_evfsneg;
    ppc_opc_table_group_efx_evx[648] = ppc_opc_evfsmul;
    ppc_opc_table_group_efx_evx[649] = ppc_opc_evfsdiv;
    ppc_opc_table_group_efx_evx[652] = ppc_opc_evfscmpgt;
    ppc_opc_table_group_efx_evx[653] = ppc_opc_evfscmplt;
    ppc_opc_table_group_efx_evx[654] = ppc_opc_evfscmpeq;
    ppc_opc_table_group_efx_evx[656] = ppc_opc_evfscfui;
    ppc_opc_table_group_efx_evx[657] = ppc_opc_evfscfsi;
    ppc_opc_table_group_efx_evx[658] = ppc_opc_evfscfuf;
    ppc_opc_table_group_efx_evx[659] = ppc_opc_evfscfsf;
    ppc_opc_table_group_efx_evx[660] = ppc_opc_evfsctui;
    ppc_opc_table_group_efx_evx[661] = ppc_opc_evfsctsi;
    ppc_opc_table_group_efx_evx[662] = ppc_opc_evfsctuf;
    ppc_opc_table_group_efx_evx[663] = ppc_opc_evfsctsf;
    ppc_opc_table_group_efx_evx[664] = ppc_opc_evfsctuiz;
    ppc_opc_table_group_efx_evx[666] = ppc_opc_evfsctsiz;
    ppc_opc_table_group_efx_evx[668] = ppc_opc_evfststgt;
    ppc_opc_table_group_efx_evx[669] = ppc_opc_evfststlt;
    ppc_opc_table_group_efx_evx[670] = ppc_opc_evfststeq;
    ppc_opc_table_group_efx_evx[704] = ppc_opc_efsadd;
    ppc_opc_table_group_efx_evx[705] = ppc_opc_efssub;
    ppc_opc_table_group_efx_evx[708] = ppc_opc_efsabs;
    ppc_opc_table_group_efx_evx[709] = ppc_opc_efsnabs;
    ppc_opc_table_group_efx_evx[710] = ppc_opc_efsneg;
    ppc_opc_table_group_efx_evx[712] = ppc_opc_efsmul;
    ppc_opc_table_group_efx_evx[713] = ppc_opc_efsdiv;
    ppc_opc_table_group_efx_evx[716] = ppc_opc_efscmpgt;
    ppc_opc_table_group_efx_evx[717] = ppc_opc_efscmplt;
    ppc_opc_table_group_efx_evx[718] = ppc_opc_efscmpeq;
    ppc_opc_table_group_efx_evx[719] = ppc_opc_efscfd;
    ppc_opc_table_group_efx_evx[720] = ppc_opc_efscfui;
    ppc_opc_table_group_efx_evx[721] = ppc_opc_efscfsi;
    ppc_opc_table_group_efx_evx[722] = ppc_opc_efscfuf;
    ppc_opc_table_group_efx_evx[723] = ppc_opc_efscfsf;
    ppc_opc_table_group_efx_evx[724] = ppc_opc_efsctui;
    ppc_opc_table_group_efx_evx[725] = ppc_opc_efsctsi;
    ppc_opc_table_group_efx_evx[726] = ppc_opc_efsctuf;
    ppc_opc_table_group_efx_evx[727] = ppc_opc_efsctsf;
    ppc_opc_table_group_efx_evx[728] = ppc_opc_efsctuiz;
    ppc_opc_table_group_efx_evx[730] = ppc_opc_efsctsiz;
    ppc_opc_table_group_efx_evx[732] = ppc_opc_efststgt;
    ppc_opc_table_group_efx_evx[733] = ppc_opc_efststlt;
    ppc_opc_table_group_efx_evx[734] = ppc_opc_efststeq;
    ppc_opc_table_group_efx_evx[736] = ppc_opc_efdadd;
    ppc_opc_table_group_efx_evx[737] = ppc_opc_efdsub;
    ppc_opc_table_group_efx_evx[738] = ppc_opc_efdcfuid;
    ppc_opc_table_group_efx_evx[739] = ppc_opc_efdcfsid;
    ppc_opc_table_group_efx_evx[740] = ppc_opc_efdabs;
    ppc_opc_table_group_efx_evx[741] = ppc_opc_efdnabs;
    ppc_opc_table_group_efx_evx[742] = ppc_opc_efdneg;
    ppc_opc_table_group_efx_evx[744] = ppc_opc_efdmul;
    ppc_opc_table_group_efx_evx[745] = ppc_opc_efddiv;
    ppc_opc_table_group_efx_evx[746] = ppc_opc_efdctuidz;
    ppc_opc_table_group_efx_evx[747] = ppc_opc_efdctsidz;
    ppc_opc_table_group_efx_evx[748] = ppc_opc_efdcmpgt;
    ppc_opc_table_group_efx_evx[749] = ppc_opc_efdcmplt;
    ppc_opc_table_group_efx_evx[750] = ppc_opc_efdcmpeq;
    ppc_opc_table_group_efx_evx[751] = ppc_opc_efdcfs;
    ppc_opc_table_group_efx_evx[752] = ppc_opc_efdcfui;
    ppc_opc_table_group_efx_evx[753] = ppc_opc_efdcfsi;
    ppc_opc_table_group_efx_evx[754] = ppc_opc_efdcfuf;
    ppc_opc_table_group_efx_evx[755] = ppc_opc_efdcfsf;
    ppc_opc_table_group_efx_evx[756] = ppc_opc_efdctui;
    ppc_opc_table_group_efx_evx[757] = ppc_opc_efdctsi;
    ppc_opc_table_group_efx_evx[758] = ppc_opc_efdctuf;
    ppc_opc_table_group_efx_evx[759] = ppc_opc_efdctsf;
    ppc_opc_table_group_efx_evx[760] = ppc_opc_efdctuiz;
    ppc_opc_table_group_efx_evx[762] = ppc_opc_efdctsiz;
    ppc_opc_table_group_efx_evx[764] = ppc_opc_efdtstgt;
    ppc_opc_table_group_efx_evx[765] = ppc_opc_efdtstlt;
    ppc_opc_table_group_efx_evx[766] = ppc_opc_efdtsteq;
    ppc_opc_table_group_efx_evx[768] = ppc_opc_evlddx;
    ppc_opc_table_group_efx_evx[769] = ppc_opc_evldd;
    ppc_opc_table_group_efx_evx[770] = ppc_opc_evldwx;
    ppc_opc_table_group_efx_evx[771] = ppc_opc_evldw;
    ppc_opc_table_group_efx_evx[772] = ppc_opc_evldhx;
    ppc_opc_table_group_efx_evx[773] = ppc_opc_evldh;
    ppc_opc_table_group_efx_evx[776] = ppc_opc_evlhhesplatx;
    ppc_opc_table_group_efx_evx[777] = ppc_opc_evlhhesplat;
    ppc_opc_table_group_efx_evx[780] = ppc_opc_evlhhousplatx;
    ppc_opc_table_group_efx_evx[781] = ppc_opc_evlhhousplat;
    ppc_opc_table_group_efx_evx[782] = ppc_opc_evlhhossplatx;
    ppc_opc_table_group_efx_evx[783] = ppc_opc_evlhhossplat;
    ppc_opc_table_group_efx_evx[784] = ppc_opc_evlwhex;
    ppc_opc_table_group_efx_evx[785] = ppc_opc_evlwhe;
    ppc_opc_table_group_efx_evx[788] = ppc_opc_evlwhoux;
    ppc_opc_table_group_efx_evx[789] = ppc_opc_evlwhou;
    ppc_opc_table_group_efx_evx[790] = ppc_opc_evlwhosx;
    ppc_opc_table_group_efx_evx[791] = ppc_opc_evlwhos;
    ppc_opc_table_group_efx_evx[792] = ppc_opc_evlwwsplatx;
    ppc_opc_table_group_efx_evx[793] = ppc_opc_evlwwsplat;
    ppc_opc_table_group_efx_evx[796] = ppc_opc_evlwhsplatx;
    ppc_opc_table_group_efx_evx[797] = ppc_opc_evlwhsplat;
    ppc_opc_table_group_efx_evx[800] = ppc_opc_evstddx;
    ppc_opc_table_group_efx_evx[801] = ppc_opc_evstdd;
    ppc_opc_table_group_efx_evx[802] = ppc_opc_evstdwx;
    ppc_opc_table_group_efx_evx[803] = ppc_opc_evstdw;
    ppc_opc_table_group_efx_evx[804] = ppc_opc_evstdhx;
    ppc_opc_table_group_efx_evx[805] = ppc_opc_evstdh;
    ppc_opc_table_group_efx_evx[816] = ppc_opc_evstwhex;
    ppc_opc_table_group_efx_evx[817] = ppc_opc_evstwhe;
    ppc_opc_table_group_efx_evx[820] = ppc_opc_evstwhox;
    ppc_opc_table_group_efx_evx[821] = ppc_opc_evstwho;
    ppc_opc_table_group_efx_evx[824] = ppc_opc_evstwwex;
    ppc_opc_table_group_efx_evx[825] = ppc_opc_evstwwe;
    ppc_opc_table_group_efx_evx[828] = ppc_opc_evstwwox;
    ppc_opc_table_group_efx_evx[829] = ppc_opc_evstwwo;
    ppc_opc_table_group_efx_evx[1027] = ppc_opc_evmhessf;
    ppc_opc_table_group_efx_evx[1031] = ppc_opc_evmhossf;
    ppc_opc_table_group_efx_evx[1032] = ppc_opc_evmheumi;
    ppc_opc_table_group_efx_evx[1033] = ppc_opc_evmhesmi;
    ppc_opc_table_group_efx_evx[1035] = ppc_opc_evmhesmf;
    ppc_opc_table_group_efx_evx[1036] = ppc_opc_evmhoumi;
    ppc_opc_table_group_efx_evx[1037] = ppc_opc_evmhosmi;
    ppc_opc_table_group_efx_evx[1039] = ppc_opc_evmhosmf;
    ppc_opc_table_group_efx_evx[1059] = ppc_opc_evmhessfa;
    ppc_opc_table_group_efx_evx[1063] = ppc_opc_evmhossfa;
    ppc_opc_table_group_efx_evx[1064] = ppc_opc_evmheumia;
    ppc_opc_table_group_efx_evx[1065] = ppc_opc_evmhesmia;
    ppc_opc_table_group_efx_evx[1067] = ppc_opc_evmhesmfa;
    ppc_opc_table_group_efx_evx[1068] = ppc_opc_evmhoumia;
    ppc_opc_table_group_efx_evx[1069] = ppc_opc_evmhosmia;
    ppc_opc_table_group_efx_evx[1071] = ppc_opc_evmhosmfa;
    ppc_opc_table_group_efx_evx[1095] = ppc_opc_evmwhssf;
    ppc_opc_table_group_efx_evx[1100] = ppc_opc_evmwhumi;
    ppc_opc_table_group_efx_evx[1101] = ppc_opc_evmwhsmi;
    ppc_opc_table_group_efx_evx[1103] = ppc_opc_evmwhsmf;
    ppc_opc_table_group_efx_evx[1107] = ppc_opc_evmwssf;
    ppc_opc_table_group_efx_evx[1112] = ppc_opc_evmwumi;
    ppc_opc_table_group_efx_evx[1113] = ppc_opc_evmwsmi;
    ppc_opc_table_group_efx_evx[1115] = ppc_opc_evmwsmf;
    ppc_opc_table_group_efx_evx[1127] = ppc_opc_evmwhssfa;
    ppc_opc_table_group_efx_evx[1128] = ppc_opc_evmwlumia;
    ppc_opc_table_group_efx_evx[1132] = ppc_opc_evmwhumia;
    ppc_opc_table_group_efx_evx[1133] = ppc_opc_evmwhsmia;
    ppc_opc_table_group_efx_evx[1135] = ppc_opc_evmwhsmfa;
    ppc_opc_table_group_efx_evx[1139] = ppc_opc_evmwssfa;
    ppc_opc_table_group_efx_evx[1144] = ppc_opc_evmwumia;
    ppc_opc_table_group_efx_evx[1145] = ppc_opc_evmwsmia;
    ppc_opc_table_group_efx_evx[1147] = ppc_opc_evmwsmfa;
    ppc_opc_table_group_efx_evx[1216] = ppc_opc_evaddusiaaw;
    ppc_opc_table_group_efx_evx[1217] = ppc_opc_evaddssiaaw;
    ppc_opc_table_group_efx_evx[1218] = ppc_opc_evsubfusiaaw;
    ppc_opc_table_group_efx_evx[1219] = ppc_opc_evsubfssiaaw;
    ppc_opc_table_group_efx_evx[1220] = ppc_opc_evmra;
    ppc_opc_table_group_efx_evx[1222] = ppc_opc_evdivws;
    ppc_opc_table_group_efx_evx[1223] = ppc_opc_evdivwu;
    ppc_opc_table_group_efx_evx[1224] = ppc_opc_evaddumiaaw;
    ppc_opc_table_group_efx_evx[1225] = ppc_opc_evaddsmiaaw;
    ppc_opc_table_group_efx_evx[1226] = ppc_opc_evsubfumiaaw;
    ppc_opc_table_group_efx_evx[1227] = ppc_opc_evsubfsmiaaw;
    ppc_opc_table_group_efx_evx[1280] = ppc_opc_evmheusiaaw;
    ppc_opc_table_group_efx_evx[1281] = ppc_opc_evmhessiaaw;
    ppc_opc_table_group_efx_evx[1283] = ppc_opc_evmhessfaaw;
    ppc_opc_table_group_efx_evx[1284] = ppc_opc_evmhousiaaw;
    ppc_opc_table_group_efx_evx[1285] = ppc_opc_evmhossiaaw;
    ppc_opc_table_group_efx_evx[1287] = ppc_opc_evmhossfaaw;
    ppc_opc_table_group_efx_evx[1288] = ppc_opc_evmheumiaaw;
    ppc_opc_table_group_efx_evx[1289] = ppc_opc_evmhesmiaaw;
    ppc_opc_table_group_efx_evx[1291] = ppc_opc_evmhesmfaaw;
    ppc_opc_table_group_efx_evx[1292] = ppc_opc_evmhoumiaaw;
    ppc_opc_table_group_efx_evx[1293] = ppc_opc_evmhosmiaaw;
    ppc_opc_table_group_efx_evx[1295] = ppc_opc_evmhosmfaaw;
    ppc_opc_table_group_efx_evx[1320] = ppc_opc_evmhegumiaa;
    ppc_opc_table_group_efx_evx[1321] = ppc_opc_evmhegsmiaa;
    ppc_opc_table_group_efx_evx[1323] = ppc_opc_evmhegsmfaa;
    ppc_opc_table_group_efx_evx[1324] = ppc_opc_evmhogumiaa;
    ppc_opc_table_group_efx_evx[1325] = ppc_opc_evmhogsmiaa;
    ppc_opc_table_group_efx_evx[1327] = ppc_opc_evmhogsmfaa;
    ppc_opc_table_group_efx_evx[1344] = ppc_opc_evmwlusiaaw;
    ppc_opc_table_group_efx_evx[1345] = ppc_opc_evmwlssiaaw;
    ppc_opc_table_group_efx_evx[1352] = ppc_opc_evmwlumiaaw;
    ppc_opc_table_group_efx_evx[1353] = ppc_opc_evmwlsmiaaw;
    ppc_opc_table_group_efx_evx[1363] = ppc_opc_evmwssfaa;
    ppc_opc_table_group_efx_evx[1368] = ppc_opc_evmwumiaa;
    ppc_opc_table_group_efx_evx[1369] = ppc_opc_evmwsmiaa;
    ppc_opc_table_group_efx_evx[1371] = ppc_opc_evmwsmfaa;
    ppc_opc_table_group_efx_evx[1408] = ppc_opc_evmheusianw;
    ppc_opc_table_group_efx_evx[1409] = ppc_opc_evmhessianw;
    ppc_opc_table_group_efx_evx[1411] = ppc_opc_evmhessfanw;
    ppc_opc_table_group_efx_evx[1412] = ppc_opc_evmhousianw;
    ppc_opc_table_group_efx_evx[1413] = ppc_opc_evmhossianw;
    ppc_opc_table_group_efx_evx[1415] = ppc_opc_evmhossfanw;
    ppc_opc_table_group_efx_evx[1416] = ppc_opc_evmheumianw;
    ppc_opc_table_group_efx_evx[1417] = ppc_opc_evmhesmianw;
    ppc_opc_table_group_efx_evx[1419] = ppc_opc_evmhesmfanw;
    ppc_opc_table_group_efx_evx[1420] = ppc_opc_evmhoumianw;
    ppc_opc_table_group_efx_evx[1421] = ppc_opc_evmhosmianw;
    ppc_opc_table_group_efx_evx[1423] = ppc_opc_evmhosmfanw;
    ppc_opc_table_group_efx_evx[1448] = ppc_opc_evmhegumian;
    ppc_opc_table_group_efx_evx[1449] = ppc_opc_evmhegsmian;
    ppc_opc_table_group_efx_evx[1451] = ppc_opc_evmhegsmfan;
    ppc_opc_table_group_efx_evx[1452] = ppc_opc_evmhogumian;
    ppc_opc_table_group_efx_evx[1453] = ppc_opc_evmhogsmian;
    ppc_opc_table_group_efx_evx[1455] = ppc_opc_evmhogsmfan;
    ppc_opc_table_group_efx_evx[1472] = ppc_opc_evmwlusianw;
    ppc_opc_table_group_efx_evx[1473] = ppc_opc_evmwlssianw;
    ppc_opc_table_group_efx_evx[1480] = ppc_opc_evmwlumianw;
    ppc_opc_table_group_efx_evx[1481] = ppc_opc_evmwlsmianw;
    ppc_opc_table_group_efx_evx[1491] = ppc_opc_evmwssfan;
    ppc_opc_table_group_efx_evx[1496] = ppc_opc_evmwumian;
    ppc_opc_table_group_efx_evx[1497] = ppc_opc_evmwsmian;
    ppc_opc_table_group_efx_evx[1499] = ppc_opc_evmwsmfan;
}

static void ppc_opc_group_efx_evx(e500_core_t *current_core)
{
    uint32 ext = current_core->current_opc & 0x7FF;

    if (ext >= (sizeof ppc_opc_table_group_efx_evx / sizeof ppc_opc_table_group_efx_evx[0]))
    {
        ppc_opc_invalid(current_core);
    }
    ppc_opc_table_group_efx_evx[ext] (current_core);
}

ppc_opc_function ppc_opc_table_group2[1015];

// main opcode 31
static void ppc_opc_init_group2(e500_core_t * core)
{
    uint i;

    //ppc_opc_table_group2 = (ppc_opc_function *)malloc(sizeof(ppc_opc_function) * 1015);
    for (i = 0; i < (sizeof ppc_opc_table_group2 / sizeof ppc_opc_table_group2[0]); i++)
    {
        ppc_opc_table_group2[i] = ppc_opc_invalid;
    }
    ppc_opc_table_group2[0] = ppc_opc_cmp;
    ppc_opc_table_group2[4] = ppc_opc_tw;
    ppc_opc_table_group2[8] = ppc_opc_subfcx;   //+
    ppc_opc_table_group2[10] = ppc_opc_addcx;   //+
    ppc_opc_table_group2[11] = ppc_opc_mulhwux;
    ppc_opc_table_group2[15] = ppc_opc_isel;
    ppc_opc_table_group2[19] = ppc_opc_mfcr;
    ppc_opc_table_group2[20] = ppc_opc_lwarx;
    ppc_opc_table_group2[23] = ppc_opc_lwzx;
    ppc_opc_table_group2[24] = ppc_opc_slwx;
    ppc_opc_table_group2[26] = ppc_opc_cntlzwx;
    ppc_opc_table_group2[28] = ppc_opc_andx;
    ppc_opc_table_group2[32] = ppc_opc_cmpl;
    ppc_opc_table_group2[40] = ppc_opc_subfx;
    ppc_opc_table_group2[47] = ppc_opc_iselgt;
    ppc_opc_table_group2[54] = ppc_opc_dcbst;
    ppc_opc_table_group2[55] = ppc_opc_lwzux;
    ppc_opc_table_group2[60] = ppc_opc_andcx;
    ppc_opc_table_group2[75] = ppc_opc_mulhwx;
    ppc_opc_table_group2[79] = ppc_opc_iseleq;
    ppc_opc_table_group2[83] = ppc_opc_mfmsr;
    ppc_opc_table_group2[86] = ppc_opc_dcbf;
    ppc_opc_table_group2[87] = ppc_opc_lbzx;
    ppc_opc_table_group2[104] = ppc_opc_negx;
    ppc_opc_table_group2[119] = ppc_opc_lbzux;
    ppc_opc_table_group2[124] = ppc_opc_norx;
    ppc_opc_table_group2[131] = ppc_opc_wrtee;
    ppc_opc_table_group2[136] = ppc_opc_subfex; //+
    ppc_opc_table_group2[138] = ppc_opc_addex;  //+
    ppc_opc_table_group2[143] = ppc_opc_isel;
    ppc_opc_table_group2[144] = ppc_opc_mtcrf;
    ppc_opc_table_group2[146] = ppc_opc_mtmsr;
    ppc_opc_table_group2[150] = ppc_opc_stwcx_;
    ppc_opc_table_group2[151] = ppc_opc_stwx;
    ppc_opc_table_group2[163] = ppc_opc_wrteei;
    ppc_opc_table_group2[166] = ppc_opc_dcbtls;
    ppc_opc_table_group2[183] = ppc_opc_stwux;
    ppc_opc_table_group2[200] = ppc_opc_subfzex;        //+
    ppc_opc_table_group2[202] = ppc_opc_addzex; //+
    ppc_opc_table_group2[207] = ppc_opc_isel;
    ppc_opc_table_group2[210] = ppc_opc_mtsr;
    ppc_opc_table_group2[215] = ppc_opc_stbx;
    ppc_opc_table_group2[232] = ppc_opc_subfmex;        //+
    ppc_opc_table_group2[234] = ppc_opc_addmex;
    ppc_opc_table_group2[235] = ppc_opc_mullwx; //+
    ppc_opc_table_group2[242] = ppc_opc_mtsrin;
    ppc_opc_table_group2[246] = ppc_opc_dcbtst;
    ppc_opc_table_group2[247] = ppc_opc_stbux;
    ppc_opc_table_group2[266] = ppc_opc_addx;   //+
    ppc_opc_table_group2[278] = ppc_opc_dcbt;
    ppc_opc_table_group2[279] = ppc_opc_lhzx;
    ppc_opc_table_group2[284] = ppc_opc_eqvx;
    ppc_opc_table_group2[306] = ppc_opc_tlbie;
    ppc_opc_table_group2[310] = ppc_opc_eciwx;
    ppc_opc_table_group2[311] = ppc_opc_lhzux;
    ppc_opc_table_group2[316] = ppc_opc_xorx;
    ppc_opc_table_group2[339] = ppc_opc_mfspr;
    ppc_opc_table_group2[343] = ppc_opc_lhax;
    ppc_opc_table_group2[335] = ppc_opc_isel;
    ppc_opc_table_group2[370] = ppc_opc_tlbia;
    ppc_opc_table_group2[371] = ppc_opc_mfspr;
    ppc_opc_table_group2[375] = ppc_opc_lhaux;
    ppc_opc_table_group2[390] = ppc_opc_dcbi;
    ppc_opc_table_group2[407] = ppc_opc_sthx;
    ppc_opc_table_group2[412] = ppc_opc_orcx;
    ppc_opc_table_group2[438] = ppc_opc_ecowx;
    ppc_opc_table_group2[439] = ppc_opc_sthux;
    ppc_opc_table_group2[444] = ppc_opc_orx;
    ppc_opc_table_group2[459] = ppc_opc_divwux; //+
    ppc_opc_table_group2[463] = ppc_opc_isel;
    ppc_opc_table_group2[467] = ppc_opc_mtspr;
    ppc_opc_table_group2[470] = ppc_opc_dcbi;
    ppc_opc_table_group2[476] = ppc_opc_nandx;
    ppc_opc_table_group2[491] = ppc_opc_divwx;  //+
    ppc_opc_table_group2[512] = ppc_opc_mcrxr;
    ppc_opc_table_group2[520] = ppc_opc_subfcox;
    ppc_opc_table_group2[522] = ppc_opc_addcox;
    ppc_opc_table_group2[527] = ppc_opc_isel;
    ppc_opc_table_group2[533] = ppc_opc_lswx;
    ppc_opc_table_group2[534] = ppc_opc_lwbrx;
    ppc_opc_table_group2[535] = ppc_opc_lfsx;
    ppc_opc_table_group2[536] = ppc_opc_srwx;
    ppc_opc_table_group2[552] = ppc_opc_subfox;
    ppc_opc_table_group2[566] = ppc_opc_tlbsync;
    ppc_opc_table_group2[567] = ppc_opc_lfsux;
    ppc_opc_table_group2[591] = ppc_opc_isel;
    ppc_opc_table_group2[595] = ppc_opc_mfsr;
    ppc_opc_table_group2[597] = ppc_opc_lswi;
    ppc_opc_table_group2[598] = ppc_opc_sync;
    ppc_opc_table_group2[599] = ppc_opc_lfdx;
    ppc_opc_table_group2[616] = ppc_opc_negox;
    ppc_opc_table_group2[631] = ppc_opc_lfdux;
    ppc_opc_table_group2[648] = ppc_opc_subfeox;
    ppc_opc_table_group2[650] = ppc_opc_addeox;
    ppc_opc_table_group2[659] = ppc_opc_mfsrin;
    ppc_opc_table_group2[661] = ppc_opc_stswx;
    ppc_opc_table_group2[662] = ppc_opc_stwbrx;
    ppc_opc_table_group2[663] = ppc_opc_stfsx;
    ppc_opc_table_group2[695] = ppc_opc_stfsux;
    ppc_opc_table_group2[712] = ppc_opc_subfzeox;
    ppc_opc_table_group2[714] = ppc_opc_addzeox;
    ppc_opc_table_group2[725] = ppc_opc_stswi;
    ppc_opc_table_group2[727] = ppc_opc_stfdx;
    ppc_opc_table_group2[744] = ppc_opc_subfzeox;
    ppc_opc_table_group2[746] = ppc_opc_addmeox;
    ppc_opc_table_group2[747] = ppc_opc_mullwox;
    ppc_opc_table_group2[758] = ppc_opc_dcba;
    ppc_opc_table_group2[759] = ppc_opc_stfdux;
    ppc_opc_table_group2[778] = ppc_opc_addox; //+
    ppc_opc_table_group2[783] = ppc_opc_isel;
    ppc_opc_table_group2[786] = ppc_opc_tlbivax;        /* TLB invalidated virtual address indexed */
    ppc_opc_table_group2[790] = ppc_opc_lhbrx;
    ppc_opc_table_group2[792] = ppc_opc_srawx;
    ppc_opc_table_group2[815] = ppc_opc_isel;
    ppc_opc_table_group2[824] = ppc_opc_srawix;
    ppc_opc_table_group2[847] = ppc_opc_isel;
    ppc_opc_table_group2[854] = ppc_opc_eieio;
    ppc_opc_table_group2[911] = ppc_opc_isel;
    ppc_opc_table_group2[914] = ppc_opc_tlbsx;
    ppc_opc_table_group2[918] = ppc_opc_sthbrx;
    ppc_opc_table_group2[922] = ppc_opc_extshx;
    ppc_opc_table_group2[946] = ppc_opc_tlbrehi;
    ppc_opc_table_group2[954] = ppc_opc_extsbx;
    ppc_opc_table_group2[971] = ppc_opc_divwuox;
    ppc_opc_table_group2[975] = ppc_opc_isel;
    ppc_opc_table_group2[943] = ppc_opc_isel;
    ppc_opc_table_group2[978] = ppc_opc_tlbwe;  /* TLB write entry */
    ppc_opc_table_group2[982] = ppc_opc_icbi;
    ppc_opc_table_group2[983] = ppc_opc_stfiwx;
    ppc_opc_table_group2[1003] = ppc_opc_divwox;
    ppc_opc_table_group2[1014] = ppc_opc_dcbz;
    ppc_opc_table_group2[822] = ppc_opc_dss;    /*Temporarily modify */
    if ((ppc_cpu_get_pvr(core) & 0xffff0000) == 0x000c0000)
    {
        /* Added for Altivec support */
        ppc_opc_table_group2[6] = ppc_opc_lvsl;
        ppc_opc_table_group2[7] = ppc_opc_lvebx;
        ppc_opc_table_group2[38] = ppc_opc_lvsr;
        ppc_opc_table_group2[39] = ppc_opc_lvehx;
        ppc_opc_table_group2[71] = ppc_opc_lvewx;
        ppc_opc_table_group2[103] = ppc_opc_lvx;
        ppc_opc_table_group2[135] = ppc_opc_stvebx;
        ppc_opc_table_group2[167] = ppc_opc_stvehx;
        ppc_opc_table_group2[199] = ppc_opc_stvewx;
        ppc_opc_table_group2[231] = ppc_opc_stvx;
        ppc_opc_table_group2[342] = ppc_opc_dst;
        ppc_opc_table_group2[359] = ppc_opc_lvxl;
        ppc_opc_table_group2[374] = ppc_opc_dstst;
        ppc_opc_table_group2[487] = ppc_opc_stvxl;
        ppc_opc_table_group2[822] = ppc_opc_dss;
    }
}

// main opcode 31
inline static void ppc_opc_group_2(e500_core_t *current_core)
{
    uint32 ext = PPC_OPC_EXT(current_core->current_opc);

    if (ext >= (sizeof ppc_opc_table_group2 / sizeof ppc_opc_table_group2[0]))
    {
        ppc_opc_invalid(current_core);
    }
    ppc_opc_table_group2[ext] (current_core);
}

// main opcode 59
static void ppc_opc_group_f1(e500_core_t *current_core)
{
    if ((current_core->msr & MSR_FP) == 0)
    {
        ppc_exception(current_core, PPC_EXC_NO_FPU, 0, 0);
        return;
    }
    uint32 ext = PPC_OPC_EXT(current_core->current_opc);

    switch (ext & 0x1f)
    {
        case 18:
            ppc_opc_fdivsx(current_core);
            return;
        case 20:
            ppc_opc_fsubsx(current_core);
            return;
        case 21:
            ppc_opc_faddsx(current_core);
            return;
        case 22:
            ppc_opc_fsqrtsx(current_core);
            return;
        case 24:
            ppc_opc_fresx(current_core);
            return;
        case 25:
            ppc_opc_fmulsx(current_core);
            return;
        case 28:
            ppc_opc_fmsubsx(current_core);
            return;
        case 29:
            ppc_opc_fmaddsx(current_core);
            return;
        case 30:
            ppc_opc_fnmsubsx(current_core);
            return;
        case 31:
            ppc_opc_fnmaddsx(current_core);
            return;
    }
    ppc_opc_invalid(current_core);
}

// main opcode 63
static void ppc_opc_group_f2(e500_core_t *current_core)
{
    if ((current_core->msr & MSR_FP) == 0)
    {
        ppc_exception(current_core, PPC_EXC_NO_FPU, 0, 0);
        return;
    }
    uint32 ext = PPC_OPC_EXT(current_core->current_opc);

    if (ext & 16)
    {
        switch (ext & 0x1f)
        {
            case 18:
                ppc_opc_fdivx(current_core);
                return;
            case 20:
                ppc_opc_fsubx(current_core);
                return;
            case 21:
                ppc_opc_faddx(current_core);
                return;
            case 22:
                ppc_opc_fsqrtx(current_core);
                return;
            case 23:
                ppc_opc_fselx(current_core);
                return;
            case 25:
                ppc_opc_fmulx(current_core);
                return;
            case 26:
                ppc_opc_frsqrtex(current_core);
                return;
            case 28:
                ppc_opc_fmsubx(current_core);
                return;
            case 29:
                ppc_opc_fmaddx(current_core);
                return;
            case 30:
                ppc_opc_fnmsubx(current_core);
                return;
            case 31:
                ppc_opc_fnmaddx(current_core);
                return;
        }
    } else
    {
        switch (ext)
        {
            case 0:
                ppc_opc_fcmpu(current_core);
                return;
            case 12:
                ppc_opc_frspx(current_core);
                return;
            case 14:
                ppc_opc_fctiwx(current_core);
                return;
            case 15:
                ppc_opc_fctiwzx(current_core);
                return;
                //--
            case 32:
                ppc_opc_fcmpo(current_core);
                return;
            case 38:
                ppc_opc_mtfsb1x(current_core);
                return;
            case 40:
                ppc_opc_fnegx(current_core);
                return;
            case 64:
                ppc_opc_mcrfs(current_core);
                return;
            case 70:
                ppc_opc_mtfsb0x(current_core);
                return;
            case 72:
                ppc_opc_fmrx(current_core);
                return;
            case 134:
                ppc_opc_mtfsfix(current_core);
                return;
            case 136:
                ppc_opc_fnabsx(current_core);
                return;
            case 264:
                ppc_opc_fabsx(current_core);
                return;
            case 583:
                ppc_opc_mffsx(current_core);
                return;
            case 711:
                ppc_opc_mtfsfx(current_core);
                return;
        }
    }
    ppc_opc_invalid(current_core);
}

ppc_opc_function ppc_opc_table_groupv[965];

static void ppc_opc_init_groupv()
{
    uint i;

    for (i = 0; i < (sizeof ppc_opc_table_groupv / sizeof ppc_opc_table_groupv[0]); i++)
    {
        ppc_opc_table_groupv[i] = ppc_opc_invalid;
    }
    ppc_opc_table_groupv[0] = ppc_opc_vaddubm;
    ppc_opc_table_groupv[1] = ppc_opc_vmaxub;
    ppc_opc_table_groupv[2] = ppc_opc_vrlb;
    ppc_opc_table_groupv[4] = ppc_opc_vmuloub;
    ppc_opc_table_groupv[5] = ppc_opc_vaddfp;
    ppc_opc_table_groupv[6] = ppc_opc_vmrghb;
    ppc_opc_table_groupv[7] = ppc_opc_vpkuhum;
    ppc_opc_table_groupv[32] = ppc_opc_vadduhm;
    ppc_opc_table_groupv[33] = ppc_opc_vmaxuh;
    ppc_opc_table_groupv[34] = ppc_opc_vrlh;
    ppc_opc_table_groupv[36] = ppc_opc_vmulouh;
    ppc_opc_table_groupv[37] = ppc_opc_vsubfp;
    ppc_opc_table_groupv[38] = ppc_opc_vmrghh;
    ppc_opc_table_groupv[39] = ppc_opc_vpkuwum;
    ppc_opc_table_groupv[64] = ppc_opc_vadduwm;
    ppc_opc_table_groupv[65] = ppc_opc_vmaxuw;
    ppc_opc_table_groupv[66] = ppc_opc_vrlw;
    ppc_opc_table_groupv[70] = ppc_opc_vmrghw;
    ppc_opc_table_groupv[71] = ppc_opc_vpkuhus;
    ppc_opc_table_groupv[103] = ppc_opc_vpkuwus;
    ppc_opc_table_groupv[129] = ppc_opc_vmaxsb;
    ppc_opc_table_groupv[130] = ppc_opc_vslb;
    ppc_opc_table_groupv[132] = ppc_opc_vmulosb;
    ppc_opc_table_groupv[133] = ppc_opc_vrefp;
    ppc_opc_table_groupv[134] = ppc_opc_vmrglb;
    ppc_opc_table_groupv[135] = ppc_opc_vpkshus;
    ppc_opc_table_groupv[161] = ppc_opc_vmaxsh;
    ppc_opc_table_groupv[162] = ppc_opc_vslh;
    ppc_opc_table_groupv[164] = ppc_opc_vmulosh;
    ppc_opc_table_groupv[165] = ppc_opc_vrsqrtefp;
    ppc_opc_table_groupv[166] = ppc_opc_vmrglh;
    ppc_opc_table_groupv[167] = ppc_opc_vpkswus;
    ppc_opc_table_groupv[192] = ppc_opc_vaddcuw;
    ppc_opc_table_groupv[193] = ppc_opc_vmaxsw;
    ppc_opc_table_groupv[194] = ppc_opc_vslw;
    ppc_opc_table_groupv[197] = ppc_opc_vexptefp;
    ppc_opc_table_groupv[198] = ppc_opc_vmrglw;
    ppc_opc_table_groupv[199] = ppc_opc_vpkshss;
    ppc_opc_table_groupv[226] = ppc_opc_vsl;
    ppc_opc_table_groupv[229] = ppc_opc_vlogefp;
    ppc_opc_table_groupv[231] = ppc_opc_vpkswss;
    ppc_opc_table_groupv[256] = ppc_opc_vaddubs;
    ppc_opc_table_groupv[257] = ppc_opc_vminub;
    ppc_opc_table_groupv[258] = ppc_opc_vsrb;
    ppc_opc_table_groupv[260] = ppc_opc_vmuleub;
    ppc_opc_table_groupv[261] = ppc_opc_vrfin;
    ppc_opc_table_groupv[262] = ppc_opc_vspltb;
    ppc_opc_table_groupv[263] = ppc_opc_vupkhsb;
    ppc_opc_table_groupv[288] = ppc_opc_vadduhs;
    ppc_opc_table_groupv[289] = ppc_opc_vminuh;
    ppc_opc_table_groupv[290] = ppc_opc_vsrh;
    ppc_opc_table_groupv[292] = ppc_opc_vmuleuh;
    ppc_opc_table_groupv[293] = ppc_opc_vrfiz;
    ppc_opc_table_groupv[294] = ppc_opc_vsplth;
    ppc_opc_table_groupv[295] = ppc_opc_vupkhsh;
    ppc_opc_table_groupv[320] = ppc_opc_vadduws;
    ppc_opc_table_groupv[321] = ppc_opc_vminuw;
    ppc_opc_table_groupv[322] = ppc_opc_vsrw;
    ppc_opc_table_groupv[325] = ppc_opc_vrfip;
    ppc_opc_table_groupv[326] = ppc_opc_vspltw;
    ppc_opc_table_groupv[327] = ppc_opc_vupklsb;
    ppc_opc_table_groupv[354] = ppc_opc_vsr;
    ppc_opc_table_groupv[357] = ppc_opc_vrfim;
    ppc_opc_table_groupv[359] = ppc_opc_vupklsh;
    ppc_opc_table_groupv[384] = ppc_opc_vaddsbs;
    ppc_opc_table_groupv[385] = ppc_opc_vminsb;
    ppc_opc_table_groupv[386] = ppc_opc_vsrab;
    ppc_opc_table_groupv[388] = ppc_opc_vmulesb;
    ppc_opc_table_groupv[389] = ppc_opc_vcfux;
    ppc_opc_table_groupv[390] = ppc_opc_vspltisb;
    ppc_opc_table_groupv[391] = ppc_opc_vpkpx;
    ppc_opc_table_groupv[416] = ppc_opc_vaddshs;
    ppc_opc_table_groupv[417] = ppc_opc_vminsh;
    ppc_opc_table_groupv[418] = ppc_opc_vsrah;
    ppc_opc_table_groupv[420] = ppc_opc_vmulesh;
    ppc_opc_table_groupv[421] = ppc_opc_vcfsx;
    ppc_opc_table_groupv[422] = ppc_opc_vspltish;
    ppc_opc_table_groupv[423] = ppc_opc_vupkhpx;
    ppc_opc_table_groupv[448] = ppc_opc_vaddsws;
    ppc_opc_table_groupv[449] = ppc_opc_vminsw;
    ppc_opc_table_groupv[450] = ppc_opc_vsraw;
    ppc_opc_table_groupv[453] = ppc_opc_vctuxs;
    ppc_opc_table_groupv[454] = ppc_opc_vspltisw;
    ppc_opc_table_groupv[485] = ppc_opc_vctsxs;
    ppc_opc_table_groupv[487] = ppc_opc_vupklpx;
    ppc_opc_table_groupv[512] = ppc_opc_vsububm;
    ppc_opc_table_groupv[513] = ppc_opc_vavgub;
    ppc_opc_table_groupv[514] = ppc_opc_vand;
    ppc_opc_table_groupv[517] = ppc_opc_vmaxfp;
    ppc_opc_table_groupv[518] = ppc_opc_vslo;
    ppc_opc_table_groupv[544] = ppc_opc_vsubuhm;
    ppc_opc_table_groupv[545] = ppc_opc_vavguh;
    ppc_opc_table_groupv[546] = ppc_opc_vandc;
    ppc_opc_table_groupv[549] = ppc_opc_vminfp;
    ppc_opc_table_groupv[550] = ppc_opc_vsro;
    ppc_opc_table_groupv[576] = ppc_opc_vsubuwm;
    ppc_opc_table_groupv[577] = ppc_opc_vavguw;
    ppc_opc_table_groupv[578] = ppc_opc_vor;
    ppc_opc_table_groupv[610] = ppc_opc_vxor;
    ppc_opc_table_groupv[641] = ppc_opc_vavgsb;
    ppc_opc_table_groupv[642] = ppc_opc_vnor;
    ppc_opc_table_groupv[673] = ppc_opc_vavgsh;
    ppc_opc_table_groupv[704] = ppc_opc_vsubcuw;
    ppc_opc_table_groupv[705] = ppc_opc_vavgsw;
    ppc_opc_table_groupv[768] = ppc_opc_vsububs;
    ppc_opc_table_groupv[770] = ppc_opc_mfvscr;
    ppc_opc_table_groupv[772] = ppc_opc_vsum4ubs;
    ppc_opc_table_groupv[800] = ppc_opc_vsubuhs;
    ppc_opc_table_groupv[802] = ppc_opc_mtvscr;
    ppc_opc_table_groupv[804] = ppc_opc_vsum4shs;
    ppc_opc_table_groupv[832] = ppc_opc_vsubuws;
    ppc_opc_table_groupv[836] = ppc_opc_vsum2sws;
    ppc_opc_table_groupv[896] = ppc_opc_vsubsbs;
    ppc_opc_table_groupv[900] = ppc_opc_vsum4sbs;
    ppc_opc_table_groupv[928] = ppc_opc_vsubshs;
    ppc_opc_table_groupv[960] = ppc_opc_vsubsws;
    ppc_opc_table_groupv[964] = ppc_opc_vsumsws;
}

// main opcode 04
static void ppc_opc_group_v(e500_core_t *current_core)
{
    uint32 ext = PPC_OPC_EXT(current_core->current_opc);

    if (current_core->speSupport)
    {
        ppc_opc_group_efx_evx(current_core);
        return;
#if 0
        if (current_core->msr & MSR_SPE)
        {
            ppc_opc_group_efx_evx();
            return;
        } else
        {
            printf("In %s, MSR Not Enable SPE! Current PC = %x\n", __func__, current_core->pc);
            return;
        }
#endif
    }
#ifndef  __VEC_EXC_OFF__
    if ((current_core->msr & MSR_VEC) == 0)
    {
        ppc_exception(current_core, PPC_EXC_NO_VEC, 0, 0);
        return;
    }
#endif
    switch (ext & 0x1f)
    {
        case 16:
            if (current_core->current_opc & PPC_OPC_Rc)
                return ppc_opc_vmhraddshs(current_core);
            else
                return ppc_opc_vmhaddshs(current_core);
        case 17:
            return ppc_opc_vmladduhm(current_core);
        case 18:
            if (current_core->current_opc & PPC_OPC_Rc)
                return ppc_opc_vmsummbm(current_core);
            else
                return ppc_opc_vmsumubm(current_core);
        case 19:
            if (current_core->current_opc & PPC_OPC_Rc)
                return ppc_opc_vmsumuhs(current_core);
            else
                return ppc_opc_vmsumuhm(current_core);
        case 20:
            if (current_core->current_opc & PPC_OPC_Rc)
                return ppc_opc_vmsumshs(current_core);
            else
                return ppc_opc_vmsumshm(current_core);
        case 21:
            if (current_core->current_opc & PPC_OPC_Rc)
                return ppc_opc_vperm(current_core);
            else
                return ppc_opc_vsel(current_core);
        case 22:
            return ppc_opc_vsldoi(current_core);
        case 23:
            if (current_core->current_opc & PPC_OPC_Rc)
                return ppc_opc_vnmsubfp(current_core);
            else
                return ppc_opc_vmaddfp(current_core);
    }
    switch (ext & 0x1ff)
    {
        case 3:
            return ppc_opc_vcmpequbx(current_core);
        case 35:
            return ppc_opc_vcmpequhx(current_core);
        case 67:
            return ppc_opc_vcmpequwx(current_core);
        case 99:
            return ppc_opc_vcmpeqfpx(current_core);
        case 227:
            return ppc_opc_vcmpgefpx(current_core);
        case 259:
            return ppc_opc_vcmpgtubx(current_core);
        case 291:
            return ppc_opc_vcmpgtuhx(current_core);
        case 323:
            return ppc_opc_vcmpgtuwx(current_core);
        case 355:
            return ppc_opc_vcmpgtfpx(current_core);
        case 387:
            return ppc_opc_vcmpgtsbx(current_core);
        case 419:
            return ppc_opc_vcmpgtshx(current_core);
        case 451:
            return ppc_opc_vcmpgtswx(current_core);
        case 483:
            return ppc_opc_vcmpbfpx(current_core);
    }

    if (ext >= (sizeof ppc_opc_table_groupv / sizeof ppc_opc_table_groupv[0]))
    {
        return ppc_opc_invalid(current_core);
    }
    return ppc_opc_table_groupv[ext] (current_core);
}

static ppc_opc_function ppc_opc_table_main[64] = {
    &ppc_opc_invalid,                   //  0
    &ppc_opc_invalid,                   //  1
    &ppc_opc_invalid,                   //  2  (tdi on 64 bit platforms)
    &ppc_opc_twi,                       //  3
    //&ppc_opc_group_efx_evx,       //  4  (altivec group 1)
    &ppc_opc_group_v,
    &ppc_opc_invalid,                   //  5
    &ppc_opc_invalid,                   //  6
    &ppc_opc_mulli,                     //  7
    &ppc_opc_subfic,                    //  8
    &ppc_opc_invalid,                   //  9
    &ppc_opc_cmpli,                     // 10
    &ppc_opc_cmpi,                      // 11
    &ppc_opc_addic,                     // 12
    &ppc_opc_addic_,                    // 13
    &ppc_opc_addi,                      // 14
    &ppc_opc_addis,                     // 15
    &ppc_opc_bcx,                       // 16
    &ppc_opc_sc,                        // 17
    &ppc_opc_bx,                        // 18
    &ppc_opc_group_1,                   // 19
    &ppc_opc_rlwimix,                   // 20
    &ppc_opc_rlwinmx,                   // 21
    &ppc_opc_invalid,                   // 22
    &ppc_opc_rlwnmx,                    // 23
    &ppc_opc_ori,                       // 24
    &ppc_opc_oris,                      // 25
    &ppc_opc_xori,                      // 26
    &ppc_opc_xoris,                     // 27
    &ppc_opc_andi_,                     // 28
    &ppc_opc_andis_,                    // 29
    &ppc_opc_invalid,                   // 30  (group_rld on 64 bit platforms)
    &ppc_opc_group_2,                   // 31
    &ppc_opc_lwz,                       // 32
    &ppc_opc_lwzu,                      // 33
    &ppc_opc_lbz,                       // 34
    &ppc_opc_lbzu,                      // 35
    &ppc_opc_stw,                       // 36
    &ppc_opc_stwu,                      // 37
    &ppc_opc_stb,                       // 38
    &ppc_opc_stbu,                      // 39
    &ppc_opc_lhz,                       // 40
    &ppc_opc_lhzu,                      // 41
    &ppc_opc_lha,                       // 42
    &ppc_opc_lhau,                      // 43
    &ppc_opc_sth,                       // 44
    &ppc_opc_sthu,                      // 45
    &ppc_opc_lmw,                       // 46
    &ppc_opc_stmw,                      // 47
    &ppc_opc_lfs,                       // 48
    &ppc_opc_lfsu,                      // 49
    &ppc_opc_lfd,                       // 50
    &ppc_opc_lfdu,                      // 51
    &ppc_opc_stfs,                      // 52
    &ppc_opc_stfsu,                     // 53
    &ppc_opc_stfd,                      // 54
    &ppc_opc_stfdu,                     // 55
    &ppc_opc_invalid,                   // 56
    &ppc_opc_invalid,                   // 57
    &ppc_opc_invalid,                   // 58  (ld on 64 bit platforms)
    &ppc_opc_group_f1,                  // 59
    &ppc_opc_invalid,                   // 60
    &ppc_opc_invalid,                   // 61
    &ppc_opc_invalid,                   // 62
    &ppc_opc_group_f2,                  // 63
};

void FASTCALL ppc_exec_opc(e500_core_t * core)
{
    uint32 mainopc = PPC_OPC_MAIN(core->current_opc);

    //7c 00 38 ce 0111 11 00 1f 31
    //printf("pc %x, %x, %x\n", core->pc, mainopc, ((core->current_opc)>>1 &0x3ff));
    ppc_opc_table_main[mainopc] (core);
}

ppc_opc_function get_opc_function(e500_core_t *current_core, uint32_t instr)
{
    uint32_t mainopc;
    uint32_t vle_mainopc;
    uint32_t InsnBitsNum;

    InsnBitsNum = InsnBitsNumJudge(instr);

    if(current_core->VLE_Enable == 0)
    {
        mainopc = PPC_OPC_MAIN(instr);
        return ppc_opc_table_main[mainopc];
    } else
    {
        if(InsnBitsNum == 16)
        {
            /*SE instruction */
            ppc_vle_se_instr_analysis_excute(current_core, instr, RETURN_FUNC_PTR);
        } else
        {
            vle_mainopc = PPC_VLE_OPC_MAIN(instr);
            //printf("in func %s, vle_mainopc: %d\n",  __FUNCTION__, vle_mainopc);
            return ppc_vle_opc_table_main[vle_mainopc];
        }
    }
}

void ppc_dec_init(e500_core_t * core)
{
    ppc_opc_init_group2(core);
    ppc_opc_init_groupv();
    ppc_opc_init_group_efx_evx(core);

    ppc_vle_opc_init_group2(core);

    if (!strcmp(core->obj->class_name, "e600_core"))
    {
        ppc_opc_table_main[17] = &ppc_opc_sc_e600;
    }

    if ((ppc_cpu_get_pvr(core) & 0xffff0000) == 0x000c0000)
    {
        ppc_opc_table_main[4] = ppc_opc_group_v;
        ppc_opc_init_groupv();
    }
}

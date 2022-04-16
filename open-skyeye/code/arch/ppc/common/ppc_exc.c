/*
 *      PearPC
 *      ppc_exc.cc
 *
 *      Copyright (C) 2003 Sebastian Biallas (sb@biallas.net)
 *      Copyright (C) 2004 Daniel Foesch (dfoesch@cs.nmsu.edu)
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

/*      Pages marked: v.???
 *      From: IBM PowerPC MicroProcessor Family: Altivec(tm) Technology...
 *              Programming Environments Manual
 */

/*
 *  Modified by Michael.Kang to implement E500 core
 */

//#include "tools/snprintf.h"
//#include "debug/tracers.h"
//#include "cpu/debug.h"
//#include "info.h"
#include "ppc_cpu.h"
#include "ppc_exc.h"
#include "ppc_mmu.h"
#include "tracers.h"
#include "skyeye.h"

/* Machine Check Syndrome Register (MCSR) Field Descriptions
 * 32 MCP Machine check input signal
 * 33 ICPERR Instruction cache parity error
 * 34 DCP_PERR Data cache push parity error
 * 35 DCPERR Data cache parity error
 * 36每55 〞 Reserved, should be cleared.
 * 56 BUS_IAERR Bus instruction address error
 * 57 BUS_RAERR Bus read address error
 * 58 BUS_WAERR Bus write address error
 * 59 BUS_IBERR Bus instruction data bus error
 * 60 BUS_RBERR Bus read data bus error
 * 61 BUS_WBERR Bus write bus error
 * 62 BUS_IPERR Bus instruction parity error
 * 63 BUS_RPERR Bus read parity error
 */

/*
 * IVOR0  Critical Input
 * IVOR1  Machine Check
 * IVOR2  Data Storage
 * IVOR3  Instruction Storage
 * IVOR4  External Input
 * IVOR5  Alignment
 * IVOR6  Program
 * IVOR7  Floating-Point Unavailable
 * IVOR8  System Call
 * IVOR9  Auxiliary Processor Unavailable
 * IVOR10 Decrementer
 * IVOR11 Fixed-Interval Timer Interrupt
 * IVOR12 Watchdog Timer Interrupt
 * IVOR13 Data TLB Error
 * IVOR14 Instruction TLB Error
 * IVOR15 Debug
 */
/*
 *      .247
 */

bool_t e500_ppc_exception(e500_core_t * core, uint32 type, uint32 flags, uint32 a)
{
    switch (type)
    {
        case CRI_INPUT:
            /*Critical Input Interrupt: ME is unchanged. All other MSR bits are cleared */
            core->srr[0] = core->pc;
            core->srr[1] = core->msr;
            core->msr &= MSR_ME;
            break;
        case MACH_CHECK:
            core->mcsrr[0] = core->pc;
            core->mcsrr[1] = core->msr;
            /*FIXME not implement MCAR and MCSR */
            /*MCAR When a machine check interrupt is taken, the machine check address register is updated with the address of the data associated with the machine check */
            break;
        case DATA_ST:
            core->srr[0] = core->pc;
            core->srr[1] = core->msr;
            /* ESR_DST         0x00800000       Storage Exception - Data miss */
            core->esr = 0x00800000;
            core->dear = a;             /* save the data address accessed by exception instruction */
            /* CE,ME,DE is unchanged, other bits should be clear */
            core->msr &= (MSR_CE | MSR_DE | MSR_ME);
            break;
        case INSN_ST:
            core->srr[0] = core->pc;
            core->srr[1] = core->msr;
            /* CE,ME,DE is unchanged, other bits should be clear */
            core->msr &= (MSR_CE | MSR_DE | MSR_ME);
            /*BO is set if the instruction fetch caused a byte-ordering exception; */
            core->esr = (1 << 14);
        case EXT_INT:
            //printf("In %s,EXT_INT\n", __FUNCTION__);
            core->srr[0] = core->pc;
            core->srr[1] = core->msr;
            /* CE,ME,DE is unchanged, other bits should be clear */
            core->msr &= (MSR_CE | MSR_DE | MSR_ME);
            break;
        case PROG:
            core->srr[0] = core->pc;
            core->srr[1] = core->msr;
            /* CE,ME,DE is unchanged, other bits should be clear */
            core->msr &= (MSR_CE | MSR_DE | MSR_ME);
            core->esr |= 1 << 25;
            break;
        case ALIGN:
            core->srr[0] = core->pc;
            core->srr[1] = core->msr;
            /* CE,ME,DE is unchanged, other bits should be clear */
            core->msr &= (MSR_CE | MSR_DE | MSR_ME);
            /* Set to the EA of a byte that is both within the range of the bytes being accessed by the memory access or cache management instruction, and within the page whose access caused the alignment exception */
            core->dear = a;
            /* SPE(bit 24) Set
             * ST is set if the instruction causing the exception is a store
             * ST is cleared for a load*/
            /*all other defined ESR bits are cleared */
            core->esr = (1 << 24);
            break;
        case FP_UN:
            fprintf(stderr, "E500 not support exception type %d, pc=0x%x.\n", type, core->pc);
            skyeye_log(Warning_log, __FUNCTION__, "E500 not support exception type %d, pc=0x%x\n", type, core->pc);
            return False;
        case SYSCALL:
            //printf(" In %s, SYSCALL exp happened,r0=%d, pc=0x%x,\n", __FUNCTION__, core->gpr[0],core->pc);
            core->srr[0] = core->npc;
            core->srr[1] = core->msr;

            /*  CE, ME, and DE are unchanged. All other MSR bits are cleared. */
            core->msr &= (MSR_CE | MSR_DE | MSR_ME);
            break;
        case AP_UN:
            fprintf(stderr, "E500 not support exception type %d, pc=0x%x.\n", type, core->pc);
            skyeye_log(Warning_log, __FUNCTION__, "E500 not support exception type %d, pc=0x%x\n", type, core->pc);
            return False;
        case DEC:
            core->srr[0] = core->npc;
            core->srr[1] = core->msr;

            /* CE,ME and DE bit unchanged, other bit should be clear */
            core->msr &= (MSR_CE | MSR_DE | MSR_ME);

            /* DIS(28) bit is set */
            core->tsr |= (1 << 28);
            //printf("In %s, timer interrupt happened.\n", __FUNCTION__);
            break;
        case FIT:
            core->srr[0] = core->npc;
            core->srr[1] = core->msr;

            /* CE,ME and DE bit unchanged, other bit should be clear */
            core->msr &= (MSR_CE | MSR_DE | MSR_ME);

            /* FIS(27) bit is set */
            core->tsr |= (1 << 27);
            break;
        case WD:
            core->csrr[0] = core->npc;
            core->csrr[1] = core->msr;

            /* CE,ME and DE bit unchanged, other bit should be clear */
            core->msr &= (MSR_ME);

            /* WIS(30) bit is set */
            core->tsr |= (1 << 30);
            break;
        case DATA_TLB:
            core->srr[0] = core->pc;
            core->srr[1] = core->msr;
            //core->esr |= ST;
            core->dear = a;             /* save the data address accessed by exception instruction */
            core->msr &= (MSR_CE | MSR_DE | MSR_ME);

            /* Update TLB */

                        /**
			 * if TLBSELD = 00, MAS0[ESEL] is updated with the next victim information for TLB0.Finially, 		      * the MAS[0] field is updated with the incremented value of TLB0[NV].Thus, ESEL points to 
			 * the current victim
			 * (the entry to be replaced), while MAS0[NV] points to the next victim to be used if a TLB0 		      * entry is replaced
			 */

                        /**
			 * update TLBSEL with TLBSELD
			 */
            core->mmu.mas[0] = (core->mmu.mas[4] & 0x10000000) | (core->mmu.mas[0] & (~0x10000000));
            /* if TLBSELD == 0, update ESEL and NV bit in MAS Register */
            if (!TLBSELD(core->mmu.mas[4]))
            {
                /* if TLBSELD == 0, ESEL = TLB[0].NV */
#if 0
                /* update ESEL of MAS0 */
                static int tlb0_nv = 0;

                //offset = ((tlb0_nv & 0x1) << 7) | (EPN(core->mmu.mas[2]) & 0x7f);
                core->mmu.tlb0_nv = tlb0_nv;
                if (tlb0_nv == 0xF)
                    tlb0_nv = 0;
                tlb0_nv++;
#endif
                core->mmu.mas[0] = (core->mmu.tlb0_nv << 18) | (core->mmu.mas[0] & 0xFFF0FFFF);
                /* update NV of MAS0 , NV = ~TLB[0].NV */
                core->mmu.mas[0] = (~core->mmu.tlb0_nv & 0x3) | (core->mmu.mas[0] & 0xFFFFFFFC);
            }

                        /**
			 *  set zeros of permis and U0 - U3
			 */
            core->mmu.mas[3] &= 0xFFFFFC00;

                        /**
			 *  set zeros of RPN
			 */
            core->mmu.mas[3] &= 0xFFF;

                        /**
			 * Set EPN to EPN of access
			 */
            core->mmu.mas[2] = (a & 0xFFFFF000) | (core->mmu.mas[2] & 0xFFF);

                        /**
			 * Set TSIZE[0 - 3] to TSIZED
			 */
            core->mmu.mas[1] = (core->mmu.mas[4] & 0xF00) | (core->mmu.mas[1] & 0xFFFFF0FF);

                        /**
			 * Set TID
			 */
            core->mmu.mas[1] = (core->mmu.mas[1] & 0xFF00FFFF) | ((core->mmu.pid[0] & 0xFF) << 16);

                        /**
			 * set Valid bit
			 */
            core->mmu.mas[1] = core->mmu.mas[1] | 0x80000000;
            /* update SPID with PID */
            core->mmu.mas[6] = (core->mmu.mas[6] & 0xFF00FFFF) | ((core->mmu.pid[0] & 0xFF) << 16);
            if (flags == PPC_MMU_WRITE)
                core->esr = 0x00800000;
            else
                core->esr = 0x0;
            break;
        case INSN_TLB:
            core->srr[0] = core->pc;
            core->srr[1] = core->msr;

            core->msr &= (MSR_CE | MSR_DE | MSR_ME);

                        /**
			 * if TLBSELD = 00, MAS0[ESEL] is updated with the next victim information for TLB0.Finially, 		      * the MAS[0] field is updated with the incremented value of TLB0[NV].Thus, ESEL points to 
			 * the current victim
			 * (the entry to be replaced), while MAS0[NV] points to the next victim to be used if a TLB0 		      * entry is replaced
			 */

                        /**
			 * update TLBSEL with TLBSELD
			 */
            core->mmu.mas[0] = (core->mmu.mas[4] & 0x10000000) | (core->mmu.mas[0] & (~0x10000000));
            /* if TLBSELD == 0, update ESEL and NV bit in MAS Register */
            if (!TLBSELD(core->mmu.mas[4]))
            {
                /* if TLBSELD == 0, ESEL = TLB[0].NV */
#if 0
                /* update ESEL of MAS0 */
                static int tlb0_nv = 0;

                //offset = ((tlb0_nv & 0x1) << 7) | (EPN(core->mmu.mas[2]) & 0x7f);
                core->mmu.tlb0_nv = tlb0_nv;
                if (tlb0_nv == 0xF)
                    tlb0_nv = 0;
                tlb0_nv++;
#endif
                core->mmu.mas[0] = (core->mmu.tlb0_nv << 18) | (core->mmu.mas[0] & 0xFFF0FFFF);
                /* update NV of MAS0 , NV = ~TLB[0].NV */
                core->mmu.mas[0] = (~core->mmu.tlb0_nv & 0x3) | (core->mmu.mas[0] & 0xFFFFFFFC);
            }

                        /**
			 *  set zeros of permis and U0 - U3
			 */
            core->mmu.mas[3] &= 0xFFFFFC00;

                        /**
			 *  set zeros of RPN
			 */
            core->mmu.mas[3] &= 0xFFF;

                        /**
			 * Set EPN to EPN of access
			 */
            core->mmu.mas[2] = (a & 0xFFFFF000) | (core->mmu.mas[2] & 0xFFF);

                        /**
			 * Set TSIZE[0 - 3] to TSIZED
			 */
            core->mmu.mas[1] = (core->mmu.mas[4] & 0xF00) | (core->mmu.mas[1] & 0xFFFFF0FF);

                        /**
			 * Set TID
			 */
            core->mmu.mas[1] = (core->mmu.mas[1] & 0xFF00FFFF) | ((core->mmu.pid[0] & 0xFF) << 16);
            /* update SPID with PID */
            core->mmu.mas[6] = (core->mmu.mas[6] & 0xFF00FFFF) | ((core->mmu.pid[0] & 0xFF) << 16);

            break;
        case DEBUG:
            core->csrr[0] = core->npc;
            core->csrr[1] = core->msr;
            /* ME bit unchanged, RI is not cleared, other bit should be clear */
            core->msr &= MSR_ME;
            core->dbsr = type;
            break;
        case SPE_UNAVAILABLE_INT:
            core->srr[0] = core->npc;
            core->srr[1] = core->msr;
            /* ME CE DE bit unchanged, other bit should be clear */
            core->msr &= (MSR_ME | MSR_CE | MSR_DE);
            /*ESR[SPV] is set */
            core->esr |= ESR_SPV;
            break;
        case SPE_DATA_INT:
            core->srr[0] = core->npc;
            core->srr[1] = core->msr;
            /* ME CE DE bit unchanged, other bit should be clear */
            core->msr &= (MSR_ME | MSR_CE | MSR_DE);
            /*ESR[SPV] is set */
            core->esr |= ESR_SPV;
            /*SPEFSCR[FG,FGH, FX, FXH] are cleared */
            core->spefscr.FG = 0;
            core->spefscr.FGH = 0;
            core->spefscr.FX = 0;
            core->spefscr.FXH = 0;
            break;
        case SPE_ROUND_INT:
            core->srr[0] = core->npc;
            core->srr[1] = core->msr;
            /* ME CE DE bit unchanged, other bit should be clear */
            core->msr &= (MSR_ME | MSR_CE | MSR_DE);
            /*ESR[SPV] is set */
            core->esr |= ESR_SPV;
            /*SPEFSCR[FINXS] are set */
            core->spefscr.FINXS = 1;
            break;
        default:
            fprintf(stderr, "E500 Unknown exception type %d.pc=0x%x\n", type, core->pc);
            skyeye_log(Error_log, __FUNCTION__, "E500 not support exception type %d, pc=0x%x\n", type, core->pc);
            return False;
    }

    if(core->hardware_vector_mode == 0)
    {
        core->npc = (core->ivpr & 0xFFFF0000) | (core->ivor[type] & 0xFFF0);  /* software vector mode */
    } else
    {
        core->npc = (core->ivpr & 0xFFFF0000) | core->hardware_vector_offset; /* hardware vector mode */
    }

    return True;
}

#define ST (1 << 24)

bool_t e600_ppc_exception(e500_core_t * core, uint32 type, uint32 flags, uint32 a)
{
    if (type != PPC_EXC_DEC)
        PPC_EXC_TRACE("@%08x: type = %08x (%08x, %08x)\n", core->pc, type, flags, a);
    switch (type)
    {
        case PPC_EXC_SYS_RESET:
            {
                core->srr[0] = core->pc;
                /*0每5 Cleared
                 *6 Loaded with equivalent MSR bit
                 *7每15 Cleared
                 *16每31 Loaded with equivalent MSR bits
                 **/
                core->srr[1] = core->msr & 0xffff0020;
                break;
            }
        case PPC_EXC_DSI:
            {                           // .271
                core->srr[0] = core->pc;
                core->srr[1] = core->msr & 0x87c0ffff;
                core->dar = a;
                core->dsisr = flags;
                printf("In %s, addr=0x%x, pc=0x%x DSI exception.\n", __FUNCTION__, a, core->pc);
                break;
            }
        case PPC_EXC_ISI:
            {                           // .274
                if (core->pc == 0)
                {
                    PPC_EXC_WARN("pc == 0 in ISI\n");
                    //SINGLESTEP("");
                }
                core->srr[0] = core->pc;
                core->srr[1] = (core->msr & 0x87c0ffff) | flags;
                break;
            }
        case PPC_EXC_DEC:
            {                           // .284
                core->srr[0] = core->npc;
                core->srr[1] = core->msr & 0x87c0ffff;
                break;
            }
        case PPC_EXC_EXT_INT:
            {
                core->srr[0] = core->pc;
                core->srr[1] = core->msr & 0x87c0ffff;
                break;
            }
        case PPC_EXC_SC:
            {                           // .285
                core->srr[0] = core->npc;
                core->srr[1] = core->msr & 0x87c0ffff;
                break;
            }
        case PPC_EXC_NO_FPU:
            {                           // .284
                core->srr[0] = core->pc;
                core->srr[1] = core->msr & 0x87c0ffff;
                break;
            }
        case PPC_EXC_NO_VEC:
            {                           // v.41
                core->srr[0] = core->pc;
                core->srr[1] = core->msr & 0x87c0ffff;
                break;
            }
        case PPC_EXC_PROGRAM:
            {                           // .283
                if (flags & PPC_EXC_PROGRAM_NEXT)
                {
                    core->srr[0] = core->npc;
                } else
                {
                    core->srr[0] = core->pc;
                }
                core->srr[1] = (core->msr & 0x87c0ffff) | flags;
                break;
            }
        case PPC_EXC_FLOAT_ASSIST:
            {
                // .288
                core->srr[0] = core->pc;
                core->srr[1] = core->msr & 0x87c0ffff;
                break;
            }
        case PPC_EXC_MACHINE_CHECK:
            {
                // .270
                if (!(core->msr & MSR_ME))
                {
                    PPC_EXC_ERR("machine check exception and MSR[ME]=0.\n");
                }
                core->srr[0] = core->pc;
                core->srr[1] = (core->msr & 0x87c0ffff) | MSR_RI;
                break;
            }
        case PPC_EXC_TRACE2:
            {                           // .286
                core->srr[0] = core->npc;
                core->srr[1] = core->msr & 0x87c0ffff;
                break;
            }
        default:
            fprintf(stderr, "E600 Unknown exception type %d.pc=0x%x\n", type, core->pc);
            skyeye_log(Error_log, __FUNCTION__, "E600 not support exception type %d, pc=0x%x\n", type, core->pc);
            return False;
    }
    ppc_mmu_tlb_invalidate(core);
    core->msr = (MSR_ME | MSR_IP);
    core->npc = type;
    core->exception_status = 1;

    return True;
}

void ppc_cpu_raise_ext_exception()
{
    ppc_cpu_atomic_raise_ext_exception();
}

void ppc_cpu_cancel_ext_exception()
{
    ppc_cpu_atomic_cancel_ext_exception();
}

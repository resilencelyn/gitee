#include <stdio.h>
#include "emul.h"
#include "instr16.h"
#include "mips_regformat.h"
#include "skyeye_pref.h"
#include "skyeye_thread.h"

extern FILE *skyeye_logfd;

/* This monster of a switch statement decodes all CPU instructions. I've
 * decided against an explicit jump table, as such implementation both
 * increases the code size and introduced additional overhead to the inner
 * decoder loop.
 */

/*  WARNING: The below code currently does not simulate any slips.
 * This should be fixed soon (it's fairly easy to simulate, too.)
 */

static inline UInt32 Xlat(UInt32 i)
{
    return i < 2 ? i + 16 : i;
}

int decode16(MIPS_State * mstate, Instr instr32)
{
    conf_object_t *obj = mstate->obj;
    bool_t isExtended = 0;
    UInt16 Extendedimm = 0;
    Instr16 Highinstr = instr32 & 0xFFFF;
    UInt32 HighOp = opcode(Highinstr);

    if (HighOp == EXTEND || HighOp == JAL)
    {
        if (mstate->pipeline == branch_delay)
            printf("Can't handle Extend or JAL in branch delay slot\n");

        if (HighOp == EXTEND)
        {
            isExtended = 1;
            Extendedimm = Highinstr;
            // Inc PC since we have already loaded next 2 byte.
            mstate->pc += 2;
        } else if (HighOp == JAL)
        {
            // Jump And Link
            mstate->gpr[31] = clear_bit(mstate->pc + 6, 0) | mstate->isa_mode;
            VA msb = clear_bits(mstate->pc + 4, 27, 0);

            mstate->branch_target =
                msb | (((bits(Highinstr, 4, 0) << 21) | (bits(Highinstr, 9, 5) << 16) | (instr32 >> 16)) << 2);
            // Keep isa_mode for JAL in MIPS16e
            if (!IS_JALX(Highinstr))
                mstate->branch_target = set_bit(mstate->branch_target, 0);
            // Inc PC since we have already loaded next 2 byte.
            mstate->pc += 2;

            return branch_delay;
        }
    }
    // Handle normal instructions
    Instr16 instr;

    if (isExtended)
        instr = instr32 >> 16;
    else
        instr = instr32 & 0xFFFF;
    UInt32 Op = opcode(instr);

    switch (Op)
    {
        case ADDIUSP:
            {
                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    mstate->gpr[Xlat(rx(instr))] = mstate->gpr[29] + sign_extend_UInt32(Extoffset, 16);
                } else
                {
                    mstate->gpr[Xlat(rx(instr))] = mstate->gpr[29] + (imm_ri(instr) << 2);
                }
                return nothing_special;
            }
        case ADDIUPC:
            {
                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    mstate->gpr[Xlat(rx(instr))] = clear_bits(mstate->pc - 2, 1, 0) + sign_extend_UInt32(Extoffset, 16);
                } else
                {
                    VA base_pc = mstate->pipeline == branch_delay ? mstate->prev_pc : mstate->pc;

                    mstate->gpr[Xlat(rx(instr))] = clear_bits(base_pc + (imm_ri(instr) << 2), 1, 0);
                }

                return nothing_special;
            }
        case B:
            {
                // Jump
                VA offset;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    offset = sign_extend_UInt32(Extoffset << 1, 17);
                } else
                    offset = sign_extend_UInt32(offset(instr) << 1, 12);

                if (mstate->pipeline == branch_delay)
                {
                    printf("[B]Can't handle branch in branch delay slot");
                }
                // We have already inc the pc of Extend instruction
                mstate->pc += 2 + offset;
                mstate->pipeline = branch_nodelay;
                return nothing_special;
            }
        case BEQZ:
            {
                // Branch On Equal to zero
                VA offset;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    offset = sign_extend_UInt32(Extoffset << 1, 17);
                } else
                {
                    offset = sign_extend_UInt32(imm_ri(instr) << 1, 9);
                }

                if (mstate->pipeline == branch_delay)
                {
                    printf("[BEQZ]Can't handle branch in branch delay slot");
                }
                if (mstate->gpr[Xlat(rx(instr))] == 0)
                {
                    // We have already inc the pc of Extend instruction
                    mstate->pc += 2 + offset;
                    mstate->pipeline = branch_nodelay;
                }
                return nothing_special;
            }
        case BNEZ:
            {
                // Branch On Not Equal to zero
                VA offset;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    offset = sign_extend_UInt32(Extoffset << 1, 17);
                } else
                {
                    offset = sign_extend_UInt32(imm_ri(instr) << 1, 9);
                }

                if (mstate->pipeline == branch_delay)
                {
                    printf("[BNEZ]Can't handle branch in branch delay slot");
                }
                if (mstate->gpr[Xlat(rx(instr))] != 0)
                {
                    // We have already inc the pc of Extend instruction
                    mstate->pc += 2 + offset;
                    mstate->pipeline = branch_nodelay;
                }
                return nothing_special;
            }
        case SHIFT:
            {
                switch (funct_shift(instr))
                {
                    case SLL:
                        {
                            // Shift Left Logical
                            if (isExtended)
                            {
                                mstate->gpr[Xlat(rx(instr))] = mstate->gpr[Xlat(ry(instr))] << bits(Extendedimm, 10, 6);
                            } else
                            {
                                UInt32 x = shift_sa(instr);

                                if (x == 0)
                                    x = 8;
                                mstate->gpr[Xlat(rx(instr))] = mstate->gpr[Xlat(ry(instr))] << x;
                            }
                            return nothing_special;
                        }
                    case SRL:
                        {
                            // Shift Right Logical
                            if (isExtended)
                            {
                                mstate->gpr[Xlat(rx(instr))] = mstate->gpr[Xlat(ry(instr))] >> bits(Extendedimm, 10, 6);
                            } else
                            {
                                UInt32 x = shift_sa(instr);

                                if (x == 0)
                                    x = 8;
                                mstate->gpr[Xlat(rx(instr))] = mstate->gpr[Xlat(ry(instr))] >> x;
                            }
                            return nothing_special;
                        }
                    case SRA:
                        {
                            // Shift Right Arithmetic
                            if (isExtended)
                            {
                                UInt32 x = bits(Extendedimm, 10, 6);

                                mstate->gpr[Xlat(rx(instr))] =
                                    sign_extend_UInt32(mstate->gpr[Xlat(ry(instr))] >> x, 32 - x);
                            } else
                            {
                                UInt32 x = shift_sa(instr);

                                if (x == 0)
                                    x = 8;
                                mstate->gpr[Xlat(rx(instr))] =
                                    sign_extend_UInt32(mstate->gpr[Xlat(ry(instr))] >> x, 32 - x);
                            }
                            return nothing_special;
                        }
                    default:
                        // Reserved instruction.
                        process_reserved_instruction(mstate);
                        return nothing_special;
                }
            }
        case RRIA:
            {
                if (funct_rria(instr) == ADDIU)
                {
                    if (isExtended)
                    {
                        VA Extoffset = (imm_rria(Extendedimm) << 11) |
                            (bits(Extendedimm, 10, 4) << 4) | imm_rria(instr);
                        mstate->gpr[Xlat(ry(instr))] = mstate->gpr[Xlat(rx(instr))] + sign_extend_UInt32(Extoffset, 15);
                    } else
                    {
                        mstate->gpr[Xlat(ry(instr))] = mstate->gpr[Xlat(rx(instr))] +
                            sign_extend_UInt32(imm_rria(instr), 4);
                    }
                } else
                    process_reserved_instruction(mstate);
                return nothing_special;
            }
        case ADDIU8:
            {
                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    mstate->gpr[Xlat(rx(instr))] += sign_extend_UInt32(Extoffset, 16);
                } else
                {
                    mstate->gpr[Xlat(rx(instr))] += sign_extend_UInt32(imm_ri(instr), 8);
                }
                return nothing_special;
            }
        case SLTI:
            {
                // Set On Less Than Immediate
                Int32 x = mstate->gpr[Xlat(rx(instr))];
                Int32 y;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    y = sign_extend_UInt32(Extoffset, 16);
                } else
                    y = imm_ri(instr);

                mstate->gpr[24] = (x < y);
                return nothing_special;
            }
        case SLTIU:
            {
                // Set On Less Than Immediate Unsigned
                UInt32 x = mstate->gpr[Xlat(rx(instr))];
                UInt32 y;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    y = sign_extend_UInt32(Extoffset, 16);
                } else
                    y = imm_ri(instr);

                mstate->gpr[24] = (x < y);
                return nothing_special;
            }
        case I8:
            {
                UInt32 fi = funct_i8(instr);

                switch (fi)
                {
                    default:
                        // Reserved instruction.
                        process_reserved_instruction(mstate);
                        return nothing_special;
                    case BTEQZ:
                        {
                            // Branch On T Equal to zero
                            VA offset;

                            if (isExtended)
                            {
                                VA Extoffset = (imm_rri(Extendedimm) << 11) |
                                    (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);
                                offset = sign_extend_UInt32(Extoffset << 1, 17);
                            } else
                            {
                                offset = sign_extend_UInt32(imm_ri(instr) << 1, 9);
                            }

                            if (mstate->pipeline == branch_delay)
                            {
                                printf("[BTEQZ]Can't handle branch in branch delay slot");
                            }
                            if (mstate->gpr[24] == 0)
                            {
                                // We have already inc the pc of Extend instruction
                                mstate->pc += 2 + offset;
                                mstate->pipeline = branch_nodelay;
                            }
                            return nothing_special;
                        }
                    case BTNEZ:
                        {
                            // Branch On T Not Equal to zero
                            VA offset;

                            if (isExtended)
                            {
                                VA Extoffset = (imm_rri(Extendedimm) << 11) |
                                    (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);
                                offset = sign_extend_UInt32(Extoffset << 1, 17);
                            } else
                            {
                                offset = sign_extend_UInt32(imm_ri(instr) << 1, 9);
                            }

                            if (mstate->pipeline == branch_delay)
                            {
                                printf("[BTNEZ]Can't handle branch in branch delay slot");
                            }
                            if (mstate->gpr[24] != 0)
                            {
                                // We have already inc the pc of Extend instruction
                                mstate->pc += 2 + offset;
                                mstate->pipeline = branch_nodelay;
                            }
                            return nothing_special;
                        }
                    case SWRASP:
                        {
                            // Store Word from ra(SP)
                            if (mstate->sync_bit)
                                sync();

                            Int32 offset;

                            if (isExtended)
                            {
                                VA Extoffset = (imm_rri(Extendedimm) << 11) |
                                    (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);
                                offset = sign_extend_UInt32(Extoffset, 16);
                            } else
                                offset = imm_ri(instr) << 2;

                            VA va = offset + mstate->gpr[29];

                            if (va & 0x3)       // Check alignment
                                process_address_error(mstate, data_load, va);
                            PA pa;

                            if (translate_vaddr(mstate, va, data_load, &pa) != TLB_SUCC)
                                return nothing_special; // Shi yang 2006-08-10

                            store(mstate, mstate->gpr[31], va, pa, 4);
                            return nothing_special;
                        }
                    case ADJSP:
                        {
                            if (isExtended)
                            {
                                VA Extoffset = (imm_rri(Extendedimm) << 11) |
                                    (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);
                                mstate->gpr[29] += sign_extend_UInt32(Extoffset, 16);
                            } else
                            {
                                mstate->gpr[29] += sign_extend_UInt32(imm_ri(instr) << 3, 11);
                            }
                            return nothing_special;
                        }
                    case SVRS:
                        {
                            if (svrs_s(instr) == RESTORE)
                            {
                                // RESTORE
                                if (isExtended)
                                {
                                    UInt32 framesize = (bits(Extendedimm, 7, 4) << 4) | svrs_framesize(instr);
                                    VA va = mstate->gpr[29] + (framesize << 3);
                                    VA va_adjust = va;
                                    PA pa;
                                    bool_t ra = svrs_ra(instr);
                                    bool_t s0 = svrs_s0(instr);
                                    bool_t s1 = svrs_s1(instr);
                                    UInt32 xsregs = bits(Extendedimm, 10, 8);
                                    UInt32 aregs = bits(Extendedimm, 3, 0);
                                    UInt32 astatic = 0;

                                    if (aregs == 1 || aregs == 5 || aregs == 9 || aregs == 13)
                                        astatic = 1;
                                    if (aregs == 2 || aregs == 6 || aregs == 10)
                                        astatic = 2;
                                    else if (aregs == 3 || aregs == 7)
                                        astatic = 3;
                                    else if (aregs == 11)
                                        astatic = 4;

                                    if ((ra || s0 || s1 || astatic) && bits(va, 1, 0))  // Check alignment
                                    {
                                        fprintf(stderr, " in %s,address unaligned va=0x%x,pc=0x%x\n",
                                                __FUNCTION__, va, mstate->pc);
                                        skyeye_exit(-1);
                                        process_address_error(mstate, data_store, va);
                                    }

                                    if (ra)
                                    {
                                        va -= 4;
                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        UInt32 y = 0;

                                        load(mstate, va, pa, &y, 4);
                                        mstate->gpr[31] = y;
                                    }
                                    switch (xsregs)
                                    {
                                        default:
                                            break;
                                        case 7:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                UInt32 y = 0;

                                                load(mstate, va, pa, &y, 4);
                                                mstate->gpr[30] = y;
                                            }
                                        case 6:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                UInt32 y = 0;

                                                load(mstate, va, pa, &y, 4);
                                                mstate->gpr[23] = y;
                                            }
                                        case 5:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                UInt32 y = 0;

                                                load(mstate, va, pa, &y, 4);
                                                mstate->gpr[22] = y;
                                            }
                                        case 4:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                UInt32 y = 0;

                                                load(mstate, va, pa, &y, 4);
                                                mstate->gpr[21] = y;
                                            }
                                        case 3:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                UInt32 y = 0;

                                                load(mstate, va, pa, &y, 4);
                                                mstate->gpr[20] = y;
                                            }
                                        case 2:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                UInt32 y = 0;

                                                load(mstate, va, pa, &y, 4);
                                                mstate->gpr[19] = y;
                                            }
                                        case 1:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                UInt32 y = 0;

                                                load(mstate, va, pa, &y, 4);
                                                mstate->gpr[18] = y;
                                            }
                                    }

                                    if (s0)
                                    {
                                        va -= 4;
                                        PA pa;

                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        UInt32 y = 0;

                                        load(mstate, va, pa, &y, 4);
                                        mstate->gpr[17] = y;
                                    }
                                    if (s1)
                                    {
                                        va -= 4;
                                        PA pa;

                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        UInt32 y = 0;

                                        load(mstate, va, pa, &y, 4);
                                        mstate->gpr[16] = y;
                                    }
                                    if (astatic > 0)
                                    {
                                        va -= 4;
                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        UInt32 y = 0;

                                        load(mstate, va, pa, &y, 4);
                                        mstate->gpr[7] = y;

                                        if (astatic > 1)
                                        {
                                            va -= 4;
                                            if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                return nothing_special; // Shi yang 2006-08-10
                                            UInt32 y = 0;

                                            load(mstate, va, pa, &y, 4);
                                            mstate->gpr[6] = y;
                                        }

                                        if (astatic > 2)
                                        {
                                            va -= 4;
                                            if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                return nothing_special; // Shi yang 2006-08-10
                                            UInt32 y = 0;

                                            load(mstate, va, pa, &y, 4);
                                            mstate->gpr[5] = y;
                                        }

                                        if (astatic > 3)
                                        {
                                            va -= 4;
                                            if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                return nothing_special; // Shi yang 2006-08-10
                                            UInt32 y = 0;

                                            load(mstate, va, pa, &y, 4);
                                            mstate->gpr[4] = y;
                                        }
                                    }
                                    // Adjustment stack
                                    mstate->gpr[29] = va_adjust;
                                } else
                                {
                                    VA va;
                                    UInt32 framesize = svrs_framesize(instr);

                                    if (framesize == 0)
                                        va = mstate->gpr[29] + 128;
                                    else
                                        va = mstate->gpr[29] + (framesize << 3);

                                    VA va_adjust = va;
                                    bool_t ra = svrs_ra(instr);
                                    bool_t s0 = svrs_s0(instr);
                                    bool_t s1 = svrs_s1(instr);

                                    if ((ra || s0 || s1) && bits(va, 1, 0))     // Check alignment
                                    {
                                        fprintf(stderr, " in %s,address unaligned va=0x%x,pc=0x%x\n",
                                                __FUNCTION__, va, mstate->pc);
                                        skyeye_exit(-1);
                                        process_address_error(mstate, data_store, va);
                                    }

                                    if (ra)
                                    {
                                        va -= 4;
                                        PA pa;

                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        UInt32 y = 0;

                                        load(mstate, va, pa, &y, 4);
                                        mstate->gpr[31] = y;
                                    }
                                    if (s0)
                                    {
                                        va -= 4;
                                        PA pa;

                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        UInt32 y = 0;

                                        load(mstate, va, pa, &y, 4);
                                        mstate->gpr[17] = y;
                                    }
                                    if (s1)
                                    {
                                        va -= 4;
                                        PA pa;

                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        UInt32 y = 0;

                                        load(mstate, va, pa, &y, 4);
                                        mstate->gpr[16] = y;
                                    }
                                    // Adjustment stack
                                    mstate->gpr[29] = va_adjust;
                                }
                                return nothing_special;
                            } else
                            {
                                // SAVE
                                if (isExtended)
                                {
                                    VA va = mstate->gpr[29];
                                    PA pa;
                                    bool_t ra = svrs_ra(instr);
                                    bool_t s0 = svrs_s0(instr);
                                    bool_t s1 = svrs_s1(instr);
                                    UInt32 xsregs = bits(Extendedimm, 10, 8);
                                    UInt32 aregs = bits(Extendedimm, 3, 0);
                                    UInt32 args = 0;

                                    if (aregs >= 4 && aregs <= 7)
                                        args = 1;
                                    else if (aregs >= 8 && aregs <= 10)
                                        args = 2;
                                    else if (aregs == 12 || aregs == 13)
                                        args = 3;
                                    else if (aregs == 14)
                                        args = 4;
                                    UInt32 astatic = 0;

                                    if (aregs == 1 || aregs == 5 || aregs == 9 || args == 13)
                                        astatic = 1;
                                    if (aregs == 2 || aregs == 6 || aregs == 10)
                                        astatic = 2;
                                    else if (aregs == 3 || aregs == 7)
                                        astatic = 3;
                                    else if (aregs == 11)
                                        astatic = 4;

                                    if ((ra || s0 || s1 || args || astatic) && bits(va, 1, 0))  // Check alignment
                                    {
                                        fprintf(stderr, " in %s,address unaligned va=0x%x,pc=0x%x\n",
                                                __FUNCTION__, va, mstate->pc);
                                        skyeye_exit(-1);
                                        process_address_error(mstate, data_store, va);
                                    }
                                    if (args > 0)
                                    {
                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        store(mstate, mstate->gpr[4], va, pa, 4);       // Fix me: Shi yang 2006-08-10

                                        if (args > 1)
                                        {
                                            if (translate_vaddr(mstate, va + 4, data_store, &pa) != TLB_SUCC)
                                                return nothing_special; // Shi yang 2006-08-10
                                            store(mstate, mstate->gpr[5], va, pa, 4);   // Fix me: Shi yang 2006-08-10
                                        }

                                        if (args > 2)
                                        {
                                            if (translate_vaddr(mstate, va + 8, data_store, &pa) != TLB_SUCC)
                                                return nothing_special; // Shi yang 2006-08-10
                                            store(mstate, mstate->gpr[6], va, pa, 4);   // Fix me: Shi yang 2006-08-10
                                        }

                                        if (args > 3)
                                        {
                                            if (translate_vaddr(mstate, va + 12, data_store, &pa) != TLB_SUCC)
                                                return nothing_special; // Shi yang 2006-08-10
                                            store(mstate, mstate->gpr[7], va, pa, 4);   // Fix me: Shi yang 2006-08-10
                                        }
                                    }

                                    if (ra)
                                    {
                                        va -= 4;
                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        store(mstate, mstate->gpr[31], va, pa, 4);      // Fix me: Shi yang 2006-08-10
                                    }
                                    switch (xsregs)
                                    {
                                        default:
                                            break;
                                        case 7:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                store(mstate, mstate->gpr[30], va, pa, 4);      // Fix me: Shi yang 2006-08-10
                                            }
                                        case 6:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                store(mstate, mstate->gpr[23], va, pa, 4);      // Fix me: Shi yang 2006-08-10
                                            }
                                        case 5:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                store(mstate, mstate->gpr[22], va, pa, 4);      // Fix me: Shi yang 2006-08-10
                                            }
                                        case 4:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                store(mstate, mstate->gpr[21], va, pa, 4);      // Fix me: Shi yang 2006-08-10
                                            }
                                        case 3:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                store(mstate, mstate->gpr[20], va, pa, 4);      // Fix me: Shi yang 2006-08-10
                                            }
                                        case 2:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                store(mstate, mstate->gpr[19], va, pa, 4);      // Fix me: Shi yang 2006-08-10
                                            }
                                        case 1:
                                            {
                                                va -= 4;
                                                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                    return nothing_special;     // Shi yang 2006-08-10
                                                store(mstate, mstate->gpr[18], va, pa, 4);      // Fix me: Shi yang 2006-08-10
                                            }
                                    }

                                    if (s0)
                                    {
                                        va -= 4;
                                        PA pa;

                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        store(mstate, mstate->gpr[17], va, pa, 4);      // Fix me: Shi yang 2006-08-10
                                    }
                                    if (s1)
                                    {
                                        va -= 4;
                                        PA pa;

                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        store(mstate, mstate->gpr[16], va, pa, 4);      // Fix me: Shi yang 2006-08-10
                                    }
                                    if (astatic > 0)
                                    {
                                        va -= 4;
                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        store(mstate, mstate->gpr[7], va, pa, 4);       // Fix me: Shi yang 2006-08-10

                                        if (astatic > 1)
                                        {
                                            va -= 4;
                                            if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                return nothing_special; // Shi yang 2006-08-10
                                            store(mstate, mstate->gpr[6], va, pa, 4);   // Fix me: Shi yang 2006-08-10
                                        }

                                        if (astatic > 2)
                                        {
                                            va -= 4;
                                            if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                return nothing_special; // Shi yang 2006-08-10
                                            store(mstate, mstate->gpr[5], va, pa, 4);   // Fix me: Shi yang 2006-08-10
                                        }

                                        if (astatic > 3)
                                        {
                                            va -= 4;
                                            if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                                return nothing_special; // Shi yang 2006-08-10
                                            store(mstate, mstate->gpr[4], va, pa, 4);   // Fix me: Shi yang 2006-08-10
                                        }
                                    }
                                    // Adjustment stack
                                    UInt32 framesize = (bits(Extendedimm, 7, 4) << 4) | svrs_framesize(instr);

                                    mstate->gpr[29] -= (framesize << 3);
                                } else
                                {
                                    VA va = mstate->gpr[29];
                                    bool_t ra = svrs_ra(instr);
                                    bool_t s0 = svrs_s0(instr);
                                    bool_t s1 = svrs_s1(instr);

                                    if ((ra || s0 || s1) && bits(va, 1, 0))     // Check alignment
                                    {
                                        fprintf(stderr, " in %s,address unaligned va=0x%x,pc=0x%x\n",
                                                __FUNCTION__, va, mstate->pc);
                                        skyeye_exit(-1);
                                        process_address_error(mstate, data_store, va);
                                    }

                                    if (ra)
                                    {
                                        va -= 4;
                                        PA pa;

                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        store(mstate, mstate->gpr[31], va, pa, 4);      // Fix me: Shi yang 2006-08-10
                                    }
                                    if (s0)
                                    {
                                        va -= 4;
                                        PA pa;

                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        store(mstate, mstate->gpr[17], va, pa, 4);      // Fix me: Shi yang 2006-08-10
                                    }
                                    if (s1)
                                    {
                                        va -= 4;
                                        PA pa;

                                        if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                                            return nothing_special;     // Shi yang 2006-08-10
                                        store(mstate, mstate->gpr[16], va, pa, 4);      // Fix me: Shi yang 2006-08-10
                                    }
                                    // Adjustment stack
                                    UInt32 framesize = svrs_framesize(instr);

                                    if (framesize == 0)
                                        mstate->gpr[29] -= 128;
                                    else
                                        mstate->gpr[29] -= (framesize << 3);
                                }
                                return nothing_special;
                            }
                        }
                    case MOV32R:
                        {
                            if (isExtended)
                            {
                                fprintf(stderr, "Error put Extend with Move instruction!\n");
                                return nothing_special;

                            } else
                            {
                                // FIXME: consider move $0,$16 as nop?
                                if (imm_ri(instr))
                                    mstate->gpr[mov_32r(instr)] = mstate->gpr[Xlat(mov_32rz(instr))];
                            }
                            return nothing_special;
                        }
                    case MOVR32:
                        {
                            if (isExtended)
                            {
                                fprintf(stderr, "Error put Extend with Move instruction!\n");
                                return nothing_special;

                            } else
                            {
                                mstate->gpr[Xlat(ry(instr))] = mstate->gpr[mov_r32(instr)];
                            }
                            return nothing_special;
                        }
                }
            }
        case LI:
            {
                // Load Immediate
                UInt32 x;

                if (isExtended)
                {
                    x = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);
                } else
                    x = imm_ri(instr);

                mstate->gpr[Xlat(rx(instr))] = x;
                return nothing_special;
            }
        case CMPI:
            {
                // Compare Immediate
                UInt32 x;

                if (isExtended)
                {
                    x = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);
                } else
                    x = imm_ri(instr);

                UInt32 y = mstate->gpr[Xlat(rx(instr))];

                mstate->gpr[24] = (y ^ x);
                return nothing_special;
            }
        case LB:
            {
                // Load Byte
                UInt32 x;
                UInt32 y = 0;

                if (mstate->sync_bit)
                    sync();
                Int32 offset;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    offset = sign_extend_UInt32(Extoffset, 16);
                } else
                    offset = imm_rri(instr);

                VA va = offset + mstate->gpr[Xlat(rx(instr))];
                PA pa;

                if (translate_vaddr(mstate, va, data_load, &pa) != TLB_SUCC)
                    return nothing_special;

                load(mstate, va, pa, &y, 1);
                x = sign_extend_UInt32(y & (0xff), 8);  // Shi yang 2006-08-10, Sign
                // extend
                mstate->gpr[Xlat(ry(instr))] = x;

                return nothing_special;
            }
        case LH:
            {
                // Load Halfword
                if (mstate->sync_bit)
                    sync();
                Int32 offset;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    offset = sign_extend_UInt32(Extoffset, 16);
                } else
                    offset = imm_rri(instr) << 1;

                VA va = offset + mstate->gpr[Xlat(rx(instr))];

                if (bit(va, 0))         // Check alignment
                    process_address_error(mstate, data_load, va);
                PA pa;

                if (translate_vaddr(mstate, va, data_load, &pa) != TLB_SUCC)
                    return nothing_special;     // Shi yang 2006-08-10

                UInt32 x;
                UInt32 y = 0;

                load(mstate, va, pa, &y, 2);
                x = sign_extend_UInt32(y & (0xffff), 16);       // Shi yang 2006-08-10, Sign extend
                mstate->gpr[Xlat(ry(instr))] = x;
                return nothing_special;
            }
        case LWSP:
            {
                // Load Word from SP
                if (mstate->sync_bit)
                    sync();

                Int32 offset;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    offset = sign_extend_UInt32(Extoffset, 16);
                } else
                    offset = imm_ri(instr) << 2;

                VA va = offset + mstate->gpr[29];

                if (va & 0x3)           // Check alignment
                    process_address_error(mstate, data_load, va);
                PA pa;

                if (translate_vaddr(mstate, va, data_load, &pa) != TLB_SUCC)
                    return nothing_special;     // Shi yang 2006-08-10

                UInt32 y = 0;

                load(mstate, va, pa, &y, 4);
                mstate->gpr[Xlat(rx(instr))] = y;
                return nothing_special;
            }
        case LW:
            {
                // Load Word
                if (mstate->sync_bit)
                    sync();

                Int32 offset;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    offset = sign_extend_UInt32(Extoffset, 16);
                } else
                    offset = imm_rri(instr) << 2;

                VA va = offset + mstate->gpr[Xlat(rx(instr))];

                if (va & 0x3)           // Check alignment
                {
                    process_address_error(mstate, data_load, va);
                }
                PA pa;

                if (translate_vaddr(mstate, va, data_load, &pa) != TLB_SUCC)
                {
                    return nothing_special;     // Shi yang 2006-08-10
                }

                UInt32 y = 0;

                load(mstate, va, pa, &y, 4);
                mstate->gpr[Xlat(ry(instr))] = y;
                return nothing_special;
            }
        case LBU:
            {
                // Load Byte Unsigned
                if (mstate->sync_bit)
                    sync();
                Int32 offset;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    offset = sign_extend_UInt32(Extoffset, 16);
                } else
                    offset = imm_rri(instr);

                VA va = offset + mstate->gpr[Xlat(rx(instr))];
                PA pa;

                if (translate_vaddr(mstate, va, data_load, &pa) != TLB_SUCC)
                    return nothing_special;

                UInt32 x;
                UInt32 y = 0;

                load(mstate, va, pa, &y, 1);
                x = y & 0xffL;          // Shi yang 2006-08-25
                mstate->gpr[Xlat(ry(instr))] = x;

                return nothing_special;
            }
        case LHU:
            {
                // Load Halfword Unsigned
                if (mstate->sync_bit)
                    sync();
                Int32 offset;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    offset = sign_extend_UInt32(Extoffset, 16);
                } else
                    offset = imm_rri(instr) << 1;

                VA va = offset + mstate->gpr[Xlat(rx(instr))];

                if (bit(va, 0))         // Check alignment
                    process_address_error(mstate, data_load, va);
                PA pa;

                if (translate_vaddr(mstate, va, data_load, &pa) != TLB_SUCC)
                    return nothing_special;     // Shi yang 2006-08-10
                UInt16 x;
                UInt32 y = 0;

                load(mstate, va, pa, &y, 2);
                x = y & 0xffffL;        // Shi yang 2006-08-25

                mstate->gpr[Xlat(ry(instr))] = x;
                return nothing_special;
            }
        case LWPC:
            {
                // Load Word from PC
                if (mstate->sync_bit)
                    sync();
                VA va;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    va = clear_bits(mstate->pc - 2, 1, 0) + sign_extend_UInt32(Extoffset, 16);
                    if (va & 0x3)       // Check alignment
                        process_address_error(mstate, data_load, va);
                } else
                {
                    VA base_pc = mstate->pipeline == branch_delay ? mstate->prev_pc : mstate->pc;

                    va = clear_bits(base_pc + (imm_ri(instr) << 2), 1, 0);
                }

                PA pa;

                if (translate_vaddr(mstate, va, data_load, &pa) != TLB_SUCC)
                    return nothing_special;     // Shi yang 2006-08-10

                UInt32 y = 0;

                load(mstate, va, pa, &y, 4);
                mstate->gpr[Xlat(rx(instr))] = y;
                return nothing_special;
            }
        case SB:
            {
                // Store Byte
                if (mstate->sync_bit)
                    sync();

                Int32 offset;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    offset = sign_extend_UInt32(Extoffset, 16);
                } else
                    offset = imm_rri(instr);

                VA va = offset + mstate->gpr[Xlat(rx(instr))];
                PA pa;

                if (translate_vaddr(mstate, va, data_store, &pa))
                    return nothing_special;     // Shi yang 2006-08-10
                store(mstate, mstate->gpr[Xlat(ry(instr))], va, pa, 1); // Fix me: Shi yang 2006-08-10
                return nothing_special;
            }
        case SH:
            {
                // Store Halfword
                if (mstate->sync_bit)
                    sync();
                Int32 offset;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    offset = sign_extend_UInt32(Extoffset, 16);
                } else
                    offset = imm_rri(instr) << 1;

                VA va = offset + mstate->gpr[Xlat(rx(instr))];

                if (bit(va, 0))         // Check alignment
                    process_address_error(mstate, data_store, va);
                PA pa;

                if (translate_vaddr(mstate, va, data_store, &pa))
                    return nothing_special;     // Shi yang 2006-08-10
                store(mstate, mstate->gpr[Xlat(ry(instr))], va, pa, 2); // Fix me: Shi yang 2006-08-10
                return nothing_special;
            }
        case SWSP:
            {
                // Store Word from SP
                if (mstate->sync_bit)
                    sync();

                Int32 offset;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    offset = sign_extend_UInt32(Extoffset, 16);
                } else
                    offset = imm_ri(instr) << 2;

                VA va = offset + mstate->gpr[29];

                if (va & 0x3)           // Check alignment
                    process_address_error(mstate, data_load, va);
                PA pa;

                if (translate_vaddr(mstate, va, data_load, &pa) != TLB_SUCC)
                    return nothing_special;     // Shi yang 2006-08-10

                store(mstate, mstate->gpr[Xlat(rx(instr))], va, pa, 4);
                return nothing_special;
            }
        case SW:
            {
                // Store Word
                if (mstate->sync_bit)
                    sync();
                Int32 offset;

                if (isExtended)
                {
                    VA Extoffset = (imm_rri(Extendedimm) << 11) | (bits(Extendedimm, 10, 5) << 5) | imm_rri(instr);

                    offset = sign_extend_UInt32(Extoffset, 16);
                } else
                    offset = imm_rri(instr) << 2;
                VA va = offset + mstate->gpr[Xlat(rx(instr))];

                if (bits(va, 1, 0))     // Check alignment
                {
                    fprintf(stderr, " in %s,address unaligned va=0x%x,pc=0x%x\n", __FUNCTION__, va, mstate->pc);
                    skyeye_exit(-1);
                    process_address_error(mstate, data_store, va);
                }
                PA pa;

                if (translate_vaddr(mstate, va, data_store, &pa) != TLB_SUCC)
                    return nothing_special;     // Shi yang 2006-08-10
                store(mstate, mstate->gpr[Xlat(ry(instr))], va, pa, 4); // Fix me: Shi yang 2006-08-10
                return nothing_special;
            }
        case RRR:
            {
                if (isExtended)
                {
                    fprintf(stderr, "Undfined ASMACRO instruction!\n");
                } else
                {
                    UInt32 fr = funct_rrr(instr);

                    if (fr == ADDU)
                    {
                        // Add Unsigned
                        UInt32 x = mstate->gpr[Xlat(rx(instr))];
                        UInt32 y = mstate->gpr[Xlat(ry(instr))];
                        UInt32 z = x + y;

                        mstate->gpr[Xlat(rz(instr))] = z;
                    } else if (fr = SUBU)
                    {
                        // Subtract Unsigned
                        UInt32 x = mstate->gpr[Xlat(rx(instr))];
                        UInt32 y = mstate->gpr[Xlat(ry(instr))];

                        mstate->gpr[Xlat(rz(instr))] = x - y;
                    } else
                        process_reserved_instruction(mstate);
                }
                return nothing_special;
            }
        case RR:
            {
                if (isExtended)
                {
                    fprintf(stderr, "There is no extend for slt instruction, pc=0x%x\n ", mstate->pc);
                    process_reserved_instruction(mstate);
                    return nothing_special;
                }

                switch (funct_rr(instr))
                {
                    case JR:
                        {
                            if (mstate->pipeline == branch_delay)
                                printf("Can't handle JR in branch delay slot\n");

                            switch (ry(instr))
                            {
                                case JRRA:
                                    {
                                        VA va = mstate->gpr[31];

                                        mstate->branch_target = va;
                                        return (bits(va, 1, 0) == 0x2) ? instr_addr_error : branch_delay;
                                    }
                                case JRRX:
                                    {
                                        VA va = mstate->gpr[Xlat(rx(instr))];

                                        mstate->branch_target = va;
                                        return (bits(va, 1, 0) == 0x2) ? instr_addr_error : branch_delay;
                                    }
                                case JALR:
                                    {
                                        // Jump and Link Register
                                        mstate->gpr[31] = clear_bit(mstate->pc + 4, 0) | mstate->isa_mode;
                                        VA va = mstate->gpr[Xlat(rx(instr))];

                                        mstate->branch_target = va;
                                        return (bits(va, 1, 0) == 0x2) ? instr_addr_error : branch_delay;
                                    }
                                case JRCRX:
                                    {
                                        // no delay slot
                                        VA va = mstate->gpr[Xlat(rx(instr))];

                                        mstate->pc = clear_bit(va, 0);
                                        mstate->isa_mode = bit(va, 0);
                                        mstate->pipeline = (bits(va, 1, 0) == 0x2) ? instr_addr_error : branch_nodelay;
                                        return nothing_special;
                                    }
                                case JRCRA:
                                    {
                                        // no delay slot
                                        VA va = mstate->gpr[31];

                                        mstate->pc = clear_bit(va, 0);
                                        mstate->isa_mode = bit(va, 0);
                                        mstate->pipeline = (bits(va, 1, 0) == 0x2) ? instr_addr_error : branch_nodelay;
                                        return nothing_special;
                                    }
                                case JALRC:
                                    {
                                        // no delay slot
                                        mstate->gpr[31] = clear_bit(mstate->pc + 2, 0) | mstate->isa_mode;
                                        VA va = mstate->gpr[Xlat(rx(instr))];

                                        mstate->pc = clear_bit(va, 0);
                                        mstate->isa_mode = bit(va, 0);
                                        mstate->pipeline = (bits(va, 1, 0) == 0x2) ? instr_addr_error : branch_nodelay;
                                        return nothing_special;
                                    }
                                default:
                                    // Reserved instruction.
                                    process_reserved_instruction(mstate);
                                    return nothing_special;
                            }
                        }
                    case SDBBP:
                        {
                            // Software Debug Breakpoint
                            fprintf(stderr, "workaround for sdbbp instruction, pc=0x%x\n ", mstate->pc);
                            return nothing_special;
                        }
                    case SLT:
                        {
                            // Set On Less Than
                            Int32 x = mstate->gpr[Xlat(rx(instr))];
                            Int32 y = mstate->gpr[Xlat(ry(instr))];

                            mstate->gpr[24] = (x < y);
                            return nothing_special;
                        }
                    case SLTU:
                        {
                            // Set On Less Than Unsigned
                            UInt32 x = mstate->gpr[Xlat(rx(instr))];
                            UInt32 y = mstate->gpr[Xlat(ry(instr))];

                            mstate->gpr[24] = (x < y);
                            return nothing_special;
                        }
                    case SLLV:
                        {
                            // Shift Word Left Logical Variable
                            UInt32 x = bits(mstate->gpr[Xlat(rx(instr))], 4, 0);

                            mstate->gpr[Xlat(ry(instr))] <<= x;
                            return nothing_special;
                        }
                    case BREAK:
                        {
                            fprintf(stderr, "workaround for break instruction, pc=0x%x\n ", mstate->pc);
                            return nothing_special;
                        }
                    case SRLV:
                        {
                            // Shift Right Logical Variable
                            UInt32 s = bits(mstate->gpr[Xlat(rx(instr))], 4, 0);

                            mstate->gpr[Xlat(ry(instr))] >>= s;
                            return nothing_special;
                        }
                    case SRAV:
                        {
                            // Shift Right Arithmetic Variable
                            UInt32 x = mstate->gpr[Xlat(ry(instr))];
                            UInt32 s = bits(mstate->gpr[Xlat(rx(instr))], 4, 0);

                            x >>= s;
                            mstate->gpr[Xlat(ry(instr))] = sign_extend_UInt32(x, 32 - s);
                            return nothing_special;
                        }
                    case CMP:
                        {
                            // Compare
                            UInt32 x = mstate->gpr[Xlat(rx(instr))];
                            UInt32 y = mstate->gpr[Xlat(ry(instr))];

                            mstate->gpr[24] = (y ^ x);
                            return nothing_special;
                        }
                    case NEG:
                        {
                            // Negate
                            Int32 x = mstate->gpr[Xlat(ry(instr))];
                            Int32 y = 0 - x;

                            mstate->gpr[Xlat(rx(instr))] = y;
                            return nothing_special;
                        }
                    case AND:
                        {
                            // And
                            mstate->gpr[Xlat(rx(instr))] &= mstate->gpr[Xlat(ry(instr))];
                            return nothing_special;
                        }
                    case OR:
                        {
                            // OR
                            mstate->gpr[Xlat(rx(instr))] |= mstate->gpr[Xlat(ry(instr))];
                            return nothing_special;
                        }
                    case XOR:
                        {
                            // XOR
                            mstate->gpr[Xlat(rx(instr))] ^= mstate->gpr[Xlat(ry(instr))];
                            return nothing_special;
                        }
                    case NOT:
                        {
                            // NOT
                            mstate->gpr[Xlat(rx(instr))] = ~mstate->gpr[Xlat(ry(instr))];
                            return nothing_special;
                        }
                    case MFHI:
                        {
                            // Move From HI
                            mstate->gpr[Xlat(rx(instr))] = mstate->hi;
                            return nothing_special;
                        }
                    case CNVT:
                        {
                            switch (ry(instr))
                            {
                                case ZEB:
                                    {
                                        uint32_t tmp = mstate->gpr[Xlat(rx(instr))] & 0xff;

                                        mstate->gpr[Xlat(rx(instr))] = tmp;
                                        return nothing_special;
                                    }
                                case ZEH:
                                    {
                                        uint32_t tmp = mstate->gpr[Xlat(rx(instr))] & 0xffff;

                                        mstate->gpr[Xlat(rx(instr))] = tmp;
                                        return nothing_special;
                                    }
                                case SEB:
                                    {
                                        uint32_t tmp = mstate->gpr[Xlat(rx(instr))] & 0xff;

                                        if (tmp & 0x80)
                                            tmp |= (-1) ^ 0xff;
                                        mstate->gpr[Xlat(rx(instr))] = tmp;
                                        return nothing_special;
                                    }
                                case SEH:
                                    {
                                        uint32_t tmp = mstate->gpr[Xlat(rx(instr))] & 0xffff;

                                        if (tmp & 0x8000)
                                            tmp |= (-1) ^ 0xffff;
                                        mstate->gpr[Xlat(rx(instr))] = tmp;
                                        return nothing_special;
                                    }
                                default:
                                    // Reserved instruction.
                                    process_reserved_instruction(mstate);
                                    return nothing_special;
                            }
                        }
                    case MFLO:
                        {
                            // Move From LO
                            mstate->gpr[Xlat(rx(instr))] = mstate->lo;
                            return nothing_special;
                        }
                    case MULT:
                        {
                            // Multiply
                            Int64 x = (int) mstate->gpr[Xlat(rx(instr))];
                            Int64 y = (int) mstate->gpr[Xlat(ry(instr))];

                            // multiply(x, y);
                            long long z = x * y;

                            mstate->hi = (z >> 32) & 0xFFFFFFFF;
                            mstate->lo = z & 0xFFFFFFFF;
                            return nothing_special;
                        }
                    case MULTU:
                        {
                            // Multiply Unsigned
                            UInt64 x = mstate->gpr[Xlat(rx(instr))];
                            UInt64 y = mstate->gpr[Xlat(ry(instr))];
                            unsigned long long z = x * y;

                            // multiply(x, y);
                            mstate->hi = (z >> 32) & 0xFFFFFFFF;
                            mstate->lo = z & 0xFFFFFFFF;
                            return nothing_special;
                        }
                    case DIV:
                        {
                            // Divide
                            Int32 x = mstate->gpr[Xlat(rx(instr))];
                            Int32 y = mstate->gpr[Xlat(ry(instr))];

                            divide_Int32(x, y);
                            return nothing_special;
                        }
                    case DIVU:
                        {
                            UInt32 x = mstate->gpr[Xlat(rx(instr))];
                            UInt32 y = mstate->gpr[Xlat(ry(instr))];

                            divide_UInt32(x, y);
                            return nothing_special;
                        }
                    default:
                        // Reserved instruction.
                        process_reserved_instruction(mstate);
                        return nothing_special;
                }
            }
        default:
            // Reserved instruction.
            process_reserved_instruction(mstate);
            return nothing_special;
    }
}

/* Copyright (C)
 * 2012 - Michael.Kang blackfin.kang@gmail.com
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/**
* @file cortex_a8_mmu.c
* @brief The mmu implementation of cortex a9
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2012-02-01
*/
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <skyeye_ram.h>

#include "armdefs.h"
#include "bank_defs.h"

#define BANK0_START 0x80000000
//static void mem_read_raw(uint32_t offset, uint32_t &value, int size)
static inline void mem_read_raw(int size, uint32_t offset, uint32_t * value)
{
    if (offset == 0)
        return;
    //printf("In %s, offset=0x%x, mem_ptr=0x%llx\n", __FUNCTION__, offset, mem_ptr);
    if (offset >= 0x80000000 && offset < 0x90000000)
    {
        mem_read(size, offset, value);
    } else
    {
        bus_read(size, offset, value);
    }
}

//static void mem_write_raw(uint32_t offset, uint32_t value, int size)
static inline void mem_write_raw(int size, uint32_t offset, uint32_t value)
{
    //printf("In %s, offset=0x%x, mem_ptr=0x%llx\n", __FUNCTION__, offset, mem_ptr);
    if (offset >= 0x80000000 && offset < 0x90000000)
    {
        mem_write(size, offset, value);
    } else
    {
        bus_write(size, offset, value);
    }
}

static inline int exclusive_detect(ARMul_State * state, ARMword addr)
{
    int i;

#if 0
    for (i = 0; i < 128; i++)
    {
        if (state->exclusive_tag_array[i] == addr)
            return 0;
    }
#endif
    if (state->exclusive_tag_array[0] == addr)
        return 0;
    else
        return -1;
}

static inline void add_exclusive_addr(ARMul_State * state, ARMword addr)
{
    int i;

#if 0
    for (i = 0; i < 128; i++)
    {
        if (state->exclusive_tag_array[i] == 0xffffffff)
        {
            state->exclusive_tag_array[i] = addr;
            //printf("In %s, add  addr 0x%x\n", __func__, addr);
            return;
        }
    }
    printf("In %s ,can not monitor the addr, out of array\n", __FUNCTION__);
#endif
    state->exclusive_tag_array[0] = addr;
    return;
}

static inline void remove_exclusive(ARMul_State * state, ARMword addr)
{
#if 0
    int i;

    for (i = 0; i < 128; i++)
    {
        if (state->exclusive_tag_array[i] == addr)
        {
            state->exclusive_tag_array[i] = 0xffffffff;
            //printf("In %s, remove  addr 0x%x\n", __func__, addr);
            return;
        }
    }
#endif
    state->exclusive_tag_array[0] = 0xFFFFFFFF;
}

/* This function encodes table 8-2 Interpreting AP bits,
 * returning non-zero if access is allowed. */
static int check_perms(ARMul_State * state, int ap, int read)
{
    int s, r, user;

    s = state->mmu.control & CONTROL_SYSTEM;
    r = state->mmu.control & CONTROL_ROM;
    /* chy 2006-02-15 , should consider system mode, don't conside 26bit mode */
    //    printf("ap is %x, user is %x, s is %x, read is %x\n", ap, user, s, read);
    //    printf("mode is %x\n", state->Mode);
    user = (state->Mode == USER32MODE) || (state->Mode == USER26MODE) || (state->Mode == SYSTEM32MODE);

    switch (ap)
    {
        case 0:
            return read && ((s && !user) || r);
        case 1:
            return !user;
        case 2:
            return read || !user;
        case 3:
            return 1;
    }
    return 0;
}

/*  ap: AP bits value.
 *  sop: section or page description  0:section 1:page
 */
static fault_t mmu_translate(ARMul_State * state, ARMword virt_addr, ARMword * phys_addr, int *ap, int *sop)
{
    {
        /* walk the translation tables */
        ARMword l1addr, l1desc;

        if (state->mmu.translation_table_ctrl
            && virt_addr << state->mmu.translation_table_ctrl >> (32 - state->mmu.translation_table_ctrl - 1))
        {
            l1addr = state->mmu.translation_table_base1;
            l1addr = (((l1addr >> 14) << 14) | (virt_addr >> 18)) & ~3;
        } else
        {
            l1addr = state->mmu.translation_table_base0;
            l1addr =
                (((l1addr >> (14 - state->mmu.translation_table_ctrl)) << (14 - state->mmu.translation_table_ctrl)) |
                 (virt_addr << state->mmu.translation_table_ctrl) >> (18 + state->mmu.translation_table_ctrl)) & ~3;
        }

        /* l1desc = mem_read_word (state, l1addr); */
        if (state->space.conf_obj != NULL)
            state->space.read(state->space.conf_obj, l1addr, &l1desc, 4);
        else
            mem_read_raw(32, l1addr, &l1desc);

#if 0
        if (virt_addr == 0xc000d2bc)
        {
            printf("mmu_control is %x\n", state->mmu.translation_table_ctrl);
            printf("mmu_table_0 is %x\n", state->mmu.translation_table_base0);
            printf("mmu_table_1 is %x\n", state->mmu.translation_table_base1);
            printf("l1addr is %x l1desc is %x\n", l1addr, l1desc);
            //               exit(-1);
        }
#endif
        switch (l1desc & 3)
        {
            case 0:
            case 3:
                /*
                 * according to Figure 3-9 Sequence for checking faults in arm manual,
                 * section translation fault should be returned here.
                 */
                {
                    return SECTION_TRANSLATION_FAULT;
                }
            case 1:
                /* coarse page table */
                {
                    ARMword l2addr, l2desc;

                    l2addr = l1desc & 0xFFFFFC00;
                    l2addr = (l2addr | ((virt_addr & 0x000FF000) >> 10)) & ~3;
                    if (state->space.conf_obj != NULL)
                        state->space.read(state->space.conf_obj, l2addr, &l2desc, 4);
                    else
                        mem_read_raw(32, l2addr, &l2desc);

                    /* chy 2003-09-02 for xscale */
                    *ap = (l2desc >> 4) & 0x3;
                    *sop = 1;           /* page */

                    switch (l2desc & 3)
                    {
                        case 0:
                            return PAGE_TRANSLATION_FAULT;
                            break;
                        case 1:
                            *phys_addr = (l2desc & 0xFFFF0000) | (virt_addr & 0x0000FFFF);
                            break;
                        case 2:
                        case 3:
                            *phys_addr = (l2desc & 0xFFFFF000) | (virt_addr & 0x00000FFF);
                            break;

                    }
                }
                break;
            case 2:
                /* section */

                *ap = (l1desc >> 10) & 3;
                *sop = 0;               /* section */
#if 0
                if (virt_addr == 0xc000d2bc)
                {
                    printf("mmu_control is %x\n", state->mmu.translation_table_ctrl);
                    printf("mmu_table_0 is %x\n", state->mmu.translation_table_base0);
                    printf("mmu_table_1 is %x\n", state->mmu.translation_table_base1);
                    printf("l1addr is %x l1desc is %x\n", l1addr, l1desc);
                    //                    printf("l2addr is %x l2desc is %x\n", l2addr, l2desc);
                    printf("ap is %x, sop is %x\n", *ap, *sop);
                    printf("mode is %d\n", state->Mode);
                    //                      exit(-1);
                }
#endif

                if (l1desc & 0x30000)
                    *phys_addr = (l1desc & 0xFF000000) | (virt_addr & 0x00FFFFFF);
                else
                    *phys_addr = (l1desc & 0xFFF00000) | (virt_addr & 0x000FFFFF);
                break;
        }
    }
    return NO_FAULT;
}

fault_t cortex_a8_mmu_write(ARMul_State * state, ARMword va, ARMword data, ARMword datatype);
fault_t cortex_a8_mmu_read(ARMul_State * state, ARMword va, ARMword * data, ARMword datatype);

int cortex_a8_mmu_init(ARMul_State * state)
{
    state->mmu.control = 0xc50078;
    state->mmu.translation_table_base = 0xDEADC0DE;
    state->mmu.domain_access_control = 0xDEADC0DE;
    state->mmu.fault_status = 0;
    state->mmu.fault_address = 0;
    state->mmu.process_id = 0;
    state->mmu.context_id = 0;
    state->mmu.thread_uro_id = 0;

    return No_exp;
}

void cortex_a8_mmu_exit(ARMul_State * state)
{
}

fault_t cortex_a8_mmu_load_instr(ARMul_State * state, ARMword va, ARMword * instr)
{
    fault_t fault;
    int c;                              /* cache bit */
    ARMword pa;                         /* physical addr */
    ARMword perm;                       /* physical addr access permissions */
    int ap, sop;

    static int debug_count = 0;         /* used for debug */

    d_msg("va = %x\n", va);

    va = mmu_pid_va_map(va);
    if (MMU_Enabled)
    {
        //            printf("MMU enabled.\n");
        //            sleep(1);
        /* align check */
        if ((va & (WORD_SIZE - 1)) && MMU_Aligned)
        {
            d_msg("align\n");
            return ALIGNMENT_FAULT;
        } else
            va &= ~(WORD_SIZE - 1);

        /* translate tlb */
        fault = mmu_translate(state, va, &pa, &ap, &sop);
        if (fault)
        {
            d_msg("translate\n");
            printf("va=0x%x, icounter=%lld, fault=%d\n", va, state->NumInstrs, fault);
            return fault;
        }

        /* no tlb, only check permission */
        if (!check_perms(state, ap, 1))
        {
            if (sop == 0)
            {
                return SECTION_PERMISSION_FAULT;
            } else
            {
                return SUBPAGE_PERMISSION_FAULT;
            }
        }
#if 0
        /*check access */
        fault = check_access(state, va, tlb, 1);
        if (fault)
        {
            d_msg("check_fault\n");
            return fault;
        }
#endif
    }

    /*if MMU disabled or C flag is set alloc cache */
    if (MMU_Disabled)
    {
        //            printf("MMU disabled.\n");
        //            sleep(1);
        pa = va;
    }
    if (state->space.conf_obj == NULL)
        state->space.read(state->space.conf_obj, pa, instr, 4);
    else
        mem_read_raw(32, pa, instr);

    return 0;
}

fault_t cortex_a8_mmu_read_byte(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    /* ARMword temp,offset; */
    fault_t fault;

    fault = cortex_a8_mmu_read(state, virt_addr, data, ARM_BYTE_TYPE);
    return fault;
}

fault_t cortex_a8_mmu_read_halfword(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    /* ARMword temp,offset; */
    fault_t fault;

    fault = cortex_a8_mmu_read(state, virt_addr, data, ARM_HALFWORD_TYPE);
    return fault;
}

fault_t cortex_a8_mmu_read_word(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    return cortex_a8_mmu_read(state, virt_addr, data, ARM_WORD_TYPE);
}

fault_t cortex_a8_mmu_read(ARMul_State * state, ARMword va, ARMword * data, ARMword datatype)
{
    fault_t fault;
    ARMword pa, real_va, temp, offset;
    ARMword perm;                       /* physical addr access permissions */
    int ap, sop;

    d_msg("va = %x\n", va);

    va = mmu_pid_va_map(va);
    real_va = va;
    /* if MMU disabled, memory_read */
    if (MMU_Disabled)
    {
        //       printf("MMU disabled cpu_id:%x addr:%x.\n", state->mmu.process_id, va);
        //       sleep(1);

        /* *data = mem_read_word(state, va); */
        if (datatype == ARM_BYTE_TYPE)
            /* *data = mem_read_byte (state, va); */
            if (state->space.conf_obj != NULL)
                state->space.read(state->space.conf_obj, va, data, 1);
            else
                mem_read_raw(8, va, data);

        else if (datatype == ARM_HALFWORD_TYPE)
            /* *data = mem_read_halfword (state, va); */
            if (state->space.conf_obj != NULL)
                state->space.read(state->space.conf_obj, va, data, 2);
            else
                mem_read_raw(16, va, data);

        else if (datatype == ARM_WORD_TYPE)
            /* *data = mem_read_word (state, va); */
            if (state->space.conf_obj != NULL)
                state->space.read(state->space.conf_obj, va, data, 4);
            else
                mem_read_raw(32, va, data);

        else
        {
            printf("SKYEYE:1 cortex_a8_mmu_read error: unknown data type %d\n", datatype);
            skyeye_exit(-1);
        }

        return 0;
    }
    //printf("MMU enabled.\n");
    //sleep(1);

    /* align check */
    if (((va & 3) && (datatype == ARM_WORD_TYPE) && MMU_Aligned) ||
        ((va & 1) && (datatype == ARM_HALFWORD_TYPE) && MMU_Aligned))
    {
        d_msg("align\n");
        return ALIGNMENT_FAULT;
    }

    /*translate va to tlb */
#if 0
    fault = mmu_translate(state, va, ARM920T_D_TLB(), &tlb);
#endif
    fault = mmu_translate(state, va, &pa, &ap, &sop);
#if 0
    if (va == 0xbebb1774 || state->Reg[15] == 0x400ff594)
    {
        //printf("In %s, current=0x%x. mode is %x, pc=0x%x\n", __FUNCTION__, state->CurrInstr, state->Mode, state->Reg[15]);
        printf("In %s, ap is %d, sop is %d, va=0x%x, pa=0x%x, fault=%d, data=0x%x\n", __FUNCTION__, ap, sop, va, pa,
               fault, data);
        int i;

        for (i = 0; i < 16; i++)
            printf("Reg[%d]=0x%x\t", i, state->Reg[i]);
        printf("\n");
    }
#endif
    if (fault)
    {
        d_msg("translate\n");
        //printf("mmu read fault at %x\n", va);
        //printf("fault is %d\n", fault);
        return fault;
    }
    //    printf("va is %x pa is %x\n", va, pa);

    /* no tlb, only check permission */
    if (!check_perms(state, ap, 1))
    {
        if (sop == 0)
        {
            return SECTION_PERMISSION_FAULT;
        } else
        {
            return SUBPAGE_PERMISSION_FAULT;
        }
    }
#if 0
    /*check access permission */
    fault = check_access(state, va, tlb, 1);
    if (fault)
        return fault;
#endif

  skip_translation:
    /* *data = mem_read_word(state, pa); */
    if (datatype == ARM_BYTE_TYPE)
    {
        /* *data = mem_read_byte (state, pa | (real_va & 3)); */
        if (state->space.conf_obj != NULL)
            state->space.read(state->space.conf_obj, pa | (real_va & 3), data, 1);
        else
            mem_read_raw(8, pa | (real_va & 3), data);

        /* mem_read_raw(32, pa | (real_va & 3), data); */
    } else if (datatype == ARM_HALFWORD_TYPE)
    {
        /* *data = mem_read_halfword (state, pa | (real_va & 2)); */
        if (state->space.conf_obj != NULL)
            state->space.read(state->space.conf_obj, pa | (real_va & 3), data, 2);
        else
            mem_read_raw(16, pa | (real_va & 3), data);

        /* mem_read_raw(32, pa | (real_va & 2), data); */
    } else if (datatype == ARM_WORD_TYPE)
        /* *data = mem_read_word (state, pa); */
        if (state->space.conf_obj != NULL)
            state->space.read(state->space.conf_obj, pa, data, 4);
        else
            mem_read_raw(32, pa, data);

    else
    {
        printf("SKYEYE:2 cortex_a8_mmu_read error: unknown data type %d\n", datatype);
        skyeye_exit(-1);
    }
    if (0 && (va == 0x2869c))
    {
        printf("In %s, pa is %x va=0x%x, value is %x pc %x, instr=0x%x\n", __FUNCTION__, pa, va, *data, state->Reg[15],
               state->CurrInstr);
    }

    /* ldrex or ldrexb */
    if (((state->CurrInstr & 0x0FF000F0) == 0x01900090) || ((state->CurrInstr & 0x0FF000F0) == 0x01d00090))
    {
        int rn = (state->CurrInstr & 0xF0000) >> 16;

        if (state->Reg[rn] == va)
        {
            add_exclusive_addr(state, pa | (real_va & 3));
            state->exclusive_access_state = 1;
        }
    }
#if 0
    if (state->pc == 0xc011a868)
    {
        printf("pa is %x value is %x size is %x\n", pa, data, datatype);
        printf("icounter is %lld\n", state->NumInstrs);
        //            exit(-1);
    }
#endif

    return 0;
}

fault_t cortex_a8_mmu_write_byte(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return cortex_a8_mmu_write(state, virt_addr, data, ARM_BYTE_TYPE);
}

fault_t cortex_a8_mmu_write_halfword(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return cortex_a8_mmu_write(state, virt_addr, data, ARM_HALFWORD_TYPE);
}

fault_t cortex_a8_mmu_write_word(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return cortex_a8_mmu_write(state, virt_addr, data, ARM_WORD_TYPE);
}

fault_t cortex_a8_mmu_write(ARMul_State * state, ARMword va, ARMword data, ARMword datatype)
{
    int b;
    ARMword pa, real_va;
    ARMword perm;                       /* physical addr access permissions */
    fault_t fault;
    int ap, sop;

#if 0
    /8 for sky_printk
        debugger.* / if (va == 0xffffffff)
        {
            putchar((char) data);
            return 0;
        }
    if (va == 0xBfffffff)
    {
        putchar((char) data);
        return 0;
    }
#endif

    d_msg("va = %x, val = %x\n", va, data);
    va = mmu_pid_va_map(va);
    real_va = va;

    if (MMU_Disabled)
    {
        /* mem_write_word(state, va, data); */
        if (datatype == ARM_BYTE_TYPE)
            /* mem_write_byte (state, va, data); */
            if (state->space.conf_obj != NULL)
                state->space.write(state->space.conf_obj, va, &data, 1);
            else
                mem_write_raw(8, va, data);

        else if (datatype == ARM_HALFWORD_TYPE)
            /* mem_write_halfword (state, va, data); */
            if (state->space.conf_obj != NULL)
                state->space.write(state->space.conf_obj, va, &data, 2);
            else
                mem_write_raw(16, va, data);

        else if (datatype == ARM_WORD_TYPE)
            /* mem_write_word (state, va, data); */
            if (state->space.conf_obj != NULL)
                state->space.write(state->space.conf_obj, va, &data, 4);
            else
                mem_write_raw(32, va, data);

        else
        {
            printf("SKYEYE:1 cortex_a8_mmu_write error: unknown data type %d\n", datatype);
            skyeye_exit(-1);
        }
        goto finished_write;
        //return 0;
    }
    /*align check */
    /* if ((va & (WORD_SIZE - 1)) && MMU_Aligned){ */
    if (((va & 3) && (datatype == ARM_WORD_TYPE) && MMU_Aligned) ||
        ((va & 1) && (datatype == ARM_HALFWORD_TYPE) && MMU_Aligned))
    {
        d_msg("align\n");
        return ALIGNMENT_FAULT;
    }
    va &= ~(WORD_SIZE - 1);

    /*tlb translate */
    fault = mmu_translate(state, va, &pa, &ap, &sop);
#if 0
    if (va == 0xbebb1774 || state->Reg[15] == 0x40102334)
    {
        //printf("In %s, current=0x%x. mode is %x, pc=0x%x\n", __FUNCTION__, state->CurrInstr, state->Mode, state->Reg[15]);
        printf("In %s, ap is %d, sop is %d, va=0x%x, pa=0x%x, fault=%d, data=0x%x\n", __FUNCTION__, ap, sop, va, pa,
               fault, data);
        int i;

        for (i = 0; i < 16; i++)
            printf("Reg[%d]=0x%x\t", i, state->Reg[i]);
        printf("\n");
    }
#endif
    if (fault)
    {
        d_msg("translate\n");
        //printf("mmu write fault at %x\n", va);
        return fault;
    }
    //    printf("va is %x pa is %x\n", va, pa);

    /* no tlb, only check permission */
    if (!check_perms(state, ap, 0))
    {
        if (sop == 0)
        {
            return SECTION_PERMISSION_FAULT;
        } else
        {
            return SUBPAGE_PERMISSION_FAULT;
        }
    }
#if 0
    /* tlb check access */
    fault = check_access(state, va, tlb, 0);
    if (fault)
    {
        d_msg("check_access\n");
        return fault;
    }
#endif
#if 0
    if (pa <= 0x502860ff && (pa + 1 << datatype) > 0x502860ff)
    {
        printf("pa is %x value is %x size is %x\n", pa, data, datatype);
    }
#endif
#if 0
    if (state->pc == 0xc011a878)
    {
        printf("write pa is %x value is %x size is %x\n", pa, data, datatype);
        printf("icounter is %lld\n", state->NumInstrs);
        exit(-1);
    }
#endif
  skip_translation:
    /* strex */
    if (((state->CurrInstr & 0x0FF000F0) == 0x01800090) || ((state->CurrInstr & 0x0FF000F0) == 0x01c00090))
    {
        /* failed , the address is monitord now. */
        int dest_reg = (state->CurrInstr & 0xF000) >> 12;

        if ((exclusive_detect(state, pa | (real_va & 3)) == 0) && (state->exclusive_access_state == 1))
        {
            remove_exclusive(state, pa | (real_va & 3));
            state->Reg[dest_reg] = 0;
            state->exclusive_access_state = 0;
        } else
        {
            state->Reg[dest_reg] = 1;
            //printf("In %s, try to strex a monitored address 0x%x\n", __FUNCTION__, pa);
            return 0;
        }
    }

    if (datatype == ARM_BYTE_TYPE)
    {
        /* mem_write_byte (state,
         * (pa | (real_va & 3)),
         * data);
         */
        if (state->space.conf_obj != NULL)
            state->space.write(state->space.conf_obj, (pa | (real_va & 3)), &data, 1);
        else
            mem_write_raw(8, (pa | (real_va & 3)), data);

    } else if (datatype == ARM_HALFWORD_TYPE)
        /* mem_write_halfword (state,
         * (pa |
         * (real_va & 2)),
         * data);
         */
        if (state->space.conf_obj != NULL)
            state->space.write(state->space.conf_obj, (pa | (real_va & 3)), &data, 2);
        else
            mem_write_raw(16, (pa | (real_va & 3)), data);

    else if (datatype == ARM_WORD_TYPE)
        /* mem_write_word (state, pa, data); */
        if (state->space.conf_obj != NULL)
            state->space.write(state->space.conf_obj, pa, &data, 4);
        else
            mem_write_raw(32, pa, data);

#if 0
    if (state->NumInstrs > 236403)
    {
        printf("write memory\n");
        printf("pa is %x value is %x size is %x\n", pa, data, datatype);
        printf("icounter is %lld\n", state->NumInstrs);
    }
#endif
    static int write_begin = 0;

  finished_write:
#if DIFF_WRITE
    if (state->CurrWrite == 0xdeadc0de || write_begin == 1)
    {
        if (state->CurrWrite == 0xdeadc0de)
            state->CurrWrite = 0;
        write_begin = 1;
        if (state->CurrWrite >= 17)
        {
            printf("Wrong write array, 0x%x", state->CurrWrite);
            exit(-1);
        }
        uint32 record_data = data;

        if (datatype == ARM_BYTE_TYPE)
            record_data &= 0xFF;
        if (datatype == ARM_HALFWORD_TYPE)
            record_data &= 0xFFFF;

        state->WriteAddr[state->CurrWrite] = pa | (real_va & 3);
        state->WriteData[state->CurrWrite] = record_data;
        state->WritePc[state->CurrWrite] = state->Reg[15];
        state->CurrWrite++;
        //printf("In %s, pc=0x%x, addr=0x%x, data=0x%x, CFlag=%d\n", __FUNCTION__, state->Reg[15],  pa | (real_va & 3), record_data, state->CFlag);
    }
#endif

    return 0;
}

ARMword cortex_a8_mmu_mrc(ARMul_State * state, ARMword instr, ARMword * value)
{
    mmu_regnum_t creg = BITS(16, 19) & 0xf;
    int OPC_1 = BITS(21, 23) & 0x7;
    int OPC_2 = BITS(5, 7) & 0x7;
    ARMword data;

    switch (creg)
    {
        case MMU_ID:
            if (OPC_2 == 0)
            {
                data = state->cpu->cpu_val;
            } else if (OPC_2 == 1)
            {
                /* Cache type:
                 * 000 0110 1 000 101 110 0 10 000 101 110 0 10
                 * */
                data = 0x0D172172;
            } else if (OPC_2 == 5)
            {
                /* Memory Model Feature 0, linux kernel use this to get cpu_architecture */
                data = 0x01100003;
            }
            break;
        case MMU_CONTROL:
            /*
             * 6:3          read as 1
             * 10           read as 0
             * 18,16        read as 1
             * */
            data = (state->mmu.control | 0xc50078) & 0xFFFFFBFF;
            break;
        case MMU_TRANSLATION_TABLE_BASE:
#if 0
            data = state->mmu.translation_table_base;
#endif
            switch (OPC_2)
            {
                case 0:
                    data = state->mmu.translation_table_base0;
                    break;
                case 1:
                    data = state->mmu.translation_table_base1;
                    break;
                case 2:
                    data = state->mmu.translation_table_ctrl;
                    break;
                default:
                    printf("mmu_mrc read UNKNOWN - p15 c2 opcode2 %d\n", OPC_2);
                    break;
            }
            break;
        case MMU_DOMAIN_ACCESS_CONTROL:
            data = state->mmu.domain_access_control;
            break;
        case MMU_FAULT_STATUS:
            /* OPC_2 = 0: data FSR value
             * */
            if (OPC_2 == 0)
                data = state->mmu.fault_status;
            if (OPC_2 == 1)
                data = state->mmu.fault_statusi;
            break;
        case MMU_FAULT_ADDRESS:
            data = state->mmu.fault_address;
            break;
        case MMU_PID:
            //data = state->mmu.process_id;
            if (OPC_2 == 0)
                data = state->mmu.process_id;
            else if (OPC_2 == 1)
                data = state->mmu.context_id;
            else if (OPC_2 == 3)
            {
                data = state->mmu.thread_uro_id;
            } else if (OPC_2 == 2)
            {
                data = state->mmu.thread_urw_id;
            } else
            {
                printf("mmu_mrc read UNKNOWN - reg %d\n", creg);
            }
            //printf("SKYEYE In %s, read pid 0x%x, OPC_2 %d, instr=0x%x\n", __FUNCTION__, data, OPC_2, instr);
            //exit(-1);
            break;
        default:
            printf("mmu_mrc read UNKNOWN - reg %d\n", creg);
            data = 0;
            break;
    }
    /*      printf("\t\t\t\t\tpc = 0x%08x\n", state->Reg[15]); */
    *value = data;
    return data;
}

#define CORTEX_A8_I_TLB (&state->mmu.u.cortex_a8_mmu.i_tlb)
#define CORTEX_A8_D_TLB (&state->mmu.u.cortex_a8_mmu.d_tlb)
static void cortex_a8_mmu_tlb_ops(ARMul_State * state, ARMword instr, ARMword value)
{
    int CRm, OPC_2;

    CRm = BITS(0, 3);
    OPC_2 = BITS(5, 7);

    /* Invalidate Inst-TLB and Data-TLB */
    if (OPC_2 == 0 && CRm == 0x7)
    {
        mmu_tlb_invalidate_all(state, CORTEX_A8_I_TLB);
        mmu_tlb_invalidate_all(state, CORTEX_A8_D_TLB);
        return;
    }

    /* Invalidate Inst-TLB and Data-TLB entry (MVA) */
    if (OPC_2 == 1 && CRm == 0x7)
    {
        mmu_tlb_invalidate_entry(state, CORTEX_A8_I_TLB, value);
        mmu_tlb_invalidate_entry(state, CORTEX_A8_D_TLB, value);
        return;
    }

    /* Invalidate Inst-TLB and Data-TLB (ASID) */
    if (OPC_2 == 2 && CRm == 0x7)
    {
    }

    /* Invalidate Data-TLB (ASID) */
    if (OPC_2 == 2 && CRm == 0x6)
    {
    }

    /* Invalidate Data-TLB entry (MVA) */
    if (OPC_2 == 1 && CRm == 0x6)
    {
        mmu_tlb_invalidate_entry(state, CORTEX_A8_D_TLB, value);
        return;
    }

    /* Invalidate Data-TLB */
    if (OPC_2 == 0 && CRm == 0x6)
    {
        mmu_tlb_invalidate_all(state, CORTEX_A8_D_TLB);
        return;
    }

    /* Invalidate Inst-TLB (ASID) */
    if (OPC_2 == 2 && CRm == 0x5)
    {
    }

    /* Invalidate Inst-TLB entry (MVA) */
    if (OPC_2 == 1 && CRm == 0x5)
    {
        mmu_tlb_invalidate_entry(state, CORTEX_A8_I_TLB, value);
        return;
    }

    /* Invalidate Inst-TLB */
    if (OPC_2 == 0 && CRm == 0x5)
    {
        mmu_tlb_invalidate_all(state, CORTEX_A8_I_TLB);
        return;
    }

    printf("Unknow OPC_2 = 0x%x CRm = 0x%x\tinstr: 0x%x\tpc: 0x%x\n", OPC_2, CRm, instr, state->pc);
}

static ARMword cortex_a8_mmu_mcr(ARMul_State * state, ARMword instr, ARMword value)
{
    mmu_regnum_t creg = BITS(16, 19) & 0xf;
    mmu_regnum_t CRm = BITS(0, 3) & 0xf;
    int OPC_1 = BITS(21, 23) & 0x7;
    int OPC_2 = BITS(5, 7) & 0x7;

    if (!strncmp(state->cpu->cpu_arch_name, "armv7", 5))
    {
        switch (creg)
        {
            case MMU_ID:
                if ((OPC_1 == 2) && (OPC_2 == 0))
                {
                    state->mmu.cache_ind = value & 0x1;
                    state->mmu.cache_level = (value >> 1) & 0x7;
                }
                break;
            case MMU_CONTROL:
                /*
                 * 6:3          read as 1
                 * 10           read as 0
                 * 18,16        read as 1
                 * 22           read as 1, is used for armv7 indicating use of the alignment model
                 * */
                if (OPC_2 == 0)
                    state->mmu.control = (value | 0xc50078) & 0xFFFFFBFF;
                else if (OPC_2 == 1)
                    state->mmu.auxiliary_control = value;
                else if (OPC_2 == 2)
                    state->mmu.coprocessor_access_control = value;
                else
                    fprintf(stderr, "In %s, wrong OPC_2 %d\n", __FUNCTION__, OPC_2);
                break;
            case MMU_TRANSLATION_TABLE_BASE:
                switch (OPC_2)
                {
                        /* int i; */
                    case 0:
#if 0
                        /* TTBR0 */
                        if (state->mmu.translation_table_ctrl & 0x7)
                        {
                            for (i = 0; i <= state->mmu.translation_table_ctrl; i++)
                                state->mmu.translation_table_base0 &= ~(1 << (5 + i));
                        }
#endif
                        state->mmu.translation_table_base0 = (value);
                        break;
                    case 1:
#if 0
                        /* TTBR1 */
                        if (state->mmu.translation_table_ctrl & 0x7)
                        {
                            for (i = 0; i <= state->mmu.translation_table_ctrl; i++)
                                state->mmu.translation_table_base1 &= 1 << (5 + i);
                        }
#endif
                        state->mmu.translation_table_base1 = (value);
                        break;
                    case 2:
                        /* TTBC */
                        state->mmu.translation_table_ctrl = value & 0x7;
                        break;
                    default:
                        printf("mmu_mcr wrote UNKNOWN - cp15 c2 opcode2 %d\n", OPC_2);
                        break;
                }
                //printf("SKYEYE In %s, write TLB_BASE 0x%x OPC_2=%d instr=0x%x\n", __FUNCTION__, value, OPC_2, instr);
                //invalidate_all_tlb(state);
                break;
            case MMU_DOMAIN_ACCESS_CONTROL:
                /* printf("mmu_mcr wrote DACR         "); */
                state->mmu.domain_access_control = value;
                break;

            case MMU_FAULT_STATUS:
                if (OPC_2 == 0)
                    state->mmu.fault_status = value & 0xFF;
                if (OPC_2 == 1)
                {
                    printf("set fault status instr\n");
                }
                break;
            case MMU_FAULT_ADDRESS:
                state->mmu.fault_address = value;
                break;

            case MMU_CACHE_OPS:
                break;
            case MMU_TLB_OPS:
                cortex_a8_mmu_tlb_ops(state, instr, value);
                break;
            case MMU_CACHE_LOCKDOWN:
                /*
                 * FIXME: cache lock down*/
                break;
            case MMU_TLB_LOCKDOWN:
                printf("SKYEYE In %s, write TLB_LOCKDOWN 0x%x OPC_2=%d instr=0x%x\n", __FUNCTION__, value, OPC_2,
                       instr);
                /* FIXME:tlb lock down */
                break;
            case MMU_PID:
                //printf("SKYEYE In %s, write pid 0x%x OPC_2=%d instr=0x%x\n", __FUNCTION__, value, OPC_2, instr);
                //state->mmu.process_id = value;
                /*0:24 should be zero. */
                //state->mmu.process_id = value & 0xfe000000;
                if (OPC_2 == 0)
                    state->mmu.process_id = value;
                else if (OPC_2 == 1)
                    state->mmu.context_id = value;
                else if (OPC_2 == 3)
                {
                    state->mmu.thread_uro_id = value;
                } else if (OPC_2 == 2)
                {
                    state->mmu.thread_urw_id = value;
                } else
                {
                    printf("mmu_mcr wrote UNKNOWN - reg %d\n", creg);
                }
                break;
            case VECTOR_BASE_ADDR:
                state->vector_remap_addr = (value & ~0x1f);
                /* Mark the vector_addr is remapped */
                state->vector_remap_flag = 1;
                printf("In %s, Line %d value 0x%x 0x%x\n", __func__, __LINE__, value, state->vector_remap_addr);
                break;
            default:
                printf("mmu_mcr wrote UNKNOWN - reg %d\n", creg);
                break;
        }
    }

    return No_exp;
}

static int cortex_a8_mmu_v2p_dbct(ARMul_State * state, ARMword virt_addr, ARMword * phys_addr)
{
    fault_t fault;
    int ap, sop;

    ARMword perm;                       /* physical addr access permissions */

    virt_addr = mmu_pid_va_map(virt_addr);
    if (MMU_Enabled)
    {

        /*align check */
        if ((virt_addr & (WORD_SIZE - 1)) && MMU_Aligned)
        {
            d_msg("align\n");
            return ALIGNMENT_FAULT;
        } else
            virt_addr &= ~(WORD_SIZE - 1);

        /*translate tlb */
        fault = mmu_translate(state, virt_addr, phys_addr, &ap, &sop);
        if (fault)
        {
            d_msg("translate\n");
            return fault;
        }

        /* permission check */
        if (!check_perms(state, ap, 1))
        {
            if (sop == 0)
            {
                return SECTION_PERMISSION_FAULT;
            } else
            {
                return SUBPAGE_PERMISSION_FAULT;
            }
        }
    }

    if (MMU_Disabled)
    {
        *phys_addr = virt_addr;
    }

    return 0;
}

mmu_ops_t cortex_a8_mmu_ops = {
    cortex_a8_mmu_init,
    cortex_a8_mmu_exit,
    cortex_a8_mmu_read_byte,
    cortex_a8_mmu_write_byte,
    cortex_a8_mmu_read_halfword,
    cortex_a8_mmu_write_halfword,
    cortex_a8_mmu_read_word,
    cortex_a8_mmu_write_word,
    cortex_a8_mmu_load_instr,
    cortex_a8_mmu_mcr,
    cortex_a8_mmu_mrc
};

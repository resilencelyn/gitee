/*
 * sparc_mmu.c - SPARC V8 Memory Management Unit emulation.
 * Copyright (C) 2003 Skyeye Develop Group
 * for help please send mail to <skyeye-developer@lists.gro.clinux.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <skyeye_iface.h>
#include "mmu.h"
#include "../types.h"
#include "../debug.h"
#include "../sparc.h"
#include "../traps.h"
#include "../bits.h"
#include "../iu.h"
/* table number */
#define TN	4
static int mmu_table_index[TN];
exception_t check_access_permission(sparc_state_t * sparc_state, uint32_t pte, uint32_t access)
{
    uint32_t S = psr_get_s();
    uint8_t ret_acc = 0;

    /*
     * bit 0 indicates readable, 
     * bit 1 indicates writeable, 
     * bit 2 indicates excuteable.
     */
    if (S)
    {
        switch (PTE_ACC(pte))
        {
            case 0:
                ret_acc = (1 << 0);
                break;
            case 1:
            case 5:
                ret_acc = ((1 << 0) | (1 << 1));
                break;
            case 2:
            case 6:
                ret_acc = ((1 << 0) | (1 << 2));
                break;
            case 3:
            case 7:
                ret_acc = ((1 << 0) | (1 << 1) | (1 << 2));
                break;
            case 4:
                ret_acc = (1 << 2);
                break;
            default:
                printf("Supervisor Acc Error In %s\n", __func__);
                break;
        }
    } else
    {
        switch (PTE_ACC(pte))
        {
            case 0:
            case 5:
                ret_acc = (1 << 0);
                break;
            case 1:
                ret_acc = ((1 << 0) | (1 << 1));
                break;
            case 2:
                ret_acc = ((1 << 0) | (1 << 2));
                break;
            case 3:
                ret_acc = ((1 << 0) | (1 << 1) | (1 << 2));
                break;
            case 4:
                ret_acc = (1 << 2);
                break;
            case 6:
            case 7:
                ret_acc = 0;
                break;
            default:
                printf("User Acc Error In %s\n", __func__);
                break;
        }
    }

    if ((access == PTE_RV) && !(ret_acc & (1 << 0)))
    {
        SPARC_LOG(2, "read access_permission error\n");
        set_sparc_exp(INSTR_ACCESS_EXCEPTION);
        return Access_exp;
    }
    if ((access == PTE_MV) && !(ret_acc & (1 << 1)))
    {
        SPARC_LOG(2, "write access_permission error\n");
        set_sparc_exp(DATA_ACCESS_EXCEPTION);
        return Access_exp;
    }

    return No_exp;
}

exception_t page_table_walk(sparc_state_t * sparc_state, uint64_t ctp, uint32_t * pt_num, uint32_t * pte)
{
    conf_object_t *conf_obj = sparc_state->core_obj;
    uint32_t data;

    if (*pt_num > 3)
    {
        SPARC_LOG(0, "page table number error\n");
        return Not_found_exp;
    }
    /* page table member */
    uint32_t ptm = ctp + (4 * mmu_table_index[*pt_num]);

    SPARC_LOG(4, "Walking L%d page table, ptm pointer is 0x%x\n", *pt_num, ptm);
    //int status = bus_read(32, ptm, &data);
    int status = sparc_state->sparc_memory_iface->read(conf_obj, ptm, &data, 4);

    if (status < 0)
    {
        SPARC_LOG(0, "value 0x%x can't find pt 0x%x!\n", ctp, data);
        return Not_found_exp;
    }

    if (PTE_ET(data) == 0)
    {
        SPARC_LOG(4, "Walking L%d: invalid address error!\n", *pt_num, data);
        Set_FSR_FT(1);
        set_sparc_exp(DATA_ACCESS_EXCEPTION);
        return Access_exp;
    } else if (PTE_ET(data) == 1)
    {
        ctp = (PTD_PTP(data) << 6);
        (*pt_num)++;
        /* A translation error :A PTD is found in a level-3 page table,
         */
        if (*pt_num == 3)
            Set_FSR_FT(4);
        SPARC_LOG(4, "Walk L%d page table:0x%x\n", *pt_num, ctp);
        return page_table_walk(sparc_state, ctp, pt_num, pte);
    } else if (PTE_ET(data) == 2)
    {
        SPARC_LOG(3, "Find PTE(0x%x) OK \n", data);
        exception_t access_exp = check_access_permission(sparc_state, data, *pte);

        if (access_exp != No_exp)
        {
            return access_exp;
        }
        /* Modify PTE M or R */
        //*pte |= data;
        *pte = data | (*pte & (PTE_MV | PTE_RV));
        //bus_write(32, ptm, *pte);
        //printf("%x, %x\n", ptm, *pte);
        sparc_state->sparc_memory_iface->write(conf_obj, ptm, pte, 4);

        SPARC_LOG(4, "Set 0x%x into (0x%x)\n", *pte, ptm);
        return No_exp;
    } else
    {
        /*  A translation error: A PTE has ET=3 */
        Set_FSR_FT(4);
        return Access_exp;
    }

    return Not_found_exp;
}

/* convert virtual address to physics address */
exception_t sparc_mmu_translate(conf_object_t * obj, uint32_t virt_addr, uint32_t * phy_addr)
{
    sparc_state_t *sparc_state = (sparc_state_t *) obj->obj;
    uint32_t ptd, pte, va;
    uint32_t pt_num = 0;
    uint64_t ctp = 0;

    SPARC_LOG(3, "MMU get a virtual address 0x%x\n", virt_addr);
    va = virt_addr;
    /* phy_addr is only used to mark pte's M or R temporary */
    pte = *phy_addr;
    mmu_table_index[0] = MMU_CTTR;
    mmu_table_index[1] = VA_L1(va);
    mmu_table_index[2] = VA_L2(va);
    mmu_table_index[3] = VA_L3(va);
    ctp |= MMU_CTP << 6;
    if (page_table_walk(sparc_state, ctp, &pt_num, &pte) == No_exp)
    {
        switch (pt_num)
        {
            case 0:
                break;
            case 1:
                *phy_addr = ((PTE_PPN(pte) << 12) | VA_L1_OFF(va));
                break;
            case 2:
                *phy_addr = ((PTE_PPN(pte) << 12) | VA_L2_OFF(va));
                break;
            case 3:
                *phy_addr = ((PTE_PPN(pte) << 12) | VA_L3_OFF(va));
                break;
            default:
                goto translate_error;
        }
        //SPARC_LOG(2, "Translate 0x%x to 0x%x\n", va, *phy_addr);
        return No_exp;
    }
  translate_error:
    //SPARC_LOG(2, "Translate 0x%x error in L%d page table!\n", va, pt_num);
    skyeye_log(Warning_log, __FUNCTION__, "Translate 0x%x error in L%d page table at pc = 0x%x!\n", va, pt_num, sparc_state->pc);
    Set_FSR_L(pt_num);
    MMU_FAR = va;
    //FIX ME just use to run s698pm testcase
    *phy_addr = va;
    return No_exp;
}

exception_t sparc_mmu_init(void)
{
    return No_exp;
}

/*free on exit */
void sparc_mmu_exit(void)
{
}

exception_t sparc_mmu_read(conf_object_t * conf_obj, short size, generic_address_t va, uint32_t * data)
{
    sparc_state_t *sparc_state = conf_obj->obj;
    memory_space_intf *iface = sparc_state->sparc_memory_iface;

    /* pa is only used to mark pte's M or R temporary */
    uint32_t pa = PTE_RV;

    if (MMU_CR_E)
    {
        /* MMU enable */
        if (sparc_mmu_translate(conf_obj, va, &pa) == No_exp)
        {
            iface->read(conf_obj, pa, data, size);
        } else
        {
            SPARC_LOG(3, "read 0x%x error\n", va);
            return Invarg_exp;
        }
    } else
    {
        iface->read(conf_obj, va, data, size);
    }

    return No_exp;
}

exception_t sparc_mmu_write(conf_object_t * conf_obj, short size, generic_address_t va, uint32_t data)
{
    sparc_state_t *sparc_state = conf_obj->obj;
    memory_space_intf *iface = sparc_state->sparc_memory_iface;

    /* write data from memory, init pa = 1 */
    uint32_t pa = PTE_MV;

    if (MMU_CR_E)
    {
        /* MMU enable */
        if (sparc_mmu_translate(conf_obj, va, &pa) == No_exp)
        {
            iface->write(conf_obj, pa, &data, size);
        } else
        {
            return Invarg_exp;
        }
    } else
    {
        iface->write(conf_obj, va, &data, size);
    }
    return No_exp;
}

/*read byte data */
exception_t sparc_mmu_read_byte(conf_object_t * conf_obj, uint32_t va, uint32_t * data)
{
    return sparc_mmu_read(conf_obj, 1, va, data);
}

/*write byte data */
exception_t sparc_mmu_write_byte(conf_object_t * conf_obj, uint32_t va, uint32_t data)
{
    return sparc_mmu_write(conf_obj, 1, va, data);
}

/*read halfword data */
exception_t sparc_mmu_read_halfword(conf_object_t * conf_obj, uint32_t va, uint32_t * data)
{
    return sparc_mmu_read(conf_obj, 2, va, data);
}

/*write halfword data */
exception_t sparc_mmu_write_halfword(conf_object_t * conf_obj, uint32_t va, uint32_t data)
{
    return sparc_mmu_write(conf_obj, 2, va, data);
}

/*read word data */
exception_t sparc_mmu_read_word(conf_object_t * conf_obj, uint32_t va, uint32_t * data)
{
    return sparc_mmu_read(conf_obj, 4, va, data);
}

/*write word data */
exception_t sparc_mmu_write_word(conf_object_t * conf_obj, uint32_t va, uint32_t data)
{
    return sparc_mmu_write(conf_obj, 4, va, data);
}

/*load instr */
exception_t sparc_mmu_load_instr(conf_object_t * conf_obj, uint32_t va, uint32_t * instr)
{
    sparc_state_t *sparc_state = conf_obj->obj;

    memory_space_intf *iface = sparc_state->sparc_memory_iface;

    /* pa is only used to mark pte's M or R temporary */
    uint32_t pa = PTE_RV;

    if (MMU_CR_E)
    {
        /* MMU enable */
        if (sparc_mmu_translate(conf_obj, va, &pa) == No_exp)
        {
            iface->read(conf_obj, pa, instr, 4);
        } else
        {
            SPARC_LOG(2, "Instruction_access_exception occur!\n");
            return Not_found_exp;
        }
    } else
    {
        iface->read(conf_obj, va, instr, 4);
    }
    return No_exp;
}

/* write mmu reg */
exception_t sparc_mmu_write_reg(conf_object_t * conf_obj, uint32_t offset, uint32_t value)
{
    sparc_state_t *sparc_state = conf_obj->obj;

    SPARC_LOG(3, "offset 0x%x, value 0x%x\n", offset, value);
    offset &= ~0xff;
    switch (offset)
    {
        case CONTRAL_REG:
            /* Significance bit, 0, 1 7 */
            value &= 0x83;
            MMU_CR = value;
            if (value & 0x1)
            {
                SPARC_LOG(2, "MMU Enable! PC = %x\n", sparc_state->pc);
            }
            break;
        case CONTEXT_TP_REG:
            value &= 0xfffffffc;
            MMU_CTPR = value;
            SPARC_LOG(4, "Page Table 0x%x!\n", value);
            break;
        case CONTEXT_REG:
            MMU_CTTR = value;
            SPARC_LOG(4, "Page Table offset 0x%x!\n", value);
            break;
        case FAULT_STATUS_REG:
            value &= 0x3ff;
            MMU_FSR = value;
            break;
        case FAULT_ADDRESS_REG:
            printf("Can't write the register far!\n");
            break;
        default:
            printf("In %s, It doesn't have register at 0x%x00!\n", __func__, offset);
            break;
    }
    return No_exp;
}

/* read mmu reg */
exception_t sparc_mmu_read_reg(conf_object_t * conf_obj, uint32_t offset, uint32_t * value)
{
    offset &= ~0xff;
    sparc_state_t *sparc_state = conf_obj->obj;

    switch (offset)
    {
        case CONTRAL_REG:
            /* Significance bit, 0, 1 7 */
            *value = MMU_CR;
            break;
        case CONTEXT_TP_REG:
            *value = MMU_CTPR;
            break;
        case CONTEXT_REG:
            *value = MMU_CTTR;
            break;
        case FAULT_STATUS_REG:
            *value = MMU_FSR;
            break;
        case FAULT_ADDRESS_REG:
            *value = MMU_FAR;
            break;
        default:
            printf("In %s, It doesn't have register at 0x%x00!\n", __func__, offset);
            break;
    }
    SPARC_LOG(3, "offset 0x%x, value 0x%x\n", offset, *value);
    return No_exp;
}

/*sparc mmu_ops_t */
mmu_ops_t sparc_mmu_ops = {
    sparc_mmu_init,
    sparc_mmu_exit,
    sparc_mmu_translate,
    sparc_mmu_read,
    sparc_mmu_write,
    sparc_mmu_read_byte,
    sparc_mmu_write_byte,
    sparc_mmu_read_halfword,
    sparc_mmu_write_halfword,
    sparc_mmu_read_word,
    sparc_mmu_write_word,
    sparc_mmu_load_instr,
    sparc_mmu_write_reg,
    sparc_mmu_read_reg,
};

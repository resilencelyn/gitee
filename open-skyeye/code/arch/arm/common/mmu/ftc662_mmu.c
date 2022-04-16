/*
 * ftc662_mmu.c - ftc662 Memory Management Unit emulation.
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

#include <assert.h>
#include <string.h>
#include "bank_defs.h"
#include "arch64_memory.h"
#include "arch64_exception.h"
#include "ftc662_mmu.h"
#include <skyeye_ram.h>
#include <sim_control.h>

MemAttrHints ShortConvertAttrsHints(ARMul_State * state, ARMword rgn, AccType accType, bool_t secondstage)
{
    MemAttrHints result = {0,0,0};

    if ((!secondstage && S1CacheDisabled(state, accType)) || (secondstage && S2CacheDisabled(state, accType)))
    {
        result.attrs = MemAttr_NC;
        result.hints = MemHint_No;
    } else
    {
        switch (rgn)
        {
            case 0:
                result.attrs = MemAttr_NC;
                result.hints = MemHint_No;
                break;
            case 1:
                result.attrs = MemAttr_WB;
                result.hints = MemHint_RWA;
                break;
            case 2:
                result.attrs = MemAttr_WT;
                result.hints = MemHint_RA;
                break;
            case 3:
                result.attrs = MemAttr_WB;
                result.hints = MemHint_RA;
                break;
        }
    }
    result.transient = False;

    return result;
}

MemoryAttributes WalkAttrDecode(ARMul_State * state, ARMword sh, ARMword orgn, ARMword irgn, bool_t secondstage)
{
    MemoryAttributes memattrs;
    AccType accType = AccType_NORMAL;

    memattrs.memtype = MemType_Normal;
    memattrs.inner = ShortConvertAttrsHints(state, irgn, accType, secondstage);
    memattrs.outer = ShortConvertAttrsHints(state, orgn, accType, secondstage);
    memattrs.shareable = ((sh >> 1) & 0x1) == 1;
    memattrs.outershareable = sh == 2;
    memattrs.tagged = False;
    memattrs.device = DeviceType_GRE;

    if (memattrs.memtype == MemType_Device)
    {
        memattrs.shareable = True;
        memattrs.outershareable = True;
    } else
    {
        if (memattrs.inner.attrs == MemAttr_NC && memattrs.outer.attrs == MemAttr_NC)
        {
            memattrs.shareable = True;
            memattrs.outershareable = True;
        }
    }

    return memattrs;
}

TLBRecord TranslateTableWalk(ARMul_State * state, ARMdword paddress, ARMdword vaddress, AccType accType,
                             bool_t secondstage, int size)
{
    TLBRecord S1;
    AddressDescriptor descAddr;
    ARMdword baseReg, inputAddr;        // Input Address is 'vaddress' for stage 1, 'ipaddress' for stage 2
    ARMword nswalk, el, inputsize, inputsize_max, inputsize_min, ps, frstblocklevel, level, stride, grainsize;  // Stage 2 translation table walks are to Secure or to Non-secure PA
    bool_t largegrain, midgrain, reversedescriptors, lookupsecure, singlepriv, update_AF, update_AP, hierattrsdisabled;

    descAddr.memattrs.memtype = MemType_Normal;
    if (!secondstage)
    {
        //First stage translation
        inputAddr = vaddress;
        el = AccessUsesEL(state, accType);
        if (el == EL2)
        {
            inputsize = 64 - state->system_reg.TCR_EL2.T0SZ;
            largegrain = state->system_reg.TCR_EL2.TG0 == 1;
            midgrain = state->system_reg.TCR_EL2.TG0 == 2;
            inputsize_max = largegrain ? 52 : 48;
            inputsize_min = largegrain ? 47 : 48;
            baseReg = state->system_reg.TTBR0_EL2;
            ps = state->system_reg.TCR_EL2.PS;
            descAddr.memattrs =
                WalkAttrDecode(state, state->system_reg.TCR_EL2.SH0, state->system_reg.TCR_EL2.ORGN0,
                               state->system_reg.TCR_EL2.IRGN0, secondstage);
            reversedescriptors = state->system_reg.SCTLR_EL2.EE;
            lookupsecure = False;
            singlepriv = True;
            update_AF = state->system_reg.TCR_EL2.HA;
            update_AP = state->system_reg.TCR_EL2.HD && update_AF;
            hierattrsdisabled = state->system_reg.TCR_EL2.HPD;
        } else
        {

        }

        if (largegrain)
        {
            //Log2(64KB page size), Largest block is 512MB (2^29 bytes)
            grainsize = 16;
            frstblocklevel = 2;
        } else if (midgrain)
        {
            //Log2(16KB page size), Largest block is 32MB (2^25 bytes)
            grainsize = 14;
            frstblocklevel = 2;
        } else
        {
            //Log2(4KB page size)  Largest block is 1GB (2^30 bytes)
            grainsize = 12;
            frstblocklevel = 1;
        }
        stride = grainsize - 3;         //Log2(page size / 8 bytes)
        level = (4 - (1 + (inputsize - grainsize - 1))) / stride;
    } else
    {
        //Second stage translation
    }
}

AddressDescriptor FirstStageTranslate(ARMul_State * state, ARMdword vaddress, AccType accType, int size)
{
    bool_t s1_enabled = False;
    bool_t secondstage = False;
    TLBRecord S1;
    ARMdword paddress = 0;

    if (accType == AccType_NV2REGISTER)
        s1_enabled = state->system_reg.SCTLR_EL2.M == 1;
    else if (HasS2Translation(state))
        s1_enabled = state->system_reg.HCR_EL2.TGE == 0 && state->system_reg.HCR_EL2.DC == 0
            && state->system_reg.SCTLR_EL1.M == 1;
    else
    {
        SCTLR_reg_t sctlr = getSCTLR(state);

        s1_enabled = sctlr.M == 1;
    }

    if (s1_enabled)
    {
        S1 = TranslateTableWalk(state, paddress, vaddress, accType, secondstage, size);
    }
}

AddressDescriptor SecondStageTranslate(ARMul_State * state, AddressDescriptor S1, ARMdword address, AccType accType,
                                       bool_t s2fs1walk, int size, bool_t hwupdatewalk)
{

}

AddressDescriptor FullTranslate(ARMul_State * state, ARMdword address, AccType accType, int size)
{
    AddressDescriptor result, S1;

    // First Stage Translation
    S1 = FirstStageTranslate(state, address, accType, size);

    if (!IsFault(S1) && !(accType == AccType_NV2REGISTER) && HasS2Translation(state))
    {
        bool_t s2fs1walk = False;
        bool_t hwupdatewalk = False;

        result = SecondStageTranslate(state, S1, address, accType, s2fs1walk, size, hwupdatewalk);
    } else
        result = S1;

    return S1;
}

AddressDescriptor TranslateAddress(ARMul_State * state, ARMdword address, AccType accType, int size)
{
    AddressDescriptor result;

    result = FullTranslate(state, address, accType, size);

    result.vaddress = address;

    return result;
}

#define BANK0_START 0x80000000
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

static inline void mem_write_raw(int size, uint32_t offset, uint32_t value)
{
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

    if (state->exclusive_tag_array[0] == addr)
        return 0;
    else
        return -1;
}

static inline void add_exclusive_addr(ARMul_State * state, ARMword addr)
{
    int i;

    state->exclusive_tag_array[0] = addr;
    return;
}

static inline void remove_exclusive(ARMul_State * state, ARMword addr)
{
    state->exclusive_tag_array[0] = 0xFFFFFFFF;
}

/* This function encodes table 8-2 Interpreting AP bits,
 * returning non-zero if access is allowed. */
static int check_perms(ARMul_State * state, int ap, int read)
{
    int s, r, user;

    s = state->mmu.SCTLR.all & CONTROL_SYSTEM;
    r = state->mmu.SCTLR.all & CONTROL_ROM;
    /* chy 2006-02-15 , should consider system mode, don't conside 26bit mode */
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

static fault_t mmu_arch64_init_lookup_level(ARMul_State * state, int * level, int ttbr_idx)
{
    if (ttbr_idx == 0)
    {
        if(state->system_reg.TCR_EL1.EPD0)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD0: 0x%llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD0, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG0)
        {
            case 0x0: /* 4kb granule size */
                switch(state->system_reg.TCR_EL1.T0SZ)
                {
                    case 16 ... 24:
                        *level = 0;
                        break;
                    case 25 ... 33:
                        *level = 1;
                        break;
                    case 34 ... 42:
                        *level = 2;
                        break;
                    case 43 ... 48:
                        *level = 3;
                        break;
                    default:
                        printf("In %s, Line : %d, 4kb granule size | T0SZ %d at PC = 0x%x!\n", __func__, __LINE__, state->system_reg.TCR_EL1.T0SZ, state->realPC);
                        return PAGE_TRANSLATION_FAULT;
                }
                break;
            case 0x2: /* 16kb granule size */
                switch(state->system_reg.TCR_EL1.T0SZ)
                {
                    case 16:
                        *level = 0;
                        break;
                    case 17 ... 27:
                        *level = 1;
                        break;
                    case 28 ... 38:
                        *level = 2;
                        break;
                    case 39 ... 48:
                        *level = 3;
                        break;
                    default:
                        printf("In %s, Line : %d, 4kb granule size | T0SZ %d at PC = 0x%x!\n", __func__, __LINE__, state->system_reg.TCR_EL1.T0SZ, state->realPC);
                        return PAGE_TRANSLATION_FAULT;
                }
                break;
            case 0x1: /* 64kb granule size */
                switch(state->system_reg.TCR_EL1.T0SZ)
                {
                    case 12 ... 15:   /* Supported only if ARMv8.2-LVA is implemented and the 64KB translation granule is used */
                        *level = 1;
                        break;
                    case 16 ... 21:
                        *level = 1;
                        break;
                    case 22 ... 34:
                        *level = 2;
                        break;
                    case 35 ... 47:
                        *level = 3;
                        break;
                    default:
                        printf("In %s, Line : %d, 4kb granule size | T0SZ %d at PC = 0x%x!\n", __func__, __LINE__, state->system_reg.TCR_EL1.T0SZ, state->realPC);
                        return PAGE_TRANSLATION_FAULT;
                }
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG0 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG0, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
        
    } else if (ttbr_idx == 1)
    {
        if(state->system_reg.TCR_EL1.EPD1 == 0)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD1: 0x%llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD1, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG1)
        {
            case 0x2: /* 4kb granule size */
                switch(state->system_reg.TCR_EL1.T1SZ)
                {
                    case 16 ... 24:
                        *level = 0;
                        break;
                    case 25 ... 33:
                        *level = 1;
                        break;
                    case 34 ... 42:
                        *level = 2;
                        break;
                    case 43 ... 48:
                        *level = 3;
                        break;
                    default:
                        printf("In %s, Line : %d, 4kb granule size | T1SZ %d at PC = 0x%x!\n", __func__, __LINE__, state->system_reg.TCR_EL1.T1SZ, state->realPC);
                        return PAGE_TRANSLATION_FAULT;
                }
                break;
            case 0x1: /* 16kb granule size */
                switch(state->system_reg.TCR_EL1.T1SZ)
                {
                    case 16:
                        *level = 0;
                        break;
                    case 17 ... 27:
                        *level = 1;
                        break;
                    case 28 ... 38:
                        *level = 2;
                        break;
                    case 39 ... 48:
                        *level = 3;
                        break;
                    default:
                        printf("In %s, Line : %d, 4kb granule size | T1SZ %d at PC = 0x%x!\n", __func__, __LINE__, state->system_reg.TCR_EL1.T1SZ, state->realPC);
                        return PAGE_TRANSLATION_FAULT;
                }
                break;
            case 0x3: /* 64kb granule size */
                switch(state->system_reg.TCR_EL1.T1SZ)
                {
                    case 12 ... 15:   /* Supported only if ARMv8.2-LVA is implemented and the 64KB translation granule is used */
                        *level = 1;
                        break;
                    case 16 ... 21:
                        *level = 1;
                        break;
                    case 22 ... 34:
                        *level = 2;
                        break;
                    case 35 ... 47:
                        *level = 3;
                        break;
                    default:
                        printf("In %s, Line : %d, 4kb granule size | T1SZ %d at PC = 0x%x!\n", __func__, __LINE__, state->system_reg.TCR_EL1.T1SZ, state->realPC);
                        return PAGE_TRANSLATION_FAULT;
                }
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG1 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG1, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
    }
    return NO_FAULT;    
}

static fault_t mmu_arch64_get_pte_desc(ARMul_State * state, ARMdword input_addr, ARMdword * output_addr)
{
    ARMword tmp_low32 = 0, tmp_high32 = 0;
    fault_t ret = NO_FAULT;
    if (state->space.conf_obj != NULL)
    {
        ret = arch64_mem_read(state, input_addr, AccType_NORMAL, &tmp_low32, &tmp_high32, 8);
        if (ret)
        {
            printf("In %s, Line : %d, tlb_entry 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
            return ret;
        }
        *output_addr = ((uint64_t)(tmp_high32) << 32) | tmp_low32;
        //printf("In %s, Line : %d, tlb_entry 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
    }
    return NO_FAULT;
}

static fault_t mmu_arch64_get_l0_pte_addr(ARMul_State * state, ARMdword virt_addr, ARMdword * output_addr, int ttbr_idx)
{
    int x = 0, y = 0;
    if (ttbr_idx == 0)
    {
        if(state->system_reg.TCR_EL1.EPD0 != 0)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD0: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD0, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG0)
        {
            case 0x0: /* 4kb granule size */
                /* Stage 1 translation of the level 0:
                * BaseAddr[PAMax-1:x] : IA[y:39] : 0b000
                * if z ≤ TnSZ ≤ 24, then x = (28 - TnSZ)
                * y = (x + 35)
                * z = 16
                */
                if (state->system_reg.TCR_EL1.T0SZ >= 16 && state->system_reg.TCR_EL1.T0SZ <= 24)
                {
                    x = 28 - state->system_reg.TCR_EL1.T0SZ;
                }
                y = 35 + x;
                *output_addr = (REG64_BITS(state->system_reg.TTBR0_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 39, y) << 3);
                //printf("In %s, Line : %d, 4kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            case 0x2: /* 16kb granule size */
                /* Stage 1 translation of the level 1:
                * BaseAddr[PAMax-1:4] : IA[47] : 0b000
                * 16 = TnSZ
                */
                if (state->system_reg.TCR_EL1.T0SZ == 16)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR0_EL1, x, 4) << 4) | (REG64_BIT(virt_addr, 47) << 3);
                } else
                {
                    printf("In %s, Line: %d, 16kb granule size | error T0SZ %d at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.T0SZ, state->realPC);
                    return PAGE_TRANSLATION_FAULT;                        
                }
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG0 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG0, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
    } else if (ttbr_idx == 1)
    {
        if(state->system_reg.TCR_EL1.EPD1 == 0)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD1: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD1, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG1)
        {
            case 0x2: /* 4kb granule size */
                /* Stage 1 translation of the level 0:
                * BaseAddr[PAMax-1:x] : IA[y:39] : 0b000
                * if z ≤ TnSZ ≤ 24, then x = (28 - TnSZ)
                * y = (x + 35)
                * z = 16
                */
                if (state->system_reg.TCR_EL1.T1SZ >= 16 && state->system_reg.TCR_EL1.T1SZ <= 24)
                {
                    x = 28 - state->system_reg.TCR_EL1.T0SZ;
                }
                y = 35 + x;
                *output_addr = (REG64_BITS(state->system_reg.TTBR1_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 39, y) << 3);
                //printf("In %s, Line : %d, 4kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            case 0x1: /* 16kb granule size */
                /* Stage 1 translation of the level 1:
                * BaseAddr[PAMax-1:4] : IA[47] : 0b000
                * 16 = TnSZ
                */
                if (state->system_reg.TCR_EL1.T1SZ == 16)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR1_EL1, x, 4) << 4) | (REG64_BIT(virt_addr, 47) << 3);
                } else
                {
                    printf("In %s, Line: %d, 16kb granule size | error T1SZ %d at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.T1SZ, state->realPC);
                    return PAGE_TRANSLATION_FAULT;                        
                }
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG1 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG1, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
    }
    return NO_FAULT;
}

static fault_t mmu_arch64_get_l1_pte_addr(ARMul_State * state, ARMdword virt_addr, ARMdword input_addr, ARMdword * output_addr, int level, int ttbr_idx)
{
    int x = 0, y = 0, z = 0, pa_max = 0;
    if (ttbr_idx == 0)
    {
        if(state->system_reg.TCR_EL1.EPD0)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD0: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD0, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG0)
        {
            case 0x0: /* 4kb granule size */
                /* Stage 1 translation of the level 1:
                * BaseAddr[PAMax-1:x] : IA[y:30] : 0b000
                * if a 25 ≤ TnSZ ≤ 33, then x = (37 - TnSZ), else x = 12
                * y = (x + 26)
                */
                if (state->system_reg.TCR_EL1.T0SZ >= 25 && state->system_reg.TCR_EL1.T0SZ <= 33)
                {
                    x = 37 - state->system_reg.TCR_EL1.T0SZ;
                } else
                {
                    x = 12;
                }
                y = 26 + x;
                if(level == 1)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR0_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 30, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, 47) << x) | (REG64_BITS(virt_addr, 30, y) << 3);
                }
                //printf("In %s, Line : %d, 4kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            case 0x2: /* 16kb granule size */
                /* Stage 1 translation of the level 1:
                * BaseAddr[PAMax-1:x] : IA[y:36] : 0b000
                * if a 17 ≤ TnSZ ≤ 27, then x = (31 - TnSZ), else x = 14
                * y = (x + 32)
                */
                if (state->system_reg.TCR_EL1.T0SZ >= 17 && state->system_reg.TCR_EL1.T0SZ <= 27)
                {
                    x = 31 - state->system_reg.TCR_EL1.T0SZ;
                } else
                {
                    x = 14;
                }
                y = 32 + x;
                if(level == 1)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR0_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 36, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, 47) << x) | (REG64_BITS(virt_addr, 36, y) << 3);
                }
                //printf("In %s, Line : %d, 16kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            case 0x1: /* 64kb granule size */
                /* Stage 1 translation of the level 1:
                * BaseAddr[PAMax-1:x] : IA[y:42] : 0b000
                * if a z ≤ TnSZ ≤ 21, then x = (25 - TnSZ)
                * z = 16 or 12
                * y = (x + 38)
                */
                if (state->system_reg.ID_AA64MMFR0_EL1&0x07 == 0x06)
                {
                    z = 16;
                    pa_max = 51;
                } else
                {
                    z = 12;
                    pa_max = 47;
                }
                if (state->system_reg.TCR_EL1.T0SZ >= z && state->system_reg.TCR_EL1.T0SZ <= 42)
                {
                    x = 25 - state->system_reg.TCR_EL1.T0SZ;
                }
                y = 38 + x;
                if(level == 1)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR0_EL1, x, pa_max) << x) | (REG64_BITS(virt_addr, 42, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, pa_max) << x) | (REG64_BITS(virt_addr, 42, y) << 3);
                }
                //printf("In %s, Line : %d, 64kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG0 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG0, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
    } else if (ttbr_idx == 1)
    {
        if(state->system_reg.TCR_EL1.EPD1)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD1: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD1, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG1)
        {
            case 0x2: /* 4kb granule size */
                /* Stage 1 translation of the level 1:
                * BaseAddr[PAMax-1:x] : IA[y:30] : 0b000
                * if a 25 ≤ TnSZ ≤ 33, then x = (37 - TnSZ), else x = 12
                * y = (x + 26)
                */
                if (state->system_reg.TCR_EL1.T1SZ >= 25 && state->system_reg.TCR_EL1.T1SZ <= 33)
                {
                    x = 37 - state->system_reg.TCR_EL1.T1SZ;
                } else
                {
                    x = 12;
                }
                y = 26 + x;
                if(level == 1)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR1_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 30, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, 47) << x) | (REG64_BITS(virt_addr, 30, y) << 3);
                }
                //printf("In %s, Line : %d, 4kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            case 0x1: /* 16kb granule size */
                /* Stage 1 translation of the level 1:
                * BaseAddr[PAMax-1:x] : IA[y:36] : 0b000
                * if a 17 ≤ TnSZ ≤ 27, then x = (31 - TnSZ), else x = 14
                * y = (x + 32)
                */
                if (state->system_reg.TCR_EL1.T1SZ >= 17 && state->system_reg.TCR_EL1.T1SZ <= 27)
                {
                    x = 31 - state->system_reg.TCR_EL1.T1SZ;
                } else
                {
                    x = 14;
                }
                y = 32 + x;
                if(level == 1)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR1_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 36, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, 47) << x) | (REG64_BITS(virt_addr, 36, y) << 3);
                }
                //printf("In %s, Line : %d, 16kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            case 0x3: /* 64kb granule size */
                /* Stage 1 translation of the level 1:
                * BaseAddr[PAMax-1:x] : IA[y:42] : 0b000
                * if a z ≤ TnSZ ≤ 21, then x = (25 - TnSZ)
                * z = 16 or 12
                * y = (x + 38)
                */
                if (state->system_reg.ID_AA64MMFR0_EL1&0x07 == 0x06)
                {
                    z = 16;
                    pa_max = 51;
                } else
                {
                    z = 12;
                    pa_max = 47;
                }
                if (state->system_reg.TCR_EL1.T1SZ >= z && state->system_reg.TCR_EL1.T1SZ <= 42)
                {
                    x = 25 - state->system_reg.TCR_EL1.T1SZ;
                }
                y = 38 + x;
                if(level == 1)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR1_EL1, x, pa_max) << x) | (REG64_BITS(virt_addr, 42, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, pa_max) << x) | (REG64_BITS(virt_addr, 42, y) << 3);
                }
                //printf("In %s, Line : %d, 64kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG1 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG1, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
    }
    return NO_FAULT;
}

static fault_t mmu_arch64_get_l1_phys_addr(ARMul_State * state, ARMdword virt_addr, ARMdword input_addr, ARMdword * phys_addr, int ttbr_idx)
{
    if (ttbr_idx == 0)
    {
        if(state->system_reg.TCR_EL1.EPD0)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD0: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD0, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG0)
        {
            case 0x0: /* 4kb granule size */
                *phys_addr = (input_addr & 0x0000FFFFFFC0000000ULL) | (virt_addr & 0x3FFFFFFFULL);
                //printf("In %s, Line : %d, 4kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG0 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG0, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
    } else if (ttbr_idx == 1)
    {
        if(state->system_reg.TCR_EL1.EPD1)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD1: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD1, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG1)
        {
            case 0x2: /* 4kb granule size */
                *phys_addr = (input_addr & 0x0000FFFFFFC0000000ULL) | (virt_addr & 0x3FFFFFFFULL);
                //printf("In %s, Line : %d, 4kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG1 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG1, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
    }
    return NO_FAULT;
}

static fault_t mmu_arch64_get_l2_pte_addr(ARMul_State * state, ARMdword virt_addr, ARMdword input_addr, ARMdword * output_addr, int level, int ttbr_idx)
{
    int x = 0, y = 0, pa_max = 0;
    if (ttbr_idx == 0)
    {
        if(state->system_reg.TCR_EL1.EPD0)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD0: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD0, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG0)
        {
            case 0x0: /* 4kb granule size */
                /* Stage 1 translation of the level 2:
                * BaseAddr[PAMax-1:x] : IA[y:21] : 0b000
                * if a 34 ≤ TnSZ ≤ 39, then x = (46 - TnSZ), else x = 12
                * y = (x + 17)
                */
                if (state->system_reg.TCR_EL1.T0SZ >= 34 && state->system_reg.TCR_EL1.T0SZ <= 39)
                {
                    x = 46 - state->system_reg.TCR_EL1.T0SZ;
                } else
                {
                    x = 12;
                }
                y = 17 + x;
                if(level == 2)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR0_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 21, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, 47) << x) | (REG64_BITS(virt_addr, 21, y) << 3);
                }
                break;
            case 0x2: /* 16kb granule size */
                /* Stage 1 translation of the level 2:
                * BaseAddr[PAMax-1:x] : IA[y:25] : 0b000
                * if a 28 ≤ TnSZ ≤ 38, then x = (42 - TnSZ), else x = 14
                * y = (x + 21)
                */
                if (state->system_reg.TCR_EL1.T0SZ >= 28 && state->system_reg.TCR_EL1.T0SZ <= 38)
                {
                    x = 42 - state->system_reg.TCR_EL1.T0SZ;
                } else
                {
                    x = 14;
                }
                y = 21 + x;
                if(level == 2)
                {
                    *output_addr = ((state->system_reg.TTBR0_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 25, y) << 3);
                } else
                {
                    *output_addr = ((input_addr, x, 47) << x) | (REG64_BITS(virt_addr, 25, y) << 3);   
                }   
                //printf("In %s, Line : %d, 16kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            case 0x1: /* 64kb granule size */
                /* Stage 1 translation of the level 2:
                * BaseAddr[PAMax-1:x] : IA[y:22] : 0b000
                * if a 29 ≤ TnSZ ≤ 34, then x = (38 - TnSZ), else x = 16
                * y = (x + 25)
                */
                if (state->system_reg.ID_AA64MMFR0_EL1 & 0x07 == 0x06)
                {
                    pa_max = 51;
                } else
                {
                    pa_max = 47;
                }
                if (state->system_reg.TCR_EL1.T0SZ >= 29 && state->system_reg.TCR_EL1.T0SZ <= 34)
                {
                    x = 38 - state->system_reg.TCR_EL1.T0SZ;
                } else
                {
                    x = 16;
                }
                y = 25 + x;
                if(level == 2)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR0_EL1, x, pa_max) << x) | (REG64_BITS(virt_addr, 22, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, pa_max) << x) | (REG64_BITS(virt_addr, 22, y) << 3);   
                }
                //printf("In %s, Line : %d, 64kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG0 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG0, state->realPC);
                return PAGE_TRANSLATION_FAULT;     
        }
    } else if (ttbr_idx == 1)
    {
        if(state->system_reg.TCR_EL1.EPD1)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD1: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD1, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG1)
        {
            case 0x2: /* 4kb granule size */
                /* Stage 1 translation of the level 2:
                * BaseAddr[PAMax-1:x] : IA[y:21] : 0b000
                * if a 34 ≤ TnSZ ≤ 39, then x = (46 - TnSZ), else x = 12
                * y = (x + 17)
                */
                if (state->system_reg.TCR_EL1.T0SZ >= 34 && state->system_reg.TCR_EL1.T0SZ <= 39)
                {
                    x = 46 - state->system_reg.TCR_EL1.T0SZ;
                } else
                {
                    x = 17;
                }
                y = 17 + x;
                if(level == 2)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR1_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 21, y) << 3);  
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, 47) << x) | (REG64_BITS(virt_addr, 21, y) << 3);
                }
                //printf("In %s, Line : %d, 4kb granule size | pte_addr[2] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            case 0x1: /* 16kb granule size */
                /* Stage 1 translation of the level 2:
                * BaseAddr[PAMax-1:x] : IA[y:25] : 0b000
                * if a 28 ≤ TnSZ ≤ 38, then x = (42 - TnSZ), else x = 14
                * y = (x + 21)
                */
                if (state->system_reg.TCR_EL1.T1SZ >= 28 && state->system_reg.TCR_EL1.T1SZ <= 38)
                {
                    x = 42 - state->system_reg.TCR_EL1.T1SZ;
                } else
                {
                    x = 14;
                }
                y = 21 + x;
                if(level == 2)
                {
                    *output_addr = ((state->system_reg.TTBR1_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 25, y) << 3);
                } else
                {
                    *output_addr = ((input_addr, x, 47) << x) | (REG64_BITS(virt_addr, 25, y) << 3);
                }
                //printf("In %s, Line : %d, 16kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            case 0x3: /* 64kb granule size */
                /* Stage 1 translation of the level 2:
                * BaseAddr[PAMax-1:x] : IA[y:22] : 0b000
                * if a 29 ≤ TnSZ ≤ 34, then x = (38 - TnSZ), else x = 16
                * y = (x + 25)
                */
                if (state->system_reg.ID_AA64MMFR0_EL1&0x07 == 0x06)
                {
                    pa_max = 51;
                } else
                {
                    pa_max = 47;
                }
                if (state->system_reg.TCR_EL1.T1SZ >= 29 && state->system_reg.TCR_EL1.T1SZ <= 34)
                {
                    x = 38 - state->system_reg.TCR_EL1.T1SZ;
                } else
                {
                    x = 16;
                }
                y = 25 + x;
                if(level == 2)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR1_EL1, x, pa_max) << x) | (REG64_BITS(virt_addr, 22, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, pa_max) << x) | (REG64_BITS(virt_addr, 22, y) << 3);
                }
                //printf("In %s, Line : %d, 64kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG1 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG1, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
    }
    return NO_FAULT;
}

static fault_t mmu_arch64_get_l2_phys_addr(ARMul_State * state, ARMdword virt_addr, ARMdword input_addr, ARMdword * phys_addr, int ttbr_idx)
{
    if (ttbr_idx == 0)
    {
        if(state->system_reg.TCR_EL1.EPD0)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD0: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD0, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG0)
        {
            case 0x0: /* 4kb granule size */     
                *phys_addr = (input_addr & 0x0000FFFFFFE00000ULL) | (virt_addr & 0x001FFFFFULL);
                //printf("In %s, Line : %d, 4kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            case 0x2: /* 16kb granule size */
                *phys_addr = (input_addr & 0x0000FFFFFE000000ULL) | (virt_addr & 0x01FFFFFFULL);
                printf("In %s, Line : %d, 16kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            case 0x1: /* 64kb granule size */
                *phys_addr = (input_addr & 0x0000FFFFE0000000ULL) | (virt_addr & 0x1FFFFFFFULL);
                printf("In %s, Line : %d, 64kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG0 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG0, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
    } else if (ttbr_idx == 1)
    {
        if(state->system_reg.TCR_EL1.EPD1)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD1: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD1, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG1)
        {
            case 0x2: /* 4kb granule size */
                *phys_addr = (input_addr & 0x0000FFFFFFE00000ULL) | (virt_addr & 0x001FFFFFULL);
                //printf("In %s, Line : %d, 4kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            case 0x1: /* 16kb granule size */
                *phys_addr = (input_addr & 0x0000FFFFFE000000ULL) | (virt_addr & 0x01FFFFFFULL);
                printf("In %s, Line : %d, 16kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            case 0x3: /* 64kb granule size */
                *phys_addr = (input_addr & 0x0000FFFFE0000000ULL) | (virt_addr & 0x1FFFFFFFULL);
                printf("In %s, Line : %d, 64kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG1 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG1, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
    }
    return NO_FAULT;
}

static fault_t mmu_arch64_get_l3_pte_addr(ARMul_State * state, ARMdword virt_addr, ARMdword input_addr, ARMdword * output_addr, int level, int ttbr_idx)
{
    int x = 0, y = 0, pa_max = 0;
    if (ttbr_idx == 0)
    {
        if(state->system_reg.TCR_EL1.EPD0)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD0: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD0, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG0)
        {
            case 0x0: /* 4kb granule size */
                /* Stage 1 translation of the level 3:
                * BaseAddr[PAMax-1:x] : IA[y:12] : 0b000
                * if a 43 ≤ TnSZ ≤ 48, then x = (55 - TnSZ), else x = 12
                * y = (x + 17)
                */
                if (state->system_reg.TCR_EL1.T0SZ >= 43 && state->system_reg.TCR_EL1.T0SZ <= 48)
                {
                    x = 55 - state->system_reg.TCR_EL1.T0SZ;
                } else
                {
                    x = 12;
                }
                y = 8 + x;
                if(level == 3)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR0_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 12, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, 47) << x) | (REG64_BITS(virt_addr, 12, y) << 3);
                }
                //printf("In %s, Line : %d, 4kb granule size | pte_addr[2] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            case 0x2: /* 16kb granule size */
                /* Stage 1 translation of the level 3:
                * BaseAddr[PAMax-1:x] : IA[y:14] : 0b000
                * if a 39 ≤ TnSZ ≤ 48, then x = (53 - TnSZ), else x = 14
                * y = (x + 10)
                */
                if (state->system_reg.TCR_EL1.T0SZ >= 39 && state->system_reg.TCR_EL1.T0SZ <= 48)
                {
                    x = 53 - state->system_reg.TCR_EL1.T0SZ;
                } else
                {
                    x = 14;
                }
                y = 10 + x;
                if(level == 3)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR0_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 14, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, 47) << x) | (REG64_BITS(virt_addr, 14, y) << 3);
                }
                //printf("In %s, Line : %d, 16kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            case 0x1: /* 64kb granule size */
                /* Stage 1 translation of the level 3:
                * BaseAddr[PAMax-1:x] : IA[y:16] : 0b000
                * if a 35 ≤ TnSZ ≤ 47, then x = (51 - TnSZ), else x = 16
                * y = (x + 12)
                */
                if (state->system_reg.ID_AA64MMFR0_EL1 & 0x07 == 0x06)
                {
                    pa_max = 51;
                } else
                {
                    pa_max = 47;
                }
                if (state->system_reg.TCR_EL1.T0SZ >= 35 && state->system_reg.TCR_EL1.T0SZ <= 47)
                {
                    x = 51 - state->system_reg.TCR_EL1.T0SZ;
                } else
                {
                    x = 16;
                }
                y = 12 + x;
                if(level == 3)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR0_EL1, x, pa_max) << x) | (REG64_BITS(virt_addr, 16, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, pa_max) << x) | (REG64_BITS(virt_addr, 16, y) << 3);
                }
                //printf("In %s, Line : %d, 64kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG0 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG0, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
    } else if (ttbr_idx == 1)
    {
        if(state->system_reg.TCR_EL1.EPD1)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD1: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD1, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG1)
        {
            case 0x2: /* 4kb granule size */
                /* Stage 1 translation of the level 3:
                * BaseAddr[PAMax-1:x] : IA[y:12] : 0b000
                * if a 43 ≤ TnSZ ≤ 48, then x = (55 - TnSZ), else x = 12
                * y = (x + 17)
                */
                if (state->system_reg.TCR_EL1.T1SZ >= 43 && state->system_reg.TCR_EL1.T1SZ <= 48)
                {
                    x = 55 - state->system_reg.TCR_EL1.T1SZ;
                } else
                {
                    x = 12;
                }
                y = 8 + x;
                if(level == 3)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR1_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 12, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, 47) << x) | (REG64_BITS(virt_addr, 12, y) << 3);
                }
                //printf("In %s, Line : %d, 4kb granule size | pte_addr[2] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            case 0x1: /* 16kb granule size */
                /* Stage 1 translation of the level 3:
                * BaseAddr[PAMax-1:x] : IA[y:14] : 0b000
                * if a 39 ≤ TnSZ ≤ 48, then x = (53 - TnSZ), else x = 14
                * y = (x + 10)
                */
                if (state->system_reg.TCR_EL1.T1SZ >= 39 && state->system_reg.TCR_EL1.T1SZ <= 48)
                {
                    x = 53 - state->system_reg.TCR_EL1.T1SZ;
                } else
                {
                    x = 14;
                }
                y = 10 + x;
                if(level == 3)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR1_EL1, x, 47) << x) | (REG64_BITS(virt_addr, 14, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, 47) << x) | (REG64_BITS(virt_addr, 14, y) << 3);
                }
                //printf("In %s, Line : %d, 16kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            case 0x3: /* 64kb granule size */
                /* Stage 1 translation of the level 3:
                * BaseAddr[PAMax-1:x] : IA[y:16] : 0b000
                * if a 35 ≤ TnSZ ≤ 47, then x = (51 - TnSZ), else x = 16
                * y = (x + 12)
                */
                if (state->system_reg.ID_AA64MMFR0_EL1 & 0x07 == 0x06)
                {
                    pa_max = 51;
                } else
                {
                    pa_max = 47;
                }
                if (state->system_reg.TCR_EL1.T1SZ >= 35 && state->system_reg.TCR_EL1.T1SZ <= 47)
                {
                    x = 51 - state->system_reg.TCR_EL1.T1SZ;
                } else
                {
                    x = 16;
                }
                y = 12 + x;
                if(level == 3)
                {
                    *output_addr = (REG64_BITS(state->system_reg.TTBR1_EL1, x, pa_max) << x) | (REG64_BITS(virt_addr, 16, y) << 3);
                } else
                {
                    *output_addr = (REG64_BITS(input_addr, x, pa_max) << x) | (REG64_BITS(virt_addr, 16, y) << 3);
                }
                //printf("In %s, Line : %d, 64kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG1 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG1, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
    }
    return NO_FAULT;
}

static fault_t mmu_arch64_get_l3_phys_addr(ARMul_State * state, ARMdword virt_addr, ARMdword input_addr, ARMdword * phys_addr, int ttbr_idx)
{
    if (ttbr_idx == 0)
    {
        if(state->system_reg.TCR_EL1.EPD0)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD0: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD0, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG0)
        {
            case 0x0: /* 4kb granule size */
                *phys_addr = (input_addr & 0x0000FFFFFFFFF000ULL) | (virt_addr & 0x00000FFFULL);
                //printf("In %s, Line : %d, 4kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            case 0x2: /* 16kb granule size */
                *phys_addr = (input_addr & 0x0000FFFFFFFFE000ULL) | (virt_addr & 0x00001FFFULL);
                //printf("In %s, Line : %d, 16kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            case 0x1: /* 64kb granule size */
                *phys_addr = (input_addr & 0x0000FFFFFFFF0000ULL) | (virt_addr & 0x0000FFFFULL);
                //printf("In %s, Line : %d, 64kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG0 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG0, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }

    } else if (ttbr_idx == 1)
    {
        if(state->system_reg.TCR_EL1.EPD1)
        {
            printf("In %s, Line: %d, TCR_EL1.EPD1: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.EPD1, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        switch(state->system_reg.TCR_EL1.TG1)
        {
            case 0x2: /* 4kb granule size */
                *phys_addr = (input_addr & 0x0000FFFFFFFFF000ULL) | (virt_addr & 0x00000FFFULL);
                //printf("In %s, Line : %d, 4kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            case 0x1: /* 16kb granule size */
                *phys_addr = (input_addr & 0x0000FFFFFFFFE000ULL) | (virt_addr & 0x00001FFFULL);
                //printf("In %s, Line : %d, 16kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            case 0x3: /* 64kb granule size */
                *phys_addr = (input_addr & 0x0000FFFFFFFF0000ULL) | (virt_addr & 0x0000FFFFULL);
                //printf("In %s, Line : %d, 64kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                break;
            default:
                printf("In %s, Line: %d, error TCR_EL1.TG1 at PC = 0x%x\n", __func__, __LINE__, state->system_reg.TCR_EL1.TG1, state->realPC);
                return PAGE_TRANSLATION_FAULT;
        }
    }
    return NO_FAULT;
}

static void mmu_arch64_set_param(ARMul_State * state,ARMdword virt_addr, int *ttbr_idx)
{
    /* check ASID size */
    if(state->system_reg.TCR_EL1.AS == 0)
    {
        if ( ((virt_addr >> 48) & 0x00FF) == 0x0000 )
        {
            *ttbr_idx = 0;
        } else if ( ((virt_addr >> 48) & 0x00FF) == 0x00FF )
        {
            *ttbr_idx = 1;
        }
    }else
    {
        if ( ((virt_addr >> 48) & 0xFFFF) == 0x0000 )
        {
            *ttbr_idx = 0;
        } else if ( ((virt_addr >> 48) & 0xFFFF) == 0xFFFF )
        {
            *ttbr_idx = 1;
        }
    }

}

static fault_t mmu_arch64_translate(ARMul_State * state, ARMdword virt_addr, ARMdword * phys_addr, int *ap, int *sop)
{
    ARMdword pte_addr[4] = {0}, pte_desc[4] = {0};
    int ttbr_idx = 0;
    fault_t ret = NO_FAULT;
    int tlb_level = 0;

    mmu_arch64_set_param(state, virt_addr, &ttbr_idx);

    ret = mmu_arch64_init_lookup_level(state, &tlb_level, ttbr_idx);
    if (ret)
    {
        printf("In %s, Line : %d, mmu_arch64_init_lookup_level failed at PC = 0x%x!\n", __func__, __LINE__, state->realPC);
        return ret;
    }

    switch(tlb_level)
    {
        case 0: /* Start translating from Level0 table */
            goto __mmu_arch64_get_l0_pte_addr;
        case 1: /* Start translating from Level1 table */
            goto __mmu_arch64_get_l1_pte_addr;
        case 2: /* Start translating from Level2 table */
            goto __mmu_arch64_get_l2_pte_addr;
        case 3: /* Start translating from Level3 table */
            goto __mmu_arch64_get_l3_pte_addr;
        default:
            return SECTION_TRANSLATION_FAULT;
    }
  
    __mmu_arch64_get_l0_pte_addr:
    {
        ret = mmu_arch64_get_l0_pte_addr(state, virt_addr, &pte_addr[0], ttbr_idx);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_addr[0]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_addr[0], state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        /* Result check of pte_desc[0] will be checked after goto jump */
        mmu_arch64_get_pte_desc(state, pte_addr[0], &pte_desc[0]);
        /* Level0 table only table entry but no block entry */
    }

    __mmu_arch64_get_l1_pte_addr:
    {
        ret = mmu_arch64_get_l1_pte_addr(state, virt_addr, pte_desc[0], &pte_addr[1], tlb_level, ttbr_idx);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_addr[1]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_addr[1], state->realPC);
            return ret;
        }

        ret = mmu_arch64_get_pte_desc(state, pte_addr[1], &pte_desc[1]);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_desc[1]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_desc[1], state->realPC);
            return ret;
        }
        switch (pte_desc[1] & 0x3)
        {
            case 2: /* invalid entry */
                printf("In %s, Line: %d, pte_addr[1] : 0x%llx pte_desc[1] :%llx Error!PC = 0x%x, virt_addr = 0x%llx\n", __func__, __LINE__, pte_addr[1], pte_desc[1], state->realPC, virt_addr);
                return SECTION_TRANSLATION_FAULT;
            case 0: /* ignored       */
            case 1: /* block   entry */
                mmu_arch64_get_l1_phys_addr(state, virt_addr, pte_desc[1], (ARMdword *)phys_addr, ttbr_idx);
                return NO_FAULT;
            case 3: /* table entry */
                break;
        }
    }

    __mmu_arch64_get_l2_pte_addr:
    {
        ret = mmu_arch64_get_l2_pte_addr(state, virt_addr, pte_desc[1], &pte_addr[2], tlb_level, ttbr_idx);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_addr[2]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_addr[2], state->realPC);
            return ret;
        }

        ret = mmu_arch64_get_pte_desc(state, pte_addr[2], &pte_desc[2]);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_desc[2]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_desc[2], state->realPC);
            return ret;
        }

        switch (pte_desc[2] & 0x3)
        {
            case 2: /* invalid entry */
                printf("In %s, Line: %d, pte_addr[2] : 0x%llx pte_desc[2] :%llx Error!PC = 0x%x, virt_addr = 0x%llx\n", __func__, __LINE__, pte_addr[2], pte_desc[2], state->realPC, virt_addr);
                return SECTION_TRANSLATION_FAULT;
            case 0: /* ignored       */
            case 1: /* block   entry */
                mmu_arch64_get_l2_phys_addr(state, virt_addr, pte_desc[2], (ARMdword *)phys_addr, ttbr_idx);
                return NO_FAULT;
            case 3: /* table entry */
                goto __mmu_arch64_get_l3_pte_addr;
                break;
        }
    }

    __mmu_arch64_get_l3_pte_addr:
    {
        ret = mmu_arch64_get_l3_pte_addr(state, virt_addr, pte_desc[2], &pte_addr[3], tlb_level, ttbr_idx);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_addr[3]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_addr[3], state->realPC);
            return ret;
        }

        ret = mmu_arch64_get_pte_desc(state, pte_addr[3], &pte_desc[3]);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_desc[3]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_desc[3], state->realPC);
            return ret;
        }

        switch (pte_desc[3] & 0x3)
        {
            case 1: /* reserved      */
            case 2: /* invalid entry */
                printf("In %s, Line : %d, 4kb granule size | virt_addr 0x%llx pte_desc = %llx pte_addr = %llx at PC = 0x%x!\n", __func__, __LINE__, virt_addr, pte_desc[3], pte_addr[3], state->realPC);
                return SECTION_TRANSLATION_FAULT;
            case 0: /* ignored       */
            case 3: /* block   entry */
                ret = mmu_arch64_get_l3_phys_addr(state, virt_addr, pte_desc[3], (ARMdword *)phys_addr, ttbr_idx);
                if (ret)
                {
                    printf("In %s, Line : %d, Read phys_addr: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                    return ret;
                }
                return NO_FAULT;
        }

    }

    return NO_FAULT;
}

static fault_t mmu_arch32_long_set_param(ARMul_State * state, ARMdword virt_addr, int * range, int * ttbr_idx)
{
    if (state->mmu.TTBCR.long_format.T1SZ == 0)
    {
        if (state->mmu.TTBCR.long_format.T0SZ == 0)
        {
            /* T0SZ_ZERO_T1SZ_ZERO */
            *range = 0;
            *ttbr_idx = 0;
        } else
        {
            /* T0SZ_NONZERO_T1SZ_ZERO */
            *range = 1;
            if ( (0x100000000ULL >> state->mmu.TTBCR.long_format.T0SZ) > virt_addr)
            {
                *ttbr_idx = 0;
            } else
            {
                *ttbr_idx = 1;
            }
        }
    } else
    {
        if (state->mmu.TTBCR.long_format.T0SZ == 0)
        {
            /* T0SZ_ZERO_T1SZ_NONZERO */
            *range = 2;
            if ( (0x100000000ULL >> state->mmu.TTBCR.long_format.T1SZ) > virt_addr )
            {
                *ttbr_idx = 0;
            } else
            {
                *ttbr_idx = 1;
            }
        } else
        {
            /* T0SZ_NONZERO_T1SZ_NONZERO */
            *range = 3;
            if ( (0x100000000ULL >> state->mmu.TTBCR.long_format.T0SZ) > virt_addr )
            {
                *ttbr_idx = 0;
            } else if ( (0x100000000ULL >> state->mmu.TTBCR.long_format.T1SZ) < virt_addr )
            {
                *ttbr_idx = 1;
            } else
            {
                return SECTION_TRANSLATION_FAULT;
            }
        }
    }
    return NO_FAULT;
}

static fault_t mmu_arch32_long_init_lookup_level(ARMul_State * state, int * level, int range)
{
    switch(range)
    {
        case 0: /* T0SZ_ZERO_T1SZ_ZERO */
            *level = 1;
            break;
        case 1: /* T0SZ_NONZERO_T1SZ_ZERO */
            if(state->mmu.TTBCR.long_format.EPD0)
            {
                printf("In %s, Line: %d, TCR_EL1.EPD0: 0x%llx at PC = 0x%x\n", __func__, __LINE__, state->mmu.TTBCR.long_format.EPD0, state->realPC);
                return SECTION_TRANSLATION_FAULT;  /* Mmu fault doesn't have to be this one, it's not finished yet !!!! */
            }
            switch(state->mmu.TTBCR.long_format.T0SZ)
            {
                case 1:
                    *level = 1;
                    break;
                case 2 ... 7:
                    *level = 2;
                    break;
                default:
                    printf("In %s, Line : %d, 4kb granule size | T0SZ %d at PC = 0x%x!\n", __func__, __LINE__, state->mmu.TTBCR.long_format.T0SZ, state->realPC);
                    return PAGE_TRANSLATION_FAULT; /* Mmu fault doesn't have to be this one, it's not finished yet !!!! */
            }
            break;
        case 2 ... 3: /* T0SZ_ZERO_T1SZ_NONZERO | T0SZ_NONZERO_T1SZ_NONZERO */
            if(state->mmu.TTBCR.long_format.EPD1)
            {
                printf("In %s, Line: %d, TCR_EL1.EPD1: 0x%llx at PC = 0x%x\n", __func__, __LINE__, state->mmu.TTBCR.long_format.EPD1, state->realPC);
                return SECTION_TRANSLATION_FAULT; /* Mmu fault doesn't have to be this one, it's not finished yet !!!! */
            }
            switch(state->mmu.TTBCR.long_format.T1SZ)
            {
                case 1:
                    *level = 1;
                    break;
                case 2 ... 7:
                    *level = 2;
                    break;
                default:
                    printf("In %s, Line : %d, 4kb granule size | T1SZ %d at PC = 0x%x!\n", __func__, __LINE__, state->mmu.TTBCR.long_format.T1SZ, state->realPC);
                    return PAGE_TRANSLATION_FAULT; /* Mmu fault doesn't have to be this one, it's not finished yet !!!! */
            }
            break;
    }
    return NO_FAULT;
}

static fault_t mmu_arch32_long_get_pte_desc(ARMul_State * state, ARMdword input_addr, ARMdword * output_addr)
{
    ARMword tmp_low32 = 0, tmp_high32 = 0;
    fault_t ret = NO_FAULT;
    if (state->space.conf_obj != NULL)
    {
        ret = arch64_mem_read(state, input_addr, AccType_NORMAL, &tmp_low32, &tmp_high32, 8);
        if (ret == NO_FAULT)
        {
            *output_addr = ((uint64_t)(tmp_high32) << 32) | tmp_low32;
            //printf("In %s, Line : %d, tlb_entry 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
        } else
        {
            printf("In %s, Line : %d, tlb_entry 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
            return ret;
        }
    }
    return NO_FAULT;
}

static fault_t mmu_arch32_long_get_l1_pte_addr(ARMul_State * state, ARMdword virt_addr, ARMdword * output_addr, int level, int ttbr_idx)
{
    int x = 0, y = 0;
    if (ttbr_idx == 0)
    {
        if(state->mmu.TTBCR.long_format.EPD0)
        {
            printf("In %s, Line: %d, mmu.TTBCR.long_format.EPD0: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->mmu.TTBCR.long_format.EPD0, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        /* only support 4kb granule size */
        /* Stage 1 translation of the level 1:
        * BaseAddr[39:x] : IA[y:30] : 0b000
        * if a 0 ≤ TnSZ ≤ 1, then x = (5 - TnSZ)
        * y = (x + 26)
        */
        if (state->mmu.TTBCR.long_format.T0SZ >= 0 && state->mmu.TTBCR.long_format.T0SZ <= 1)
        {
            x = 5 - state->mmu.TTBCR.long_format.T0SZ;
        }
        y = 26 + x;
        *output_addr = (REG64_BITS(state->mmu.TTBR0.all, x, 39) << x) | (REG64_BITS(virt_addr, 30, y) << 3);
        //printf("In %s, Line : %d, 4kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
    } else if (ttbr_idx == 1)
    {
        if(state->mmu.TTBCR.long_format.EPD1)
        {
            printf("In %s, Line: %d, TTBCR.long_format.EPD1: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->mmu.TTBCR.long_format.EPD1, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        /* only support 4kb granule size */
        /* Stage 1 translation of the level 1:
        * BaseAddr[39:x] : IA[y:30] : 0b000
        * if a 0 ≤ TnSZ ≤ 1, then x = (5 - TnSZ)
        * y = (x + 26)
        */
        if (state->mmu.TTBCR.long_format.T1SZ >= 0 && state->mmu.TTBCR.long_format.T1SZ <= 1)
        {
            x = 5 - state->mmu.TTBCR.long_format.T1SZ;
        }
        y = 26 + x;
        *output_addr = (REG64_BITS(state->mmu.TTBR1.all, x, 39) << x) | (REG64_BITS(virt_addr, 30, y) << 3);
        //printf("In %s, Line : %d, 4kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
    }

    return NO_FAULT;    
}

static fault_t mmu_arch32_long_get_l2_pte_addr(ARMul_State * state, ARMdword virt_addr, ARMdword input_addr, ARMdword * output_addr, int level, int ttbr_idx)
{
    int x = 0, y = 0;
    if (ttbr_idx == 0)
    {
        if(state->mmu.TTBCR.long_format.EPD0)
        {
            printf("In %s, Line: %d, mmu.TTBCR.long_format.EPD0: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->mmu.TTBCR.long_format.EPD0, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;            
        }
        /* only support 4kb granule size */
        /* Stage 1 translation of the level 2:
        * BaseAddr[39:x] : IA[y:21] : 0b000
        * if a 2 ≤ TnSZ ≤ 7, then x = (14 - TnSZ), else x = 12
        * y = (x + 17)
        */
        if (state->mmu.TTBCR.long_format.T0SZ >= 2 && state->mmu.TTBCR.long_format.T0SZ <= 7)
        {
            x = 14 - state->mmu.TTBCR.long_format.T0SZ;
        } else
        {
            x = 12;
        }
        y = 17 + x;
        if(level == 2)
        {
            *output_addr = (REG64_BITS(state->mmu.TTBR0.all, x, 39) << x) | (REG64_BITS(virt_addr, 21, y) << 3);
        } else
        {
            *output_addr = (REG64_BITS(input_addr, x, 39) << x) | (REG64_BITS(virt_addr, 21, y) << 3);
        }
        //printf("In %s, Line : %d, 4kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
    } else if (ttbr_idx == 1)
    {
        if(state->mmu.TTBCR.long_format.EPD1)
        {
            printf("In %s, Line: %d, TTBCR.long_format.EPD1: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->mmu.TTBCR.long_format.EPD1, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        /* only support 4kb granule size */
        /* Stage 1 translation of the level 2:
        * BaseAddr[39:x] : IA[y:21] : 0b000
        * if a 2 ≤ TnSZ ≤ 7, then x = (14 - TnSZ), else x = 12
        * y = (x + 17)
        */
        if (state->mmu.TTBCR.long_format.T1SZ >= 2 && state->mmu.TTBCR.long_format.T1SZ <= 7)
        {
            x = 14 - state->mmu.TTBCR.long_format.T1SZ;
        } else
        {
            x = 12;
        }
        y = 17 + x;
        if(level == 2)
        {
            *output_addr = (REG64_BITS(state->mmu.TTBR1.all, x, 39) << x) | (REG64_BITS(virt_addr, 21, y) << 3);
        } else
        {
            *output_addr = (REG64_BITS(input_addr, x, 39) << x) | (REG64_BITS(virt_addr, 21, y) << 3);
        }
        //printf("In %s, Line : %d, 4kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
    }
    return NO_FAULT;
}

static fault_t mmu_arch32_long_get_l3_pte_addr(ARMul_State * state, ARMdword virt_addr, ARMdword input_addr, ARMdword * output_addr, int level, int ttbr_idx)
{
    if (ttbr_idx == 0)
    {
        if(state->mmu.TTBCR.long_format.EPD0)
        {
            printf("In %s, Line: %d, mmu.TTBCR.long_format.EPD0: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->mmu.TTBCR.long_format.EPD0, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;            
        }
        /* only support 4kb granule size */
        /* Stage 1 translation of the level 3:
        * BaseAddr[39:12] : IA[20:12] : 0b000
        */
        if(level == 3)
        {
            *output_addr = (REG64_BITS(state->mmu.TTBR0.all, 12, 39) << 12) | (REG64_BITS(virt_addr, 12, 20) << 12);
        } else
        {
            *output_addr = (REG64_BITS(input_addr, 12, 39) << 12) | (REG64_BITS(virt_addr, 12, 20) << 3);
        }
        //printf("In %s, Line : %d, 4kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
    } else if (ttbr_idx == 1)
    {
        if(state->mmu.TTBCR.long_format.EPD1)
        {
            printf("In %s, Line: %d, TTBCR.long_format.EPD1: 0x%llx | virt_addr: %llx at PC = 0x%x\n", __func__, __LINE__, state->mmu.TTBCR.long_format.EPD1, virt_addr, state->realPC);
            return SECTION_TRANSLATION_FAULT;
        }
        /* only support 4kb granule size */
        /* Stage 1 translation of the level 3:
        * BaseAddr[39:12] : IA[20:12] : 0b000
        */
        if(level == 3)
        {
            *output_addr = (REG64_BITS(state->mmu.TTBR1.all, 12, 39) << 12) | (REG64_BITS(virt_addr, 12, 20) << 12);
        } else
        {
            *output_addr = (REG64_BITS(input_addr, 12, 39) << 12) | (REG64_BITS(virt_addr, 12, 20) << 12);
        }
        //printf("In %s, Line : %d, 4kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
    }
    return NO_FAULT;
}

static fault_t mmu_arch32_long_translate(ARMul_State * state, ARMdword virt_addr, ARMdword * phys_addr, int *ap, int *sop)
{
    /* page table entry address (table head address + index) */
    ARMdword pte_addr[4] = {0}; /* no level 0 */
    /* page table entry desc */
    ARMdword pte_desc[4] = {0}; /* no level 0 */
    int ttbr_idx = 0, range = 0, tlb_level = 0;
    fault_t ret = NO_FAULT;

    ret = mmu_arch32_long_set_param(state, virt_addr, &range, &ttbr_idx);
    if (ret)
    {
        printf("In %s, Line : %d, mmu_arch32_long_set_param failed at PC = 0x%x!\n", __func__, __LINE__, state->realPC);
        return ret;
    }

    ret = mmu_arch32_long_init_lookup_level(state, &tlb_level, range);
    if (ret)
    {
        printf("In %s, Line : %d, mmu_arch32_long_init_lookup_level failed at PC = 0x%x!\n", __func__, __LINE__, state->realPC);
        return ret;
    }

    switch(tlb_level)
    {
        case 1: /* Start translating from Level1 table */
            goto __mmu_arch32_long_get_l1_pte_addr;
            break;
        case 2: /* Start translating from Level2 table */
            goto __mmu_arch32_long_get_l2_pte_addr;
            break;
        case 3: /* Start translating from Level3 table */
            goto __mmu_arch32_long_get_l3_pte_addr;
            break;
        default:
            return SECTION_TRANSLATION_FAULT;
    }

    __mmu_arch32_long_get_l1_pte_addr:
    {
        ret = mmu_arch32_long_get_l1_pte_addr(state, virt_addr, &pte_addr[1], tlb_level, ttbr_idx);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_addr[1]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_addr[1], state->realPC);
            return ret;
        }

        ret = mmu_arch32_long_get_pte_desc(state, pte_addr[1], &pte_desc[1]);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_desc[1]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_desc[1], state->realPC);
            return ret;
        }

        switch (pte_desc[1] & 0x3)
        {
            case 2: /* invalid entry */
                printf("In %s, Line: %d, pte_addr[1] : 0x%llx pte_desc[1] :%llx Error!PC = 0x%x, virt_addr = 0x%llx\n", __func__, __LINE__, pte_addr[1], pte_desc[1], state->realPC, virt_addr);
                return SECTION_TRANSLATION_FAULT;
            case 0: /* ignored       */
                //printf("In %s, Line : %d, 4kb granule size | virt_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, virt_addr, state->realPC);
            case 1: /* block   entry */
                /* block support 1GB memory region  */
                *phys_addr = (pte_desc[1] & 0x0000FFFFFFC00000ULL) | (virt_addr & 0x3FFFFFFFULL); /* only support 4kb granule size */
                //printf("In %s, Line : %d, 4kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                return NO_FAULT;
            case 3: /* table entry */
                break;
        }
    }

    __mmu_arch32_long_get_l2_pte_addr:
    {
        ret = mmu_arch32_long_get_l2_pte_addr(state, virt_addr, pte_desc[1], &pte_addr[2], tlb_level, ttbr_idx);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_addr[2]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_addr[2], state->realPC);
            return ret;
        }

        ret = mmu_arch32_long_get_pte_desc(state, pte_addr[2], &pte_desc[2]);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_desc[2]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_desc[2], state->realPC);
            return ret;
        }

        switch (pte_desc[2] & 0x3)
        {
            case 2: /* invalid entry */
                printf("In %s, Line: %d, pte_addr[1] : 0x%llx pte_desc[1] :%llx Error!PC = 0x%x, virt_addr = 0x%llx\n", __func__, __LINE__, pte_addr[1], pte_desc[1], state->realPC, virt_addr);
                printf("In %s, Line: %d, pte_addr[2] : 0x%llx pte_desc[2] :%llx Error!PC = 0x%x, virt_addr = 0x%llx\n", __func__, __LINE__, pte_addr[2], pte_desc[2], state->realPC, virt_addr);
                return PAGE_TRANSLATION_FAULT;
            case 0: /* ignored       */
                //printf("In %s, Line : %d, 4kb granule size | virt_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, virt_addr, state->realPC);
            case 1: /* block   entry */
                /* block support 2M memory region  */
                *phys_addr = (pte_desc[2] & 0x0000FFFFFFE00000ULL) | (virt_addr & 0x001FFFFFULL); /* only support 4kb granule size */
                //printf("In %s, Line : %d, 4kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                return NO_FAULT;
            case 3: /* table entry */
                break;
        }
    }

    __mmu_arch32_long_get_l3_pte_addr:
    {
        
        ret = mmu_arch32_long_get_l3_pte_addr(state, virt_addr, pte_desc[2], &pte_addr[3], tlb_level, ttbr_idx);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_addr[3]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_addr[3], state->realPC);
            return ret;
        }

        ret = mmu_arch32_long_get_pte_desc(state, pte_addr[3], &pte_desc[3]);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_desc[3]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_desc[3], state->realPC);
            return ret;
        }
        switch(pte_desc[3]&0x3)
        {
            case 1: /* invalid entry */
                printf("In %s, Line: %d, pte_addr[1] : 0x%llx pte_desc[1] :%llx Error!PC = 0x%x, virt_addr = 0x%llx\n", __func__, __LINE__, pte_addr[1], pte_desc[1], state->realPC, virt_addr);
                printf("In %s, Line: %d, pte_addr[2] : 0x%llx pte_desc[2] :%llx Error!PC = 0x%x, virt_addr = 0x%llx\n", __func__, __LINE__, pte_addr[2], pte_desc[2], state->realPC, virt_addr);
                printf("In %s, Line: %d, pte_addr[3] : 0x%llx pte_desc[3] :%llx Error!PC = 0x%x, virt_addr = 0x%llx\n", __func__, __LINE__, pte_addr[3], pte_desc[3], state->realPC, virt_addr);                
                return PAGE_TRANSLATION_FAULT;
            case 0: /* ignored       */
            case 2: /* ignored       */
                //printf("In %s, Line : %d, 4kb granule size | virt_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, virt_addr, state->realPC);
            case 3: /* block   entry */
                /* block support 4kb memory page  */
                *phys_addr = (pte_desc[3] & 0x0000FFFFFFFFF000ULL) | (virt_addr & 0x00000FFFULL); /* only support 4kb granule size */
                //printf("In %s, Line : %d, 4kb granule size | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                return NO_FAULT;
        }

    }
    return NO_FAULT;
}

static fault_t mmu_arch32_short_set_param(ARMul_State * state, ARMword virt_addr, int * ttbr_idx)
{
    if (state->mmu.TTBCR.short_format.N == 0)
    {
        *ttbr_idx = 0;
    } else 
    {
        if ( (0x100000000ULL >> state->mmu.TTBCR.short_format.N) > virt_addr )
        {
            *ttbr_idx = 0;
        } else
        {
            *ttbr_idx = 1;
        }
    }
    return NO_FAULT;
}

static fault_t mmu_arch32_short_get_pte_desc(ARMul_State * state, ARMword input_addr, ARMword * output_addr)
{
    fault_t ret = NO_FAULT;
    if (state->space.conf_obj != NULL)
    {
        ret = state->space.read(state->space.conf_obj, input_addr, output_addr, 4);
        if (ret)
        {
           printf("In %s, Line : %d, tlb_entry 0x%lx failed at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
           return ret;
        }
    } else
    {
        mem_read_raw(32, input_addr, output_addr);
    }
    return NO_FAULT;
}

static fault_t mmu_arch32_short_get_l1_pte_addr(ARMul_State * state, ARMword virt_addr, ARMword * output_addr, int ttbr_idx)
{
    int x = 0, y = 0;
    if (ttbr_idx == 0)
    {
        /* Stage 1 translation of the level 1:
        * Descriptor address [31:x] : [x - 1:2] : 0b00
        * x = 14 - N;
        * y = 31 - N
        */
        x = 14 - state->mmu.TTBCR.short_format.N;
        y = 31 - state->mmu.TTBCR.short_format.N;
        *output_addr = (REG32_BITS(state->mmu.TTBR0.all, x, 31) << x) | (REG32_BITS(virt_addr, 20, y) << 2);
        //printf("In %s, Line : %d, pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
    } else if (ttbr_idx == 1)
    {
        /* Stage 1 translation of the level 1:
        * Descriptor address [31:x] : [x - 1:2] : 0b00
        * x = 14 - N;
        * y = 31 - N
        */
        x = 14 - state->mmu.TTBCR.short_format.N;
        y = 31 - state->mmu.TTBCR.short_format.N;
        *output_addr = (REG32_BITS(state->mmu.TTBR1.all, x, 31) << x) | (REG32_BITS(virt_addr, 20, y) << 2);
        //printf("In %s, Line : %d, 4kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
    }

    return NO_FAULT;    
}

static fault_t mmu_arch32_short_get_l2_pte_addr(ARMul_State * state, ARMword virt_addr, ARMword * output_addr, int ttbr_idx)
{
    if (ttbr_idx == 0)
    {
        /* Stage 1 translation of the level 1:
        * Descriptor address [31:10] : [9:2] : 0b00
        */
        *output_addr = (REG32_BITS(state->mmu.TTBR0.all, 10, 31) << 10) | (REG32_BITS(virt_addr, 12, 19) << 2);
        //printf("In %s, Line : %d, pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
    } else if (ttbr_idx == 1)
    {
        /* Stage 1 translation of the level 1:
        * Descriptor address [31:10] : [9:2] : 0b00
        */
        *output_addr = (REG32_BITS(state->mmu.TTBR1.all, 10, 31) << 10) | (REG32_BITS(virt_addr, 12, 19) << 2);
        //printf("In %s, Line : %d, 4kb granule size | pte_addr[1] 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *output_addr, state->realPC);
    }

    return NO_FAULT;    
}

/*  ap: AP bits value.
 *  sop: section or page description  0:section 1:page
 */
static fault_t mmu_arch32_short_translate(ARMul_State * state, ARMword virt_addr, ARMdword * phys_addr, int *ap, int *sop)
{
    /* walk the translation tables */
    ARMword l1addr, l1desc;
    /* page table entry address (table head address + index) */
    ARMdword pte_addr[3] = {0}; /* no level 0 */
    /* page table entry desc */
    ARMdword pte_desc[3] = {0}; /* no level 0 */
    int ttbr_idx = 0;
    fault_t ret = NO_FAULT;

    ret = mmu_arch32_short_set_param(state, virt_addr, &ttbr_idx);
    if (ret)
    {
        printf("In %s, Line : %d, mmu_arch32_short_set_param failed at PC = 0x%x!\n", __func__, __LINE__, state->realPC);
        return ret;
    }

    __mmu_arch32_short_get_l1_pte_addr:
    {
        ret = mmu_arch32_short_get_l1_pte_addr(state, virt_addr, &pte_addr[1], ttbr_idx);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_addr[1]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_addr[1], state->realPC);
            return ret;
        }

        ret = mmu_arch32_short_get_pte_desc(state, pte_addr[1], &pte_desc[1]);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_desc[1]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_desc[1], state->realPC);
            return ret;
        }

        switch (pte_desc[1] & 0x3)
        {
            case 0: /* invalid entry */
            case 2: /* Section or Supersection */
            case 3: /* Section or Supersection */
                if ( !REG32_BIT(pte_desc[1], 18) ) /* Section */
                {
                    *phys_addr = (REG32_BITS(pte_desc[1], 20, 31) << 20) | (REG32_BITS(virt_addr, 0, 19));
                    //printf("In %s, Line : %d, Section | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                } else /* Supersection */
                {
                    *phys_addr = (REG32_BITS(pte_desc[1], 5, 8) << 31) | (REG32_BITS(pte_desc[1], 20, 23) << 28) | (REG32_BITS(pte_desc[2], 24, 31) << 24) | (REG32_BITS(virt_addr, 0, 23));
                    //printf("In %s, Line : %d, Supersection | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                }
                /* block support 1GB memory region  */
                return NO_FAULT;
            case 1: /* table entry */
                break;
        }
    }

    __mmu_arch32_short_get_l2_pte_addr:
    {
        ret = mmu_arch32_short_get_l2_pte_addr(state, virt_addr, &pte_addr[1], ttbr_idx);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_addr[2]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_addr[2], state->realPC);
            return ret;
        }

        ret = mmu_arch32_short_get_pte_desc(state, pte_addr[2], &pte_desc[2]);
        if (ret)
        {
            printf("In %s, Line : %d, Read pte_desc[2]: 0x%llx failed at PC = 0x%x!\n", __func__, __LINE__, pte_desc[2], state->realPC);
            return ret;
        }

        switch (pte_desc[2] & 0x3)
        {
            case 0: /* invalid entry */
                return PAGE_TRANSLATION_FAULT;
            case 1: /* Large page */
                *phys_addr = (REG32_BITS(pte_desc[2], 16, 31) << 16) | (REG32_BITS(virt_addr, 0, 15));
                //printf("In %s, Line : %d, Large page | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                return NO_FAULT;
            case 2: /* Small page */
            case 3: /* Small page */
                *phys_addr = (REG32_BITS(pte_desc[2], 12, 31) << 12) | (REG32_BITS(virt_addr, 0, 11));
                //printf("In %s, Line : %d, Small page | phys_addr 0x%llx at PC = 0x%x!\n", __func__, __LINE__, *phys_addr, state->realPC);
                return NO_FAULT;
        }
    }

    return NO_FAULT;
}

static fault_t mmu_translate(ARMul_State * state, ARMdword virt_addr, ARMdword * phys_addr, int *ap, int *sop)
{
    fault_t ret = NO_FAULT;
    switch(state->translate_type)
    {
        case arch32_short_translate:
            ret = mmu_arch32_short_translate(state, (ARMword)virt_addr, (ARMword *)phys_addr, ap, sop);
            if (ret)
            {
                printf("In %s, Line :%d, translate virt_addr : 0x%x fault :%d\n", __func__, __LINE__, virt_addr, ret);
                return ret;
            }
            break;
        case arch32_long_translate:
            ret = mmu_arch32_long_translate(state, virt_addr, phys_addr, ap, sop);
            if (ret)
            {
                printf("In %s, Line :%d, translate virt_addr : 0x%x fault :%d\n", __func__, __LINE__, virt_addr, ret);
                return ret;
            }
            break;            
        case arch64_translate:
            ret = mmu_arch64_translate(state, virt_addr, phys_addr, ap, sop);
            if (ret)
            {
                printf("In %s, Line :%d, translate virt_addr : 0x%x fault :%d\n", __func__, __LINE__, virt_addr, ret);
                return ret;
            }
            break;
        default:
            printf("In %s, Line :%d, translate virt_addr : 0x%x fault :%d\n", __func__, __LINE__, virt_addr, ret);
            return ret;
    }
    return NO_FAULT;
}

int ftc662_mmu_init(ARMul_State * state)
{
    state->mmu.SCTLR.all = 0xc50078;
    state->mmu.control = state->mmu.SCTLR.all;
    state->mmu.translation_table_base = 0xDEADC0DE;
    state->mmu.domain_access_control = 0xDEADC0DE;
    state->mmu.fault_status = 0;
    state->mmu.fault_address = 0;
    state->mmu.process_id = 0;
    state->mmu.context_id = 0;
    state->mmu.thread_uro_id = 0;

    state->system_reg.SCTLR_EL1.all = state->mmu.SCTLR.all;
    state->system_reg.CCSIDR_EL1.CCSIDR = 2;
    state->system_reg.CLIDR_EL1.CLIDR = 0x7000012;
    return No_exp;
}

void ftc662_mmu_exit(ARMul_State * state)
{
}



fault_t ftc662_mmu_load_instr(ARMul_State * state, ARMword va, ARMword * instr)
{
    fault_t fault;
    ARMdword pa;                         /* physical addr */
    int ap, sop;

    static int debug_count = 0;         /* used for debug */

    d_msg("va = %x\n", va);

    va = mmu_pid_va_map(va);
    if (MMU_Enabled)
    {
        /* align check */
        if ((va & (WORD_SIZE - 1)) && MMU_Aligned)
        {
            d_msg("align\n");
            return ALIGNMENT_FAULT;
        } else
        {
            va &= ~(WORD_SIZE - 1);
        }
            
        /* translate */
        fault = mmu_translate(state, (uint64_t)va, &pa, &ap, &sop);
        if (fault)
        {
            printf("In %s, Line :%d, translate virt_addr : 0x%x fault :%d\n", __func__, __LINE__, va, fault);
            return fault;
        }
      
    }

    /*if MMU disabled or C flag is set alloc cache */
    if (MMU_Disabled)
    {
        pa = va;
    }
    if (state->space.conf_obj == NULL)
        state->space.read(state->space.conf_obj, pa, instr, 4);
    else
        mem_read_raw(32, pa, instr);

    return 0;
}

fault_t ftc662_mmu_write(ARMul_State * state, ARMword va, ARMword data, ARMword datatype);
fault_t ftc662_mmu_read(ARMul_State * state, ARMword va, ARMword * data, ARMword datatype);

fault_t ftc662_mmu_read_byte(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    fault_t fault;
    fault = ftc662_mmu_read(state, virt_addr, data, ARM_BYTE_TYPE);
    return fault;
}

fault_t ftc662_mmu_read_halfword(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    fault_t fault;

    fault = ftc662_mmu_read(state, virt_addr, data, ARM_HALFWORD_TYPE);
    return fault;
}

fault_t ftc662_mmu_read_word(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    return ftc662_mmu_read(state, virt_addr, data, ARM_WORD_TYPE);
}

fault_t ftc662_mmu_read(ARMul_State * state, ARMword va, ARMword * data, ARMword datatype)
{
    fault_t fault = NO_FAULT;
    ARMdword pa = 0, real_va = 0, temp = 0, offset = 0;
    int ap = 0, sop = 0;

    va = mmu_pid_va_map(va);
    real_va = va;

    /* if MMU disabled, memory_read */
    if (MMU_Disabled)
    {
        if (datatype == ARM_BYTE_TYPE)
        {
            if (state->space.conf_obj != NULL)
                state->space.read(state->space.conf_obj, va, data, 1);
            else
                mem_read_raw(8, va, data);

        } else if (datatype == ARM_HALFWORD_TYPE)
        {
            if (state->space.conf_obj != NULL)
                state->space.read(state->space.conf_obj, va, data, 2);
            else
                mem_read_raw(16, va, data);

        } else if (datatype == ARM_WORD_TYPE)
        {
            if (state->space.conf_obj != NULL)
                state->space.read(state->space.conf_obj, va, data, 4);
            else
                mem_read_raw(32, va, data);

        } else
        {
            printf("SKYEYE:In %s line:%d error: unknown data type %d\n", __func__, __LINE__, datatype);
            skyeye_exit(-1);
        }

        return 0;
    }

    /* align check */
    if (state->mmu.SCTLR.M == 1)
    {
        if (((va & 3) && (datatype == ARM_WORD_TYPE) && MMU_Aligned) ||
            ((va & 1) && (datatype == ARM_HALFWORD_TYPE) && MMU_Aligned))
        {
            printf("In %s line:%d mmu read align fault at %x\n", __func__, __LINE__, va);
            return ALIGNMENT_FAULT;
        }
    }

    /* translate */
    fault = mmu_translate(state, (uint64_t)va, &pa, &ap, &sop);
    if (fault)
    {
        printf("In %s, Line :%d, translate virt_addr : 0x%x fault :%d\n", __func__, __LINE__, va, fault);
        return fault;
    }

  skip_translation:
    if (datatype == ARM_BYTE_TYPE)
    {
        if (state->space.conf_obj != NULL)
        {
            state->space.read(state->space.conf_obj, pa | (real_va & 3), data, 1);
        }
        else
        {
            mem_read_raw(8, pa | (real_va & 3), data);
        }

    } else if (datatype == ARM_HALFWORD_TYPE)
    {
        if (state->space.conf_obj != NULL)
        {
            state->space.read(state->space.conf_obj, pa | (real_va & 3), data, 2);
        }
        else
        {
            mem_read_raw(16, pa | (real_va & 3), data);
        }

    } else if (datatype == ARM_WORD_TYPE)
        if (state->space.conf_obj != NULL)
        {
            state->space.read(state->space.conf_obj, pa, data, 4);
        }
        else
        {
            mem_read_raw(32, pa, data);
        }
        
    else
    {
        printf("SKYEYE:In %s line = %d error: unknown data type %d\n", __func__, __LINE__, datatype);
        skyeye_exit(-1);
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

    return 0;
}

fault_t ftc662_mmu_write_byte(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return ftc662_mmu_write(state, virt_addr, data, ARM_BYTE_TYPE);
}

fault_t ftc662_mmu_write_halfword(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return ftc662_mmu_write(state, virt_addr, data, ARM_HALFWORD_TYPE);
}

fault_t ftc662_mmu_write_word(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return ftc662_mmu_write(state, virt_addr, data, ARM_WORD_TYPE);
}

fault_t ftc662_mmu_write(ARMul_State * state, ARMword va, ARMword data, ARMword datatype)
{
    int b;
    ARMdword pa = 0, real_va = 0;
    fault_t fault;
    int ap, sop;

    d_msg("va = %x, val = %x\n", va, data);
    va = mmu_pid_va_map(va);
    real_va = va;

    if (MMU_Disabled)
    {
        if (datatype == ARM_BYTE_TYPE)
            if (state->space.conf_obj != NULL)
                state->space.write(state->space.conf_obj, va, &data, 1);
            else
                mem_write_raw(8, va, data);

        else if (datatype == ARM_HALFWORD_TYPE)
            if (state->space.conf_obj != NULL)
                state->space.write(state->space.conf_obj, va, &data, 2);
            else
                mem_write_raw(16, va, data);

        else if (datatype == ARM_WORD_TYPE)
            if (state->space.conf_obj != NULL)
                state->space.write(state->space.conf_obj, va, &data, 4);
            else
                mem_write_raw(32, va, data);

        else
        {
            printf("SKYEYE:1 ftc662_mmu_write error: unknown data type %d\n", datatype);
            skyeye_exit(-1);
        }
        return NO_FAULT;
    }

    /* align check */
    if (state->mmu.SCTLR.M == 1)
    {
        if (((va & 3) && (datatype == ARM_WORD_TYPE) && MMU_Aligned) ||
            ((va & 1) && (datatype == ARM_HALFWORD_TYPE) && MMU_Aligned))
        {
            printf("In %s line:%d mmu read align fault at %x\n", __func__, __LINE__, va);
            return ALIGNMENT_FAULT;
        }
    }
    
    va &= ~(WORD_SIZE - 1);

     /* translate */
    fault = mmu_translate(state, (uint64_t)va, &pa, &ap, &sop);
    if (fault)
    {
        printf("In %s, Line :%d, translate virt_addr : 0x%x fault :%d\n", __func__, __LINE__, va, fault);
        return fault;
    }

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
            return 0;
        }
    }

    if (datatype == ARM_BYTE_TYPE)
    {
        if (state->space.conf_obj != NULL)
            state->space.write(state->space.conf_obj, (pa | (real_va & 3)), &data, 1);
        else
            mem_write_raw(8, (pa | (real_va & 3)), data);

    } else if (datatype == ARM_HALFWORD_TYPE)
    {
        if (state->space.conf_obj != NULL)
            state->space.write(state->space.conf_obj, (pa | (real_va & 3)), &data, 2);
        else
            mem_write_raw(16, (pa | (real_va & 3)), data);
    } else if (datatype == ARM_WORD_TYPE)
    {
        if (state->space.conf_obj != NULL)
            state->space.write(state->space.conf_obj, pa, &data, 4);
        else
            mem_write_raw(32, pa, data);
    }

    return 0;
}

ARMword mrc_processIDRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2)
{
    ARMword data = 0;

    switch (OPC_1)
    {
        case 0:
            //MIDR
            if (OPC_2 == 0)
            {
                data = state->system_reg.MIDR_EL1;

                break;
            } else if (OPC_2 == 5)
            {
                //MPDIR Multiprocessor Affinity Register
                data = state->system_reg.MPIDR_EL1;
                break;
            } else
            {
                printf("In %s, %d, Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2, state->pc);
                break;
            }
        case 1:
            if (CRm == 0)
            {
                switch (OPC_2)
                {
                    case 0:
                        data = state->system_reg.CCSIDR_EL1.CCSIDR;
                        break;
                    case 1:
                        data = state->system_reg.CLIDR_EL1.CLIDR;
                        break;
                    case 2:
                        data = state->system_reg.CCSIDR2_EL1.CCSIDR2;
                        break;
                    case 7:
                        data = state->system_reg.AIDR_EL1.AIDR;
                        break;
                    default:
                        printf("In %s, %d, Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2, state->pc);
                        break;
                }
            } else
            {
                printf("In %s, %d, Not Implement CRm = %d at PC = %x\n", __func__, __LINE__, CRm, state->pc);
            }
            break;
        case 2:
            if (CRm == 0 && OPC_2 == 0)
            {
                data = state->system_reg.CSSELR_EL1;
            } else
            {
                printf("In %s, %d, Not Implement CRm = %d at PC = %x\n", __func__, __LINE__, CRm, state->pc);
            }
            break;
        default:
            printf("In %s, %d, Not Implement OPC_1 = %d at PC = %x\n", __func__, __LINE__, OPC_1, state->pc);
            break;
    }

    return data;
}

ARMword mrc_processSystemControlRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2)
{
    ARMword data = 0;

    switch (OPC_1)
    {
        case 0:
            if (CRm == 0)
            {
                switch (OPC_2)
                {
                    case 0:
                        data = state->system_reg.SCTLR_EL1.all;
                        break;
                    case 1:
                        data = state->system_reg.ACTLR_EL1.ACTLR;
                        break;
                    case 2:
                        data = state->system_reg.CPACR_EL1.CAPCR;
                        break;
                    case 3:
                        data = state->system_reg.ACTLR_EL1.ARCLR2;
                        break;
                    default:
                        printf("In %s, %d, Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2, state->pc);
                        break;
                }
            } else
            {
                printf("In %s, %d, Not Implement CRm = %d at PC = %x\n", __func__, __LINE__, CRm, state->pc);
            }
            break;
        default:
            printf("In %s, %d, Not Implement OPC_1 = %d at PC = %x\n", __func__, __LINE__, OPC_1, state->pc);
            break;
    }

    return data;
}

ARMword mrc_processMemorySystemControlRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2)
{
    ARMword data = 0;

    if (OPC_1 == 0)
    {
        if (CRm == 0)
        {
            switch (OPC_2)
            {
                case 0:
                    data = state->mmu.TTBR0.all;
                    //printf("In %s, %d, mmu.TTBR0.all = 0x%x at PC = %x\n", __func__, __LINE__, state->mmu.TTBR0.all, state->realPC);
                    break;
                case 1:
                    data = state->mmu.TTBR1.all;
                    break;
                case 2:
                    /* TTBC */
                    data = state->mmu.TTBCR.all;
                    break;
                default:
                    printf("In %s, %d, Not Implement CRm = %d at PC = %x\n", __func__, __LINE__, OPC_2, state->realPC);
                    break;
            }
        } else
        {
            printf("In %s, %d, Not Implement CRm = %d at PC = %x\n", __func__, __LINE__, CRm, state->realPC);
        }
    } else
    {
        printf("In %s, %d, Not Implement OPC_1 = %d at PC = %x\n", __func__, __LINE__, OPC_1, state->realPC);
    }

    return data;
}

ARMword mrc_processGICSystemRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2)
{
    printf("In %s, %d, Not implement OPC_1 = %d CRm = %d OPC_2 = %d PC = %x\n", __func__, __LINE__, OPC_1, CRm, OPC_2, state->realPC);
}

ARMword mrc_processMemorySystemFaultRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2)
{
    ARMword data = 0;

    switch (OPC_1)
    {
        case 0:
            if (CRm == 0)
            {
                switch (OPC_2)
                {
                    case 0:
                        data = state->system_reg.DFSR;
                        break;
                    default:
                        printf("In %s, line: %d Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2, state->realPC);
                        break;
                }
            } else
            {
                printf("In %s, Not Implement CRm = %d at PC = %x\n", __func__, CRm, state->realPC);
            }
            break;
        default:
            printf("In %s, Not Implement OPC_1 = %d at PC = %x\n", __func__, OPC_1, state->realPC);
            break;
    }

    return data;
}

ARMword mrc_processCacheOrAddressTranslate(ARMul_State * state, int OPC_1, int CRm, int OPC_2)
{
    ARMword data = 0;

    switch (OPC_1)
    {
        case 0:
            if (CRm == 5)
            {
                switch (OPC_2)
                {
                    case 0:
                        //IC IALLU instruction
                        break;
                    case 4:
                        //CP15 ISB instruction
                        break;
                    default:
                        printf("In %s, line: %d Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2,
                               state->realPC);
                        break;
                }
            } else if (CRm == 6)
            {
                switch (OPC_2)
                {
                    case 1:
                        //DC IMVAC instruction : Data Cache line Invalidate by VA to PoC
                        break;
                    case 2:
                        //DC ISW instruction: Data Cache line Invalidate by Set/Way
                        break;
                    default:
                        printf("In %s, line: %d Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2,
                               state->realPC);
                        break;
                }
            } else if (CRm == 14)
            {
                switch (OPC_2)
                {
                    case 1:
                        //DCCMVAU instruction : Data Cache line Clean by VA to PoU
                        break;
                    case 2:
                        //DCCIMVAC instruction: Data Cache line Clean and Invalidate by VA to PoC
                        break;
                    default:
                        printf("In %s, line: %d Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2,
                               state->realPC);
                        break;
                }
            } else
            {
                printf("In %s, Not Implement CRm = %d at PC = %x\n", __func__, CRm, state->realPC);
            }
            break;
        default:
            printf("In %s, Not Implement OPC_1 = %d at PC = %x\n", __func__, OPC_1, state->realPC);
            break;
    }

    return data;
}

ARMword mrc_processTLBOperation(ARMul_State * state, int OPC_1, int CRm, int OPC_2)
{
    printf("In %s, %d, Not implement OPC_1 = %d CRm = %d OPC_2 = %d PC = %x\n", __func__, __LINE__, OPC_1, CRm, OPC_2, state->realPC);
}

ARMword mrc_processPerformanceMontiors(ARMul_State * state, int OPC_1, int CRm, int OPC_2)
{
    printf("In %s, %d, Not implement OPC_1 = %d CRm = %d OPC_2 = %d PC = %x\n", __func__, __LINE__, OPC_1, CRm, OPC_2, state->realPC);
}

ARMword mrc_processMemoryMapRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2)
{
    ARMword data = 0;

    if (OPC_1 == 0 && CRm ==2)
    {
        if (OPC_2 == 0)
        {
            data = state->system_reg.MAIR_EL1.low32;
        } else if (OPC_2)
        {
            data = state->system_reg.MAIR_EL1.high32;
        } else
        {
            printf("In %s, %d, Not implment OPC_2 = %d PC = %x\n", __func__, __LINE__, OPC_2, state->realPC);
        }
    } else
    {
        printf("In %s, %d, Not implment OPC_1 = %d CRm = %d  OPC_2 = %d PC = %x\n", __func__, __LINE__, OPC_1, CRm, OPC_2, state->realPC);
    }
    return data;
}

ARMword mrc_processTCMAccess(ARMul_State * state, int OPC_1, int CRm, int OPC_2)
{
    printf("In %s, %d, Not implement OPC_1 = %d CRm = %d OPC_2 = %d PC = %x\n", __func__, __LINE__, OPC_1, CRm, OPC_2, state->realPC);
}

ARMword mrc_processContextThreadIDRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2)
{
    //printf("In %s, %d, Not implement OPC_1 = %d CRm = %d OPC_2 = %d PC = %x\n", __func__, __LINE__, OPC_1, CRm, OPC_2, //state->realPC);
    ARMword data = 0;

    switch (OPC_1)
    {
        case 0:
            if(CRm = 0)
            {
                switch (OPC_2)
                {
                    case 1:
                        data = state->system_reg.CONTEXTIDR_EL1.all;
                        break;
                    case 2:
                        data = state->system_reg.TPIDR_EL0;
                        break;
                    case 3:
                        data = state->system_reg.TPIDRRO_EL0;
                        break;
                    case 4:
                        printf("In %s, Not Implement TPIDRPRW\n", __func__);
                        break;
                    default:
                        printf("In %s, Not Implement OPC_2 = %d at PC = %x\n", __func__, OPC_2, state->realPC);
                        break;
                }
            }
            break;
        default:
            printf("In %s, Not Implement OPC_1 = %d at PC = %x\n", __func__, OPC_1, state->realPC);
            break;
    }
    return data;
}

ARMword mrc_processGenericTimerRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2)
{
    ARMword data = 0;

    switch (OPC_1)
    {
        case 0:
            if (CRm == 0)
            {
                switch (OPC_2)
                {
                    case 0:
                        data = state->system_reg.CNTPCT_EL0;
                        break;
                    default:
                        printf("In %s, Not Implement OPC_2 = %d at PC = %x\n", __func__, OPC_2, state->realPC);
                        break;
                }
            } else if (CRm == 1)
            {
                switch (OPC_2)
                {
                    case 0:
                        data = state->system_reg.CNTKCTL_EL1;
                        break;
                    default:
                        printf("In %s, Not Implement OPC_2 = %d at PC = %x\n", __func__, OPC_2, state->realPC);
                        break;
                }
            } else if (CRm == 2)
            {
                switch (OPC_2)
                {
                    case 0:
                        //If CNTP_CTL_EL0.ENABLE is 1, the value returned is (CNTP_CVAL_EL0 - CNTPCT_EL0)
                        if (state->system_reg.CNTP_CTL_EL0.ENABLE)
                        {
                            data = state->system_reg.CNTP_CVAL_EL0 - state->system_reg.CNTPCT_EL0;
                        } else
                        {
                            data = 0;
                        }
                        break;
                    case 1:
                        data = state->system_reg.CNTP_CTL_EL0.all;
                        break;
                    default:
                        printf("In %s, Not Implement OPC_2 = %d at PC = %x\n", __func__, OPC_2, state->realPC);
                        break;
                }
            } else if (CRm == 3)
            {
                switch (OPC_2)
                {
                    case 0:
                        data = state->system_reg.CNTV_TVAL_EL0;
                        break;
                    case 1:
                        data = state->system_reg.CNTV_CTL_EL0.all;
                        break;
                    case 2:
                        data = state->system_reg.CNTV_CVAL_EL0;
                        break;
                    default:
                        printf("In %s, Not Implement OPC_2 = %d at PC = %x\n", __func__, OPC_2, state->realPC);
                        break;
                }
            } else
            {
                printf("In %s, Not Implement CRm = %d at PC = %x\n", __func__, CRm, state->realPC);
            }
            break;
        case 3:
            if (CRm == 0 && OPC_2 == 0)
            {
                data = state->system_reg.CNTFRQ_EL0;
            } else
            {
                printf("In %s, Line:%d, Not Implement CRm = %d OPC_2 = %d at PC = %x\n", __func__, __LINE__, CRm, OPC_2, state->realPC);
            }
            break;
        default:
            printf("In %s, Not Implement OPC_1 = %d at PC = %x\n", __func__, OPC_1, state->realPC);
            break;
    }

    return data;
}

ARMword ftc662_mmu_mrc(ARMul_State * state, ARMword instr, ARMword * value)
{
    int CRn = BITS(16, 19) & 0xf;
    int OPC_1 = BITS(21, 23) & 0x7;
    int OPC_2 = BITS(5, 7) & 0x7;
    int CRm = BITS(0, 3) & 0xf;
    ARMword data = 0;

    switch (CRn)
    {
        case 0:
            data = mrc_processIDRegister(state, OPC_1, CRm, OPC_2);
            break;
        case 1:
            data = mrc_processSystemControlRegister(state, OPC_1, CRm, OPC_2);
            break;
        case 2:
            data = mrc_processMemorySystemControlRegister(state, OPC_1, CRm, OPC_2);
            break;
        case 3:
            data = state->mmu.domain_access_control;
            break;
        case 4:
            data = mrc_processGICSystemRegister(state, OPC_1, CRm, OPC_2);
            break;
        case 5:
        case 6:
            data = mrc_processMemorySystemFaultRegister(state, OPC_1, CRm, OPC_2);
            break;
        case 7:
            data = mrc_processCacheOrAddressTranslate(state, OPC_1, CRm, OPC_2);
            break;
        case 8:
            data = mrc_processTLBOperation(state, OPC_1, CRm, OPC_2);
            break;
        case 9:
            data = mrc_processPerformanceMontiors(state, OPC_1, CRm, OPC_2);
            break;
        case 10:
            data = mrc_processMemoryMapRegister(state, OPC_1, CRm, OPC_2);
            break;
        case 11:
            data = mrc_processTCMAccess(state, OPC_1, CRm, OPC_2);
            break;
        case 12:
            //Not implement
            break;
        case 13:
            data = mrc_processContextThreadIDRegister(state, OPC_1, CRm, OPC_2);
            break;
        case 14:
            data = mrc_processGenericTimerRegister(state, OPC_1, CRm, OPC_2);
            break;
        default:
            printf("In %s, Not Implement CRn = %d at PC = %x\n", __func__, CRn, state->realPC);
            break;
    }

    *value = data;
    return data;
}

void mcr_processIDRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2, ARMword value)
{
    switch (OPC_1)
    {
        case 1:
            if (CRm == 0)
            {
                switch (OPC_2)
                {
                    case 0:
                        state->system_reg.CCSIDR_EL1.CCSIDR = value;
                        break;
                    case 1:
                        state->system_reg.CLIDR_EL1.CLIDR = value;
                        break;
                    case 2:
                        state->system_reg.CCSIDR2_EL1.CCSIDR2 = value;
                        break;
                    case 7:
                        state->system_reg.AIDR_EL1.AIDR = value;
                        break;
                    default:
                        printf("In %s, Not Implement OPC_2 = %d at PC = %x\n", __func__, OPC_2, state->realPC);
                        break;
                }
            } else
            {
                printf("In %s, Not Implement CRm = %d at PC = %x\n", __func__, CRm, state->realPC);
            }
            break;
        case 2:
            if (CRm == 0 && OPC_2 == 0)
            {
                state->system_reg.CSSELR_EL1 = value;
            } else
            {
                printf("In %s, Not Implement CRm = %d at PC = %x\n", __func__, CRm, state->realPC);
            }
            break;
        default:
            printf("In %s, Not Implement OPC_1 = %d at PC = %x\n", __func__, OPC_1, state->realPC);
            break;
    }

}

void mcr_processSystemControlRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2, ARMword value)
{
    switch (OPC_1)
    {
        case 0:
            if (CRm == 0)
            {
                switch (OPC_2)
                {
                    case 0:
                        state->system_reg.SCTLR_EL1.all = value;
                        state->mmu.SCTLR.all = value;
                        state->mmu.control = state->mmu.SCTLR.all;
                        break;
                    case 1:
                        state->system_reg.ACTLR_EL1.ACTLR = value;
                        break;
                    case 2:
                        state->system_reg.CPACR_EL1.CAPCR = value;
                        break;
                    case 3:
                        state->system_reg.ACTLR_EL1.ARCLR2 = value;
                        break;
                    default:
                        printf("In %s, Not Implement OPC_2 = %d at PC = %x\n", __func__, OPC_2, state->realPC);
                        break;
                }
            } else
            {
                printf("In %s, Not Implement CRm = %d at PC = %x\n", __func__, CRm, state->realPC);
            }
            break;
        default:
            printf("In %s, Not Implement OPC_1 = %d at PC = %x\n", __func__, OPC_1, state->realPC);
            break;
    }

}

void mcr_processMemorySystemControlRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2, ARMword value)
{

    if (OPC_1 == 0)
    {
        if (CRm == 0)
        {
            switch (OPC_2)
            {
                case 0:
                    state->mmu.TTBR0.all = (value);
                    state->mmu.translation_table_base0 = (value);
                    //printf("In %s, TTBR0.all = 0%lx at PC = %x\n", __func__, value, state->realPC);
                    break;
                case 1:
                    state->mmu.TTBR1.all = (value);
                    state->mmu.translation_table_base1 = (value);
                    //printf("In %s, TTBR1.all = 0%lx at PC = %x\n", __func__, value, state->realPC);
                    break;
                case 2:
                    /* TTBCR */
                    state->mmu.TTBCR.all = value;
                    state->mmu.translation_table_ctrl = value;
                    //printf("In %s, %d, state->mmu.TTBCR.all = 0x%x at PC = %x\n", __func__, __LINE__, state->mmu.TTBCR.all, state->realPC);
                    if (!state->isAArch64)
                    {
                        if ((state->mmu.TTBCR.all >> 31 & 0x01) == 0x01)
                        {
                            state->translate_type = arch32_long_translate;
                        } else
                        {
                            state->translate_type = arch32_short_translate;
                        }
                    }
                    break;
                default:
                    printf("In %s, Not Implement OPC_2 = %d at PC = %x\n", __func__, OPC_2, state->realPC);
                    break;
            }
        } else
        {
            printf("In %s, Not Implement CRm = %d at PC = %x\n", __func__, OPC_2, state->realPC);
        }
    } else
    {
        printf("In %s, Not Implement OPC_1 = %d at PC = %x\n", __func__, OPC_1, state->realPC);
    }
}

void mcr_processGICSystemRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2, ARMword value)
{

    printf("In %s, %d, Not implment OPC_1 = %d CRm = %d OPC_2 = %d PC = %x\n", __func__, __LINE__, OPC_1, CRm, OPC_2, state->realPC);
}

void mcr_processMemorySystemFaultRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2, ARMword value)
{
    printf("In %s, %d, Not implment OPC_1 = %d CRm = %d OPC_2 = %d PC = %x\n", __func__, __LINE__, OPC_1, CRm, OPC_2, state->realPC);
}

void mcr_processCacheOrAddressTranslate(ARMul_State * state, int OPC_1, int CRm, int OPC_2, ARMword value)
{
    switch (OPC_1)
    {
        case 0:
            if (CRm == 1)
            {
                 //ICIALLUIS Instruction cache invalidate all.Invalidate all instruction caches Inner Shareable to PoU.
                 break;
            } else if (CRm == 5)
            {
                switch (OPC_2)
                {
                    case 0:
                        //ICIALLU instruction cache invalidate all
                        break;
                    case 1:
                        //ICIMVA Instruction cache invalidate by MVA
                        break;
                    case 4:
                        //CP15 ISB instruction
                        break;
                    case 6:
                        //BPIALL Branch predictor invalidate all
                        break;
                    case 7:
                        //BPIMVA Branch predictor invalidate by MVA
                        break;
                    default:
                        printf("In %s, line: %d Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2,
                               state->realPC);
                        break;
                }
            } else if (CRm == 6)
            {
                switch (OPC_2)
                {
                    case 1:
                        //DC IMVAC instruction : Data Cache line Invalidate by VA to PoC
                        break;
                    case 2:
                        //DC ISW instruction: Data Cache line Invalidate by Set/Way
                        break;
                    default:
                        printf("In %s, line: %d Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2,
                               state->realPC);
                        break;
                }
            } else if (CRm == 14)
            {
                switch (OPC_2)
                {
                    case 1:
                        //DCCMVAU instruction : Data Cache line Clean by VA to PoU
                        break;
                    case 2:
                        //DCCIMVAC instruction: Data Cache line Clean and Invalidate by VA to PoC
                        break;
                    default:
                        printf("In %s, line: %d Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2,
                               state->realPC);
                        break;
                }
            } else if (CRm == 10)
            {
                switch (OPC_2)
                {
                    case 1:
                        //DCCMVAC, Clean data cache line by MVA to PoC
                        break;
                    case 2:
                        //DCCSW, Clean data cache line by set/way
                        break;
                    case 4:
                        //CP15DSB, Data Synchronization Barrier operation
                        break;
                    default:
                        printf("In %s, line: %d Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2,
                               state->realPC);
                        break;
                }
                break;
            } else if (CRm == 11)
            {
                //DCCMVAU data cache clean by MVA to PoU
                break;
            } else
            {
                printf("In %s, %d, Not Implement CRm = %d at PC = %x\n", __func__, __LINE__, CRm, state->realPC);
            }
            break;
        default:
            printf("In %s, %d, Not Implement OPC_1 = %d at PC = %x\n", __func__, __LINE__, OPC_1, state->realPC);
            break;
    }

}

void mcr_processTLBOperation(ARMul_State * state, int OPC_1, int CRm, int OPC_2, ARMword value)
{
    switch (OPC_1)
    {
        case 0:
            if (CRm == 7)
            {
                switch (OPC_2)
                {
                    case 0:
                        /*TLBIALL instruction */
                        break;
                    case 1:
                        /*TLB ID Flush Entry*/
                        break;
                    default:
                        printf("In %s, line: %d Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2,
                               state->realPC);
                        break;

                }
            }
            break;
        default:
            printf("In %s, Not Implement OPC_1 = %d at PC = %x\n", __func__, OPC_1, state->realPC);
            break;
    }
}

void mcr_processPerformanceMontiors(ARMul_State * state, int OPC_1, int CRm, int OPC_2, ARMword value)
{
    printf("In %s, %d, Not implment OPC_1 = %d CRm = %d OPC_2 = %d PC = %x\n", __func__, __LINE__, OPC_1, CRm, OPC_2, state->realPC);
}

void mcr_processMemoryMapRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2, ARMword value)
{
    if( OPC_1 == 0 && CRm == 2)
    {
        if (OPC_2 == 0)
        {
            state->system_reg.MAIR_EL1.low32 = value;
        } else if (OPC_2 == 1)
        {
            state->system_reg.MAIR_EL1.high32 = value;
        } else
        {
            printf("In %s, %d, Not implment OPC_2 = %d PC = %x\n", __func__, __LINE__, OPC_2, state->realPC);
        }
    } else
    {
        printf("In %s, %d, Not implment OPC_1 = %d CRm = %d  OPC_2 = %d PC = %x\n", __func__, __LINE__, OPC_1, CRm, OPC_2, state->realPC);
    }
}

void mcr_processTCMAccess(ARMul_State * state, int OPC_1, int CRm, int OPC_2, ARMword value)
{
    printf("In %s, %d, Not implment OPC_1 = %d CRm = %d OPC_2 = %d PC = %x\n", __func__, __LINE__, OPC_1, CRm, OPC_2, state->realPC);
}

void mcr_processExceptionRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2, ARMword value)
{
    switch (OPC_1)
    {
        case 0:
            if (CRm == 0)
            {
                switch (OPC_2)
                {
                    case 0:
                        state->system_reg.VBAR_EL1 = (value & ~0x1f);
                        state->vector_remap_addr = (value & ~0x1f);
                        /* Mark the vector_addr is remapped */
                        state->vector_remap_flag = 1;
                        break;
                    default:
                        printf("In %s, line: %d Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2,
                               state->realPC);
                        break;

                }
            }
            break;
        default:
            printf("In %s, Not Implement OPC_1 = %d at PC = %x\n", __func__, OPC_1, state->realPC);
            break;
    }

}

void mcr_processContextThreadIDRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2, ARMword value)
{
    switch (OPC_1)
    {
        case 0:
            if (CRm == 0)
            {
                switch (OPC_2)
                {
                    case 1:
                        state->system_reg.CONTEXTIDR_EL1.PROCID = value;
                        break;
                    case 2:
                        state->system_reg.TPIDR_EL0 = value;
                        break;
                    case 3:
                        state->system_reg.TPIDRRO_EL0 = value;
                        break;
                    case 4:
                        printf("In %s, Not Implement TPIDRPRW\n", __func__);
                        break;
                    default:
                        printf("In %s, line: %d Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2, state->realPC);
                        break;
                }
            } else
            {
                 printf("In %s, line: %d Not Implement OPC_2 = %d at PC = %x\n", __func__, __LINE__, OPC_2, state->realPC);
            }
            break;
        default:
            printf("In %s, Not Implement OPC_1 = %d at PC = %x\n", __func__, OPC_1, state->realPC);
            break;
    }
}

void mcr_processGenericTimerRegister(ARMul_State * state, int OPC_1, int CRm, int OPC_2, ARMword value)
{
    switch (OPC_1)
    {
        case 0:
            if (CRm == 0)
            {
                switch (OPC_2)
                {
                    case 0:
                        state->system_reg.CNTPCT_EL0 = value;
                        break;
                    default:
                        printf("In %s, Not Implement OPC_2 = %d at PC = %x\n", __func__, OPC_2, state->realPC);
                        break;
                }
            } else if (CRm == 1)
            {
                switch (OPC_2)
                {
                    case 0:
                        state->system_reg.CNTKCTL_EL1 = value;
                        break;
                    default:
                        printf("In %s, Not Implement OPC_2 = %d at PC = %x\n", __func__, OPC_2, state->realPC);
                        break;
                }
            } else if (CRm == 2)
            {
                switch (OPC_2)
                {
                    case 0:
                        //On a write of this register, CNTP_CVAL_EL0 is set to (CNTPCT_EL0 + TimerValue)
                        state->system_reg.CNTP_CVAL_EL0 = state->system_reg.CNTPCT_EL0 + value;
                        break;
                    case 1:
                        state->system_reg.CNTP_CTL_EL0.all = value;
                        break;
                    default:
                        printf("In %s, Not Implement OPC_2 = %d at PC = %x\n", __func__, OPC_2, state->realPC);
                        break;
                }
            } else if (CRm == 3)
            {
                switch (OPC_2)
                {
                    case 0:
                        state->system_reg.CNTV_TVAL_EL0 = state->system_reg.CNTVCT_EL0 + value;
                        break;
                    case 1:
                        state->system_reg.CNTV_CTL_EL0.all = value;
                        break;
                    case 2:
                        state->system_reg.CNTV_CVAL_EL0 = value;
                        break;
                    default:
                        printf("In %s, Not Implement OPC_2 = %d at PC = %x\n", __func__, OPC_2, state->pc);
                        break;
                }
            } else
            {
                printf("In %s, Not Implement CRm = %d at PC = %x\n", __func__, CRm, state->realPC);
            }
            break;
        case 3:
            if (CRm == 0 && OPC_2 == 0)
            {
                state->system_reg.CNTFRQ_EL0 = value;
            } else
            {
                printf("In %s, Line:%d, Not Implement CRm = %d OPC_2 = %d at PC = %x\n", __func__, __LINE__, CRm, OPC_2, state->realPC);
            }
            break;
        default:
            printf("In %s, Not Implement OPC_1 = %d at PC = %x\n", __func__, OPC_1, state->realPC);
            break;
    }

}

/*
 *system register               CRn             opc1            CRm             opc2
 *MIDR                                  c0              0                       c0              0,4,6,7
 *CTR                                   c0              0                       c0              1
 *TCMTR                                 c0              0                       c0              2
 *TLBTR                                 c0              0                       c0              3
 *MPIDR                                 c0              0                       c0              5
 *ID_PFR0                               c0              0                       c1              0
 *ID_PFR1                               c0              0                       c1              0
 *ID_DFR0                               c0              0                       c1              0
 *ID_AFR0                               c0              0                       c1              0
 *ID_MMFR0                              c0              0                       c1              0
 *ID_MMFR1                              c0              0                       c1              0
 *ID_MMFR2                              c0              0                       c1              0
 *ID_MMFR3                              c0              0                       c1              0
 *ID_ISAR0                              c0              0                       c1              0
 *ID_ISAR1                              c0              0                       c1              0
 *ID_ISAR2                              c0              0                       c1              0
 *ID_ISAR3                              c0              0                       c1              0
 *ID_ISAR4                              c0              0                       c1              0
 *ID_ISAR5                              c0              0                       c1              0
 *ID_MMFR4                              c0              0                       c1              0
 *ID_PFR2                               c0              0                       c1              0
 *CCSIDR                                c0              0                       c1              0
 *CLIDR                         c0              0                       c1              0
 *CCSIDR2                               c0              0                       c1              0
 *AIDR                          c0              0                       c1              0
 *CSSELR                                c0              0                       c1              0
 *VPIDR                         c0              0                       c1              0
 *VMPIDR                                c0              0                       c1              0
 *SCTLR                         c0              0                       c1              0
 *ACTLR                         c0              0                       c1              0
 *CPACR                         c0              0                       c1              0
 *ACTLR2                                c0              0                       c1              0
 *SCR                           c0              0                       c1              0
 *SDER                          c0              0                       c1              0
 *NSACR                         c0              0                       c1              0
 *TRFCR                         c0              0                       c1              0
 *SDCR                          c0              0                       c1              0
 * */
static ARMword ftc662_mmu_mcr(ARMul_State * state, ARMword instr, ARMword value)
{
    int CRn = BITS(16, 19) & 0xf;
    int OPC_1 = BITS(21, 23) & 0x7;
    int OPC_2 = BITS(5, 7) & 0x7;
    int CRm = BITS(0, 3) & 0xf;

    switch (CRn)
    {
        case 0:
            mcr_processIDRegister(state, OPC_1, CRm, OPC_2, value);
            break;
        case 1:
            mcr_processSystemControlRegister(state, OPC_1, CRm, OPC_2, value);
            break;
        case 2:
            mcr_processMemorySystemControlRegister(state, OPC_1, CRm, OPC_2, value);
            break;
        case 3:
            state->mmu.domain_access_control = value;
            break;
        case 4:
            mcr_processGICSystemRegister(state, OPC_1, CRm, OPC_2, value);
            break;
        case 5:
        case 6:
            mcr_processMemorySystemFaultRegister(state, OPC_1, CRm, OPC_2, value);
            break;
        case 7:
            mcr_processCacheOrAddressTranslate(state, OPC_1, CRm, OPC_2, value);
            break;
        case 8:
            mcr_processTLBOperation(state, OPC_1, CRm, OPC_2, value);
            break;
        case 9:
            mcr_processPerformanceMontiors(state, OPC_1, CRm, OPC_2, value);
            break;
        case 10:
            mcr_processMemoryMapRegister(state, OPC_1, CRm, OPC_2, value);
            break;
        case 11:
            mcr_processTCMAccess(state, OPC_1, CRm, OPC_2, value);
            break;
        case 12:
            mcr_processExceptionRegister(state, OPC_1, CRm, OPC_2, value);
            break;
        case 13:
            mcr_processContextThreadIDRegister(state, OPC_1, CRm, OPC_2, value);
            break;
        case 14:
            mcr_processGenericTimerRegister(state, OPC_1, CRm, OPC_2, value);
            break;
        default:
            printf("In %s, Not Implement CRn = %d at PC = %x\n", __func__, CRn, state->realPC);
            break;
    }

    return No_exp;
}

mmu_ops_t ftc662_mmu_ops = {
    ftc662_mmu_init,
    ftc662_mmu_exit,
    ftc662_mmu_read_byte,
    ftc662_mmu_write_byte,
    ftc662_mmu_read_halfword,
    ftc662_mmu_write_halfword,
    ftc662_mmu_read_word,
    ftc662_mmu_write_word,
    ftc662_mmu_load_instr,
    ftc662_mmu_mcr,
    ftc662_mmu_mrc,
    NULL
};

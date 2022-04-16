/*
 * cortex_r4_mpu.c - cortex_r4 Memory Protection Unit emulation.
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
#include "armdefs.h"

static fault_t cortex_r4_mpu_write(ARMul_State * state, ARMword va, ARMword data, ARMword datatype);
static fault_t cortex_r4_mpu_read(ARMul_State * state, ARMword va, ARMword * data, ARMword datatype);

static uint32_t region_size_map[32] = {
    //        0          1          2         3
    0, 0, 0, 0,

    //                              4          5          6           7           8           9          10           11
    0x1F, 0x3F, 0x7F, 0xFF, 0x1FF, 0x3FF, 0x7FF, 0xFFF,

    //       12         13         14         15         16          17          18          19          20           21
    0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF, 0x1FFFF, 0x3FFFF, 0x7FFFF, 0xFFFFF, 0x1FFFFF, 0x3FFFFF,

    //       22         23         24         25         26          27          28          29          30           31
    0x7FFFFF, 0xFFFFFF, 0x1FFFFFF, 0x3FFFFFF, 0x7FFFFFF, 0xFFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF,
};

#define MPU_AP_READ  0x010
#define MPU_AP_WRITE 0x001
#define MPU_AP_UNP   0x100

static uint32_t mpu_access_table[8][2] = {
    // Privileged, User Read/Write
    {0x000, 0x000},
    {0x011, 0x000},
    {0x011, 0x010},
    {0x011, 0x011},
    {0x100, 0x100},
    {0x010, 0x000},
    {0x010, 0x010},
    {0x000, 0x000},
};

int mpu_get_address_region_number(ARMul_State * state, ARMword address)
{
    int i;

    for (i = MPU_REGION_SIZE - 1; i >= 0; i--)
    {
        if ((address >= state->mpu.regions[i].base_address) && (address < state->mpu.regions[i].end_address))
        {
            if (state->mpu.regions[i].enable)
            {
                uint32_t subregion_size;
                uint32_t subregion_index;
                uint32_t subregion_offset;

                subregion_size = region_size_map[state->mpu.regions[i].size] / MPU_SUBREGION_NUM;
                subregion_offset = address - state->mpu.regions[i].base_address;
                subregion_index = subregion_offset / subregion_size;
                if (state->mpu.regions[i].subregion_enables & (1 << subregion_index))
                {
                    //printf("========== subregion_enables is disable 0x%x subregion_enables: %d i: %d\n", address, state->mpu.regions[i].subregion_enables, i);
                    return -2;
                } else
                {
                    //printf("[%02d]: address: 0x%08x subregion_index: %d\n", i, address, subregion_index);
                    return i;
                }
            }
        }
    }

    return -1;
}

#define ARM_PRIV_MODE 0
#define ARM_USER_MODE 1
int mpu_get_executive_authority(ARMul_State * state)
{
    int mode_result = -1;

    // CPST M[4-0]
    if (state->Cpsr & 0xF)
    {
        mode_result = ARM_PRIV_MODE;
    } else
    {
        mode_result = ARM_USER_MODE;
    }

    return mode_result;
}

int cortex_r4_mpu_init(ARMul_State * state)
{
    state->mpu.process_id = 0;

    return 0;
}

void cortex_r4_mpu_exit(ARMul_State * state)
{
};

static fault_t cortex_r4_mpu_read_byte(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    // ARMword temp, offset;
    return cortex_r4_mpu_read(state, virt_addr, data, ARM_BYTE_TYPE);
}

static fault_t cortex_r4_mpu_read_halfword(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    // ARMword temp, offset;
    return cortex_r4_mpu_read(state, virt_addr, data, ARM_HALFWORD_TYPE);
}

static fault_t cortex_r4_mpu_read_word(ARMul_State * state, ARMword virt_addr, ARMword * data)
{
    return cortex_r4_mpu_read(state, virt_addr, data, ARM_WORD_TYPE);
}

static fault_t cortex_r4_mpu_read(ARMul_State * state, ARMword va, ARMword * data, ARMword datatype)
{
    fault_t fault = NO_FAULT;

    int region_number;
    int exec_mode;
    int ap;
    int have_permission;

    if (state->mpu.SCTLR & SCTLR_M)
    {

        if (state->mpu.SCTLR & SCTLR_A)
        {
            if (va % 4 != 0)
            {
                return TLB_READ_MISS;
            }
        }

        region_number = mpu_get_address_region_number(state, va);
        exec_mode = mpu_get_executive_authority(state);

        if (region_number < 0)
        {
            //printf("[Read]: Get region number error, address 0x%x\n", va);
            skyeye_log(Warning_log, __FUNCTION__, "Get region number error, address 0x%x\n", va);

            // CHECK: Background fault
            if (region_number == -2)
            {
                /* A background fault is generated when the MPU is enabled and a memory access is made to an
                 * address that is not within an enabled subregion of an MPU region.
                 * A background fault does not occur if the background region is enabled and the access is Privileged.
                 */
                if ((state->mpu.SCTLR & SCTLR_BR) && (exec_mode = ARM_PRIV_MODE))
                {
                } else
                {
                    return TLB_READ_MISS;
                }
            }
        }

        ap = state->mpu.regions[region_number].access_permissions;
        have_permission = 0;

        // Check Access data permission bit encoding
        if (mpu_access_table[ap][exec_mode] & MPU_AP_READ)
        {
            have_permission = 1;
        }

        if (!have_permission)
        {
            //printf("  [Read] hava_permission error\n");
            //printf("   have_permission: %d ap: %d exec_mode: %d \n", have_permission, ap, exec_mode);
            return TLB_READ_MISS;
        }
    }

    switch (datatype)
    {
        case ARM_BYTE_TYPE:
            {
                state->memory_space_iface->read(state->memory_space, va, data, 1);
            }
            break;
        case ARM_HALFWORD_TYPE:
            {
                state->memory_space_iface->read(state->memory_space, va, data, 2);
            }
            break;
        case ARM_WORD_TYPE:
            {
                state->memory_space_iface->read(state->memory_space, va, data, 4);
            }
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "SKYEYE:1 cortex_r4_mpu_read error: unknown data type %d\n", datatype);
            skyeye_exit(-1);
            break;
    }

    return fault;
}

static fault_t cortex_r4_mpu_write_byte(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return cortex_r4_mpu_write(state, virt_addr, data, ARM_BYTE_TYPE);
}

static fault_t cortex_r4_mpu_write_halfword(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return cortex_r4_mpu_write(state, virt_addr, data, ARM_HALFWORD_TYPE);
}

static fault_t cortex_r4_mpu_write_word(ARMul_State * state, ARMword virt_addr, ARMword data)
{
    return cortex_r4_mpu_write(state, virt_addr, data, ARM_WORD_TYPE);
}

static int op_compare(const void *ptr1, const void *ptr2)
{
    map_info_t *iterator1 = *(map_info_t **) ptr1;
    map_info_t *iterator2 = *(map_info_t **) ptr2;

    if (iterator1->base_addr < iterator2->base_addr)
    {
        return -1;
    } else
    {
        if (iterator1->base_addr >= iterator2->base_addr
            && (iterator1->base_addr <= iterator2->base_addr + iterator2->length))
        {
            return 0;
        }
    }
    return 1;
}

static exception_t memory_space_write(ARMul_State * state, conf_object_t * obj, generic_address_t addr,
                                      void *buf, size_t count)
{
    addr_space_t *space = obj->obj;
    exception_t ret;

    if (space->extra_core != NULL)
    {
        return space->extra_iface->write(space->extra_core, addr, buf, count);
    }

    map_info_t key;

    key.base_addr = addr;
    map_info_t *pkey = &key;
    map_info_t **iterator;

    iterator = bsearch(&pkey, space->map_array, space->map_count, sizeof (map_info_t *), op_compare);
    if (iterator != NULL)
    {
        RW_WRLOCK(((*iterator)->lock));
        if ((state->mode == 1 || state->mode == 3) && (strcmp((*iterator)->obj->class_name, "ram") == 0))
        {
#if FAST_MEMORY
            memcpy(addr, buf, count);
#else
            ret = (*iterator)->memory_space->write((*iterator)->obj, (addr - (*iterator)->base_addr), buf, count);
#endif
        } else
        {
            ret = (*iterator)->memory_space->write((*iterator)->obj, (addr - (*iterator)->base_addr), buf, count);
        }
        RW_UNLOCK(((*iterator)->lock));
        return ret;
    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "Can't access the address 0x%x\n", addr);
    }

    return Not_found_exp;
}

static fault_t cortex_r4_mpu_write(ARMul_State * state, ARMword va, ARMword data, ARMword datatype)
{
    fault_t fault = NO_FAULT;

    int region_number;
    int exec_mode;
    int ap;
    int have_permission;

    if (state->mpu.SCTLR & SCTLR_M)
    {

        if (state->mpu.SCTLR & SCTLR_A)
        {
            if (va % 4 != 0)
            {
                return TLB_WRITE_MISS;
            }
        }

        region_number = mpu_get_address_region_number(state, va);
        exec_mode = mpu_get_executive_authority(state);

        if (region_number < 0)
        {
            //printf("[Read]: Get region number error, address 0x%x\n", va);
            skyeye_log(Warning_log, __FUNCTION__, "Get region number error, address 0x%x\n", va);

            // CHECK: Background fault
            if (region_number == -2)
            {
                /* A background fault is generated when the MPU is enabled and a memory access is made to an
                 * address that is not within an enabled subregion of an MPU region.
                 * A background fault does not occur if the background region is enabled and the access is Privileged.
                 */
                if ((state->mpu.SCTLR & SCTLR_BR) && (exec_mode = ARM_PRIV_MODE))
                {
                } else
                {
                    return TLB_WRITE_MISS;
                }
            }
        }

        ap = state->mpu.regions[region_number].access_permissions;
        have_permission = 0;

        // Check Access data permission bit encoding
        if (mpu_access_table[ap][exec_mode] & MPU_AP_WRITE)
        {
            have_permission = 1;
        }

        if (!have_permission)
        {
            //printf("  [Write] hava_permission error\n");
            //printf("   have_permission: %d ap: %d exec_mode: %d table: 0x%x\n", have_permission, ap, exec_mode, mpu_access_table[ap][exec_mode]);
            return TLB_WRITE_MISS;
        }
    }

    switch (datatype)
    {
        case ARM_BYTE_TYPE:
            {
                memory_space_write(state, state->memory_space, va, &data, 1);
            }
            break;
        case ARM_HALFWORD_TYPE:
            {
                memory_space_write(state, state->memory_space, va, &data, 2);
            }
            break;
        case ARM_WORD_TYPE:
            {
                memory_space_write(state, state->memory_space, va, &data, 4);
            }
            break;
        default:
            //printf ("SKYEYE:1 cortex_r4_mpu_write error: unknown data type %d\n", datatype);
            skyeye_log(Error_log, __FUNCTION__, "SKYEYE:1 cortex_r4_mpu_write error: unknown data type %d\n", datatype);
            skyeye_exit(-1);
    }

    return fault;
}

static fault_t cortex_r4_mpu_load_instr(ARMul_State * state, ARMword va, ARMword * instr)
{
    // Check MPU region XN
    int32_t region_number = mpu_get_address_region_number(state, va);

    uint32_t XN = state->mpu.regions[region_number].XN;

    if (XN == 0)
    {
        state->memory_space_iface->read(state->memory_space, va, instr, 4);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "[0x%08X] no instruction fetches enabled.\n", va);
    }
    return 0;
};

static ARMword cortex_r4_mpu_mcr(ARMul_State * state, ARMword instr, ARMword value)
{
    mmu_regnum_t CRn_reg = BITS(16, 19) & 0xf;
    mmu_regnum_t CRm_reg = instr & 0xf;
    int OPC_1 = BITS(21, 23) & 0x7;
    int OPC_2 = BITS(5, 7) & 0x7;

    //printf("[MCR]: 15, %d, r%d, cr%d cr%d, {%d}\n", OPC_1, BITS (12, 15), CRn_reg, CRm_reg, OPC_2);
    //printf("       pc: 0x%x value: 0x%x\n\n", state->Reg[15] - 8, value);
    switch (CRn_reg)
    {
        case MMU_CONTROL:
            /* Before the MPU is enabled, you must program at least one valid protection region.
             * If you do not do this, the processor enters a state that only reset can recover.
             */
            state->mpu.SCTLR = value & 0xFB2A7807;      // 0xFB2A7807 Cortex R4 MPU SCTLR MASK Code.
            break;
        case 6:
            switch (CRm_reg)
            {
                case 1:
                    {
                        switch (OPC_2)
                        {
                            case 0:
                                // region base address
                                // You must align this to a region-sizedboundary.
                                // For example, if a region size of 8KB is programmed for a given region,
                                // the base address must be a multiple of 8KB.
                                state->mpu.regions[CUR_REGION_NUM].base_address = value;
                                // If the region is not aligned correctly, this results in Unpredictable behavior.
                                break;
                            case 2:
                                {
                                    // region size. region enable. subregion enables
                                    /*  31         16 15                 8 7 6 5          1    0
                                     *  庚-----------房-------------------房--房-----------房-------庖
                                     *  |  Reserved  | Sub-region disable |   |Region size | Enable |
                                     *  弩-----------拂-------------------拂--拂-----------拂-------彼
                                     *
                                     * Sub-region disable  Each bit position represents a sub-region, 0-7
                                     * Region size         Defines the region size
                                     * Enable              Enables or disables a memory region
                                     *
                                     *  Defines the region size:           12 b01100 = 8KB   22 b10110 = 8MB
                                     *   0 b00000 - b00011 =Unpredictable  13 b01101 = 16KB  23 b10111 = 16MB
                                     *   4 b00100 = 32 bytes               14 b01110 = 32KB  24 b11000 = 32MB
                                     *   5 b00101 = 64 bytes               15 b01111 = 64KB  25 b11001 = 64MB
                                     *   6 b00110 = 128 bytes              16 b10000 = 128KB 26 b11010 = 128MB
                                     *   7 b00111 = 256 bytes              17 b10001 = 256KB 27 b11011 = 256MB
                                     *   8 b01000 = 512 bytes              18 b10010 = 512KB 28 b11100 = 512MB
                                     *   9 b01001 = 1KB                    19 b10011 = 1MB   29 b11101 = 1GB
                                     *  10 b01010 = 2KB                    20 b10100 = 2MB   30 b11110 = 2GB
                                     *  11 b01011 = 4KB                    21 b10101 = 4MB   31 b11111 = 4GB
                                     *
                                     */

                                    uint32_t subregion_enables = (value >> 8) & 0xFF;
                                    uint32_t region_size = (value >> 1) & 0x1F;
                                    uint32_t enable = value & 0x1;

                                    uint32_t region_size2 = 0;

                                    if ((region_size >= 4) && (region_size <= 31))
                                    {
                                        region_size2 = region_size_map[region_size];
                                    } else
                                    {
                                        // FIXME: This error
                                        skyeye_log(Error_log, __FUNCTION__, "MPU set region_size error, value is %d\n",
                                                   value);
                                        //printf("MPU set region_size error, value is %d %d 0x%x\n", value, region_size, region_size);
                                        break;
                                    }

                                    state->mpu.regions[CUR_REGION_NUM].size = region_size;
                                    state->mpu.regions[CUR_REGION_NUM].end_address =
                                        state->mpu.regions[CUR_REGION_NUM].base_address + region_size2;

                                    state->mpu.regions[CUR_REGION_NUM].enable = enable;
                                    state->mpu.regions[CUR_REGION_NUM].subregion_enables = subregion_enables;
                                    //printf("[CUR_REGION_NUM: %d]: 0x%08x - 0x%08x XN: 0x%x enable: %d\n", CUR_REGION_NUM, state->mpu.regions[CUR_REGION_NUM].base_address, state->mpu.regions[CUR_REGION_NUM].end_address, state->mpu.regions[CUR_REGION_NUM].XN, enable);
                                    break;
                                }
                            case 4:
                                {
                                    // region attributes type and access permissions
                                    /*  31          13 12  11 10  8 7 6 5   3  2  1  0
                                     *  庚-----------房---房-房----房--房----房-房-房-庖
                                     *  |  Reserved  | XN |  |  AP |   | TEX | S| C| B|
                                     *  弩-----------拂---拂-拂----拂--拂----拂-拂-拂-彼
                                     *
                                     * XN  eXecute Never. Determines if a region of memory is executable:
                                     * AP  Access permission. Defines the data access permissions. For more information on AP bit values
                                     * TEX Type extension. Defines the type extension attribute a 
                                     * S   Share. Determines if the memory region is Shared or Non-shared:
                                     * C   bit a :
                                     * B   bit a :
                                     *
                                     *
                                     * AP bit | Privileged permissions | User permissions | Description
                                     * b000     No access                No access          All accesses generate a permission fault
                                     * b001     Read/write               No access          Privileged access only
                                     * b010     Read/write               Read-only          Writes in User mode generate permission faults
                                     * b011     Read/write               Read/write         Full access
                                     * b100     UNP                      UNP                Reserved
                                     * b101     Read-only                No access          Privileged read-only
                                     * b110     Read-only                Read-only          Privileged/User read-only
                                     * b111     UNP                      UNP                Reserved
                                     */

                                    uint32_t XN = (value >> 12) & 0x1;  // eXecute Never.
                                    uint32_t AP = (value >> 8) & 0x3;   // Access permission. Defines the data access permissions.
                                    uint32_t TEX = (value >> 3) & 0x3;  // Type extension. Defines the type extension attribute a 
                                    uint32_t S = (value >> 2) & 0x1;    // Share. Determines if the memory region is Shared or Non-shared:
                                    uint32_t C = (value >> 1) & 0x1;    // C bit
                                    uint32_t B = (value >> 0) & 0x1;    // B bit

                                    uint32_t shareable = 0;
                                    mpu_memory_type_t memory_type;

                                    // Type extension.
                                    switch (TEX)
                                    {
                                            uint32_t CB = value >> 0 & 0x3;     // C and B bit

                                        case 0x0:
                                            switch (CB)
                                            {
                                                case 0x0:
                                                    memory_type = MPU_MT_Strongly_ordered;
                                                    shareable = 1;
                                                    break;
                                                case 0x1:
                                                    memory_type = MPU_MT_Device;
                                                    shareable = 1;
                                                    break;
                                                case 0x2:
                                                    memory_type = MPU_MT_Normal;
                                                    shareable = S;
                                                    break;
                                                case 0x3:
                                                    memory_type = MPU_MT_Normal;
                                                    shareable = S;
                                                    break;
                                            }
                                            break;
                                        case 0x1:
                                            switch (CB)
                                            {
                                                case 0x0:
                                                    memory_type = MPU_MT_Normal;
                                                    shareable = S;
                                                    break;
                                                case 0x1:
                                                    // Reserved
                                                    break;
                                                case 0x2:
                                                    // Reserved
                                                    break;
                                                case 0x3:
                                                    memory_type = MPU_MT_Normal;
                                                    shareable = S;
                                                    break;
                                            }
                                            break;
                                        case 0x2:
                                            if (CB == 0)
                                            {
                                                memory_type = MPU_MT_Device;
                                                shareable = 0;
                                            }
                                            // When CB != 0, Reserved
                                            break;
                                        case 0x3:
                                            // Reserved
                                            break;
                                        default:
                                            {
                                                /*
                                                 * When TEX[2] == 1, the memory region is cacheable memory, and the rest of the encoding
                                                 * defines the Inner and Outer cache policies:
                                                 * TEX[1:0] Defines the Outer cache policy.
                                                 * C,B Defines the Inner cache policy.
                                                 *
                                                 */
                                                if (TEX & 0x4)
                                                {
                                                    // Defines the Outer cache policy.
                                                    switch (TEX & 0x3)
                                                    {
                                                        case 0x0:
                                                            // Non-cacheable
                                                            break;
                                                        case 0x1:
                                                            // Write-back, write-allocate
                                                            break;
                                                        case 0x2:
                                                            // Write-through, no write-allocate
                                                            break;
                                                        case 0x3:
                                                            // Write-back, no write-allocate
                                                            break;
                                                    }

                                                    // Defines the Inner cache policy.
                                                    switch (CB)
                                                    {
                                                        case 0x0:
                                                            // Non-cacheable
                                                            break;
                                                        case 0x1:
                                                            // Write-back, write-allocate
                                                            break;
                                                        case 0x2:
                                                            // Write-through, no write-allocate
                                                            break;
                                                        case 0x3:
                                                            // Write-back, no write-allocate
                                                            break;
                                                    }
                                                }
                                            }
                                    }

                                    state->mpu.regions[CUR_REGION_NUM].XN = XN;

                                    state->mpu.regions[CUR_REGION_NUM].memory_type = memory_type;
                                    state->mpu.regions[CUR_REGION_NUM].shareable = shareable;

                                    // Access permission.
                                    state->mpu.regions[CUR_REGION_NUM].access_permissions = AP;
                                    break;
                                }
                        }
                        break;
                    }
                case 2:
                    // Setup region number
                    if (value < MPU_REGION_SIZE)
                    {
                        state->mpu.curr_region_number = value;
                    } else
                    {
                        skyeye_log(Error_log, __FUNCTION__,
                                   "MPU set curr region number error, value is %d, MPU_REGION_SIZE: %d\n", value,
                                   MPU_REGION_SIZE);
                    }
                    break;
            }
            break;
        case MPU_CACHE_LOCKDOWN:
            /* FIXME: cache lock down */
            break;
        case MPU_TLB_LOCKDOWN:
            /* FIXME: tlb lock down */
            break;
        case MPU_PID:
            /* 0:24 should be zero. */
            state->mpu.process_id = value & 0xfe000000;
            break;
        case XSCALE_CP15_COPRO_ACCESS:
            /* FIXME: */
            break;
        default:
            printf("mpu_mcr write UNKNOWN - reg %d\n", CRn_reg);
            break;
    }

    return No_exp;
}

ARMword cortex_r4_mpu_mrc(ARMul_State * state, ARMword instr, ARMword * value)
{
    mmu_regnum_t CRn_reg = BITS(16, 19) & 0xf;
    mmu_regnum_t CRm_reg = instr & 0xf;
    int OPC_1 = BITS(21, 23) & 0x7;
    int OPC_2 = BITS(5, 7) & 0x7;
    ARMword data;

    switch (CRn_reg)
    {
        case MPU_CONTROL:
            data = state->mpu.SCTLR;
            break;
        case MPU_ID:
            switch (CRn_reg)
            {
                case 0x0:
                    data = state->cpu->cpu_val;
                    break;
                case 0x1:
                    data = 0x210030;
                    break;
                case 0x2:
                    /* TCM status */
                    data = 0x0;
                    break;
            }
            break;
        case MPU_PID:
            data = state->mmu.process_id;
            break;
        case MPU_CACHE_LOCKDOWN:
            break;
        case XSCALE_CP15_COPRO_ACCESS:
            /* FIXME: */
            break;
        default:
            printf("mpu_mrc read UNKNOWN - reg %d\n", CRn_reg);
            data = 0;
            break;
    }
    *value = data;

    //printf("[MRC]: 15, %d, r%d, cr%d cr%d, {%d}\n", OPC_1, BITS (12, 15), CRn_reg, CRm_reg, OPC_2);
    //printf("       pc: 0x%x value: 0x%x\n\n", state->Reg[15] - 8, *value);
    return data;
}

static int cortex_r4_mpu_v2p_dbct(ARMul_State * state, ARMword virt_addr, ARMword * phys_addr)
{
    *phys_addr = virt_addr;
    return (0);
}

/* cortex_r4 mmu_ops_t */
mmu_ops_t cortex_r4_mpu_ops = {
    cortex_r4_mpu_init,
    cortex_r4_mpu_exit,
    cortex_r4_mpu_read_byte,
    cortex_r4_mpu_write_byte,
    cortex_r4_mpu_read_halfword,
    cortex_r4_mpu_write_halfword,
    cortex_r4_mpu_read_word,
    cortex_r4_mpu_write_word,
    cortex_r4_mpu_load_instr,
    cortex_r4_mpu_mcr,
    cortex_r4_mpu_mrc,
    cortex_r4_mpu_v2p_dbct,
};

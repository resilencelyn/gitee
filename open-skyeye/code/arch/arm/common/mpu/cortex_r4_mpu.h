/*
 * cortex_r4_mpu.h - cortex_r4 Memory Protection Unit emulation.
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

#ifndef _CORTEX_R4_MPU_H_
#define _CORTEX_R4_MPU_H_

typedef struct cortex_r4_mpu_s
{
    tlb_s main_tlb;
    tlb_s lockdown_tlb;

    cache_s i_cache;
    cache_s d_cache;
    wb_s wb_t;
} cortex_r4_mpu_t;

typedef enum mpu_regnum_t
{
    MPU_ID = 0,
    MPU_CONTROL = 1,
    MPU_TRANSLATION_TABLE_BASE = 2,
    MPU_DOMAIN_ACCESS_CONTROL = 3,
    MPU_FAULT_ADDRESS = 6,
    MPU_CACHE_OPS = 7,
    MPU_TLB_OPS = 8,
    MPU_CACHE_LOCKDOWN = 9,
    MPU_TLB_LOCKDOWN = 10,
    MPU_PID = 13,
} mpu_regnum_t;

extern mmu_ops_t cortex_r4_mpu_ops;

#endif /*_CORTEX_R4_MMU_H_*/

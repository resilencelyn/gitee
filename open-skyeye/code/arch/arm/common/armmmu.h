/*
 * armmmu.h - Memory Management Unit emulation.
 * ARMulator extensions for the ARM7100 family.
 * Copyright (C) 1999  Ben Williamson
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

#ifndef _ARMMMU_H_
#define _ARMMMU_H_

#define WORD_SHT			2
#define WORD_SIZE			(1<<WORD_SHT)
/* The MMU is accessible with MCR and MRC operations to copro 15: */

#define MMU_COPRO			(15)

/* Register numbers in the MMU: */

typedef enum mmu_regnum_t
{
    MMU_ID = 0,
    MMU_CONTROL = 1,
    MMU_TRANSLATION_TABLE_BASE = 2,
    MMU_DOMAIN_ACCESS_CONTROL = 3,
    MMU_FAULT_STATUS = 5,
    MMU_FAULT_ADDRESS = 6,
    MMU_CACHE_OPS = 7,
    MMU_TLB_OPS = 8,
    MMU_CACHE_LOCKDOWN = 9,
    MMU_TLB_LOCKDOWN = 10,
    MMU_PID = 13,

    /*MMU_V4 */
    MMU_V4_CACHE_OPS = 7,
    MMU_V4_TLB_OPS = 8,

    /*MMU_V3 */
    MMU_V3_FLUSH_TLB = 5,
    MMU_V3_FLUSH_TLB_ENTRY = 6,
    MMU_V3_FLUSH_CACHE = 7,

    /*MMU Intel SA-1100 */
    MMU_SA_RB_OPS = 9,
    MMU_SA_DEBUG = 14,
    MMU_SA_CP15_R15 = 15,
    //chy 2003-08-24
    /*Intel xscale CP15 */
    XSCALE_CP15_CACHE_TYPE = 0,
    XSCALE_CP15_AUX_CONTROL = 1,
    XSCALE_CP15_COPRO_ACCESS = 15,
    VECTOR_BASE_ADDR = 12
} mmu_regnum_t;

/* Bits in the control register */

#define CONTROL_MMU			(1<<0)
#define CONTROL_ALIGN_FAULT		(1<<1)
#define CONTROL_CACHE			(1<<2)
#define CONTROL_DATA_CACHE		(1<<2)
#define CONTROL_WRITE_BUFFER		(1<<3)
#define CONTROL_BIG_ENDIAN		(1<<7)
#define CONTROL_SYSTEM			(1<<8)
#define CONTROL_ROM			(1<<9)
#define CONTROL_UNDEFINED               (1<<10)
#define CONTROL_BRANCH_PREDICT          (1<<11)
#define CONTROL_INSTRUCTION_CACHE       (1<<12)
#define CONTROL_VECTOR                  (1<<13)
#define CONTROL_RR                      (1<<14)
#define CONTROL_L4                      (1<<15)
#define CONTROL_XP                      (1<<23)
#define CONTROL_EE                      (1<<25)

/*Macro defines for MMU state */
#define MMU_CTL (state->mmu.control)
#define MMU_Enabled (state->mmu.control & CONTROL_MMU)
#define MMU_Disabled (!(MMU_Enabled))
#define MMU_Aligned (state->mmu.control & CONTROL_ALIGN_FAULT)

#define MMU_ICacheEnabled (MMU_CTL & CONTROL_INSTRUCTION_CACHE)
#define MMU_ICacheDisabled (!(MMU_ICacheDisabled))

#define MMU_DCacheEnabled (MMU_CTL & CONTROL_DATA_CACHE)
#define MMU_DCacheDisabled (!(MMU_DCacheEnabled))

#define MMU_CacheEnabled (MMU_CTL & CONTROL_CACHE)
#define MMU_CacheDisabled (!(MMU_CacheEnabled))

#define MMU_WBEnabled (MMU_CTL & CONTROL_WRITE_BUFFER)
#define MMU_WBDisabled (!(MMU_WBEnabled))

/*virt_addr exchange according to CP15.R13(process id virtul mapping) */
#define PID_VA_MAP_MASK	0xfe000000
#define mmu_pid_va_map(va) ({\
	ARMword ret; \
	if ((va) & PID_VA_MAP_MASK)\
		ret = (va); \
	else \
		ret = ((va) | (state->mmu.process_id & PID_VA_MAP_MASK));\
	ret;\
})

/* FS[3:0] in the fault status register: */

typedef enum fault_t
{
    NO_FAULT = 0x0,
    ALIGNMENT_FAULT = 0x1,

    SECTION_TRANSLATION_FAULT = 0x5,
    PAGE_TRANSLATION_FAULT = 0x7,
    SECTION_DOMAIN_FAULT = 0x9,
    PAGE_DOMAIN_FAULT = 0xB,
    SECTION_PERMISSION_FAULT = 0xD,
    SUBPAGE_PERMISSION_FAULT = 0xF,

    /* defined by skyeye */
    TLB_READ_MISS = 0x30,
    TLB_WRITE_MISS = 0x40,

    /* DFSR: FS[40] TTBCR_EAE == 0 */
    

} fault_t;

typedef struct mmu_ops_s
{
    /*initilization */
    int (*init) (ARMul_State * state);
    /*free on exit */
    void (*exit) (ARMul_State * state);
    /*read byte data */
      fault_t(*read_byte) (ARMul_State * state, ARMword va, ARMword * data);
    /*write byte data */
      fault_t(*write_byte) (ARMul_State * state, ARMword va, ARMword data);
    /*read halfword data */
      fault_t(*read_halfword) (ARMul_State * state, ARMword va, ARMword * data);
    /*write halfword data */
      fault_t(*write_halfword) (ARMul_State * state, ARMword va, ARMword data);
    /*read word data */
      fault_t(*read_word) (ARMul_State * state, ARMword va, ARMword * data);
    /*write word data */
      fault_t(*write_word) (ARMul_State * state, ARMword va, ARMword data);
    /*load instr */
      fault_t(*load_instr) (ARMul_State * state, ARMword va, ARMword * instr);
    /*mcr */
      ARMword(*mcr) (ARMul_State * state, ARMword instr, ARMword val);
    /*mrc */
      ARMword(*mrc) (ARMul_State * state, ARMword instr, ARMword * val);

    /*ywc 2005-04-16 convert virtual address to physics address */
    int (*v2p_dbct) (ARMul_State * state, ARMword virt_addr, ARMword * phys_addr);
} mmu_ops_t;

#include "mmu/tlb.h"
#include "mmu/rb.h"
#include "mmu/wb.h"
#include "mmu/cache.h"

/*special process mmu.h */
#include "mmu/sa_mmu.h"
#include "mmu/arm7100_mmu.h"
#include "mmu/arm920t_mmu.h"
#include "mmu/arm926ejs_mmu.h"
#include "mmu/arm1176jzf_s_mmu.h"
#include "mmu/cortex_a9_mmu.h"
#include "mmu/cortex_a8_mmu.h"
#include "mpu/cortex_r4_mpu.h"


typedef union 
{
    uint32_t all;
    struct 
    {
        uint32_t M:1;          /* bits[0] */
        uint32_t A:1;          /* bits[1] */
        uint32_t C:1;          /* bits[2] */
        uint32_t nTLSMD:1;     /* bits[3] */
        uint32_t LSMAOE:1;     /* bits[4] */
        uint32_t CP15BEN:1;    /* bits[5] */
        uint32_t UNK:1;        /* bits[6] */
        uint32_t ITD:1;        /* bits[7] */

        uint32_t SED:1;        /* bits[8] */
        uint32_t RES0_0:1;     /* bits[9] */
        uint32_t EnRCTX:1;     /* bits[10] */
        uint32_t RES1_0:1;     /* bits[11] */
        uint32_t I:1;          /* bits[12] */
        uint32_t V:1;          /* bits[13] */
        uint32_t RES0_1:2;     /* bits[15:14] */

        uint32_t nTWI:1;       /* bits[16] */
        uint32_t RES0_2:1;     /* bits[17] */
        uint32_t nTWE:1;       /* bits[18] */
        uint32_t WXN:1;        /* bits[19] */
        uint32_t UWXN:1;       /* bits[20] */
        uint32_t RES0_3:1;     /* bits[21] */
        uint32_t RES1_1:1;     /* bits[22] */
        uint32_t SPAN:1;       /* bits[23] */

        uint32_t RES0_4:1;     /* bits[24] */
        uint32_t EE:1;         /* bits[25] */
        uint32_t RES0_5:1;     /* bits[26:27] */
        uint32_t TRE:1;        /* bits[28] */
        uint32_t AFE:1;        /* bits[29] */
        uint32_t TE:1;         /* bits[30] */
        uint32_t DSSBSS:1;     /* bits[31] */

    };

}sctlr_reg_t;

typedef union
{
    uint32_t all;

    struct
    {
        uint32_t N:3;      /* bits[2:0] */
        uint32_t RES0:1;   /* bits[3] */

        uint32_t PD0:1;    /* bits[4] */
        uint32_t PD1:1;    /* bits[5] */
        
        uint32_t RES1:25;  /* bits[30:6] */
        uint32_t EAE:1;    /* bits[31] */

    }short_format;

    struct
    {
        uint32_t T0SZ:3;  /* bits[2:0] */

        uint32_t RES0:4;  /* bits[6:3] */

        uint32_t EPD0:1;  /* bits[7] */
        uint32_t IRGN0:2; /* bits[9:8] */

        uint32_t ORGN0:2; /* bits[11:10] */
        uint32_t SH0:2;   /* bits[13:12] */

        uint32_t RES1:2;  /* bits[15:14] */
        uint32_t T1SZ:3;  /* bits[18:16] */
        uint32_t RES2:3;  /* bits[21:19] */

        uint32_t A1:1;    /* bits[22] */
        uint32_t EPD1:1;   /* bits[23] */
        uint32_t IRGN1:2; /* bits[25:24] */
        uint32_t ORGN1:2; /* bits[27:26] */
        uint32_t SH1:2;   /* bits[29:28] */
        uint32_t IMPLEMENTATION_DEFINED:1; /* bits[30] */
        uint32_t EAE:1;   /* bits[31] */
    }long_format;
} ttbcr_reg_t;

typedef union 
{
    uint64_t all;

    struct 
    {
        uint32_t IRGN_1:1;  /* bits[0] */
        uint32_t S:1;       /* bits[1] */
        uint32_t IMP:1;     /* bits[2] */

        uint32_t RGN:2;     /* bits[4:3] */
        uint32_t NOS:1;     /* bits[5] */
        uint32_t IRGN_0:1;  /* bits[6] */

        uint32_t TTB0:25;   /* bits[31:7] */
        uint32_t RES0;      /* bits[63:32] */
    }short_format;
    struct 
    {
        uint64_t CnP:1;  /* bits[0] */
        uint64_t BADDR:47;  /* bits[47:1] */
        uint64_t ASID:8;  /* bits[55:48] */
        uint64_t RES0:8;  /* bits[63:56] */
    }long_format;
    
}ttbr0_reg_t;

typedef union
{
    uint64_t all;

    struct
    {
        uint64_t FLG:2;        /* bits[1:0] */
        uint64_t AttrIndx:3;   /* bits[4:2] */
        uint64_t NS:1;         /* bits[5] */
        uint64_t AP:2;         /* bits[7:6] */

        uint64_t SH:2;         /* bits[9:8] */
        uint64_t AF:1;         /* bits[10] */
        uint64_t nG:1;         /* bits[11] */

        uint64_t BADDR:40;     /* bits[51:12] */

        uint64_t Contiguous:1; /* bits[52] */
        uint64_t PXN:1;        /* bits[53] */
        uint64_t XN:1;         /* bits[54] */
        uint64_t IGNORED_0:4;  /* bits[58:55] */
        uint64_t PBHA:4;       /* bits[62:59] */
        uint64_t IGNORED_1:1;  /* bits[63] */
    }stage1_block;  /* long format block*/

    struct
    {
        uint64_t FLG:2;         /* bits[1:0] */
        uint64_t IGNORED_0:10;  /* bits[11:2] */

        uint64_t BADDR:40;      /* bits[51:12] */

        uint64_t IGNORED_1:5;   /* bits[58:52] */
        uint64_t PXNTable:1;    /* bits[59] */
        uint64_t XNTable:1;     /* bits[60] */
        uint64_t APTable:2;     /* bits[62:61] */
        uint64_t NSTable:1;     /* bits[63] */
    }stage1_table;  /* long format table*/

    /* data */
}arch32_long_pte_t;    /* arch32 page table entry descriptors */


typedef union
{
    uint32_t all;
    struct
    {
        uint64_t FLG:2;        /* bits[1:0] */
        uint64_t AttrIndx:3;   /* bits[4:2] */
        uint64_t NS:1;         /* bits[5] */
        uint64_t AP:2;         /* bits[7:6] */

        uint64_t SH:2;         /* bits[9:8] */
        uint64_t AF:1;         /* bits[10] */
        uint64_t nG:1;         /* bits[11] */

        uint64_t BADDR:40;     /* bits[51:12] */

        uint64_t Contiguous:1; /* bits[52] */
        uint64_t PXN:1;        /* bits[53] */
        uint64_t XN:1;         /* bits[54] */
        uint64_t IGNORED_0:4;  /* bits[58:55] */
        uint64_t PBHA:4;       /* bits[62:59] */
        uint64_t IGNORED_1:1;  /* bits[63] */
    }l1_table;  /* long format block*/

    struct
    {
        uint64_t FLG:2;         /* bits[1:0] */
        uint64_t IGNORED_0:10;  /* bits[11:2] */

        uint64_t BADDR:40;      /* bits[51:12] */

        uint64_t IGNORED_1:5;   /* bits[58:52] */
        uint64_t PXNTable:1;    /* bits[59] */
        uint64_t XNTable:1;     /* bits[60] */
        uint64_t APTable:2;     /* bits[62:61] */
        uint64_t NSTable:1;     /* bits[63] */
    }l2;  /* long format table*/

    /* data */
}arch32_short_pte_t;    /* arch32 page table entry descriptors */

typedef union 
{
    uint64_t all;
    struct 
    {
        uint32_t IRGN_0:1;  /* bits[0] */
        uint32_t S:1;       /* bits[1] */
        uint32_t IMP:1;     /* bits[2] */

        uint32_t RGN:2;     /* bits[4:3] */
        uint32_t NOS:1;     /* bits[5] */
        uint32_t IRGN_1:1;  /* bits[6] */

        uint32_t TTB1:25;   /* bits[31:7] */
        uint32_t RES0;      /* bits[63:32] */
    }short_format;
    struct 
    {
        uint64_t CnP:1;  /* bits[0] */
        uint64_t BADDR:47;  /* bits[47:1] */
        uint64_t ASID:8;  /* bits[55:48] */
        uint64_t RES0:8;  /* bits[63:56] */
    }long_format;
    
}ttbr1_reg_t;

typedef struct mmu_state_t
{
    ARMword control;
    ARMword translation_table_base;
    /* dyf 201-08-11 for arm1176 */
    ARMword auxiliary_control;
    ARMword coprocessor_access_control;
    ARMword translation_table_base0;
    ARMword translation_table_base1;
    ARMword translation_table_ctrl;
    /* arm1176 end */

    /* ARMv8-32 / ARMv7 start */
    sctlr_reg_t SCTLR;
    ttbcr_reg_t TTBCR;
    ttbr0_reg_t TTBR0;
    ttbr1_reg_t TTBR1;
    /* ARMv8-32 / ARMv7 end */

    ARMword domain_access_control;
    ARMword fault_status;
    ARMword fault_statusi;              /* prefetch fault status */
    ARMword fault_address;
    ARMword last_domain;
    ARMword process_id;
    ARMword context_id;
    ARMword thread_uro_id;
    ARMword thread_urw_id;
    ARMword cache_locked_down;
    ARMword tlb_locked_down;
    //chy 2003-08-24 for xscale
    ARMword cache_type;                 // 0
    ARMword aux_control;                // 1
    ARMword copro_access;               // 15

    ARMword cache_ind;
    ARMword cache_level;

    mmu_ops_t ops;
    union
    {
        sa_mmu_t sa_mmu;
        arm7100_mmu_t arm7100_mmu;
        arm920t_mmu_t arm920t_mmu;
        arm926ejs_mmu_t arm926ejs_mmu;
        cortex_a8_mmu_t cortex_a8_mmu;
        cortex_r4_mpu_t cortex_r4_mpu;
    } u;
} mmu_state_t;

int mmu_init(ARMul_State * state);
int mmu_reset(ARMul_State * state);
void mmu_exit(ARMul_State * state);

fault_t mmu_read_word(ARMul_State * state, ARMword virt_addr, ARMword * data);
fault_t mmu_write_word(ARMul_State * state, ARMword virt_addr, ARMword data);
fault_t mmu_load_instr(ARMul_State * state, ARMword virt_addr, ARMword * instr);

ARMword mmu_mrc(ARMul_State * state, ARMword instr, ARMword * value);
void mmu_mcr(ARMul_State * state, ARMword instr, ARMword value);

/*ywc 20050416 */
int mmu_v2p_dbct(ARMul_State * state, ARMword virt_addr, ARMword * phys_addr);

fault_t mmu_read_byte(ARMul_State * state, ARMword virt_addr, ARMword * data);
fault_t mmu_read_halfword(ARMul_State * state, ARMword virt_addr, ARMword * data);
fault_t mmu_read_word(ARMul_State * state, ARMword virt_addr, ARMword * data);
fault_t mmu_write_byte(ARMul_State * state, ARMword virt_addr, ARMword data);
fault_t mmu_write_halfword(ARMul_State * state, ARMword virt_addr, ARMword data);
fault_t mmu_write_word(ARMul_State * state, ARMword virt_addr, ARMword data);
#endif /* _ARMMMU_H_ */

/*
 *      PearPC
 *      ppc_cpu.h
 *
 *      Copyright (C) 2003, 2004 Sebastian Biallas (sb@biallas.net)
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

#ifndef __PPC_CPU_H__
#define __PPC_CPU_H__
#include <skyeye_thread.h>
#include <skyeye_obj.h>
#include <skyeye_mach.h>
#include <skyeye_exec.h>
#include "skyeye_types.h"
#include <skyeye_lock.h>
#include <skyeye_addr_space.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
//#include "types.h"

#include <skyeye_system.h>

/* tlb entry */
typedef struct ppc_tlb_entry_s
{
    uint32 v;
    uint32 ts;
    uint32 tid;
    uint32 epn;
    uint32 rpn;
    uint32 size;
    uint32 usxrw;
    uint32 wimge;
    uint32 x;
    uint32 u;
    uint32 iprot;
} ppc_tlb_entry_t;

#define L2_TLB0_SIZE 256
#define L2_TLB1_SIZE 16

typedef struct e500_mmu_s
{
    uint64 pid[3];
    uint64 mmucsr0;
    uint64 mmucfg;
    uint64 tlbcfg[2];
    uint64 mas[8];
    uint32 tlb0_nv;

    ppc_tlb_entry_t l2_tlb0_4k[L2_TLB0_SIZE];   /* unified, filled by tlbwe instruction */
    ppc_tlb_entry_t l2_tlb1_vsp[L2_TLB1_SIZE];  /* filled by tlbwe insructions */
} e500_mmu_t;

#define TLBSEL(e) ((e >> 28) & 0x1)
#define ESEL(e) ((e >> 16) & 0xf)
#define EPN(e) (e >> 12)
#define TLBSELD(e) ((e & 0x10000000) >> 28)

struct PPC_CPU_State_s;
struct e500_core_s;

typedef union Vector_t
{
    uint64 d[2];
    sint64 sd[2];
    float f[4];
    uint32 w[4];
    sint32 sw[4];
    uint16 h[8];
    sint16 sh[8];
    uint8 b[16];
    sint8 sb[16];
} Vector_t;

#define PAGE_SIZE         0x1000 / 4             // 4K size
#define PAGE_NUM          0x100000

#define HASH_PAGE(addr)   ((addr) >> 12)
#define HASH_OFFSET(addr) (((addr) & 0xFFF) / 4) // one instruction covers 4 bytes
#define SELFMODIFY_CNT    20

typedef void (*ppc_opc_function) (struct e500_core_s *core);

typedef struct _insn_cache
{
    uint32_t insn;
    uint32_t pc;
    uint32_t selfmodify_pc;
    uint32_t selfmodify_cnt;
    uint32_t selfmodify_addr[SELFMODIFY_CNT];
    ppc_opc_function opc_func;
} insn_cache_t;

typedef union _spefscr_reg_t
{
    uint32_t all;
    struct
    {
        uint8_t FRMC:2;
        uint8_t FOVFE:1;
        uint8_t FUNFE:1;
        uint8_t FDBZE:1;
        uint8_t FINVE:1;
        uint8_t FINXE:1;
        uint8_t res2:1;
        uint8_t FOVF:1;
        uint8_t FUNF:1;
        uint8_t FDBZ:1;
        uint8_t FINV:1;
        uint8_t FX:1;
        uint8_t FG:1;
        uint8_t OV:1;
        uint8_t SOV:1;
        uint8_t res1:1;
        uint8_t FOVFS:1;
        uint8_t FUNFS:1;
        uint8_t FDBZS:1;
        uint8_t FINVS:1;
        uint8_t FINXS:1;
        uint8_t res0:2;
        uint8_t FOVFH:1;
        uint8_t FUNFH:1;
        uint8_t FDBZH:1;
        uint8_t FINVH:1;
        uint8_t FXH:1;
        uint8_t FGH:1;
        uint8_t OVH:1;
        uint8_t SOVH:1;
    };
} spefscr_reg_t;

typedef enum
{
    E200_FAMILY = 0,
    E300_CORE = 1,
    E600_CORE = 2,
    UNDEFINE_CORE
}arch_type_t;

/**
 *  * description for e500 core, refer to E500CORERM manual
 *   */
typedef struct e500_core_s
{
    // * uisa
    uint32 gpr[32];
    uint32 gprh[32];
    uint64 fpr[32];
    uint32 cr;
    uint32 fpscr;
    uint32 xer;                         // spr 1
    uint32 xer_ca;                      // carry from xer
    uint32 lr;                          // spr 8
    uint32 ctr;                         // spr 9
    // *oea
    uint32 msr;
    uint32 pvr;                         // spr 287

    uint32 pc;
    uint32 npc;
    uint32 phys_pc;
    uint32 reserve;
    bool_t have_reservation;
    uint32 icount;

    // * memory managment
    uint32 ibatu[4];                    // spr 528, 530, 532, 534
    uint32 ibatl[4];                    // spr 529, 531, 533, 535
    uint32 ibat_bl17[4];                // for internal use

    uint32 dbatu[4];                    // spr 536, 538, 540, 542
    uint32 dbatl[4];                    // spr 537, 539, 541, 543
    uint32 dbat_bl17[4];                // for internal use

    uint32 sdr1;                        // spr 25 (page table base address)

    uint32 sr[16];

    // * exception handling

    uint32 dar;                         // spr 19
    uint32 dsisr;                       // spr 18
    uint32 sprg[10];                     // spr 272-275  604,605

    //uint32 sprg[4];
    uint32 srr[2];                      // spr 26-27

    //    * misc
    uint32 dec;                         // spr 22
    uint32 ear;                         // spr 282 .101
    uint32 pir;                         // spr 1032
    uint64 tb;                          // .75 spr 284(l)/285(u)

    uint32 hid[16];
    //* internal

    uint64 pdec;                        // more precise version of dec
    uint64 ptb;                         // more precise version of tb

    /* e600 specific register */
    uint32 e600_ibatu[4];               // spr 560, 562, 564, 566
    uint32 e600_ibatl[4];               // spr 561, 563, 565, 567
    uint32 e600_dbatu[4];               // spr 568, 570, 572, 574
    uint32 e600_dbatl[4];               // spr 569, 571, 573, 575
    uint32 e600_pte[2];                 // spr 981, spr 982
    uint32 e600_tlbmiss;                // spr 980
    uint32 e600_ictc;                   // spr 1019
    uint32 e600_hid[2];                 // spr 1008, 1009
    uint32 e600_upmc[6];                // spr 937, 938, 941, 942, 929, 930
    uint32 e600_usiar;                  // spr 939
    uint32 e600_ummcr[3];               // spr 936, 940, 928
    uint32 e600_sprg[4];                // spr 276-279

    uint32 e600_ldstcr;                 // spr 1016
    uint32 e600_ldstdb;                 // spr 1012
    uint32 e600_msscr0;                 // spr 1014
    uint32 e600_msssr0;                 // spr 1015
    uint32 e600_ictrl;                  // spr 1011
    uint32 e600_l2cr;                   // spr 1017
    uint32 e600_mmcr[3];                // spr 952,956,944
    uint32 e600_bamr;                   // spr 951

    /* e500 specific register */
    uint32 l1csr[4];                    /* L1 cache constrol and status */
    uint32 l1cfg[2];                    /* L1 cache configure register */
    uint32 csrr[2];                     /* Critical save/restore register */
    uint32 mcsrr[2];                    /* Machine check save/restore register */
    uint32 esr;                         /* Exception syndrome register */
    uint32 mcsr;                        /* Machine check syndrome register */
    uint32 dear;                        /* Data exception address register */
    uint32 dbcr[4];                     /* Debug control register */
    uint32 dbsr;                        /* Debug status register */
    uint32 tcr;                         /* Timer control register */
    uint32 tsr;                         /* Timer status register */
    uint32 dac[2];                      /* Data address compare */
    uint32 ivpr;                        /* Interrupt vector */
    uint32 ivor[43];                    /* 0 = Critical input */
    uint32 iac[4];                      /* Instruction address compare */

    uint32 tbl;
    uint32 tbu;
    uint32 syscall_number;

    spefscr_reg_t spefscr;
    uint32 usprg0;
    uint32 bbear;//spr513,e500 only
    uint32 bbtar;//spr514,e500 only
    uint32 atbl;//spr526,e500 only
    uint32 atbu;//spr527,e500 only
    uint32 mcar;
    uint32 iabr0;
    uint32 iabr1;
    uint32 bucsr;
    uint32 Dabr;
    uint32 dvc[2];
    uint32 dbsrwr;

    uint32 lper;
    uint32 lperu;
    uint32 lpidr;
    uint32 lratcfg;
    uint32 lratps;
    uint32 tlb_ps[4];

    uint32 mas5_mas6;
    uint32 mas8_mas1;
    uint32 mas7_mas3;
    uint32 mas0_mas1;

    uint32 gsprg[4];
    uint32 gsrr[2];
    uint32 gepr;
    uint32 gdear;
    uint32 gpir;
    uint32 gesr;
    uint32 givpr;
    uint32 gintlevel;
    uint32 geridr;

    uint32 givor[36];

    uint32 tir;
    uint32 tensr;
    uint32 tens;
    uint32 tenc;

    uint32 npidr;
    uint32 l2cfg0;
    uint32 mcaru;
    uint32 nspd;
    uint32 nspc;
    uint32 pwrmgtcr0;
    uint32 l1finv1;
    uint32 eplc;
    uint32 epsc;
    uint32 epidr;
    uint32 eptcfg;
    uint32 epcr;
    uint32 edbrac0;

    uint32 ppr32;
    uint32 cdcsr0;
    uint32 dbrr0;
    uint32 epr;

    uint32 l2errinten;
    uint32 l2errattr;
    uint32 l2erraddr;
    uint32 l2erreaddr;
    uint32 l2errctl;
    uint32 l2errdis;

    uint32 intlevel;
    uint32 mcarua;
    uint32 dsrr[2];
    uint32 ddam;
    uint32 dvcu[2];
    uint32 devent;
    uint32 sccsrbar;

    uint32 e600_pmc[6];                 // spr 953,954,957,958,945,946
    uint32 e600_l2errinj[2];
    uint32 e600_l2errinjctl;
    uint32 e600_l2captdata[2];
    uint32 e600_l2captecc;
    uint32 e600_l2errdet;

    /*  e300 specific register */
    uint32 e300_ibcr;
    uint32 e300_dbcr;
    uint32 e300_mbar;
    uint32 e300_dabr2;
    uint32 e300_imiss;
    uint32 e300_icmp;
    uint32 e300_dmiss;
    uint32 e300_dcmp;
    uint32 e300_hash1;
    uint32 e300_hash2;
    uint32 e300_rpa;

    uint32 ipr;
    uint32 iack;
    uint32 ipi_flag;
    uint32 current_opc;

    uint32 svr;                         // spr 286
    uint32 siar;                        // spr 955 Sampled Instruction Address Register
    uint32 trace_exception;
    uint32_t mode;
    uint32_t old_mode;

    // for altivec
    uint32 vscr;
    uint32 vrsave;                      // spr 256 e600 only
    Vector_t vr[36];                    // <--- this MUST be 16-byte alligned
    uint32 vtemp;

    e500_mmu_t mmu;
    uint32 effective_code_page;
    // for generic cpu core
    uint8 *physical_code_page;

    /* @ asyn_exc_flag is used for ppc_dyncom.
     *  * To record the exceptions or interrupts which are asynchronous.
     *   * They are detected in per_cpu_step.*/
#define DYNCOM_ASYN_EXC_DEC       ( 1 << 0 )
#define DYNCOM_ASYN_EXC_EXT_INT   ( 1 << 1 )

    uint32 asyn_exc_flag;
    uint32 interrupt_dec_flag;
    uint32 interrupt_ext_int_flag;

    int (*effective_to_physical) (struct e500_core_s * core, uint32 addr, int flags, uint32 * result);

      bool_t(*ppc_exception) (struct e500_core_s * core, uint32 type, uint32 flags, uint32 a);

      uint32(*get_ccsr_base) (uint32 ccsr_reg);

    uint32 ccsr_size;

    void (*dec_io_do_cycle) (struct e500_core_s * core);

    RWLOCK_T dec_lock;
    RWLOCK_T ext_int_lock;
    conf_object_t *dyncom_cpu;

    conf_object_t *obj;
    struct PPC_CPU_State_s *cpu;
    memory_space_intf *core_memory_iface;

    conf_object_t *memory_space;
    memory_space_intf *memory_space_iface;

    conf_object_t *memory_ccsr;
    memory_space_intf *memory_ccsr_iface;

    uint32_t freq_hz;
    uint64_t stop_step;
    uint64_t insn_num;
    generic_address_t load_addr;
    uint32_t load_mask;
    uint32_t code_start_addr;
    uint32_t code_end_addr;

    uint32_t debug_mode;

    breakpoint_mgt_t bp_manager;

    bool_t exception_pending;
    bool_t dec_exception;
    bool_t ext_exception;
    bool_t stop_exception;
    bool_t singlestep_ignore;

    uint32 pagetable_base;
    int pagetable_hashmask;

    int exception_status;
    arch_type_t arch_type;

    pthread_rwlock_t check_stepping_rwlock;
    insn_cache_t **insn_cache;
    uint32_t index_array[PAGE_NUM];
    uint32_t index_count;
    uint32_t tmp_cnt;
    int64_t acc;
    uint32_t isWake;
    uint32_t speSupport;
    char exception_name[100];

    uint32 msrp;                        // spr 311
    uint32_t VLE_Enable;
    uint8_t InsnBitsNum;
    uint8_t CurrInsnPrefix4bit;

    uint32_t hardware_vector_offset;
    uint32_t hardware_vector_mode;
} e500_core_t;

#define E500
#define IPI0 (1 >> 0)

#define PPC_MHz(v) ((v)*1000*1000)

#define TB_TO_PTB_FACTOR	10

#define PPC_MODEL "ppc_model"
#define PPC_CPU_MODEL "ppc_cpu"
#define PPC_CLOCK_FREQUENCY PPC_MHz(10)
#define PPC_BUS_FREQUENCY PPC_MHz(10)
#define PPC_TIMEBASE_FREQUENCY (PPC_CLOCK_FREQUENCY / TB_TO_PTB_FACTOR)

typedef struct PPC_CPU_State_s
{
    e500_core_t *core;
    uint32_t bptr;
    uint32_t eebpcr;
    uint32_t ccsr;
    uint32_t core_num;
    /* The core id that boot from
     */
    uint32_t boot_core_id;
} PPC_CPU_State;

/*
 * cr: .67
 * 0- 3 cr0
 * 4- 7 cr1
 * 8-11 cr2
 * 12-15 cr3
 * 16-19 cr4
 * 20-23 cr5
 * 24-27 cr6
 * 28-31 cr7
 */

#define CR_CR0(v) ((v)>>28)
#define CR_CR1(v) (((v)>>24)&0xf)
#define CR_CRx(v, x) (((v)>>(4*(7-(x))))&0xf)

/*
 * cr0 bits: .68
 * lt
 * gt
 * eq
 * so
 */
#define CR_CR0_LT (1<<31)
#define CR_CR0_GT (1<<30)
#define CR_CR0_EQ (1<<29)
//#define CR_CR0_EQ (1 << 2)
#define CR_CR0_SO (1<<28)

/*
 * cr1 bits: .68
 * 4 Floating-point exception (FX)
 * 5 Floating-point enabled exception (FEX)
 * 6 Floating-point invalid exception (VX)
 * 7 Floating-point overflow exception (OX)
 */
#define CR_CR1_FX (1<<27)
#define CR_CR1_FEX (1<<26)
#define CR_CR1_VX (1<<25)
#define CR_CR1_OX (1<<24)

/*
 * FPSCR bits: .70
 *
 */
#define FPSCR_FX (1<<31)
#define FPSCR_FEX (1<<30)
#define FPSCR_VX (1<<29)
#define FPSCR_OX (1<<28)
#define FPSCR_UX (1<<27)
#define FPSCR_ZX (1<<26)
#define FPSCR_XX (1<<25)
#define FPSCR_VXSNAN (1<<24)
#define FPSCR_VXISI (1<<23)
#define FPSCR_VXIDI (1<<22)
#define FPSCR_VXZDZ (1<<21)
#define FPSCR_VXIMZ (1<<20)
#define FPSCR_VXVC (1<<19)
#define FPSCR_FR (1<<18)
#define FPSCR_FI (1<<17)

#define FPSCR_FPRF(v) (((v)>>12)&0x1f)

#define FPSCR_res0 (1<<11)
#define FPSCR_VXSOFT (1<<10)
#define FPSCR_VXSQRT (1<<9)
#define FPSCR_VXCVI (1<<8)
#define FPSCR_VXVE (1<<7)
#define FPSCR_VXOE (1<<6)
#define FPSCR_VXUE (1<<5)
#define FPSCR_VXZE (1<<4)
#define FPSCR_VXXE (1<<3)
#define FPSCR_VXNI (1<<2)
#define FPSCR_RN(v) ((v)&3)

#define FPSCR_RN_NEAR 0
#define FPSCR_RN_ZERO 1
#define FPSCR_RN_PINF 2
#define FPSCR_RN_MINF 3

/*
 * VSCR bits:
 * sat = summary saturation
 * nj = non-java floating-point mode
 */
#define VSCR_SAT 1
#define VSCR_NJ (1<<16)

/*
 * xer bits:
 * 0 so
 * 1 ov
 * 2 carry
 * 3-24 res
 * 25-31 number of bytes for lswx/stswx
 */
#define XER_SO (1<<31)
#define XER_OV (1<<30)
#define XER_CA (1<<29)
#define XER_n(v) ((v)&0x7f)

/*
 * msr: .83
 * 0-12 res
 * 13   POW     power management enabled
 * 14   res
 * 15   ILE     exception little-endian mode
 * 16   EE      enable external interrupt
 * 17   PR      privilege level (0=sv)
 * 18   FP      floating point avail
 * 19   ME      maschine check exception enable
 * 20   FE0     floation point exception mode 0
 * 21   SE   single step enable
 * 22   BE      branch trace enable
 * 23   FE1     floation point exception mode 1
 * 24   res
 * 25   IP      exception prefix
 * 26   IR   intruction address translation
 * 27   DR   data address translation
 * 28-29res
 * 30   RI      recoverable exception
 * 31   LE   little endian mode
 *
 */
#define MSR_SF		(1<<31)
#define MSR_UNKNOWN	(1<<30)
#define MSR_GS      (1<<28)
#define MSR_UNKNOWN2	(1<<27)
#define MSR_VEC		(1<<25)
#define MSR_SPE		(1<<25)
#define MSR_WE		(1<<18)
#define MSR_POW		(1<<18)
#define MSR_TGPR	(1<<15)         // 603(e)
#define MSR_CE		(1<<17)
#define MSR_ILE		(1<<16)
#define MSR_EE		(1<<15)
#define MSR_PR		(1<<14)
#define MSR_FP		(1<<13)
#define MSR_ME		(1<<12)
#define MSR_FE0		(1<<11)
#define MSR_UBLE	(1<<10)
#define MSR_SE		(1<<10)
#define MSR_BE		(1<<9)
#define MSR_DE		(1<<9)
#define MSR_FE1		(1<<8)
#define MSR_IP		(1<<6)
#define MSR_IS		(1<<5)
#define MSR_IR		(1<<5)
#define MSR_DS		(1<<4)
#define MSR_DR		(1<<4)
#define MSR_PM		(1<<2)
#define MSR_RI		(1<<1)
#define MSR_LE		(1)

//#define PPC_CPU_UNSUPPORTED_MSR_BITS (/*MSR_POW|*/MSR_ILE|MSR_BE|MSR_IP|MSR_LE)
#define PPC_CPU_UNSUPPORTED_MSR_BITS (~(MSR_POW | MSR_UNKNOWN | MSR_UNKNOWN2 | MSR_VEC | MSR_EE | MSR_PR | MSR_FP | MSR_ME | MSR_FE0 | MSR_SE | MSR_FE1 | MSR_IR | MSR_DR | MSR_RI))

#define MSR_RFI_SAVE_MASK (0x200ff73)

#define ESR_SPV     (1 << 7)
/*
 * BAT Register: .88
 * upper:
 * 0-14  BEPI Block effective page index.
 * 15-18 res
 * 19-29 BL   Block length.
 * 30    Vs   Supervisor mode valid bit.
 * 31    Vp   User mode valid bit.
 * lower:
 * 0-14  BRPN This field is used in conjunction with the BL field to generate highorder bits of the physical address of the block.
 * 15-24 res
 * 25-28 WIMG Memory/cache access mode bits
 * 29    res
 * 30-31 PP   Protection bits for block.
 *
 * BAT Area
 * Length               BL Encoding
 * 128 Kbytes   000 0000 0000
 * 256 Kbytes   000 0000 0001
 * 512 Kbytes   000 0000 0011
 * 1 Mbyte              000 0000 0111
 * 2 Mbytes     000 0000 1111
 * 4 Mbytes     000 0001 1111
 * 8 Mbytes     000 0011 1111
 * 16 Mbytes    000 0111 1111
 * 32 Mbytes    000 1111 1111
 * 64 Mbytes    001 1111 1111
 * 128 Mbytes   011 1111 1111
 * 256 Mbytes   111 1111 1111
 */

#define BATU_BEPI(v) ((v)&0xfffe0000)
#define BATU_BL(v)   (((v)&0x1ffc)>>2)
#define BATU_Vs      (1<<1)
#define BATU_Vp      (1)
#define BATL_BRPN(v) ((v)&0xfffe0000)

#define BAT_EA_OFFSET(v) ((v)&0x1ffff)
#define BAT_EA_11(v)     ((v)&0x0ffe0000)
#define BAT_EA_4(v)      ((v)&0xf0000000)

/*
 * sdr1: .91
 * 0-15 The high-order 16 bits of the 32-bit physical address of the page table
 * 16-22 res
 * 23-31 Mask for page table address
 */

#define SDR1_HTABORG(v) (((v)>>16)&0xffff)
#define SDR1_HTABMASK(v) ((v)&0x1ff)
#define SDR1_PAGETABLE_BASE(v) ((v)&0xffff)

/*
 * sr: .94
 * 0    T=0:
 * 1    Ks   sv prot
 * 2    Kp   user prot
 * 3    N    No execute
 * 4-7  res
 * 8-31 VSID Virtual Segment ID
 *
 * 0     T=1:
 * 1     Ks
 * 2     Kp
 * 3-11  BUID       Bus Unit ID
 * 12-31 CNTRL_SPEC
 */
#define SR_T  (1<<31)
#define SR_Ks (1<<30)
#define SR_Kp (1<<29)
#define SR_N  (1<<28)
#define SR_VSID(v)       ((v)&0xffffff)
#define SR_BUID(v)       (((v)>>20)&0x1ff)
#define SR_CNTRL_SPEC(v) ((v)&0xfffff)

#define EA_SR(v)         (((v)>>28)&0xf)
#define EA_PageIndex(v)  (((v)>>12)&0xffff)
#define EA_Offset(v)	((v)&0xfff)
#define EA_API(v)		(((v)>>22)&0x3f)

#define PA_RPN(v)        (((v)>>12)&0xfffff)
#define PA_Offset(v)     ((v)&0xfff)

/*
 * PTE: .364
 * 0     V
 * 1-24  VSID
 * 25    H
 * 26-31 API
 */

#define PTE1_V       (1<<31)
#define PTE1_VSID(v) (((v)>>7)&0xffffff)
#define PTE1_H       (1<<6)
#define PTE1_API(v)  ((v)&0x3f)

#define PTE2_RPN(v)  ((v)&0xfffff000)
#define PTE2_R       (1<<8)
#define PTE2_C       (1<<7)
#define PTE2_WIMG(v) (((v)>>3)&0xf)
#define PTE2_PP(v)   ((v)&3)

#define PPC_L1_CACHE_LINE_SIZE  32
#define PPC_LG_L1_CACHE_LINE_SIZE   5
#define PPC_MAX_L1_COPY_PREFETCH    4

/*
 *      special registers
 */
#define HID0	1008                    /* Checkstop and misc enables */
#define HID1	1009                    /* Clock configuration */
#define iabr	1010                    /* Instruction address breakpoint register */
#define ictrl	1011                    /* Instruction Cache Control */
#define ldstdb	1012                    /* Load/Store Debug */
#define dabr	1013                    /* Data address breakpoint register */
#define msscr0	1014                    /* Memory subsystem control */
#define msscr1	1015                    /* Memory subsystem debug */
#define msssr0	1015                    /* Memory Subsystem Status */
#define ldstcr	1016                    /* Load/Store Status/Control */
#define l2cr2	1016                    /* L2 Cache control 2 */
#define l2cr	1017                    /* L2 Cache control */
#define l3cr	1018                    /* L3 Cache control */
#define ictc	1019                    /* I-cache throttling control */
#define thrm1	1020                    /* Thermal management 1 */
#define thrm2	1021                    /* Thermal management 2 */
#define thrm3	1022                    /* Thermal management 3 */
//#define pir   1023    /* Processor ID Register */

//;     hid0 bits
#define HID0_emcp	0
#define HID0_emcpm	0x80000000
#define HID0_dbp	1
#define HID0_dbpm	0x40000000
#define HID0_eba	2
#define HID0_ebam	0x20000000
#define HID0_ebd	3
#define HID0_ebdm	0x10000000
#define HID0_sbclk	4
#define HID0_sbclkm	0x08000000
#define HID0_eclk	6
#define HID0_eclkm	0x02000000
#define HID0_par	7
#define HID0_parm	0x01000000
#define HID0_sten	7
#define HID0_stenm	0x01000000
#define HID0_doze	8
#define HID0_dozem	0x00800000
#define HID0_nap	9
#define HID0_napm	0x00400000
#define HID0_sleep	10
#define HID0_sleepm	0x00200000
#define HID0_dpm	11
#define HID0_dpmm	0x00100000
#define HID0_riseg	12
#define HID0_risegm	0x00080000
#define HID0_eiec	13
#define HID0_eiecm	0x00040000
#define HID0_mum	14
#define HID0_mumm	0x00020000
#define HID0_nhr	15
#define HID0_nhrm	0x00010000
#define HID0_ice	16
#define HID0_icem	0x00008000
#define HID0_dce	17
#define HID0_dcem	0x00004000
#define HID0_ilock	18
#define HID0_ilockm	0x00002000
#define HID0_dlock	19
#define HID0_dlockm	0x00001000
#define HID0_icfi	20
#define HID0_icfim	0x00000800
#define HID0_dcfi	21
#define HID0_dcfim	0x00000400
#define HID0_spd	22
#define HID0_spdm	0x00000200
#define HID0_sge	24
#define HID0_sgem	0x00000080
#define HID0_dcfa	25
#define HID0_dcfam	0x00000040
#define HID0_btic	26
#define HID0_bticm	0x00000020
#define HID0_lrstk	27
#define HID0_lrstkm	0x00000010
#define HID0_abe	28
#define HID0_abem	0x00000008
#define HID0_fold	28
#define HID0_foldm	0x00000008
#define HID0_bht	29
#define HID0_bhtm	0x00000004
#define HID0_nopdst	30
#define HID0_nopdstm	0x00000002
#define HID0_nopti	31
#define HID0_noptim	0x00000001

#define DEC_TIMER_ENABLE    0x04000000

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief Get the instance of running core
*
* @return the core instance
*/
#if 0
    static inline e500_core_t *get_current_core()
    {
        skyeye_system_t *system = system_get();
          return (e500_core_t *) (system->socs[soc_id].cpus[core_id].cpu->obj);
    }
#endif

#define CURRENT_CORE get_current_core()

#if 0                                   /* deprecated */
    static inline PPC_CPU_State *get_current_cpu()
    {

        e500_core_t *core = get_current_core();
        PPC_CPU_State *cpu = (PPC_CPU_State *) core->cpu;

        return cpu;

    }
#endif

/**
* @brief Get the core instance boot from
*
* @return
*/
#if  0                                  /* deprecated */
    static inline e500_core_t *get_boot_core()
    {
        PPC_CPU_State *cpu = get_current_cpu();

        return &cpu->core[cpu->boot_core_id];
    }
#endif

    void ppc_cpu_atomic_raise_ext_exception();
    void ppc_cpu_atomic_cancel_ext_exception();
    extern uint32 gBreakpoint;
    extern uint32 gBreakpoint2;
    void ppc_set_singlestep_v(bool_t v, const char *file, int line, const char *format, ...);
    void ppc_set_singlestep_nonverbose(bool_t v);
    void ppc_machine_check_exception();
    uint32 ppc_cpu_get_pvr(e500_core_t * core);

    generic_address_t ppc_get_pc(conf_object_t * conf_obj);
    void ppc_set_pc(conf_object_t * conf_obj, generic_address_t pc);

    uint64 ppc_get_step(conf_object_t * conf_obj);
    endian_t ppc_get_endian(conf_object_t * obj);
    align_t ppc_get_alignment(conf_object_t * obj);
    char *ppc_get_architecture(conf_object_t * obj);

    uint32 ppc_get_regval_by_id(conf_object_t * conf_obj, int id);
    exception_t ppc_set_regval_by_id(conf_object_t * conf_obj, uint32 value, int id);
    char *ppc_get_regname_by_id(conf_object_t * conf_obj, int id);
    uint32 ppc_get_regnum(conf_object_t * conf_obj);
    exception_t ppc_set_regval_by_id(conf_object_t * conf_obj, uint32 value, int id);
    uint32 ppc_get_regid_by_name(conf_object_t * conf_obj, char *name);

    uint64 ppc_get_reg64val_by_id(conf_object_t * conf_obj, int id);
    exception_t ppc_set_reg64val_by_id(conf_object_t * conf_obj, uint64 value, int id);

    conf_object_t *new_ppc_core(char *obj_name);
    void ppc_register_attribute(conf_class_t * clss);
    void config_ppc_core(conf_object_t * obj);
    void free_ppc_core(conf_object_t * dev);
    exception_t ppc_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count);
    exception_t ppc_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count);
    get_page_t *ppc_get_page(conf_object_t * obj, generic_address_t addr);
    exception_t ppc_memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index);
    exception_t ppc_memory_space_get(conf_object_t * obj, conf_object_t ** obj2, char **port, int index);

    uint64_t ppc_run(conf_object_t * running_core);
    void ppc_step_once(conf_object_t * running_core);
    void ppc_stepi(conf_object_t * obj, uint64_t steps);
    void ppc_power(conf_object_t * obj, uint32_t wake_flag);
    void ppc_set_textsection_info(conf_object_t * obj, generic_address_t code_start_addr,
                                  generic_address_t code_end_addr);

    int ppc_insert_bp(conf_object_t * conf_obj, generic_address_t addr);
    int ppc_delete_bp(conf_object_t * conf_obj, generic_address_t addr);
    uint32_t ppc_get_bp_numbers(conf_object_t * conf_obj);
    uint32_t ppc_get_bp_addr_by_id(conf_object_t * conf_obj, int id);
    int ppc_get_trigger(conf_object_t * conf_obj);
    bool_t ppc_check_bp_trigger(conf_object_t * conf_obj);
    int ppc_clear_bp_trigger(conf_object_t * conf_obj);
    uint32_t ppc_get_address_width(conf_object_t * obj);
    char *ppc_disassemble(conf_object_t * obj, generic_address_t addr);
    void check_trace_exception(e500_core_t * core);
    void ppc_set_memory_watch_status(conf_object_t * obj, uint32_t addr, int status);
    int ppc_get_cpu_exception_num(conf_object_t * obj);
    void ppc_set_cpu_exception_by_id(conf_object_t * obj, int exception_id);
    char *ppc_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id);

    void clear_msr_special_bits(e500_core_t *current_core);

#ifdef __cplusplus
}
#endif

#endif

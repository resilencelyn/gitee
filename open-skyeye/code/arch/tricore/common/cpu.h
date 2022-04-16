/*************************************************************************
	> TriCore simulation for SkyEye: main CPU struct
	> File Name    : cpu.h
	> Author       : jiachao
	> Mail         : jiachao@digiproto.com
	> Created Time : 2021/10/26 09:42:09
************************************************************************/

#ifndef _CPU_H
#define _CPU_H

#include <skyeye_types.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_attribute.h>
#include <skyeye_iface.h>
#include <skyeye_core_intf.h>
#include <skyeye_disassemble.h>
#include <skyeye_int_register.h>
#include <skyeye_log.h>
#include <memory_space.h>
#include "breakpoint.h"
#include <skyeye_mm.h>
#include <sim_control.h>
#include "softfloat_type.h"

enum
{
    /* 1 bit to define user level / supervisor access */
    ACCESS_USER  = 0x00,
    ACCESS_SUPER = 0x01,
    /* 1 bit to indicate direction */
    ACCESS_STORE = 0x02,
    /* Type of instruction that generated the access */
    ACCESS_CODE  = 0x10, /* Code fetch access                */
    ACCESS_INT   = 0x20, /* Integer load/store access        */
    ACCESS_FLOAT = 0x30, /* floating point load/store access */
};

#define MASK_PCXI_PCPN 0xff000000
#define MASK_PCXI_PIE_1_3  0x00800000
#define MASK_PCXI_PIE_1_6  0x00200000
#define MASK_PCXI_UL   0x00400000
#define MASK_PCXI_PCXS 0x000f0000
#define MASK_PCXI_PCXO 0x0000ffff

#define MASK_PSW_USB 0xff000000
#define MASK_USB_C   0x80000000
#define MASK_USB_V   0x40000000
#define MASK_USB_SV  0x20000000
#define MASK_USB_AV  0x10000000
#define MASK_USB_SAV 0x08000000
#define MASK_PSW_PRS 0x00003000
#define MASK_PSW_IO  0x00000c00
#define MASK_PSW_IS  0x00000200
#define MASK_PSW_GW  0x00000100
#define MASK_PSW_CDE 0x00000080
#define MASK_PSW_CDC 0x0000007f
#define MASK_PSW_FPU_RM 0x3000000

#define MASK_SYSCON_PRO_TEN 0x2
#define MASK_SYSCON_FCD_SF  0x1

#define MASK_CPUID_MOD     0xffff0000
#define MASK_CPUID_MOD_32B 0x0000ff00
#define MASK_CPUID_REV     0x000000ff

#define MASK_ICR_PIPN 0x00ff0000
#define MASK_ICR_IE_1_3   0x00000100
#define MASK_ICR_IE_1_6   0x00008000
#define MASK_ICR_CCPN 0x000000ff

#define MASK_FCX_FCXS 0x000f0000
#define MASK_FCX_FCXO 0x0000ffff

#define MASK_LCX_LCXS 0x000f0000
#define MASK_LCX_LCX0 0x0000ffff

#define MASK_DBGSR_DE 0x1
#define MASK_DBGSR_HALT 0x6
#define MASK_DBGSR_SUSP 0x10
#define MASK_DBGSR_PREVSUSP 0x20
#define MASK_DBGSR_PEVT 0x40
#define MASK_DBGSR_EVTSRC 0x1f00

#define TRICORE_HFLAG_KUU     0x3
#define TRICORE_HFLAG_UM0     0x00002 /* user mode-0 flag          */
#define TRICORE_HFLAG_UM1     0x00001 /* user mode-1 flag          */
#define TRICORE_HFLAG_SM      0x00000 /* kernel mode flag          */

/**
 * TriCoreCPU:
 * @env: #TriCoreState
 *
 * A TriCore CPU.
 */
struct TriCoreState
{
    conf_object_t *obj;
    /* GPR Register */
    uint32_t gpr_a[16];
    uint32_t gpr_d[16];
    /* CSFR Register */
    uint32_t PCXI;
    /* Frequently accessed PSW_USB bits are stored separately for efficiency.
       This contains all the other bits.  Use psw_{read,write} to access
       the whole PSW.  */
    uint32_t PSW;
    uint32_t PC;
    uint32_t SYSCON;
    uint32_t CPU_ID;
    uint32_t CORE_ID;
    /* PSW flag cache for faster execution
    */
    uint32_t PSW_USB_C;
    uint32_t PSW_USB_V;   /* Only if bit 31 set, then flag is set  */
    uint32_t PSW_USB_SV;  /* Only if bit 31 set, then flag is set  */
    uint32_t PSW_USB_AV;  /* Only if bit 31 set, then flag is set. */
    uint32_t PSW_USB_SAV; /* Only if bit 31 set, then flag is set. */
    uint32_t BIV;
    uint32_t BTV;
    uint32_t ISP;
    uint32_t ICR;
    uint32_t FCX;
    uint32_t LCX;
    uint32_t COMPAT;

    /* Mem Protection Register */
    uint32_t DPR0_0L;
    uint32_t DPR0_0U;
    uint32_t DPR0_1L;
    uint32_t DPR0_1U;
    uint32_t DPR0_2L;
    uint32_t DPR0_2U;
    uint32_t DPR0_3L;
    uint32_t DPR0_3U;

    uint32_t DPR1_0L;
    uint32_t DPR1_0U;
    uint32_t DPR1_1L;
    uint32_t DPR1_1U;
    uint32_t DPR1_2L;
    uint32_t DPR1_2U;
    uint32_t DPR1_3L;
    uint32_t DPR1_3U;

    uint32_t DPR2_0L;
    uint32_t DPR2_0U;
    uint32_t DPR2_1L;
    uint32_t DPR2_1U;
    uint32_t DPR2_2L;
    uint32_t DPR2_2U;
    uint32_t DPR2_3L;
    uint32_t DPR2_3U;

    uint32_t DPR3_0L;
    uint32_t DPR3_0U;
    uint32_t DPR3_1L;
    uint32_t DPR3_1U;
    uint32_t DPR3_2L;
    uint32_t DPR3_2U;
    uint32_t DPR3_3L;
    uint32_t DPR3_3U;

    uint32_t CPR0_0L;
    uint32_t CPR0_0U;
    uint32_t CPR0_1L;
    uint32_t CPR0_1U;
    uint32_t CPR0_2L;
    uint32_t CPR0_2U;
    uint32_t CPR0_3L;
    uint32_t CPR0_3U;

    uint32_t CPR1_0L;
    uint32_t CPR1_0U;
    uint32_t CPR1_1L;
    uint32_t CPR1_1U;
    uint32_t CPR1_2L;
    uint32_t CPR1_2U;
    uint32_t CPR1_3L;
    uint32_t CPR1_3U;

    uint32_t CPR2_0L;
    uint32_t CPR2_0U;
    uint32_t CPR2_1L;
    uint32_t CPR2_1U;
    uint32_t CPR2_2L;
    uint32_t CPR2_2U;
    uint32_t CPR2_3L;
    uint32_t CPR2_3U;

    uint32_t CPR3_0L;
    uint32_t CPR3_0U;
    uint32_t CPR3_1L;
    uint32_t CPR3_1U;
    uint32_t CPR3_2L;
    uint32_t CPR3_2U;
    uint32_t CPR3_3L;
    uint32_t CPR3_3U;

    uint32_t DPM0;
    uint32_t DPM1;
    uint32_t DPM2;
    uint32_t DPM3;

    uint32_t CPM0;
    uint32_t CPM1;
    uint32_t CPM2;
    uint32_t CPM3;

    /* Memory Management Registers */
    uint32_t MMU_CON;
    uint32_t MMU_ASI;
    uint32_t MMU_TVA;
    uint32_t MMU_TPA;
    uint32_t MMU_TPX;
    uint32_t MMU_TFA;
    /* {1.3.1 only */
    uint32_t BMACON;
    uint32_t SMACON;
    uint32_t DIEAR;
    uint32_t DIETR;
    uint32_t CCDIER;
    uint32_t MIECON;
    uint32_t PIEAR;
    uint32_t PIETR;
    uint32_t CCPIER;
    /*} */
    /* Debug Registers */
    uint32_t DBGSR;
    uint32_t EXEVT;
    uint32_t CREVT;
    uint32_t SWEVT;
    uint32_t TR0EVT;
    uint32_t TR1EVT;
    uint32_t DMS;
    uint32_t DCX;
    uint32_t DBGTCR;
    uint32_t CCTRL;
    uint32_t CCNT;
    uint32_t ICNT;
    uint32_t M1CNT;
    uint32_t M2CNT;
    uint32_t M3CNT;
    /* Floating Point Registers */
    float_status fp_status;
    uint32_t hflags;    /* CPU State */

    /* Internal CPU feature flags.  */
    uint64_t features;

    uint32_t opcode;
    uint32_t freq_hz;
    uint32_t mode;
    uint32_t next_pc;
    uint32_t insn_num;
    uint32_t stop_step;
    uint32_t dbgEnable;
    char *dbg_filename;
    FILE *dbg_fp;
    breakpoint_mgt_t bp_manager;
    struct
    {
        conf_object_t* obj;
        memory_space_intf* iface;
    } memory_space;
    void (*dump_state)(conf_object_t *opaque, FILE *f);
};

typedef struct TriCoreState tricore_cpu_t;

uint32_t tricore_step_once(conf_object_t* opaque);
void decode_opc_and_exec(conf_object_t* opaque, uint32_t opcode);
void tricore_cpu_dump_state(conf_object_t* opaque, FILE *f);
#endif

/*  armdefs.h -- ARMulator common definitions:  ARM6 Instruction Emulator.
 * Copyright (C) 1994 Advanced RISC Machines Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; eithere version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#ifndef _ARMDEFS_H_
#define _ARMDEFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//teawater add for arm2x86 2005.02.14-------------------------------------------
// koodailar remove it for mingw 2005.12.18----------------
//anthonylee modify it for portable 2007.01.30
#include "portable/mman.h"

#include "arm_regformat.h"
//AJ2D--------------------------------------------------------------------------

//teawater add for arm2x86 2005.07.03-------------------------------------------

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <memory_space.h>
#include <skyeye_core.h>
//AJ2D--------------------------------------------------------------------------
#if 0
#if 0
#define DIFF_STATE 1
#define __FOLLOW_MODE__ 0
#else
#define DIFF_STATE 0
#define __FOLLOW_MODE__ 1
#endif
#endif

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

#define LOW 0
#define HIGH 1
#define LOWHIGH 1
#define HIGHLOW 2

#ifndef u8
#define u8      unsigned char
#define u16     unsigned short
#define u32     unsigned int
#define u64     unsigned long long
#endif /*u8 */

//#include <skyeye_defs.h>
#include <skyeye_types.h>

#define ARM_BYTE_TYPE 		0
#define ARM_HALFWORD_TYPE 	1
#define ARM_WORD_TYPE 		2

//the define of cachetype
#define NONCACHE  0
#define DATACACHE  1
#define INSTCACHE  2

#ifndef __STDC__
typedef char *VoidStar;
#endif

typedef unsigned long long ARMdword;    /* must be 64 bits wide */
typedef unsigned int ARMword;           /* must be 32 bits wide */
typedef unsigned char ARMbyte;          /* must be 8 bits wide */
typedef unsigned short ARMhword;        /* must be 16 bits wide */
typedef struct ARMul_State ARMul_State;
typedef struct ARMul_io ARMul_io;
typedef struct ARMul_Energy ARMul_Energy;

//teawater add for arm2x86 2005.06.24-------------------------------------------
#include <stdint.h>
//AJ2D--------------------------------------------------------------------------
/*
 * //chy 2005-05-11
 * #ifndef __CYGWIN__
 * //teawater add for arm2x86 2005.02.14-------------------------------------------
 * typedef unsigned char           uint8_t;
 * typedef unsigned short          uint16_t;
 * typedef unsigned int            uint32_t;
 * #if defined (__x86_64__)
 * typedef unsigned long           uint64_t;
 * #else
 * typedef unsigned long long      uint64_t;
 * #endif
 * ////AJ2D--------------------------------------------------------------------------
 * #endif
 */

#include "armmmu.h"
//#include "lcd/skyeye_lcd.h"

#include "skyeye.h"
#include "skyeye_device.h"
//#include "net/skyeye_net.h"
#include "skyeye_config.h"

typedef unsigned ARMul_CPInits(ARMul_State * state);
typedef unsigned ARMul_CPExits(ARMul_State * state);
typedef unsigned ARMul_LDCs(ARMul_State * state, unsigned type, ARMword instr, ARMword value);
typedef unsigned ARMul_STCs(ARMul_State * state, unsigned type, ARMword instr, ARMword * value);
typedef unsigned ARMul_MRCs(ARMul_State * state, unsigned type, ARMword instr, ARMword * value);
typedef unsigned ARMul_MCRs(ARMul_State * state, unsigned type, ARMword instr, ARMword value);
typedef unsigned ARMul_MRRCs(ARMul_State * state, unsigned type, ARMword instr, ARMword * value1, ARMword * value2);
typedef unsigned ARMul_MCRRs(ARMul_State * state, unsigned type, ARMword instr, ARMword value1, ARMword value2);
typedef unsigned ARMul_CDPs(ARMul_State * state, unsigned type, ARMword instr);
typedef unsigned ARMul_CPReads(ARMul_State * state, unsigned reg, ARMword * value);
typedef unsigned ARMul_CPWrites(ARMul_State * state, unsigned reg, ARMword value);

#define MPU_REGION_SIZE   12
#define MPU_SUBREGION_NUM 8

typedef enum
{
    MPU_MT_Strongly_ordered,
    MPU_MT_Device,
    MPU_MT_Normal,
} mpu_memory_type_t;

typedef struct mpu_state_s
{
    ARMword curr_region_number;
    ARMword control;
    struct
    {
        ARMword XN;                     // eXecute Never
        ARMword base_address;
        ARMword size;
        ARMword end_address;
        ARMword attributes;
        mpu_memory_type_t memory_type;
        ARMword shareable;
        ARMword access_permissions;
        ARMword subregion_enables;
        ARMword enable;
    } regions[MPU_REGION_SIZE];

#define SCTLR_BR   0x20000
#define SCTLR_C    0x4
#define SCTLR_A    0x2
#define SCTLR_M    0x1
    ARMword SCTLR;                      // c1, System Control Register

    ARMword process_id;
} mpu_state_t;

#define CUR_REGION_NUM state->mpu.curr_region_number

//added by ksh,2004-3-5
struct ARMul_io
{
    ARMword *instr;                     //to display the current interrupt state
    ARMword *net_flag;                  //to judge if network is enabled
    ARMword *net_int;                   //netcard interrupt

    //ywc,2004-04-01
    ARMword *ts_int;
    ARMword *ts_is_enable;
    ARMword *ts_addr_begin;
    ARMword *ts_addr_end;
    ARMword *ts_buffer;
};

/* added by ksh,2004-11-26,some energy profiling */
struct ARMul_Energy
{
    int energy_prof;                    /* <tktan>  BUG200103282109 : for energy profiling */
    int enable_func_energy;             /* <tktan> BUG200105181702 */
    char *func_energy;
    int func_display;                   /* <tktan> BUG200103311509 : for function call display */
    int func_disp_start;                /* <tktan> BUG200104191428 : to start func profiling */
    char *start_func;                   /* <tktan> BUG200104191428 */

    FILE *outfile;                      /* <tktan> BUG200105201531 : direct console to file */
    long long tcycle, pcycle;
    float t_energy;
    void *cur_task;                     /* <tktan> BUG200103291737 */
    long long t_mem_cycle, t_idle_cycle, t_uart_cycle;
    long long p_mem_cycle, p_idle_cycle, p_uart_cycle;
    long long p_io_update_tcycle;
    /*record CCCR,to get current core frequency */
    ARMword cccr;
};

typedef struct
{
    bool_t isExec[4];
    ARMword insnNum;
    ARMword runInsn;
} it_block_insn_t;

typedef union
{
    uint64_t X;
    struct
    {
        uint32_t W;
        uint32_t High;
    };
} general_reg_t;

/*A 128-bit register named Q0 to Q31
 *A 64-bit register named D0 to D31
 *A 32-bit register named S0 to S31.
 *A 16-bit register named H0 to H31
 *A 8-bit register named B0 to B31.
 */
typedef union
{
    struct
    {
        uint64_t d[2];
    } dword;
    struct
    {
        uint32_t s[4];
    } word;
    struct
    {
        uint16_t h[8];
    } hword;
    struct
    {
        uint8_t b[16];
    } byte;
} simd_fp_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t RES0:8;
        uint32_t IOE:1;
        uint32_t DZE:1;
        uint32_t OFE:1;
        uint32_t UFE:1;
        uint32_t IXE:1;
        uint32_t res1:2;
        uint32_t IDE:1;
        uint32_t Len:3;
        uint32_t FZ16:1;
        uint32_t Stride:2;
        uint32_t RMode:2;
        uint32_t FZ:1;
        uint32_t DN:1;
        uint32_t AHP:1;
    };
} FPCR_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t IOC:1;
        uint32_t DZC:1;
        uint32_t OFC:1;
        uint32_t UFC:1;
        uint32_t IXC:1;
        uint32_t RES1:2;
        uint32_t IDC:1;
        uint32_t RES2:19;
        uint32_t QC:1;
        uint32_t V:1;
        uint32_t C:1;
        uint32_t Z:1;
        uint32_t N:1;
    };
} FPSR_reg_t;

typedef union ProcState
{
    uint64_t all;
    struct
    {
        uint8_t N:1;                    // Negative condition flag
        uint8_t Z:1;                    // Zero condition flag
        uint8_t C:1;                    // Carry condition flag
        uint8_t V:1;                    // oVerflow condition flag
        uint8_t D:1;                    // Debug mask bit [AArch64 only]
        uint8_t A:1;                    // SError interrupt mask bit
        uint8_t I:1;                    // IRQ mask bit
        uint8_t F:1;                    // FIQ mask bit
        uint8_t PAN:1;                  // Privileged Access Never Bit [v8.1]
        uint8_t UAO:1;                  // User Access Override [v8.2]
        uint8_t DIT:1;                  // Data Independent Timing [v8.4]
        uint8_t TCO:1;                  // Tag Check Override [v8.5, AArch64 only]
        uint8_t BTYPE:2;                // Branch Type [v8.5]
        uint8_t SS:1;                   // Software step bit
        uint8_t IL:1;                   // Illegal Execution state bit
        uint8_t EL:2;                   // Exception Level
        uint8_t nRW:1;                  // not Register Width: 0=64, 1=32
        uint8_t SP:1;                   // Stack pointer select: 0=SP0, 1=SPx [AArch64 only]
        uint8_t Q:1;                    // Cumulative saturation flag [AArch32 only]
        uint8_t Ge:4;                   // Greater than or Equal flags [AArch32 only]
        uint8_t SSBS:1;                 // Speculative Store Bypass Safe
        uint8_t IT:8;                   // If-then bits, RES0 in CPSR [AArch32 only]
        uint8_t J:1;                    // J bit, RES0 [AArch32 only, RES0 in SPSR and CPSR]
        uint8_t T:1;                    // T32 bit, RES0 in CPSR [AArch32 only]
        uint8_t E:1;                    // Endianness bit [AArch32 only]
        uint8_t M:5;                    // Mode field [AArch32 only]
    };
} ProcState_reg_t;

typedef union
{
    uint64_t all;

    struct
    {
        uint32_t RES0:2;
        uint32_t EL:2;                  //Current Exception level
    };
} CurrentEL_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t RES0:6;
        uint32_t F:1;                   //FIQ mask bit
        uint32_t I:1;                   //IRQ mask bit
        uint32_t A:1;                   //SError interrupt mask bit
        uint32_t D:1;                   //Process state D mask
    };
} DAIF_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t RES0:24;
        uint32_t DIT:1;                 //Data Independent Timing
    };
} DIT_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t RES0:28;
        uint32_t V:1;                   //Negative condition flag
        uint32_t C:1;                   //Zero condition flag
        uint32_t Z:1;                   //Carry condition flag
        uint32_t N:1;                   //Overflow condition flag
    };
} NZCV_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t RES0:22;
        uint32_t PAN:1;                 //Privileged Access Never
    };
} PAN_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t SP:1;                  //Stack pointer to use
    };
} SPSel_EL3_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t M:5;                   //
        uint32_t T:1;                   //T32 Instruction set state
        uint32_t F:1;                   //FIQ interrupt mask
        uint32_t I:1;                   //IRQ interrupt mask.
        uint32_t A:1;                   //SError interrupt mask
        uint32_t E:1;                   //Set to the value of PSTATE.E
        uint32_t IT0:6;                 //IT[7:2]Set to the value of PSTATE.IT[7:2]
        uint32_t GE:4;                  //Greater than or Equal flags
        uint32_t IL:1;                  //Illegal Execution state
        uint32_t DIT:1;                 //Data Independent Timing
        uint32_t PAN:1;                 //Privileged Access Never
        uint32_t SSBS:1;                //Set to the value of PSTATE.SSBS
        uint32_t J:1;                   //the {J, T} bits determined the AArch32 Instruction set state
        uint32_t IT1:2;                 //IT[1:0] Set to the value of PSTATE.IT[1:0]
        uint32_t Q:1;                   //Overflow or saturation flag
        uint32_t V:1;                   //Overflow Condition flag
        uint32_t C:1;                   //Carry Condition flag
        uint32_t Z:1;                   //Zero Condition flag
        uint32_t N:1;                   //Negative Condition flag
    };
} SPSR_UNEL_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t M0:4;                  //AArch32 Mode Set to the value of PSTATE.M[3:0] on taking an exception to EL1
        uint32_t M1:1;                  //Execution state Set to 0b1, the value of PSTATE.nRW,
        uint32_t T:1;                   //T32 Instruction set state
        uint32_t F:1;                   //FIQ interrupt mask
        uint32_t I:1;                   //IRQ interrupt mask
        uint32_t A:1;                   //SError interrupt mask
        uint32_t E:1;                   //Set to the value of PSTATE.E
        uint32_t IT0:6;                 //IT[7:2]Set to the value of PSTATE.IT[7:2]
        uint32_t GE:4;                  //Greater than or Equal flags
        uint32_t IL:1;                  //Illegal Execution state
        uint32_t SS:1;                  //Software Step
        uint32_t PAN:1;                 //Privileged Access Never
        uint32_t SSBS:1;                //Set to the value of PSTATE.SSBS
        uint32_t DIT:1;                 //Data Independent Timing
        uint32_t IT1:2;                 //IT[1:0] Set to the value of PSTATE.IT[1:0]
        uint32_t Q:1;                   //Overflow or saturation flag
        uint32_t V:1;                   //Overflow Condition flag
        uint32_t C:1;                   //Carry Condition flag
        uint32_t Z:1;                   //Zero Condition flag
        uint32_t N:1;                   //Negative Condition flag
    };
} SPSR_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t M0:4;                  //AArch32 Mode Set to the value of PSTATE.M[3:0] on taking an exception to EL1
        uint32_t M1:1;                  //Execution state Set to 0b1, the value of PSTATE.nRW,
        uint32_t T:1;                   //T32 Instruction set state
        uint32_t F:1;                   //FIQ interrupt mask
        uint32_t I:1;                   //IRQ interrupt mask
        uint32_t A:1;                   //SError interrupt mask
        uint32_t E:1;                   //Set to the value of PSTATE.E
        uint32_t IT0:6;                 //IT[7:2]Set to the value of PSTATE.IT[7:2]
        uint32_t GE:4;                  //Greater than or Equal flags
        uint32_t IL:1;                  //Illegal Execution state
        uint32_t SS:1;                  //Software Step
        uint32_t PAN:1;                 //Privileged Access Never
        uint32_t SSBS:1;                //Set to the value of PSTATE.SSBS
        uint32_t DIT:1;                 //Data Independent Timing
        uint32_t IT1:2;                 //IT[1:0] Set to the value of PSTATE.IT[1:0]
        uint32_t Q:1;                   //Overflow or saturation flag
        uint32_t V:1;                   //Overflow Condition flag
        uint32_t C:1;                   //Carry Condition flag
        uint32_t Z:1;                   //Zero Condition flag
        uint32_t N:1;                   //Negative Condition flag
    };
} A32_SPSR_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t M0:4;                  //AArch32 Mode Set to the value of PSTATE.M[3:0] on taking an exception to EL1
        uint32_t M1:1;                  //Execution state Set to 0b1, the value of PSTATE.nRW,
        uint32_t RES0:1;
        uint32_t F:1;                   //FIQ interrupt mask
        uint32_t I:1;                   //IRQ interrupt mask
        uint32_t A:1;                   //SError interrupt mask
        uint32_t D:1;                   //Set to the value of PSTATE.E
        uint32_t BTYPE:2;               //Branch Type Indicator. Set to the value of PSTATE.BTYPE
        uint32_t SSBS:1;                //Set to the value of PSTATE.SSBS
        uint32_t RES1:7;
        uint32_t IL:1;                  //Illegal Execution state
        uint32_t SS:1;                  //Software Step
        uint32_t PAN:1;                 //Privileged Access Never
        uint32_t UAO:1;                 //User Access Override. Set to the value of PSTATE.UAO
        uint32_t DIT:1;                 //Data Independent Timing
        uint32_t TCO:1;                 //Tag Check Override Set to the value of PSTATE.TCO
        uint32_t RES2:2;
        uint32_t V:1;                   //Overflow Condition flag
        uint32_t C:1;                   //Carry Condition flag
        uint32_t Z:1;                   //Zero Condition flag
        uint32_t N:1;                   //Negative Condition flag
    };
} A64_SPSR_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t RES0:12;
        uint32_t SSBS:1;                //Speculative Store Bypass Safe
    };
} SSBS_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t RES0:25;
        uint32_t TCO:1;                 //Allows memory tag checks to be globally disabled
    };
} TCO_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t RES0:23;
        uint32_t UAO:1;                 //User Access Override
    };
} UAO_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ACTLR;
        uint32_t ARCLR2;
    };
} ACTLR_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t HACTLR;
        uint32_t HACTLR2;
    };
} ACTLR_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ADFSR;
        uint32_t RES;
    };
} AFSR0_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t HADFSR;
        uint32_t RES;
    };
} AFSR0_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t AIFSR;
        uint32_t RES;
    };
} AFSR1_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t HAIFSR;
        uint32_t RES;
    };
} AFSR1_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t AIDR;
        uint32_t RES;
    };
} AIDR_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t AMAIR0;
        uint32_t AMAIR1;
    };
} AMAIR_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t HAMAIR0;
        uint32_t HAMAIR1;
    };
} AMAIR_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t CCSIDR2;
        uint32_t RES0;
    };
    struct
    {
        uint32_t NumSets:24;
    };
} CCSIDR2_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t CCSIDR;
        uint32_t CCSIDR2;
    };
    struct
    {
        uint32_t LineSize:3;
        uint32_t Associativity:21;
        uint32_t RES0:8;
        uint32_t NumSets:24;
    };
} CCSIDR_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t CLIDR;
        uint32_t RES0;
    };
    struct
    {
        uint32_t Ctype1:3;
        uint32_t Ctype2:3;
        uint32_t Ctype3:3;
        uint32_t Ctype4:3;
        uint32_t Ctype5:3;
        uint32_t Ctype6:3;
        uint32_t Ctype7:3;
        uint32_t LoUIS:3;
        uint32_t Loc:3;
        uint32_t LoUU:3;
        uint32_t ICB:3;
    };
} CLIDR_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t PROCID:32;
        uint32_t RES0:32;
    };
} CONTEXTIDR_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t PROCID:32;
        uint32_t RES0:32;
    };
} CONTEXTIDR_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t CAPCR;
        uint32_t RES;
    };
    struct
    {
        uint32_t RES0:16;
        uint32_t ZEN:2;
        uint32_t RES1:2;
        uint32_t FPEN:2;
        uint32_t RES2:6;
        uint32_t TTA:1;
    };
} CPACR_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t HCPTR;
        uint32_t RES;
    };
    struct
    {
        uint32_t RES0:16;
        uint32_t ZEN:2;
        uint32_t RES1:2;
        uint32_t FPEN:2;
        uint32_t RES2:6;
        uint32_t TTA:1;
        uint32_t RES3:1;
        uint32_t TAM:1;
        uint32_t TCPAC:1;
    };
} CPTR_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t RES0:8;
        uint32_t EZ:1;
        uint32_t RES1:1;
        uint32_t TFP:1;
        uint32_t RES2:9;
        uint32_t TTA:1;
        uint32_t RES3:9;
        uint32_t TAM:1;
        uint32_t TCPAC:1;
    };
} CPTR_EL3_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t DFSR;
        uint32_t RES;
    };
    struct
    {
        uint32_t ISS:25;
        uint32_t IL:1;
        uint32_t EC:6;
    };
} ESR_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t HSR;
        uint32_t RES;
    };
    struct
    {
        uint32_t ISS:25;
        uint32_t IL:1;
        uint32_t EC:6;
    };
} ESR_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ISS:25;
        uint32_t IL:1;
        uint32_t EC:6;
    };
} ESR_EL3_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t DFAR;
        uint32_t IFAR;
    };
} FAR_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t HDFAR;
        uint32_t HIFAR;
    };
    struct
    {
        uint32_t DFAR;
        uint32_t IFAR;
    };
} FAR_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t FPEXC;
        uint32_t RES;
    };
    struct
    {
        uint32_t IOF:1;
        uint32_t DZF:1;
        uint32_t OFF:1;
        uint32_t UFF:1;
        uint32_t IXF:1;
        uint32_t RES0:2;
        uint32_t IDF:1;
        uint32_t VECITR:3;
        uint32_t RES1:15;
        uint32_t TFV:1;
        uint32_t VV:1;
        uint32_t FP2V:1;
        uint32_t DEX:1;
        uint32_t EN:1;
        uint32_t EX:1;
    };
} FPEXC32_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t HACR;
        uint32_t RES;
    };
} HACR_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t HCR;
        uint32_t HCR2;
    };
    struct
    {
        uint32_t VM:1;
        uint32_t SWIO:1;
        uint32_t PTW:1;
        uint32_t FMO:1;
        uint32_t IMO:1;
        uint32_t AMO:1;
        uint32_t VF:1;
        uint32_t VI:1;
        uint32_t VSE:1;
        uint32_t FB:1;
        uint32_t BSU:2;
        uint32_t DC:1;
        uint32_t TWI:1;
        uint32_t TWE:1;
        uint32_t TID0:1;
        uint32_t TID1:1;
        uint32_t TID2:1;
        uint32_t TID3:1;
        uint32_t TSC:1;
        uint32_t TIDCP:1;
        uint32_t TACR:1;
        uint32_t TSW:1;
        uint32_t TPCF:1;
        uint32_t TPU:1;
        uint32_t TTLB:1;
        uint32_t TVM:1;
        uint32_t TGE:1;
        uint32_t TDZ:1;
        uint32_t HCD:1;
        uint32_t TRVM:1;
        uint32_t RW:1;
        uint32_t CD:1;
        uint32_t ID:1;
        uint32_t E2H:1;
        uint32_t TLOR:1;
        uint32_t TERR:1;
        uint32_t TEA:1;
        uint32_t MIOCNCE:1;
        uint32_t RES0:1;
        uint32_t APK:1;
        uint32_t API:1;
        uint32_t NV0:1;
        uint32_t NV1:1;
        uint32_t AT:1;
        uint32_t NV2:1;
        uint32_t FWB:1;
        uint32_t FIEN:1;
        uint32_t RES1:1;
        uint32_t TID4:1;
        uint32_t TICAB:1;
        uint32_t RES2:1;
        uint32_t TOCU:1;
        uint32_t EnSCXT:1;
        uint32_t TTLBIS:1;
        uint32_t TTLBOS:1;
        uint32_t ATA:1;
        uint32_t DCT:1;
        uint32_t TID5:1;
    };
} HCR_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t HPFAR;
        uint32_t RES;
    };
    struct
    {
        uint64_t RES0:4;
        uint64_t FIPA0:36;
        uint64_t FIPA1:4;
        uint64_t RES1:19;
        uint64_t NS:1;
    };
} HPFAR_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t HSTR;
        uint32_t RES;
    };
    struct
    {
        uint32_t T0:1;
        uint32_t T1:1;
        uint32_t T2:1;
        uint32_t T3:1;
        uint32_t T4:1;
        uint32_t T5:1;
        uint32_t T6:1;
        uint32_t T7:1;
        uint32_t T8:1;
        uint32_t T9:1;
        uint32_t T10:1;
        uint32_t T11:1;
        uint32_t T12:1;
        uint32_t T13:1;
        uint32_t T14:1;
        uint32_t T15:1;
    };
} HSTR_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ID_AFR0;
        uint32_t ID_RES;
    };
    struct
    {
        uint32_t IMP0:4;
        uint32_t IMP1:4;
        uint32_t IMP2:4;
        uint32_t IMP3:4;
    };
} ID_AFR0_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ID_DFR0;
        uint32_t ID_RES;
    };
    struct
    {
        uint32_t CopDbg:4;
        uint32_t CopSDbg:4;
        uint32_t MMapDbg:4;
        uint32_t CopTrc:4;
        uint32_t MMapTrc:4;
        uint32_t MProfDbg:4;
        uint32_t PerfMon:4;
        uint32_t TraceFilt:4;
    };
} ID_DFR0_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ID_ISAR0;
        uint32_t ID_RES;
    };
    struct
    {
        uint32_t Swap:4;
        uint32_t BitCount:4;
        uint32_t BitField:4;
        uint32_t CmpBranch:4;
        uint32_t Coproc:4;
        uint32_t Debug:4;
        uint32_t Divide:4;
    };
} ID_ISAR0_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ID_ISAR1;
        uint32_t ID_RES;
    };
    struct
    {
        uint32_t Endian:4;
        uint32_t Except:4;
        uint32_t Except_AR:4;
        uint32_t Extend:4;
        uint32_t IfThen:4;
        uint32_t Immediate:4;
        uint32_t Interwork:4;
        uint32_t Jazelle:4;
    };
} ID_ISAR1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ID_ISAR2;
        uint32_t ID_RES;
    };
    struct
    {
        uint32_t LoadStore:4;
        uint32_t MemHint:4;
        uint32_t MultiAccessInt:4;
        uint32_t Mult:4;
        uint32_t MultS:4;
        uint32_t MultU:4;
        uint32_t PSR_AR:4;
        uint32_t Reversal:4;
    };
} ID_ISAR2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ID_ISAR3;
        uint32_t ID_RES;
    };
    struct
    {
        uint32_t Saturate:4;
        uint32_t SIMD:4;
        uint32_t SVC:4;
        uint32_t SynchPrim:4;
        uint32_t TabBranch:4;
        uint32_t T32Copy:4;
        uint32_t TrueNOP:4;
        uint32_t T32EE:4;
    };
} ID_ISAR3_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ID_ISAR4;
        uint32_t ID_RES;
    };
    struct
    {
        uint32_t Unpriv:4;
        uint32_t WithShifts:4;
        uint32_t Writeback:4;
        uint32_t SMC:4;
        uint32_t Barrier:4;
        uint32_t SynchPrim_frac:4;
        uint32_t PSR_M:4;
        uint32_t SWP_frac:4;
    };
} ID_ISAR4_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ID_ISAR5;
        uint32_t ID_RES;
    };
    struct
    {
        uint32_t SEVL:4;
        uint32_t AES:4;
        uint32_t SHA1:4;
        uint32_t SHA2:4;
        uint32_t CRC32:4;
        uint32_t RES0:4;
        uint32_t RDM:4;
        uint32_t VCMA:4;
    };
} ID_ISAR5_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ID_ISAR6;
        uint32_t ID_RES;
    };
    struct
    {
        uint32_t JSCVT:4;
        uint32_t DP:4;
        uint32_t FHM:4;
        uint32_t SB:4;
        uint32_t SPECRES:4;
    };
} ID_ISAR6_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ID_PFR0;
        uint32_t ID_RES;
    };
    struct
    {
        uint32_t State0:4;
        uint32_t State1:4;
        uint32_t State2:4;
        uint32_t State3:4;
        uint32_t CSV2:4;
        uint32_t AMU:4;
        uint32_t DIT:4;
        uint32_t RAS:4;
    };
} ID_PFR0_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ID_PFR1;
        uint32_t ID_RES;
    };
    struct
    {
        uint32_t ProgMod:4;
        uint32_t Security:4;
        uint32_t MProgMod:4;
        uint32_t Virtualization:4;
        uint32_t GenTimer:4;
        uint32_t See_frac:4;
        uint32_t Virt_frac:4;
        uint32_t GIC:4;
    };
} ID_PFR1_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ID_PFR2;
        uint32_t ID_RES;
    };
    struct
    {
        uint32_t CSV3:4;
        uint32_t SSBS:4;
        uint32_t RAS_frac:4;
    };
} ID_PFR2_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t IFSR;
        uint32_t RES;
    };
    struct
    {
        uint32_t FS03:4;
        uint32_t RES0:5;
        uint32_t LPAE:1;
        uint32_t FS4:1;
        uint32_t RES1:1;
        uint32_t Ext:1;
        uint32_t RES2:3;
        uint32_t FnV:1;
    };
} IFSR32_EL2_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t ISR;
        uint32_t RES;
    };
    struct
    {
        uint32_t RES0:6;
        uint32_t F:1;
        uint32_t I:1;
        uint32_t A:1;
    };
} ISR_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t SCR;
        uint32_t RES;
    };
    struct
    {
        uint32_t NS:1;
        uint32_t IRQ:1;
        uint32_t FIQ:1;
        uint32_t EA:1;
        uint32_t RES0:3;
        uint32_t SMD:1;
        uint32_t HCE:1;
        uint32_t SIF:1;
        uint32_t RW:1;
        uint32_t ST:1;
        uint32_t TWI:1;
        uint32_t TWE:1;
        uint32_t FLOR:1;
        uint32_t TERR:1;
        uint32_t APK:1;
        uint32_t API:1;
        uint32_t EEL2:1;
        uint32_t EASE:1;
        uint32_t NMEA:1;
        uint32_t FIEN:1;
        uint32_t RES1:3;
        uint32_t EnSCXT:1;
        uint32_t ATA:1;
    };
} SCR_EL3_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint8_t ENABLE:1;               //Enables the timer
        uint8_t IMASK:1;                //Timer interrupt mask bit
        uint8_t ISTATUS:1;              //The status of the timer
    };
} CNTP_CTL_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t M:1;
        uint32_t A:1;
        uint32_t C:1;
        uint32_t res0:4;
        uint32_t ITD:1;
        uint32_t SED:1;
        uint32_t UMA:1;
        uint32_t EnRCTX:1;
        uint32_t EOS:1;
        uint32_t I:1;
        uint32_t res1:12;
        uint32_t EE:1;
        uint32_t res2:16;
        uint32_t ATA0:1;
        uint32_t ATA:1;
    };
} SCTLR_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint8_t T0SZ:6;
        uint8_t res0:2;
        uint8_t IRGN0:2;
        uint8_t ORGN0:2;
        uint8_t SH0:2;
        uint8_t TG0:2;
        uint8_t PS:3;
        uint8_t res1:1;
        uint8_t TBI:1;
        uint8_t HA:1;
        uint8_t HD:1;
        uint8_t res2:1;
        uint8_t HPD:1;
    };
} TCR_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint8_t T0SZ:6;
        uint8_t RES0_1:1;
        uint8_t EPD0:1;

        uint8_t IRGN0:2;
        uint8_t ORGN0:2;
        uint8_t SH0:2;
        uint8_t TG0:2;

        uint8_t T1SZ:6;
        uint8_t A1:1;
        uint8_t EPD1:1;

        uint8_t IRGN1:2;
        uint8_t ORGN1:2;
        uint8_t SH1:2;
        uint8_t TG1:2;

        uint8_t IPS:3;
        uint8_t RES0_2:1;
        uint8_t AS:1;
        uint8_t TBI0:1;
        uint8_t TBI1:1;
        uint8_t HA:1;

        uint8_t HD:1;
        uint8_t HPD0:1;
        uint8_t HPD1:1;        
        uint8_t HWU059:1;
        uint8_t HWU060:1;
        uint8_t HWU061:1;
        uint8_t HWU062:1;
        uint8_t HWU159:1;

        uint8_t HWU160:1;
        uint8_t HWU161:1;
        uint8_t HWU162:1;
        uint8_t TBID0:1;
        uint8_t TBID1:1;
        uint8_t NFD0:1;
        uint8_t NFD1:1;

        uint8_t E0PD0:1;
        uint8_t E0PD1:1;
        uint8_t TCMA0:1;
        uint8_t TCMA1:1;
        uint8_t RES0_3:1;  
    };
} TCR_EL1_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t SS:1;
        uint32_t RES0:5;
        uint32_t ERR:1;
        uint32_t RES1:5;
        uint32_t TDCC:1;
        uint32_t KDE:1;
        uint32_t HDE:1;
        uint32_t MDE:1;
        uint32_t WI:3;
        uint32_t SC2:1;
        uint32_t RES2:1;
        uint32_t TDA:1;
        uint32_t INTdis:2;
        uint32_t RES3:2;
        uint32_t TXU:1;
        uint32_t RXO:1;
        uint32_t RES4:1;
        uint32_t TXfull:1;
        uint32_t RXfull:1;
        uint32_t TFO:1;
    };
} MDSCR_reg_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t low32;
        uint32_t high32;
    };
} MAIR_reg_t;


typedef struct
{
    CurrentEL_reg_t CurrentEL;
    DAIF_reg_t DAIF;
    DIT_reg_t DIT;
    NZCV_reg_t NZCV;
    PAN_reg_t PAN;
    SPSel_EL3_reg_t SPSel;
    SSBS_reg_t SSBS;
    TCO_reg_t TCO;
    UAO_reg_t UAO;
    ARMdword ELR_EL1;
    ARMdword ELR_EL2;
    ARMdword ELR_EL3;
    FPCR_reg_t FPCR;
    FPSR_reg_t FPSR;
    ARMdword SP_EL0;
    ARMdword SP_EL1;
    ARMdword SP_EL2;
    //ARMdword SP_EL3;
    //SPSR_UNEL_reg_t SPSR_abt;
    SPSR_EL1_reg_t SPSR_EL1;
    //A32_SPSR_EL2_reg_t A32_SPSR_EL2;
    A64_SPSR_EL2_reg_t A64_SPSR_EL2;
    //A32_SPSR_EL2_reg_t A32_SPSR_EL3;
    A64_SPSR_EL2_reg_t A64_SPSR_EL3;
    //SPSR_UNEL_reg_t SPSR_fiq;
    //SPSR_UNEL_reg_t SPSR_irq;
    //SPSR_UNEL_reg_t SPSR_und;
    //AArch64 system registers
    ACTLR_EL1_reg_t ACTLR_EL1;          //Auxiliary Control Register (EL1)
    ACTLR_EL2_reg_t ACTLR_EL2;          //Auxiliary Control Register (EL2)
    ARMdword ACTLR_EL3;                 //Auxiliary Control Register (EL3)
    AFSR0_EL1_reg_t AFSR0_EL1;          //Auxiliary Fault Status Register 0 (EL1)
    AFSR0_EL2_reg_t AFSR0_EL2;          //Auxiliary Fault Status Register 0 (EL2)
    ARMdword AFSR0_EL3;                 //Auxiliary Fault Status Register 0 (EL3)
    AFSR1_EL1_reg_t AFSR1_EL1;          //Auxiliary Fault Status Register 1(EL1)
    AFSR1_EL2_reg_t AFSR1_EL2;          //Auxiliary Fault Status Register 1(EL2)
    ARMdword AFSR1_EL3;                 //Auxiliary Fault Status Register 1(EL3)
    AIDR_EL1_reg_t AIDR_EL1;            //Auxiliary ID Register
    AMAIR_EL1_reg_t AMAIR_EL1;          //Auxiliary Memory Attribute Indirection Register (EL1)
    AMAIR_EL2_reg_t AMAIR_EL2;          //Auxiliary Memory Attribute Indirection Register (EL2)
    ARMdword AMAIR_EL3;                 //Auxiliary Memory Attribute Indirection Register (EL3)
    ARMdword APDAKeyHi_EL1;             //Pointer Authentication Key A for Data (bits[127:64])
    ARMdword APDAKeyLo_EL1;             //Pointer Authentication Key A for Data (bits[63:0])
    ARMdword APDBKeyHi_EL1;             //Pointer Authentication Key B for Data (bits[127:64])
    ARMdword APDBKeyLo_EL1;             //Pointer Authentication Key B for Data (bits[63:0])
    ARMdword APGAKeyHi_EL1;             //Pointer Authentication Key A for Code (bits[127:64])
    ARMdword APGAKeyLo_EL1;             //Pointer Authentication Key A for Code (bits[63:0])
    ARMdword APIAKeyHi_EL1;             //Pointer Authentication Key A for Instruction (bits[127:64])
    ARMdword APIAKeyLo_EL1;             //Pointer Authentication Key A for Instruction (bits[63:0])
    ARMdword APIBKeyHi_EL1;             //Pointer Authentication Key B for Instruction (bits[127:64])
    ARMdword APIBKeyLo_EL1;             //Pointer Authentication Key B for Instruction (bits[63:0])
    CCSIDR2_EL1_reg_t CCSIDR2_EL1;      //Current Cache Size ID Register 2
    CCSIDR_EL1_reg_t CCSIDR_EL1;        //Current Cache Size ID Register
    CLIDR_EL1_reg_t CLIDR_EL1;          //Cache Level ID Register
    CONTEXTIDR_EL1_reg_t CONTEXTIDR_EL1;        //Context ID Register (EL1)
    CONTEXTIDR_EL2_reg_t CONTEXTIDR_EL2;        //Context ID Register (EL2)
    CPACR_EL1_reg_t CPACR_EL1;          //Architectural Feature Access Control Register
    CPTR_EL2_reg_t CPTR_EL2;            //Architectural Feature Trap Register (EL2)
    CPTR_EL3_reg_t CPTR_EL3;            //Architectural Feature Trap Register (EL3)
    ARMdword CSSELR_EL1;                //Cache Size Selection Register
    ARMdword CTR_EL0;                   //Cache Type Register
    ARMdword DACR32_EL2;                //Domain Access Control Register
    ARMdword DCZID_EL0;                 //Data Cache Zero ID register
    ESR_EL1_reg_t ESR_EL1;              //Exception Syndrome Register (EL1)
    ESR_EL2_reg_t ESR_EL2;              //Exception Syndrome Register (EL2)
    ESR_EL3_reg_t ESR_EL3;              //Exception Syndrome Register (EL3)
    FAR_EL1_reg_t FAR_EL1;              //Fault Address Register (EL1)
    FAR_EL2_reg_t FAR_EL2;              //Fault Address Register (EL2)
    ARMdword FAR_EL3;                   //Fault Address Register (EL3)
    FPEXC32_EL2_reg_t FPEXC32_EL2;      //Floating-Point Exception Control register
    ARMdword GCR_EL1;                   //Tag Control Register
    ARMdword GMID_EL1;                  //Multiple tag transfer ID register
    HACR_EL2_reg_t HACR_EL2;            //Hypervisor Auxiliary Control Register
    HCR_EL2_reg_t HCR_EL2;              //Hypervisor Configuration Register
    HPFAR_EL2_reg_t HPFAR_EL2;          //Hypervisor IPA Fault Address Register
    HSTR_EL2_reg_t HSTR_EL2;            //Hypervisor System Trap Register
    ARMdword ID_AA64AFR0_EL1;           // AArch64 Auxiliary Feature Register 0
    ARMdword ID_AA64AFR1_EL1;           // AArch64 Auxiliary Feature Register 1
    ARMdword ID_AA64DFR0_EL1;           // AArch64 Debug Feature Register 0
    ARMdword ID_AA64FFR1_EL1;           // AArch64 Debug Feature Register 1
    ARMdword ID_AA64ISAR0_EL1;          // AArch64 Instruction Set Attribute Register 0
    ARMdword ID_AA64ISAR1_EL1;          // AArch64 Instruction Set Attribute Register 1
    ARMdword ID_AA64MMFR0_EL1;          // AArch64 Instruction Set Attribute Register 0
    ARMdword ID_AA64MMFR1_EL1;          // AArch64 Instruction Set Attribute Register 1
    ARMdword ID_AA64MMFR2_EL1;          // AArch64 Instruction Set Attribute Register 1
    ARMdword ID_AA64PFR0_EL1;           // AArch64 Processor Feature Register 0
    ARMdword ID_AA64PFR1_EL1;           // AArch64 Processor Feature Register 1
    //ID registers
    ID_AFR0_reg_t ID_AFR0_EL1;          //AArch32 Auxiliary Feature Register 0
    ID_DFR0_reg_t ID_DFR0_EL1;          //AArch32 Debug Feature Register 0
    ID_ISAR0_reg_t ID_ISAR0_EL1;        //AArch32 Instruction Set Attribute Register 0
    ID_ISAR1_reg_t ID_ISAR1_EL1;        //AArch32 Instruction Set Attribute Register 1
    ID_ISAR2_reg_t ID_ISAR2_EL1;        //AArch32 Instruction Set Attribute Register 2
    ID_ISAR3_reg_t ID_ISAR3_EL1;        //AArch32 Instruction Set Attribute Register 3
    ID_ISAR4_reg_t ID_ISAR4_EL1;        //AArch32 Instruction Set Attribute Register 4
    ID_ISAR5_reg_t ID_ISAR5_EL1;        //AArch32 Instruction Set Attribute Register 4
    ID_ISAR6_reg_t ID_ISAR6_EL1;        //AArch32 Instruction Set Attribute Register 4
    ID_ISAR6_reg_t ID_MMFR0_EL1;        //AArch32 Memory Model Feature Register 0
    ID_ISAR6_reg_t ID_MMFR1_EL1;        //AArch32 Memory Model Feature Register 1
    ID_ISAR6_reg_t ID_MMFR2_EL1;        //AArch32 Memory Model Feature Register 2
    ID_ISAR6_reg_t ID_MMFR3_EL1;        //AArch32 Memory Model Feature Register 3
    ID_ISAR6_reg_t ID_MMFR4_EL1;        //AArch32 Memory Model Feature Register 4
    ID_PFR0_EL1_reg_t ID_PFR0_EL1;      //AArch32 Processor Feature Register 0
    ID_PFR1_EL1_reg_t ID_PFR1_EL1;      //AArch32 Processor Feature Register 1
    ID_PFR2_EL1_reg_t ID_PFR2_EL1;      //AArch32 Processor Feature Register 2
    IFSR32_EL2_reg_t IFSR32_EL2;        //Instruction Fault Status Register (EL2)
    ISR_EL1_reg_t ISR_EL1;              //Interrupt Status Register
    ARMdword LORC_EL1;                  // LORegion Control (EL1)
    ARMdword LOREA_EL1;                 // LORegion End Address (EL1)
    ARMdword LORID_EL1;                 // LORegionID (EL1)
    ARMdword LORN_EL1;                  // LORegion Number (EL1)
    ARMdword LORSA_EL1;                 // LORegion Start Address (EL1)
    MAIR_reg_t MAIR_EL1;                  // Memory Attribute Indirection Register (EL1)
    MAIR_reg_t MAIR_EL2;                  // Memory Attribute Indirection Register (EL2)
    MAIR_reg_t MAIR_EL3;                  // Memory Attribute Indirection Register (EL3)
    ARMdword MIDR_EL1;                  // Main ID Register
    ARMdword MPIDR_EL1;                 // Multiprocessor Affinity Register
    ARMdword MVFR0_EL1;                 // AArch32 Media and VFP Feature Register 0
    ARMdword MVFR1_EL1;                 // AArch32 Media and VFP Feature Register 1
    ARMdword MVFR2_EL1;                 // AArch32 Media and VFP Feature Register 2
    ARMdword PAR_EL1;                   // Physical Address Register
    ARMdword REVIDR_EL1;                // Revision ID Register
    ARMdword RGSR_EL1;                  // Random Allocation Tag Seed Register
    ARMdword RMR_EL1;                   // Reset Management Register (EL1)
    ARMdword RMR_EL2;                   // Reset Management Register (EL2)
    ARMdword RMR_EL3;                   // Reset Management Register (EL3)
    ARMdword RNDR;                      // Random Number
    ARMdword RNDRRS;                    // Reseeded Random Number
    ARMdword RVBAR_EL1;                 // Reset Vector Base Address Register (if EL2 and EL3 not implemented)
    ARMdword RVBAR_EL2;                 // Reset Vector Base Address Register (if EL3 not implemented)
    SCR_EL3_reg_t SCR_EL3;              // Secure Configuration Register
    SCTLR_reg_t SCTLR_EL1;              // System Control Register (EL1)
    SCTLR_reg_t SCTLR_EL2;              // System Control Register (EL2)
    SCTLR_reg_t SCTLR_EL3;              // System Control Register (EL3)
    ARMdword SCXTNUM_EL0;               // EL0 Read/Write Software Context Number
    ARMdword SCXTNUM_EL1;               // EL1 Read/Write Software Context Number
    ARMdword SCXTNUM_EL2;               // EL2 Read/Write Software Context Number
    ARMdword SCXTNUM_EL3;               // EL3 Read/Write Software Context Number
    TCR_EL1_reg_t TCR_EL1;                   // Translation Control Register (EL1)
    TCR_reg_t TCR_EL2;                  // Translation Control Register (EL2)
    ARMdword TCR_EL3;                   // Translation Control Register (EL3)
    ARMdword TFSRE0_EL1;                // Tag Fail Status Register (EL0)
    ARMdword TFSR_EL1;                  // Tag Fail Status Register (EL1)
    ARMdword TFSR_EL2;                  // Tag Fail Status Register (EL2)
    ARMdword TFSR_EL3;                  // Tag Fail Status Register (EL3)
    ARMdword TPIDR_EL0;                 // EL0 Read/Write Software Thread ID Register
    ARMdword TPIDR_EL1;                 // EL1 Read/Write Software Thread ID Register
    ARMdword TPIDR_EL2;                 // EL2 Read/Write Software Thread ID Register
    ARMdword TPIDR_EL3;                 // EL3 Read/Write Software Thread ID Register
    ARMdword TPIDRRO_EL0;               // EL0 Read-Only Software Thread ID Register
    ARMdword TTBR0_EL1;                 // Translation Table Base Register 0 (EL1)
    ARMdword TTBR0_EL2;                 // Translation Table Base Register 0 (EL2)
    ARMdword TTBR0_EL3;                 // Translation Table Base Register 0 (EL3)
    ARMdword TTBR1_EL1;                 // Translation Table Base Register 1 (EL1)
    ARMdword TTBR1_EL2;                 // Translation Table Base Register 1 (EL2)
    ARMdword VBAR_EL1;                  // Vector Base Address Register (EL1)
    ARMdword VBAR_EL2;                  // Vector Base Address Register (EL2)
    ARMdword VBAR_EL3;                  // Vector Base Address Register (EL3)
    ARMdword VMPIDR_EL2;                // Virtualization Multiprocessor ID Register
    ARMdword VNCR_EL2;                  // Virtual Nested Control Register
    ARMdword VPIDR_EL2;                 // Virtualization Processor ID Register
    ARMdword VSTCR_EL2;                 // Virtualization Secure Translation Control Register
    ARMdword VSTTBR_EL2;                // Virtualization Secure Translation Table Base Register
    ARMdword VTCR_EL2;                  // Virtualization Translation Control Register
    ARMdword VTTBR_EL2;                 // Virtualization Translation Table Base Register
    //Debug registers
    ARMdword S3_C15_C1;
    MDSCR_reg_t MDSCR_EL1;
    ARMdword CNTFRQ_EL0;                //Clock frequency
    ARMdword CNTKCTL_EL1;               // Counter-timer Kernel Control register
    CNTP_CTL_reg_t CNTP_CTL_EL0;        //Counter-timer Physical Timer Control register
    ARMdword CNTP_CVAL_EL0;             //Counter-timer Physical Timer CompareValue register
    ARMdword CNTP_TVAL_EL0;             //Counter-timer Physical Timer TimerValue register
    ARMdword CNTPCT_EL0;                //Counter-timer Physical Count
    CNTP_CTL_reg_t CNTV_CTL_EL0;        //Counter-timer Virtual  Timer Control register
    ARMdword CNTV_CVAL_EL0;             //Counter-timer Virtual  Timer CompareValue register
    ARMdword CNTV_TVAL_EL0;             //Counter-timer Virtual  Timer TimerValue register
    ARMdword CNTVCT_EL0;                //Counter-timer Virtual Count register
    ARMdword DFAR;
    ARMdword DFSR;
} SystemRegister_t;

typedef union
{
    uint64_t all;
    struct
    {
        uint8_t STATUS:6;
        uint8_t ERR:1;
        uint8_t A:1;
        uint8_t EL:2;
        uint8_t RW:4;
        uint8_t HDE:1;
        uint8_t RES0:1;
        uint8_t SDD:1;
        uint8_t RES1:1;
        uint8_t NS:1;
        uint8_t SC2:1;
        uint8_t MA:1;
        uint8_t TDA:1;
        uint8_t INTdis:2;
        uint8_t ITE:1;
        uint8_t PipeAdv:1;
        uint8_t TXU:1;
        uint8_t RXO:1;
        uint8_t ITO:1;
        uint8_t RXfull:1;
        uint8_t TXfull:1;
        uint8_t TFO:1;
    };
} EDSCR_reg_t;

typedef struct
{
    ARMdword DBGAUTHSTATUS_EL1;
    ARMdword DEGBCR_EL1[16];
    ARMdword DEGBVR_EL1[16];
    ARMdword DBGCLAIMCLR_EL1;
    ARMdword DBGCLAIMSET_EL1;
    ARMdword DBGDTRRX_EL0;
    ARMdword DBGDTRTX_EL0;
    ARMdword DBGWCR_EL1[16];
    ARMdword DBGWVR_EL1[16];
    ARMdword EDAA32PFR;
    ARMdword EDACR;
    ARMdword EDCIDR0;
    ARMdword EDCIDR1;
    ARMdword EDCIDR2;
    ARMdword EDCIDR3;
    ARMdword EDCIDSR;
    ARMdword EDDEVAFF0;
    ARMdword EDDEVAFF1;
    ARMdword EDDEVARCH;
    ARMdword EDDEVID;
    ARMdword EDDEVID1;
    ARMdword EDDEVID2;
    ARMdword EDDEVTYPE;
    ARMdword EDDFR;
    ARMdword EDECCR;
    ARMdword EDECR;
    ARMdword EDESR;
    ARMdword EDITCTRL;
    ARMdword EDITR;
    ARMdword EDLAR;
    ARMdword EDLSR;
    ARMdword EDPCSR;
    ARMdword EDPFR;
    ARMdword EDPIDR0;
    ARMdword EDPIDR1;
    ARMdword EDPIDR2;
    ARMdword EDPIDR3;
    ARMdword EDPIDR4;
    ARMdword EDPRCR;
    ARMdword EDPRSR;
    ARMdword EDRCR;
    EDSCR_reg_t EDSCR;
    ARMdword EDVIDSR;
    ARMdword EDWAR;
    ARMdword MIDR_EL1;
    ARMdword OSLAR_EL1;
    ARMdword DBGDTR_EL0;
    ARMdword DBGPRCR_EL1;
    ARMdword DBGVCR32_EL2;
    ARMdword DLR_EL0;
    ARMdword DSPSR_EL0;
    ARMdword MDCCINT_EL1;
    ARMdword MDCCSR_EL0;
    ARMdword MDCR_EL2;
    ARMdword MDCR_EL3;
    ARMdword MDRAR_EL1;
    ARMdword OSDLR_EL1;
    ARMdword OSDTRRX_EL1;
    ARMdword OSDTRTX_EL1;
    ARMdword OSECCR_EL1;
    ARMdword OSLSR_EL1;
    ARMdword SDER32_EL2;
    ARMdword SDER32_EL3;
    ARMdword TRFCR_EL1;
    ARMdword TRFCR_EL2;
} DebugRegister_t;

enum SystemRegister_PEState_Map
{
    System_CurrentEL = 0,
    System_DAIF,
    System_DIT,
    System_NZCV,
    System_PAN,
    System_SPSel,
    System_SSBS,
    System_TCO,
    System_UAO,
    System_MAX_REG
};

struct cortex_m_system_control_registers
{
    uint32_t ICT;
    uint32_t SYSTICK_CS;
    uint32_t SYSTICK_RL;
    uint32_t SYSTICK_CUR;
    uint32_t SYSTICK_CAL;
    uint32_t NVIC_ISER0;
    uint32_t NVIC_ICER0;
    uint32_t NVIC_ISPR0;
    uint32_t NVIC_ICPR0;
    uint32_t NVIC_IABR0;
    uint32_t NVIC_IPR0;
    uint32_t NVIC_IPR1;
    uint32_t NVIC_IPR2;
    uint32_t NVIC_IPR3;
    uint32_t CPUID;
    uint32_t ICSR;
    uint32_t VTOR;
    uint32_t AIRCR;
    uint32_t SCR;
    uint32_t CCR;
    uint32_t SHPR1;
    uint32_t SHPR2;
    uint32_t SHPR3;
    uint32_t SHCSR;
    uint32_t CFSR;
    uint32_t HFSR;
    uint32_t DFSR;
    uint32_t MMFAR;
    uint32_t BFAR;
    uint32_t AFSR;
};

#define VFP_REG_NUM 64
typedef uint32_t(*is_io_space_t) (generic_address_t addr);
struct ARMul_State
{
    ARMword Emulate;                    /* to start and stop emulation */
    unsigned EndCondition;              /* reason for stopping */
    unsigned ErrorCode;                 /* type of illegal instruction */

    /* Order of the following register should not be modified */
    ARMword Reg[16];                    /* the current register file */
    ARMword Cpsr;                       /* the current psr */
    ARMword Spsr_copy;
    ARMword phys_pc;
    ARMword Reg_usr[7];                 /* R8---R14 USER REG bankup */
    ARMword Reg_svc[2];                 /* R13_SVC R14_SVC */
    ARMword Reg_abort[2];               /* R13_ABORT R14_ABORT */
    ARMword Reg_undef[2];               /* R13 UNDEF R14 UNDEF */
    ARMword Reg_irq[2];                 /* R13_IRQ R14_IRQ */
    ARMword Reg_firq[7];                /* R8---R14 FIRQ */
    ARMword Spsr[7];                    /* the exception psr's */
    ARMword Mode;                       /* the current mode */
    ARMword Bank;                       /* the current register bank */
    ARMword exclusive_tag;
    ARMword exclusive_state;
    ARMword exclusive_result;
    ARMword CP15[VFP_BASE - CP15_BASE];
    ARMword VFP[3];                     /* FPSID, FPSCR, and FPEXC */
    int vector_remap_flag;
    uint32_t vector_remap_addr;
    /* VFPv2 and VFPv3-D16 has 16 doubleword registers (D0-D16 or S0-S31).
     * VFPv3-D32/ASIMD may have up to 32 doubleword registers (D0-D31),
     * and only 32 singleword registers are accessible (S0-S31). */
    ARMword ExtReg[VFP_REG_NUM];
    /* ---- End of the ordered registers ---- */

    ARMword RegBank[7][16];             /* all the registers */
    //chy:2003-08-19, used in arm xscale
    /* 40 bit accumulator.  We always keep this 64 bits wide,
     * and move only 40 bits out of it in an MRA insn.  */
    ARMdword Accumulator;

    ARMword NFlag, ZFlag, CFlag, VFlag, IFFlags;        /* dummy flags for speed */
    unsigned long long int icounter, debug_icounter, kernel_icounter;
    unsigned int shifter_carry_out;
    //ARMword translate_pc;

    /* add armv6 flags dyf:2010-08-09 */
    ARMword GEFlag, EFlag, AFlag, QFlags;
    //chy:2003-08-19, used in arm v5e|xscale
    ARMword SFlag;
#ifdef MODET
    ARMword TFlag;                      /* Thumb state */
#endif
    ARMword instr, pc, temp;            /* saved register state */
    ARMword loaded, decoded;            /* saved pipeline state */
    //chy 2006-04-12 for ICE breakpoint
    ARMword loaded_addr, decoded_addr;  /* saved pipeline state addr */
    unsigned int NumScycles, NumNcycles, NumIcycles, NumCcycles, NumFcycles;    /* emulated cycles used */
    unsigned long long NumInstrs;       /* the number of instructions executed */
    unsigned NextInstr;
    unsigned VectorCatch;               /* caught exception mask */
    unsigned CallDebug;                 /* set to call the debugger */
    unsigned CanWatch;                  /* set by memory interface if its willing to suffer the
                                         * overhead of checking for watchpoints on each memory
                                         * access */
    unsigned int StopHandle;

    char *CommandLine;                  /* Command Line from ARMsd */

    ARMul_CPInits *CPInit[16];          /* coprocessor initialisers */
    ARMul_CPExits *CPExit[16];          /* coprocessor finalisers */
    ARMul_LDCs *LDC[16];                /* LDC instruction */
    ARMul_STCs *STC[16];                /* STC instruction */
    ARMul_MRCs *MRC[16];                /* MRC instruction */
    ARMul_MCRs *MCR[16];                /* MCR instruction */
    ARMul_MRRCs *MRRC[16];              /* MRRC instruction */
    ARMul_MCRRs *MCRR[16];              /* MCRR instruction */
    ARMul_CDPs *CDP[16];                /* CDP instruction */
    ARMul_CPReads *CPRead[16];          /* Read CP register */
    ARMul_CPWrites *CPWrite[16];        /* Write CP register */
    unsigned char *CPData[16];          /* Coprocessor data */
    unsigned char const *CPRegWords[16];        /* map of coprocessor register sizes */

    unsigned EventSet;                  /* the number of events in the queue */
    unsigned int Now;                   /* time to the nearest cycle */
    struct EventNode **EventPtr;        /* the event list */

    unsigned Debug;                     /* show instructions as they are executed */
    unsigned NresetSig;                 /* reset the processor */
    unsigned NfiqSig;
    unsigned NirqSig;

    unsigned abortSig;
    unsigned NtransSig;
    unsigned bigendSig;
    unsigned prog32Sig;
    unsigned data32Sig;
    unsigned syscallSig;

    /* 2004-05-09 chy
     * ----------------------------------------------------------
     * read ARM Architecture Reference Manual
     * 2.6.5 Data Abort
     * There are three Abort Model in ARM arch.
     *
     * Early Abort Model: used in some ARMv3 and earlier implementations. In this
     * model, base register wirteback occurred for LDC,LDM,STC,STM instructions, and
     * the base register was unchanged for all other instructions. (oldest)
     *
     * Base Restored Abort Model: If a Data Abort occurs in an instruction which
     * specifies base register writeback, the value in the base register is
     * unchanged. (strongarm, xscale)
     *
     * Base Updated Abort Model: If a Data Abort occurs in an instruction which
     * specifies base register writeback, the base register writeback still occurs.
     * (arm720T)
     *
     * read PART B
     * chap2 The System Control Coprocessor  CP15
     * 2.4 Register1:control register
     * L(bit 6): in some ARMv3 and earlier implementations, the abort model of the
     * processor could be configured:
     * 0=early Abort Model Selected(now obsolete)
     * 1=Late Abort Model selceted(same as Base Updated Abort Model)
     *
     * on later processors, this bit reads as 1 and ignores writes.
     * -------------------------------------------------------------
     * So, if lateabtSig=1, then it means Late Abort Model(Base Updated Abort Model)
     * if lateabtSig=0, then it means Base Restored Abort Model
     */
    unsigned lateabtSig;

    ARMword Vector;                     /* synthesize aborts in cycle modes */
    ARMword Aborted;                    /* sticky flag for aborts */
    ARMword Reseted;                    /* sticky flag for Reset */
    ARMword Inted, LastInted;           /* sticky flags for interrupts */
    ARMword Base;                       /* extra hand for base writeback */
    ARMword AbortAddr;                  /* to keep track of Prefetch aborts */

    const struct Dbg_HostosInterface *hostif;

    int verbose;                        /* non-zero means print various messages like the banner */

    mmu_state_t mmu;
    int mmu_inited;
    //mem_state_t mem;
    /*remove io_state to skyeye_mach_*.c files */
    //io_state_t io;
    /* point to a interrupt pending register. now for skyeye-ne2k.c
     * later should move somewhere. e.g machine_config_t*/

    mpu_state_t mpu;

    //chy: 2003-08-11, for different arm core type
    unsigned is_v4;                     /* Are we emulating a v4 architecture (or higher) ?  */
    unsigned is_v5;                     /* Are we emulating a v5 architecture ?  */
    unsigned is_v5e;                    /* Are we emulating a v5e architecture ?  */
    unsigned is_v6;                     /* Are we emulating a v6 architecture ?  */
    unsigned is_v7;                     /* Are we emulating a v7 architecture ?  */
    unsigned is_XScale;                 /* Are we emulating an XScale architecture ?  */
    unsigned is_iWMMXt;                 /* Are we emulating an iWMMXt co-processor ?  */
    unsigned is_ep9312;                 /* Are we emulating a Cirrus Maverick co-processor ?  */
    //chy 2005-09-19
    unsigned is_pxa27x;                 /* Are we emulating a Intel PXA27x co-processor ?  */
    unsigned is_v6m;                    /* Are we emulating a v6m architecture ?  */
    unsigned is_v8;                     /* Are we emulating a v8 architecture ?  */
    //chy: seems only used in xscale's CP14
    unsigned int LastTime;              /* Value of last call to ARMul_Time() */
    ARMword CP14R0_CCD;                 /* used to count 64 clock cycles with CP14 R0 bit 3 set */

    //added by ksh:for handle different machs io 2004-3-5
    ARMul_io mach_io;

    /*added by ksh,2004-11-26,some energy profiling */
    ARMul_Energy energy;

    //teawater add for next_dis 2004.10.27-----------------------
    int disassemble;
    //AJ2D------------------------------------------

    //teawater add for arm2x86 2005.02.15-------------------------------------------
    uint32_t trap;
    uint32_t tea_break_addr;
    uint32_t tea_break_ok;
    int tea_pc;
    //AJ2D--------------------------------------------------------------------------
    //teawater add for arm2x86 2005.07.03-------------------------------------------

    /*
     * 2007-01-24 removed the term-io functions by Anthony Lee,
     * moved to "device/uart/skyeye_uart_stdio.c".
     */

    //AJ2D--------------------------------------------------------------------------
    //teawater add for arm2x86 2005.07.05-------------------------------------------
    //arm_arm A2-18
    int abort_model;                    //0 Base Restored Abort Model, 1 the Early Abort Model, 2 Base Updated Abort Model
    //AJ2D--------------------------------------------------------------------------
    //teawater change for return if running tb dirty 2005.07.09---------------------
    void *tb_now;
    //AJ2D--------------------------------------------------------------------------

    //teawater add for record reg value to ./reg.txt 2005.07.10---------------------
    FILE *tea_reg_fd;
    //AJ2D--------------------------------------------------------------------------

    /*added by ksh in 2005-10-1 */
    cpu_config_t *cpu;
    //mem_config_t *mem_bank;

    /* added LPC remap function */
    uint32_t vector_remap_size;

    uint64_t step;
    uint64_t stop_step;
    uint32_t cycle;
    int stop_simulator;
    conf_object_t *dyncom_cpu;
    //      FILE * state_log;
    //diff log
    //#if DIFF_STATE
    FILE *state_log;
    //#endif
    /* monitored memory for exclusice access */
    ARMword exclusive_tag_array[128];
    /* 1 means exclusive access and 0 means open access */
    ARMword exclusive_access_state;

    memory_space_intf space;
    addr_space_t *bus_space;
    uint32 CurrInstr;
    uint32 last_pc;                     /* the last pc executed */
    uint32 last_instr;                  /* the last inst executed */
    uint32 WriteAddr[17];
    uint32 WriteData[17];
    uint32 WritePc[17];
    uint32 CurrWrite;
    uint8 *RAM;
    conf_object_t *cpu_obj;
    conf_object_t *memory_space;
    memory_space_intf *memory_space_iface;
    memory_space_intf *arm11_memory_iface;
    core_running_mode_t mode;
    is_io_space_t is_io_space;
    breakpoint_mgt_t bp_manager;

    uint32_t M0_exception;              //ISR_number
    ARMword PSP;
    ARMword MSP;
    ARMword PRIMASK;                    //Interrupt mask register
    /* Thumb-2 conditional execution bits.  */
    ARMword condexec_cond;
    ARMword condexec_mask;
    ARMword inITblock;

    it_block_insn_t it_block;
    //ARMV8 AArch64 Registers
    general_reg_t R[31];                //31 general-purpose register, R0 to R30. A 64-bit general-purpose register named X0 to X30. A 32-bit general-purpose register named W0 to W30. X30 is used as call link register
    ARMdword A64_SP;                    //A 64-bit dedicated Stack Pointer register
    ProcState_reg_t PSTATE;             //
    ARMdword ZR;                        //Reading the zero register ZR, accessed as X[31]
    ARMdword A64_PC;
    ARMdword A64_phys_pc;
    simd_fp_reg_t V[32];                //32 SIMD&FP registers
    //Special-purpose registers
    SystemRegister_t system_reg;
    DebugRegister_t debug_reg;
    ARMword stack_address;
    ARMword gic_interface_base_addr;
    ARMword gic_interface_length;
    gic_interface_intf *gic_iface;
    conf_object_t *gic;
    ARMword cpuID;
    ARMword realPC;
    ARMword isAArch64;
    ARMword translate_type;
    uint32_t M3_exception;              //ISR_number
    struct cortex_m_system_control_registers scb_reg;

    multcore_event_signal_intf *event_iface;
    conf_object_t *event;
};

typedef enum
{
    arch32_short_translate = 0,
    arch32_long_translate = 1,
    arch64_translate = 2
} translateType_t;

#define DIFF_WRITE 0
typedef ARMul_State arm_core_t;

#define ResetPin NresetSig
#define FIQPin NfiqSig
#define IRQPin NirqSig
#define AbortPin abortSig
#define TransPin NtransSig
#define BigEndPin bigendSig
#define Prog32Pin prog32Sig
#define Data32Pin data32Sig
#define LateAbortPin lateabtSig

/***************************************************************************\
*                        Types of ARM we know about                         *
\***************************************************************************/

/* The bitflags */
#define ARM_Fix26_Prop   0x01
#define ARM_Nexec_Prop   0x02
#define ARM_Debug_Prop   0x10
#define ARM_Isync_Prop   ARM_Debug_Prop
#define ARM_Lock_Prop    0x20
//chy 2003-08-11
#define ARM_v4_Prop      0x40
#define ARM_v5_Prop      0x80
/*jeff.du 2010-08-05 */
#define ARM_v6_Prop      0xc0

#define ARM_v5e_Prop     0x100
#define ARM_XScale_Prop  0x200
#define ARM_ep9312_Prop  0x400
#define ARM_iWMMXt_Prop  0x800
//chy 2005-09-19
#define ARM_PXA27X_Prop  0x1000
#define ARM_v7_Prop      0x2000
#define ARM_v8_Prop      0x4000

/* ARM2 family */
#define ARM2    (ARM_Fix26_Prop)
#define ARM2as  ARM2
#define ARM61   ARM2
#define ARM3    ARM2

#ifdef ARM60                            /* previous definition in armopts.h */
#undef ARM60
#endif

/* ARM6 family */
#define ARM6    (ARM_Lock_Prop)
#define ARM60   ARM6
#define ARM600  ARM6
#define ARM610  ARM6
#define ARM620  ARM6

/***************************************************************************\
*                   Macros to extract instruction fields                    *
\***************************************************************************/

#define BIT(n) ( (ARMword)(instr>>(n))&1)       /* bit n of instruction */
#define BITS(m,n) ( (ARMword)(instr<<(31-(n))) >> ((31-(n))+(m)) )      /* bits m to n of instr */
#define TOPBITS(n) (instr >> (n))       /* bits 31 to n of instr */

/***************************************************************************\
*                        Macros to extract reg fields                      *
\***************************************************************************/

#define REG32_BIT(reg,n) (( (ARMword)(reg)>>(n))&0x1)      /* bit n of reg64 */
#define REG64_BIT(reg,n) (((ARMdword)(reg)>>(n))&0x1)      /* bit n of reg64 */

#define REG32_BITS(reg,m,n) (  (ARMword)((reg)<<(31-(n))) >> ((31-(n))+(m)) )      /* bits m to n of reg32 */
#define REG64_BITS(reg,m,n) ( (ARMdword)((reg)<<(63-(n))) >> ((63-(n))+(m)) )      /* bits m to n of reg64 */

/***************************************************************************\
*                      The hardware vector addresses                        *
\***************************************************************************/

#define ARMResetV 0L
#define ARMUndefinedInstrV 4L
#define ARMSWIV 8L
#define ARMPrefetchAbortV 12L
#define ARMDataAbortV 16L
#define ARMAddrExceptnV 20L
#define ARMIRQV 24L
#define ARMFIQV 28L
#define ARMErrorV 32L                   /* This is an offset, not an address ! */

#define ARMul_ResetV ARMResetV
#define ARMul_UndefinedInstrV ARMUndefinedInstrV
#define ARMul_SWIV ARMSWIV
#define ARMul_PrefetchAbortV ARMPrefetchAbortV
#define ARMul_DataAbortV ARMDataAbortV
#define ARMul_AddrExceptnV ARMAddrExceptnV
#define ARMul_IRQV ARMIRQV
#define ARMul_FIQV ARMFIQV

/***************************************************************************\
*                          Mode and Bank Constants                          *
\***************************************************************************/

#define USER26MODE 0L
#define FIQ26MODE 1L
#define IRQ26MODE 2L
#define SVC26MODE 3L
#define USER32MODE 16L
#define FIQ32MODE 17L
#define IRQ32MODE 18L
#define SVC32MODE 19L
#define ABORT32MODE 23L
#define UNDEF32MODE 27L
//chy 2006-02-15 add system32 mode
#define SYSTEM32MODE 31L

#define ARM32BITMODE (state->Mode > 3)
#define ARM26BITMODE (state->Mode <= 3)
#define ARMMODE (state->Mode)
#define ARMul_MODEBITS 0x1fL
#define ARMul_MODE32BIT ARM32BITMODE
#define ARMul_MODE26BIT ARM26BITMODE

#define USERBANK 0
#define FIQBANK 1
#define IRQBANK 2
#define SVCBANK 3
#define ABORTBANK 4
#define UNDEFBANK 5
#define DUMMYBANK 6
#define SYSTEMBANK USERBANK
#define BANK_CAN_ACCESS_SPSR(bank)  \
  ((bank) != USERBANK && (bank) != SYSTEMBANK && (bank) != DUMMYBANK)

/***************************************************************************\
*                  Definitons of things in the emulator                     *
\***************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
    extern void ARMul_EmulateInit(void);
    extern void ARMul_Reset(ARMul_State * state);
#ifdef __cplusplus
}
#endif
extern ARMul_State *ARMul_NewState(ARMul_State * state);
extern ARMword ARMul_DoProg(ARMul_State * state);
extern ARMword ARMul_DoInstr(ARMul_State * state);

/***************************************************************************\
*                Definitons of things for event handling                    *
\***************************************************************************/

extern void ARMul_ScheduleEvent(ARMul_State * state, unsigned int delay, unsigned (*func) ());
extern void ARMul_EnvokeEvent(ARMul_State * state);
extern unsigned int ARMul_Time(ARMul_State * state);

/***************************************************************************\
*                          Useful support routines                          *
\***************************************************************************/

extern ARMword ARMul_GetReg(ARMul_State * state, unsigned mode, unsigned reg);
extern void ARMul_SetReg(ARMul_State * state, unsigned mode, unsigned reg, ARMword value);
extern ARMword ARMul_GetPC(ARMul_State * state);
extern ARMword ARMul_GetNextPC(ARMul_State * state);
extern void ARMul_SetPC(ARMul_State * state, ARMword value);
extern ARMword ARMul_GetR15(ARMul_State * state);
extern void ARMul_SetR15(ARMul_State * state, ARMword value);

extern ARMword ARMul_GetCPSR(ARMul_State * state);
extern void ARMul_SetCPSR(ARMul_State * state, ARMword value);
extern ARMword ARMul_GetSPSR(ARMul_State * state, ARMword mode);
extern void ARMul_SetSPSR(ARMul_State * state, ARMword mode, ARMword value);
extern void ARMul_FreeState(ARMul_State * state);

/***************************************************************************\
*                  Definitons of things to handle aborts                    *
\***************************************************************************/

extern void ARMul_Abort(ARMul_State * state, ARMword address);

#ifdef MODET
#define ARMul_ABORTWORD (state->TFlag ? 0xefffdfff : 0xefffffff)        /* SWI -1 */
#define ARMul_PREFETCHABORT(address) if (state->AbortAddr == 1) \
                                        state->AbortAddr = (address & (state->TFlag ? ~1L : ~3L))
#else
#define ARMul_ABORTWORD 0xefffffff      /* SWI -1 */
#define ARMul_PREFETCHABORT(address) if (state->AbortAddr == 1) \
                                        state->AbortAddr = (address & ~3L)
#endif
#define ARMul_DATAABORT(address) state->abortSig = HIGH ; \
                                 state->Aborted = ARMul_DataAbortV ;
#define ARMul_CLEARABORT state->abortSig = LOW

/***************************************************************************\
*              Definitons of things in the memory interface                 *
\***************************************************************************/

extern unsigned ARMul_MemoryInit(ARMul_State * state, unsigned int initmemsize);
extern void ARMul_MemoryExit(ARMul_State * state);

extern ARMword ARMul_LoadInstrS(ARMul_State * state, ARMword address, ARMword isize);
extern ARMword ARMul_LoadInstrN(ARMul_State * state, ARMword address, ARMword isize);
extern ARMword ARMV8_LoadInstrN(ARMul_State * state, ARMdword address, ARMword isize);

#ifdef __cplusplus
extern "C"
{
#endif
    extern ARMword ARMul_ReLoadInstr(ARMul_State * state, ARMword address, ARMword isize);
#ifdef __cplusplus
}
#endif
extern ARMword ARMul_LoadWordS(ARMul_State * state, ARMword address);
extern ARMword ARMul_LoadWordN(ARMul_State * state, ARMword address);
extern ARMword ARMul_LoadHalfWord(ARMul_State * state, ARMword address);
extern ARMword ARMul_LoadByte(ARMul_State * state, ARMword address);

extern void ARMul_StoreWordS(ARMul_State * state, ARMword address, ARMword data);
extern void ARMul_StoreWordN(ARMul_State * state, ARMword address, ARMword data);
extern void ARMul_StoreHalfWord(ARMul_State * state, ARMword address, ARMword data);
extern void ARMul_StoreByte(ARMul_State * state, ARMword address, ARMword data);

extern ARMword ARMul_SwapWord(ARMul_State * state, ARMword address, ARMword data);
extern ARMword ARMul_SwapByte(ARMul_State * state, ARMword address, ARMword data);

extern void ARMul_Icycles(ARMul_State * state, unsigned number, ARMword address);
extern void ARMul_Ccycles(ARMul_State * state, unsigned number, ARMword address);

extern ARMword ARMul_ReadWord(ARMul_State * state, ARMword address);
extern ARMword ARMul_ReadByte(ARMul_State * state, ARMword address);
extern void ARMul_WriteWord(ARMul_State * state, ARMword address, ARMword data);
extern void ARMul_WriteByte(ARMul_State * state, ARMword address, ARMword data);

extern ARMword ARMul_MemAccess(ARMul_State * state, ARMword, ARMword,
                               ARMword, ARMword, ARMword, ARMword, ARMword, ARMword, ARMword, ARMword);

/***************************************************************************\
*            Definitons of things in the co-processor interface             *
\***************************************************************************/

#define ARMul_FIRST 0
#define ARMul_TRANSFER 1
#define ARMul_BUSY 2
#define ARMul_DATA 3
#define ARMul_INTERRUPT 4
#define ARMul_DONE 0
#define ARMul_CANT 1
#define ARMul_INC 3

#define ARMul_CP13_R0_FIQ       0x1
#define ARMul_CP13_R0_IRQ       0x2
#define ARMul_CP13_R8_PMUS      0x1

#define ARMul_CP14_R0_ENABLE    0x0001
#define ARMul_CP14_R0_CLKRST    0x0004
#define ARMul_CP14_R0_CCD       0x0008
#define ARMul_CP14_R0_INTEN0    0x0010
#define ARMul_CP14_R0_INTEN1    0x0020
#define ARMul_CP14_R0_INTEN2    0x0040
#define ARMul_CP14_R0_FLAG0     0x0100
#define ARMul_CP14_R0_FLAG1     0x0200
#define ARMul_CP14_R0_FLAG2     0x0400
#define ARMul_CP14_R10_MOE_IB   0x0004
#define ARMul_CP14_R10_MOE_DB   0x0008
#define ARMul_CP14_R10_MOE_BT   0x000c
#define ARMul_CP15_R1_ENDIAN    0x0080
#define ARMul_CP15_R1_ALIGN     0x0002
#define ARMul_CP15_R5_X         0x0400
#define ARMul_CP15_R5_ST_ALIGN  0x0001
#define ARMul_CP15_R5_IMPRE     0x0406
#define ARMul_CP15_R5_MMU_EXCPT 0x0400
#define ARMul_CP15_DBCON_M      0x0100
#define ARMul_CP15_DBCON_E1     0x000c
#define ARMul_CP15_DBCON_E0     0x0003

extern unsigned ARMul_CoProInit(ARMul_State * state);
extern void ARMul_CoProExit(ARMul_State * state);
extern void ARMul_CoProAttach(ARMul_State * state, unsigned number,
                              ARMul_CPInits * init, ARMul_CPExits * exit,
                              ARMul_LDCs * ldc, ARMul_STCs * stc,
                              ARMul_MRCs * mrc, ARMul_MCRs * mcr,
                              ARMul_MRRCs * mrrc, ARMul_MCRRs * mcrr,
                              ARMul_CDPs * cdp, ARMul_CPReads * read, ARMul_CPWrites * write);
extern void ARMul_CoProDetach(ARMul_State * state, unsigned number);

/***************************************************************************\
*               Definitons of things in the host environment                *
\***************************************************************************/

extern unsigned ARMul_OSInit(ARMul_State * state);
extern void ARMul_OSExit(ARMul_State * state);

#ifdef __cplusplus
extern "C"
{
#endif

    extern unsigned ARMul_OSHandleSWI(ARMul_State * state, ARMword number);
    uint32_t arm_step_once(ARMul_State *state);
#ifdef __cplusplus
}
#endif

extern ARMword ARMul_OSLastErrorP(ARMul_State * state);

extern ARMword ARMul_Debug(ARMul_State * state, ARMword pc, ARMword instr);
extern unsigned ARMul_OSException(ARMul_State * state, ARMword vector, ARMword pc);
extern int rdi_log;

/***************************************************************************\
*                            Host-dependent stuff                           *
\***************************************************************************/

#ifdef macintosh
pascal void SpinCursor(short increment);        /* copied from CursorCtl.h */

# define HOURGLASS           SpinCursor( 1 )
# define HOURGLASS_RATE      1023       /* 2^n - 1 */
#endif

//teawater add for arm2x86 2005.02.14-------------------------------------------
/*ywc 2005-03-31 */
/*
 * #include "arm2x86.h"
 * #include "arm2x86_dp.h"
 * #include "arm2x86_movl.h"
 * #include "arm2x86_psr.h"
 * #include "arm2x86_shift.h"
 * #include "arm2x86_mem.h"
 * #include "arm2x86_mul.h"
 * #include "arm2x86_test.h"
 * #include "arm2x86_other.h"
 * #include "list.h"
 * #include "tb.h"
 */
#define EQ 0
#define NE 1
#define CS 2
#define CC 3
#define MI 4
#define PL 5
#define VS 6
#define VC 7
#define HI 8
#define LS 9
#define GE 10
#define LT 11
#define GT 12
#define LE 13
#define AL 14
#define NV 15

#ifndef NFLAG
#define NFLAG	state->NFlag
#endif //NFLAG

#ifndef ZFLAG
#define ZFLAG	state->ZFlag
#endif //ZFLAG

#ifndef CFLAG
#define CFLAG	state->CFlag
#endif //CFLAG

#ifndef VFLAG
#define VFLAG	state->VFlag
#endif //VFLAG

#ifndef IFLAG
#define IFLAG	(state->IFFlags >> 1)
#endif //IFLAG

#ifndef FFLAG
#define FFLAG	(state->IFFlags & 1)
#endif //FFLAG

#ifndef IFFLAGS
#define IFFLAGS	state->IFFlags
#endif //VFLAG

#define FLAG_MASK	0xf0000000
#define NBIT_SHIFT	31
#define ZBIT_SHIFT	30
#define CBIT_SHIFT	29
#define VBIT_SHIFT	28
#define SKYEYE_OUTREGS(fd) { fprintf ((fd), "R %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,C %x,S %x,%x,%x,%x,%x,%x,%x,M %x,B %x,E %x,I %x,P %x,T %x,L %x,D %x,",\
                         state->Reg[0],state->Reg[1],state->Reg[2],state->Reg[3], \
                         state->Reg[4],state->Reg[5],state->Reg[6],state->Reg[7], \
                         state->Reg[8],state->Reg[9],state->Reg[10],state->Reg[11], \
                         state->Reg[12],state->Reg[13],state->Reg[14],state->Reg[15], \
			 state->Cpsr,   state->Spsr[0], state->Spsr[1], state->Spsr[2],\
                         state->Spsr[3],state->Spsr[4], state->Spsr[5], state->Spsr[6],\
			 state->Mode,state->Bank,state->ErrorCode,state->instr,state->pc,\
			 state->temp,state->loaded,state->decoded);}

#define SKYEYE_OUTMOREREGS(fd) { fprintf ((fd),"\
RUs %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,\
RF  %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,\
RI  %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,\
RS  %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,\
RA  %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,\
RUn %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n",\
                         state->RegBank[0][0],state->RegBank[0][1],state->RegBank[0][2],state->RegBank[0][3], \
                         state->RegBank[0][4],state->RegBank[0][5],state->RegBank[0][6],state->RegBank[0][7], \
                         state->RegBank[0][8],state->RegBank[0][9],state->RegBank[0][10],state->RegBank[0][11], \
                         state->RegBank[0][12],state->RegBank[0][13],state->RegBank[0][14],state->RegBank[0][15], \
                         state->RegBank[1][0],state->RegBank[1][1],state->RegBank[1][2],state->RegBank[1][3], \
                         state->RegBank[1][4],state->RegBank[1][5],state->RegBank[1][6],state->RegBank[1][7], \
                         state->RegBank[1][8],state->RegBank[1][9],state->RegBank[1][10],state->RegBank[1][11], \
                         state->RegBank[1][12],state->RegBank[1][13],state->RegBank[1][14],state->RegBank[1][15], \
                         state->RegBank[2][0],state->RegBank[2][1],state->RegBank[2][2],state->RegBank[2][3], \
                         state->RegBank[2][4],state->RegBank[2][5],state->RegBank[2][6],state->RegBank[2][7], \
                         state->RegBank[2][8],state->RegBank[2][9],state->RegBank[2][10],state->RegBank[2][11], \
                         state->RegBank[2][12],state->RegBank[2][13],state->RegBank[2][14],state->RegBank[2][15], \
                         state->RegBank[3][0],state->RegBank[3][1],state->RegBank[3][2],state->RegBank[3][3], \
                         state->RegBank[3][4],state->RegBank[3][5],state->RegBank[3][6],state->RegBank[3][7], \
                         state->RegBank[3][8],state->RegBank[3][9],state->RegBank[3][10],state->RegBank[3][11], \
                         state->RegBank[3][12],state->RegBank[3][13],state->RegBank[3][14],state->RegBank[3][15], \
                         state->RegBank[4][0],state->RegBank[4][1],state->RegBank[4][2],state->RegBank[4][3], \
                         state->RegBank[4][4],state->RegBank[4][5],state->RegBank[4][6],state->RegBank[4][7], \
                         state->RegBank[4][8],state->RegBank[4][9],state->RegBank[4][10],state->RegBank[4][11], \
                         state->RegBank[4][12],state->RegBank[4][13],state->RegBank[4][14],state->RegBank[4][15], \
                         state->RegBank[5][0],state->RegBank[5][1],state->RegBank[5][2],state->RegBank[5][3], \
                         state->RegBank[5][4],state->RegBank[5][5],state->RegBank[5][6],state->RegBank[5][7], \
                         state->RegBank[5][8],state->RegBank[5][9],state->RegBank[5][10],state->RegBank[5][11], \
                         state->RegBank[5][12],state->RegBank[5][13],state->RegBank[5][14],state->RegBank[5][15] \
		);}

#define SA1110        0x6901b110
#define SA1100        0x4401a100
#define PXA250	      0x69052100
#define PXA270 	      0x69054110
//#define PXA250              0x69052903
// 0x69052903;  //PXA250 B1 from intel 278522-001.pdf

extern void ARMul_UndefInstr(ARMul_State *, ARMword);
extern void ARMul_FixCPSR(ARMul_State *, ARMword, ARMword);
extern void ARMul_FixSPSR(ARMul_State *, ARMword, ARMword);
extern void ARMul_ConsolePrint(ARMul_State *, const char *, ...);
extern void ARMul_SelectProcessor(ARMul_State *, unsigned);

#define DIFF_LOG 0
#define SAVE_LOG 0

void arm_set_pc(ARMul_State * state, generic_address_t pc);
generic_address_t arm_get_pc(ARMul_State * state);
uint64_t arm_get_steps(conf_object_t * conf_obj);
endian_t arm_get_endian(conf_object_t * conf_obj);
align_t arm_get_alignment(conf_object_t * conf_obj);
void arm_reset(conf_object_t * conf_obj);

uint32_t arm_get_regvalue_by_id(ARMul_State * state, uint32_t id);
char *arm_get_regname_by_id(ARMul_State * state, uint32_t id);
exception_t arm_set_regvalue_by_id(ARMul_State * state, uint32_t value, int id);
uint32 arm_get_regid_by_name(ARMul_State * state, char *name);
uint32 arm_get_regnum(ARMul_State * state);

uint64_t arm_arch64_get_regvalue_by_id(ARMul_State * state, uint32_t id);
char *arm_arch64_get_regname_by_id(ARMul_State * state, uint32_t id);
exception_t arm_arch64_set_regvalue_by_id(ARMul_State * state, uint64_t value, uint32_t id);
uint32 arm_arch64_get_regid_by_name(ARMul_State * state, char *name);
uint32 arm_arch64_get_regnum(ARMul_State * state);

uint32_t arm_arch64_step_once(ARMul_State * state);

void arm_run(conf_object_t * obj);
void arm_stop(conf_object_t * obj);
char *arm_get_architecture(conf_object_t * obj);

/*ARM Cortex-M系列处理器系统控制寄存器相关设计*/
#define CPU_SYSTEM_CONTROL_SPACE_BASEADDR 0xE000E000
#define _ICT            4
#define _SYSTICK_CS     0x10
#define _SYSTICK_RL     0x14
#define _SYSTICK_CUR    0x18
#define _SYSTICK_CAL    0x1c
#define _NVIC_ISER0     0x100
#define _NVIC_ICER0     0x180
#define _NVIC_ISPR0     0x200
#define _NVIC_ICPR0     0x280
#define _NVIC_IABR0     0x300
#define _NVIC_IPR0      0x400
#define _NVIC_IPR1      0x404
#define _NVIC_IPR2      0x408
#define _NVIC_IPR3      0x40c
#define _CPUID          0xd00
#define _ICSR           0xd04
#define _VTOR           0xd08
#define _AIRCR          0xd0c
#define _SCR            0xd10
#define _CCR            0xd14
#define _SHPR1          0xd18
#define _SHPR2          0xd1c
#define _SHPR3          0xd20
#define _SHCSR          0xd24
#define _CFSR           0xd28
#define _HFSR           0xd2c
#define _DFSR           0xd30
#define _MMFAR          0xd34
#define _BFAR           0xd38
#define _AFSR           0xd3c

#endif /* _ARMDEFS_H_ */

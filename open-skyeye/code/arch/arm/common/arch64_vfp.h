
/*************************************************************************
  > File Name    : armv8_vfp.h
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/9/12 15:56:22
 ************************************************************************/

#ifndef _ARMV8_VFP_H
#define _ARMV8_VFP_H

#include <skyeye_fp.h>

typedef enum
{
    FPExc_InvalidOp = 0,
    FPExc_DivideByZero,
    FPExc_Overflow,
    FPExc_Underflow,
    FPExc_Inexact,
    FPExc_InputDenorm
} FPExc_type_t;

typedef void AArch64DecodeFn(ARMul_State * state);

typedef struct AArch64DecodeTable
{
    uint32_t pattern;
    uint32_t mask;
    AArch64DecodeFn *disas_fn;
} AArch64DecodeTable;

void disas_fp_3src(ARMul_State * state);
void disas_fp_fixed_conv(ARMul_State * state);
void disas_fp_ccomp(ARMul_State * state);
void disas_fp_2src(ARMul_State * state);
void disas_fp_csel(ARMul_State * state);
void disas_fp_imm(ARMul_State * state);
void disas_fp_compare(ARMul_State * state);
void disas_fp_1src(ARMul_State * state);
void disas_fp_int_conv(ARMul_State * state);

void handle_fp_fcvt(ARMul_State * state, int opcode, int rd, int rn, int dtype, int ntype);
void handle_fp_1src_single(ARMul_State * state, int opcode, int rd, int rn);
void handle_fp_1src_double(ARMul_State * state, int opcode, int rd, int rn);
void handle_fp_1src_half(ARMul_State * state, int opcode, int rd, int rn);
static void single_fp_compare(ARMul_State * state, unsigned int rn, unsigned int rm,
                              bool_t cmp_with_zero, bool_t signal_all_nans);
static void double_fp_compare(ARMul_State * state, unsigned int rn, unsigned int rm,
                              bool_t cmp_with_zero, bool_t signal_all_nans);
static void handle_fp_fcvtzi(ARMul_State * state, ARMword rmode);
static void handle_fp_fcvtpi(ARMul_State * state, ARMword rmode);
static void ftoui(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword rmode);
static void ftosi(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword rmode);
static void fdtoui(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword rmode);
static void fdtosi(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword rmode);

static void handle_fp_scvtf(ARMul_State * state);
static void fsitof(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword sf);
static void fuitof(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword sf);
static void fsitod(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword sf);
static void fuitod(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword sf);

static void handle_fp_2src_single(ARMul_State * state, int opcode);
static void handle_fp_2src_double(ARMul_State * state, int opcode);
static void handle_fp_2src_half(ARMul_State * state, int opcode);
static void FPProcessException(ARMul_State * state, FPExc_type_t FPexc_type);

static void vfp_single_fmax(ARMul_State * state, int rd, int rn, int rm);
static void vfp_single_fmin(ARMul_State * state, int rd, int rn, int rm);
static void vfp_double_fmax(ARMul_State * state, int rd, int rn, int rm);
static void vfp_double_fmin(ARMul_State * state, int rd, int rn, int rm);

static void handle_fmov(ARMul_State * state, bool_t sf, int rd, int rn, int type, bool_t itof);

extern uint32_t vfp_single_fmul(ARMul_State * state, int sd, int sn, int m, uint32_t fpscr);
extern uint32_t vfp_single_fdiv(ARMul_State * state, int sd, int sn, int m, uint32_t fpscr);
extern uint32_t vfp_single_fadd(ARMul_State * state, int sd, int sn, int m, uint32_t fpscr);
extern uint32_t vfp_single_fsub(ARMul_State * state, int sd, int sn, int m, uint32_t fpscr);

extern uint32_t vfp_double_fmul(ARMul_State * state, int dd, int dn, int dm, uint32_t fpscr);
extern uint32_t vfp_double_fdiv(ARMul_State * state, int dd, int dn, int dm, uint32_t fpscr);
extern uint32_t vfp_double_fadd(ARMul_State * state, int dd, int dn, int dm, uint32_t fpscr);
extern uint32_t vfp_double_fsub(ARMul_State * state, int dd, int dn, int dm, uint32_t fpscr);

extern void disas_data_proc_fp(ARMul_State * state);
extern void disas_data_proc_simd(ARMul_State * state);

/*Memory Operation field */
enum MemOp
{
    MO_8 = 0,
    MO_16 = 1,
    MO_32 = 2,
    MO_64 = 3
};
#endif

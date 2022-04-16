
/*************************************************************************
  > File Name    : armv8_vfp_insn.c
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/9/12 15:54:20
 ************************************************************************/

/* FP-specific subcases of table C3-6 (SIMD and FP data processing)
 *   31  30  29 28     25 24                          0
 * +---+---+---+---------+-----------------------------+
 * |   | 0 |   | 1 1 1 1 |                             |
 * +---+---+---+---------+-----------------------------+
 */
#include "arm_regformat.h"
#include "armemu.h"
#include "armos.h"

#include "skyeye_callback.h"
#include "skyeye_bus.h"
#include "skyeye_pref.h"
#include "skyeye.h"
#include "arch64_decode.h"
#include "arch64_exception.h"
#include "skyeye_types.h"
#include "arch64_vfp.h"
#include <math.h>
#include <assert.h>
#include "vfp/vfp_helper.h"
#include "vfp/asm_vfp.h"
#include "armdefs.h"
#include "arch64_helper.h"

/* The imm8 encodes the sign bit, enough bits to represent an exponent in
 * the range 01....1xx to 10....0xx, and the most significant 4 bits of
 * the mantissa; see VFPExpandImm() in the v8 ARM ARM.
 */
uint64_t vfp_expand_imm(int size, uint8_t imm8)
{
    uint64_t imm = 0;

    switch (size)
    {
        case 1:
            //double-precision
            imm = (extract32(imm8, 7, 1) ? 0x8000 : 0) |
                (extract32(imm8, 6, 1) ? 0x3fc0 : 0x4000) | extract32(imm8, 0, 6);
            imm <<= 48;
            break;
        case 0:
            //single-precision
            imm = (extract32(imm8, 7, 1) ? 0x8000 : 0) |
                (extract32(imm8, 6, 1) ? 0x3e00 : 0x4000) | (extract32(imm8, 0, 6) << 3);
            imm <<= 16;
            break;
        case 3:
            //half-precision
            imm = (extract32(imm8, 7, 1) ? 0x8000 : 0) |
                (extract32(imm8, 6, 1) ? 0x3000 : 0x4000) | (extract32(imm8, 0, 6) << 6);
            break;
        default:
            printf("In %s.Line:%d Unallocated Size = %d\n", __func__, __LINE__, size);
            break;
    }
    return imm;
}

static void disas_simd_three_reg_same(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_three_reg_same_extra(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_three_reg_diff(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_two_reg_misc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_across_lanes(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_copy(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_mod_imm(ARMul_State * state)
{
    ARMword insn = state->instr;

    int rd = extract32(insn, 0, 5);
    int cmode = extract32(insn, 12, 4);
    int cmode_3_1 = extract32(cmode, 1, 3);
    int cmode_0 = extract32(cmode, 0, 1);
    int o2 = extract32(insn, 11, 1);
    uint64_t abcdefgh = extract32(insn, 5, 5) | (extract32(insn, 16, 3) << 5);
    bool_t is_neg = extract32(insn, 29, 1);
    bool_t is_q = extract32(insn, 30, 1);
    uint64_t imm = 0;
    int size;

    if (o2 != 0 || ((cmode == 0xf) && is_neg && !is_q))
    {
        /* Check for FMOV (vector, immediate) - half-precision */
        if (o2 && cmode == 0xf)
        {
            printf("In %s.Line:%d.Unallocated Instruction:0x%08x!\n", __func__, __LINE__, insn);
            return;
        }
    }

    /* See AdvSIMDExpandImm() in ARM */
    switch (cmode_3_1)
    {
        case 0:                        /* Replicate(Zeros(24):imm8, 2) */
        case 1:                        /* Replicate(Zeros(16):imm8:Zeros(8), 2) */
        case 2:                        /* Replicate(Zeros(8):imm8:Zeros(16), 2) */
        case 3:                        /* Replicate(imm8:Zeros(24), 2) */
            {
                int shift = cmode_3_1 * 8;

                imm = bitfield_replicate(abcdefgh << shift, 32);
                break;
            }
        case 4:                        /* Replicate(Zeros(8):imm8, 4) */
        case 5:                        /* Replicate(imm8:Zeros(8), 4) */
            {
                int shift = (cmode_3_1 & 0x1) * 8;

                imm = bitfield_replicate(abcdefgh << shift, 16);
                break;
            }
        case 6:
            if (cmode_0)
            {
                /* Replicate(Zeros(8):imm8:Ones(16), 2) */
                imm = (abcdefgh << 16) | 0xffff;
            } else
            {
                /* Replicate(Zeros(16):imm8:Ones(8), 2) */
                imm = (abcdefgh << 8) | 0xff;
            }
            imm = bitfield_replicate(imm, 32);
            break;
        case 7:
            if (!cmode_0 && !is_neg)
            {
                imm = bitfield_replicate(abcdefgh, 8);
            } else if (!cmode_0 && is_neg)
            {
                int i;

                imm = 0;
                for (i = 0; i < 8; i++)
                {
                    if ((abcdefgh) & (1 << i))
                    {
                        imm |= 0xffULL << (i * 8);
                    }
                }
            } else if (cmode_0)
            {
                if (is_neg)
                {
                    imm = (abcdefgh & 0x3f) << 48;
                    if (abcdefgh & 0x80)
                    {
                        imm |= 0x8000000000000000ULL;
                    }
                    if (abcdefgh & 0x40)
                    {
                        imm |= 0x3fc0000000000000ULL;
                    } else
                    {
                        imm |= 0x4000000000000000ULL;
                    }
                } else
                {
                    if (o2)
                    {
                        /* FMOV (vector, immediate) - half-precision */
                        imm = vfp_expand_imm(MO_16, abcdefgh);
                        /* now duplicate across the lanes */
                        imm = bitfield_replicate(imm, 16);
                    } else
                    {
                        imm = (abcdefgh & 0x3f) << 19;
                        if (abcdefgh & 0x80)
                        {
                            imm |= 0x80000000;
                        }
                        if (abcdefgh & 0x40)
                        {
                            imm |= 0x3e000000;
                        } else
                        {
                            imm |= 0x40000000;
                        }
                        imm |= (imm << 32);
                    }
                }
            }
            break;
        default:
            fprintf(stderr, "%s: cmode_3_1: %x\n", __func__, cmode_3_1);
    }

    if (cmode_3_1 != 7 && is_neg)
    {
        imm = ~imm;
    }

    if (!((cmode & 0x9) == 0x1 || (cmode & 0xd) == 0x9))
    {
        /* MOVI or MVNI, with MVNI negation handled above.  */
        WRITE_Vd_REG(rd, imm);
    } else
    {
        /* ORR or BIC, with BIC negation to AND handled above.  */
        uint64_t tmp = READ_Vd_REG(rd);

        if (is_neg)
        {
            imm &= tmp;
        } else
        {
            imm |= tmp;
        }
        WRITE_Vd_REG(rd, imm);
    }
}

static void disas_simd_shift_imm(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_tb(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_zip_trn(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_ext(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_scalar_three_reg_same(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_scalar_three_reg_same_extra(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_scalar_three_reg_diff(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_scalar_two_reg_misc(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_scalar_pairwise(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_scalar_copy(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_indexed(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_scalar_shift_imm(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_crypto_aes(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_crypto_three_reg_sha(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_crypto_two_reg_sha(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_crypto_three_reg_sha512(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_crypto_two_reg_sha512(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_crypto_four_reg(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_crypto_xar(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_crypto_three_reg_imm2(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_three_reg_same_fp16(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_two_reg_misc_fp16(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

static void disas_simd_scalar_three_reg_same_fp16(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

/* C3.6 Data processing - SIMD, inc Crypto
 *
 * As the decode gets a little complex we are using a table based
 * approach for this part of the decode.
 */
static const AArch64DecodeTable data_proc_simd[] = {
    /* pattern  ,  mask     ,  fn                        */
    {0x0e200400, 0x9f200400, disas_simd_three_reg_same},
    {0x0e008400, 0x9f208400, disas_simd_three_reg_same_extra},
    {0x0e200000, 0x9f200c00, disas_simd_three_reg_diff},
    {0x0e200800, 0x9f3e0c00, disas_simd_two_reg_misc},
    {0x0e300800, 0x9f3e0c00, disas_simd_across_lanes},
    {0x0e000400, 0x9fe08400, disas_simd_copy},
    {0x0f000000, 0x9f000400, disas_simd_indexed},       /* vector indexed */
    /* simd_mod_imm decode is a subset of simd_shift_imm, so must precede it */
    {0x0f000400, 0x9ff80400, disas_simd_mod_imm},
    {0x0f000400, 0x9f800400, disas_simd_shift_imm},
    {0x0e000000, 0xbf208c00, disas_simd_tb},
    {0x0e000800, 0xbf208c00, disas_simd_zip_trn},
    {0x2e000000, 0xbf208400, disas_simd_ext},
    {0x5e200400, 0xdf200400, disas_simd_scalar_three_reg_same},
    {0x5e008400, 0xdf208400, disas_simd_scalar_three_reg_same_extra},
    {0x5e200000, 0xdf200c00, disas_simd_scalar_three_reg_diff},
    {0x5e200800, 0xdf3e0c00, disas_simd_scalar_two_reg_misc},
    {0x5e300800, 0xdf3e0c00, disas_simd_scalar_pairwise},
    {0x5e000400, 0xdfe08400, disas_simd_scalar_copy},
    {0x5f000000, 0xdf000400, disas_simd_indexed},       /* scalar indexed */
    {0x5f000400, 0xdf800400, disas_simd_scalar_shift_imm},
    {0x4e280800, 0xff3e0c00, disas_crypto_aes},
    {0x5e000000, 0xff208c00, disas_crypto_three_reg_sha},
    {0x5e280800, 0xff3e0c00, disas_crypto_two_reg_sha},
    {0xce608000, 0xffe0b000, disas_crypto_three_reg_sha512},
    {0xcec08000, 0xfffff000, disas_crypto_two_reg_sha512},
    {0xce000000, 0xff808000, disas_crypto_four_reg},
    {0xce800000, 0xffe00000, disas_crypto_xar},
    {0xce408000, 0xffe0c000, disas_crypto_three_reg_imm2},
    {0x0e400400, 0x9f60c400, disas_simd_three_reg_same_fp16},
    {0x0e780800, 0x8f7e0c00, disas_simd_two_reg_misc_fp16},
    {0x5e400400, 0xdf60c400, disas_simd_scalar_three_reg_same_fp16},
    {0x00000000, 0x00000000, NULL}
};

void handle_fp_3src_single(ARMul_State * state)
{
    ARMword instr = state->instr;
    int rd = BITS(0, 4);
    int rn = BITS(5, 9);
    int ra = BITS(10, 14);
    int rm = BITS(16, 20);
    bool_t o0 = BITS(15, 15);
    bool_t o1 = BITS(21, 21);

    s_precision va_rd, va_rn, va_ra, va_rm;

    va_rn.data = READ_Vs_REG(rn);
    va_rm.data = READ_Vs_REG(rm);
    va_ra.data = READ_Vs_REG(ra);

    if (o1)
        va_ra.value = -va_ra.value;

    if (o1 != o0)
        va_rn.value = -va_rn.value;

    /* These are fused multiply-add */
    va_rd.value = va_rn.value * va_rm.value + va_ra.value;
    WRITE_Vs_REG(rd, va_rd.data);
}

void handle_fp_3src_double(ARMul_State * state)
{
    ARMword instr = state->instr;
    int rd = BITS(0, 4);
    int rn = BITS(5, 9);
    int ra = BITS(10, 14);
    int rm = BITS(16, 20);
    bool_t o0 = BITS(15, 15);
    bool_t o1 = BITS(21, 21);

    d_precision va_rd, va_rn, va_ra, va_rm;

    va_rn.data = READ_Vd_REG(rn);
    va_rm.data = READ_Vd_REG(rm);
    va_ra.data = READ_Vd_REG(ra);

    if (o1)
        va_ra.value = -va_ra.value;

    if (o1 != o0)
        va_rn.value = -va_rn.value;

    /* These are fused multiply-add */
    va_rd.value = va_rn.value * va_rm.value + va_ra.value;
    WRITE_Vd_REG(rd, va_rd.data);
}

void handle_fp_3src_half(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

/* Floating point data-processing (3 source)
 *   31  30  29 28       24 23  22  21  20  16  15  14  10 9    5 4    0
 * +---+---+---+-----------+------+----+------+----+------+------+------+
 * | M | 0 | S | 1 1 1 1 1 | type | o1 |  Rm  | o0 |  Ra  |  Rn  |  Rd  |
 * +---+---+---+-----------+------+----+------+----+------+------+------+
 */

void disas_fp_3src(ARMul_State * state)
{
    ARMword instr = state->instr;
    int type = BITS(22, 23);

    printf("type = %d\n", type);
    switch (type)
    {
        case 0:
            handle_fp_3src_single(state);
            break;
        case 1:
            handle_fp_3src_double(state);
            break;
        case 3:
            handle_fp_3src_half(state);
            break;
        default:
            UNALLOCATED_INSTR;
    }
}

/* Floating point <-> fixed point conversions
 *   31   30  29 28       24 23  22  21 20   19 18    16 15   10 9    5 4    0
 * +----+---+---+-----------+------+---+-------+--------+-------+------+------+
 * | sf | 0 | S | 1 1 1 1 0 | type | 0 | rmode | opcode | scale |  Rn  |  Rd  |
 * +----+---+---+-----------+------+---+-------+--------+-------+------+------+
 */
void disas_fp_fixed_conv(ARMul_State * state)
{

    NOT_IMPLEMENT;
}

/* Floating point conditional compare
 *   31  30  29 28       24 23  22  21 20  16 15  12 11 10 9    5  4   3    0
 * +---+---+---+-----------+------+---+------+------+-----+------+----+------+
 * | M | 0 | S | 1 1 1 1 0 | type | 1 |  Rm  | cond | 0 1 |  Rn  | op | nzcv |
 * +---+---+---+-----------+------+---+------+------+-----+------+----+------+
 */
void disas_fp_ccomp(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

/* Floating point data-processing (2 source)
 *   31  30  29 28       24 23  22  21 20  16 15    12 11 10 9    5 4    0
 * +---+---+---+-----------+------+---+------+--------+-----+------+------+
 * | M | 0 | S | 1 1 1 1 0 | type | 1 |  Rm  | opcode | 1 0 |  Rn  |  Rd  |
 * +---+---+---+-----------+------+---+------+--------+-----+------+------+
 */
void disas_fp_2src(ARMul_State * state)
{
    ARMword instr = state->instr;

    int ftype = BITS(22, 23);
    int opcode = BITS(12, 15);

    if (opcode > 8)
    {
        UNALLOCATED_INSTR;
        return;
    }

    switch (ftype)
    {
        case 0:
            handle_fp_2src_single(state, opcode);
            break;
        case 1:
            handle_fp_2src_double(state, opcode);
            break;
        case 3:
            handle_fp_2src_half(state, opcode);
            break;
        default:
            UNALLOCATED_INSTR;
    }
}

/* Floating point conditional select
 *   31  30  29 28       24 23  22  21 20  16 15  12 11 10 9    5 4    0
 * +---+---+---+-----------+------+---+------+------+-----+------+------+
 * | M | 0 | S | 1 1 1 1 0 | type | 1 |  Rm  | cond | 1 1 |  Rn  |  Rd  |
 * +---+---+---+-----------+------+---+------+------+-----+------+------+
 */
void disas_fp_csel(ARMul_State * state)
{
    NOT_IMPLEMENT;
}

/* Floating point immediate
 *   31  30  29 28       24 23  22  21 20        13 12   10 9    5 4    0
 * +---+---+---+-----------+------+---+------------+-------+------+------+
 * | M | 0 | S | 1 1 1 1 0 | type | 1 |    imm8    | 1 0 0 | imm5 |  Rd  |
 * +---+---+---+-----------+------+---+------------+-------+------+------+
 */
void disas_fp_imm(ARMul_State * state)
{
    ARMword instr = state->instr;
    int rd = BITS(0, 4);
    int imm8 = BITS(13, 20);
    int type = BITS(22, 23);
    int size = type + 2;
    uint64_t imm;
    bit128 result;

    imm = vfp_expand_imm(type, imm8);

    result.lo64 = imm;
    result.hi64 = 0;
    SET_VFP_DATA(size, rd, result);
}

/* Floating point compare
 *   31  30  29 28       24 23  22  21 20  16 15 14 13  10    9    5 4     0
 * +---+---+---+-----------+------+---+------+-----+---------+------+-------+
 * | M | 0 | S | 1 1 1 1 0 | type | 1 |  Rm  | op  | 1 0 0 0 |  Rn  |  op2  |
 * +---+---+---+-----------+------+---+------+-----+---------+------+-------+
 */
void disas_fp_compare(ARMul_State * state)
{
    ARMword instr = state->instr;
    unsigned int mos, type, rm, op, rn, opc, op2r;

    mos = BITS(29, 31);
    type = BITS(22, 23);
    rm = BITS(16, 20);
    op = BITS(14, 15);
    rn = BITS(5, 9);
    opc = BITS(3, 4);
    op2r = BITS(0, 2);

    if (mos || op || op2r)
    {
        UNALLOCATED_INSTR;
        return;
    }

    switch (type)
    {
        case 0:
            single_fp_compare(state, rn, rm, opc & 1, opc & 2);
            break;
        case 1:
            double_fp_compare(state, rn, rm, opc & 1, opc & 2);
            break;
        case 3:
            NOT_IMPLEMENT;
            //half_fp_compare(state, rn, rm, opc & 1, opc & 2);
            break;
    }

}

/* Floating point data-processing (1 source)
 *   31  30  29 28       24 23  22  21 20    15 14       10 9    5 4    0
 * +---+---+---+-----------+------+---+--------+-----------+------+------+
 * | M | 0 | S | 1 1 1 1 0 | type | 1 | opcode | 1 0 0 0 0 |  Rn  |  Rd  |
 * +---+---+---+-----------+------+---+--------+-----------+------+------+
 */
void disas_fp_1src(ARMul_State * state)
{
    ARMword instr = state->instr;
    int type = BITS(22, 23);
    int opcode = BITS(15, 20);
    int rn = BITS(5, 9);
    int rd = BITS(0, 4);

    switch (opcode)
    {
        case 0x4:
        case 0x5:
        case 0x7:
            {
                /* FCVT between half, single and double precision */
                int dtype = opcode & 3;

                if (type == 2 || dtype == type)
                {
                    UNALLOCATED_INSTR;
                    return;
                }
                handle_fp_fcvt(state, opcode, rd, rn, dtype, type);
                break;
            }
        case 0x0 ... 0x3:
        case 0x8 ... 0xc:
        case 0xe ... 0xf:
            /* 32-to-32 and 64-to-64 ops */

            switch (type)
            {
                case 0:
                    handle_fp_1src_single(state, opcode, rd, rn);
                    break;
                case 1:
                    handle_fp_1src_double(state, opcode, rd, rn);
                    break;
                case 3:
                    handle_fp_1src_half(state, opcode, rd, rn);
                    break;
                default:
                    UNALLOCATED_INSTR;
            }
            break;
        default:
            UNALLOCATED_INSTR;
            break;
    }
}

/* Floating point <-> integer conversions
 *   31   30  29 28       24 23  22  21 20   19 18 16 15         10 9  5 4  0
 * +----+---+---+-----------+------+---+-------+-----+-------------+----+----+
 * | sf | 0 | S | 1 1 1 1 0 | type | 1 | rmode | opc | 0 0 0 0 0 0 | Rn | Rd |
 * +----+---+---+-----------+------+---+-------+-----+-------------+----+----+
 */
void disas_fp_int_conv(ARMul_State * state)
{
    ARMword instr = state->instr;
    int rmode = BITS(19, 20);
    int opcode = BITS(16, 18);
    int type = BITS(22, 23);
    int rd = BITS(0, 4);
    int rn = BITS(5, 9);
    bool_t sf = BITS(31, 31);

    if (opcode > 5)
    {
        /* FMOV */
        bool_t itof = opcode & 1;

        if (rmode >= 2)
        {
            UNALLOCATED_INSTR;
            return;
        }

        switch (sf << 3 | type << 1 | rmode)
        {
            case 0x0:                  /* 32 bit */
            case 0xa:                  /* 64 bit */
            case 0xd:                  /* 64 bit to top half of quad */
            case 0x6:                  /* 16-bit float, 32-bit int */
            case 0xe:                  /* 16-bit float, 64-bit int */
                break;
            default:
                /* all other sf/type/rmode combinations are invalid */
                UNALLOCATED_INSTR;
                return;
        }

        handle_fmov(state, sf, rd, rn, type, itof);
    } else
    {
        if (rmode == 3)
        {
            handle_fp_fcvtzi(state, FPSCR_ROUND_TOZERO);
        } else if (rmode == 1)
        {
            handle_fp_fcvtpi(state, FPSCR_ROUND_PLUSINF);
        } else if (rmode == 0)
        {
            handle_fp_scvtf(state);
        } else
        {
            NOT_IMPLEMENT;
        }
    }
}

void handle_fp_fcvt(ARMul_State * state, int opcode, int rd, int rn, int dtype, int ntype)
{
    switch (ntype)
    {
        case 0:
            if (dtype == 1)
            {
                /* Single to double */
                s_precision va_rn;
                d_precision va_rd;

                va_rn.data = READ_Vs_REG(rn);
                va_rd.value = (double) va_rn.value;
                WRITE_Vd_REG(rd, va_rd.data);
            } else
            {
                /* Single to half */
                NOT_IMPLEMENT;
            }
            break;
        case 1:
            if (dtype == 0)
            {
                /* Double to single */
                d_precision va_rn;
                s_precision va_rd;

                va_rn.data = READ_Vd_REG(rn);
                va_rd.value = (float) va_rn.value;
                WRITE_Vs_REG(rd, va_rd.data);
            } else
            {
                /* Double to half */
                NOT_IMPLEMENT;
            }
            break;
        case 3:
            if (dtype == 0)
            {
                /* Half to single */
                NOT_IMPLEMENT;
            } else
            {
                /* Half to double */
                NOT_IMPLEMENT;
            }
            break;
        default:
            NOT_IMPLEMENT;
            break;
    }
}

/* Floating-point data-processing (1 source) - single precision */
void handle_fp_1src_single(ARMul_State * state, int opcode, int rd, int rn)
{
    ARMword instr = state->instr;
    s_precision va_rd, va_rn;

    va_rn.data = READ_Vs_REG(rn);

    switch (opcode)
    {
        case 0x0:                      /* FMOV */
            va_rd.data = va_rn.data;
            break;
        case 0x1:                      /* FABS */
            if (va_rn.value < 0)
                va_rn.value = -va_rn.value;
            va_rd.value = va_rn.value;
            break;
        case 0x2:                      /* FNEG */
            va_rd.value = -va_rn.value;
            break;
        case 0x3:                      /* FSQRT */
            va_rd.value = sqrt(va_rn.value);
            break;
        case 0x8:                      /* FRINTN */
        case 0x9:                      /* FRINTP */
        case 0xa:                      /* FRINTM */
        case 0xb:                      /* FRINTZ */
        case 0xc:                      /* FRINTA */
            NOT_IMPLEMENT;
            break;
        case 0xe:                      /* FRINTX */
            NOT_IMPLEMENT;
            break;
        case 0xf:                      /* FRINTI */
            NOT_IMPLEMENT;
            break;
        default:
            NOT_IMPLEMENT;
    }

    WRITE_Vs_REG(rd, va_rd.data);
}

/* Floating-point data-processing (1 source) - double precision */
void handle_fp_1src_double(ARMul_State * state, int opcode, int rd, int rn)
{
    ARMword instr = state->instr;
    d_precision va_rd, va_rn;

    va_rn.data = READ_Vd_REG(rn);

    switch (opcode)
    {
        case 0x0:                      /* FMOV */
            va_rd.data = va_rn.data;
            break;
        case 0x1:                      /* FABS */
            if (va_rn.value < 0)
                va_rn.value = -va_rn.value;
            va_rd.value = va_rn.value;
            break;
        case 0x2:                      /* FNEG */
            va_rd.value = -va_rn.value;
            break;
        case 0x3:                      /* FSQRT */
            va_rd.value = sqrt(va_rn.value);
            break;
        case 0x8:                      /* FRINTN */
        case 0x9:                      /* FRINTP */
        case 0xa:                      /* FRINTM */
        case 0xb:                      /* FRINTZ */
        case 0xc:                      /* FRINTA */
            NOT_IMPLEMENT;
            break;
        case 0xe:                      /* FRINTX */
            NOT_IMPLEMENT;
            break;
        case 0xf:                      /* FRINTI */
            NOT_IMPLEMENT;
            break;
        default:
            NOT_IMPLEMENT;
    }

    WRITE_Vd_REG(rd, va_rd.data);
}

/* Floating-point data-processing (1 source) - half precision */
void handle_fp_1src_half(ARMul_State * state, int opcode, int rd, int rn)
{
    NOT_IMPLEMENT;
}

static void single_fp_compare(ARMul_State * state, unsigned int rn, unsigned int rm,
                              bool_t cmp_with_zero, bool_t signal_all_nans)
{
    int32_t src_rn, src_rm;
    uint32_t ret = 0;
    struct pstate_nzcv_t pstate_nzcv;

    FPCR_reg_t fpcr = state->system_reg.FPCR;

    src_rn = READ_Vs_REG(rn);
    if (cmp_with_zero)
        src_rm = 0;
    else
        src_rm = READ_Vs_REG(rm);

    if (vfp_single_packed_exponent(src_rm) == 255 && vfp_single_packed_mantissa(src_rm))
    {
        if (signal_all_nans || !(vfp_single_packed_mantissa(src_rm) & (1ULL << (VFP_SINGLE_MANTISSA_BITS - 1))))
        {
            /*
             * Signalling NaN, or signalling on quiet NaN state.nzcv = 0011
             */
            pstate_nzcv.nf = 0;
            pstate_nzcv.zf = 0;
            pstate_nzcv.cf = 1;
            pstate_nzcv.vf = 1;
            NOT_IMPLEMENT;
            //Call Exception
        }
    }

    if (vfp_single_packed_exponent(src_rn) == 255 && vfp_single_packed_mantissa(src_rn))
    {
        ret |= FPSCR_C | FPSCR_V;
        if (signal_all_nans || !(vfp_single_packed_mantissa(src_rn) & (1ULL << (VFP_SINGLE_MANTISSA_BITS - 1))))
        {
            /*
             * Signalling NaN, or signalling on quiet NaN state.nzcv = 0011
             */
            pstate_nzcv.nf = 0;
            pstate_nzcv.zf = 0;
            pstate_nzcv.cf = 1;
            pstate_nzcv.vf = 1;
            NOT_IMPLEMENT;
            //Call Exception
        }
    }

    if (ret == 0)
    {
        //printf("In %s, d=%lld, m =%lld\n ", __FUNCTION__, d, m);
        if (src_rn == src_rm || vfp_single_packed_abs(src_rn | src_rm) == 0)
        {
            /*
             * equal state.nzcv = 0110
             */
            pstate_nzcv.nf = 0;
            pstate_nzcv.zf = 1;
            pstate_nzcv.cf = 1;
            pstate_nzcv.vf = 0;

        } else if (vfp_single_packed_sign(src_rn ^ src_rm))
        {
            /*
             * different signs
             */
            if (vfp_single_packed_sign(src_rn))
            {
                /*
                 * src_rn is negative, so src_rn < src_rm state.nzcv = 1000
                 */
                pstate_nzcv.nf = 1;
                pstate_nzcv.zf = 0;
                pstate_nzcv.cf = 0;
                pstate_nzcv.vf = 0;
            } else
            {
                /*
                 * src_rn is positive, so src_rn > src_rm state.nzcv = 0010
                 */
                pstate_nzcv.nf = 0;
                pstate_nzcv.zf = 0;
                pstate_nzcv.cf = 1;
                pstate_nzcv.vf = 0;
            }
        } else if ((vfp_single_packed_sign(src_rn) != 0) ^ (src_rn < src_rm))
        {
            /*
             * src_rn < src_rm state.nzcv = 1000
             */
            pstate_nzcv.nf = 1;
            pstate_nzcv.zf = 0;
            pstate_nzcv.cf = 0;
            pstate_nzcv.vf = 0;
        } else if ((vfp_single_packed_sign(src_rn) != 0) ^ (src_rn > src_rm))
        {
            /*
             * src_rn > src_rm state.nzcv = 0010
             */
            pstate_nzcv.nf = 0;
            pstate_nzcv.zf = 0;
            pstate_nzcv.cf = 1;
            pstate_nzcv.vf = 0;
        }
    }

    setPStateNZCV(pstate_nzcv);
    return;
}

static void double_fp_compare(ARMul_State * state, unsigned int rn, unsigned int rm,
                              bool_t cmp_with_zero, bool_t signal_all_nans)
{
    int64_t src_rn, src_rm;
    uint32_t ret = 0;

    FPCR_reg_t fpcr = state->system_reg.FPCR;
    struct pstate_nzcv_t pstate_nzcv;

    src_rn = READ_Vd_REG(rn);
    if (cmp_with_zero)
        src_rm = 0;
    else
        src_rm = READ_Vd_REG(rm);

    if (vfp_double_packed_exponent(src_rm) == 2047 && vfp_double_packed_mantissa(src_rm))
    {
        if (signal_all_nans || !(vfp_double_packed_mantissa(src_rm) & (1ULL << (VFP_DOUBLE_MANTISSA_BITS - 1))))
        {
            /*
             * Signalling NaN, or signalling on quiet NaN state.nzcv = 0011
             */
            pstate_nzcv.nf = 0;
            pstate_nzcv.zf = 0;
            pstate_nzcv.cf = 1;
            pstate_nzcv.vf = 1;
            NOT_IMPLEMENT;
            //Call Exception
        }
    }

    if (vfp_double_packed_exponent(src_rn) == 2047 && vfp_double_packed_mantissa(src_rn))
    {
        if (signal_all_nans || !(vfp_double_packed_mantissa(src_rn) & (1ULL << (VFP_DOUBLE_MANTISSA_BITS - 1))))
        {
            /*
             * Signalling NaN, or signalling on quiet NaN state.nzcv = 0011
             */
            pstate_nzcv.nf = 0;
            pstate_nzcv.zf = 0;
            pstate_nzcv.cf = 1;
            pstate_nzcv.vf = 1;
            NOT_IMPLEMENT;
            //Call Exception
        }
    }

    if (ret == 0)
    {
        //printf("In %s, d=%lld, m =%lld\n ", __FUNCTION__, d, m);
        if (src_rn == src_rm || vfp_double_packed_abs(src_rn | src_rm) == 0)
        {
            /*
             * equal state.nzcv = 0110
             */
            pstate_nzcv.nf = 0;
            pstate_nzcv.zf = 1;
            pstate_nzcv.cf = 1;
            pstate_nzcv.vf = 0;

        } else if (vfp_double_packed_sign(src_rn ^ src_rm))
        {
            /*
             * different signs
             */
            if (vfp_double_packed_sign(src_rn))
            {
                /*
                 * src_rn is negative, so src_rn < src_rm state.nzcv = 1000
                 */
                pstate_nzcv.nf = 1;
                pstate_nzcv.zf = 0;
                pstate_nzcv.cf = 0;
                pstate_nzcv.vf = 0;
            } else
            {
                /*
                 * src_rn is positive, so src_rn > src_rm state.nzcv = 0010
                 */
                pstate_nzcv.nf = 0;
                pstate_nzcv.zf = 0;
                pstate_nzcv.cf = 1;
                pstate_nzcv.vf = 0;
            }
        } else if ((vfp_double_packed_sign(src_rn) != 0) ^ (src_rn < src_rm))
        {

            /*
             * src_rn < src_rm state.nzcv = 1000
             */
            pstate_nzcv.nf = 1;
            pstate_nzcv.zf = 0;
            pstate_nzcv.cf = 0;
            pstate_nzcv.vf = 0;
        } else if ((vfp_double_packed_sign(src_rn) != 0) ^ (src_rn > src_rm))
        {
            /*
             * src_rn > src_rm state.nzcv = 0010
             */
            pstate_nzcv.nf = 0;
            pstate_nzcv.zf = 0;
            pstate_nzcv.cf = 1;
            pstate_nzcv.vf = 0;
        }
    }

    setPStateNZCV(pstate_nzcv);
    return;
}

static void handle_fp_fcvtzi(ARMul_State * state, ARMword rmode)
{
    ARMword instr = state->instr;
    ARMword ftype, sf, Rn, Rd, opc;

    ftype = BITS(22, 23);
    sf = BITS(31, 31);
    Rn = BITS(5, 9);
    Rd = BITS(0, 4);
    opc = BITS(16, 18);

    switch (ftype)
    {
        case 0:
            if (opc & 1)
                ftoui(state, Rn, Rd, rmode);
            else
                ftosi(state, Rn, Rd, rmode);
            break;
        case 1:
            if (opc & 1)
                fdtoui(state, Rn, Rd, rmode);
            else
                fdtosi(state, Rn, Rd, rmode);
            break;
        case 3:
            NOT_IMPLEMENT;
            //half_fp_fcvtzi(state, rn, rm, opc & 1, opc & 2);
            break;
    }
}

static void handle_fp_fcvtpi(ARMul_State * state, ARMword rmode)
{
    NOT_IMPLEMENT;

}
static void ftoui(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword rmode)
{
    struct vfp_single vsm;
    u32 d, exceptions = 0;
    ARMword src;
    int tm;

    src = READ_Vs_REG(Rn);
    vfp_single_unpack(&vsm, src);

    /*
     * Do we have a denormalised number?
     */
    tm = vfp_single_type(&vsm);
    if (tm & VFP_DENORMAL)
        exceptions |= FPSCR_IDC;

    if (tm & VFP_NAN)
        vsm.sign = 0;

    if (vsm.exponent >= 127 + 32)
    {
        d = vsm.sign ? 0 : 0xffffffff;
        exceptions = FPSCR_IOC;
    } else if (vsm.exponent >= 127 - 1)
    {
        int shift = 127 + 31 - vsm.exponent;
        u32 rem, incr = 0;

        /*
         * 2^0 <= m < 2^32-2^8
         */
        d = (vsm.significand << 1) >> shift;
        rem = vsm.significand << (33 - shift);

        if (rmode == FPSCR_ROUND_NEAREST)
        {
            incr = 0x80000000;
            if ((d & 1) == 0)
                incr -= 1;
        } else if (rmode == FPSCR_ROUND_TOZERO)
        {
            incr = 0;
        } else if ((rmode == FPSCR_ROUND_PLUSINF) ^ (vsm.sign != 0))
        {
            incr = ~0;
        }

        if ((rem + incr) < rem)
        {
            if (d < 0xffffffff)
                d += 1;
            else
                exceptions |= FPSCR_IOC;
        }

        if (d && vsm.sign)
        {
            d = 0;
            exceptions |= FPSCR_IOC;
        } else if (rem)
            exceptions |= FPSCR_IXC;
    } else
    {
        d = 0;
        if (vsm.exponent | vsm.significand)
        {
            exceptions |= FPSCR_IXC;
            if (rmode == FPSCR_ROUND_PLUSINF && vsm.sign == 0)
                d = 1;
            else if (rmode == FPSCR_ROUND_MINUSINF && vsm.sign)
            {
                d = 0;
                exceptions |= FPSCR_IOC;
            }
        }
    }

    state->system_reg.FPSR.all = exceptions;
    WRITE_X_REG(Rd, d);
}

static void ftosi(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword rmode)
{
    struct vfp_single vsm;
    u32 d, exceptions = 0;
    ARMword src;
    int tm;

    src = READ_Vs_REG(Rn);
    vfp_single_unpack(&vsm, src);

    /*
     * Do we have a denormalised number?
     */
    tm = vfp_single_type(&vsm);
    if (vfp_single_type(&vsm) & VFP_DENORMAL)
        exceptions |= FPSCR_IDC;

    if (tm & VFP_NAN)
    {
        d = 0;
        exceptions |= FPSCR_IOC;
    } else if (vsm.exponent >= 127 + 32)
    {
        /*
         * m >= 2^31-2^7: invalid
         */
        d = 0x7fffffff;
        if (vsm.sign)
            d = ~d;
        exceptions |= FPSCR_IOC;
    } else if (vsm.exponent >= 127 - 1)
    {
        int shift = 127 + 31 - vsm.exponent;
        u32 rem, incr = 0;

        /* 2^0 <= m <= 2^31-2^7 */
        d = (vsm.significand << 1) >> shift;
        rem = vsm.significand << (33 - shift);

        if (rmode == FPSCR_ROUND_NEAREST)
        {
            incr = 0x80000000;
            if ((d & 1) == 0)
                incr -= 1;
        } else if (rmode == FPSCR_ROUND_TOZERO)
        {
            incr = 0;
        } else if ((rmode == FPSCR_ROUND_PLUSINF) ^ (vsm.sign != 0))
        {
            incr = ~0;
        }

        if ((rem + incr) < rem && d < 0xffffffff)
            d += 1;
        if (d > 0x7fffffff + (vsm.sign != 0))
        {
            d = 0x7fffffff + (vsm.sign != 0);
            exceptions |= FPSCR_IOC;
        } else if (rem)
            exceptions |= FPSCR_IXC;

        if (vsm.sign)
            d = -d;
    } else
    {
        d = 0;
        if (vsm.exponent | vsm.significand)
        {
            exceptions |= FPSCR_IXC;
            if (rmode == FPSCR_ROUND_PLUSINF && vsm.sign == 0)
                d = 1;
            else if (rmode == FPSCR_ROUND_MINUSINF && vsm.sign)
                d = -1;
        }
    }

    state->system_reg.FPSR.all = exceptions;
    WRITE_X_REG(Rd, d);
}

static void fdtoui(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword rmode)
{
    struct vfp_double vdm;
    u64 d, exceptions = 0;
    ARMdword src;
    int tm;

    src = READ_Vd_REG(Rn);
    vfp_double_unpack(&vdm, src);

    /*
     * Do we have a denormalised number?
     */
    tm = vfp_double_type(&vdm);
    if (tm & VFP_DENORMAL)
        exceptions |= FPSCR_IDC;

    if (tm & VFP_NAN)
        vdm.sign = 0;

    if (vdm.exponent >= 1023 + 32)
    {
        d = vdm.sign ? 0 : 0xffffffff;
        exceptions = FPSCR_IOC;
    } else if (vdm.exponent >= 1023 - 1)
    {
        int shift = 1023 + 63 - vdm.exponent;
        u64 rem, incr = 0;

        /*
         * 2^0 <= m < 2^32-2^8
         */
        if (shift == 64)
            d = 0;
        else
            d = (vdm.significand << 1) >> shift;
        rem = vdm.significand << (65 - shift);

        if (rmode == FPSCR_ROUND_NEAREST)
        {
            incr = 0x8000000000000000ULL;
            if ((d & 1) == 0)
                incr -= 1;
        } else if (rmode == FPSCR_ROUND_TOZERO)
        {
            incr = 0;
        } else if ((rmode == FPSCR_ROUND_PLUSINF) ^ (vdm.sign != 0))
        {
            incr = ~0ULL;
        }

        if ((rem + incr) < rem)
        {
            if (d < 0xffffffff)
                d += 1;
            else
                exceptions |= FPSCR_IOC;
        }

        if (d && vdm.sign)
        {
            d = 0;
            exceptions |= FPSCR_IOC;
        } else if (rem)
            exceptions |= FPSCR_IXC;
    } else
    {
        d = 0;
        if (vdm.exponent | vdm.significand)
        {
            exceptions |= FPSCR_IXC;
            if (rmode == FPSCR_ROUND_PLUSINF && vdm.sign == 0)
                d = 1;
            else if (rmode == FPSCR_ROUND_MINUSINF && vdm.sign)
            {
                d = 0;
                exceptions |= FPSCR_IOC;
            }
        }
    }

    state->system_reg.FPSR.all = exceptions;
    WRITE_X_REG(Rd, d);
}

static void fdtosi(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword rmode)
{
    struct vfp_double vdm;
    u64 d, exceptions = 0;
    ARMdword src;
    int tm;

    src = READ_Vd_REG(Rn);
    vfp_double_unpack(&vdm, src);

    /*
     * Do we have a denormalised number?
     */
    tm = vfp_double_type(&vdm);
    if (tm & VFP_DENORMAL)
        exceptions |= FPSCR_IDC;

    if (tm & VFP_NAN)
    {
        d = 0;
        exceptions |= FPSCR_IOC;
    } else if (vdm.exponent >= 1023 + 32)
    {
        d = 0x7fffffff;
        if (vdm.sign)
            d = ~d;
        exceptions |= FPSCR_IOC;
    } else if (vdm.exponent >= 1023 - 1)
    {
        int shift = 1023 + 63 - vdm.exponent;   /* 58 */
        u64 rem, incr = 0;

        if (shift == 64)
            d = 0;
        else
            d = (vdm.significand << 1) >> shift;
        rem = vdm.significand << (65 - shift);

        if (rmode == FPSCR_ROUND_NEAREST)
        {
            incr = 0x8000000000000000ULL;
            if ((d & 1) == 0)
                incr -= 1;
        } else if (rmode == FPSCR_ROUND_TOZERO)
        {
            incr = 0;
        } else if ((rmode == FPSCR_ROUND_PLUSINF) ^ (vdm.sign != 0))
        {
            incr = ~0ULL;
        }

        if ((rem + incr) < rem && d < 0xffffffff)
            d += 1;
        if (d > 0x7fffffff + (vdm.sign != 0))
        {
            d = 0x7fffffff + (vdm.sign != 0);
            exceptions |= FPSCR_IOC;
        } else if (rem)
            exceptions |= FPSCR_IXC;

        if (vdm.sign)
            d = -d;
    } else
    {
        d = 0;
        if (vdm.exponent | vdm.significand)
        {
            exceptions |= FPSCR_IXC;
            if (rmode == FPSCR_ROUND_PLUSINF && vdm.sign == 0)
                d = 1;
            else if (rmode == FPSCR_ROUND_MINUSINF && vdm.sign)
                d = -1;
        }
    }

    state->system_reg.FPSR.all = exceptions;
    WRITE_X_REG(Rd, d);
}

static void handle_fp_scvtf(ARMul_State * state)
{
    ARMword instr = state->instr;
    ARMword ftype, sf, Rn, Rd, opc;

    ftype = BITS(22, 23);
    sf = BITS(31, 31);
    Rn = BITS(5, 9);
    Rd = BITS(0, 4);
    opc = BITS(16, 18);

    switch (ftype)
    {
        case 0:
            if (opc & 1)
                fsitof(state, Rn, Rd, sf);
            else
                fuitof(state, Rn, Rd, sf);
            break;
        case 1:
            if (opc & 1)
                fsitod(state, Rn, Rd, sf);
            else
                fuitod(state, Rn, Rd, sf);
            break;
        case 3:
            NOT_IMPLEMENT;
            //half_fp_scvtf(state, Rn, Rd);
            break;
    }
}

static void fsitof(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword sf)
{
    s_precision va_rd;
    int64_t src_rn;

    if (!sf)
    {
        READ_W_REG(Rn, src_rn);
    } else
        src_rn = READ_X_REG(Rn);

    va_rd.value = (float) src_rn;
    WRITE_Vs_REG(Rd, va_rd.data);
}

static void fuitof(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword sf)
{
    s_precision va_rd;
    uint64_t src_rn;

    if (!sf)
    {
        READ_W_REG(Rn, src_rn);
    } else
        src_rn = READ_X_REG(Rn);

    va_rd.value = (float) src_rn;
    WRITE_Vs_REG(Rd, va_rd.data);
}

static void fsitod(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword sf)
{
    d_precision va_rd;
    int64_t src_rn;

    if (!sf)
    {
        READ_W_REG(Rn, src_rn);
    } else
        src_rn = READ_X_REG(Rn);

    va_rd.value = (double) src_rn;
    WRITE_Vd_REG(Rd, va_rd.data);
}

static void fuitod(ARMul_State * state, ARMword Rn, ARMword Rd, ARMword sf)
{
    d_precision va_rd;
    uint64_t src_rn;

    if (!sf)
    {
        READ_W_REG(Rn, src_rn);
    } else
        src_rn = READ_X_REG(Rn);

    va_rd.value = (double) src_rn;
    WRITE_Vd_REG(Rd, va_rd.data);
}

static void handle_fp_2src_single(ARMul_State * state, int opcode)
{
    ARMword instr = state->instr;
    uint32_t exceptions;
    uint32_t fpscr = (u32) state->system_reg.FPCR.all;

    int rd = BITS(0, 4);
    int rn = BITS(5, 9);
    int rm = BITS(16, 20);

    int sm = READ_Vd_REG(rm);

    switch (opcode)
    {
        case 0x0:                      /* FMUL */
            exceptions = vfp_single_fmul(state, rd, rn, sm, fpscr);
            break;
        case 0x1:                      /* FDIV */
            exceptions = vfp_single_fdiv(state, rd, rn, sm, fpscr);
            break;
        case 0x2:                      /* FADD */
            exceptions = vfp_single_fadd(state, rd, rn, sm, fpscr);
            break;
        case 0x3:                      /* FSUB */
            exceptions = vfp_single_fsub(state, rd, rn, sm, fpscr);
            break;
        case 0x4:                      /* FMAX */
        case 0x6:                      /* FMAXNM */
            vfp_single_fmax(state, rd, rn, rm);
            break;
        case 0x5:                      /* FMIN */
        case 0x7:                      /* FMINNM */
            vfp_single_fmin(state, rd, rn, rm);
            break;
        case 0x8:                      /* FNMUL */
            {
                s_precision tmp_rd;

                exceptions = vfp_single_fmul(state, rd, rn, sm, fpscr);
                tmp_rd.data = READ_Vs_REG(rd);
                tmp_rd.value = -tmp_rd.value;
                WRITE_Vs_REG(rd, tmp_rd.data);
            }
            break;
        default:
            NOT_IMPLEMENT;
            break;
    }
}

static void handle_fp_2src_double(ARMul_State * state, int opcode)
{
    ARMword instr = state->instr;
    uint32_t exceptions = 0;
    uint32_t fpscr = (u32) state->system_reg.FPCR.all;

    int rd = BITS(0, 4);
    int rn = BITS(5, 9);
    int rm = BITS(16, 20);

    switch (opcode)
    {
        case 0x0:                      /* FMUL */
            exceptions = vfp_double_fmul(state, rd, rn, rm, fpscr);
            break;
        case 0x1:                      /* FDIV */
            exceptions = vfp_double_fdiv(state, rd, rn, rm, fpscr);
            break;
        case 0x2:                      /* FADD */
            exceptions = vfp_double_fadd(state, rd, rn, rm, fpscr);
            break;
        case 0x3:                      /* FSUB */
            exceptions = vfp_double_fsub(state, rd, rn, rm, fpscr);
            break;
        case 0x4:                      /* FMAX */
        case 0x6:                      /* FMAXNM */
            vfp_double_fmax(state, rd, rn, rm);
            break;
        case 0x5:                      /* FMIN */
        case 0x7:                      /* FMINNM */
            vfp_double_fmin(state, rd, rn, rm);
            break;
        case 0x8:                      /* FNMUL */
            {
                d_precision tmp_rd;

                exceptions = vfp_double_fmul(state, rd, rn, rm, fpscr);
                tmp_rd.data = READ_Vd_REG(rd);
                tmp_rd.value = -tmp_rd.value;
                WRITE_Vd_REG(rd, tmp_rd.data);
            }
            break;
        default:
            NOT_IMPLEMENT;
            break;
    }
    if (exceptions & 0x3f)
        FPProcessException(state, exceptions);
}

static void handle_fp_2src_half(ARMul_State * state, int opcode)
{
    NOT_IMPLEMENT;
}

static void vfp_single_fmax(ARMul_State * state, int rd, int rn, int rm)
{
    int32_t src_rn, src_rm;

    src_rn = vfp_get_float(state, rn);
    src_rm = vfp_get_float(state, rm);

    if (vfp_single_packed_sign(src_rn ^ src_rm))
    {
        if (vfp_single_packed_sign(src_rn) == 0)
        {
            /*
             * src_rn is positive, so src_rn > src_rm
             */
            vfp_put_float(state, rd, src_rn);
        } else
        {
            /*
             * src_rn is negative, so src_rn < src_rm
             */
            vfp_put_float(state, rd, src_rm);
        }
    } else if ((vfp_single_packed_sign(src_rn) != 0) ^ (src_rn < src_rm))
    {
        /*
         * src_rn < src_rm
         */
        vfp_put_float(state, rd, src_rm);
    } else if ((vfp_single_packed_sign(src_rn) != 0) ^ (src_rn > src_rm))
    {
        /*
         * src_rn > src_rm
         */
        vfp_put_float(state, rd, src_rn);
    } else
        vfp_put_float(state, rd, src_rn);
}

static void vfp_single_fmin(ARMul_State * state, int rd, int rn, int rm)
{
    int32_t src_rn, src_rm;

    src_rn = vfp_get_float(state, rn);
    src_rm = vfp_get_float(state, rm);

    if (vfp_single_packed_sign(src_rn ^ src_rm))
    {
        if (vfp_single_packed_sign(src_rn) == 0)
        {
            /*
             * src_rn is positive, so src_rn > src_rm
             */
            vfp_put_float(state, rd, src_rm);
        } else
        {
            /*
             * src_rn is negative, so src_rn < src_rm
             */
            vfp_put_float(state, rd, src_rn);
        }
    } else if ((vfp_single_packed_sign(src_rn) != 0) ^ (src_rn < src_rm))
    {
        /*
         * src_rn < src_rm
         */
        vfp_put_float(state, rd, src_rn);
    } else if ((vfp_single_packed_sign(src_rn) != 0) ^ (src_rn > src_rm))
    {
        /*
         * src_rn > src_rm
         */
        vfp_put_float(state, rd, src_rm);
    } else
        vfp_put_float(state, rd, src_rn);
}

static void vfp_double_fmax(ARMul_State * state, int rd, int rn, int rm)
{
    int64_t src_rn, src_rm;

    src_rn = vfp_get_double(state, rn);
    src_rm = vfp_get_double(state, rm);

    if (vfp_double_packed_sign(src_rn ^ src_rm))
    {
        if (vfp_double_packed_sign(src_rn) == 0)
        {
            /*
             * src_rn is positive, so src_rn > src_rm
             */
            vfp_put_double(state, rd, src_rn);
        } else
        {
            /*
             * src_rn is negative, so src_rn < src_rm
             */
            vfp_put_double(state, rd, src_rm);
        }
    } else if ((vfp_double_packed_sign(src_rn) != 0) ^ (src_rn < src_rm))
    {
        /*
         * src_rn < src_rm
         */
        vfp_put_double(state, rd, src_rm);
    } else if ((vfp_double_packed_sign(src_rn) != 0) ^ (src_rn > src_rm))
    {
        /*
         * src_rn > src_rm
         */
        vfp_put_double(state, rd, src_rn);
    } else
        vfp_put_double(state, rd, src_rn);
}

static void vfp_double_fmin(ARMul_State * state, int rd, int rn, int rm)
{
    int64_t src_rn, src_rm;

    src_rn = vfp_get_double(state, rn);
    src_rm = vfp_get_double(state, rm);

    if (vfp_double_packed_sign(src_rn ^ src_rm))
    {
        if (vfp_double_packed_sign(src_rn) == 0)
        {
            /*
             * src_rn is positive, so src_rn > src_rm
             */
            vfp_put_double(state, rd, src_rm);
        } else
        {
            /*
             * src_rn is negative, so src_rn < src_rm
             */
            vfp_put_double(state, rd, src_rn);
        }
    } else if ((vfp_double_packed_sign(src_rn) != 0) ^ (src_rn < src_rm))
    {
        /*
         * src_rn < src_rm
         */
        vfp_put_double(state, rd, src_rn);
    } else if ((vfp_double_packed_sign(src_rn) != 0) ^ (src_rn > src_rm))
    {
        /*
         * src_rn > src_rm
         */
        vfp_put_double(state, rd, src_rm);
    } else
        vfp_put_double(state, rd, src_rn);
}

static void handle_fmov(ARMul_State * state, bool_t sf, int rd, int rn, int type, bool_t itof)
{
    /* FMOV: gpr to or from float, double, or top half of quad fp reg,
     * without conversion.
     */

    if (itof)
    {
        switch (type)
        {
            case 0:
                /* 32 bit */
                {
                    ARMword src_rn;

                    READ_W_REG(rn, src_rn);
                    WRITE_Vs_REG(rd, src_rn);
                }
                break;
            case 1:
                /* 64 bit */
                {
                    ARMdword src_rn = READ_X_REG(rn);

                    WRITE_Vd_REG(rd, src_rn);
                }
                break;
            case 2:
                /* 64 bit to top half. */
                {
                    ARMdword src_rn = READ_X_REG(rn);

                    WRITE_Vq_REG(rd, 0, src_rn);
                }
                break;
            case 3:
                /* 16 bit */
                {
                    uint16_t src_rn = (uint16_t) READ_X_REG(rn);

                    WRITE_Vh_REG(rd, src_rn);
                }
                break;
            default:
                NOT_IMPLEMENT;
        }
    } else
    {
        switch (type)
        {
            case 0:
                /* 32 bit */
                {
                    ARMword src_rn = READ_Vs_REG(rn);

                    WRITE_W_REG(rd, src_rn);
                }
                break;
            case 1:
                /* 64 bit */
                {
                    ARMdword src_rn = READ_Vd_REG(rn);

                    WRITE_X_REG(rd, src_rn);
                }
                break;
            case 2:
                /* 64 bits from top half */
                {
                    ARMdword tmp, src_rn;

                    READ_Vq_REG(rn, tmp, src_rn);
                    WRITE_X_REG(rd, src_rn);
                }
                break;
            case 3:
                /* 16 bit */
                {
                    uint16_t src_rn;

                    src_rn = READ_Vh_REG(rn);
                    if (sf)
                    {
                        WRITE_X_REG(rd, src_rn);
                    } else
                    {
                        WRITE_W_REG(rd, src_rn);
                    }
                }
                break;
            default:
                NOT_IMPLEMENT;
                break;
        }
    }
}

static void FPProcessException(ARMul_State * state, ARMword exceptions)
{
    //NOT_IMPLEMENT;
}

/* FP-specific subcases of table C3-6 (SIMD and FP data processing)
 *   31  30  29 28     25 24                          0
 * +---+---+---+---------+-----------------------------+
 * |   | 0 |   | 1 1 1 1 |                             |
 * +---+---+---+---------+-----------------------------+
 */
void disas_data_proc_fp(ARMul_State * state)
{
    ARMword instr = state->instr;

    if (BITS(24, 24))
    {
        /* Floating point data-processing (3 source) */
        DEBUG_INSN;
        disas_fp_3src(state);
    } else if (BITS(21, 21) == 0)
    {
        /* Floating point to fixed point conversions */
        DEBUG_INSN;
        disas_fp_fixed_conv(state);
    } else
    {
        switch (BITS(10, 11))
        {
            case 1:
                /* Floating point conditional compare */
                DEBUG_INSN;
                disas_fp_ccomp(state);
                break;
            case 2:
                /* Floating point data-processing (2 source) */
                DEBUG_INSN;
                disas_fp_2src(state);
                break;
            case 3:
                /* Floating point conditional select */
                DEBUG_INSN;
                disas_fp_csel(state);
                break;
            case 0:
                switch (ctz32(BITS(12, 15)))
                {
                    case 0:            /* [15:12] == xxx1 */
                        /* Floating point immediate */
                        DEBUG_INSN;
                        disas_fp_imm(state);
                        break;
                    case 1:            /* [15:12] == xx10 */
                        /* Floating point compare */
                        DEBUG_INSN;
                        disas_fp_compare(state);
                        break;
                    case 2:            /* [15:12] == x100 */
                        /* Floating point data-processing (1 source) */
                        DEBUG_INSN;
                        disas_fp_1src(state);
                        break;
                    case 3:            /* [15:12] == 1000 */
                        UNALLOCATED_INSTR;
                        break;
                    default:           /* [15:12] == 0000 */
                        /* Floating point <-> integer conversions */
                        DEBUG_INSN;
                        disas_fp_int_conv(state);
                        break;
                }
                break;
        }
    }
}

static inline AArch64DecodeFn *lookup_disas_fn(const AArch64DecodeTable * table, uint32_t insn)
{
    const AArch64DecodeTable *tptr = table;

    while (tptr->mask)
    {
        if ((insn & tptr->mask) == tptr->pattern)
        {
            return tptr->disas_fn;
        }
        tptr++;
    }
    return NULL;
}

void disas_data_proc_simd(ARMul_State * state)
{
    ARMword instr = state->instr;

    /* Note that this is called with all non-FP cases from
     * table C3-6 so it must UNDEF for entries not specifically
     * allocated to instructions in that table.
     */
    AArch64DecodeFn *fn = lookup_disas_fn(&data_proc_simd[0], instr);

    if (fn)
    {
        fn(state);
    } else
    {
        UNALLOCATED_INSTR;
    }
}

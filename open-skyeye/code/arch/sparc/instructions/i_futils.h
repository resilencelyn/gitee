#ifndef _FUTILS_H_
#define _FUTILS_H_

#include "../common/fpu.h"

/*
 * Operations on packed single-precision numbers
 */
//#define fpu_double_packed_sign(v)       ((v) & (1ULL << 63))
//#define fpu_double_packed_negate(v)     ((v) ^ (1ULL << 63))
//#define fpu_double_packed_abs(v)        ((v) & ~(1ULL << 63))
//#define fpu_double_packed_exponent(v)   (((v) >> FPU_DOUBLE_MANTISSA_BITS) & ((1 << FPU_DOUBLE_EXPONENT_BITS) - 1))
//#define fpu_double_packed_mantissa(v)   ((v) & ((1ULL << FPU_DOUBLE_MANTISSA_BITS) - 1))

/* ftt must be 0 before fp exception occur */
#define dfsr_set_ftt(x) LETS(REG_FPSR, OR(RSPR(REG_FPSR), SHL(x, CONST(FSR_ftt_first))))
//#define fsr_set_ftt(x)        FPSRREG |= (x << FSR_ftt_first)
#define dfsr_clear_ftt() LETS(REG_FPSR, AND(RSPR(REG_FPSR), CONST(~(7 << FSR_ftt_first))))
//#define fsr_clear_ftt()       FPSRREG &= ~(7 << FSR_ftt_first)
#define dfsr_clear_rd() LETS(REG_FPSR, AND(RSPR(REG_FPSR), CONST(~(3 << FSR_RD_first))))
//#define fsr_clear_rd()        FPSRREG &= ~(3 << FSR_RD_first)
#define dfsr_set_rd(x) \
	do{ \
		dfsr_clear_rd(); \
		LETS(REG_FPSR, OR(RSPR(REG_FPSR), SHL(x, CONST(FSR_RD_first)))); \
	}while(0)
//#define fsr_set_rd(x) fsr_clear_rd();                                 \
//                                      FPSRREG |= (x << FSR_RD_first)
#define dfsr_set_cexc(x) \
	do{ \
		LETS(REG_FPSR, AND(RSPR(REG_FPSR), CONST(~(0x1f << FSR_cexc_first)))) \
		LETS(REG_FPSR, OR(RSPR(REG_FPSR), SHL(x, CONST(FSR_cexc_first)))); \
	}while(0)
//#define fsr_set_cexc(x)       FPSRREG &= ~(0x1f << FSR_cexc_first);           \
//                                 FPSRREG |= x << FSR_cexc_first
#define dfsr_get_cexc() AND(LSHR(RSPR(REG_FPSR), CONST(FSR_cexc_first)), CONST(0x1f))
//#define fsr_get_cexc() (FPSRREG >> FSR_cexc_first) & 0x1f
#define dfsr_set_aexc(x) \
	do{ \
		LETS(REG_FPSR, AND(RSPR(REG_FPSR), CONST(~(0x1f << FSR_aexc_first)))) \
		LETS(REG_FPSR, OR(RSPR(REG_FPSR), SHL(x, CONST(FSR_aexc_first)))); \
	}while(0)
//#define fsr_set_aexc(x)       FPSRREG &= ~(0x1f << FSR_aexc_first);           \
//                                 FPSRREG |= (x << FSR_aexc_first)
#define dfsr_get_TEM() AND(LSHR(RSPR(REG_FPSR), CONST(FSR_TEM_first)), CONST(0x1f))
//#define fsr_get_TEM() ((FPSRREG >> FSR_TEM_first) & 0x1f)
#define dfsr_set_fcc(x) \
	do{ \
		LETS(REG_FPSR, AND(RSPR(REG_FPSR), CONST(~(0x3 << FSR_fcc_first)))) \
		LETS(REG_FPSR, OR(RSPR(REG_FPSR), SHL(x, CONST(FSR_fcc_first)))); \
	}while(0)
//#define fsr_set_fcc(x)        FPSRREG &= ~(0x3 << FSR_fcc_first);             \
//                                 FPSRREG |= (x << FSR_fcc_first)
#define dfsr_get_fcc() AND(LSHR(RSPR(REG_FPSR), CONST(FSR_fcc_first)), CONST(0x3))
//#define fsr_get_fcc() (FPSRREG >> FSR_fcc_first) & 0x3;

#endif

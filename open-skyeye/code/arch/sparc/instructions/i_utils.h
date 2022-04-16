/*
 * =====================================================================================
 *
 *       Filename:  i_utils.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/05/08 15:17:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

#ifndef _I_UTILS_H__
#define _I_UTILS_H__

#include "sparc.h"
#include "traps.h"
#include "bits.h"
#include "iu.h"

//#include "skyeye_dyncom.h"

#define sign_ext22(x)   sign_ext(x, 22)
#define sign_ext13(x)   sign_ext(x, 13)

//#define COREREG(cpu, r) (((sparc_state_t*)(cpu->cpu_data->obj))->r)
//#define REGV(cpu, x) (COREREG(cpu, regwptr)[(x > 7)][x - 8*(x > 7)])

/* bit operatation */
#define BITS(v, n, m) LSHR(AND((v), CONST(bitsmask((n), (m)))), CONST(m))
#define BIT(v, n) BITS(v, n, n)

#define SETBITS(v, n, m) OR((v), CONST(bitsmask((n), (m))))
#define SETBIT(v, n) SETBITS((v), (n), (n))

#define CLRBITS(v, n, m) AND((v), CONST(~bitsmask((n), (m))))
#define CLRBIT(v, n) CLRBITS((v), (n), (n))

/* set/get psr state */
#define dpsr_get_s()              BIT(RSPR(REG_PSR), PSR_S)
//#define dpsr_get_s(cond)          bit(sparc_state.psr, PSR_S)

#define dpsr_get_carry()          BIT(RSPR(REG_PSR), PSR_icc_C)
//#define dpsr_get_carry()          bit(sparc_state.psr, PSR_icc_C)
#define dpsr_set_carry(cond)      LETS(REG_PSR, SELECT(cond, SETBIT(RSPR(REG_PSR), PSR_icc_C), RSPR(REG_PSR)))
//#define dpsr_set_carry()          set_bit(sparc_state.psr, PSR_icc_C)
#define dpsr_clear_carry(cond)    LETS(REG_PSR, SELECT(cond, CLRBIT(RSPR(REG_PSR), PSR_icc_C), RSPR(REG_PSR)))
//#define dpsr_clear_carry()        clear_bit(sparc_state.psr, PSR_icc_C)

#define dpsr_get_overflow()       BIT(RSPR(REG_PSR), PSR_icc_V)
//#define dpsr_get_overflow()       bit(sparc_state.psr, PSR_icc_V)
#define dpsr_set_overflow(cond)   LETS(REG_PSR, SELECT(cond, SETBIT(RSPR(REG_PSR), PSR_icc_V), RSPR(REG_PSR)))
//#define dpsr_set_overflow()       set_bit(sparc_state.psr, PSR_icc_V)
#define dpsr_clear_overflow(cond) LETS(REG_PSR, SELECT(cond, CLRBIT(RSPR(REG_PSR), PSR_icc_V), RSPR(REG_PSR)))
//#define dpsr_clear_overflow()     clear_bit(sparc_state.psr, PSR_icc_V)

#define dpsr_get_zero()           BIT(RSPR(REG_PSR), PSR_icc_Z)
//#define dpsr_get_zero()           bit(sparc_state.psr, PSR_icc_Z)
#define dpsr_set_zero(cond)       LETS(REG_PSR, SELECT(cond, SETBIT(RSPR(REG_PSR), PSR_icc_Z), RSPR(REG_PSR)))
//#define dpsr_set_zero()           set_bit(sparc_state.psr, PSR_icc_Z)
#define dpsr_clear_zero(cond)     LETS(REG_PSR, SELECT(cond, CLRBIT(RSPR(REG_PSR), PSR_icc_Z), RSPR(REG_PSR)))
//#define dpsr_clear_zero()         clear_bit(sparc_state.psr, PSR_icc_Z)

#define dpsr_get_neg()           BIT(RSPR(REG_PSR), PSR_icc_N)
//#define dpsr_get_neg()           bit(sparc_state.psr, PSR_icc_N)
#define dpsr_set_neg(cond)       LETS(REG_PSR, SELECT(cond, SETBIT(RSPR(REG_PSR), PSR_icc_N), RSPR(REG_PSR)))
//#define dpsr_set_neg()           set_bit(sparc_state.psr, PSR_icc_N)
#define dpsr_clear_neg(cond)     LETS(REG_PSR, SELECT(cond, CLRBIT(RSPR(REG_PSR), PSR_icc_N), RSPR(REG_PSR)))
//#define dpsr_clear_neg()         clear_bit(sparc_state.psr, PSR_icc_N)

void clear_icc(sparc_state_t * sparc_state);
int sign_ext(int x, int nbits);

/* condition if drop trap */
#define CDROPTRAP(t, cond, traptype) CDROPTRAP_##t(cond, traptype)
#define CDROPTRAP_BOOK(cond, traptype) \
	do{ \
		LETS(REG_TRAP, SELECT(cond, CONST(traptype), RSPR(REG_TRAP))); \
	}while(0)
#define DROPTRAP(traptype) \
	do{ \
		LETS(REG_TRAP, CONST(traptype)); \
	}while(0)

/* dyncom */
//void clear_icc_dyncom(cpu_t *cpu, BasicBlock *bb);
#define CLEAR_ICC() \
	do{ \
		LETS(REG_PSR, CLRBIT(RSPR(REG_PSR), PSR_icc_Z)); \
		LETS(REG_PSR, CLRBIT(RSPR(REG_PSR), PSR_icc_N)); \
		LETS(REG_PSR, CLRBIT(RSPR(REG_PSR), PSR_icc_V)); \
		LETS(REG_PSR, CLRBIT(RSPR(REG_PSR), PSR_icc_C)); \
	}while(0)

#endif

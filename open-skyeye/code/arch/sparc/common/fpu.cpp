/*
 * =====================================================================================
 *      Description:  fpu.c
 *
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David Yu, keweihk@gmail.com
 *        Company:  Tsinghua skyeye team
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "types.h"
#include "sparc.h"
#include "traps.h"
#include "bits.h"
#include "iu.h"
#include "debug.h"
#include "fpu.h"
#include "sparc_instr.h"
#include "skyeye_config.h"

extern sparc_return_t iu_i_register(sparc_instruction_t * i_new);
extern sparc_instruction_t i_fdivd;
extern sparc_instruction_t i_fdivs;
extern sparc_instruction_t i_fstod;
extern sparc_instruction_t i_fstoi;
extern sparc_instruction_t i_fcmpd;
extern sparc_instruction_t i_fcmps;
extern sparc_instruction_t i_fcmpes;
extern sparc_instruction_t i_fbne;
extern sparc_instruction_t i_fbug;
extern sparc_instruction_t i_fbuge;
extern sparc_instruction_t i_fbule;
extern sparc_instruction_t i_fbo;
extern sparc_instruction_t i_fbe;
extern sparc_instruction_t i_fmovs;
extern sparc_instruction_t i_fnegs;
extern sparc_instruction_t i_fcmped;
extern sparc_instruction_t i_fdtos;
extern sparc_instruction_t i_fmuls;
extern sparc_instruction_t i_fmuld;
extern sparc_instruction_t i_fsubd;
extern sparc_instruction_t i_fsubs;
extern sparc_instruction_t i_fadds;
extern sparc_instruction_t i_faddd;
extern sparc_instruction_t i_fitod;
extern sparc_instruction_t i_fitos;
extern sparc_instruction_t i_fsqrtd;
extern sparc_instruction_t i_fbge;
extern sparc_instruction_t i_fbl;
extern sparc_instruction_t i_fbg;
extern sparc_instruction_t i_fbul;
extern sparc_instruction_t i_fabss;
extern sparc_instruction_t i_fdtoi;
extern sparc_instruction_t i_fble;

/* define for estimate double float over or under flow */
d_precision D_max, D_min;
s_precision S_max, S_min;
static void fpu_init_state(sparc_state_t * sparc_state)
{
    /* define for estimate double float over or under flow */
#ifdef __WIN32__
    D_max.data = 0x7fefffffffffffffULL;
    D_min.data = 0x0010000000000000ULL;
    S_max.data = 0x7fefffffUL;
    S_min.data = 0x00100000UL;
#else
    D_max.data = 0x7fefffffffffffffUL;
    D_min.data = 0x0010000000000000UL;
    S_max.data = 0x7fefffffU;
    S_min.data = 0x00100000U;
#endif
    FPSRREG = 0x0;
}
static void fpu_isa_register(void)
{
    iu_i_register(&i_fdivd);
    iu_i_register(&i_fdivs);
    iu_i_register(&i_fstod);
    iu_i_register(&i_fstoi);
    iu_i_register(&i_fcmpd);
    iu_i_register(&i_fbne);
    iu_i_register(&i_fbe);
    iu_i_register(&i_fbug);
    iu_i_register(&i_fbuge);
    iu_i_register(&i_fbule);
    iu_i_register(&i_fbo);
    iu_i_register(&i_fcmps);
    iu_i_register(&i_fcmpes);
    iu_i_register(&i_fmovs);
    iu_i_register(&i_fnegs);
    iu_i_register(&i_fcmped);
    iu_i_register(&i_fdtos);
    iu_i_register(&i_fmuls);
    iu_i_register(&i_fmuld);
    iu_i_register(&i_fsubd);
    iu_i_register(&i_fsubs);
    iu_i_register(&i_fadds);
    iu_i_register(&i_faddd);
    iu_i_register(&i_fitod);
    iu_i_register(&i_fitos);
    iu_i_register(&i_fsqrtd);
    iu_i_register(&i_fbge);
    iu_i_register(&i_fbl);
    iu_i_register(&i_fbg);
    iu_i_register(&i_fbul);
    iu_i_register(&i_fabss);
    iu_i_register(&i_fdtoi);
    iu_i_register(&i_fble);
}

int init_sparc_fpu(sparc_state_t * sparc_state)
{
    fpu_init_state(sparc_state);
    fpu_isa_register();
    return 0;
}

uint32_t fpu_get_single(sparc_state_t * sparc_state, unsigned int reg)
{
    uint32_t val = 0;

    val = FPREG(reg);
    return val;
}

void fpu_put_single(sparc_state_t * sparc_state, uint64_t val, unsigned int reg)
{
    FPREG(reg) = val;
}

uint64_t fpu_get_double(sparc_state_t * sparc_state, unsigned int reg)
{
    uint64_t val = 0;

    val = ((uint64_t) FPREG(reg) << 32 | FPREG(reg + 1));
    return val;
}

void fpu_put_double(sparc_state_t * sparc_state, uint64_t val, unsigned int reg)
{
    FPREG(reg) = (uint32_t) (val >> 32);
    FPREG(reg + 1) = val;
}

void fpu_setfsr_exc(sparc_state_t * sparc_state, uint32_t x)
{
    uint32_t tmp;

    tmp = fsr_get_cexc();
    fsr_set_aexc(tmp);
    fsr_set_cexc(x);
}

/* 
 * exception occur return 1, otherwise 0.
 */
int ieee_754_exception(sparc_state_t * sparc_state, uint32_t cexc)
{
    uint32_t tem = fsr_get_TEM();

    fpu_setfsr_exc(sparc_state, cexc);
    if ((cexc & tem) != 0)
    {
        /* ieee 754 exception occur */
        traps->signal(sparc_state->obj, FP_EXCEPTION);
        fsr_set_ftt(IEEE_754_exception);
        return 1;
    }

    return 0;
}

uint32_t get_double_type(double x)
{
    uint32_t type = 0;

    if ((isinf(x)) != 0)
        type |= FPU_INFINITY;
    if ((isnan(x)) != 0)
        type |= FPU_NAN;
    if (x == 0)
        type |= FPU_ZERO;

    return type;
}

/*
 *      PearPC
 *      ppc_opc.h
 *
 *      Copyright (C) 2003 Sebastian Biallas (sb@biallas.net)
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

#ifndef __PPC_OPC_H__
#define __PPC_OPC_H__

#include "skyeye_types.h"

static inline void ppc_update_cr0(e500_core_t *current_core, uint32 r)
{
    current_core->cr &= 0x0fffffff;
    if (!r)
    {
        current_core->cr |= CR_CR0_EQ;
    } else if (r & 0x80000000)
    {
        current_core->cr |= CR_CR0_LT;
    } else
    {
        current_core->cr |= CR_CR0_GT;
    }
    if (current_core->xer & XER_SO)
        current_core->cr |= CR_CR0_SO;
}

void ppc_opc_bx(e500_core_t *current_core);
void ppc_opc_bcx(e500_core_t *current_core);
void ppc_opc_bcctrx(e500_core_t *current_core);
void ppc_opc_bclrx(e500_core_t *current_core);

void ppc_opc_dcba(e500_core_t *current_core);
void ppc_opc_dcbf(e500_core_t *current_core);
void ppc_opc_dcbi(e500_core_t *current_core);
void ppc_opc_dcbst(e500_core_t *current_core);
void ppc_opc_dcbt(e500_core_t *current_core);
void ppc_opc_dcbtst(e500_core_t *current_core);

void ppc_opc_eciwx(e500_core_t *current_core);
void ppc_opc_ecowx(e500_core_t *current_core);
void ppc_opc_eieio(e500_core_t *current_core);

void ppc_opc_icbi(e500_core_t *current_core);
void ppc_opc_isync(e500_core_t *current_core);

void ppc_opc_mcrf(e500_core_t *current_core);
void ppc_opc_mcrfs(e500_core_t *current_core);
void ppc_opc_mcrxr(e500_core_t *current_core);
void ppc_opc_mfcr(e500_core_t *current_core);
void ppc_opc_mffsx(e500_core_t *current_core);
void ppc_opc_mfmsr(e500_core_t *current_core);
void ppc_opc_mfspr(e500_core_t *current_core);
void ppc_opc_mfsr(e500_core_t *current_core);
void ppc_opc_mfsrin(e500_core_t *current_core);
void ppc_opc_mftb(e500_core_t *current_core);
void ppc_opc_mtcrf(e500_core_t *current_core);
void ppc_opc_mtfsb0x(e500_core_t *current_core);
void ppc_opc_mtfsb1x(e500_core_t *current_core);
void ppc_opc_mtfsfx(e500_core_t *current_core);
void ppc_opc_mtfsfix(e500_core_t *current_core);
void ppc_opc_mtmsr(e500_core_t *current_core);
void ppc_opc_mtspr(e500_core_t *current_core);
void ppc_opc_mtsr(e500_core_t *current_core);
void ppc_opc_mtsrin(e500_core_t *current_core);

void ppc_opc_rfi(e500_core_t *current_core);
void ppc_opc_sc(e500_core_t *current_core);
void ppc_opc_sc_e600(e500_core_t *current_core);
void ppc_opc_sync(e500_core_t *current_core);
void ppc_opc_tlbia(e500_core_t *current_core);
void ppc_opc_tlbie(e500_core_t *current_core);
void ppc_opc_tlbsync(e500_core_t *current_core);
void ppc_opc_tw(e500_core_t *current_core);
void ppc_opc_twi(e500_core_t *current_core);

#endif

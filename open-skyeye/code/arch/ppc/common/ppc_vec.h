/*
 *      PearPC
 *      ppc_vec.h
 *
 *      Copyright (C) 2004 Daniel Foesch (dfoesch@cs.nmsu.edu)
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

#ifndef __PPC_VEC_H__
#define __PPC_VEC_H__

#define PPC_OPC_VRc	(1<<10)

/* Rather than write each function to be endianless, we're writing these
 *   defines to do an endianless access to elements of the vector.
 *
 * These are for ADDRESSED vector elements.  Usually, most vector operations
 *   can be performed in either direction without care, so most of the
 *   for-loops should not use these, as it will introduce unneeded code
 *   for little-endian systems.
 */
#if HOST_ENDIANESS == HOST_ENDIANESS_LE

#define VECT_B(reg, index)	((reg).b[15 - (index)])
#define VECT_SB(reg, index)	((reg).sb[15 - (index)])
#define VECT_H(reg, index)	((reg).h[7 - (index)])
#define VECT_SH(reg, index)	((reg).sh[7 - (index)])
#define VECT_W(reg, index)	((reg).w[3 - (index)])
#define VECT_SW(reg, index)	((reg).sw[3 - (index)])
#define VECT_D(reg, index)	((reg).d[1 - (index)])
#define VECT_SD(reg, index)	((reg).sd[1 - (index)])

#define VECT_EVEN(index)	(((index) << 1) + 1)
#define VECT_ODD(index)		(((index) << 1) + 0)

#elif HOST_ENDIANESS == HOST_ENDIANESS_BE

#define VECT_B(reg, index)	((reg).b[(index)])
#define VECT_SB(reg, index)	((reg).sb[(index)])
#define VECT_H(reg, index)	((reg).h[(index)])
#define VECT_SH(reg, index)	((reg).sh[(index)])
#define VECT_W(reg, index)	((reg).w[(index)])
#define VECT_SW(reg, index)	((reg).sw[(index)])
#define VECT_D(reg, index)	((reg).d[(index)])
#define VECT_SD(reg, index)	((reg).sd[(index)])

#define VECT_EVEN(index)	(((index) << 1) + 0)
#define VECT_ODD(index)		(((index) << 1) + 1)

#else
#error Endianess not supported!
#endif

//#define VECTOR_DEBUG  fprintf(stderr, "[PPC/VEC] %s\n", __FUNCTION__)
#define VECTOR_DEBUG

//#define VECTOR_DEBUG_COMMON   fprintf(stderr, "[PPC/VEC] %s\n", __FUNCTION__)
#define VECTOR_DEBUG_COMMON

/* Undefine this to turn of the MSR_VEC check for vector instructions. */
//#define __VEC_EXC_OFF__

#include "skyeye_types.h"

//#include "tools/snprintf.h"

void ppc_opc_vperm(e500_core_t *current_core);
void ppc_opc_vsel(e500_core_t *current_core);
void ppc_opc_vsrb(e500_core_t *current_core);
void ppc_opc_vsrh(e500_core_t *current_core);
void ppc_opc_vsrw(e500_core_t *current_core);
void ppc_opc_vsrab(e500_core_t *current_core);
void ppc_opc_vsrah(e500_core_t *current_core);
void ppc_opc_vsraw(e500_core_t *current_core);
void ppc_opc_vsr(e500_core_t *current_core);
void ppc_opc_vsro(e500_core_t *current_core);
void ppc_opc_vslb(e500_core_t *current_core);
void ppc_opc_vslh(e500_core_t *current_core);
void ppc_opc_vslw(e500_core_t *current_core);
void ppc_opc_vsl(e500_core_t *current_core);
void ppc_opc_vslo(e500_core_t *current_core);
void ppc_opc_vsldoi(e500_core_t *current_core);
void ppc_opc_vrlb(e500_core_t *current_core);
void ppc_opc_vrlh(e500_core_t *current_core);
void ppc_opc_vrlw(e500_core_t *current_core);
void ppc_opc_vmrghb(e500_core_t *current_core);
void ppc_opc_vmrghh(e500_core_t *current_core);
void ppc_opc_vmrghw(e500_core_t *current_core);
void ppc_opc_vmrglb(e500_core_t *current_core);
void ppc_opc_vmrglh(e500_core_t *current_core);
void ppc_opc_vmrglw(e500_core_t *current_core);
void ppc_opc_vspltb(e500_core_t *current_core);
void ppc_opc_vsplth(e500_core_t *current_core);
void ppc_opc_vspltw(e500_core_t *current_core);
void ppc_opc_vspltisb(e500_core_t *current_core);
void ppc_opc_vspltish(e500_core_t *current_core);
void ppc_opc_vspltisw(e500_core_t *current_core);
void ppc_opc_mfvscr(e500_core_t *current_core);
void ppc_opc_mtvscr(e500_core_t *current_core);
void ppc_opc_vpkuhum(e500_core_t *current_core);
void ppc_opc_vpkuwum(e500_core_t *current_core);
void ppc_opc_vpkpx(e500_core_t *current_core);
void ppc_opc_vpkuhus(e500_core_t *current_core);
void ppc_opc_vpkshss(e500_core_t *current_core);
void ppc_opc_vpkuwus(e500_core_t *current_core);
void ppc_opc_vpkswss(e500_core_t *current_core);
void ppc_opc_vpkuhus(e500_core_t *current_core);
void ppc_opc_vpkshus(e500_core_t *current_core);
void ppc_opc_vpkuwus(e500_core_t *current_core);
void ppc_opc_vpkswus(e500_core_t *current_core);
void ppc_opc_vupkhsb(e500_core_t *current_core);
void ppc_opc_vupkhpx(e500_core_t *current_core);
void ppc_opc_vupkhsh(e500_core_t *current_core);
void ppc_opc_vupklsb(e500_core_t *current_core);
void ppc_opc_vupklpx(e500_core_t *current_core);
void ppc_opc_vupklsh(e500_core_t *current_core);
void ppc_opc_vaddubm(e500_core_t *current_core);
void ppc_opc_vadduhm(e500_core_t *current_core);
void ppc_opc_vadduwm(e500_core_t *current_core);
void ppc_opc_vaddfp(e500_core_t *current_core);
void ppc_opc_vaddcuw(e500_core_t *current_core);
void ppc_opc_vaddubs(e500_core_t *current_core);
void ppc_opc_vaddsbs(e500_core_t *current_core);
void ppc_opc_vadduhs(e500_core_t *current_core);
void ppc_opc_vaddshs(e500_core_t *current_core);
void ppc_opc_vadduws(e500_core_t *current_core);
void ppc_opc_vaddsws(e500_core_t *current_core);
void ppc_opc_vsububm(e500_core_t *current_core);
void ppc_opc_vsubuhm(e500_core_t *current_core);
void ppc_opc_vsubuwm(e500_core_t *current_core);
void ppc_opc_vsubfp(e500_core_t *current_core);
void ppc_opc_vsubcuw(e500_core_t *current_core);
void ppc_opc_vsububs(e500_core_t *current_core);
void ppc_opc_vsubsbs(e500_core_t *current_core);
void ppc_opc_vsubuhs(e500_core_t *current_core);
void ppc_opc_vsubshs(e500_core_t *current_core);
void ppc_opc_vsubuws(e500_core_t *current_core);
void ppc_opc_vsubsws(e500_core_t *current_core);
void ppc_opc_vmuleub(e500_core_t *current_core);
void ppc_opc_vmulesb(e500_core_t *current_core);
void ppc_opc_vmuleuh(e500_core_t *current_core);
void ppc_opc_vmulesh(e500_core_t *current_core);
void ppc_opc_vmuloub(e500_core_t *current_core);
void ppc_opc_vmulosb(e500_core_t *current_core);
void ppc_opc_vmulouh(e500_core_t *current_core);
void ppc_opc_vmulosh(e500_core_t *current_core);
void ppc_opc_vmaddfp(e500_core_t *current_core);
void ppc_opc_vmhaddshs(e500_core_t *current_core);
void ppc_opc_vmladduhm(e500_core_t *current_core);
void ppc_opc_vmhraddshs(e500_core_t *current_core);
void ppc_opc_vmsumubm(e500_core_t *current_core);
void ppc_opc_vmsumuhm(e500_core_t *current_core);
void ppc_opc_vmsummbm(e500_core_t *current_core);
void ppc_opc_vmsumshm(e500_core_t *current_core);
void ppc_opc_vmsumuhs(e500_core_t *current_core);
void ppc_opc_vmsumshs(e500_core_t *current_core);
void ppc_opc_vsum4ubs(e500_core_t *current_core);
void ppc_opc_vsum4sbs(e500_core_t *current_core);
void ppc_opc_vsum4shs(e500_core_t *current_core);
void ppc_opc_vsum2sws(e500_core_t *current_core);
void ppc_opc_vsumsws(e500_core_t *current_core);
void ppc_opc_vnmsubfp(e500_core_t *current_core);
void ppc_opc_vavgub(e500_core_t *current_core);
void ppc_opc_vavgsb(e500_core_t *current_core);
void ppc_opc_vavguh(e500_core_t *current_core);
void ppc_opc_vavgsh(e500_core_t *current_core);
void ppc_opc_vavguw(e500_core_t *current_core);
void ppc_opc_vavgsw(e500_core_t *current_core);
void ppc_opc_vmaxub(e500_core_t *current_core);
void ppc_opc_vmaxsb(e500_core_t *current_core);
void ppc_opc_vmaxuh(e500_core_t *current_core);
void ppc_opc_vmaxsh(e500_core_t *current_core);
void ppc_opc_vmaxuw(e500_core_t *current_core);
void ppc_opc_vmaxsw(e500_core_t *current_core);
void ppc_opc_vmaxfp(e500_core_t *current_core);
void ppc_opc_vminub(e500_core_t *current_core);
void ppc_opc_vminsb(e500_core_t *current_core);
void ppc_opc_vminuh(e500_core_t *current_core);
void ppc_opc_vminsh(e500_core_t *current_core);
void ppc_opc_vminuw(e500_core_t *current_core);
void ppc_opc_vminsw(e500_core_t *current_core);
void ppc_opc_vminfp(e500_core_t *current_core);
void ppc_opc_vrfin(e500_core_t *current_core);
void ppc_opc_vrfip(e500_core_t *current_core);
void ppc_opc_vrfim(e500_core_t *current_core);
void ppc_opc_vrfiz(e500_core_t *current_core);
void ppc_opc_vrefp(e500_core_t *current_core);
void ppc_opc_vrsqrtefp(e500_core_t *current_core);
void ppc_opc_vlogefp(e500_core_t *current_core);
void ppc_opc_vexptefp(e500_core_t *current_core);
void ppc_opc_vcfux(e500_core_t *current_core);
void ppc_opc_vcfsx(e500_core_t *current_core);
void ppc_opc_vctsxs(e500_core_t *current_core);
void ppc_opc_vctuxs(e500_core_t *current_core);
void ppc_opc_vand(e500_core_t *current_core);
void ppc_opc_vandc(e500_core_t *current_core);
void ppc_opc_vor(e500_core_t *current_core);
void ppc_opc_vnor(e500_core_t *current_core);
void ppc_opc_vxor(e500_core_t *current_core);
void ppc_opc_vcmpequbx(e500_core_t *current_core);
void ppc_opc_vcmpequhx(e500_core_t *current_core);
void ppc_opc_vcmpequwx(e500_core_t *current_core);
void ppc_opc_vcmpeqfpx(e500_core_t *current_core);
void ppc_opc_vcmpgtubx(e500_core_t *current_core);
void ppc_opc_vcmpgtsbx(e500_core_t *current_core);
void ppc_opc_vcmpgtuhx(e500_core_t *current_core);
void ppc_opc_vcmpgtshx(e500_core_t *current_core);
void ppc_opc_vcmpgtuwx(e500_core_t *current_core);
void ppc_opc_vcmpgtswx(e500_core_t *current_core);
void ppc_opc_vcmpgtfpx(e500_core_t *current_core);
void ppc_opc_vcmpgefpx(e500_core_t *current_core);
void ppc_opc_vcmpbfpx(e500_core_t *current_core);

#endif

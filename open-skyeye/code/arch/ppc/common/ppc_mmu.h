/*
 *      PearPC
 *      ppc_mmu.h
 *
 *      Copyright (C) 2003, 2004 Sebastian Biallas (sb@biallas.net)
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

#ifndef __PPC_MMU_H__
#define __PPC_MMU_H__

#include "skyeye_types.h"
#include "ppc_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

    extern uint8 *gMemory;
    extern uint32 gMemorySize;

#define PPC_MMU_READ  1
#define PPC_MMU_WRITE 2
#define PPC_MMU_CODE  4
#define PPC_MMU_SV    8
#define PPC_MMU_NO_EXC 16

#define PPC_MMU_OK 0
#define PPC_MMU_EXC 1
#define PPC_MMU_FATAL 2

    int e500_mmu_init(e500_mmu_t * mmu);
    uint32_t get_end_of_page(uint32 phys_addr);
    uint32_t get_begin_of_page(uint32_t phys_addr);
    int ppc_effective_to_physical(e500_core_t * core, uint32 addr, int flags, uint32 * result);
    int e600_effective_to_physical(e500_core_t * core, uint32 addr, int flags, uint32 * result);
    int e500_effective_to_physical(e500_core_t * core, uint32 addr, int flags, uint32 * result);
    bool_t ppc_mmu_set_sdr1(e500_core_t *current_core, uint32 newval, bool_t quiesce);
    void ppc_mmu_tlb_invalidate(e500_core_t * core);

    int ppc_read_physical_dword(e500_core_t * core, uint32 addr, uint64 * result);
    int ppc_read_physical_word(e500_core_t * core, uint32 addr, uint32 * result);
    int ppc_read_physical_half(e500_core_t * core, uint32 addr, uint16 * result);
    int ppc_read_physical_byte(e500_core_t * core, uint32 addr, uint8 * result);

    int ppc_read_effective_code(e500_core_t * core, uint32 addr, uint32 * result);
    int ppc_read_effective_dword(e500_core_t * core, uint32 addr, uint64 * result);
    int ppc_read_effective_word(e500_core_t * core, uint32 addr, uint32 * result);
    int ppc_read_effective_half(e500_core_t * core, uint32 addr, uint16 * result);
    int ppc_read_effective_byte(e500_core_t * core, uint32 addr, uint8 * result);

    int ppc_write_physical_dword(e500_core_t * core, uint32 addr, uint64 data);
    int ppc_write_physical_word(e500_core_t * core, uint32 addr, uint32 data);
    int ppc_write_physical_half(e500_core_t * core, uint32 addr, uint16 data);
    int ppc_write_physical_byte(e500_core_t * core, uint32 addr, uint8 data);

    int ppc_write_effective_dword(e500_core_t * core, uint32 addr, uint64 data);
    int ppc_write_effective_word(e500_core_t * core, uint32 addr, uint32 data);
    int ppc_write_effective_half(e500_core_t * core, uint32 addr, uint16 data);
    int ppc_write_effective_byte(e500_core_t * core, uint32 addr, uint8 data);

    int ppc_direct_physical_memory_handle(uint32 addr, uint8 * ptr);
    int ppc_direct_effective_memory_handle(e500_core_t * core, uint32 addr, uint8 * ptr);
    int ppc_direct_effective_memory_handle_code(e500_core_t * core, uint32 addr, uint8 * ptr);
    bool_t ppc_mmu_page_create(uint32 ea, uint32 pa);
    bool_t ppc_mmu_page_free(uint32 ea);
    bool_t ppc_init_physical_memory(uint32 size);

    /*
     * pte: (page table entry)
     * 1st word:
     * 0     V    Valid
     * 1-24  VSID Virtual Segment ID
     * 25    H    Hash function
     * 26-31 API  Abbreviated page index
     * 2nd word:
     * 0-19  RPN  Physical page number
     * 20-22 res
     * 23    R    Referenced bit
     * 24    C    Changed bit
     * 25-28 WIMG Memory/cache control bits
     * 29    res
     * 30-31 PP   Page protection bits
     */

    /*
     *      MMU Opcodes
     */
    void ppc_opc_dcbz(e500_core_t *current_core);
    void ppc_opc_dcbtls(e500_core_t *current_core);

    void ppc_opc_lbz(e500_core_t *current_core);
    void ppc_opc_lbzu(e500_core_t *current_core);
    void ppc_opc_lbzux(e500_core_t *current_core);
    void ppc_opc_lbzx(e500_core_t *current_core);
    void ppc_opc_lfd(e500_core_t *current_core);
    void ppc_opc_lfdu(e500_core_t *current_core);
    void ppc_opc_lfdux(e500_core_t *current_core);
    void ppc_opc_lfdx(e500_core_t *current_core);
    void ppc_opc_lfs(e500_core_t *current_core);
    void ppc_opc_lfsu(e500_core_t *current_core);
    void ppc_opc_lfsux(e500_core_t *current_core);
    void ppc_opc_lfsx(e500_core_t *current_core);
    void ppc_opc_lha(e500_core_t *current_core);
    void ppc_opc_lhau(e500_core_t *current_core);
    void ppc_opc_lhaux(e500_core_t *current_core);
    void ppc_opc_lhax(e500_core_t *current_core);
    void ppc_opc_lhbrx(e500_core_t *current_core);
    void ppc_opc_lhz(e500_core_t *current_core);
    void ppc_opc_lhzu(e500_core_t *current_core);
    void ppc_opc_lhzux(e500_core_t *current_core);
    void ppc_opc_lhzx(e500_core_t *current_core);
    void ppc_opc_lmw(e500_core_t *current_core);
    void ppc_opc_lswi(e500_core_t *current_core);
    void ppc_opc_lswx(e500_core_t *current_core);
    void ppc_opc_lwarx(e500_core_t *current_core);
    void ppc_opc_lwbrx(e500_core_t *current_core);
    void ppc_opc_lwz(e500_core_t *current_core);
    void ppc_opc_lwzu(e500_core_t *current_core);
    void ppc_opc_lwzux(e500_core_t *current_core);
    void ppc_opc_lwzx(e500_core_t *current_core);
    void ppc_opc_lvx(e500_core_t *current_core);                 /* for altivec support */
    void ppc_opc_lvxl(e500_core_t *current_core);
    void ppc_opc_lvebx(e500_core_t *current_core);
    void ppc_opc_lvehx(e500_core_t *current_core);
    void ppc_opc_lvewx(e500_core_t *current_core);
    void ppc_opc_lvsl(e500_core_t *current_core);
    void ppc_opc_lvsr(e500_core_t *current_core);
    void ppc_opc_dst(e500_core_t *current_core);

    void ppc_opc_stb(e500_core_t *current_core);
    void ppc_opc_stbu(e500_core_t *current_core);
    void ppc_opc_stbux(e500_core_t *current_core);
    void ppc_opc_stbx(e500_core_t *current_core);
    void ppc_opc_stfd(e500_core_t *current_core);
    void ppc_opc_stfdu(e500_core_t *current_core);
    void ppc_opc_stfdux(e500_core_t *current_core);
    void ppc_opc_stfdx(e500_core_t *current_core);
    void ppc_opc_stfiwx(e500_core_t *current_core);
    void ppc_opc_stfs(e500_core_t *current_core);
    void ppc_opc_stfsu(e500_core_t *current_core);
    void ppc_opc_stfsux(e500_core_t *current_core);
    void ppc_opc_stfsx(e500_core_t *current_core);
    void ppc_opc_sth(e500_core_t *current_core);
    void ppc_opc_sthbrx(e500_core_t *current_core);
    void ppc_opc_sthu(e500_core_t *current_core);
    void ppc_opc_sthux(e500_core_t *current_core);
    void ppc_opc_sthx(e500_core_t *current_core);
    void ppc_opc_stmw(e500_core_t *current_core);
    void ppc_opc_stswi(e500_core_t *current_core);
    void ppc_opc_stswx(e500_core_t *current_core);
    void ppc_opc_stw(e500_core_t *current_core);
    void ppc_opc_stwbrx(e500_core_t *current_core);
    void ppc_opc_stwcx_(e500_core_t *current_core);
    void ppc_opc_stwu(e500_core_t *current_core);
    void ppc_opc_stwux(e500_core_t *current_core);
    void ppc_opc_stwx(e500_core_t *current_core);
    void ppc_opc_stvx(e500_core_t *current_core);                /* for altivec support */
    void ppc_opc_stvxl(e500_core_t *current_core);
    void ppc_opc_stvebx(e500_core_t *current_core);
    void ppc_opc_stvehx(e500_core_t *current_core);
    void ppc_opc_stvewx(e500_core_t *current_core);
    void ppc_opc_dstst(e500_core_t *current_core);
    void ppc_opc_dss(e500_core_t *current_core);
    void ppc_opc_tlbivax(e500_core_t *current_core);
    void ppc_opc_tlbwe(e500_core_t *current_core);
    void ppc_opc_tlbrehi(e500_core_t *current_core);
    void ppc_opc_wrteei(e500_core_t *current_core);
    void ppc_opc_wrtee(e500_core_t *current_core);
#ifdef __cplusplus
}
#endif
#endif

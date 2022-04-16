/* Copyright (C) 
 * 2013 - Michael.Kang blackfin.kang@gmail.com
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/**
* @file coff.h
* @brief The header of coff format
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#ifndef __COFF_H__
#define __COFF_H__

#include <skyeye_types.h>

// TI's COFF v0 file header
typedef struct __attribute__ ((packed))
{
    uint16_t f_magic;                   // magic number
    uint16_t f_nscns;                   // number of sections
    uint32_t f_timdat;                  // time and date stamp
    uint32_t f_symptr;                  // file ptr to symtab
    uint32_t f_nsyms;                   // number of symtab entries
    uint16_t f_opthdr;                  // size of optional header
    uint16_t f_flags;                   // flags
} filehdr_v0;

// TI's COFF V1/V2 file header
typedef struct __attribute__ ((packed))
{
    uint16_t f_magic;                   // magic number
    uint16_t f_nscns;                   // number of sections
    uint32_t f_timdat;                  // time and date stamp
    uint32_t f_symptr;                  // file ptr to symtab
    uint32_t f_nsyms;                   // number of symtab entries
    uint16_t f_opthdr;                  // size of optional header
    uint16_t f_flags;                   // flags
    uint16_t f_target_id;               // COFF-TI specific: TI target magic number that can execute the file
} filehdr_v12;

// Section header for TI's COFF2 files
typedef struct __attribute__ ((packed))
{
    char s_name[8];                     // section name
    uint32_t s_paddr;                   // physical address
    uint32_t s_vaddr;                   // virtual address
    uint32_t s_size;                    // section size
    uint32_t s_scnptr;                  // file ptr to raw data
    uint32_t s_relptr;                  // file ptr to relocation
    uint32_t s_lnnoptr;                 // file ptr to line numbers
    uint32_t s_nreloc;                  // number of reloc entries
    uint32_t s_nlnno;                   // number of line number entries
    uint32_t s_flags;                   // flags
    uint16_t s_reserved;                // reserved
    uint16_t s_page;                    // memory page number */
} scnhdr_v2;

// s_flags
#define STYP_REG      0x00000000        // Regular section (allocated, relocated, loaded)
#define STYP_DSECT    0x00000001        // Dummy section (relocated, not allocated, not loaded)
#define STYP_NOLOAD   0x00000002        // Noload section (allocated, relocated, not loaded)
#define STYP_GROUP    0x00000004        // Grouped section (formed from several input sections). Other devices
#define STYP_PAD      0x00000008        // Padding section (loaded, not allocated, not relocated). Other devices:
#define STYP_COPY     0x00000010        // Copy section (relocated, loaded, but not allocated; relocation entries
#define STYP_TEXT     0x00000020        // Section contains executable code
#define STYP_DATA     0x00000040        // Section contains initialized data
#define STYP_BSS      0x00000080        // Section contains uninitialized data
#define S_NEWFCN      0x00000100
#define STYP_INFO     0x00000200
#define STYP_OVER     0x00000400
#define STYP_LIB      0x00000800
#define STYP_BLOCK    0x00001000        // Alignment used as a blocking factor.
#define STYP_PASS     0x00002000        // Section should pass through unchanged.
#define STYP_CLINK    0x00004000        // Section requires conditional linking
#define STYP_VECTOR   0x00008000        // Section contains vector table.
#define STYP_PADDED   0x00010000        // section has been padded.

/*
 * STYP_GROUP, STYP_PAD     : Applies to C2800, C5400, and C5500 only.
 * STYP_BLOCK, STYP_PASS    : Reserved for C2800, C5400, and C5500.
 * STYP_VECTOR, STYP_PADDED : Applies to C6000 only.
 */

/* ECOFF uses some additional section flags.  */
#define STYP_RDATA      0x00000100
#define STYP_SDATA      0x00000200
#define STYP_SBSS       0x00000400
#define STYP_GOT        0x00001000
#define STYP_DYNAMIC    0x00002000
#define STYP_DYNSYM     0x00004000
#define STYP_RELDYN     0x00008000
#define STYP_DYNSTR     0x00010000
#define STYP_HASH       0x00020000
#define STYP_LIBLIST    0x00040000
#define STYP_CONFLIC    0x00100000
#define STYP_ECOFF_FINI 0x01000000
#define STYP_EXTENDESC  0x02000000      /* 0x02FFF000 bits => scn type, rest clr */
#define STYP_LITA       0x04000000
#define STYP_LIT8       0x08000000
#define STYP_LIT4       0x10000000
#define STYP_ECOFF_LIB  0x40000000
#define STYP_ECOFF_INIT 0x80000000
#define STYP_OTHER_LOAD (STYP_ECOFF_INIT | STYP_ECOFF_FINI)

/* extended section types */
#define STYP_COMMENT    0x2100000
#define STYP_RCONST     0x2200000
#define STYP_XDATA      0x2400000
#define STYP_PDATA      0x2800000

#define DOT_DEBUG       ".debug"
#define DOT_ZDEBUG      ".zdebug"
#define GNU_LINKONCE_WI ".gnu.linkonce.wi."
#define GNU_LINKONCE_WT ".gnu.linkonce.wt."
#define DOT_RELOC       ".reloc"

typedef struct __attribute__ ((packed))
{
    uint16_t o_magic;                   // magic number
    uint16_t o_vstamp;                  // version stamp
    uint32_t o_tsize;                   // text size in bytes, padded to FW bdry
    uint32_t o_dsize;                   // initialized data
    uint32_t o_bsize;                   // uninitialized data
    uint32_t o_entry;                   // entry point
    uint32_t o_text_start;              // base of text used for this file
    uint32_t o_data_start;              // base of data used for this file
} aouthdr;

bool_t is_coff(const char *filename);
exception_t load_coff(memory_space_intf * memory_space, const char *filename, addr_type_t addr_type);
int check_s_flags(scnhdr_v2 * section_header);
int check_s_flags_print(scnhdr_v2 * section_header);

#define IMAGE_SCN_CNT_CODE	(0x00000020)
#define IMAGE_SCN_MEM_READ	(0x40000000)
#define IMAGE_SCN_TYPE_NO_PAD	(0x00000008)
#define IMAGE_SCN_LNK_OTHER	(0x00000100)
#define IMAGE_SCN_MEM_NOT_CACHED	(0x04000000)
#define IMAGE_SCN_MEM_NOT_PAGED	(0x08000000)
#define IMAGE_SCN_MEM_EXECUTE	(0x20000000)
#define IMAGE_SCN_MEM_WRITE	(0x80000000)
#define IMAGE_SCN_MEM_DISCARDABLE	(0x02000000)
#define IMAGE_SCN_MEM_SHARED	(0x10000000)
#define IMAGE_SCN_LNK_REMOVE	(0x00000800)
#define IMAGE_SCN_CNT_CODE	(0x00000020)
#define IMAGE_SCN_CNT_INITIALIZED_DATA	(0x00000040)
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA	(0x00000080)
#define IMAGE_SCN_LNK_INFO	(0x00000200)
#define IMAGE_SCN_LNK_COMDAT	(0x00001000)
#define SEC_COFF_NOREAD (0x40000000)

#endif

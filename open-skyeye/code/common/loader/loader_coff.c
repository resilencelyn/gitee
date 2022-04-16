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
* @file loader_coff.c
* @brief load the coff executable file
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#include <config.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "portable/mman.h"
#include "bank_defs.h"
#include "skyeye_pref.h"
#include "skyeye_config.h"
#include "skyeye_ram.h"
#include "coff.h"
#include "hex.h"
#include "elf.h"
#include <bfd.h>

// magic numbers for big-endian headers
const uint16_t BEH_COFF_V1_MAGIC = 0xc100;
const uint16_t BEH_COFF_V2_MAGIC = 0xc200;
const uint16_t BEH_AOUT_MAGIC = 0x0801;

// TI's IDs for big-endian headers
const uint16_t BEH_TARGET_ID_TMS320_C1X_C2X_C5X = 0x9200;
const uint16_t BEH_TARGET_ID_TMS320_C3X_C4X = 0x9300;
const uint16_t BEH_TARGET_ID_C80 = 0x9500;
const uint16_t BEH_TARGET_ID_TMS320_C54X = 0x9800;
const uint16_t BEH_TARGET_ID_TMS320_C6X = 0x9900;
const uint16_t BEH_TARGET_ID_TMS320_C28X = 0x9d00;

#if 0

const char get_arch_name(short magic)
{
    switch (magic)
    {
        case LEH_TARGET_ID_TMS320_C1X_C2X_C5X:
        case BEH_TARGET_ID_TMS320_C1X_C2X_C5X:
            return "TI TMS320C1x/C2x/C5x";
        case LEH_TARGET_ID_TMS320_C3X_C4X:
        case BEH_TARGET_ID_TMS320_C3X_C4X:
            return "TI TMS320C3x/C4x";
        case LEH_TARGET_ID_C80:
        case BEH_TARGET_ID_C80:
            return "TI C80";
        case LEH_TARGET_ID_TMS320_C54X:
        case BEH_TARGET_ID_TMS320_C54X:
            return "TI TMS320C54x";
        case LEH_TARGET_ID_TMS320_C6X:
        case BEH_TARGET_ID_TMS320_C6X:
            return "TI TMS320C6x";
        case LEH_TARGET_ID_TMS320_C28X:
        case BEH_TARGET_ID_TMS320_C28X:
            return "TI TMS320C28x";
        case LEH_COFF_V1_MAGIC:
        case BEH_COFF_V1_MAGIC:
        case LEH_COFF_V2_MAGIC:
        case BEH_COFF_V2_MAGIC:
            switch (target_id)
            {
                case TARGET_ID_TMS320_C1X_C2X_C5X:
                    return "TI TMS320C1x/C2x/C5x";
                case TARGET_ID_TMS320_C3X_C4X:
                    return "TI TMS320C3x/C4x";
                case TARGET_ID_C80:
                    return "TI C80";
                case TARGET_ID_TMS320_C54X:
                    return "TI TMS320C54x";
                case TARGET_ID_TMS320_C6X:
                    return "TI TMS320C6x";
                case TARGET_ID_TMS320_C28X:
                    return "TI TMS320C28x";
            }
            break;
    }

    return "?";
}

unsigned int get_filehdr_size(short ti_coff_version)
{
    switch (ti_coff_version)
    {
        case 0:
            return sizeof (filehdr_v0);
        case 1:
        case 2:
            return sizeof (filehdr_v12);
    }
    return 0;
}
#endif
bool_t is_coff(const char *filename)
{
    filehdr_v12 header;
    bool_t ret = False;
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        fprintf(stderr, "In %s, can not open file %s\n", __FUNCTION__, filename);
        exit(-1);
    }

    if (fread(&header, sizeof (filehdr_v12), 1, file) != 1)
    {
#if __WIN32__
        fprintf(stderr, "Workaround for windows fopen function in %s\n", __FUNCTION__);
#else
        goto out;
#endif
    }
    //printf("In %s, header.f_magic = 0x%x\n", __FUNCTION__, header.f_magic);
    if (header.f_magic != 0xc1 && header.f_magic != 0xc2)
        ret = False;
    else
        ret = True;
  out:
    fclose(file);
    return ret;
}

exception_t load_coff(memory_space_intf * memory_space, const char *filename, addr_type_t addr_type)
{
    //1334
    struct coff_header *coffFile;
    struct stat coff_fstat;
    int tmp_fd = open(filename, O_RDONLY);

    if (tmp_fd == -1)
    {
        fprintf(stderr, "open %s error: %s\n", filename, strerror(errno));
        goto out;
    }
    fstat(tmp_fd, &coff_fstat);
    /* malloc */
    coffFile = mmap(NULL, coff_fstat.st_size, PROT_READ, MAP_PRIVATE, tmp_fd, 0);
    if (coffFile == NULL || coffFile == MAP_FAILED)
    {
        fprintf(stderr, "mmap error: %s\n", strerror(errno));
        goto out;
    }
    filehdr_v12 *filehdr = ((char *) coffFile + 0x0);
    aouthdr *aout_header = ((char *) coffFile + sizeof (filehdr_v12));

#if 0
    printf("In %s, num_sections=%d, magic=0x%x, target_id=0x%x\n", __FUNCTION__, filehdr->f_nscns, filehdr->f_magic,
           filehdr->f_target_id);
    printf("In %s, num_sections=%d\n", __FUNCTION__, filehdr->f_nscns);
    printf("In %s, sizeof(filehdr_v12)=0x%x, entry=0x%x, text_start=0x%x\n", __FUNCTION__, sizeof (filehdr_v12),
           aout_header->o_entry, aout_header->o_text_start);
#endif
    scnhdr_v2 *section_header;

    //section_header = (scnhdr_v2 *)((char*)coffFile + sizeof(filehdr_v12) + sizeof(aouthdr));
    int section_num = filehdr->f_nscns;

    int i;

    for (i = 0; i < section_num; i++)
    {
        section_header =
            (scnhdr_v2 *) ((char *) coffFile + sizeof (filehdr_v12) + sizeof (aouthdr) + i * sizeof (scnhdr_v2));
        if (section_header->s_size != 0 && section_header->s_scnptr != 0 && section_header->s_paddr != 0)
        {
            //printf("In %s, section_header name=%c%c%c%c%c%c%c, paddr=0x%x, vaddr=0x%x, size=0x%x, offset=0x%x\n", __FUNCTION__, section_header->s_name[0], section_header->s_name[1], section_header->s_name[2], section_header->s_name[3], section_header->s_name[4], section_header->s_name[5], section_header->s_name[6], section_header->s_paddr, section_header->s_vaddr, section_header->s_size, section_header->s_scnptr);
            //printf("In %s, section_header name=%s, paddr=0x%x, vaddr=0x%x, size=0x%x, offset=0x%x\n", __FUNCTION__, section_header->s_name, section_header->s_paddr, section_header->s_vaddr, section_header->s_size, section_header->s_scnptr);
            uint8_t *start = (uint8_t *) ((char *) coffFile + section_header->s_scnptr);
            int j = 0;

#if 1
            for (; j < section_header->s_size; j++)
            {
                if (bus_write(8, section_header->s_paddr + j, start[j]) != 0)
                {
                    printf("SKYEYE: write physical address 0x%x error!!!\n", section_header->s_paddr + j);
                    printf("line:%d\n", __LINE__);
                    return Excess_range_exp;
                }
            }
#endif
        }
    }
#if 0
    uint8_t *text_start = (uint8_t *) ((char *) coffFile + 0x536);

    printf("The first instrution is 0x%x\n", *(unsigned long *) text_start);
    /* text loading */
    uint32_t load_addr = 0x810000;
    uint32_t length = 0x66E0;

    i = 0;
#if 1
    for (; i < length; i++)
    {
        if (bus_write(8, load_addr + i, text_start[i]) != 0)
        {
            printf("SKYEYE: write physical address 0x%x error!!!\n", load_addr + i);
            return Excess_range_exp;
        }
    }
#endif
    /* cinit */
    //load_addr = 0x817b68;
    load_addr = 0x817b48;
    length = 0x154;
    //uint8_t* cinit_start = (uint8_t *)((char*)coffFile + 0x6d76);
    //uint8_t* cinit_start = (uint8_t *)((char*)coffFile + 0x6d56);
    uint8_t *cinit_start = (uint8_t *) ((char *) coffFile + 0x6d48);

    i = 0;
    for (; i < length; i++)
    {
        if (bus_write(8, load_addr + i, cinit_start[i]) != 0)
        {
            printf("SKYEYE: write physical address 0x%x error!!!\n", load_addr + i);
            return Excess_range_exp;
        }
    }
    printf("data at cinit is 0x%x\n", *(unsigned long *) cinit_start);
    /* const */
    load_addr = 0x817ca0;
    length = 0x132;
    uint8_t *const_start = (uint8_t *) ((char *) coffFile + 0x6c16);

    i = 0;
    for (; i < length; i++)
    {
        if (bus_write(8, load_addr + i, const_start[i]) != 0)
        {
            printf("SKYEYE: write physical address 0x%x error!!!\n", load_addr + i);
            return Excess_range_exp;
        }
    }
    printf("data at const is 0x%x\n", *(unsigned long *) const_start);
#endif
    sky_pref_t *pref = get_skyeye_pref();
    unsigned long load_base = pref->exec_load_base;
    unsigned long load_mask = pref->exec_load_mask;

    pref->exec_load_base = 0x0;
    pref->exec_load_mask = 0xFFFFFFFF;
    skyeye_config_t *config = get_current_config();

    //config->start_address = 0x816320;
    config->start_address = aout_header->o_entry;

  out:
    close(tmp_fd);
    //sleep(10);
    return No_exp;
}

void coff_styp_to_sec_flags(scnhdr_v2 * internal_s, uint32_t * flags_ptr)
{
    //struct scnhdr_v2 *internal_s = (struct scnhdr_v2 *) hdr;
    uint32_t styp_flags = internal_s->s_flags;
    uint32_t sec_flags = 0;

    if (styp_flags & STYP_NOLOAD)
        sec_flags |= SEC_NEVER_LOAD;

    /* For 386 COFF, at least, an unloadable text or data section is
     * actually a shared library section.  */
    if ((styp_flags & STYP_TEXT)
        || (styp_flags & STYP_ECOFF_INIT)
        || (styp_flags & STYP_ECOFF_FINI)
        || (styp_flags & STYP_DYNAMIC)
        || (styp_flags & STYP_LIBLIST)
        || (styp_flags & STYP_RELDYN)
        || styp_flags == STYP_CONFLIC
        || (styp_flags & STYP_DYNSTR) || (styp_flags & STYP_DYNSYM) || (styp_flags & STYP_HASH))
    {
        if (sec_flags & SEC_NEVER_LOAD)
            sec_flags |= SEC_CODE | SEC_COFF_SHARED_LIBRARY;
        else
            sec_flags |= SEC_CODE | SEC_LOAD | SEC_ALLOC;
    } else if ((styp_flags & STYP_DATA)
               || (styp_flags & STYP_RDATA)
               || (styp_flags & STYP_SDATA)
               || styp_flags == STYP_PDATA
               || styp_flags == STYP_XDATA || (styp_flags & STYP_GOT) || styp_flags == STYP_RCONST)
    {
        if (sec_flags & SEC_NEVER_LOAD)
            sec_flags |= SEC_DATA | SEC_COFF_SHARED_LIBRARY;
        else
            sec_flags |= SEC_DATA | SEC_LOAD | SEC_ALLOC;
        if ((styp_flags & STYP_RDATA) || styp_flags == STYP_PDATA || styp_flags == STYP_RCONST)
            sec_flags |= SEC_READONLY;
    } else if ((styp_flags & STYP_BSS) || (styp_flags & STYP_SBSS))
        sec_flags |= SEC_ALLOC;
    else if ((styp_flags & STYP_INFO) || styp_flags == STYP_COMMENT)
        sec_flags |= SEC_NEVER_LOAD;
    else if ((styp_flags & STYP_LITA) || (styp_flags & STYP_LIT8) || (styp_flags & STYP_LIT4))
        sec_flags |= SEC_DATA | SEC_LOAD | SEC_ALLOC | SEC_READONLY;
    else if (styp_flags & STYP_ECOFF_LIB)
        sec_flags |= SEC_COFF_SHARED_LIBRARY;
    else
        sec_flags |= SEC_ALLOC | SEC_LOAD;

    *flags_ptr = sec_flags;
}

void styp_to_sec_flags(scnhdr_v2 * internal_s, uint32_t * flags_ptr)
{
    //struct internal_scnhdr *internal_s = (struct internal_scnhdr *) hdr;
    unsigned long styp_flags = internal_s->s_flags;
    uint32_t sec_flags;
    uint32_t result = 0;
    uint32_t is_dbg = 0;
    char *name = internal_s->s_name;

    if (CONST_STRNEQ(name, DOT_DEBUG) || CONST_STRNEQ(name, DOT_ZDEBUG)
#ifdef COFF_LONG_SECTION_NAMES
        || CONST_STRNEQ(name, GNU_LINKONCE_WI) || CONST_STRNEQ(name, GNU_LINKONCE_WT)
#endif
        || CONST_STRNEQ(name, ".stab"))
        is_dbg = TRUE;

    /* Assume read only unless IMAGE_SCN_MEM_WRITE is specified.  */
    //sec_flags = SEC_READONLY;

    /* If section disallows read, then set the NOREAD flag. */
    if ((styp_flags & IMAGE_SCN_MEM_READ) == 0)
        sec_flags |= SEC_COFF_NOREAD;

    /* Process each flag bit in styp_flags in turn.  */
    while (styp_flags)
    {
        unsigned long flag = styp_flags & -styp_flags;
        char *unhandled = NULL;

        styp_flags &= ~flag;

        /* We infer from the distinct read/write/execute bits the settings
         * of some of the bfd flags; the actual values, should we need them,
         * are also in pei_section_data (abfd, section)->pe_flags.  */

        switch (flag)
        {
            case STYP_DSECT:
                //unhandled = "STYP_DSECT";
                sec_flags |= SEC_ALLOC | SEC_LOAD;
                break;
            case STYP_GROUP:
                unhandled = "STYP_GROUP";
                sec_flags |= SEC_ALLOC | SEC_LOAD;
                break;
            case STYP_COPY:
                unhandled = "STYP_COPY";
                sec_flags |= SEC_ALLOC | SEC_LOAD;
                break;
            case STYP_OVER:
                unhandled = "STYP_OVER";
                sec_flags |= SEC_ALLOC | SEC_LOAD;
                break;
#ifdef SEC_NEVER_LOAD
            case STYP_NOLOAD:
                sec_flags |= SEC_NEVER_LOAD;
                break;
#endif
            case IMAGE_SCN_MEM_READ:
                sec_flags &= ~SEC_COFF_NOREAD;
                break;
            case IMAGE_SCN_TYPE_NO_PAD:
                /* Skip.  */
                break;
            case IMAGE_SCN_LNK_OTHER:
                unhandled = "IMAGE_SCN_LNK_OTHER";
                break;
            case IMAGE_SCN_MEM_NOT_CACHED:
                unhandled = "IMAGE_SCN_MEM_NOT_CACHED";
                break;
            case IMAGE_SCN_MEM_NOT_PAGED:
                /* Generate a warning message rather using the 'unhandled'
                 * variable as this will allow some .sys files generate by
                 * other toolchains to be processed.  See bugzilla issue 196.  */
                printf("Warning: Ignoring section flag IMAGE_SCN_MEM_NOT_PAGED in section %s", name);
                break;
            case IMAGE_SCN_MEM_EXECUTE:
                sec_flags |= SEC_CODE;
                break;
            case IMAGE_SCN_MEM_WRITE:
                sec_flags &= ~SEC_READONLY;
                break;
            case IMAGE_SCN_MEM_DISCARDABLE:
                /* The MS PE spec says that debug sections are DISCARDABLE,
                 * but the presence of a DISCARDABLE flag does not necessarily
                 * mean that a given section contains debug information.  Thus
                 * we only set the SEC_DEBUGGING flag on sections that we
                 * recognise as containing debug information.  */
                if (is_dbg
#ifdef _COMMENT
                    || strcmp(name, _COMMENT) == 0
#endif
                    )
                {
                    sec_flags |= SEC_DEBUGGING | SEC_READONLY;
                }
                break;
            case IMAGE_SCN_MEM_SHARED:
                sec_flags |= SEC_COFF_SHARED;
                break;
            case IMAGE_SCN_LNK_REMOVE:
                if (!is_dbg)
                    sec_flags |= SEC_EXCLUDE;
                break;
            case IMAGE_SCN_CNT_CODE:
                sec_flags |= SEC_CODE | SEC_ALLOC | SEC_LOAD;
                break;
            case IMAGE_SCN_CNT_INITIALIZED_DATA:
                if (is_dbg)
                    sec_flags |= SEC_DEBUGGING;
                else
                    sec_flags |= SEC_DATA | SEC_ALLOC | SEC_LOAD;
                break;
            case IMAGE_SCN_CNT_UNINITIALIZED_DATA:
                sec_flags |= SEC_ALLOC;
                break;
            case IMAGE_SCN_LNK_INFO:
                /* We mark these as SEC_DEBUGGING, but only if COFF_PAGE_SIZE is
                 * defined.  coff_compute_section_file_positions uses
                 * COFF_PAGE_SIZE to ensure that the low order bits of the
                 * section VMA and the file offset match.  If we don't know
                 * COFF_PAGE_SIZE, we can't ensure the correct correspondence,
                 * and demand page loading of the file will fail.  */
#ifdef COFF_PAGE_SIZE
                sec_flags |= SEC_DEBUGGING;
#endif
                break;
            case IMAGE_SCN_LNK_COMDAT:
                /* COMDAT gets very special treatment.  */
                //sec_flags = handle_COMDAT (abfd, sec_flags, hdr, name, section);
                break;
            default:
                /* Silently ignore for now.  */
                sec_flags |= SEC_ALLOC | SEC_LOAD;
                break;
        }

        /* If the section flag was not handled, report it here.  */
        if (unhandled != NULL)
        {
            //printf("(%s): Section flag %s (0x%x) ignored", name, unhandled, flag);
            result = FALSE;
        }
    }

#if defined (COFF_LONG_SECTION_NAMES) && defined (COFF_SUPPORT_GNU_LINKONCE)
    /* As a GNU extension, if the name begins with .gnu.linkonce, we
     * only link a single copy of the section.  This is used to support
     * g++.  g++ will emit each template expansion in its own section.
     * The symbols will be defined as weak, so that multiple definitions
     * are permitted.  The GNU linker extension is to actually discard
     * all but one of the sections.  */
    if (CONST_STRNEQ(name, ".gnu.linkonce"))
        sec_flags |= SEC_LINK_ONCE | SEC_LINK_DUPLICATES_DISCARD;
#endif

    if (flags_ptr)
        *flags_ptr = sec_flags;

    return result;
}

int check_s_flags_print(scnhdr_v2 * section_header)
{
    uint32_t sec_flags;
    char *comma = "";

    //coff_styp_to_sec_flags(section_header, &sec_flags);
    styp_to_sec_flags(section_header, &sec_flags);

    //if (section_header->s_nreloc != 0) {
    //      sec_flags |= REC_RELOC;
    //}
    if (section_header->s_scnptr != 0)
    {
        sec_flags |= SEC_HAS_CONTENTS;
    }
#if 0
#define PF(x, y) \
  if (sec_flags & x) { printf ("%s%s", comma, y); comma = ", "; }
#else
#define PF(x, y) \
  if (sec_flags & x) { printf ("%s%s", comma, y); comma = ", "; }
#endif

    printf("\033[32m");
    PF(SEC_HAS_CONTENTS, "CONTENTS");
    PF(SEC_ALLOC, "ALLOC");
    PF(SEC_CONSTRUCTOR, "CONSTRUCTOR");
    PF(SEC_LOAD, "LOAD");
    PF(SEC_RELOC, "RELOC");
    PF(SEC_READONLY, "READONLY");
    PF(SEC_CODE, "CODE");
    PF(SEC_DATA, "DATA");
    PF(SEC_ROM, "ROM");
    PF(SEC_DEBUGGING, "DEBUGGING");
    PF(SEC_NEVER_LOAD, "NEVER_LOAD");
    PF(SEC_EXCLUDE, "EXCLUDE");
    PF(SEC_SORT_ENTRIES, "SORT_ENTRIES");
#if 0
    if (bfd_get_arch(abfd) == bfd_arch_tic54x)
    {
        PF(SEC_TIC54X_BLOCK, "BLOCK");
        PF(SEC_TIC54X_CLINK, "CLINK");
    }
#endif
    PF(SEC_SMALL_DATA, "SMALL_DATA");
#if 0
    if (bfd_get_flavour(abfd) == bfd_target_coff_flavour)
        PF(SEC_COFF_SHARED, "SHARED");
#endif
    PF(SEC_THREAD_LOCAL, "THREAD_LOCAL");
    PF(SEC_GROUP, "GROUP");

    printf("\033[0m");
    printf("\n");

    return 1;
}

int check_s_flags(scnhdr_v2 * section_header)
{
    uint32_t sec_flags;
    char *comma = "";

    //coff_styp_to_sec_flags(section_header, &sec_flags);
    styp_to_sec_flags(section_header, &sec_flags);

    //if (section_header->s_nreloc != 0) {
    //      sec_flags |= REC_RELOC;
    //}
    if (section_header->s_scnptr != 0)
    {
        sec_flags |= SEC_HAS_CONTENTS;
    }

    if ((sec_flags & SEC_CODE) || (sec_flags & SEC_DATA))
    {
        return 1;
    } else
    {
        return 0;
    }

}

#if 0
char *comma = "";

#define PF(x, y) \
	if (section_header->s_flags & x) { printf("%s%s", comma, y); comma = ", ";}

printf("\033[32m");
PF(STYP_REG, "STYP_REG");
PF(STYP_DSECT, "STYP_DSECT");
PF(STYP_NOLOAD, "STYP_NOLOAD");
PF(STYP_GROUP, "STYP_GROUP");
PF(STYP_PAD, "STYP_PAD");
PF(STYP_COPY, "STYP_COPY");
PF(STYP_TEXT, "STYP_TEXT");
PF(STYP_DATA, "STYP_DATA");
PF(STYP_BSS, "STYP_BSS");
PF(S_NEWFCN, "S_NEWFCN");
PF(STYP_INFO, "STYP_INFO");
PF(STYP_OVER, "STYP_OVER");
PF(STYP_LIB, "STYP_LIB");
PF(STYP_BLOCK, "STYP_BLOCK");
PF(STYP_PASS, "STYP_PASS");
PF(STYP_CLINK, "STYP_CLINK");
PF(STYP_VECTOR, "STYP_VECTOR");
PF(STYP_PADDED, "STYP_PADDED");
printf("\033[0m");
printf("\n");
#endif

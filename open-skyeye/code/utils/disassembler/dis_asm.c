/*
 * dis_asm.c - disassemble implementation for all the arch. We refer to some
 * source code from binutils.
 * 
 * Copyright (C) 2003-2010 Skyeye Develop Group
 * for help please send mail to <skyeye-developer@lists.sf.linuxforum.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */
/*
 * 01/16/2010   Michael.Kang  <blackfin.kang@gmail.com>
 */
#include "config.h"
#include <string.h>
#include <dis-asm.h>
#include <assert.h>
#include <libiberty.h>
#include <stdlib.h>
#include <stdarg.h>

#include "skyeye_types.h"
#include "skyeye_arch.h"
#include "skyeye_ram.h"
#include "skyeye_addr_space.h"
#include "skyeye_interface.h"
#include "skyeye_iface.h"
#include "skyeye_obj.h"
#include "skyeye_core_intf.h"

#include "skyeye_disassemble.h"

/* The number of zeroes we want to see before we start skipping them.
 * The number is arbitrarily chosen.  */

#define DEFAULT_SKIP_ZEROES 8

/* The number of zeroes to skip at the end of a section.  If the
 * number of zeroes at the end is between SKIP_ZEROES_AT_END and
 * SKIP_ZEROES, they will be disassembled.  If there are fewer than
 * SKIP_ZEROES_AT_END, they will be skipped.  This is a heuristic
 * attempt to avoid disassembling zeroes inserted by section
 * alignment.  */

#define DEFAULT_SKIP_ZEROES_AT_END 3

/* The sorted symbol table.  */
static asymbol **sorted_syms;

/* Number of symbols in `sorted_syms'.  */
static long sorted_symcount = 0;

/* Endianness to disassemble for, or default if BFD_ENDIAN_UNKNOWN.  */
static enum bfd_endian endian = BFD_ENDIAN_LITTLE;

/* Architecture to disassemble for, or default if NULL.  */
static char *machine = NULL;
static char cpu[100];
static conf_object_t *cpu_global;

/* Target specific options to the disassembler.  */
static char *disassembler_options = NULL;

struct disassemble_info disasm_info;
bfd *abfd;
disassembler_ftype disassemble_fn = NULL;

/* Exit status.  */
static int exit_status = 0;

conf_object_t *get_simics_or_skyeye_core(conf_object_t *);

static int disasm_read_memory(bfd_vma memaddr, bfd_byte * myaddr, unsigned int length, struct disassemble_info *info)
{
    exception_t ret;
    int i;
    conf_object_t *core = get_conf_obj(cpu);
    conf_object_t *op_core = get_simics_or_skyeye_core(core);
    memory_space_intf *space_iface = (memory_space_intf *) SKY_get_iface(core, MEMORY_SPACE_INTF_NAME);

    if (space_iface != NULL)
    {
        ret = space_iface->read(op_core, memaddr, myaddr, length);
    }
    return 0;
}

static bfd_boolean symbol_is_valid(asymbol * sym, struct disassemble_info *info)
{
    return FALSE;
}

static void objdump_print_address(bfd_vma vma, struct disassemble_info *info)
{
}

/* Determine if the given address has a symbol associated with it.  */

static int objdump_symbol_at_address(bfd_vma vma, struct disassemble_info *info)
{
    asymbol *sym;

    //sym = find_symbol_for_address (vma, info, NULL);
    return (sym != NULL && (bfd_asymbol_value(sym) == vma));
}

/*disassemble buf for web_sprintf */
char web_disassemble_buf[1024];

void clear_web_disassemble_buf()
{
    web_disassemble_buf[0] = '\0';
}

char *read_web_disassemble_buf()
{
    return web_disassemble_buf;
}

/*
 *overload sprintf function for cloudskyeye.
 *We will calculate the length of the data buffer has been first
 *and continue to write the data buffer is not refreshed.
 */
int web_sprintf(char *str, const char *format, ...)
{
    va_list ap;
    int size = 0;
    int str_len = 0;

    for (size = 0; '\0' != str[size]; size++) ;
    va_start(ap, format);
    str_len = vsprintf(str + size, format, ap);
    return str_len;
}

/*spacial for cloudskyeye disassemble */
void web_init_disassemble(char *cpuname, char *archname, endian_t endianess)
{
    abfd = malloc(sizeof (bfd));
    /*register disassemble output function with web_sprintf. */
    init_disassemble_info(&disasm_info, web_disassemble_buf, (fprintf_ftype) web_sprintf);
    disasm_info.print_address_func = objdump_print_address;
    disasm_info.symbol_at_address_func = objdump_symbol_at_address;

    /*
     * choose arch infor since we will select different disassemble function.
     */
    const bfd_arch_info_type *info = bfd_scan_arch(archname);

    if (info == NULL)
    {
        printf("Can't use supplied machine %s\n", archname);
        return;
    }

    abfd->arch_info = info;

    if (endianess == Big_endian)
    {
        disasm_info.display_endian = disasm_info.endian = BFD_ENDIAN_BIG;
    } else
    {
        /* set the default endianess is BFD_ENDIAN_LITTLE */
        disasm_info.display_endian = disasm_info.endian = BFD_ENDIAN_LITTLE;
    }
    /*
     * endian selection
     */
    if (endian != BFD_ENDIAN_UNKNOWN)
    {
        struct bfd_target *xvec;

        xvec = xmalloc(sizeof (struct bfd_target));
        //memcpy (xvec, abfd->xvec, sizeof(struct bfd_target));
        xvec->byteorder = disasm_info.display_endian;
        abfd->xvec = xvec;
    }
    /* Get a disassemble function according to the arch and endian of abfd */
    disassemble_fn = disassembler(abfd);
    if (!disassemble_fn)
    {
        printf("Can't disassemble for architecture %s\n", bfd_printable_arch_mach(bfd_get_arch(abfd), 0));
        exit_status = 1;
        return;
    }

    disasm_info.flavour = bfd_get_flavour(abfd);
    disasm_info.arch = bfd_get_arch(abfd);
    disasm_info.mach = bfd_get_mach(abfd);
    disasm_info.disassembler_options = disassembler_options;
    disasm_info.octets_per_byte = bfd_octets_per_byte(abfd);
    disasm_info.skip_zeroes = DEFAULT_SKIP_ZEROES;
    disasm_info.skip_zeroes_at_end = DEFAULT_SKIP_ZEROES_AT_END;
    disasm_info.disassembler_needs_relocs = FALSE;

    disasm_info.symtab = sorted_syms;
    disasm_info.symtab_size = sorted_symcount;
    disasm_info.read_memory_func = disasm_read_memory;
    free(sorted_syms);
    memcpy(cpu, cpuname, strlen(cpuname) + 1);
}

void init_disassemble(char *cpuname, char *archname, endian_t endianess)
{
    abfd = malloc(sizeof (bfd));
    init_disassemble_info(&disasm_info, stdout, (fprintf_ftype) fprintf);
    disasm_info.print_address_func = objdump_print_address;
    disasm_info.symbol_at_address_func = objdump_symbol_at_address;

    /*FIXME : It is simple method to use the bfd's tic6x but it is not good! */
    const bfd_arch_info_type *info = bfd_scan_arch(archname);

    if (info == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can't use supplied machine %s\n", archname);
        return;
    }

    abfd->arch_info = info;
    if (endianess == Big_endian)
    {
        disasm_info.display_endian = disasm_info.endian = BFD_ENDIAN_BIG;
    } else
    {
        /* set the default endianess is BFD_ENDIAN_LITTLE */
        disasm_info.display_endian = disasm_info.endian = BFD_ENDIAN_LITTLE;
    }
    /*
     * endian selection
     */
    if (endian != BFD_ENDIAN_UNKNOWN)
    {
        struct bfd_target *xvec;

        xvec = xmalloc(sizeof (struct bfd_target));
        //memcpy (xvec, abfd->xvec, sizeof (struct bfd_target));
        xvec->byteorder = disasm_info.endian;
        abfd->xvec = xvec;
    }
    /* Get a disassemble function according to the arch and endian of abfd */
    disassemble_fn = disassembler(abfd);
    if (!disassemble_fn)
    {
        /*
         * non_fatal (_("Can't disassemble for architecture %s\n"),
         * bfd_printable_arch_mach (bfd_get_arch (abfd), 0));
         */
        skyeye_log(Error_log, __FUNCTION__, "Can't disassemble for architecture %s\n",
                   bfd_printable_arch_mach(bfd_get_arch(abfd), 0));
        exit_status = 1;
        return;
    }
    disasm_info.flavour = bfd_get_flavour(abfd);
    disasm_info.arch = bfd_get_arch(abfd);
    disasm_info.mach = bfd_get_mach(abfd);
    disasm_info.disassembler_options = disassembler_options;
    disasm_info.octets_per_byte = bfd_octets_per_byte(abfd);
    disasm_info.skip_zeroes = DEFAULT_SKIP_ZEROES;
    disasm_info.skip_zeroes_at_end = DEFAULT_SKIP_ZEROES_AT_END;
    disasm_info.disassembler_needs_relocs = FALSE;

    disasm_info.symtab = sorted_syms;
    disasm_info.symtab_size = sorted_symcount;
    disasm_info.read_memory_func = disasm_read_memory;
    free(sorted_syms);
    memcpy(cpu, cpuname, strlen(cpuname) + 1);
}

/*special for cloudskyeye disassemble */
void web_disassemble(char *cpuname, char *archname, endian_t endianess, generic_address_t addr)
{
    /* if abfd is NULL, we need to run initilization of disassembler */
    //if (abfd == NULL){
    web_init_disassemble(cpuname, archname, endianess);
    //}
    /* if disassemble_fn is NULL, some errors happened. */
    if (disassemble_fn == NULL)
    {
        return;
    }

    disassemble_fn(addr, &disasm_info);
}

/**********************************************************************/

static int bfd_disasm_read_memory(bfd_vma memaddr, bfd_byte * myaddr, unsigned int length,
                                  struct disassemble_info *info)
{

    exception_t ret;
    int i;
    memory_space_intf *space_iface = (memory_space_intf *) SKY_get_iface(cpu_global, MEMORY_SPACE_INTF_NAME);

    if (space_iface != NULL)
    {
        for (i = 0; i < length; i++)
        {
            space_iface->read(cpu_global, memaddr + i, myaddr + i, 1);
        }
    }
    return 0;
}

void bfd_init_disassemble(char *archname, endian_t endianess)
{
    abfd = malloc(sizeof (bfd));
    /*register disassemble output function with web_sprintf. */
    init_disassemble_info(&disasm_info, web_disassemble_buf, (fprintf_ftype) web_sprintf);
    disasm_info.print_address_func = objdump_print_address;
    disasm_info.symbol_at_address_func = objdump_symbol_at_address;

    /*
     * choose arch infor since we will select different disassemble function.
     */
    const bfd_arch_info_type *info = bfd_scan_arch(archname);

    if (info == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can't use supplied machine %s\n", archname);
        return;
    }
    abfd->arch_info = info;

    if (endianess == Big_endian)
    {
        disasm_info.display_endian = disasm_info.endian = BFD_ENDIAN_BIG;
    } else
    {
        /* set the default endianess is BFD_ENDIAN_LITTLE */
        disasm_info.display_endian = disasm_info.endian = BFD_ENDIAN_LITTLE;
    }
    /*
     * endian selection
     */
    if (endian != BFD_ENDIAN_UNKNOWN)
    {
        struct bfd_target *xvec;

        xvec = xmalloc(sizeof (struct bfd_target));
        //memcpy (xvec, abfd->xvec, sizeof(struct bfd_target));
        xvec->byteorder = disasm_info.display_endian;
        abfd->xvec = xvec;
    }
    /* Get a disassemble function according to the arch and endian of abfd */
    disassemble_fn = disassembler(abfd);
    if (!disassemble_fn)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can't disassemble for architecture %s\n",
                   bfd_printable_arch_mach(bfd_get_arch(abfd), 0));
        exit_status = 1;
        return;
    }

    disasm_info.flavour = bfd_get_flavour(abfd);
    disasm_info.arch = bfd_get_arch(abfd);
    disasm_info.mach = bfd_get_mach(abfd);
    disasm_info.disassembler_options = disassembler_options;
    disasm_info.octets_per_byte = bfd_octets_per_byte(abfd);
    disasm_info.skip_zeroes = DEFAULT_SKIP_ZEROES;
    disasm_info.skip_zeroes_at_end = DEFAULT_SKIP_ZEROES_AT_END;
    disasm_info.disassembler_needs_relocs = FALSE;

    disasm_info.symtab = sorted_syms;
    disasm_info.symtab_size = sorted_symcount;
    disasm_info.read_memory_func = bfd_disasm_read_memory;
    disassemble_init_for_target(&disasm_info);
    free(sorted_syms);
}

void bfd_disassemble(char *archname, endian_t endianess, generic_address_t addr)
{
    bfd_init_disassemble(archname, endianess);
    /* if disassemble_fn is NULL, some errors happened. */
    if (disassemble_fn == NULL)
    {
        return;
    }

    disassemble_fn(addr, &disasm_info);
}

conf_object_t *get_simics_or_skyeye_core(conf_object_t * core)
{
    conf_object_t *op_core;

    if (SKY_has_attribute(core, "simics_core") == No_exp)
    {
        attr_value_t core_attr = SKY_get_attribute(core, "simics_core");

        op_core = (conf_object_t *) SKY_attr_object(core_attr);
    } else
    {
        op_core = core;
    }
    return op_core;
}

char *disassemble_func(conf_object_t * core, generic_address_t addr)
{
    cpu_global = get_simics_or_skyeye_core(core);
    clear_web_disassemble_buf();
    core_info_intf *iface = SKY_get_iface(core, CORE_INFO_INTF_NAME);
    endian_t endianess = iface->get_endian(core);

    //endian_t endianess = Little_endian;
    char *archname = iface->get_architecture(core);

    bfd_disassemble(archname, endianess, addr);
    return read_web_disassemble_buf();
}

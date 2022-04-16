/*  armsym.c -- Main instruction emulation:  SA11x Instruction Emulator.
 * Copyright (C) 2001 Princeton University
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

/**
* @file symbol.c
* @brief the symbol management
* @author Michael.Kang blackfin.kang@gmail.com
* @version
* @date 2011-04-30
*/

#include <config.h>
#include <assert.h>
#include <stdlib.h>
#include "bfd.h"
#include <search.h>
#include "skyeye_symbol.h"
#include "skyeye_mm.h"
#include "skyeye_log.h"
#include "symbol.h"
#include "portable/portable.h"

/*
 * FIXME: GNU hash table isn't exist everywhere!!!
 */

static long storage_needed = 0;
static asymbol **symbol_table = NULL;
static unsigned long number_of_symbols = 0;

static long remove_useless_symbols(asymbol ** symbols, long count)
{
    register asymbol **in_ptr = symbols, **out_ptr = symbols;

    while (--count >= 0)
    {
        asymbol *sym = *in_ptr++;

        if (strstr(sym->name, "gcc2_compiled"))
            continue;
        if (strstr(sym->name, "\*ABS\*"))
            continue;
        if (sym->name == NULL || sym->name[0] == '\0')
            continue;
        if (sym->flags & (BSF_DEBUGGING))
            continue;
        if (bfd_is_und_section(sym->section) || bfd_is_com_section(sym->section))
            continue;

        *out_ptr++ = sym;
    }
    return out_ptr - symbols;
}

static int compare_symbols(const void *ap, const void *bp)
{
    const asymbol *a = *(const asymbol **) ap;
    const asymbol *b = *(const asymbol **) bp;

    if (bfd_asymbol_value(a) > bfd_asymbol_value(b))
        return 1;
    else if (bfd_asymbol_value(a) < bfd_asymbol_value(b))
        return -1;
    return 0;
}

/**************************************************************************
  This function read the symbol list and store into a table
  It then generates a hash table based on the value of function symbol
  and the data is the pointer to struct funcsym defined in armsym.h

  The GNU hash table is used.
**************************************************************************/

/***************
 * added by ksh
 ***************/

/**
* @brief  initialization of a symbol table
*
* @param filename
* @param arch_name
*/
void init_skyeye_symbol_table(char *filename)
{
    const char *target_name = "elf32-sparc";
    asymbol *symptr;
    int key;
    bfd *abfd;

    if (!filename)
    {
        skyeye_info("Can not get correct kernel filename!Maybe your skyeye.conf have something wrong!\n");
        return;
    }
    //abfd = bfd_openr (filename, get_bfd_target(arch_name));
    abfd = bfd_openr(filename, target_name);

    if (!bfd_check_format(abfd, bfd_object))
    {
        bfd_close(abfd);
        skyeye_log(Debug_log, __FUNCTION__, "wrong bfd format\n");
        return;
    }

    storage_needed = bfd_get_symtab_upper_bound(abfd);
    if (storage_needed < 0)
    {
        printf("FAIL\n");
        exit(0);
    }

    symbol_table = (asymbol **) skyeye_mm_zero(storage_needed);
    if (symbol_table == NULL)
    {
        fprintf(stderr, "Can not alloc memory for symbol table.\n");
        exit(0);
    }

    number_of_symbols = bfd_canonicalize_symtab(abfd, symbol_table);
    number_of_symbols = remove_useless_symbols(symbol_table, number_of_symbols);
#if 0
    int i = 0;

    for (i = 0; i < number_of_symbols; i++)
    {
        printf("symbol[%d] : %s\n", i, bfd_asymbol_name(symbol_table[i]));
    }
#endif
    if (number_of_symbols < 0)
    {
        printf("faild for bfd getting symbol number\n");
        exit(0);
    }
    qsort(symbol_table, number_of_symbols, sizeof (asymbol *), compare_symbols);

    return;
}

/**
* @brief get the symbol of a given address
*
* @param address
*
* @return
*/
char *skyeye_get_sym(generic_address_t address)
{
    static int last_sym = -1;
    static int function_index;
    long int min = -1;
    long max = number_of_symbols;
    char *sym_name = NULL;
    int sym, bestaddr;

    sym = bestaddr = 0;
    bfd_vma sa = 0;

    while (min < max - 1)
    {
        sym = (min + max) / 2;
        sa = bfd_asymbol_value(symbol_table[sym]);
        /*printf("checking %4d %08x %s\n",
         * sym, sa, bfd_asymbol_name (symtab[sym])); */
        if (sa > address)
        {
            max = sym;
        } else if (sa < address)
        {
            min = sym;
        } else
        {
            min = sym;
            break;
        }
    }
    if (min != -1 && min != last_sym)
    {
        bestaddr = bfd_asymbol_value(symbol_table[min]);
        sym_name = bfd_asymbol_name(symbol_table[min]);
#if 0
        if (sym_name == NULL)
        {
            sym_name = bfd_asymbol_name(symbol_table[function_index]);
        } else
        {
            function_index = min;
            printf("<skyeye>min: %d, function_index: %d, sym_name: %s\n", min, function_index, sym_name);
        }
#endif
        last_sym = min;
    } else if (min != -1)
    {
        sym_name = bfd_asymbol_name(symbol_table[min]);
    }
    return sym_name;
}

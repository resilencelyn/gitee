#include "config.h"
#include <string.h>
#include <dis-asm.h>
#include <assert.h>
#include <libiberty.h>
#include <stdlib.h>
#include <stdarg.h>

#include <skyeye_types.h>
#include <skyeye_mm.h>
#include <skyeye_iface.h>
#include <skyeye_core_intf.h>
#include <skyeye_int_register.h>
#include <memory_space.h>
#include <skyeye_attribute.h>

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

/* Target specific options to the disassembler.  */
static char *disassembler_options = NULL;

struct disassemble_info disasm_info;
bfd *abfd;
disassembler_ftype disassemble_fn = NULL;
static int exit_status = 0;

static conf_object_t *space_obj = NULL;
static memory_space_intf *space_iface = NULL;

/*disassemble buf for sprintf */
char disassemble_buf[1024];

void clear_disassemble_buf()
{
    disassemble_buf[0] = '\0';
}

char *read_disassemble_buf()
{
    return disassemble_buf;
}

static void objdump_print_address(bfd_vma vma, struct disassemble_info *info)
{
    return;
}

/* Determine if the given address has a symbol associated with it.  */

static int objdump_symbol_at_address(bfd_vma vma, struct disassemble_info *info)
{
    asymbol *sym;

    return (sym != NULL && (bfd_asymbol_value(sym) == vma));
}

static int disasm_read_memory(bfd_vma memaddr, bfd_byte * myaddr, unsigned int length, struct disassemble_info *info)
{
    int ret;
    int i;

    if ((space_iface != NULL) && (space_obj != NULL))
    {
        for (i = 0; i < length; i++)
        {
            ret = space_iface->read(space_obj, memaddr + i, (myaddr + i), 1);
        }
    } else
    {
        printf("can not read the memory as memory space interface or space object is NULL\n");
    }
    return 0;
}

/*
 *overload sprintf function for cloudskyeye.
 *We will calculate the length of the data buffer has been first
 *and continue to write the data buffer is not refreshed.
 */
int dis_sprintf(char *str, const char *format, ...)
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
void init_disassemble(conf_object_t * obj)
{
    abfd = skyeye_mm_zero(sizeof (bfd));
    /*register disassemble output function with sprintf. */
    init_disassemble_info(&disasm_info, disassemble_buf, (fprintf_ftype) dis_sprintf);
    disasm_info.print_address_func = objdump_print_address;
    disasm_info.symbol_at_address_func = objdump_symbol_at_address;

    space_iface = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);
    const core_info_intf *core_iface = (core_info_intf *) SKY_get_iface(obj, CORE_INFO_INTF_NAME);

    if ((space_iface == NULL) || (core_iface == NULL))
    {
        printf("can not get %s interface\n", (space_iface == NULL) ? "memory" : "core");
        return;
    }

    space_obj = obj;
#if 0
    attr_value_t space_value = SKY_get_attribute(obj, "memory_object");

    space_obj = SKY_attr_object_or_nil(space_value);
#endif

    const char *arch_name = core_iface->get_architecture(obj);
    const bfd_arch_info_type *info = bfd_scan_arch(arch_name);

    if (info == NULL)
    {
        printf("<skyeye>Can't use supplied arch %s\n", arch_name);
        return;
    }

    abfd->arch_info = info;
    endian_t endian = core_iface->get_endian(obj);

    if (endian == Big_endian)
    {
        disasm_info.display_endian = disasm_info.endian = BFD_ENDIAN_BIG;
    } else
    {
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
        /*
         * non_fatal (_("Can't disassemble for architecture %s\n"),
         * bfd_printable_arch_mach(bfd_get_arch(abfd), 0));
         */
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

    return;
}

void disassemble(conf_object_t * obj, generic_address_t addr)
{
    if (abfd == NULL)
    {
        init_disassemble(obj);
    }
    if ((abfd == NULL) || (disassemble_fn == NULL))
    {
        printf("<skyeye>can not disassemble as bfd not be initialized");
        return;
    }
    disassemble_fn(addr, &disasm_info);

    return;
}

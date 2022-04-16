/*
 * =====================================================================================
 *
 *       Filename:  x86_regdefs.c
 *
 *    Description:  necessary x86 definition for skyeye debugger
 *
 *        Version:  1.0
 *        Created:  15/04/08 15:45:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

/*
 * 2009-11-02 Michael.Kang, rewrite according to the new framework.
 */
#include "skyeye.h"
#include "skyeye2gdb.h"
#include "skyeye_types.h"
#include "skyeye_arch.h"
#include <skyeye_iface.h>
#include <skyeye_core_intf.h>

#define NUMREGS 16

/* Number of bytes of registers.  */
#define NUMREGBYTES (NUMREGS * 4)
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  x86_regiter_raw_size
 *  Description:  This function returns the number of bytes register for the
 *  given register number.
 *  In the case of the x86 architecture, all the registers's got 4 bytes.
 * =====================================================================================
 */
static int x86_register_raw_size(int x)
{
    return 4;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  x86_regiter_byte
 *  Description:  This function return the byte from which the register is store
 *  on.
 * =====================================================================================
 */
static int x86_register_byte(int x)
{
    return 4 * x;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  x86_store_register
 *  Description:  This function stores the value given in the 'memory' pointer,
 *  to the 'x' register.
 * =====================================================================================
 */
static int x86_store_register(conf_object_t * obj, int rn, unsigned char *memory)
{
    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(obj, CORE_INFO_INTF_NAME);

    skyeye_reg_intf *reg_intf = (skyeye_reg_intf *) SKY_get_iface(obj, SKYEYE_REG_INTF);

    reg_intf->set_regvalue_by_id(obj, frommem(core_info->get_endian(obj), memory), rn);

    return 0;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  x86_fetch_register
 *  Description:  Send the register information out to memory.
 * =====================================================================================
 */
static int x86_fetch_register(conf_object_t * obj, int rn, unsigned char *memory)
{
    uint32 val;
    skyeye_reg_intf *reg_intf = (skyeye_reg_intf *) SKY_get_iface(obj, SKYEYE_REG_INTF);
    uint32 regval = reg_intf->get_regvalue_by_id(obj, rn);

    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(obj, CORE_INFO_INTF_NAME);

    tomem(core_info->get_endian(obj), memory, regval);

    return 0;
}

/*
 * register x86 register type to the array
 */
void init_x86_register_defs(void)
{

    static register_defs_t x86_reg_defs;

    x86_reg_defs.name = "x86";
    x86_reg_defs.register_raw_size = x86_register_raw_size;
    x86_reg_defs.register_bytes = NUMREGBYTES;
    x86_reg_defs.register_byte = x86_register_byte;
    x86_reg_defs.num_regs = NUMREGS;
    x86_reg_defs.max_register_raw_size = 4;
    x86_reg_defs.byte_width = EIGHT_BITS_PER_BYTE;
    x86_reg_defs.store_register = x86_store_register;
    x86_reg_defs.fetch_register = x86_fetch_register;
    register_reg_type(&x86_reg_defs);
}

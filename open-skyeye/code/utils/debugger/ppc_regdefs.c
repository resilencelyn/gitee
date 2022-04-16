/*
 * ppc_regdefs.c - necessary ppc definition for skyeye debugger
 * Copyright (C) 2003 Skyeye Develop Group
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
 * 12/21/2006   Michael.Kang  <blackfin.kang@gmail.com>
 */

/*
 * according to GDB_SOURCE/gdb/regformats/ppc-reg.dat
 32:r0  32:r1  32:r2  32:r3  32:r4  32:r5  32:r6  32:r7
 32:r8  32:r9  32:r10 32:r11 32:r12 32:r13 32:r14 32:r15
 32:r16 32:r17 32:r18 32:r19 32:r20 32:r21 32:r22 32:r23
 32:r24 32:r25 32:r26 32:r27 32:r28 32:r29 32:r30 32:r31

 64:f0  64:f1  64:f2  64:f3  64:f4  64:f5  64:f6  64:f7
 64:f8  64:f9  64:f10 64:f11 64:f12 64:f13 64:f14 64:f15
 64:f16 64:f17 64:f18 64:f19 64:f20 64:f21 64:f22 64:f23
 64:f24 64:f25 64:f26 64:f27 64:f28 64:f29 64:f30 64:f31

 32:pc  32:ps  32:cr  32:lr  32:ctr 32:xer 32:fpscr

 * in gdb internal, it seem that there is 72 register, need to dig more.
 */

#include <skyeye_types.h>
#include "skyeye2gdb.h"
#include <skyeye_arch.h>
#include "skyeye_types.h"
#include "skyeye_arch.h"
#include <skyeye_iface.h>
#include <skyeye_core_intf.h>
#include <setjmp.h>
#include "gdbserver.h"

const char *ppc_name[] = {
    "R0",
    "R1",
    "R2",
    "R3",
    "R4",
    "R5",
    "R6",
    "R7",
    "R8",
    "R9",
    "R10",
    "R11",
    "R12",
    "R13",
    "R14",
    "R15",
    "R16",
    "R17",
    "R18",
    "R19",
    "R20",
    "R21",
    "R22",
    "R23",
    "R24",
    "R25",
    "R26",
    "R27",
    "R28",
    "R29",
    "R30",
    "R31",
    "F0",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "F25",
    "F26",
    "F27",
    "F28",
    "F29",
    "F30",
    "F31",
    "PC",
    "MSR",
    "CR",
    "LR",
    "CTR",
    "XER",
    "FPSCR",
    "NPC",
    "DEC",
    NULL
};

static int ppc_register_raw_size(int x)
{
#if 0
    if (x > 31 && x < 64)
        return 8;
    else
#endif
        return 4;
}
static int ppc_register_byte(int x)
{
#if 0
    if (x < 32)
        return (4 * x);
    else if (31 < x && x < 64)
        return (32 * 4 + (x - 32) * 8);
    else
        return (32 * 4 + 32 * 8 + (x - 64) * 4);
#endif
    return 4 * x;
}

static int ppc_store_register(conf_object_t * obj, int rn, unsigned char *memory)
{

    gdbserver_device *dev = obj->obj;
    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(obj, CORE_INFO_INTF_NAME);

    skyeye_reg_intf *reg_intf = (skyeye_reg_intf *) SKY_get_iface(obj, SKYEYE_REG_INTF);

    reg_intf->set_regvalue_by_id(obj, frommem(core_info->get_endian(obj), memory), rn);

    // reg_intf->set_regvalue_by_id(obj, frommem(dev->current_reg_type->endian_flag, memory), rn);

    return 0;
}

static int ppc_fetch_register(conf_object_t * obj, int rn, unsigned char *memory)
{

    gdbserver_device *dev = obj->obj;
    skyeye_reg_intf *reg_intf = (skyeye_reg_intf *) SKY_get_iface(obj, SKYEYE_REG_INTF);
    uint32 regval = reg_intf->get_regvalue_by_id(obj, rn);
    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(obj, CORE_INFO_INTF_NAME);

    tomem(core_info->get_endian(obj), memory, regval);
    return 0;
}

/*
 * register powerpc register type to the array
 */
void init_ppc_register_defs(void)
{
    /* initialize the struct of powerpc register defination */
    static register_defs_t ppc_reg_defs;

    ppc_reg_defs.name = "powerpc";
    ppc_reg_defs.register_raw_size = ppc_register_raw_size;
    ppc_reg_defs.register_byte = ppc_register_byte;
    ppc_reg_defs.register_bytes = (32 + 32 + 9) * 4;
    ppc_reg_defs.num_regs = 73; // TODO: Update it when arch_interface definition changed
    ppc_reg_defs.max_register_raw_size = 4;
    ppc_reg_defs.byte_width = EIGHT_BITS_PER_BYTE;
    ppc_reg_defs.store_register = ppc_store_register;
    ppc_reg_defs.fetch_register = ppc_fetch_register;

    register_reg_type(&ppc_reg_defs);
}

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
* @file ti_c28x_regdefs.c
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-08-02
*/

#include "skyeye2gdb.h"
#include "skyeye_arch.h"

#include "skyeye2gdb.h"
#include "skyeye_arch.h"
#include "skyeye_types.h"
#include <skyeye_iface.h>
#include <skyeye_core_intf.h>
#include <regformat/sparc_regformat.h>

typedef enum
{
    ACC = 0, XAR0, XAR1, XAR2,          /*  0  1  2  3 */
    XAR3, XAR4, XAR5, XAR6,             /*  4  5  6  7 */
    XAR7, DP, IFR, IER,                 /*  8  9 10 11 */
    P, RPC, IC, SPR,                    /* 12 13 14 15 */
    ST0, ST1, XT, PCR,                  /* 16 17 18 19 */
    NUM_REGS,
} reg_id_t;

static int register_raw_size(int x)
{
    if (x == DP || x == IFR || x == IER || x == SPR || x == ST0 || x == ST1)
        return 2;
    else
        return 4;
}

static int register_byte(int x)
{
    char bytes[NUM_REGS] = {
        0, 4, 8, 12,
        16, 20, 24, 28,
        32, 36, 38, 40,
        42, 46, 50, 54,
        56, 58, 60, 64,
    };

    return bytes[x];
}

static int store_register(conf_object_t * obj, int rn, unsigned long *memory)
{
    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(obj, CORE_INFO_INTF_NAME);
    int bytes[NUM_REGS] = {
        15, 0, 1, 2,                    /*   XAR0   XAR1   XAR2   XAR3 */
        3, 4, 5, 6,                     /*   XAR4   XAR5   XAR6   XAR7 */
        7, 9, 18, 19,                   /* PC_REG DP_REG SP_REG XT_REG */
        12, 14, 13, 10,                 /*  P_REG   RPTC    RPC    ACC */
        16, 17, 11, 8,                  /*    ST0    ST1    IFR    IER */
    };

    skyeye_reg_intf *reg_intf = (skyeye_reg_intf *) SKY_get_iface(obj, SKYEYE_REG_INTF);

    reg_intf->set_regvalue_by_id(obj, frommem(core_info->get_endian(obj), memory), bytes[rn]);

    return 0;
}

static int fetch_register(conf_object_t * obj, int rn, unsigned char *memory)
{
    uint32 val;
    int bytes[NUM_REGS] = {
        15, 0, 1, 2,                    /*   XAR0   XAR1   XAR2   XAR3 */
        3, 4, 5, 6,                     /*   XAR4   XAR5   XAR6   XAR7 */
        7, 9, 18, 19,                   /* PC_REG DP_REG SP_REG XT_REG */
        12, 14, 13, 10,                 /*  P_REG   RPTC    RPC    ACC */
        16, 17, 11, 8,                  /*    ST0    ST1    IFR    IER */
    };

    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(obj, CORE_INFO_INTF_NAME);

    skyeye_reg_intf *reg_intf = (skyeye_reg_intf *) SKY_get_iface(obj, SKYEYE_REG_INTF);
    uint32 regval = reg_intf->get_regvalue_by_id(obj, bytes[rn]);

    if (regval == -1)
        regval = 0;
    tomem(core_info->get_endian(obj), memory, regval);

    return 0;
}

void init_ti_c28x_register_defs(void)
{
    static register_defs_t _reg_defs;

    _reg_defs.name = "c28x";
    _reg_defs.register_raw_size = register_raw_size;
    _reg_defs.register_bytes = (NUM_REGS * 4 - 6 * 2);
    _reg_defs.register_byte = register_byte;
    _reg_defs.num_regs = NUM_REGS;
    _reg_defs.max_register_raw_size = 4;
    _reg_defs.byte_width = SIXTEEN_BITS_PER_BYTE;
    _reg_defs.store_register = store_register;
    _reg_defs.fetch_register = fetch_register;
    register_reg_type(&_reg_defs);
}

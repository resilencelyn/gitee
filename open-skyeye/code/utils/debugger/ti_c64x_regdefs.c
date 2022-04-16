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
* @file ti_c64x_regdefs.c
* @brief The register format of ti c64x, refer to ${GDB_SOURCE}/gdb/regformats/tic6x-c64xp.dat
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-08-02
*/

#include "skyeye2gdb.h"
#include "skyeye_arch.h"

#if 0
#include "regformat/c6k_regformat.h"
#include "skyeye_int_register.h"
#include "skyeye_interface.h"
#endif

#include "skyeye2gdb.h"
#include "skyeye_arch.h"
#include "skyeye_types.h"
#include <skyeye_iface.h>
#include <skyeye_core_intf.h>
#include <regformat/sparc_regformat.h>
#define NUM_REGS 43

/*
 * typedef enum{
 * A0 = 0, A1, A2, A3, A4, A5, A6, A7,
 * A8, A9, A10,    A11,    A12,    A13,    A14,    A15,
 * B0, B1, B2, B3, B4, B5, B6, B7,
 * B8, B9, B10,    B11,    B12,    B13,    B14,    B15,
 * CSR_REG,PC_REG,AMR_REG,IFR_REG,IER_REG,ISTP_REG,IRP_REG,
 * NRP_REG,FADCR_REG,FAUCR_REG,FMCR_REG,
 * C67X_REGNUM
 * }reg_id_t;
 * 
 * typedef enum{
 * A0 = 0, A1, A2, A3, A4, A5, A6, A7,
 * A8, A9, A10,    A11,    A12,    A13,    A14,    A15,
 * B0, B1, B2, B3, B4, B5, B6, B7,
 * B8, B9, B10,    B11,    B12,    B13,    B14,    B15,
 * CSR_REG,PC_REG,
 * A16,    A17,    A18,    A19,    A20,    A21,    A22,    A23,
 * A24,    A25,    A26,    A27,    A28,    A29,    A30,    A31,
 * B16,    B17,    B18,    B19,    B20,    B21,    B22,    B23,
 * B24,    B25,    B26,    B27,    B28,    B29,    B30,    B31,
 * AMR_REG,IFR_REG,IER_REG,ISTP_REG,IRP_REG,
 * NRP_REG,FADCR_REG,FAUCR_REG,FMCR_REG,
 * GFPGFR_REG,ER_REG,EM_REG,DIER_REG,
 * C64X_REGNUM
 * }reg_id_t;
 * 
 * typedef enum{
 * A0 = 0, A1, A2, A3, A4, A5, A6, A7,
 * A8, A9, A10,    A11,    A12,    A13,    A14,    A15,
 * B0, B1, B2, B3, B4, B5, B6, B7,
 * B8, B9, B10,    B11,    B12,    B13,    B14,    B15,
 * CSR_REG, PC_REG,
 * A16, A17, A18, A19, A20, A21, A22, A23,
 * A24, A25, A26,    A27,    A28,    A29,    A30,    A31,
 * B16, B17, B18, B19, B20, B21, B22, B23,
 * B24, B25, B26,    B27,    B28,    B29,    B30,    B31,
 * //c64x+ c674
 * TSR_REG=66,ILC_REG,RILC_REG,
 * //all
 * AMR_REG,IFR_REG,IER_REG,ISTP_REG,IRP_REG,
 * NRP_REG,FADCR_REG,FAUCR_REG,FMCR_REG,
 * //c67x+ c64x
 * GFPGFR_REG,ER_REG,EM_REG,DIER_REG,
 * //c64x+ c674 c66
 * ITSR_REG,NTSR_REG,EFR_REG,ECR_REG,
 * IEER_REG,SSR_REG,GPLYA_REG,GPLYB_REG,TSCL_REG,TSCH_REG,DNUM_REG,
 * C66X_REGNUM
 * }reg_id_t;
 */

static int register_raw_size(int x)
{
    return 4;
}
static int register_byte(int x)
{
    return (x * 4);
}
static int store_register(conf_object_t * obj, int rn, unsigned long *memory)
{
    //bigendSig = state->bigendSig;
#if 0
    generic_arch_t *arch_instance = get_arch_instance("");

    if (rn >= 0 && rn < 16)
    {
        arch_instance->set_regval_by_id(rn, frommem(memory));
    } else if (rn == 32)
        arch_instance->set_regval_by_id(CSR_REG, frommem(memory));
    else if (rn == 33)
        arch_instance->set_regval_by_id(PC_REG, frommem(memory));
    else if (rn >= 16 && rn < 32)
        arch_instance->set_regval_by_id(rn - 16 + B0, frommem(memory));
#if 0
    else if (rn >= 34 && rn < 50)
        arch_instance->set_regval_by_id(rn - 34 + A16, frommem(memory));
    else if (rn >= 50 && rn < 66)
        arch_instance->set_regval_by_id(rn - 50 + B16, frommem(memory));
    else if (rn == 66)
        arch_instance->set_regval_by_id(TSR_REG, frommem(memory));
    else if (rn == 67)
        arch_instance->set_regval_by_id(ILC_REG, frommem(memory));
    else if (rn == 68)
        arch_instance->set_regval_by_id(ILC_REG, frommem(memory));
#endif
    else                                /* FIXME */
        ;                               /* something wrong */

#endif
    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(obj, CORE_INFO_INTF_NAME);

    /*
     * if(rn == 32)
     * rn = 69;
     * else if(rn == 33) //csr
     * rn = 32;
     * else if(rn ==34 ) //amr
     * rn = 70;
     * else if(rn ==35 )
     * rn = 71;
     * else if(rn ==36 )
     * rn = 72;
     * else if(rn ==37 )
     * rn = 73;
     * else if(rn ==38 )
     * rn = 72;
     * else if(rn ==39 )  //pc
     * rn = 33;
     * else if(rn ==40 )
     * rn = 74;
     * else if(rn ==41 )
     * rn = 75;
     * else if(rn ==42 )
     * rn = 76;
     * else 
     * ;
     */
    skyeye_reg_intf *reg_intf = (skyeye_reg_intf *) SKY_get_iface(obj, SKYEYE_REG_INTF);

    reg_intf->set_regvalue_by_id(obj, frommem(core_info->get_endian(obj), memory), rn);

    return 0;
}

static int fetch_register(conf_object_t * obj, int rn, unsigned char *memory)
{
#if 0
    uint32 regval;
    skyeye_reg_intf *reg_intf = (skyeye_reg_intf *) SKY_get_interface(obj, SKYEYE_REG_INTF);

    int regno = 0;

    if (rn >= 0 && rn < 16)
    {
        regno = rn;
    } else if (rn == 32)
        regno = CSR_REG;
    else if (rn == 33)
        regno = PC_REG;
    else if (rn >= 16 && rn < 32)
        regno = rn - 16 + B0;
    else if (rn >= 34 && rn < 50)
        regno = rn - 34 + A0;
    else if (rn >= 50 && rn < 66)
        regno = rn - 50 + B0;
    else if (rn == 66)
        regno = TSR_REG;
    else if (rn == 67)
        regno = ILC_REG;
    else if (rn == 68)
        regno = RILC_REG;
    else;

    regval = reg_intf->get_regvalue_by_id(obj, regno);
    tomem(memory, regval);
    //printf("rn:%d, regno:%d, memory:%x\n", rn, regno, *(unsigned int *)memory);
#endif
    uint32 val;

    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(obj, CORE_INFO_INTF_NAME);

    /*
     * if(rn == 32)
     * rn = 69;
     * else if(rn == 33) //csr
     * rn = 32;
     * else if(rn ==34 ) //amr
     * rn = 70;
     * else if(rn ==35 )
     * rn = 71;
     * else if(rn ==36 )
     * rn = 72;
     * else if(rn ==37 )
     * rn = 73;
     * else if(rn ==38 )
     * rn = 72;
     * else if(rn ==39 )  //pc
     * rn = 33;
     * else if(rn ==40 )
     * rn = 74;
     * else if(rn ==41 )
     * rn = 75;
     * else if(rn ==42 )
     * rn = 76;
     * else 
     * ;
     */

    skyeye_reg_intf *reg_intf = (skyeye_reg_intf *) SKY_get_iface(obj, SKYEYE_REG_INTF);
    uint32 regval = reg_intf->get_regvalue_by_id(obj, rn);

    //if the register value is FFFFFFFF, then it been set to 0 which is wrong
    //if(regval == -1)
    //      regval  =  0;
    tomem(core_info->get_endian(obj), memory, regval);

    return 0;
}

static int register_num(conf_object_t * obj)
{

    skyeye_reg_intf *reg_intf = (skyeye_reg_intf *) SKY_get_iface(obj, SKYEYE_REG_INTF);
    uint32 regnum = reg_intf->get_regnum(obj);

    //FIXME: for now the tic6x gdb can not select for diff mach,so just return C67X reg num
    return regnum;
    //return NUM_REGS;

}

/*
 * register arm register type to the array
 */
void init_ti_c64x_register_defs(void)
{
    static register_defs_t _reg_defs;

    _reg_defs.name = "c6k";
    _reg_defs.register_raw_size = register_raw_size;
    _reg_defs.register_bytes = (NUM_REGS * 4);
    _reg_defs.register_byte = register_byte;
    _reg_defs.num_regs = NUM_REGS;
    _reg_defs.max_register_raw_size = 4;
    _reg_defs.byte_width = EIGHT_BITS_PER_BYTE;
    _reg_defs.store_register = store_register;
    _reg_defs.fetch_register = fetch_register;
    _reg_defs.register_num = register_num;
    register_reg_type(&_reg_defs);
}

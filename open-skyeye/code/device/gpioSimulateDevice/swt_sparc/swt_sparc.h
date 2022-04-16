/* Copyright (C)
 * 
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
 * @file swt_sparc.h
 * @brief The definition of system controller 
 * @author 
 * @version 78.77
 */

/* Autogenerated by SkyEye script */
#ifndef __SWT_SPARC_H__
#define __SWT_SPARC_H__

typedef struct
{
    uint8_t lss1_xb;
    uint8_t lss1_yb;
    uint8_t lss1_zb;
    uint8_t lss2_xb;
    uint8_t lss2_yb;
    uint8_t lss2_zb;
}lss_sw_t;

typedef struct
{
    uint8_t Rcwx;
    uint8_t Rcwy;
    uint8_t Rcwz;
    uint8_t Rcws;
    uint8_t st1;
    uint8_t st2;
    uint8_t st3;
    uint8_t fog1;
    uint8_t fog2;
    uint8_t Ires;
}power_sw_t;

typedef struct
{
    uint32_t swt_reg_1;   //0x10015500       开关量寄存器1
    uint32_t swt_reg_2;   //0x10015504       开关量寄存器2
    uint32_t swt_reg_3;   //0x10015508       开关量寄存器3
    uint32_t swt_reg_4;   //0x1001550c       开关量寄存器4
}swt_sparc_reg_t;

typedef struct swt_sparc_device
{
    conf_object_t* obj;
    swt_sparc_reg_t* regs;
    io_linker_intf* io_linker_iface;
    conf_object_t* io_linker;
    lss_sw_t lss_sw;
    power_sw_t power_sw;
}swt_sparc_device;

#define SWT_REG_1 0x0
#define SWT_REG_2 0x4
#define SWT_REG_3 0x8
#define SWT_REG_4 0xc
static char* regs_name[] =
{
    "swt_reg_1",
    "swt_reg_2",
    "swt_reg_3",
    "swt_reg_4",
    NULL
};
#define BUFF_SIZE 1024
#endif

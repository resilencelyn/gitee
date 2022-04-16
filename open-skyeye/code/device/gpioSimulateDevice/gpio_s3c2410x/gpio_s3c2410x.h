/* Copyright (C)
* 2011 - Michael.Kang blackfin.kang@gmail.com
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
* @file gpio_s3c2410x.h
* @brief The definition of system controller for s3c2410x
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
*/

#ifndef __SYSCTRL_S3C2410x_H__
#define __SYSCTRL_S3C2410x_H__

typedef struct gpio_reg{
	uint32 gpacon;		/* 0x56000000 */
	uint32 gpadat;

	uint32 gpbcon;		/* 0x56000010 */
	uint32 gpbdat;
	uint32 gpbup;

	uint32 gpccon;		/* 0x56000020 */
	uint32 gpcdat;
	uint32 gpcup;

	uint32 gpdcon;		/* 0x56000030 */
	uint32 gpddat;
	uint32 gpdup;

	uint32 gpecon;		/* 0x56000040 */
	uint32 gpedat;
	uint32 gpeup;

	uint32 gpfcon;		/* 0x56000050 */
	uint32 gpfdat;
	uint32 gpfup;

	uint32 gpgcon;		/* 0x56000060 */
	uint32 gpgdat;
	uint32 gpgup;

	uint32 gphcon;		/* 0x56000070 */
	uint32 gphdat;
	uint32 gphup;

	uint32 misccr;		/* 0x56000080 */
	uint32 dclkcon;		/* 0x56000084 */

	uint32 extint0;		/* 0x56000088 */
	uint32 extint1;		/* 0x5600008c */
	uint32 extint2;		/* 0x56000090 */

	uint32 eintflt0;	/* 0x56000094 */
	uint32 eintflt1;	/* 0x56000098 */
	uint32 eintflt2;	/* 0x5600009c */
	uint32 eintflt3;	/* 0x4c6000a0 */

	uint32 eintmask;	/* 0x560000a4 */
	uint32 eintpend;	/* 0x560000a8 */

	uint32 gstatus0;	/* 0x560000ac */
	uint32 gstatus1;	/* 0x560000b0 */
	uint32 gstatus2;	/* 0x560000b4 */
	uint32 gstatus3;	/* 0x560000b8 */
	uint32 gstatus4;	/* 0x560000bc */
}gpio_reg_t;

static char* regs_name[] = {
	"gpacon",
	"gpadat",
	"gpbcon",
	"gpbdat",
	"gpbup",
	"gpccon",
	"gpcdat",
	"gpcup",
	"gpdcon",
	"gpddat",
	"gpdup",
	"gpecon",
	"gpedat",
	"gpeup",
	"gpfcon",
	"gpfdat",
	"gpfup",
	"gpgcon",
	"gpgdat",
	"gpgup",
	"gphcon",
	"gphdat",
	"gphup",
	"misccr",
	"dclkcon",
	"extint0",
	"extint1",
	"extint2",
	"eintflt0",
	"eintflt1",
	"eintflt2",
	"eintflt3",
	"eintmask",
	"eintpend",
	"gstatus0",
	"gstatus1",
	"gstatus2",
	"gstatus3",
	"gstatus4",
	 NULL
};
typedef struct s3c2410x_gpio_device{
	conf_object_t* obj;
	gpio_reg_t* regs;
	int line_no;
	conf_object_t* signal_target;
	general_signal_intf* master;
	char** regs_name;
}s3c2410x_gpio_device;

#endif

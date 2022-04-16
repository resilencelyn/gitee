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
* @file sysctrl_s3c2410x.h
* @brief The definition of system controller for s3c2410x
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
*/

#ifndef __SYSCTRL_S3C2410x_H__
#define __SYSCTRL_S3C2410x_H__

typedef struct sysctrl_reg{
	uint32_t locktime;
	uint32_t mpllcon;
	uint32_t upllcon;
	uint32_t clkcon;
	uint32_t clkslow;
	uint32_t clkdivn;
}sysctrl_reg_t;

static char *regs_name[] = {
	"locktime",
	"mpllcon",
	"upllcon",
	"clkcon",
	"clkslow",
	"clkdivn",
	NULL
};

typedef struct s3c2410x_sysctrl_device{
	conf_object_t* obj;
	sysctrl_reg_t* regs;
	char **regs_name;
}s3c2410x_sysctrl_device;

#endif

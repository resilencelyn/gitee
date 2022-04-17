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
* @file camif_s3c6410.h
* @brief The definition of system controller for s3c6410
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
*/

#ifndef __CAMIF_S3C6410_H__
#define __CAMIF_S3C6410_H__

typedef struct camif_reg{
	uint32 cisrcfmt; 
	uint32 ciwdofst;
	uint32 cigctrl;
	uint32 ciwdofst2;
}camif_reg_t; 

char *regs_name[] = {
	"cisrcfmt",
	"ciwdofst",
	"cigctrl",
	"ciwdofst2",
	NULL
};
typedef struct s3c6410_camif_device{
	conf_object_t* obj;
	camif_reg_t* regs;
	char**regs_name;
}s3c6410_camif_device;

#endif
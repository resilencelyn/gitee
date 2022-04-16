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
* @file intc_6713.h
* @brief The definition of system controller 
* @author 
* @version 78.77
*/

/* Autogenerated by SkyEye script */
#ifndef __intc_6713_H__
#define __intc_6713_H__

typedef struct intc_6713_reg{
	uint32_t ext_pol;
}intc_6713_reg_t;

typedef struct intc_6713_device{
	conf_object_t* obj;
	intc_6713_reg_t* regs;
}intc_6713_device;

static char* regs_name[] = {
	"ext_pol",
	NULL
};

#endif

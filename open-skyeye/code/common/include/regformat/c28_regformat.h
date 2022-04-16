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
* @file c6k_regformat.h
* @brief The definition of REGISTER
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-08-02
*/
#ifndef __C28_REGFORMAT_H__
#define __C28_REGFORMAT_H__
typedef enum{
	XAR0 = 0,
	XAR1,
	XAR2,
	XAR3,
	XAR4,
	XAR5,
	XAR6,
	XAR7,
	PC_REG,
	DP_REG,
	SP_REG,
	XT_REG,
	P_REG,
	RPTC,
	RPC,
	ACC,
	ST0,
	ST1,
	IFR,
	IER,
	C28_REGNUM,
}reg_id_t;
#endif

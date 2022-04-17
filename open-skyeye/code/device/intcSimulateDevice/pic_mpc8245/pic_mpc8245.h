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
* @file pic_mpc8245.h
* @brief The definition of system controller 
* @author 
* @version 78.77
*/

/* Autogenerated by SkyEye script */
#ifndef __pic_mpc8245_H__
#define __pic_mpc8245_H__

#define GLOBAL_TIMER_NUM 4
#define SERIAL_VPR_NUM 16
typedef struct pic_mpc8245_reg{
	uint32_t gcr;
	uint32_t gtccr[GLOBAL_TIMER_NUM];
	uint32_t gtbcr[GLOBAL_TIMER_NUM];
	uint32_t gtvpr[GLOBAL_TIMER_NUM];
	uint32_t gtdr[GLOBAL_TIMER_NUM];
	uint32_t svpr[SERIAL_VPR_NUM];
	uint32_t svr;
	uint32_t iack;
	uint32_t eoi;
}pic_mpc8245_reg_t;

typedef struct pic_mpc8245_device{
	conf_object_t* obj;
	pic_mpc8245_reg_t* regs;
	core_signal_intf* signal_iface;
	conf_object_t* signal;

	conf_object_t *cpu;
	int uart_signal;
}pic_mpc8245_device;

static char* regs_name[] = {
	NULL
};

#endif
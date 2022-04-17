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
* @file timer_c6474.h
* @brief The definition of system controller 
* @author 
* @version 78.77
*/

/* Autogenerated by SkyEye script */
#ifndef __TIMER_C6474_H__
#define __TIMER_C6474_H__
#include <skyeye_lock.h>

#define MASK(a, b) ((1 << ((b) - (a) + 1)) - 1)
#define BITS(r, a, b) (((r) >> (a)) & ((1 << (1 + (b)-(a)))-1))
#define SET_BITS(r, a, b, v) (((r) & (~(MASK(a, b) << (a)))) | ((v) << (a)))

/*Timer Register Address :0x2940000*/ 
typedef struct timer_c6474_reg{
	uint32_t clkspd; 
	union {
		uint64_t cnt;
		struct{
			uint32_t cntlo;
			uint32_t cnthi;
		};
	};
	union {
		uint64_t prd;
		struct{
			uint32_t prdlo;
			uint32_t prdhi;
		};
	};
	uint32_t tcr;
	uint32_t tgcr;
	uint32_t wdtcr;
}timer_c6474_reg_t;

typedef struct device{
	conf_object_t* obj;
	timer_c6474_reg_t* regs;
	int timer_scheduler_id;
	conf_object_t* core;
	core_signal_intf* core_signal;
	int cnthi_shadow;
	int old_tcr;
	int sche_id1;
	int sche_id2;
	int sche_id3;
	int timer_count;
	int toutl;
	RWLOCK_T lock;
	int int_number;
}timer_c6474_device;

static char* regs_name[] = {
	"CLKSPD",
	"CNTLO",
	"CNTHI",
	"PRDLO",
	"PRDHI",
	"TCR",
	"TGCR",
	"WDTCR",
	NULL
};
#define CLKSPD 0x4
#define CNTLO  0x10
#define CNTHI  0x14
#define PRDLO  0x18
#define PRDHI  0x1c
#define TCR    0x20
#define TGCR   0x24
#define WDTCR  0x28

#define TIMMODE ((dev->regs->tgcr >> 2) & 0x3)
#define ENAMODE_HI ((dev->regs->tcr >> 22) & 0x3)

#endif
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
* @file timer_c28.h
* @brief The definition of system controller 
* @author 
* @version 78.77
*/

/* Autogenerated by SkyEye script */
#ifndef __TIMER_C28_H__
#define __TIMER_C28_H__
#include <skyeye_system.h>

struct  TCR_BITS {          // bits  description
   uint16_t    rsvd1:4;       // 3:0   reserved
   uint16_t    TSS:1;         // 4     Timer Start/Stop
   uint16_t    TRB:1;         // 5     Timer reload
   uint16_t    rsvd2:4;       // 9:6   reserved
   uint16_t    SOFT:1;        // 10    Emulation modes
   uint16_t    FREE:1;        // 11
   uint16_t    rsvd3:2;       // 12:13 reserved
   uint16_t    TIE:1;         // 14    Output enable
   uint16_t    TIF:1;         // 15    Interrupt flag
};
union TCR_REG{
	uint16_t  all;
	struct TCR_BITS bit;
};
// TPR: Pre-scale low bit definitions:
struct  TPR_BITS {        // bits  description
   uint16_t     TDDR:8;     // 7:0   Divide-down low
   uint16_t     PSC:8;      // 15:8  Prescale counter low
};
union TPR_REG {
   uint16_t           all;
   struct TPR_BITS  bit;
};

// TPRH: Pre-scale high bit definitions:
struct  TPRH_BITS {       // bits  description
   uint16_t     TDDRH:8;      // 7:0   Divide-down high
   uint16_t     PSCH:8;       // 15:8  Prescale counter high
};

union TPRH_REG {
   uint16_t           all;
   struct TPRH_BITS bit;
};

// TIM, TIMH: Timer register definitions:
struct TIM_REG {
   uint16_t  LSW;
   uint16_t  MSW;
};

union TIM_GROUP {
   uint32_t          all;
   struct TIM_REG  half;
};

// PRD, PRDH: Period register definitions:
struct PRD_REG {
   uint16_t  LSW;
   uint16_t  MSW;
};

union PRD_GROUP {
   uint32_t          all;
   struct PRD_REG  half;
};

//---------------------------------------------------------------------------
// CPU Timer Register File:
// Address : 0xc00
typedef struct CPUTIMER_REGS {
   union TIM_GROUP TIM;   // Timer counter register
   union PRD_GROUP PRD;   // Period register
   union TCR_REG   TCR;   // Timer control register
   union TPR_REG   TPR;   // Timer pre-scale low
   union TPRH_REG  TPRH;  // Timer pre-scale high
}timer_c28_reg_t;

typedef struct timer_c28_device{
	conf_object_t* obj;
	timer_c28_reg_t* regs;
	conf_object_t* signal;
	general_signal_intf* signal_iface;

	core_signal_intf* core_iface;
	conf_object_t* core_signal;
	int interrupt_number;
    time_handle_t *handle;
}timer_c28_device;

static char* regs_name[] = {
	"TIM",
	"TIMH",
	"PRD",
	"PRDH",
	"TCR",
	"TPR",
	"TPRH",
	NULL
};

static uint32_t regs_offset[] = {
	0x0000,
	0x0001,
	0x0002,
	0x0003,
	0x0004,
	0x0006,
	0x0007,
};

#define REG_NUM 7
#define CPU_FREQ 150 

#define TIM_LSW 0x0 
#define TIM_MSW 0x1 
#define PRD_LSW 0x2 
#define PRD_MSW 0x3 
#define TCR_REG 0x4 
#define TPR_LSW     0x6 
#define TPRH_MSW    0x7 

#endif



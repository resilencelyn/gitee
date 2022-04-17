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
* @file timer_c5510.h
* @brief The definition of system controller 
* @author 
* @version 78.77
*/

/* Autogenerated by SkyEye script */
#ifndef __timer_c5510_H__
#define __timer_c5510_H__

typedef struct {
	uint16_t RESERVED 	:1;
	uint16_t DATOUT 	:1;//Data output bit
	uint16_t POLAR 		:1;//Timer-output polarity bit
	uint16_t CP 		:1;//clock mode/pilse mode bit
	uint16_t TSS 		:1;//Timer stop status bit (1 : stop; 0 : start) 
	uint16_t ARB 		:1;//Auto-reload bit
	uint16_t PWID 		:2;//Timer-output pulse width bit
	uint16_t FREE 		:1;//free run bit
	uint16_t SOFT 		:1;//soft stop bit
	uint16_t TLB 		:1;//timer load bit
	uint16_t FUNC 		:2;//Function bits for the timer pin
	uint16_t ERR_TIM 	:1;//Timer-pin error flag
	uint16_t INTEXT 	:1;//Internal-to-external clock change indictor.
	uint16_t IDLEEN 	:1;//idle enable bit for the timer  
}TCR_REG;

typedef union{
	uint16_t all;
	TCR_REG bit;
}TCR_REG_T;
/*Timer Register*/ 
typedef struct timer_c5510_reg{
	int16_t tim;  //timer0 address : 0x1000 timer1 address: 0x2400
	uint16_t prd;  //timer0 address : 0x1001 timer1 address: 0x2401
	TCR_REG_T tcr; //timer0 address : 0x1002 timer1 address: 0x2402 
	uint16_t prsc; //PSC bits 9-6 of PRSC TDDR bits 3-0 of PRSC timer0 address : 0x1003 timer1 address: 0x24030
}timer_c5510_reg_t;

typedef struct timer_c5510_device{
	conf_object_t* obj;
	timer_c5510_reg_t* regs;
	int timer_scheduler_id;
	conf_object_t* core;
	core_signal_intf* core_signal;
	int psc;
	int tddr;
	int int_number;
}timer_c5510_device;

#define TIM  0
#define PRD  1
#define TCR  2
#define PRSC 3
static char* regs_name[] = {
	"tim",
	"prd",
	"tcr",
	"prsc",
	NULL
};
static uint32_t regs_offset[] = {
	0x0000,
	0x0001,
	0x0002,
	0x0003,
};
#endif
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
* @file timer_6713.h
* @brief The definition of system controller 
* @author 
* @version 78.77
*/

/* Autogenerated by SkyEye script */
#ifndef __timer_6713_H__
#define __timer_6713_H__

#include "skyeye_system.h"
typedef struct timer_6713_reg{
	uint32_t ctrl;
	uint32_t prd;
	uint32_t cnt;
}timer_6713_reg_t;

typedef struct timer_6713_device{
	conf_object_t* obj;
	timer_6713_reg_t* regs;
	int timer_scheduler_id;
	conf_object_t* core;
	core_signal_intf* core_signal;
    
    conf_object_t* edma;
    edma_device_event_t* edma_event;
    
	int scale;
	int interrupt_number;
    time_handle_t *handle;
}timer_6713_device;

static char* regs_name[] = {
	"ctrl",
	"prd",
	"cnt",
	NULL
};
#endif

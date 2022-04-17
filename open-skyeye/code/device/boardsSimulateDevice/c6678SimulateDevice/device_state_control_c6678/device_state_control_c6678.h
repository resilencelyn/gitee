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
#ifndef __device_state_control_c6678_H__
#define __device_state_control_c6678_H__

/*IPCGR0-7 IPCAR0-7 Address : 0x2620240*/
typedef struct device_state_control_c6678_reg{
    //reserved

	uint32_t ipcgr[8]; 
    
    uint32_t reserved1[7];

    uint32_t ipcgrh;

    uint32_t ipcar[8];

    uint32_t reserved2[7];

    uint32_t ipcarh;

}device_state_control_c6678_reg_t;

typedef struct device_state_control_c6678{
	conf_object_t* obj;
	device_state_control_c6678_reg_t* regs;
	conf_object_t* core;
	general_signal_intf* general_signal;
    int interrupt_number;
}device_state_control_c6678_device;

static char* regs_name[] = {
	"ipcgr0",
	"ipcgr1",
	"ipcgr2",
	"ipcgr3",
	"ipcgr4",
	"ipcgr5",
	"ipcgr6",
	"ipcgr7",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"ipcgrh",
	"ipcar0",
	"ipcar1",
	"ipcar2",
	"ipcar3",
	"ipcar4",
	"ipcar5",
	"ipcar6",
	"ipcar7",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
    "ipcarh",
	NULL
};

#endif
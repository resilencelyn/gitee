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
#ifndef __gpio_c64x_H__
#define __gpio_c64x_H__

/*GPIO Address : 0x2B00000*/
typedef struct gpio_reg{
	/** GPIO Interrupt Per-Bank Enable Register */
	uint32_t BINTEN; 

	/** GPIO Direction Register */
	uint32_t IODIR;

	/** GPIO Output Data Register */
	uint32_t OUT_DATA;

	/** GPIO Set Data Register */
	uint32_t SET_DATA;

	/** GPIO Clear Data Register */
	uint32_t CLR_DATA;

	/** GPIO input Data Register */
	uint32_t IN_DATA;
	/** GPIO Set Rising Edge Interrupt Register */
	uint32_t SET_RIS_TRIG;

	/** GPIO Clear Rising Edge Interrupt
	 * Register */
	uint32_t CLR_RIS_TRIG;

	/** GPIO Set Falling Edge Interrupt
	 * Register */
	uint32_t SET_FAL_TRIG;

	/** GPIO Clear Falling Edge
	 * Interrupt Register */
	uint32_t CLR_FAL_TRIG;    
}gpio_c64x_reg_t;

typedef struct gpio_c64x{
	conf_object_t* obj;
	gpio_c64x_reg_t* regs;
	conf_object_t* core;
	core_signal_intf* core_signal;
	conf_object_t *gpio_c64x_connector;
	skyeye_gpio_connector_intf *connector_iface;
	conf_object_t *gpio_c64x_gate_a;
	skyeye_gate_a_intf *gate_a_iface;
	conf_object_t *gpio_c64x_gate_b;
	skyeye_gate_b_intf *gate_b_iface;
}gpio_c64x_device;

static char* regs_name[] = {
	"BINTEN",
	"IODIR",
	"OUT_DATA",
	"SET_DATA",
	"CLR_DATA",
	"IN_DATA",
	"SET_RIS_TRIG",
	"CLR_RIS_TRIG",
	"SET_FAL_TRIG",
	"CLR_FAL_TRIG",
	NULL
};

#define EN (dev->regs->BINTEN & 0x1)
#endif

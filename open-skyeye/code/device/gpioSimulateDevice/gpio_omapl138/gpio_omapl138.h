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
* @file gpio_omapl138.h
* @brief The definition of system controller for omapl138
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
*/

#ifndef __GPIO_OMAPL138_H__
#define __GPIO_OMAPL138_H__

typedef enum{
	Bank0 = 0,
	Bank1,
	Bank2,
	Bank3,
	Bank4,
	Bank5,
	Bank6,
	Bank7,
	Bank8,
}Gpio_Bank_t;

typedef enum{
	Gpio_pin0 = 0,
	Gpio_pin1,
	Gpio_pin2,
	Gpio_pin3,
	Gpio_pin4,
	Gpio_pin5,
	Gpio_pin6,
	Gpio_pin7,
	Gpio_pin8,
	Gpio_pin9,
	Gpio_pin10,
	Gpio_pin11,
	Gpio_pin12,
	Gpio_pin13,
	Gpio_pin14,
	Gpio_pin15,
}Gpio_Pin_t;

typedef struct gpio_reg{
	uint32_t gpio_revid;		/* 0x1e26000 */
	uint32_t gpio_binten;
	uint32_t gpio_dir[5];
	uint32_t gpio_out_data[5];
	uint32_t gpio_set_data[5];
	uint32_t gpio_clr_data[5];
	uint32_t gpio_in_data[5];
	uint32_t gpio_set_ris_trig[5];
	uint32_t gpio_clr_ris_trig[5];
	uint32_t gpio_set_fal_trig[5];
	uint32_t gpio_clr_fal_trig[5];
	uint32_t gpio_intstat[5];
}gpio_reg_t;

static char* regs_name[] = {
	"gpio_revid",
	"gpio_binten",
	"gpio_dir01",
	"gpio_dir23",
	"gpio_dir45",
	"gpio_dir67",
	"gpio_dir8",
	"gpio_out_data01",
	"gpio_out_data23",
	"gpio_out_data45",
	"gpio_out_data67",
	"gpio_out_data8",
	"gpio_set_data01",
	"gpio_set_data23",
	"gpio_set_data45",
	"gpio_set_data67",
	"gpio_set_data8",
	"gpio_clr_data01",
	"gpio_clr_data23",
	"gpio_clr_data45",
	"gpio_clr_data67",
	"gpio_clr_data8",
	"gpio_in_data01",
	"gpio_in_data23",
	"gpio_in_data45",
	"gpio_in_data67",
	"gpio_in_data8",
	"gpio_set_ris_trig01",
	"gpio_set_ris_trig23",
	"gpio_set_ris_trig45",
	"gpio_set_ris_trig67",
	"gpio_set_ris_trig8",
	"gpio_clr_ris_trig01",
	"gpio_clr_ris_trig23",
	"gpio_clr_ris_trig45",
	"gpio_clr_ris_trig67",
	"gpio_clr_ris_trig8",
	"gpio_set_fal_trig01",
	"gpio_set_fal_trig23",
	"gpio_set_fal_trig45",
	"gpio_set_fal_trig67",
	"gpio_set_fal_trig8",
	"gpio_clr_fal_trig01",
	"gpio_clr_fal_trig23",
	"gpio_clr_fal_trig45",
	"gpio_clr_fal_trig67",
	"gpio_clr_fal_trig8",
	"gpio_intstat01",
	"gpio_intstat23",
	"gpio_intstat45",
	"gpio_intstat67",
	"gpio_intstat8",
	 NULL
};
typedef struct omapl138_gpio_device{
	conf_object_t* obj;
	gpio_reg_t* regs;
	general_signal_intf* signal_iface;
	conf_object_t* signal;
    
    conf_object_t* gpio_link;
    gpio_linker_pin_intf* gpio_linker_intf;
    
    char *connect_list;
    int connect_quanaity;
    int connect_pin[128];

	char** regs_name;

	uint32_t gpio0_px_data;
	uint32_t pin_num;
	char *circuit_display;
}omapl138_gpio_device;

#define INT_ENABLE(n) (dev->regs->gpio_binten & (0x1 << n))

static uint32_t regs_offset[] = {
	0x0000,
	0x0008,
	0x0010,
	0x0038,
	0x0060,
	0x0088,
	0x00b0,
	0x0014,
	0x003c,
	0x0064,
	0x008c,
	0x00b4,
	0x0018,
	0x0040,
	0x0068,
	0x0090,
	0x00b8,
	0x001c,
	0x0044,
	0x006c,
	0x0094,
	0x00bc,
	0x0020,
	0x0048,
	0x0070,
	0x0098,
	0x00c0,
	0x0024,
	0x004c,
	0x0074,
	0x009c,
	0x00c4,
	0x0028,
	0x0050,
	0x0078,
	0x00a0,
	0x00c8,
	0x002c,
	0x0054,
	0x007c,
	0x00a4,
	0x00cc,
	0x0030,
	0x0058,
	0x0080,
	0x00a8,
	0x00d0,
	0x0034,
	0x005c,
	0x0084,
	0x00ac,
	0x00d4,
	
};

#define OUTOFRANGE (-1)

// electrical character of pins */
typedef enum pin_type {
	OUTPUT = 0,
	INPUT = 1,
} pin_type_t;

#endif

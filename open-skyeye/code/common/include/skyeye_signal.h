/*
	skyeye_signal.h - define the signal framework for skyeye
	Copyright (C) 2010 Skyeye Develop Group
	for help please send mail to <skyeye-developer@lists.gro.clinux.org>
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 
*/
/*
 *	2010-08-15 Michael.Kang<blackfin.kang@gmail.com>
 */
#ifndef __SKYEYE_SIGNAL_H__
#define __SKYEYE_SIGNAL_H__
typedef enum signal{
	/* maintain the previous level of the signal, do none of change */
	Prev_level = -1,
	/* change the signal level to low */
	Low_level,
	/* change the signal level to high */
	High_level,
}signal_t;

typedef enum Irq_type{
	/* Non-Peripheral Interrupts */
	Non_Periph_Type = 1,
	/* Peripheral Group Interrupts */
	Periph_Type,
}irq_type_t;

typedef struct{
                /* Normal irq signal in arm */
                signal_t irq;
                /* Fast irq signal in arm */
                signal_t firq;
                /*exceptions in cortex-M0*/
                uint32_t m0_irq;
		/* reset signal in arm dyf add when move sa1100 to soc dir  2010.9.21*/
		signal_t reset;
}arm_signal_t;

typedef	struct{
	/* Hardware interrupt signal in mips */
	int HARD_IP;
}mips_signal_t;

typedef	struct{
	/* Normal interrupt signal in powerpc */
	signal_t intr;
	/* Critical interrupt signal in powerpc */
	signal_t cintr;
	signal_t core_reset;
    uint32_t irq_vector_number;
    uint8_t  hardware_vector_mode;
}powerpc_signal_t;

typedef	struct{
	/* set interrupt flag register in c6k core */
	signal_t ifr[16]; 
}c6k_signal_t;

typedef	struct{
	/* set interrupt flag register in c5k core */
	signal_t ifr[32]; 
}c5k_signal_t;

typedef	struct{
	/* set interrupt flag register in c54 core */
	signal_t ifr[16]; 
}c54_signal_t;
typedef	struct{
	/* set interrupt flag register in c28 core */
    int pie_x;
    int pie_y;
    int pie_group;
	signal_t intr[12][8]; //PIE VECTOR TABLE have 96 interrupts. 12Groups, every group have 8 interrupt,so have 96 interrupts.  
	signal_t cpu_intr[16];
	irq_type_t irq_type; 
}c28_signal_t;
typedef	struct{
	/* set interrupt flag register in sparc core */
	signal_t irq_no; 
	signal_t level;
}sparc_signal_t;

typedef union interrupt_signal{
	arm_signal_t arm_signal;
	mips_signal_t mips_signal;
	powerpc_signal_t powerpc_signal;
	c6k_signal_t c6k_signal;
	c5k_signal_t c5k_signal;
	c54_signal_t c54_signal;
	c28_signal_t c28_signal;
	sparc_signal_t sparc_signal;
}interrupt_signal_t;

typedef struct signal_interface{
	conf_object_t* conf_obj;
	int (*raise_signal)(conf_object_t* target, int line);
	int (*lower_signal)(conf_object_t* target, int line);
	int (*event_signal)(conf_object_t* target, int line,void *args);
}general_signal_intf;
typedef general_signal_intf general_signal_iface_t;

#define  GENERAL_SIGNAL_INTF_NAME "general_signal_intf"

exception_t send_signal(conf_object_t* conf_obj, interrupt_signal_t* signal);

typedef struct simple_signal_interface{
	conf_object_t* conf_obj;
	int (*trigger)(conf_object_t* target);
}simple_signal_intf;
#define SIMPLE_SIGNAL_INTF_NAME "simple_signal_intf"

typedef enum {
	SIGNAL_Inactive,
	SIGNAL_Pending,
	SIGNAL_Active,
} signal_status_t;

typedef struct core_signal{
	conf_object_t* obj;
	exception_t (*signal)(conf_object_t* obj, interrupt_signal_t* signal);
	signal_status_t (*get_signal_status)(conf_object_t* obj);
}core_signal_intf;
#define CORE_SIGNAL_INTF_NAME "core_signal_intf"

typedef struct core_vruntime_interval {
	conf_object_t* obj;
	int (*set_vruntime_interval)(conf_object_t* obj, uint64_t interval);
	int (*unset_vruntime_interval)(conf_object_t* obj);
} core_vruntime_interval_intf;
#define CORE_VRUNTIME_INTERVAL "core_vruntime_interval"

typedef struct lidd_lcd {
	conf_object_t *obj;
	int (*set_cs_data)(conf_object_t *obj, uint32_t cs_data);
	int (*set_cs_addr)(conf_object_t *obj, uint32_t cs_addr);
} lidd_lcd_intf;
#define LIDD_LCD_INTF_NAME "lidd_lcd_intf"

#endif

/*
	skyeye_mach_s3c6410x.c - define machine S3C6410 for skyeye
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
/**
* @file skyeye_mach_s3c6410x.c
* @brief The implementation of 6410
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-11-12
*/


#include <skyeye_config.h>
#include <skyeye_core.h>
#include <skyeye_sched.h>
#include <skyeye_lock.h>
#include <skyeye_class.h>
#include <skyeye_addr_space.h>
#include <skyeye_android_intf.h>
#include "s3c6410x.h"
#include "skyeye_internal.h"
#include "skyeye_checkpoint.h"
#include <skyeye_interface.h>
#include <skyeye_lcd_intf.h>
#include <skyeye_keypad_intf.h>
#include <skyeye_io.h>
#include <skyeye_log.h>
#include <skyeye_uart.h>
#include <skyeye_mm.h>
#include <skyeye_signal.h>
#include <skyeye_attr.h>
#include <skyeye_attribute.h>
#include <skyeye_iface.h>
#include <skyeye_core_intf.h>
#include <skyeye_system.h>

#ifdef __CYGWIN__
#include <time.h>
#endif
s3c6410x_io_t s3c6410x_io;
lcd_control_intf* mach_lcd_ctrl = NULL;
#define io s3c6410x_io

typedef struct s3c6410_mach{
	conf_object_t* obj;
	common_machine_attr_t attr;
	addr_space_t* space;
	conf_object_t* core;
	conf_object_t *signal;
	general_signal_intf  *signal_iface;
}s3c6410_mach_t;

int s3c6410_update_boot_info(conf_object_t *obj)
{
	int i;
	s3c6410_mach_t *mach = obj->obj;
	sys_soc_t *soc = obj->sys_struct;
	for (i = 0; i < soc->cpu_cnt; i++) {
		sys_cpu_t *cpu = soc->cpus + i;
		//skyeye_reg_intf *reg_iface = (skyeye_reg_intf *)SKY_get_iface(cpu->cpu, SKYEYE_REG_INTF);
		/* The environment for boot linux */
		//reg_iface->set_regvalue_by_id(cpu->cpu, 0, 0);
		/* machine ID for SMDK6410 */
		//reg_iface->set_regvalue_by_id(cpu->cpu, 1626, 1);
		/* The atag or dtb address for linux booting */
		//reg_iface->set_regvalue_by_id(cpu->cpu, 0x50000100, 2);
	}
	return 0;
}

static void
s3c6410x_update_int (void *arch_instance)
{
	interrupt_signal_t interrupt_signal;
	if (io.vic0fiqstatus != 0 || io.vic1fiqstatus != 0) {
		interrupt_signal.arm_signal.firq =  Low_level;
	} else {
		interrupt_signal.arm_signal.firq =  High_level;
	}

	if (io.vic0irqstatus != 0 || io.vic1irqstatus != 0) {
		interrupt_signal.arm_signal.irq =  Low_level;
	} else {
		interrupt_signal.arm_signal.irq =  High_level;
	}
	interrupt_signal.arm_signal.reset =  Prev_level;
	//conf_object_t* core = SIM_current_processor();
	conf_object_t *core = get_conf_obj("arm11_0");
	send_signal(core, &interrupt_signal);
}



static void
io_do_cycle (conf_object_t* conf_obj)
{
	int i;
	io.tc_prescale --;
	if (io.tc_prescale < 0) {
	}
#ifdef MK_LCD
	static int lcd_prescale = 10000;
	if(lcd_prescale-- <= 0){
		lcd_prescale = 10000;
		mach_lcd_ctrl->lcd_update(mach_lcd_ctrl->conf_obj);
	}
#endif
}





typedef struct s3c6410_vic_dev{
	conf_object_t* obj;
}s3c6410_vic_device;
static int vic_raise_signal(conf_object_t* target, int line){
	int irq_no;
	if(line < 32){	/* line < 32 vic0 */
		irq_no = line;
		io.vic0rawintr |= 1 << irq_no;
		io.vic0irqstatus |=  ((1 << irq_no) & ~(io.vic0intselect) & io.vic0intenable);
		io.vic0fiqstatus |=  ((1 << irq_no) & io.vic0intselect & io.vic0intenable);
	}else{		/* line >= 32 vic1 */
		irq_no = line - 32;
		io.vic1rawintr |= 1 << irq_no;
		io.vic1irqstatus |=  ((1 << irq_no) & ~(io.vic1intselect) & io.vic1intenable);
		io.vic1fiqstatus |=  ((1 << irq_no) & io.vic1intselect & io.vic1intenable);
	}

	s3c6410x_update_int (NULL);
	return 0;
}
static int vic_lower_signal(conf_object_t* target, int line){
	int irq_no;
	if(line < 32){	/* line < 32 vic0 */
		irq_no = line;
		io.vic0irqstatus &= ~irq_no;
	}else{		/* line >= 32 vic1 */
		irq_no = line - 32;
		io.vic1irqstatus &= ~irq_no;
	}

	s3c6410x_update_int(NULL);
	return 0;
}


static conf_object_t* new_s3c6410_mach(char* obj_name){
	s3c6410_mach_t* mach = skyeye_mm_zero(sizeof(s3c6410_mach_t));
	mach->obj = new_conf_object(obj_name, mach);
	mach->attr.core_num = 0;

		/*
		 *
	SKY_register_interface(io_memory, mach->obj, MEMORY_SPACE_INTF_NAME);	
	*/

#if 0
	/* Register lcd io memory to the whole address space */
	conf_object_t* lcd = pre_conf_obj("s3c6410_lcd_0", "s3c6410_lcd");
	if(lcd != NULL){
		memory_space_intf* lcd_io_memory = (memory_space_intf*)SKY_get_interface(lcd, MEMORY_SPACE_INTF_NAME);
		DBG("In %s, get the interface instance 0x%x\n", __FUNCTION__, lcd_io_memory);
		exception_t ret;
        	ret = SKY_add_map(phys_mem, lcd, 0x77100000, 0x100000, 0x0, lcd_io_memory, 1, 1);

		if (getenv("ANDROID") != NULL)
		{
			conf_object_t* sdl_painter = pre_conf_obj("lcd_sdl_0", "lcd_sdl");

			conf_object_t* android = pre_conf_obj("android_0", "android");
			android_interface_t* android_if = SKY_get_interface(android, ANDROID_INTF_NAME);

			conf_object_t* keypad = pre_conf_obj("s3c6410_keypad_0", "s3c6410_keypad");
			memory_space_intf* keypad_io_memory = (memory_space_intf*)SKY_get_interface(keypad, MEMORY_SPACE_INTF_NAME);
			lcd_keypad_t* lcd_keypad = (lcd_control_intf*)SKY_get_interface(keypad, LCD_KEYPAD_INTF_NAME);
			DBG("In %s, get the interface instance 0x%x\n", __FUNCTION__, keypad_io_memory);
			ret = SKY_add_map(phys_mem, keypad, 0x7E00A000, 0x1000, 0x0, keypad_io_memory, 1, 1);
			if(ret != No_exp){
				skyeye_log(Error_log, __FUNCTION__, "Can not register io memory for keypad\n");
			}

			/* register touchscreen for lcd_gtk */
			SKY_register_iface(lcd_ts, sdl_painter->objname, LCD_TS_INTF_NAME);

			/* register keypad for lcd_gtk */
			SKY_register_iface(lcd_keypad, sdl_painter->objname, LCD_KEYPAD_INTF_NAME);

			/* register keypad for lcd_gtk */
			SKY_register_iface(android_if, sdl_painter->objname, ANDROID_INTF_NAME);

			lcd_control_intf* lcd_ctrl = (lcd_control_intf*)SKY_get_interface(sdl_painter, LCD_CTRL_INTF_NAME);
			attr_value_t* attr = make_new_attr(Val_ptr, lcd_ctrl);

			mach_lcd_ctrl = lcd_ctrl;
			/* set the attribute of lcd */
			SKY_set_attr(lcd, "lcd_ctrl_0", attr);

			simple_signal_intf* refresh_signal = (simple_signal_intf*)SKY_get_interface(sdl_painter, SIMPLE_SIGNAL_INTF_NAME);
			simple_signal_intf* slave_signal = (simple_signal_intf*)SKY_get_interface(lcd, SIMPLE_SIGNAL_INTF_NAME);
			/* connect the signal line, so lcd get notified when gtk finished refresh */
			refresh_signal->conf_obj = slave_signal->conf_obj;
			refresh_signal->trigger = slave_signal->trigger;

			general_signal_intf* keypad_intr_signal = (lcd_control_intf*)SKY_get_interface(keypad, GENERAL_SIGNAL_INTF_NAME);
			keypad_intr_signal->conf_obj = vic_signal->conf_obj;
			keypad_intr_signal->raise_signal = vic_signal->raise_signal;
			keypad_intr_signal->lower_signal = vic_signal->lower_signal;
			android_if->start_android();
		}
		else{
#ifdef GTK_LCD
			/* set the lcd_ctrl_0 attribute for lcd */
			conf_object_t* gtk_painter = pre_conf_obj("gtk_lcd_0", "gtk_lcd");
			/* register touchscreen for lcd_gtk */
			SKY_register_interface(lcd_ts, gtk_painter->objname, LCD_TS_INTF_NAME);

			lcd_control_intf* lcd_ctrl = (lcd_control_intf*)SKY_get_interface(gtk_painter, LCD_CTRL_INTF_NAME);
			attr_value_t* attr = make_new_attr(Val_ptr, lcd_ctrl);

			mach_lcd_ctrl = lcd_ctrl;
			/* set the attribute of lcd */
			SKY_set_attr(lcd, "lcd_ctrl_0", attr);

			simple_signal_intf* refresh_signal = (simple_signal_intf*)SKY_get_interface(gtk_painter, SIMPLE_SIGNAL_INTF_NAME);
			simple_signal_intf* slave_signal = (simple_signal_intf*)SKY_get_interface(lcd, SIMPLE_SIGNAL_INTF_NAME);
			/* connect the signal line, so lcd get notified when gtk finished refresh */
			refresh_signal->conf_obj = slave_signal->conf_obj;
			refresh_signal->trigger = slave_signal->trigger;
#endif
		}

		general_signal_intf* lcd_intr_signal = (lcd_control_intf*)SKY_get_interface(lcd, GENERAL_SIGNAL_INTF_NAME);
		lcd_intr_signal->conf_obj = vic_signal->conf_obj;
		lcd_intr_signal->raise_signal = vic_signal->raise_signal;
		lcd_intr_signal->lower_signal = vic_signal->lower_signal;


	}
	else{
		printf("can not initlize the lcd, maybe the module not exist\n");
	}
	register_io_cycle(io_do_cycle, NULL);
#endif 
	return  mach->obj;

}



static void s3c6410_register_attribute(conf_class_t* clss){
}

static void s3c6410_register_interface(conf_class_t* clss){
	static const skyeye_machine_intf inface = {
		.update_boot_info = s3c6410_update_boot_info
	};
	SKY_register_iface(clss, MACHINE_INTF_NAME, &inface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MACHINE_INTF_NAME,
			.iface = &inface,
		}
 	};
	class_register_ifaces(clss, ifaces);
	return;
}


static void free_s3c6410_mach(conf_object_t *mach)
{
	s3c6410_mach_t *s3c6410_mach = mach->obj;
	skyeye_free(s3c6410_mach->obj);
	skyeye_free(s3c6410_mach);
	return;
}

static exception_t reset_s3c6410_mach(conf_object_t* mach)
{
	s3c6410_mach_t *s3c6410_mach = mach->obj;
	addr_space_t *phys_mem = s3c6410_mach->attr.space;
	space_obj_reset(phys_mem, NULL);
	exception_t ret = reset_conf_obj(s3c6410_mach->attr.core);
	return No_exp;
}
void init_s3c6410x_mach(){
	static skyeye_class_t class_data = {
		.class_name = "s3c6410",
		.class_desc = "s3c6410 machine",
		.new_instance = new_s3c6410_mach,
		.free_instance = free_s3c6410_mach,
		.get_attr = NULL,
		.set_attr = NULL
	};
	conf_class_t* class = SKY_register_soc_class(class_data.class_name, &class_data);
	s3c6410_register_attribute(class);
	s3c6410_register_interface(class);
}

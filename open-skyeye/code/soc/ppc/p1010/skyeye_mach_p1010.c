/*
	skyeye_mach_p1010.c - define machine AM3359 for skyeye
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
* @file skyeye_mach_p1010.c
* @date 2015-11-02
*/


#include <skyeye_config.h>
#include <skyeye_core.h>
#include <skyeye_sched.h>
#include <skyeye_lock.h>
#include <skyeye_class.h>
#include <skyeye_addr_space.h>
#include <skyeye_android_intf.h>
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
#include "arch/ppc/common/ppc_boot.h"


typedef struct p1010_mach{
	conf_object_t* obj;
} p1010_mach_t;

static int set_bootcmd(sys_cpu_t *cpu)
{
	skyeye_reg_intf *reg_iface;
	memory_space_intf *memory_iface;

	char * bootcmd = "root=/dev/ram0 console=ttyCPM0 mem=64M";
	const int bootcmd_start= 9 * 1024 * 1024;

	const int bd_start = 8 * 1024 * 1024;
	const int initrd_start  = 32 * 1024 * 1024, initrd_size = 2 * 1024 * 1024;
	bd_t t;
	size_t addr, cnt;
	char *src;

	if ((reg_iface = (skyeye_reg_intf*)SKY_get_iface(cpu->cpu, SKYEYE_REG_INTF)) == NULL)
		return -1;
	if ((memory_iface = (memory_space_intf *)SKY_get_iface(cpu->cpu, MEMORY_SPACE_INTF_NAME)) == NULL)
		return -2;

	memset(&t, '\0', sizeof(t));

        t.bi_immr_base = 0xe0000000;
        t.bi_busfreq = 100 * 1024 * 1024;
        t.bi_intfreq = 500 * 1024 * 1024;
        t.bi_baudrate = 9600;
        t.bi_memsize = 64 * 1024 * 1024;

	for (cnt = 0, src = (char *)&t, addr = bd_start; cnt < sizeof(t); cnt++) {
		memory_iface->write(cpu->cpu, addr++, &src[cnt], 1);
	}

	for (cnt = 0, src = bootcmd, addr = bootcmd_start; cnt < strlen(bootcmd) + 1; cnt++) {
		memory_iface->write(cpu->cpu, addr++, &src[cnt], 1);
	}

	/* r4 - Starting address of the init RAM disk, r5 - Ending address of the init RAM disk */
	reg_iface->set_regvalue_by_id(cpu->cpu, bd_start, 3);
	reg_iface->set_regvalue_by_id(cpu->cpu, initrd_start, 4);
	reg_iface->set_regvalue_by_id(cpu->cpu, initrd_start + initrd_size, 5);
	reg_iface->set_regvalue_by_id(cpu->cpu, bootcmd_start, 6);
	reg_iface->set_regvalue_by_id(cpu->cpu, bootcmd_start + strlen(bootcmd) + 1, 7);

	return 0;
}
static int p1010_update_boot_info(conf_object_t *obj)
{
	p1010_mach_t *mach = obj->obj;
	sys_soc_t *soc = obj->sys_struct;

	skyeye_reg_intf *reg_iface;
	sys_cpu_t *cpu;
	int i;

#if 1
	for(i = 0; i < soc->cpu_cnt; i++){
		if ((cpu = soc->cpus + i) != NULL) {
			/* The environment for boot linux */
			if ((reg_iface = (skyeye_reg_intf*)SKY_get_iface(cpu->cpu, SKYEYE_REG_INTF)) == NULL)
				continue;
			//set_bootcmd(cpu);
			reg_iface->set_regvalue_by_id(cpu->cpu, 0x00800000, 3);
		}
	}
#endif
	return 0;
}

static conf_object_t *new_p1010_mach(char* obj_name){
	p1010_mach_t* mach = skyeye_mm_zero(sizeof(p1010_mach_t));
	mach->obj = new_conf_object(obj_name, mach);

	return  mach->obj;
}

static void p1010_register_attribute(conf_class_t* clss){
}

static void p1010_register_interface(conf_class_t* clss){
	static const skyeye_machine_intf inface = {
		.update_boot_info = p1010_update_boot_info
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
static void free_p1010_mach(conf_object_t *mach)
{
	p1010_mach_t *p1010_mach = mach->obj;
	skyeye_free(p1010_mach->obj);
	skyeye_free(p1010_mach);
	return;
}
void init_p1010_mach(void){
	static skyeye_class_t class_data = {
		.class_name = "p1010",
		.class_desc = "p1010 machine",
		.new_instance = new_p1010_mach,
		.free_instance = free_p1010_mach,
		.get_attr = NULL,
		.set_attr = NULL
	};
	conf_class_t* class = SKY_register_soc_class(class_data.class_name, &class_data);

	p1010_register_attribute(class);
	p1010_register_interface(class);
}

/* Copyright (C) 2007-2008 The Android Open Source Project
**
** This software is licensed under the terms of the GNU General Public
** License version 2, as published by the Free Software Foundation, and
** may be copied, distributed, and modified under those terms.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
*/
//#include "android/android.h"
//#include "android/utils/debug.h"
//#include "android/utils/duff.h"
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#define DEBUG
#include <skyeye_log.h>

#include "sdhci_s3c6410.h"

static exception_t s3c6410_sdhci_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	uint32_t ret;
	struct s3c6410_fb_device *s = opaque->obj;
	DBG("In %s, offset=0x%x\n", __FUNCTION__, offset);
	switch(offset) {
	default:
		printf("Can not read the register at 0x%x\n", offset);
		return 0;
	}
}

//static void s3c6410_fb_write(void *opaque, target_phys_addr_t offset,
 //                       uint32_t val)
static exception_t s3c6410_sdhci_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct s3c6410_sdhci_device *s = opaque->obj;
	DBG("In %s, offset=0x%x\n", __FUNCTION__, offset);
	uint32_t val = *(uint32_t*)buf;
	switch(offset) {
        default:
            break;
	}

	return No_exp;
}
static conf_object_t* new_s3c6410_sdhci(char* obj_name){
	s3c6410_sdhci_device* dev = skyeye_mm_zero(sizeof(s3c6410_sdhci_device));
	dev->obj = new_conf_object(obj_name, dev);
	sdhci_reg_t* regs =  skyeye_mm_zero(sizeof(sdhci_reg_t));
	dev->regs = regs;

	return dev->obj;
}
void free_s3c6410_sdhci(conf_object_t* dev){
	
}

void init_s3c6410_sdhci(){
	static skyeye_class_t class_data = {
		.class_name = "s3c6410_sdhci",
		.class_desc = "s3c6410 sdhci to support mmc",
		.new_instance = new_s3c6410_sdhci,
		.free_instance = free_s3c6410_sdhci,
		.get_attr = NULL,
		.set_attr = NULL
	};
	conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = s3c6410_sdhci_read,
		.write = s3c6410_sdhci_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		}
 	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, CONNECTS_NULL);

	return;
}

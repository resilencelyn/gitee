/* Copyright (C)
 * 2013 - David Yu keweihk@gmail.com
 * This progflash_s29al016d is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This progflash_s29al016d is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this progflash_s29al016d; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 *
 */
/**
 * @file flash_s29al016d.c
 * @brief The implementation of flash_s29al016d
 * @author David Yu keweihk@gmail.com
 * @version 78.77
 * @date 2013-04-08
 */

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_attribute.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_dev_intf.h>
#include <skyeye_uart_ops.h>
#include <skyeye_arch.h>
#include <skyeye_swapendian.h>
#define DEBUG
#include <skyeye_log.h>

#include "flash_s29al016d.h"

static exception_t flash_s29al016d_read(conf_object_t *opaque, generic_address_t addr, void* buf, size_t count)
{
	flash_s29al016d_device_t* dev = opaque->obj;
	exception_t ret;

	if (dev->image_space)
		ret = dev->image_space->read(dev->image, addr, buf, count);
//	printf("read addr :%x, buf:%x\n", addr, *(uint16_t*)buf);
	if(ret != No_exp){
		printf("In %s, %s Not_found addr 0x%x\n", __func__, dev->obj->class_name, addr);
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t flash_s29al016d_write(conf_object_t *opaque, generic_address_t addr, void* buf, size_t count)
{

	flash_s29al016d_device_t *dev = opaque->obj;
	uint32_t start, end = 0;
	uint16_t data = *(uint16_t*)buf;
	exception_t ret;

	/* judge the endianess */
	if(dev->endian == Big_endian)
		data = half_from_BE(*(uint16_t*)buf);
	else
		data = *(uint16_t*)buf;

	//printf("In %s, data :%x, count:%x, cnt:%x\n", __func__, data, count, dev->cnt);
	
	if (CMD_WORD_PROGRAM(dev)) {
		/* write data to addr */
		if (dev->image_space)
			ret = dev->image_space->write(dev->image, addr, buf, count);

		if (ret != No_exp)
			printf("write error data :%x at addr :%x\n", addr, data);
		goto reset;
	}

	if (CMD_ERASE(dev)) {
		switch (data) {
			case 0x10: /* Chip-Erase 256kBytes*/
				start = addr;
				end = start + CHIP_SIZE;
				break;

			case 0x30: /* Sector-Erase: 8KBytes/sector */
				start = addr;
				end = start + SECTOR_SIZE;
				break;
			default:
				start = end = 0x0;
				break;
		}

		if (end > start && end <= CHIP_SIZE) {
			for (addr = start; addr < end; addr += count) {
				if (dev->image_space)
					ret = dev->image_space->write(dev->image, addr, &dev->erase_val, count);
				if (ret != No_exp)
					printf("write error data :%x at addr :%x\n", addr, data);
			}
			
			DEBUG("*** Erase(start:0x%08x, end:0x%08x)\n", start, end);
		} else {
			printf("*** ERROR: Erase(start:0x%08x, end:0x%08x)\n", start, end);
		}

		goto reset;
	}

	if (dev->n_bus < 6) {
		dev->bus[dev->n_bus].addr = addr;
		dev->bus[dev->n_bus].data = data;
		dev->n_bus += 1;

		dev->cnt = dev->n_bus;

	}
	goto exit;

reset:
	dev->cnt = dev->n_bus = 0;
	memset(&dev->bus[0], 0, sizeof(dev->bus[0]) * 6);
exit:
	return No_exp;
}

static conf_object_t* new_flash_s29al016d(char* obj_name)
{
	flash_s29al016d_device_t* dev= skyeye_mm_zero(sizeof(flash_s29al016d_device_t));
	dev->obj = new_conf_object(obj_name, dev);
	dev->erase_val = 0xffff;
	
	return dev->obj;
}

static void free_flash_s29al016d(conf_object_t* conf_obj){
	flash_s29al016d_device_t* dev = conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev);
}

static exception_t reset_flash_s29al016d(conf_object_t* opaque, const char* args)
{
	flash_s29al016d_device_t *dev = opaque->obj;

	return No_exp;
}
static attr_value_t get_attr_endian(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	flash_s29al016d_device_t* dev = (flash_s29al016d_device_t*)conf_obj->obj;
	return SKY_make_attr_uinteger(dev->endian);
}

static exception_t set_attr_endian(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	flash_s29al016d_device_t* dev = (flash_s29al016d_device_t*)conf_obj->obj;
	dev->endian = SKY_attr_uinteger(*value);
	return No_exp;
}

void flash_s29al016d_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "endian", get_attr_endian, NULL, set_attr_endian, NULL, SKY_Attr_Optional, "uinteger", "endian of flash_s29al016d");
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	flash_s29al016d_device_t* dev = (flash_s29al016d_device_t*)obj->obj;
	dev->image = obj2;
	dev->image_space = (memory_space_intf*)SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);
	if (dev->image_space == NULL){
		if (dev->image == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", MEMORY_SPACE_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", MEMORY_SPACE_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
 
static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	flash_s29al016d_device_t* dev = (flash_s29al016d_device_t*)obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}
void init_flash_s29al016d(){
	static skyeye_class_t class_data = {
		.class_name = "flash_s29al016d",
		.class_desc = "flash_s29al016d device",
		.new_instance = new_flash_s29al016d,
		.free_instance = free_flash_s29al016d,
		.reset_instance = reset_flash_s29al016d,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = flash_s29al016d_read,
		.write = flash_s29al016d_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);	

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = MEMORY_SPACE_INTF_NAME,
			.set = memory_set,
			.get = memory_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	flash_s29al016d_register_attribute(clss);
}

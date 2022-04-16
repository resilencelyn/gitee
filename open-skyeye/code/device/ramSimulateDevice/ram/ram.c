/* Copyright (C)
 * 2013 - David Yu keweihk@gmail.com
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 *
 */
/**
 * @file ram.c
 * @brief The implementation of ram
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
#include <skyeye_arch.h>
#include <skyeye_swapendian.h>
#define DEBUG
#include <skyeye_log.h>

#include "ram.h"
static exception_t ram_read_word(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint32_t data;
	exception_t ret;
	ram_device_t* dev = opaque->obj;

	if (dev->image_space)
		ret = dev->image_space->read(dev->image, offset, &data, 4);
	if(ret != No_exp){
		printf("In %s, %s not found offset 0x%x\n", __func__, opaque->objname, offset);
		return Not_found_exp;
	}
	/* judge the endianess */
	if(dev->endian == Big_endian)
		*(uint32_t*)buf = word_from_BE(data);
	else
		*(uint32_t*)buf = data;

	return No_exp;
}

static exception_t ram_read_halfword(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint16_t data;
	exception_t ret;
	ram_device_t* dev = opaque->obj;

	if (dev->image_space)
		ret = dev->image_space->read(dev->image, offset, &data, 2);
	if(ret != No_exp){
		printf("In %s, %s not found offset 0x%x\n", __func__, opaque->objname, offset);
		return Not_found_exp;
	}
	/* judge the endianess */
	if(dev->endian == Big_endian)
		*(uint16_t*)buf = half_from_BE(data);
	else
		*(uint16_t*)buf = data;
	return No_exp;
}

static exception_t ram_read_byte(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint8_t data;
	exception_t ret;
	ram_device_t* dev = opaque->obj;

	if (dev->image_space)
		ret = dev->image_space->read(dev->image, offset, &data, 1);
	if(ret != No_exp){
		printf("In %s, %s not found offset 0x%x\n", __func__, opaque->objname, offset);
		return Not_found_exp;
	}
	//generic_arch_t* arch_instance = get_arch_instance(NULL);
	*(uint8_t*)buf = data;
	
	return No_exp;
}

static exception_t ram_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	ram_device_t *dev = opaque->obj;
	switch(count){
		case 4:
			return  ram_read_word(opaque, offset,  buf);
		case 2:
			return  ram_read_halfword(opaque, offset,  buf);
		case 1:
			return  ram_read_byte(opaque, offset,  buf);
		default:
			printf("In %s, count %d is error!\n", __func__, count);
			break;
	}

	return No_exp;
}

static exception_t ram_write_word(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint32_t data;
	exception_t ret;
	ram_device_t* dev = opaque->obj;
	//generic_arch_t* arch_instance = get_arch_instance(NULL);
	/* judge the endianess */
	if(dev->endian == Big_endian)
		data = word_from_BE(*(uint32_t*)buf);
	else
		data = *(uint32_t*)buf;

	if (dev->image_space)
		ret = dev->image_space->write(dev->image, offset, &data, 4);
	if(ret != No_exp){
		printf("In %s, %s not_found offset 0x%x\n", __func__, opaque->objname, offset);
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t ram_write_halfword(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint16_t data;
	exception_t ret;
	ram_device_t* dev = opaque->obj;
	//generic_arch_t* arch_instance = get_arch_instance(NULL);
	/* judge the endianess */
	if(dev->endian == Big_endian)
		data = half_from_BE(*(uint16_t*)buf);
	else
		data = *(uint16_t*)buf;

	if (dev->image_space)
		ret = dev->image_space->write(dev->image, offset, &data, 2);
	if(ret != No_exp){
		printf("In %s, %s not found offset 0x%x\n", __func__, opaque->objname, offset);
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t ram_write_byte(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint8_t data;
	exception_t ret;
	ram_device_t* dev = opaque->obj;
	//generic_arch_t* arch_instance = get_arch_instance(NULL);
	/* judge the endianess */
	data = *(uint8_t*)buf;

	if (dev->image_space)
		ret = dev->image_space->write(dev->image, offset, &data, 1);
	if(ret != No_exp){
		printf("In %s, %s not found offset 0x%x\n", __func__, opaque->objname, offset);
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t ram_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	switch(count){
		case 4:
			return  ram_write_word(opaque, offset,  buf);
		case 2:
			return  ram_write_halfword(opaque, offset,  buf);
		case 1:
			return  ram_write_byte(opaque, offset,  buf);
		default:
			printf("In %s, count %d is error!\n", __func__, count);
	}
	return No_exp;
}

static get_page_t * ram_get_page(conf_object_t *obj, physical_address_t offset){
	ram_device_t *dev = obj->obj;
	if(dev->image_space){
		return dev->image_space->get_page(dev->image, offset);
	}
	return (get_page_t *)NULL;
}
static conf_object_t* new_ram(char* obj_name)
{
	ram_device_t* dev= skyeye_mm_zero(sizeof(ram_device_t));
	dev->obj = new_conf_object(obj_name, dev);
	return dev->obj;
}

static void free_ram(conf_object_t* conf_obj){
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
}

static exception_t reset_ram(conf_object_t* opaque, const char* args)
{
	ram_device_t *dev = opaque->obj;
	char result[64];
	int index;
	attr_value_t value;
	return No_exp;
}

static attr_value_t get_attr_endian(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	ram_device_t* dev = (ram_device_t*)conf_obj->obj;
	return SKY_make_attr_uinteger(dev->endian);
}

static exception_t set_attr_endian(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	ram_device_t* dev = (ram_device_t*)conf_obj->obj;
	dev->endian = SKY_attr_uinteger(*value);
	return No_exp;
}

static exception_t image_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	ram_device_t* dev = (ram_device_t*)obj->obj;
	dev->image = obj2;
	dev->image_space = (memory_space_intf*)SKY_get_iface(obj2, MEMORY_SPACE_INTF_NAME);

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

static exception_t image_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	ram_device_t* dev = (ram_device_t*)obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}

void ram_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "endian", get_attr_endian, NULL, set_attr_endian, NULL, SKY_Attr_Optional, "uinteger", "endian of ram");
}

void init_ram(){
	static skyeye_class_t class_data = {
		.class_name = "ram",
		.class_desc = "ram device",
		.new_instance = new_ram,
		.free_instance = free_ram,
		.reset_instance = reset_ram,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_am335x SC_am3359 SC_at91rm9200 SC_imx6 SC_omapl138 SC_s3c2410x1 SC_s3c6410 SC_stm32 SC_c6416 SC_c6446 SC_c6474 SC_c6678 SC_c6713 
						SC_c6746 SC_c6748 SC_c2812 SC_f28335 SC_c5416 SC_c55 SC_c5510 SC_mpc755 SC_mpc5121 SC_mpc8245 SC_mpc8378 SC_mpc8560 SC_mpc8641d_mach SC_p1010 
						SC_t2080 SC_leon2 SC_tsc695f),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = ram_read,
		.write = ram_write,
		.get_page = ram_get_page,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);	

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.set = image_set,
			.get = image_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	ram_register_attribute(clss);
}

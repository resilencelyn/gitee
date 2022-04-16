/* Copyright (C)
 * 2013 - David Yu keweihk@gmail.com
 * This progc6713_flash is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This progc6713_flash is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this progc6713_flash; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 *
 */
/**
 * @file c6713_flash.c
 * @brief The implementation of c6713_flash
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

#include "c6713_flash.h"
#if 0
static char* c6713_flash_attr[] = {"image", "endian"};
static exception_t c6713_flash_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	c6713_flash_device_t *dev = opaque->obj;
	int index;
	//parse the pac6713_flasheter
	for(index = 0; index < 3; index++){
		if(!strncmp(attr_name, c6713_flash_attr[index], strlen(c6713_flash_attr[index])))
			break;
	}
	switch(index){
		case 0:
			dev->image = (memory_space_intf*)SKY_get_iface(value->u.object, MEMORY_SPACE_INTF_NAME);
			dev->image_intface = (skyeye_image_intf *)SKY_get_iface(value->u.object, SKYEYE_IMAGE_INTF_NAME);
			break;
		case 1:
			dev->endian = value->u.uinteger;
		default:
			printf("pac6713_flasheter error\n");
			return Invarg_exp;
	}
	return No_exp;
}
#endif
static exception_t c6713_flash_read_word(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint32_t data;
	exception_t ret;
	c6713_flash_device_t* dev = opaque->obj;

	if (dev->image_space)
		ret = dev->image_space->read(dev->image, offset, &data, 4);
	if(ret != No_exp){
		printf("In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
		return Not_found_exp;
	}
	//generic_arch_t* arch_instance = get_arch_instance(NULL);
	/* judge the endianess */
	if(dev->endian == Big_endian)
		*(uint32_t*)buf = word_from_BE(data);
	else
		*(uint32_t*)buf = data;

	return No_exp;
}

static exception_t c6713_flash_read_halfword(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint16_t data;
	exception_t ret;
	c6713_flash_device_t* dev = opaque->obj;

	if (dev->image_space)
		ret = dev->image_space->read(dev->image, offset, &data, 2);

	if(ret != No_exp){
		printf("In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
		return Not_found_exp;
	}
	//generic_arch_t* arch_instance = get_arch_instance(NULL);
	/* judge the endianess */
	if(dev->endian == Big_endian)
		*(uint16_t*)buf = half_from_BE(data);
	else
		*(uint16_t*)buf = data;
	return No_exp;
}

static exception_t c6713_flash_read_byte(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint8_t data;
	exception_t ret;
	c6713_flash_device_t* dev = opaque->obj;

	if (dev->image_space)
		ret = dev->image_space->read(dev->image, offset, &data, 1);

	if(ret != No_exp){
		printf("In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
		return Not_found_exp;
	}
	//generic_arch_t* arch_instance = get_arch_instance(NULL);
	*(uint8_t*)buf = data;
	
	return No_exp;
}

static exception_t c6713_flash_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	c6713_flash_device_t *dev = opaque->obj;
	switch(count){
		case 4:
			return  c6713_flash_read_word(opaque, offset,  buf);
		case 2:
			return  c6713_flash_read_halfword(opaque, offset,  buf);
		case 1:
			return  c6713_flash_read_byte(opaque, offset,  buf);
		default:
			printf("In %s, count %d is error!\n", __func__, count);
			break;
	}

	return No_exp;
}

static exception_t c6713_flash_write_word(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint32_t data;
	exception_t ret;
	c6713_flash_device_t* dev = opaque->obj;
	/* judge the endianess */

	if(dev->endian == Big_endian)
		data = word_from_BE(*(uint32_t*)buf);
	else
		data = *(uint32_t*)buf;

	if (dev->image_space)
		ret = dev->image_space->write(dev->image, offset, &data, 4);

	if(ret != No_exp){
		printf("In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t c6713_flash_write_halfword(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint16_t data;
	exception_t ret;
	c6713_flash_device_t* dev = opaque->obj;

	/* judge the endianess */
	if(dev->endian == Big_endian)
		data = half_from_BE(*(uint16_t*)buf);
	else
		data = *(uint16_t*)buf;

	if (dev->image_space)
		ret = dev->image_space->write(dev->image, offset, &data, 2);

	if(ret != No_exp){
		printf("In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t c6713_flash_write_byte(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint8_t data;
	exception_t ret;
	c6713_flash_device_t* dev = opaque->obj;
	/* judge the endianess */
	data = *(uint8_t*)buf;

	if (dev->image_space)
		ret = dev->image_space->write(dev->image, offset, &data, 1);

	if(ret != No_exp){
		printf("In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t c6713_flash_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	switch(count){
		case 4:
			return  c6713_flash_write_word(opaque, offset,  buf);
		case 2:
			return  c6713_flash_write_halfword(opaque, offset,  buf);
		case 1:
			return  c6713_flash_write_byte(opaque, offset,  buf);
		default:
			printf("In %s, count %d is error!\n", __func__, count);
	}
	return No_exp;
}

static get_page_t * c6713_flash_get_page(conf_object_t *obj, physical_address_t offset){
	c6713_flash_device_t *dev = obj->obj;
	if(dev->image_space){
		return dev->image_space->get_page(dev->image, offset);
	}
	return (get_page_t *)NULL;
}
static conf_object_t* new_c6713_flash(char* obj_name)
{
	c6713_flash_device_t* dev= skyeye_mm_zero(sizeof(c6713_flash_device_t));
	dev->obj = new_conf_object(obj_name, dev);
	return dev->obj;
}

static void free_c6713_flash(conf_object_t* conf_obj){
	c6713_flash_device_t* dev = conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev);
}

static exception_t reset_c6713_flash(conf_object_t* opaque, const char* args)
{
	c6713_flash_device_t *dev = opaque->obj;
	char result[64];
	int index;
	attr_value_t value;
#if 0
	if(args == NULL){
		//default operations
		return No_exp;
	}else{
		//parse the pac6713_flasheter
		for(index = 0; index < 3; index++){
			if(!strncmp(args, c6713_flash_attr[index], strlen(c6713_flash_attr[index])))
				break;
		}
	}
#endif
	return No_exp;
}
static attr_value_t get_attr_endian(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	c6713_flash_device_t* dev = (c6713_flash_device_t*)conf_obj->obj;
	return SKY_make_attr_uinteger(dev->endian);
}

static exception_t set_attr_endian(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	c6713_flash_device_t* dev = (c6713_flash_device_t*)conf_obj->obj;
	dev->endian = SKY_attr_uinteger(*value);
	return No_exp;
}

void c6713_flash_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "endian", get_attr_endian, NULL, set_attr_endian, NULL, SKY_Attr_Optional, "uinteger", "endian of c6713_flash");
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	c6713_flash_device_t* dev = (c6713_flash_device_t*)obj->obj;
	dev->image = obj2;
	dev->image_space = (memory_space_intf*)SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);
	return No_exp;
}
 
static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	c6713_flash_device_t* dev = (c6713_flash_device_t*)obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}
void init_c6713_flash(){
	static skyeye_class_t class_data = {
		.class_name = "c6713_flash",
		.class_desc = "c6713_flash device",
		.new_instance = new_c6713_flash,
		.free_instance = free_c6713_flash,
		.reset_instance = reset_c6713_flash,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_c6713),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = c6713_flash_read,
		.write = c6713_flash_write,
		.get_page = c6713_flash_get_page,
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

	c6713_flash_register_attribute(clss);
}

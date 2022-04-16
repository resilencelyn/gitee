/* Copyright (C)
 * 2013 - David Yu keweihk@gmail.com
 * This program16 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program16; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 *
 */
/**
 * @file ram16.c
 * @brief The implementation of ram16
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

#include "ram16.h"

static exception_t ram16_read_word_hl(conf_object_t *opaque, generic_address_t offset, void* buf, int hl_flag)
{
	printf("In %s, Not implement this function.\n", __func__);
}

static exception_t ram16_read_word(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint32_t data;
	exception_t ret;
	ram16_device_t* dev = opaque->obj;

	if (dev->image_space)
		ret = dev->image_space->read(dev->image, offset * 2, &data, 4);
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

static exception_t ram16_read_halfword_hl(conf_object_t *opaque, generic_address_t offset, void* buf, int hl_flag)
{
	printf("In %s, Not implement this function.\n", __func__);
}

static exception_t ram16_read_halfword(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint16_t data;
	exception_t ret;
	ram16_device_t* dev = opaque->obj;

	if (dev->image_space)
		ret = dev->image_space->read(dev->image, offset * 2, &data, 2);
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

static exception_t ram16_read_byte_hl(conf_object_t *opaque, generic_address_t offset, void* buf, int hl_flag)
{
	ram16_device_t* dev = opaque->obj;
	uint8_t data;
	exception_t ret;

	if (hl_flag)
	{
		if (dev->image_space)
			ret = dev->image_space->read(dev->image, offset * 2 + 1, &data, 1);
	} else
	{
		if (dev->image_space)
			ret = dev->image_space->read(dev->image, offset * 2, &data, 1);
	}
	if(ret != No_exp){
		printf("In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
		return Not_found_exp;
	}
	//generic_arch_t* arch_instance = get_arch_instance(NULL);
	*(uint8_t*)buf = data;

	return No_exp;
}

static exception_t ram16_read_byte(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint8_t data;
	exception_t ret;
	ram16_device_t* dev = opaque->obj;

	if (dev->image_space)
		ret = dev->image_space->read(dev->image, offset * 2, &data, 1);
	if(ret != No_exp){
		printf("In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
		return Not_found_exp;
	}
	//generic_arch_t* arch_instance = get_arch_instance(NULL);
	*(uint8_t*)buf = data;

	return No_exp;
}

static exception_t ram16_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	ram16_device_t *dev = opaque->obj;
	switch(count){
		case 4:
			return  ram16_read_word(opaque, offset,  buf);
		case 2:
			return  ram16_read_halfword(opaque, offset,  buf);
		case 1:
			return  ram16_read_byte(opaque, offset,  buf);
		default:
			printf("In %s, count %d is error!\n", __func__, count);
			break;
	}

	return No_exp;
}

static exception_t ram16_read_hl(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count, int hl_flag)
{
	ram16_device_t *dev = opaque->obj;
	switch(count){
		case 4:
			return  ram16_read_word_hl(opaque, offset, buf, hl_flag);
		case 2:
			return  ram16_read_halfword_hl(opaque, offset, buf, hl_flag);
		case 1:
			return  ram16_read_byte_hl(opaque, offset, buf, hl_flag);
		default:
			printf("In %s, count %d is error!\n", __func__, count);
			break;
	}

	return No_exp;
}

static exception_t ram16_write_word(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint32_t data;
	exception_t ret;
	ram16_device_t* dev = opaque->obj;

	/* judge the endianess */
	if(dev->endian == Big_endian)
		data = word_from_BE(*(uint32_t*)buf);
	else
		data = *(uint32_t*)buf;

	if (dev->image_space)
		ret = dev->image_space->write(dev->image, offset * 2, &data, 4);
	if(ret != No_exp){
		printf("In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t ram16_write_word_hl(conf_object_t *opaque, generic_address_t offset, void* buf, int hl_flag)
{
	printf("In %s, Not implement this function.\n", __func__);
}

static exception_t ram16_write_halfword_hl(conf_object_t *opaque, generic_address_t offset, void* buf, int hl_flag)
{
	printf("In %s, Not implement this function.\n", __func__);
}

static exception_t ram16_write_halfword(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint16_t data;
	exception_t ret;
	ram16_device_t* dev = opaque->obj;

	/* judge the endianess */
	if(dev->endian == Big_endian)
		data = half_from_BE(*(uint16_t*)buf);
	else
		data = *(uint16_t*)buf;

	if (dev->image_space)
		ret = dev->image_space->write(dev->image, offset * 2, &data, 2);

	if(ret != No_exp){
		printf("In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t ram16_write_byte(conf_object_t *opaque, generic_address_t offset, void* buf)
{
	uint8_t data;
	exception_t ret;
	ram16_device_t* dev = opaque->obj;

	/* judge the endianess */
	data = *(uint8_t*)buf;

	if (dev->image_space)
		ret = dev->image_space->write(dev->image, offset * 2, &data, 1);
	if(ret != No_exp){
		printf("In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t ram16_write_byte_hl(conf_object_t *opaque, generic_address_t offset, void* buf, int hl_flag)
{
	ram16_device_t* dev = opaque->obj;
	uint8_t data;
	data = *(uint8_t*)buf;
	exception_t ret;

	if (hl_flag)
	{
		if (dev->image_space)
			ret = dev->image_space->write(dev->image, offset * 2 + 1, &data, 1);
	} else
	{
		if (dev->image_space)
			ret = dev->image_space->write(dev->image, offset * 2, &data, 1);
	}
	if(ret != No_exp){
		printf("In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t ram16_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	switch(count){
		case 4:
			return  ram16_write_word(opaque, offset,  buf);
		case 2:
			return  ram16_write_halfword(opaque, offset,  buf);
		case 1:
			return  ram16_write_byte(opaque, offset,  buf);
		default:
			printf("In %s, count %d is error!\n", __func__, count);
	}
	return No_exp;
}

static exception_t ram16_write_hl(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count, int hl_flag)
{
	switch(count){
		case 4:
			return  ram16_write_word_hl(opaque, offset, buf, hl_flag);
		case 2:
			return  ram16_write_halfword_hl(opaque, offset, buf, hl_flag);
		case 1:
			return  ram16_write_byte_hl(opaque, offset, buf, hl_flag);
		default:
			printf("In %s, count %d is error!\n", __func__, count);
	}
	return No_exp;
}

static get_page_t * ram16_get_page(conf_object_t *obj, physical_address_t offset){
	ram16_device_t *dev = obj->obj;
	if(dev->image_space){
		return dev->image_space->get_page(dev->image, offset);
	}
	return (get_page_t *)NULL;
}

static char* mbi_ram16_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t mbi_ram16_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	ram16_device_t *dev = conf_obj->obj;
	return No_exp;
}

static uint32_t mbi_ram16_get_regnum(conf_object_t *conf_obj)
{
	return 0;
}

static uint32_t mbi_ram16_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	ram16_device_t *dev = conf_obj->obj;
	uint32_t* regs_value = 0;
	return *regs_value;
}

static uint32_t mbi_ram16_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	return 0;
}

static conf_object_t* new_ram16(char* obj_name)
{
	ram16_device_t* dev= skyeye_mm_zero(sizeof(ram16_device_t));
	dev->obj = new_conf_object(obj_name, dev);
	return dev->obj;
}

static void free_ram16(conf_object_t* conf_obj){
	ram16_device_t* dev = conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev);
}

static exception_t reset_ram16(conf_object_t* opaque, const char* args)
{
	ram16_device_t *dev = opaque->obj;
	char result[64];
	int index;
	attr_value_t value;
#if 0
	if(args == NULL){
		//default operations
		return No_exp;
	}else{
		//parse the param16eter
		for(index = 0; index < 3; index++){
			if(!strncmp(args, ram16_attr[index], strlen(ram16_attr[index])))
				break;
		}
	}
#endif
	return No_exp;
}

static attr_value_t get_attr_endian(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	ram16_device_t* dev = (ram16_device_t*)conf_obj->obj;
	return SKY_make_attr_uinteger(dev->endian);
}

static exception_t set_attr_endian(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	ram16_device_t* dev = (ram16_device_t*)conf_obj->obj;
	dev->endian = SKY_attr_uinteger(*value);
	return No_exp;
}

static exception_t image_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	ram16_device_t* dev = (ram16_device_t*)obj->obj;
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
	ram16_device_t* dev = (ram16_device_t*)obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}


void ram16_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "endian", get_attr_endian, NULL, set_attr_endian, NULL, SKY_Attr_Optional, "uinteger", "endian of ram16");
}

void init_ram16(){
	static skyeye_class_t class_data = {
		.class_name = "ram16",
		.class_desc = "ram16 device",
		.new_instance = new_ram16,
		.free_instance = free_ram16,
		.reset_instance = reset_ram16,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_f28335),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = ram16_read,
		.write = ram16_write,
		.get_page = ram16_get_page,
		.read_hl = ram16_read_hl,
		.write_hl = ram16_write_hl
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);	

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = mbi_ram16_get_regvalue_by_id,
		.get_regname_by_id  = mbi_ram16_get_regname_by_id,
		.set_regvalue_by_id = mbi_ram16_set_regvalue_by_id,
		.get_regnum         = mbi_ram16_get_regnum,
		.get_regid_by_name  = mbi_ram16_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		[0] = (struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME, 
			.iface = &io_memory,
		},
	};
	static const struct ConnectDescription connects[] = {
		[0] = (struct ConnectDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.set = image_set,
			.get = image_get,
		}
	};
	ram16_register_attribute(clss);
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

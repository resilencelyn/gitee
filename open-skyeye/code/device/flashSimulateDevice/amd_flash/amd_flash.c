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
* @file amd_flash.c
* @brief The implementation of system controller
* @author Kewei Yu : keweihk@gmail.com
* @version 78.77
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_interface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>

#include "amd_flash.h"

static char* amd_flash_attr[] = {"image"};
static exception_t amd_flash_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
        struct amd_flash_device *dev = opaque->obj;
	struct flash_am29_io* flash_io = &(dev->flash_io);
        int index;
        //parse the parameter
        for(index = 0; index < sizeof(amd_flash_attr)/sizeof(amd_flash_attr[0]);
									 index++){
                if(!strncmp(attr_name, amd_flash_attr[index], strlen(amd_flash_attr[index])))
                        break;
        }
        switch(index){
                case 0:
                        flash_io->image = (memory_space_intf*)SKY_get_interface(value->u.object, MEMORY_SPACE_INTF_NAME);
                        break;
                default:
                        printf("parameter error\n");
                        return Invarg_exp;
        }
        return No_exp;
}

static exception_t amd_flash_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct amd_flash_device *dev = opaque->obj;
	struct flash_am29_io* flash_io = &(dev->flash_io);
	switch(count){
		case 4:
			return  flash_io->flash_intf.read_word(flash_io, offset,  buf);
		case 2:
			return  flash_io->flash_intf.read_halfword(flash_io, offset,  buf);
		case 1:
			return  flash_io->flash_intf.read_byte(flash_io, offset,  buf);
		default:
			printf("In %s, count %d is error!\n", __func__, count);
			break;
	}
	return No_exp;
}

static exception_t amd_flash_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct amd_flash_device *dev = opaque->obj;
	struct flash_am29_io* flash_io = &(dev->flash_io);
	switch(count){
		case 4:
			return  flash_io->flash_intf.write_word(flash_io, offset,  *buf);
		case 2:
			return  flash_io->flash_intf.write_halfword(flash_io, offset,  *buf);
		case 1:
			return  flash_io->flash_intf.write_byte(flash_io, offset,  *buf);
		default:
			printf("In %s, count %d is error!\n", __func__, count);
			break;
	}

	return No_exp;

}

static void flash_am29lv640_bautoselect(struct flash_am29_io *io, uint32_t offset, uint8_t *data)
{
	switch (offset & 0xff) {
		case 0: *data = 0x1; break;
		case 2: *data = (strncmp(io->type, "Am29LV640", 10) == 0 ? 0xda : 0x5b); break;
//		case 4: *data = 0x1; break; /* write-protected */

		default: *data = 0x0; break;
	}
}


static void flash_am29lv640_wautoselect(struct flash_am29_io *io, uint32_t offset, uint16_t *data)
{
	switch (offset & 0xff) {
		case 0: *data = 0x1; break;
		case 1: *data = (strncmp(io->type, "Am29LV640", 10) == 0 ? 0x22da : 0x225b); break;
//		case 2: *data = 0x1; break; /* write-protected */

		default: *data = 0x0; break;
	}
}
static conf_object_t* new_amd_flash(char* obj_name){
	amd_flash_device* dev = skyeye_mm_zero(sizeof(amd_flash_device));
	dev->obj = new_conf_object(obj_name, dev);

	exception_t ret = flash_am29_setup(&dev->flash_io, NULL, 0,
			flash_am29lv640_bautoselect, flash_am29lv640_wautoselect,
			0x20000000, 0x8000);

	return dev->obj;
}

void free_amd_flash(conf_object_t* obj){
	amd_flash_device* dev = obj->obj;
	conf_object_t* conf_image = dev->flash_io.image->conf_obj;
	if(conf_image == NULL)
		return;
	conf_object_t* class_obj = get_conf_obj(conf_image->class_name);
	skyeye_class_t* class_data = class_obj->obj;
	/* free ram's attrabute : image */
	if(class_data->free_instance)
		class_data->free_instance(conf_image);
	skyeye_free(dev->obj);
	skyeye_free(dev);
	
}

void init_amd_flash(){
	static skyeye_class_t class_data = {
		.class_name = "amd_flash",
		.class_desc = "amd_flash",
		.new_instance = new_amd_flash,
		.free_instance = free_amd_flash,
		.get_attr = NULL,
		.set_attr = amd_flash_set_attr
	};

	conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);

	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = amd_flash_read,
		.write = amd_flash_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		}
 	};
	class_register_ifaces(clss, ifaces);

}

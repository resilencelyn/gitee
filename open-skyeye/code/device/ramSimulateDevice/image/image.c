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
 */
/**
 * @file image.c
 * @brief The implementation of system controller
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
#include <skyeye_dev_intf.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_ram.h>
#include <skyeye_device.h>
#include <stdlib.h>
#define DEBUG
#include <skyeye_log.h>

#include "image.h"

static exception_t alloc_mem_dyncom(conf_object_t* opaque, uint32_t index)
{
	image_module_t *dev = opaque->obj;
	dev->image_ptr[index] = skyeye_mm_align(0x1000, BANK_SIZE);
	if(dev->image_ptr[index] == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Malloc memory error!\n");
		return Malloc_exp;
	}
	dev->index_array[dev->index_count] = index;
	dev->index_count++;
	return No_exp;
}

static exception_t image_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	image_module_t *dev = opaque->obj;
	if(offset > dev->size){
		skyeye_log(Error_log, __FUNCTION__,"image is not enough!\n");
		return Not_found_exp;
	}
	while(count > 0){
		int index = offset / BANK_SIZE;
		int bank_offset = offset % BANK_SIZE;
		if(dev->image_ptr[index]){
			*(char*)buf++ = (dev->image_ptr)[index][bank_offset];
			count--; offset++;
		}
		else{
			if (Malloc_exp == alloc_mem_dyncom(opaque, index))
			{
				return Malloc_exp;
			}
		}
	}

	return No_exp;
}

static exception_t image_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	image_module_t *dev = opaque->obj;
	if(offset > dev->size){
		skyeye_log(Error_log, __FUNCTION__, "image is not enough!\n");
		return Not_found_exp;
	}
	while(count > 0){
		int index = offset / BANK_SIZE;
		int bank_offset = offset % BANK_SIZE;
		if(dev->image_ptr[index]){
			dev->image_ptr[index][bank_offset] = *(char*)buf++;
			count--; offset++;
		}
		else{
			if (Malloc_exp == alloc_mem_dyncom(opaque, index))
			{
				return Malloc_exp;
			}
		}
	}

	return No_exp;
}

static get_page_t *image_get_page(conf_object_t *obj, physical_address_t offset){
	image_module_t *dev = obj->obj;
	if(offset > dev->size){
		skyeye_log(Error_log, __FUNCTION__,"image is not enough!\n");
		return (get_page_t *)NULL;
	}
	int index = offset / BANK_SIZE;
	int bank_offset = offset % BANK_SIZE;
        if(dev->image_ptr[index] == NULL){
		if(alloc_mem_dyncom(obj, index) != No_exp)
			return (get_page_t *)NULL;
	}
	dev->page.page_base =  &(dev->image_ptr[index][bank_offset & 0xFFFFF000]);
	return  &(dev->page); 
}


static uint32_t image_get_page_size(conf_object_t *obj)
{
	image_module_t* dev = obj->obj;
	return BANK_SIZE;

}

static uint32_t image_get_page_count(conf_object_t *obj)
{
	image_module_t* dev = obj->obj;
	return dev->index_count;

}

static int32_t image_get_page_index_by_id(conf_object_t *obj, uint32_t id)
{
	image_module_t* dev = obj->obj;
	if(id >= dev->index_count){
		skyeye_log(Error_log, __FUNCTION__, "id is larger than the max count");
		return -1;
	}
	return dev->index_array[id];
}

static char * image_get_page_content(conf_object_t *obj, uint32_t index)
{
	image_module_t* dev = obj->obj;
	
	return dev->image_ptr[index];
}

static exception_t  image_set_page_content(conf_object_t *obj, void *buf, uint32_t index)
{
	image_module_t* dev = obj->obj;
	alloc_mem_dyncom(obj, index);
	memmove(dev->image_ptr[index], buf, BANK_SIZE);
	return No_exp;
}

static exception_t  image_clear_all_pages(conf_object_t *obj)
{
	image_module_t* dev = obj->obj;
	int i, index;
	
	for(i = 0; i < dev->index_count; i++){
		index = dev->index_array[i];
		skyeye_align_free(dev->image_ptr[index]);
		dev->image_ptr[index] = NULL;
	}
	
	dev->index_count = 0;
	return No_exp;
}
static void image_size_set(conf_object_t *obj, uint32_t size)
{
	image_module_t* dev = obj->obj;
	dev->size = size;
}




static conf_object_t* new_image(char* obj_name)
{
	image_module_t* dev = skyeye_mm_zero(sizeof(image_module_t));
	uint8_t** image_ptr = skyeye_mm_zero(sizeof(uint8_t*) * 256);
	dev->image_ptr = image_ptr;
	dev->obj = new_conf_object(obj_name, dev);
	dev->index_count = 0;
	return dev->obj;
}

static void free_image(conf_object_t* conf_obj){
	image_module_t* dev = conf_obj->obj;
	int i, index;
	for(i = 0; i < dev->index_count; i++){
		index = dev->index_array[i];
		skyeye_align_free(dev->image_ptr[index]);
	}
	skyeye_free(dev->image_ptr);
	skyeye_free(dev);
	skyeye_free(conf_obj);
}

static exception_t reset_image(conf_object_t* opaque, const char* args)
{
	image_module_t *dev = opaque->obj;
	int i, index;
	for(i = 0; i < dev->index_count; i++){
		index = dev->index_array[i];
		skyeye_free(dev->image_ptr[index]);
		dev->image_ptr[index] = NULL;
	}
	dev->index_count = 0;
	return No_exp;
}


static void dbg_read(char *buf, uint32_t offset, uint32_t size)
{
	uint32_t off, i;
	uint32_t *val;

	off = offset;
	val = (uint32_t *)buf;

	for (i = 0; i < size / 4; i++) {
		printf("ADDR: 0x%08x\t%08x\n", off, val[i]);
		off += 4;
	}
}
static int load_image(conf_object_t *obj)
{
	FILE *pfile;
	char buf[8 * 1024];
	uint32_t offset, ret, filesize;
	image_module_t *dev = (image_module_t *)obj->obj;

	if ((pfile = fopen(dev->load_file, "rb")) == NULL) {
		printf("[%s:%s:%d]: fopen faild.\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}

	fseek(pfile, 0l, SEEK_END);
	filesize = ftell(pfile);
	fseek(pfile, 0l, SEEK_SET);

	dev->size = filesize;

	offset = 0;

	while ((ret = fread(buf, 1, sizeof(buf), pfile))) {
		image_write(obj, offset, buf, ret);
		//dbg_read(buf, offset, ret);
		offset += ret;
	}

	if (!feof(pfile)) {
		printf("[%s:%s:%d]: fread faild.\n", __FILE__, __FUNCTION__, __LINE__);
		fclose(pfile);
		return -3;
	}

	fclose(pfile);
	return 0;
}
static attr_value_t get_attr_save_path(void *arg, conf_object_t *conf_obj, attr_value_t *idx)
{
	image_module_t *dev = conf_obj->obj;
	return SKY_make_attr_string(dev->save_path);
}
static exception_t set_attr_save_path(void *arg, conf_object_t *conf_obj, attr_value_t *value, attr_value_t *idx)
{
	image_module_t *dev = conf_obj->obj;
	strncpy(dev->save_path, SKY_attr_string(*value), sizeof(dev->save_path));
	return No_exp;
}
static attr_value_t get_attr_load_file(void *arg, conf_object_t *conf_obj, attr_value_t *idx)
{
	image_module_t *dev = conf_obj->obj;
	return SKY_make_attr_string(dev->load_file);
}
static exception_t set_attr_load_file(void *arg, conf_object_t *conf_obj, attr_value_t *value, attr_value_t *idx)
{
	image_module_t *dev = conf_obj->obj;

	if (strlen(SKY_attr_string(*value))) {
		strncpy(dev->load_file, SKY_attr_string(*value), sizeof(dev->load_file));

		load_image(conf_obj);
	}
	return No_exp;
}
DEF_UINTEGER_ATTR_FUNC(image_module_t, size); 
static void image_register_attribute(conf_class_t* clss){
	DEF_ATTR(size, uinteger, "The size of the storage memory");
	SKY_register_attribute(clss, "save_path",
			get_attr_save_path, NULL,
			set_attr_save_path, NULL,
			SKY_Attr_Optional, "string",
			"image save path.");
	SKY_register_attribute(clss, "load_file",
			get_attr_load_file, NULL,
			set_attr_load_file, NULL,
			SKY_Attr_Optional, "string",
			"load file to image.");
}


void init_image(){
	static skyeye_class_t class_data = {
		.class_name = "image",
		.class_desc = "image module",
		.new_instance = new_image,
		.free_instance = free_image,
		.reset_instance = reset_image,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = image_read,
		.write = image_write,
		.get_page = image_get_page,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_image_info info = {
		.get_page_size = image_get_page_size,
		.get_page_count = image_get_page_count,
		.get_page_index_by_id = image_get_page_index_by_id,
		.get_page_content = image_get_page_content,
		.set_page_content = image_set_page_content,
		.clear_all_pages = image_clear_all_pages,
		.set_image_size = image_size_set,
	};
	SKY_register_iface(clss, SKYEYE_IMAGE_INFO, &info);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_IMAGE_INFO,
			.iface = &info,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, CONNECTS_NULL);
	image_register_attribute(clss);
}

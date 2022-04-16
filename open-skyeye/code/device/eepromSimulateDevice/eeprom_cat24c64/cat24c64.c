/* Copyright (C) 
* 2014 - Michael.Kang blackfin.kang@gmail.com
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
* @file cat24c64.c
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-12-05
*/

#include <stdio.h>
#include <stdlib.h>
#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_exec.h>
#include <skyeye_cell.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_log.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_dev_intf.h>
#include "skyeye_thread.h"
#include "cat24c64.h"

#ifndef __MINGW32__
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

static uint8_t image_read(conf_object_t *opaque, uint32_t offset);

static uint8_t image_write(conf_object_t *opaque, uint32_t offset, uint8_t value);

static int cat24c64_i2c_set_state(conf_object_t* opaque, i2c_device_state_t state, uint8_t address)
{
	cat24c64_dev *dev=  (cat24c64_dev *)opaque->obj;
	dev->op_mod = state;
	dev->addr_count = 2;
	return 0;
}

static uint8_t cat24c64_i2c_read_data(conf_object_t *opaque){
	cat24c64_dev *dev=  (cat24c64_dev *)opaque->obj;
	uint8_t ch;
	ch = image_read(opaque, dev->r_offset);
	dev->r_offset++;
	return ch;
}

static void  cat24c64_i2c_write_data(conf_object_t *opaque, uint8_t value){
	cat24c64_dev *dev=  (cat24c64_dev *)opaque->obj;
	uint32_t temp_offset = 0;
	if(dev->op_mod == -1)                                     /*i2c bus idle*/
		return;
	else{
		if(dev->addr_count == 0 && dev->op_mod == 0)
		{    /*slave receive mod*/
			image_write(opaque, dev->w_offset++, value);
		}else
		{
			(dev->addr_count)--;
			if(dev->addr_count == 1)
			{
				dev->addr_h = value;
			}else if(dev->addr_count == 0)
			{
				dev->addr_l = value;
				temp_offset |= (dev->addr_h << 8) & 0xff00;
				temp_offset |= (dev->addr_l & 0xff);
				dev->w_offset = temp_offset;
				dev->r_offset = dev->w_offset;
			}
		}
	}
	return;
}

static uint8_t cat24c64_i2c_get_address(conf_object_t *opaque)
{
	cat24c64_dev *dev=  (cat24c64_dev *)opaque->obj;
	return dev->address;
}

static exception_t eeprom_cat24c64_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	cat24c64_dev *dev=  (cat24c64_dev *)opaque->obj;
	return No_exp;
}

static int8_t *get_eeprom_addr(cat24c64_dev *dev)
{
	char *dev_name = dev->obj->objname;
#ifndef __MINGW32__
	int32_t fd, character_count, ret;
	char *enve = NULL, *map;
	char imagepath[256], dirpath[256];
	struct stat sb;
	memset(imagepath, '\0', 256);
	if(!enve)
	{
		enve = getenv("HOME");
		if(enve == NULL)
			return NULL;
		character_count = strlen(enve) + strlen("/") + strlen(".skyeye") + strlen("/") + strlen(dev_name) + strlen(".image") + 1;
		if(character_count > 256)
			return NULL;
		sprintf(imagepath, "%s/.skyeye/%s.image", enve, dev_name);
		sprintf(dirpath, "%s/.skyeye", enve, dev_name);
		ret = access(dirpath, 0);
		if(ret < 0)
		{
			ret = mkdir(dirpath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		}
		fd = open(imagepath, O_RDWR | O_CREAT, 0755);
		if(fd < 0)
			return NULL;
		ret = truncate(imagepath, dev->capacity);
		fstat(fd, &sb);
		dev->mapped_size = sb.st_size;

		if(ret < 0)
			return NULL;
		if(ret < 0)
			return NULL;
		map = (char *)mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if(map == (void *)-1)
			return NULL;
		close(fd);
		return  map;
	}
#else
	char imagename[256];
	memset(imagename, '\0', 256);
	sprintf(imagename, "%s_cat24c64.image", dev_name);
    HANDLE file = CreateFile(imagename, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(file == INVALID_HANDLE_VALUE)
		return NULL;
	SetFilePointer(file, dev->capacity, NULL, FILE_BEGIN);
	SetEndOfFile(file);
	HANDLE fileMappingObject = CreateFileMapping(file, NULL, PAGE_READWRITE, 0, 0, NULL);
	if(fileMappingObject == NULL)
		return NULL;
 	char * mappedFileAddress = MapViewOfFile(fileMappingObject, FILE_MAP_ALL_ACCESS, 0, 0, 200);
	if(mappedFileAddress == NULL)
		return NULL;
	CloseHandle(file);
	return mappedFileAddress;

#endif
	return NULL;
}

static uint8_t image_read(conf_object_t *opaque, uint32_t offset)
{
	cat24c64_dev *dev=  (cat24c64_dev *)opaque->obj;
	return *(dev->mapped_addr + offset);
}

static uint8_t image_write(conf_object_t *opaque, uint32_t offset, uint8_t value)
{
	int ret;
	cat24c64_dev *dev=  (cat24c64_dev *)opaque->obj;
	*(dev->mapped_addr + offset) = value;
	return 1;
}

static conf_object_t* new_eeprom_cat24c64(char* obj_name)
{
	cat24c64_dev* dev = skyeye_mm_zero(sizeof(cat24c64_dev));
	dev->obj = new_conf_object(obj_name, dev);
	dev->capacity = CAPACITY;
#if 0
	i2c_device_intf *i2c_dev_iface = skyeye_mm_zero(sizeof(i2c_device_intf));
	i2c_dev_iface->set_state = cat24c64_i2c_set_state;
	i2c_dev_iface->write_data = cat24c64_i2c_write_data;
	i2c_dev_iface->read_data = cat24c64_i2c_read_data;
	i2c_dev_iface->get_address = cat24c64_i2c_get_address;
	SKY_register_interface((void*)i2c_dev_iface, obj_name, I2C_DEVICE_INTF_NAME);
#endif
	dev->mapped_addr= get_eeprom_addr(dev);
	dev->w_offset = 0;
	dev->r_offset = 0;
	dev->address = 0x50;

	return dev->obj;
}

static exception_t reset_eeprom_cat24c64(conf_object_t* opaque, const char* args)
{
	cat24c64_dev *dev = opaque->obj;
	return No_exp;
}

static exception_t free_eeprom_cat24c64(conf_object_t* conf_obj)
{
	cat24c64_dev *dev = conf_obj->obj;
#ifndef __MINGW32__
	munmap((void *)dev->mapped_addr, dev->mapped_size);
#else
	UnmapViewOfFile((void *)dev->mapped_addr);
#endif

	return No_exp;
}

static void eeprom_cat24c64_register_interface(conf_class_t* class)
{
	static const i2c_device_intf i2c_dev_iface = {
		.set_state = cat24c64_i2c_set_state,
		.write_data = cat24c64_i2c_write_data,
		.read_data = cat24c64_i2c_read_data,
		.get_address = cat24c64_i2c_get_address,
	};
	SKY_register_iface(class, I2C_DEVICE_INTF_NAME, &i2c_dev_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = I2C_DEVICE_INTF_NAME,
			.iface = &i2c_dev_iface,
		}
 	};

	class_register_ifaces(class, ifaces);
	return;
}

static exception_t i2c_bus_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	cat24c64_dev *dev = obj->obj;
	*obj2 = dev->i2c_bus_obj;
	*port = NULL;
	return No_exp;
}

static exception_t i2c_bus_set(conf_object_t* obj, conf_object_t* obj2,const char *port, int index){
	cat24c64_dev *dev = obj->obj;
	dev->i2c_bus_obj = obj2;
	dev->i2c_bus_iface = (i2c_bus_intf *)SKY_get_iface(dev->i2c_bus_obj, I2C_BUS_INTF_NAME);
	if (dev->i2c_bus_iface == NULL)
	{
		if (dev->i2c_bus_obj == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Get 'i2c_bus_iface' interface from NULL fail.");
		else
			skyeye_log(Error_log, __FUNCTION__, "Get 'i2c_bus_iface' interface from %s fail.", obj2->objname);
	}
/*
	if (Error == dev->i2c_bus_iface->register_device(obj2, obj, dev->dev_id)){
		printf("In %s, Register c6k_bu61580_dev device Error.\n", __FUNCTION__);
	}
	*/

	return No_exp;
}

static void eeprom_cat24c64_connects_interface(conf_class_t* class)
{
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = I2C_BUS_INTF_NAME,
			.set = i2c_bus_set,
			.get = i2c_bus_get,
		}
	};
	class_register_connects(class, connects);

	return;
}

void init_eeprom_cat24c64()
{
	static skyeye_class_t class_data = {
		.class_name = "eeprom_cat24c64",
		.class_desc = "eeprom_cat24c64",
		.new_instance = new_eeprom_cat24c64,
		.reset_instance = reset_eeprom_cat24c64,
		.free_instance = free_eeprom_cat24c64,
		.get_attr = NULL,
		.set_attr = eeprom_cat24c64_set_attr,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	eeprom_cat24c64_register_interface(class);
	eeprom_cat24c64_connects_interface(class);
}

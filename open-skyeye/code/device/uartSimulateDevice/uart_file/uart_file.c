/*
 * Copyright (C)
 * 2011 - Michael.Kang blackfin.kang@gmail.com
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

/*
 * @file uart_file.c
 * @brief The implementation of skyeye uart term
 * @author David.Yu keweihk@gmail.com
 * @version 78.77
 * @date 2012-2-2
 */
/*
 * © Copyright 2013 eJim Lee. All Rights Reserved.
 * File:    uart_file.c
 * Email:   lyjforjob@gmail.com
 * Date:    2013-12-01
 * info:    change xterm and cmd to putty with telnet protocol
 */


#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_uart_ops.h>
#include "skyeye_thread.h"
#include "skyeye_dev_intf.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>
#include "uart_file.h"

#ifndef __MINGW32__
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <windows.h>
#endif



static void create_file(uart_file_device* dev){
	dev->fp = fopen(dev->filename, "wb+");
	if(dev->fp == NULL){
		skyeye_log(Error_log, __FUNCTION__, "can not open file: %s\n", dev->filename);
		return NULL;
	}
	return;
}


static exception_t uart_file_read(conf_object_t *opaque, void* buf, size_t count)
{
	uart_file_device *dev = opaque->obj;
	return No_exp;
}

static exception_t uart_file_write(conf_object_t *opaque, void* buf, size_t count)
{
	uart_file_device *dev = opaque->obj;
	if(dev->fp != NULL){
		fwrite(buf, 1, count, dev->fp);
		fflush(dev->fp);
	}
	else
		skyeye_log(Error_log, __FUNCTION__, "No file point is opend!\n");
	return No_exp;
}

static attr_value_t get_attr_filename(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	uart_file_device* dev = conf_obj->obj;
	attr_value_t value = SKY_make_attr_string(dev->filename);
	return value;
}
static exception_t set_attr_filename(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	uart_file_device* dev = conf_obj->obj;
	char *filename;
	filename = SKY_attr_string(*value);
	strncpy(dev->filename, filename, MAX_FILE_NAME_LEN);
	create_file(dev);
	return No_exp;
}

void uart_file_register_attribute(conf_class_t* clss) {

	SKY_register_attribute(clss, "filename", get_attr_filename, NULL, set_attr_filename, NULL, SKY_Attr_Optional, "string", "uart file name");

}

static void init_filename(conf_object_t *obj)
{
	uart_file_device* dev = obj->obj;
	int i;
	for (i = 0; i < MAX_FILE_NAME_LEN; i++){
		dev->filename[i] = 0;
	}
	return;
}
static conf_object_t* new_uart_file(const char* obj_name){
	uart_file_device* dev_uart = skyeye_mm_zero(sizeof(uart_file_device));
	dev_uart->obj = new_conf_object(obj_name, dev_uart);
	init_filename(dev_uart->obj);
	dev_uart->fp = NULL;
	return dev_uart->obj;
}

exception_t free_uart_file(conf_object_t* dev){
	uart_file_device* dev_uart = dev->obj;
	if(dev_uart->fp)
		fclose(dev_uart->fp);
	return No_exp;
}

void init_uart_file(){
	static skyeye_class_t class_data = {
		.class_name = "uart_file",
		.class_desc = "uart_file",
		.new_instance = new_uart_file,
		.free_instance = free_uart_file,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const uart_file_intf uart_file_iface = {
		.read = uart_file_read,
		.write = uart_file_write
	};
	SKY_register_iface(clss, UART_FILE_INTF, &uart_file_iface);
	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = UART_FILE_INTF,
			.iface = &uart_file_iface,
		}
 	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, CONNECTS_NULL);

	uart_file_register_attribute(clss);
}

/*
 * =====================================================================================
 *
 *       Filename:  skyeye_device.c
 *
 *    Description:  This file implements the LEON2 UART on-chip device.
 *
 *        Version:  1.0
 *        Created:  24/06/08 10:51:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

#include <skyeye_config.h>
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
#include <skyeye_attr.h>
#include <simics/base_types.h>
#include <skyeye_dev_intf.h>

#include "device.h"



extern int top_write(unsigned target, unsigned offset, void *buf, unsigned len);
extern int top_read(unsigned target, unsigned offset, void *buf, unsigned len);
static exception_t skyeye_device_write(conf_object_t *obj, unsigned int offset, void* buf, size_t count)
{
	skyeye_device_dev *dev = (skyeye_device_dev*)obj->obj;
	printf("%s Call, offset %d\n", __func__, offset);
	top_write(dev->target_id, offset, buf, count);
	return No_exp;
}

static exception_t skyeye_device_read(conf_object_t *obj, generic_address_t offset, void* buf, size_t count)
{

	skyeye_device_dev *dev = (skyeye_device_dev*)obj->obj;
	printf("%s Call, offset %d\n", __func__, offset);
	top_read(dev->target_id, offset, buf, count);
	return No_exp;
}


static conf_object_t* new_skyeye_device(const char* obj_name)
{
	skyeye_device_dev* skyeye_device = (skyeye_device_dev*)skyeye_mm_zero(sizeof(skyeye_device_dev));
	skyeye_device->obj = new_conf_object(obj_name, skyeye_device);
	skyeye_device->target_id = 0;
	return skyeye_device->obj;
}

static exception_t reset_skyeye_device(conf_object_t* obj, const char* args)
{
	skyeye_device_dev *dev = (skyeye_device_dev*)obj->obj;
	return No_exp;
}

static exception_t free_skyeye_device(conf_object_t* obj)
{
	skyeye_device_dev* dev = (skyeye_device_dev*)obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev);
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* obj, attr_value_t* idx){
	skyeye_device_dev* dev = (skyeye_device_dev*)(obj->obj);
	attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	skyeye_device_dev* dev = (skyeye_device_dev*)(obj->obj);
	dev->int_number = SKY_attr_uinteger(*value);
	return No_exp;
}



static exception_t uart_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	skyeye_device_dev* dev = (skyeye_device_dev*)(obj->obj);
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);
	if(dev->signal_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from %s fail.", obj2->objname);
	}

	return No_exp;
}
 
static exception_t uart_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	skyeye_device_dev* dev = (skyeye_device_dev*)(obj->obj);
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}


static void register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "irq_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of leon2 uart");
	return;
}



void init_skyeye_device(){
	static skyeye_class_t class_data = {
		class_name : "skyeye_device",
		class_desc : "skyeye_device",
		new_instance : new_skyeye_device,
		free_instance : free_skyeye_device,
		reset_instance : reset_skyeye_device,
		get_attr : NULL,
		set_attr : NULL,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		NULL,
		read : skyeye_device_read,
		write : skyeye_device_write,
		get_page:NULL,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			name : MEMORY_SPACE_INTF_NAME, 
			iface : &io_memory,
		},
 	};

	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			name : GENERAL_SIGNAL_INTF_NAME,
			set : uart_signal_set,
			get : uart_signal_get,
		}

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	register_attribute(clss);
	return;
}

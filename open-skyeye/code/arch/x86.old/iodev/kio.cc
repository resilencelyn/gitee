/* Copyright (C) 
* 2015 - Michael.Kang blackfin.kang@gmail.com
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
* @file kio.cc
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2015-06-18
*/

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_KIO

#include "cpu/cpu.h"
#include "kio.h"

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
#include <skyeye_core.h>
#include <skyeye_signal.h>
#define DEBUG
#include <skyeye_log.h>

bx_kio_c *theKioDevice = NULL;
#define LOG_THIS theKioDevice->
#define BX_KIO_THIS this->

#define RegBase 0x8a10

static exception_t gpio_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count);

static exception_t gpio_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count);
 

int libkio_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theKioDevice = new bx_kio_c();
  bx_devices.pluginKio = theKioDevice;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theKioDevice, BX_PLUGIN_KIO);

  return(0); // Success
}

void libkio_LTX_plugin_fini(void)
{
  delete theKioDevice;
}

struct bx_kio_s_type {
  Bit16u kio_registers[8]; /* different gpio value */
} bx_kio_s;


bx_kio_c::bx_kio_c()
{
  put("IODBG");
}

void bx_kio_c::init(void)
{
  DEV_register_ioread_handler(this, read_handler, RegBase,"BOCHS KIO", 4);
  DEV_register_iowrite_handler(this, write_handler, RegBase,"BOCHS KIO", 4);
}

Bit32u bx_kio_c::read_handler(void *this_ptr, Bit32u addr, unsigned io_len)
{
  bx_kio_c *bx_kio_ptr = (bx_kio_c *) this_ptr;
  return bx_kio_ptr->read(addr, io_len);
}

Bit32u bx_kio_c::read(Bit32u addr, unsigned io_len)
{
	Bit32u reg_value;
 	gpio_read(theKioDevice->gpio, addr - RegBase, &reg_value, io_len);
	return reg_value;

}

void bx_kio_c::write_handler(void *this_ptr, Bit32u addr, Bit32u dvalue, unsigned io_len)
{
  bx_kio_c *bx_kio_ptr = (bx_kio_c *) this_ptr;
  bx_kio_ptr->write(addr, dvalue, io_len);
}

void bx_kio_c::write(Bit32u addr, Bit32u dvalue, unsigned io_len)
{
	Bit32u reg_value = dvalue;
	gpio_write(theKioDevice->gpio, addr - RegBase, &reg_value, io_len);
	return;
}

void bx_kio_c::mem_read(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data){
	return;
}

void bx_kio_c::mem_write(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data){
}

/////////////////////////////////////// SkyEye gpio class /////////////////////////////

typedef struct pin_reg{
	uint32_t pin;
}gpio_reg_t;

typedef struct gpio{
	conf_object_t* obj;
	gpio_reg_t* regs;
	conf_object_t* signal;
	core_signal_intf* signal_iface;
	conf_object_t *gpio_connector;
	skyeye_gpio_connector_intf *connector_iface;
	int interrupt_number;
	conf_object_t *gpio_gate_a;
	skyeye_gate_a_intf *gate_a_iface;
	conf_object_t *gpio_gate_b;
	skyeye_gate_b_intf *gate_b_iface;
	uint32_t index;
}gpio_device;

static char* regs_name[] = {
	"pin",
	NULL
};

exception_t  update_gpio(conf_object_t *obj, uint32_t regvalue){
	gpio_device *dev = (gpio_device *)(obj->obj);
	gpio_reg_t* regs = dev->regs;
	regs->pin = regvalue;
	return No_exp;
}
exception_t  update_gpio_on_line(conf_object_t *obj, uint32_t regvalue){
	gpio_device *dev = (gpio_device *)(obj->obj);
	if(dev->gpio_connector && dev->connector_iface){
		dev->connector_iface->update(dev->gpio_connector, dev->regs->pin);
	}
	if(dev->gpio_gate_a && dev->gate_a_iface){
		dev->gate_a_iface->update(dev->gpio_gate_a, dev->regs->pin);
	}
	if(dev->gpio_gate_b && dev->gate_b_iface){
		dev->gate_b_iface->update(dev->gpio_gate_b, dev->regs->pin);
	}
	return No_exp;
}


static char* gpio_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t gpio_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	gpio_device *dev = (gpio_device *)(conf_obj->obj);
	gpio_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static exception_t gpio_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id){
	gpio_device *dev = (gpio_device *)(conf_obj->obj);
	gpio_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;
	update_gpio_on_line(conf_obj, regs->pin);
	return No_exp;
}
uint32_t gpio_get_regid_by_name(conf_object_t* opaque, char* name){
	uint32_t regnum = 0, i = 0;
	gpio_device *dev = (gpio_device *)opaque->obj;
	gpio_reg_t* regs = dev->regs;
	regnum = sizeof(gpio_reg_t)/ 4;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

uint32_t gpio_get_regnum(conf_object_t* opaque){
	gpio_device *dev = (gpio_device *)opaque->obj;
	gpio_reg_t* regs = dev->regs;
	return  sizeof(gpio_reg_t) / 4;
}

static exception_t gpio_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	gpio_device *dev = (gpio_device *)opaque->obj;
	gpio_reg_t* regs = dev->regs;
	switch (offset) {
		case 0:
			regs->pin = *(uint32_t *)buf;
			update_gpio_on_line(opaque, regs->pin);
			break;
		default:
			break;
	}

	return No_exp;
}

static exception_t gpio_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	gpio_device *dev = (gpio_device *)opaque->obj;
	gpio_reg_t* regs = dev->regs;
	switch (offset) {
		case 0:
			*(uint32_t *)buf = regs->pin;
			break;
		default:
			break;
	}
	return No_exp;
}

static conf_object_t* new_gpio(const char* obj_name){
	gpio_device* dev = (gpio_device *)skyeye_mm_zero(sizeof(gpio_device));
	gpio_reg_t* regs =  (gpio_reg_t*)skyeye_mm_zero(sizeof(gpio_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	dev->regs = regs;
	dev->gpio_connector = NULL;
	dev->connector_iface = NULL;
	dev->gpio_gate_a = NULL;
	dev->gate_a_iface = NULL;
	dev->gpio_gate_b = NULL;
	dev->gate_b_iface = NULL;
	theKioDevice->gpio = dev->obj;

	return dev->obj;
}
static exception_t free_gpio(conf_object_t* dev){
	return No_exp;	
}
extern "C" void init_kio_gpio();
static void gpio_register_interface(conf_class_t* clss){
	static const skyeye_gpio_connector_intf iface = {
		update :  update_gpio
	};
	SKY_register_iface(clss, SKYEYE_GPIO_CONNECTOR, &iface);

	static const memory_space_intf io_memory = {
		conf_obj : NULL,
		read : gpio_read,
		write : gpio_write,
		get_page : NULL
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		conf_obj : NULL,
		get_regvalue_by_id : gpio_get_regvalue_by_id,
		get_regname_by_id : gpio_get_regname_by_id,
		set_regvalue_by_id : gpio_set_regvalue_by_id,
		get_regid_by_name : gpio_get_regid_by_name,
		get_regnum : gpio_get_regnum
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

}

static attr_value_t get_attr_connector(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	gpio_device *dev = (gpio_device *)conf_obj->obj;
	attr_value_t gpio_connector = SKY_make_attr_object(dev->gpio_connector);
	return gpio_connector;
}
static exception_t set_attr_connector(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	gpio_device *dev = (gpio_device *)conf_obj->obj;
	dev->gpio_connector = SKY_attr_object(*value);
	dev->connector_iface = (skyeye_gpio_connector_intf *)SKY_get_iface(dev->gpio_connector, SKYEYE_GPIO_CONNECTOR);
	update_gpio_on_line(conf_obj, dev->regs->pin);
	return No_exp;
}

static exception_t connector_set(conf_object_t *conf_obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_device *dev = (gpio_device *)conf_obj->obj;
	dev->gpio_connector = obj2;
	dev->connector_iface = (skyeye_gpio_connector_intf *)SKY_get_iface(dev->gpio_connector, SKYEYE_GPIO_CONNECTOR);
	if (dev->connector_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_GPIO_CONNECTOR, obj2->objname);
	}
	update_gpio_on_line(conf_obj, dev->regs->pin);
	return No_exp;
}
 
static exception_t connector_get(conf_object_t *conf_obj, conf_object_t **obj2, char **port, int index)
{
	gpio_device *dev = (gpio_device *)conf_obj->obj;
	*obj2 = dev->gpio_connector;
	*port = NULL;
	return No_exp;
}

static exception_t gate_b_set(conf_object_t *conf_obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_device *dev = (gpio_device *)conf_obj->obj;
	dev->gpio_gate_b = obj2;
	dev->gate_b_iface = (skyeye_gate_b_intf*)SKY_get_iface(dev->gpio_gate_b, LOGIC_GATE_B);
	if (dev->gate_b_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", LOGIC_GATE_B, obj2->objname);
	}
	return No_exp;
}
 
static exception_t gate_b_get(conf_object_t *conf_obj, conf_object_t **obj2, char **port, int index)
{
	gpio_device *dev = (gpio_device *)conf_obj->obj;
	*obj2 = dev->gpio_gate_b;
	*port = NULL;
	return No_exp;
}


static exception_t gate_a_set(conf_object_t *conf_obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_device *dev = (gpio_device *)conf_obj->obj;
	dev->gpio_gate_a = obj2;
	dev->gate_a_iface = (skyeye_gate_a_intf*)SKY_get_iface(dev->gpio_gate_a, LOGIC_GATE_A);
	if (dev->gate_a_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", LOGIC_GATE_A, obj2->objname);
	}
	return No_exp;
}
 
static exception_t gate_a_get(conf_object_t *conf_obj, conf_object_t **obj2, char **port, int index)
{
	gpio_device *dev = (gpio_device *)conf_obj->obj;
	*obj2 = dev->gpio_gate_a;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_board_index(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	gpio_device *dev = (gpio_device *)conf_obj->obj;
        attr_value_t index = SKY_make_attr_uinteger(dev->index);
        return index;
}
static exception_t set_attr_board_index(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	gpio_device *dev = (gpio_device *)conf_obj->obj;
        dev->index = SKY_attr_uinteger(*value);
	theKioDevice->gpio_value[dev->index] = &(dev->regs->pin);
	theKioDevice->gpio_conf_obj[dev->index] = conf_obj;
        return No_exp;
}

static void gpio_register_attribute(conf_class_t* clss){
        SKY_register_attribute(clss, "board_index", get_attr_board_index, NULL, set_attr_board_index, NULL, SKY_Attr_Optional, "uinteger", "the size of the storage space");
}

void init_kio_gpio(){
	static skyeye_class_t class_data = {
		class_name : "kio",
		class_desc : "kio gpio",
		new_instance : new_gpio,
		free_instance : free_gpio,
		reset_instance : NULL,
		get_attr : NULL,
		set_attr : NULL,
		kind : Class_Persistent,
		interface_list : NULL,
		iface_list : {
			NULL,
			0
		},
		connect_list : {
			NULL,
			0
		},
		ifaces : NULL,
		connects : NULL
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const skyeye_gpio_connector_intf iface = {
		update :  update_gpio
	};
	SKY_register_iface(clss, SKYEYE_GPIO_CONNECTOR, &iface);

	static const memory_space_intf io_memory = {
		conf_obj : NULL,
		read : gpio_read,
		write : gpio_write,
		get_page : NULL
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		conf_obj : NULL,
		get_regvalue_by_id : gpio_get_regvalue_by_id,
		get_regname_by_id : gpio_get_regname_by_id,
		set_regvalue_by_id : gpio_set_regvalue_by_id,
		get_regid_by_name : gpio_get_regid_by_name,
		get_regnum : gpio_get_regnum
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			name : SKYEYE_GPIO_CONNECTOR,
			iface : &iface,
		},
		(struct InterfaceDescription) {
			name : MEMORY_SPACE_INTF_NAME,
			iface : &io_memory,
		},
		(struct InterfaceDescription) {
			name : SKYEYE_REG_INTF, 
			iface : &reg_intf,
		},
 	};

	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			name : LOGIC_GATE_A,
			set : gate_a_set,
			get : gate_a_get,
		},
		(struct ConnectDescription) {
			name : LOGIC_GATE_B,
			set : gate_b_set,
			get : gate_b_get,
		},
		(struct ConnectDescription) {
			name : SKYEYE_GPIO_CONNECTOR,
			set : connector_set,
			get : connector_get,
		},


	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	gpio_register_attribute(clss);
}
#endif /* if BX_SUPPORT_KIO */

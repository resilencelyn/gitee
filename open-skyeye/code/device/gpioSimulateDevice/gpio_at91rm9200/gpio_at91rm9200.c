/* Copyright (C)
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
/**
* @file gpio_at91rm9200.c
* @brief The implementation of system controller
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_attribute.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>

#include "gpio_at91rm9200.h"

static exception_t at91rm9200_gpio_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{

	struct at91rm9200_gpio_device *dev = opaque->obj;
	int i;

	i = (offset >> 9) & 0x3;
	offset = offset & 0xff;

	switch(offset) {
		case PIO_PSR:
			*(uint32_t*)buf = dev->regs[i].pio_psr;
			break;
		case PIO_OSR:
			*(uint32_t*)buf = dev->regs[i].pio_osr;
			break;
		case PIO_IFSR:
			*(uint32_t*)buf = dev->regs[i].pio_ifsr;
			break;
		case PIO_ODSR:
			*(uint32_t*)buf = dev->regs[i].pio_odsr;
			break;
		case PIO_PDSR:
			*(uint32_t*)buf = dev->regs[i].pio_pdsr;
			break;
		case PIO_IMR:
			*(uint32_t*)buf = dev->regs[i].pio_imr;
			break;
		case PIO_ISR:
			*(uint32_t*)buf = dev->regs[i].pio_isr;
			break;
		case PIO_MDSR:
			*(uint32_t*)buf = dev->regs[i].pio_mdsr;
			break;
		case PIO_PUSR:
			*(uint32_t*)buf = dev->regs[i].pio_pusr;
			break;
		case PIO_ABSR:
			*(uint32_t*)buf = dev->regs[i].pio_absr;
			break;
		case PIO_OWSR:
			*(uint32_t*)buf = dev->regs[i].pio_owsr;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__,"Can not read the register at 0x%x in gpio\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t at91rm9200_gpio_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct at91rm9200_gpio_device *dev = opaque->obj;
	uint32_t val = *(uint32_t*)buf, i;

	i = (offset >> 9) & 0x3;
	offset = offset & 0xff;

	switch(offset) {
		case PIO_PER:
			dev->regs[i].pio_per = val;
			dev->regs[i].pio_psr |= val;
			break;
		case PIO_PDR:
			dev->regs[i].pio_pdr = val;
			dev->regs[i].pio_psr &= ~val;
			break;
		case PIO_OER:
			dev->regs[i].pio_oer = val;
			dev->regs[i].pio_osr |= val;
			break;
		case PIO_ODR:
			dev->regs[i].pio_odr = val;
			dev->regs[i].pio_osr &= ~val;
			break;
		case PIO_IFER:
			dev->regs[i].pio_ifer = val;
			dev->regs[i].pio_ifsr |= val;
			break;
		case PIO_IFDR:
			dev->regs[i].pio_ifdr = val;
			dev->regs[i].pio_ifsr &= ~val;
			break;
		case PIO_SODR:
			dev->regs[i].pio_sodr = val;
			dev->regs[i].pio_odsr |= val;
			dev->regs[i].pio_pdsr |= val;
			break;
		case PIO_CODR:
			dev->regs[i].pio_codr = val;
			dev->regs[i].pio_odsr &= ~val;
			dev->regs[i].pio_pdsr &= ~val;
			break;
		case PIO_IER:
			dev->regs[i].pio_ier = val;
			dev->regs[i].pio_imr |= val;
			break;
		case PIO_IDR:
			dev->regs[i].pio_idr = val;
			dev->regs[i].pio_imr &= ~val;
			break;
		case PIO_MDER:
			dev->regs[i].pio_mder = val;
			dev->regs[i].pio_mdsr |= val;
			break;
		case PIO_MDDR:
			dev->regs[i].pio_mddr = val;
			dev->regs[i].pio_mdsr &= ~val;
			break;
		case PIO_PUDR:
			dev->regs[i].pio_pudr = val;
			dev->regs[i].pio_pusr &= ~val;
			break;
		case PIO_PUER:
			dev->regs[i].pio_puer = val;
			dev->regs[i].pio_pusr |= val;
			break;
		case PIO_ASR:
			dev->regs[i].pio_asr = val;
			dev->regs[i].pio_absr &= ~val;
			break;
		case PIO_BSR:
			dev->regs[i].pio_bsr = val;
			dev->regs[i].pio_absr |= val;
			break;
		case PIO_OWER:
			dev->regs[i].pio_ower = val;
			dev->regs[i].pio_owsr |= val;
			break;
		case PIO_OWDR:
			dev->regs[i].pio_owdr = val;
			dev->regs[i].pio_owsr &= ~val;
			break;
		case PIO_ODSR:
			dev->regs[i].pio_odsr = dev->regs[i].pio_owsr & val;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__, "Can not write the register at 0x%x in gpio\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static char* at91rm9200_gpio_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	at91rm9200_gpio_device *dev = conf_obj->obj;
	return dev->regs_name[id];
}

static uint32_t at91rm9200_gpio_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	at91rm9200_gpio_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static exception_t at91rm9200_gpio_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	at91rm9200_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t at91rm9200_gpio_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	at91rm9200_gpio_device *dev = conf_obj->obj;
	return  sizeof(dev->regs) / 4;
}

static uint32_t at91rm9200_gpio_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	at91rm9200_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = (dev->regs);
	uint32_t regnum = sizeof(dev->regs) / 4;
	int i;

	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_at91rm9200_gpio(char* obj_name){
	at91rm9200_gpio_device* dev = skyeye_mm_zero(sizeof(at91rm9200_gpio_device));
	dev->obj = new_conf_object(obj_name, dev);
	int i;

	/* init gpio regs */
	for (i = 0; i < PIONUMS; i++) {
		dev->regs[i].pio_psr = 0x0000;

		dev->regs[i].pio_osr = 0x0000;

		dev->regs[i].pio_ifsr = 0x0000;

		dev->regs[i].pio_odsr = 0x0000;

		dev->regs[i].pio_imr = 0x0000;
		dev->regs[i].pio_isr = 0x0000;

		dev->regs[i].pio_mdsr = 0x0000;

		dev->regs[i].pio_pusr = 0x0000;

		dev->regs[i].pio_absr = 0x0000;

		dev->regs[i].pio_owsr = 0x0000;
	}

	dev->regs_name = regs_name;
	return dev->obj;
}
static exception_t free_at91rm9200_gpio(conf_object_t* opaque) {
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

void at91rm9200_gpio_register_attribute(conf_class_t* clss){

}

void init_at91rm9200_gpio(){
	static skyeye_class_t class_data = {
		.class_name = "at91rm9200_gpio",
		.class_desc = "at91rm9200 gpio",
		.new_instance = new_at91rm9200_gpio,
		.free_instance = free_at91rm9200_gpio,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_at91rm9200),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = at91rm9200_gpio_read,
		.write = at91rm9200_gpio_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = at91rm9200_gpio_get_regvalue_by_id,
		.get_regname_by_id = at91rm9200_gpio_get_regname_by_id,
		.set_regvalue_by_id = at91rm9200_gpio_set_regvalue_by_id,
		.get_regnum = at91rm9200_gpio_get_regnum,
		.get_regid_by_name = at91rm9200_gpio_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		}
 	};
	static const struct ConnectDescription connects[] = {
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	at91rm9200_gpio_register_attribute(clss);
}

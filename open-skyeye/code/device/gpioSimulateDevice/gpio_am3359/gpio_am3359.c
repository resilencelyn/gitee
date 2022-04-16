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
* @file gpio_am3359.c
* @brief
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-12-04
*/

#include <stdio.h>
#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include "skyeye_thread.h"

#include "gpio_am3359.h"

static exception_t am3359_gpio_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	am3359_gpio_dev  *dev = opaque->obj;

	if (!strncmp(attr_name, "signal", strlen("signal"))) {
		dev->signal = (general_signal_intf *)SKY_get_iface(value->u.object, GENERAL_SIGNAL_INTF_NAME);
	} else if (!strncmp(attr_name, "int_number", strlen("int_number"))) {
		dev->interrupt_number  = value->u.uinteger;
	} else {
		printf("parameter error\n");
		return Invarg_exp;
	}
	return No_exp;
}

static exception_t am3359_gpio_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	am3359_gpio_dev *dev = opaque->obj;
	switch(offset)
	{
		case GPIO_REVISION:
			*(uint32_t *)buf = dev->regs.revision ;
			break;
		case GPIO_SYSCONFIG:
			*(uint32_t *)buf = dev->regs.sysconfig;
			break;
		case GPIO_EOI:
			*(uint32_t *)buf = dev->regs.eoi;
			break;
		case GPIO_IRQSTATUS_RAW_0:
			*(uint32_t *)buf = dev->regs.irqstatus_raw_0;
			break;
		case GPIO_IRQSTATUS_RAW_1:
			*(uint32_t *)buf = dev->regs.irqstatus_raw_1;
		         break;
		case GPIO_IRQSTATUS_0:
			*(uint32_t *)buf = dev->regs.irqstatus_0;
		         break;
		case GPIO_IRQSTATUS_1:
			*(uint32_t *)buf = dev->regs.irqstatus_1;
		         break;
		case GPIO_IRQSTATUS_SET_0:
			*(uint32_t *)buf = dev->regs.irqstatus_set_0;
		         break;
		case GPIO_IRQSTATUS_SET_1:
			*(uint32_t *)buf = dev->regs.irqstatus_set_1;
		         break;
		case GPIO_IRQSTATUS_CLR_0:
			*(uint32_t *)buf = dev->regs.irqstatus_clr_0;
		         break;
		case GPIO_IRQSTATUS_CLR_1:
			*(uint32_t *)buf = dev->regs.irqstatus_clr_1;
		         break;
		case GPIO_IRQWAKEN_0:
			*(uint32_t *)buf = dev->regs.irqwaken_0;
		         break;
		case GPIO_IRQWAKEN_1:
			*(uint32_t *)buf = dev->regs.irqwaken_1;
		         break;
		case GPIO_SYSSTATUS:
			*(uint32_t *)buf = dev->regs.sysstatus;
		         break;
		case GPIO_CTRL:
			*(uint32_t *)buf = dev->regs.ctrl;
		         break;
		case GPIO_OE:
			*(uint32_t *)buf = dev->regs.oe;
		         break;
		case GPIO_DATAIN:
			*(uint32_t *)buf = dev->regs.datain;
		         break;
		case GPIO_DATAOUT:
			*(uint32_t *)buf = dev->regs.dataout;
		         break;
		case GPIO_LEVELDETECT0:
			*(uint32_t *)buf = dev->regs.levedetect0;
		         break;
		case GPIO_LEVELDETECT1:
			*(uint32_t *)buf = dev->regs.levedetect1;
		         break;
		case GPIO_RISINGDETECT:
			*(uint32_t *)buf = dev->regs.risingdetect;
		         break;
		case GPIO_FALLINGDETECT:
			*(uint32_t *)buf = dev->regs.fallingdetect;
		         break;
		case GPIO_DEBOUNCENABLE:
			*(uint32_t *)buf = dev->regs.debouncenable;
		         break;
		case GPIO_DEBOUNCINGTIME:
			*(uint32_t *)buf = dev->regs.debouncingtime;
			break;
		case GPIO_CLEARDATAOUT:
			*(uint32_t *)buf = dev->regs.dataout;
			break;
		case GPIO_SETDATAOUT:
			*(uint32_t *)buf = dev->regs.dataout;
			break;
                default:
			skyeye_log(Error_log, __FUNCTION__,"Can not read the register at 0x%x in %s\n", offset, dev->obj->objname);
			break;
	}
	return  No_exp;
}

static exception_t am3359_gpio_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	am3359_gpio_dev *dev = opaque->obj;
	switch(offset)
	{
		case GPIO_REVISION:
			dev->regs.revision = *(uint32_t *)buf;
			break;
		case GPIO_SYSCONFIG:
			dev->regs.sysconfig = *(uint32_t *)buf;
			if(dev->regs.sysconfig & 0x2)
				dev->regs.sysstatus |= 0x1;
			break;
		case GPIO_EOI:
			dev->regs.eoi = *(uint32_t *)buf;
			break;
		case GPIO_IRQSTATUS_RAW_0:
			dev->regs.irqstatus_raw_0 |= *(uint32_t *)buf;
			break;
		case GPIO_IRQSTATUS_RAW_1:
			dev->regs.irqstatus_raw_1 |= *(uint32_t *)buf;
		         break;
		case GPIO_IRQSTATUS_0:
			dev->regs.irqstatus_0 &= ~(*(uint32_t *)buf);
		         break;
		case GPIO_IRQSTATUS_1:
			dev->regs.irqstatus_1 &= ~(*(uint32_t *)buf);
		         break;
		case GPIO_IRQSTATUS_SET_0:
			dev->regs.irqstatus_set_0 = *(uint32_t *)buf;
			dev->regs.irq_set_status0 |= dev->regs.irqstatus_set_0;
		         break;
		case GPIO_IRQSTATUS_SET_1:
			dev->regs.irqstatus_set_1 = *(uint32_t *)buf;
			dev->regs.irq_set_status1 |= dev->regs.irqstatus_set_1;
		         break;
		case GPIO_IRQSTATUS_CLR_0:
			dev->regs.irqstatus_clr_0 = *(uint32_t *)buf;
			dev->regs.irq_set_status0 &= ~dev->regs.irqstatus_clr_0;
		         break;
		case GPIO_IRQSTATUS_CLR_1:
			dev->regs.irqstatus_clr_1 = *(uint32_t *)buf;
			dev->regs.irq_set_status1 &= dev->regs.irqstatus_clr_1;
		         break;
		case GPIO_IRQWAKEN_0:
			dev->regs.irqwaken_0 = *(uint32_t *)buf;
		         break;
		case GPIO_IRQWAKEN_1:
			dev->regs.irqwaken_0 = *(uint32_t *)buf;
		         break;
		case GPIO_CTRL:
			dev->regs.ctrl = *(uint32_t *)buf;
		         break;
		case GPIO_OE:
			dev->regs.oe = *(uint32_t *)buf;
		         break;
		case GPIO_DATAOUT:
			dev->regs.dataout = *(uint32_t *)buf;
		         break;
		case GPIO_LEVELDETECT0:
			dev->regs.levedetect0 = *(uint32_t *)buf;
		         break;
		case GPIO_LEVELDETECT1:
			dev->regs.levedetect1 = *(uint32_t *)buf;
		         break;
		case GPIO_RISINGDETECT:
			dev->regs.risingdetect = *(uint32_t *)buf;
		         break;
		case GPIO_FALLINGDETECT:
			dev->regs.fallingdetect = *(uint32_t *)buf;
		         break;
		case GPIO_DEBOUNCENABLE:
			dev->regs.debouncenable = *(uint32_t *)buf;
		         break;
		case GPIO_DEBOUNCINGTIME:
			dev->regs.debouncingtime = *(uint32_t *)buf;
			break;
		case GPIO_CLEARDATAOUT:
			dev->regs.cleardataout = *(uint32_t *)buf;
			dev->regs.dataout &= ~dev->regs.cleardataout;
			break;
		case GPIO_SETDATAOUT:
			dev->regs.setdataout = *(uint32_t *)buf;
			dev->regs.dataout |= dev->regs.setdataout;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__,"Can not write the register at 0x%x in %s\n", offset, dev->obj->objname);
			break;
	}
	return  No_exp;
}
uint32_t am3359_gpio_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	am3359_gpio_dev *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(&dev->regs) + id;
	return *regs_value;
}

static exception_t am3359_gpio_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	am3359_gpio_dev *dev = conf_obj->obj;
	struct registers *regs = &dev->regs;
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t am3359_gpio_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	am3359_gpio_dev *dev = conf_obj->obj;
	return  sizeof(dev->regs) / 4;
}

static uint32_t am3359_gpio_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	am3359_gpio_dev *dev = conf_obj->obj;
	struct registers *regs = &dev->regs;
	uint32_t i, regnum = sizeof(dev->regs)/ 4;

	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
char* am3359_gpio_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	am3359_gpio_dev *dev = conf_obj->obj;
	return dev->regs_name[id];
}

static void init_regs(am3359_gpio_dev *dev)
{
	memset(&dev->regs, 0, sizeof(dev->regs));

	dev->regs.revision 		= 0x50600801;
	dev->regs.oe			= 0xffffffff;

	dev->regs_name 			= regs_name;
}
static conf_object_t* new_am3359_gpio(char* obj_name)
{
	am3359_gpio_dev* dev = skyeye_mm_zero(sizeof(am3359_gpio_dev));

	dev->obj = new_conf_object(obj_name, dev);
	init_regs(dev);

	return dev->obj;
}

static exception_t reset_am3359_gpio(conf_object_t* opaque, const char* args)
{
	am3359_gpio_dev *dev = opaque->obj;
	init_regs(dev);

	return No_exp;
}

static exception_t free_am3359_gpio(conf_object_t* conf_obj)
{
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);

	return No_exp;
}
void init_am3359_gpio() {
	static skyeye_class_t class_data = {
		.class_name = "am3359_gpio",
		.class_desc = "am3359_gpio",
		.new_instance = new_am3359_gpio,
		.reset_instance = reset_am3359_gpio,
		.free_instance = free_am3359_gpio,
		.get_attr = NULL,
		.set_attr = am3359_gpio_set_attr,
		.module_type = SKYML_ONCHIP(SC_am335x SC_am3359),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = am3359_gpio_read,
		.write = am3359_gpio_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = am3359_gpio_get_regvalue_by_id,
		.get_regname_by_id = am3359_gpio_get_regname_by_id,
		.set_regvalue_by_id = am3359_gpio_set_regvalue_by_id,
		.get_regnum = am3359_gpio_get_regnum,
		.get_regid_by_name = am3359_gpio_get_regid_by_name,
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
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, CONNECTS_NULL);
}

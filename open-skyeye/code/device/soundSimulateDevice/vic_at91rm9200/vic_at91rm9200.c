/* Copyright (C)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundatdev->; either version 2
* of the License, or (at your optdev->) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundatdev->, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/
/**
* @file vic_at91rm9200.c
* @brief The implementatdev-> of system controller
* @author
* @versdev-> 78.77
*/

#include <skyeye_lock.h>
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

#include "vic_at91rm9200.h"

static int vic_at91rm9200_raise_signal(conf_object_t *opaque, int line){
	vic_at91rm9200_device *dev = opaque->obj;
	interrupt_signal_t interrupt_signal;
	uint32_t res = 1 << line;


	/* interrupt is masked */
	if ((dev->regs.aic_imr & res) == 0)
		return No_exp;

	dev->regs.aic_ipr |= res;
	dev->regs.aic_isr = line;

	if (line == 0) {
		/* firq interrupt */
		dev->regs.aic_fvr = dev->regs.aic_svr[0];
		dev->regs.aic_cisr |= 1;

		interrupt_signal.arm_signal.firq = Low_level;
		interrupt_signal.arm_signal.irq = Prev_level;
		interrupt_signal.arm_signal.reset = Prev_level;

		if (dev->signal_iface)
			dev->signal_iface->signal(dev->signal, &interrupt_signal);
	} else {
		if (dev->regs.aic_ffsr & res) {
			/* force fast irq */
			dev->regs.aic_fvr = dev->regs.aic_svr[0];
			dev->regs.aic_cisr |= 1;

			interrupt_signal.arm_signal.firq = Low_level;
			interrupt_signal.arm_signal.irq = Prev_level;
			interrupt_signal.arm_signal.reset = Prev_level;

			if (dev->signal_iface)
				dev->signal_iface->signal(dev->signal, &interrupt_signal);
		} else {
			/* irq */
			dev->regs.aic_ivr = dev->regs.aic_svr[line];
			dev->regs.aic_cisr |= 1 << 1;

			interrupt_signal.arm_signal.firq = Prev_level;
			interrupt_signal.arm_signal.irq = Low_level;
			interrupt_signal.arm_signal.reset = Prev_level;

			if (dev->signal_iface)
				dev->signal_iface->signal(dev->signal, &interrupt_signal);
		}
	}

	return No_exp;
}

static int vic_at91rm9200_lower_signal(conf_object_t *opaque, int line)
{
	struct vic_at91rm9200_device *dev = opaque->obj;
	return No_exp;
}

static void src_moder_filter(vic_at91rm9200_device *dev, uint32_t *val)
{
	int i, mod;

	for (i = 0; i < 32; i++) {
		if (*val & (1 << i)) {
			mod = (dev->regs.aic_smr[i] >> 5) & 0x3;
			/* level trigger */
			if (mod == 0 || mod == 2) {
				*val &= ~(1 << i);
			}
		}
	}
}

static exception_t vic_at91rm9200_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	vic_at91rm9200_device *dev = opaque->obj;
	uint32_t index, irqno, mod;

	if (offset >= AIC_SMR && offset < AIC_SVR) {
		index = offset >> 2;
		*(uint32_t *)buf = dev->regs.aic_smr[index];
	} else if (offset >= AIC_SVR && offset < AIC_IVR) {
		index = (offset - AIC_SVR) >> 2;
		*(uint32_t *)buf = dev->regs.aic_svr[index];
	} else {
		switch (offset) {
			case AIC_IVR:
				if (dev->regs.aic_ipr >> 1 == 0) {
					*(uint32_t *)buf = dev->regs.aic_spu;
				} else {
					irqno = dev->regs.aic_isr & 0x1f;
					dev->regs.aic_ipr |= 1 << irqno;
					/* only not force fast interrupt we auto clean pending reg */
					if ((dev->regs.aic_ffsr & (1 << irqno)) == 0) {
						/* disable cisr nIRQ, when raise signal to enable? */
						dev->regs.aic_cisr &= ~(1 << 1);

						/* if edge triggered, clean ipr */
						mod = (dev->regs.aic_smr[irqno] >> 5) & 0x3;
						if (mod == 1 || mod == 3)
							dev->regs.aic_ipr &= ~(1 << irqno);
					}
					/* if catch an interrupt, aic_ivr is equal aic_svr[irqno]
					 * *(uint32_t *)buf = dev->regs.aic_svr[irqno];
					 */
					*(uint32_t *)buf = dev->regs.aic_ivr;
				}
				break;
			case AIC_FVR:
				if ((dev->regs.aic_ipr & 0x1) == 0) {
					*(uint32_t *)buf = dev->regs.aic_spu;
				} else {
					irqno = dev->regs.aic_isr & 0x1f;
					if (irqno == 0) {
						/* if edge triggered, clean ipr */
						mod = (dev->regs.aic_smr[0] >> 5) & 0x3;
						if (mod == 1 || mod == 3) {
							dev->regs.aic_ipr &= ~0x1;
							/* disable de-asserts the nFIQ line on the
							 * processor when raise signal to enable? */
							dev->regs.aic_ipr &= ~1;
						}
						/* if catch an interrupt, aic_fvr is equal aic_svr[0]
						 * *(uint32_t *)buf = dev->regs.aic_svr[0];
						 */
						*(uint32_t *)buf = dev->regs.aic_fvr;
					}
				}
				break;
			case AIC_ISR:
				*(uint32_t *)buf = dev->regs.aic_isr;
				dev->regs.aic_isr = 0;
				dev->regs.aic_ipr = 0;
				dev->regs.aic_ivr = 0;
				break;
			case AIC_IPR:
				*(uint32_t *)buf = dev->regs.aic_ipr;
				dev->regs.aic_ipr = 0;
				break;
			case AIC_IMR:
				*(uint32_t *)buf = dev->regs.aic_imr;
				break;
			case AIC_CISR:
				*(uint32_t *)buf = dev->regs.aic_cisr;
				break;
			case AIC_SPU:
				*(uint32_t *)buf = dev->regs.aic_spu;
				break;
			case AIC_DCR:
				*(uint32_t *)buf = dev->regs.aic_dcr;
				break;
			case AIC_FFSR:
				*(uint32_t *)buf = dev->regs.aic_ffsr;
				break;
			default:
				skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in vic_at91rm9200\n", offset);
				break;
		}
	}
	return No_exp;
}

static exception_t vic_at91rm9200_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	vic_at91rm9200_device *dev = opaque->obj;
	uint32_t val, index;

	val = *(uint32_t *)buf;

	if (offset >= AIC_SMR && offset < AIC_SVR) {
		index = offset >> 2;
		dev->regs.aic_smr[index] = val;
	} else if (offset >= AIC_SVR && offset < AIC_IVR) {
		index = (offset - AIC_SVR) >> 2;
		dev->regs.aic_svr[index] = val;
	} else {
		switch (offset) {
			case AIC_IECR:
				dev->regs.aic_iecr = val;
				dev->regs.aic_imr |= val;
				break;
			case AIC_IDCR:
				dev->regs.aic_idcr = val;
				dev->regs.aic_imr &= ~val;
				break;
			case AIC_ICCR:
				dev->regs.aic_iccr = val;
				src_moder_filter(dev, &val);
				dev->regs.aic_ipr &= ~val;
				break;
			case AIC_ISCR:
				dev->regs.aic_iscr = val;
				src_moder_filter(dev, &val);
				dev->regs.aic_ipr |= val;
				break;
			case AIC_EOICR:
				dev->regs.aic_eoicr = val;
				break;
			case AIC_SPU:
				dev->regs.aic_spu = val;
				break;
			case AIC_DCR:
				dev->regs.aic_dcr = val;
				break;
			case AIC_FFER:
				dev->regs.aic_ffer = val;
				dev->regs.aic_ffsr |= val;
				break;
			case AIC_FFDR:
				dev->regs.aic_ffdr = val;
				dev->regs.aic_ffsr &= ~val;
				break;
			default:
				skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in vic_at91rm9200\n", offset);
				break;
		}
	}
	return No_exp;
}

static char* vic_at91rm9200_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t vic_at91rm9200_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	vic_at91rm9200_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t vic_at91rm9200_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t vic_at91rm9200_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers)/ 4;
	int i;

	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t vic_at91rm9200_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id) {
	vic_at91rm9200_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static exception_t vic_at91rm9200_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	vic_at91rm9200_device *dev = opaque->obj;
}

static conf_object_t* new_vic_at91rm9200(char* obj_name){
	vic_at91rm9200_device* dev = skyeye_mm_zero(sizeof(vic_at91rm9200_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	memset(regs, 0, sizeof(struct registers));
	return dev->obj;
}
static exception_t free_vic_at91rm9200(conf_object_t* opaque) {
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx) {
	vic_at91rm9200_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(0);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	vic_at91rm9200_device *dev = conf_obj->obj;
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	vic_at91rm9200_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);
	if(dev->signal_iface == NULL) {
		if (dev->signal == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);
	}
	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	vic_at91rm9200_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
static vic_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");
}
void init_vic_at91rm9200(){
	static skyeye_class_t class_data = {
		.class_name = "at91rm9200_vic",
		.class_desc = "at91rm9200_vic",
		.new_instance = new_vic_at91rm9200,
		.free_instance = free_vic_at91rm9200,
		.get_attr = NULL,
		.set_attr = vic_at91rm9200_set_attr,
		.module_type = SKYML_ONCHIP(SC_at91rm9200),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = vic_at91rm9200_read,
		.write = vic_at91rm9200_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = vic_at91rm9200_get_regvalue_by_id,
		.get_regname_by_id = vic_at91rm9200_get_regname_by_id,
		.set_regvalue_by_id = vic_at91rm9200_set_regvalue_by_id,
		.get_regnum = vic_at91rm9200_get_regnum,
		.get_regid_by_name = vic_at91rm9200_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const general_signal_intf signal_intf = {
		.raise_signal = vic_at91rm9200_raise_signal,
		.lower_signal = vic_at91rm9200_lower_signal,
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.iface = &signal_intf,
		},
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
 	};

	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);


	vic_register_attribute(clss);
}

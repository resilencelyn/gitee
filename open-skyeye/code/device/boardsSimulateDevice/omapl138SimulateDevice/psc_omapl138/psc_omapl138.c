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
* @file psc_omapl138.c
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
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_attribute.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_core_intf.h>

#include "psc_omapl138.h"

static exception_t omapl138_psc_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct omapl138_psc_device *dev = opaque->obj;
	psc_reg_t* regs = dev->regs;
	uint32_t index;

	if (offset >= MDSTAT && offset < MDCTL) {
		index = (offset - MDSTAT) >> 2;
		*(uint32_t*)buf = regs->mdstat[index];

		return No_exp;
	}

	if (offset >= MDCTL && offset <= MDEND) {
		index = (offset - MDCTL) >> 2;
		*(uint32_t*)buf = regs->mdctl[index];

		return No_exp;
	}

	switch(offset) {
		case REVID:
			*(uint32_t*)buf = regs->revid;
			break;
		case INTEVAL:
			*(uint32_t*)buf = regs->inteval;
			break;
		case MERRPR0:
			*(uint32_t*)buf = regs->merrpr0;
			break;
		case MERRCR0:
			*(uint32_t*)buf = regs->merrcr0;
			break;
		case PERRPR:
			*(uint32_t*)buf = regs->perrpr;
			break;
		case PERRCR:
			*(uint32_t*)buf = regs->perrcr;
			break;
		case PTCMD:
			*(uint32_t*)buf = regs->ptcmd;
			break;
		case PTSTAT:
			*(uint32_t*)buf = regs->ptstat;
			break;
		case PDSTAT0:
			*(uint32_t*)buf = regs->pdstat0;
			break;
		case PDSTAT1:
			*(uint32_t*)buf = regs->pdstat1;
			break;
		case PDCTL0:
			*(uint32_t*)buf = regs->pdctl0;
			break;
		case PDCTL1:
			*(uint32_t*)buf = regs->pdctl1;
			break;
		case PDCFG0:
			*(uint32_t*)buf = regs->pdcfg0;
			break;
		case PDCFG1:
			*(uint32_t*)buf = regs->pdcfg1;
			break;

		case EPCPR:
			*(uint32_t*)buf = regs->epcpr;
			break;

		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in psc\n", offset);
			*(uint32_t*)buf = 0;
			break;
	}
	return No_exp;
}

static exception_t omapl138_psc_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct omapl138_psc_device *dev = opaque->obj;
	psc_reg_t* regs = dev->regs;
	uint32_t index, val = *(uint32_t*)buf;

	if (offset >= MDCTL && offset <= MDEND) {
		index = (offset - MDCTL) >> 2;

		if (index == 14) {
			if ((val & (1 << 8)) == 0) {
				/* arm reset */
			}
		} else if (index == 15) {
			if ((val & (1 << 8)) == 0) {
				/* dsp reset */
				if (dev->exec_iface != NULL)
					dev->exec_iface->power(dev->c6k_obj, 1);
			}
		}

		regs->mdctl[index] = val;

		regs->mdstat[index] &= ~0x1f;
		regs->mdstat[index] |= (val & 0x1f);

		return No_exp;
	}

	switch(offset) {
		case INTEVAL:
			regs->inteval = val;
			break;
		case MERRCR0:
			regs->merrcr0 = val;
			regs->merrpr0 &= ~val;
			regs->mdstat[15] &= ~((1 << 16) | (1 << 17));
			regs->mdstat[14] &= ~((1 << 16) | (1 << 17));
			break;
		case PERRCR:
			regs->perrcr = val;
			regs->perrpr &= ~val;
			//regs->pdstat1
			break;
		case PTCMD:
			regs->ptcmd = val;
			if (val & 0x1) {
				/* state translate */
				regs->ptstat |= 0x1;
				/* wait to zero */
				regs->ptstat &= ~0x1;

				regs->epcpr |= 0x1;
			}
			if (val & 0x2) {
				/* state translate */
				regs->ptstat |= 0x2;
				/* wait to zero */
				regs->ptstat &= ~0x2;

				regs->epcpr |= 0x2;
			}
			break;
		case PDCTL0:
			regs->pdctl0 = val;
			break;
		case PDCTL1:
			regs->pdctl1 = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in psc\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static char* omapl138_psc_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	omapl138_psc_device *dev = conf_obj->obj;
	return dev->regs_name[id];
}

static uint32_t omapl138_psc_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	omapl138_psc_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static exception_t omapl138_psc_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	omapl138_psc_device *dev = conf_obj->obj;
	psc_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t omapl138_psc_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(psc_reg_t) / 4;
}

static uint32_t omapl138_psc_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	omapl138_psc_device *dev = conf_obj->obj;
	uint32_t regnum;
	regnum = sizeof(psc_reg_t) / 4;
	int i;
	for(i =0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_omapl138_psc(char* obj_name){
	omapl138_psc_device* dev = skyeye_mm_zero(sizeof(omapl138_psc_device));
	psc_reg_t* regs =  skyeye_mm_zero(sizeof(psc_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	int i;

	/* init psc regs */
	regs->revid	= 0x44825a00;
	regs->pdctl0	= 0x001ff101;
	regs->pdctl1	= 0x001ff101;
	regs->pdcfg0	= 0x0000000d;
	regs->pdcfg1	= 0x0000000d;

	for (i = 0; i < 32; i++)
		regs->mdstat[i] = 0x00000b00;

	dev->regs = regs;
	dev->regs_name = regs_name;

	return dev->obj;
}
static exception_t free_omapl138_psc(conf_object_t* opaque) {
	omapl138_psc_device* dev = opaque->obj;

	skyeye_free(dev->regs);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

void omapl138_psc_register_attribute(conf_class_t* clss){
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	omapl138_psc_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	omapl138_psc_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
static exception_t power_interface_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	omapl138_psc_device *dev = obj->obj;
	dev->c6k_obj = obj2;
	dev->exec_iface = (core_exec_intf *)SKY_get_iface(dev->c6k_obj, CORE_EXEC_INTF_NAME);

	if (dev->exec_iface == NULL){
		if (dev->c6k_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_EXEC_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_EXEC_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
static exception_t power_interface_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	omapl138_psc_device *dev = obj->obj;
	*obj2 = dev->c6k_obj;
	*port = NULL;
	return No_exp;
}
void init_omapl138_psc(){
	static skyeye_class_t class_data = {
		.class_name = "omapl138_psc",
		.class_desc = "omapl138_psc",
		.new_instance = new_omapl138_psc,
		.free_instance = free_omapl138_psc,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_omapl138 SC_c6748),
	};
	conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = omapl138_psc_read,
		.write = omapl138_psc_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = omapl138_psc_get_regvalue_by_id,
		.get_regname_by_id = omapl138_psc_get_regname_by_id,
		.set_regvalue_by_id = omapl138_psc_set_regvalue_by_id,
		.get_regnum = omapl138_psc_get_regnum,
		.get_regid_by_name = omapl138_psc_get_regid_by_name,
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
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
		(struct ConnectDescription) {
			.name = CORE_EXEC_INTF_NAME,
			.set = power_interface_set,
			.get = power_interface_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	omapl138_psc_register_attribute(clss);
}

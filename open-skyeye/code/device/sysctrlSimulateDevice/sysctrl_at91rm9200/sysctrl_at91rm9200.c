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
* @file sysctrl_at91rm9200.c
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

#include "sysctrl_at91rm9200.h"

static exception_t at91rm9200_sysctrl_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct at91rm9200_sysctrl_device *dev = opaque->obj;
	sysctrl_reg_t* regs = dev->regs;
	switch(offset) {
		case PMC_SCSR:
			*(uint32_t*)buf = regs->pmc_scsr;
			break;
		case PMC_PCSR:
			*(uint32_t*)buf = regs->pmc_pcsr;
			break;
		case CKGR_MOR:
			*(uint32_t*)buf = regs->ckgr_mor;
			break;
		case CKGR_MCFR:
			*(uint32_t*)buf = regs->ckgr_mcfr;
			break;
		case CKGR_PLLAR:
			*(uint32_t*)buf = regs->ckgr_pllar;
			break;
		case CKGR_PLLBR:
			*(uint32_t*)buf = regs->ckgr_pllbr;
			break;
		case PMC_MCKR:
			*(uint32_t*)buf = regs->pmc_mckr;
			break;
		case PMC_PCK0:
			*(uint32_t*)buf = regs->pmc_pck0;
			break;
		case PMC_PCK1:
			*(uint32_t*)buf = regs->pmc_pck1;
			break;
		case PMC_PCK2:
			*(uint32_t*)buf = regs->pmc_pck2;
			break;
		case PMC_PCK3:
			*(uint32_t*)buf = regs->pmc_pck3;
			break;
		case PMC_SR:
			*(uint32_t*)buf = regs->pmc_sr;
			break;
		case PMC_IMR:
			*(uint32_t*)buf = regs->pmc_imr;
			break;
		default:
			printf("Can not read the register at 0x%x in sysctrl\n", offset);
			*(uint32_t*)buf = 0;
			return No_exp;
	}
	return No_exp;
}

static void start_oscillator_untile_stable(struct at91rm9200_sysctrl_device *dev)
{
	uint32_t oscount, mainrdy, mainf;
	oscount = (dev->regs->ckgr_mor >> 8) & 0xf;

	/* oscount decreasing by slow clk / 8, max time is 62ms */
	dev->regs->pmc_sr |= 1;
	if ((dev->regs->pmc_imr & 1) == 0) {
		/* raise signal */
		if (dev->signal_iface != NULL)
			dev->signal_iface->raise_signal(dev->signal, dev->sig_no);
	}

	mainrdy = 1 << 16;
	/* main oscillator is 3.6864MHZ, so mainf is 0x708 */
	mainf = 0x708;

	dev->regs->ckgr_mcfr |= mainrdy;
	dev->regs->ckgr_mcfr |= 0xffff & mainf;
}
static exception_t at91rm9200_sysctrl_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct at91rm9200_sysctrl_device *dev = opaque->obj;
	sysctrl_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf, moscen, css, mckrdy, nmckrdy;

	switch(offset) {
		case PMC_SCER:
			regs->pmc_scer = val;
			regs->pmc_scsr |= val;
			break;
		case PMC_SCDR:
			regs->pmc_scdr = val;
			regs->pmc_scsr &= ~val;
			break;
		case PMC_PCER:
			regs->pmc_pcer = val;
			regs->pmc_pcsr |= val;
			break;
		case PMC_PCDR:
			regs->pmc_pcdr = val;
			regs->pmc_pcsr &= ~val;
			break;
		case CKGR_MOR:
			moscen = val & 1;
			regs->ckgr_mor = val;
			/* disable main oscillator */
			if (moscen == 0) {
				regs->pmc_sr &= ~1;
			} else {
				start_oscillator_untile_stable(dev);
			}
			break;
		case CKGR_PLLAR:
			regs->ckgr_pllar = val;
			regs->pmc_sr &= ~(1 << 1);
			/* wait count decrseing to 0 */
			regs->pmc_sr |= 1 << 1;
			if ((regs->pmc_imr & ( 1 << 1)) == 0) {
				/* raise signal */
				if (dev->signal_iface != NULL)
					dev->signal_iface->raise_signal(dev->signal, dev->sig_no);
			}
			break;
		case CKGR_PLLBR:
			regs->ckgr_pllbr = val;
			regs->pmc_sr &= ~(1 << 2);
			/* wait count decrseing to 0 */
			regs->pmc_sr |= 1 << 2;
			if ((regs->pmc_imr & (1 << 2)) == 0) {
				/* raise signal */
				if (dev->signal_iface != NULL)
					dev->signal_iface->raise_signal(dev->signal, dev->sig_no);
			}
			break;
		case PMC_MCKR:
			css = regs->pmc_mckr & 3;
			if (css != (regs->pmc_mckr & 3)) {
				regs->pmc_sr &= ~(1 << 3);
			}
			if (regs->pmc_mckr != val)
				regs->pmc_mckr = val;
			break;
		case PMC_PCK0:
			regs->pmc_pck0 = val;
			break;
		case PMC_PCK1:
			regs->pmc_pck1 = val;
			break;
		case PMC_PCK2:
			regs->pmc_pck2 = val;
			break;
		case PMC_PCK3:
			regs->pmc_pck3 = val;
			break;
		case PMC_IER:
			mckrdy = regs->pmc_sr & (1 << 3);
			mckrdy = mckrdy >> 3;

			nmckrdy = val & (1 << 3);
			nmckrdy = nmckrdy >> 3;

			if (mckrdy == 0 && nmckrdy == 1) {
				if ((regs->pmc_imr & (1 << 3)) == 0) {
					/* raise signal */
					if (dev->signal_iface != NULL)
						dev->signal_iface->raise_signal(dev->signal, dev->sig_no);
				}
			}

			regs->pmc_ier = val;
			regs->pmc_imr |= val;
			break;
		case PMC_IDR:
			regs->pmc_idr = val;
			regs->pmc_imr &= ~val;
			break;
		default:
			printf("Can not write the register at 0x%x in sysctrl\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}


static char* at91rm9200_sysctrl_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	at91rm9200_sysctrl_device *dev = conf_obj->obj;
	return dev->regs_name[id];
}

static uint32_t at91rm9200_sysctrl_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	at91rm9200_sysctrl_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static exception_t at91rm9200_sysctrl_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	at91rm9200_sysctrl_device *dev = conf_obj->obj;
	sysctrl_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t at91rm9200_sysctrl_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(sysctrl_reg_t) / 4;
}

static uint32_t at91rm9200_sysctrl_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	at91rm9200_sysctrl_device *dev = conf_obj->obj;
	uint32_t regnum;
	regnum = sizeof(sysctrl_reg_t) / 4;
	int i;
	for(i =0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_at91rm9200_sysctrl(char* obj_name){
	at91rm9200_sysctrl_device* dev = skyeye_mm_zero(sizeof(at91rm9200_sysctrl_device));
	sysctrl_reg_t* regs =  skyeye_mm_zero(sizeof(sysctrl_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init sysctrl regs */
	regs->pmc_scsr 		= 0x0001;

	regs->pmc_pcsr 		= 0x0000;

	regs->ckgr_mor 		= 0x0000;
	regs->ckgr_pllar 	= 0x3f00;
	regs->ckgr_pllbr 	= 0x3f00;
	/* where to find mckr init val ? */
	regs->pmc_mckr 		= 0x0202;

	regs->pmc_pck0 		= 0x0000;
	regs->pmc_pck1 		= 0x0000;
	regs->pmc_pck2 		= 0x0000;
	regs->pmc_pck3 		= 0x0000;

	regs->pmc_imr 		= 0x0000;

	dev->regs = regs;
	dev->regs_name = regs_name;

	/* 18.432MHZ / 3 * (28 + 1), this val is wrote by me,
	 * can't find suitable values from official documents,
	 * I think this val should be set by kernel code, but
	 * kernel don't init this register. If not set this
	 * register, the kernel can run, only the boot message:
	 * "Clocks: CPU 0 MHZ, master 0 MHZ, main 18.432 MHZ."
	 */
	regs->ckgr_pllar |= 3;
	regs->ckgr_pllar |= 28 << 16;

	dev->sig_no = 1;

	return dev->obj;
}
static exception_t free_at91rm9200_sysctrl(conf_object_t* opaque) {
	at91rm9200_sysctrl_device* dev = opaque->obj;

	skyeye_free(dev->regs);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

void at91rm9200_sysctrl_register_attribute(conf_class_t* clss){
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	at91rm9200_sysctrl_device *dev = obj->obj;
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
	at91rm9200_sysctrl_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
void init_at91rm9200_sysctrl(){
	static skyeye_class_t class_data = {
		.class_name = "at91rm9200_sysctrl",
		.class_desc = "at91rm9200 sysctrl",
		.new_instance = new_at91rm9200_sysctrl,
		.free_instance = free_at91rm9200_sysctrl,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_at91rm9200),
	};
	conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = at91rm9200_sysctrl_read,
		.write = at91rm9200_sysctrl_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = at91rm9200_sysctrl_get_regvalue_by_id,
		.get_regname_by_id = at91rm9200_sysctrl_get_regname_by_id,
		.set_regvalue_by_id = at91rm9200_sysctrl_set_regvalue_by_id,
		.get_regnum = at91rm9200_sysctrl_get_regnum,
		.get_regid_by_name = at91rm9200_sysctrl_get_regid_by_name,
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
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	at91rm9200_sysctrl_register_attribute(clss);
}

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
* @file syscfg_omapl138.c
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

#include "syscfg_omapl138.h"

static int do_with_chipsig(conf_object_t *opaque, int val)
{
	struct omapl138_syscfg_device *dev = opaque->obj;
	syscfg_reg_t* regs = dev->regs;
	int i;

	/*-----------------------------------------
	 *		SYSCFG_CHIPINT0 --> ARM(28)
	 *		SYSCFG_CHIPINT1 --> ARM(29)
	 * DSP(5)  <--	SYSCFG_CHIPINT2 --> ARM(30)
	 * DSP(67) <--	SYSCFG_CHIPINT3	--> ARM(31)
	 * DSP(??) <--	SYSCFG_CHIPINT4
	 *-----------------------------------------*/
	for (i = 0; i < 5; i++) {
		if (val & (1 << i)) {
			/* raise signal */
			if (i == 0) {
				/* CHIPSIG0, DSP TO ARM, SYSCFG_CHIPINT0 interrupt 28 */
				if (dev->arm_signal.signal_iface != NULL) {
					dev->arm_signal.signal_iface->raise_signal(
							dev->arm_signal.signal, 28);
				}
			} else if (i == 1) {
				/* CHIPSIG1, DSP TO ARM, SYSCFG_CHIPINT1 interrupt 29 */
				if (dev->arm_signal.signal_iface != NULL) {
					dev->arm_signal.signal_iface->raise_signal(
							dev->arm_signal.signal, 29);
				}
			} else if (i == 2) {
				/* CHIPSIG2, ARM TO DSP, SYSCFG_CHIPINT2 interrupt 5 */
				if (dev->c674x_signal.signal_iface != NULL) {
					dev->c674x_signal.signal_iface->raise_signal(
							dev->c674x_signal.signal, 5);
				}
				/* CHIPSIG2, DSP TO ARM, SYSCFG_CHIPINT2 interrupt 30 */
				if (dev->arm_signal.signal_iface != NULL) {
					dev->arm_signal.signal_iface->raise_signal(
							dev->arm_signal.signal, 30);
				}
			} else if (i == 3) {
				/* CHIPSIG3, ARM TO DSP, SYSCFG_CHIPINT3 interrupt 67 */
				if (dev->c674x_signal.signal_iface != NULL) {
					dev->c674x_signal.signal_iface->raise_signal(
							dev->c674x_signal.signal, 67);
				}
				/* CHIPSIG3, DSP TO ARM, SYSCFG_CHIPINT3 interrupt 31 */
				if (dev->arm_signal.signal_iface != NULL) {
					dev->arm_signal.signal_iface->raise_signal(
							dev->arm_signal.signal, 31);
				}
			} else if (i == 4) {
				/* CHIPSIG4, ARM TO DSP, NMI interrupt ? */
				skyeye_log(Error_log, __FUNCTION__, "CHIPSIG4 don't implement!\n");
			}
		}
	}
}
static exception_t omapl138_syscfg_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct omapl138_syscfg_device *dev = opaque->obj;
	syscfg_reg_t* regs = dev->regs;
	uint32_t index;

	if (offset >= PINMUX && offset < SUSPSRC) {
		index = (offset - PINMUX) >> 2;
		*(uint32_t*)buf = regs->pinmux[index];

		return No_exp;
	}

	switch(offset) {
		case REVID:
			*(uint32_t*)buf = regs->revid;
			break;
		case DEVIDR0:
			*(uint32_t*)buf = regs->devidr0;
			break;
		case BOOTCFG:
			*(uint32_t*)buf = regs->bootcfg;
			break;
		case KICK0R:
			*(uint32_t*)buf = regs->kick0r;
			break;
		case KICK1R:
			*(uint32_t*)buf = regs->kick1r;
			break;
		case HOST0CFG:
			*(uint32_t*)buf = regs->host0cfg;
			break;
		case HOST1CFG:
			*(uint32_t*)buf = regs->host1cfg;
			break;
		case IRAWSTAT:
			*(uint32_t*)buf = regs->irawstat;
			break;
		case IENSTAT:
			*(uint32_t*)buf = regs->ienstat;
			break;
		case IENSET:
			*(uint32_t*)buf = regs->ienset;
			break;
		case IENCLR:
			*(uint32_t*)buf = regs->ienclr;
			break;
		case EOI:
			*(uint32_t*)buf = regs->eoi;
			break;
		case FLTADDR:
			*(uint32_t*)buf = regs->fltaddr;
			break;
		case FLTSTAT:
			*(uint32_t*)buf = regs->fltstat;
			break;
		case MSTPRI0:
			*(uint32_t*)buf = regs->mstpri0;
			break;
		case MSTPRI1:
			*(uint32_t*)buf = regs->mstpri1;
			break;
		case MSTPRI2:
			*(uint32_t*)buf = regs->mstpri2;
			break;
		case SUSPSRC:
			*(uint32_t*)buf = regs->suspsrc;
			break;
		case CHIPSIG:
			*(uint32_t*)buf = regs->chipsig;
			break;
		case CHIPSIG_CLR:
			*(uint32_t*)buf = regs->chipsig_clr;
			break;
		case CFGCHIP0:
			*(uint32_t*)buf = regs->cfgchip0;
			break;
		case CFGCHIP1:
			*(uint32_t*)buf = regs->cfgchip1;
			break;
		case CFGCHIP2:
			*(uint32_t*)buf = regs->cfgchip2;
			break;
		case CFGCHIP3:
			*(uint32_t*)buf = regs->cfgchip3;
			break;
		case CFGCHIP4:
			*(uint32_t*)buf = regs->cfgchip4;
			break;
		case VTPIO_CTL:
			*(uint32_t*)buf = regs->vtpio_ctl;
			break;
		case DDR_SLEW:
			*(uint32_t*)buf = regs->ddr_slew;
			break;
		case DEEPSLEEP:
			*(uint32_t*)buf = regs->deepsleep;
			break;
		case PUPD_ENA:
			*(uint32_t*)buf = regs->pupd_ena;
			break;
		case PUPD_SEL:
			*(uint32_t*)buf = regs->pupd_sel;
			break;
		case RXACTIVE:
			*(uint32_t*)buf = regs->rxactive;
			break;
		case PWRDN:
			*(uint32_t*)buf = regs->pwrdn;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in syscfg\n", offset);
			*(uint32_t*)buf = 0;
			break;
	}
	return No_exp;
}

static exception_t omapl138_syscfg_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct omapl138_syscfg_device *dev = opaque->obj;
	syscfg_reg_t* regs = dev->regs;
	uint32_t index, val = *(uint32_t*)buf;

	if (offset >= PINMUX && offset < SUSPSRC) {
		index = (offset - PINMUX) >> 2;
		regs->pinmux[index] = val;

		return No_exp;
	}

	switch(offset) {
		case KICK0R:
			regs->kick0r = val;
			break;
		case KICK1R:
			regs->kick1r = val;
			break;
		case HOST0CFG:
			regs->host0cfg = val;
			break;
		case HOST1CFG:
			regs->host1cfg = val;
			break;
		case IRAWSTAT:
			regs->irawstat |= val;
			break;
		case IENSTAT:
			regs->ienstat &= ~val;
			break;
		case IENSET:
			regs->ienset = val;
			dev->syscfg_inter |= val;
			break;
		case IENCLR:
			regs->ienclr = val;
			dev->syscfg_inter &= ~val;
			break;
		case EOI:
			regs->eoi = val;
			regs->ienstat = 0;
			regs->irawstat = 0;
			break;
		case MSTPRI0:
			regs->mstpri0 = val;
			break;
		case MSTPRI1:
			regs->mstpri1 = val;
			break;
		case MSTPRI2:
			regs->mstpri2 = val;
			break;
		case SUSPSRC:
			regs->suspsrc = val;
			break;
		case CHIPSIG:
			regs->chipsig = val;
			do_with_chipsig(opaque, val);
			break;
		case CHIPSIG_CLR:
			regs->chipsig_clr = val;
			regs->chipsig &= ~val;
			break;
		case CFGCHIP0:
			regs->cfgchip0 = val;
			break;
		case CFGCHIP1:
			regs->cfgchip1 = val;
			break;
		case CFGCHIP2:
			regs->cfgchip2 = val;
			if (((val >> 9) & 0x3) == 0)
				regs->cfgchip2 |= (0x1 << 17);
			break;
		case CFGCHIP3:
			regs->cfgchip3 = val;
			break;
		case CFGCHIP4:
			regs->cfgchip4 = val;
			break;
		case VTPIO_CTL:
			regs->vtpio_ctl = val;
			break;
		case DDR_SLEW:
			regs->ddr_slew = val;
			break;
		case DEEPSLEEP:
			regs->deepsleep = val;
			break;
		case PUPD_ENA:
			regs->pupd_ena = val;
			break;
		case PUPD_SEL:
			regs->pupd_sel = val;
			break;
		case RXACTIVE:
			regs->rxactive = val;
			break;
		case PWRDN:
			regs->pwrdn = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in syscfg\n", offset);
			return No_exp;
	}
	return No_exp;
}


static char* omapl138_syscfg_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	omapl138_syscfg_device *dev = conf_obj->obj;
	return dev->regs_name[id];
}

static uint32_t omapl138_syscfg_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	omapl138_syscfg_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static exception_t omapl138_syscfg_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	omapl138_syscfg_device *dev = conf_obj->obj;
	syscfg_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t omapl138_syscfg_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(syscfg_reg_t) / 4;
}

static uint32_t omapl138_syscfg_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	omapl138_syscfg_device *dev = conf_obj->obj;
	uint32_t regnum;
	regnum = sizeof(syscfg_reg_t) / 4;
	int i;
	for(i =0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_omapl138_syscfg(char* obj_name){
	omapl138_syscfg_device* dev = skyeye_mm_zero(sizeof(omapl138_syscfg_device));
	syscfg_reg_t* regs =  skyeye_mm_zero(sizeof(syscfg_reg_t));
	dev->obj = new_conf_object(obj_name, dev);

	/* init syscfg regs */
	regs->revid	= 0x4e840102;
	regs->devidr0	= 0x1b7d102f;
	regs->host1cfg	= 0x00700000;
	regs->mstpri0	= 0x44222222;
	regs->mstpri1	= 0x44440000;
	regs->mstpri2	= 0x54604404;
	regs->suspsrc	= 0xffffffff;
	regs->cfgchip2	= 0x0000ef00;
	regs->cfgchip3	= 0x0000ff00;
	regs->cfgchip4	= 0x0000ff00;
	regs->vtpio_ctl	= 0x00000077;
	regs->deepsleep	= 0x0000ffff;
	regs->pupd_ena	= 0xffffffff;
	regs->pupd_sel	= 0xc3ffffff;
	regs->rxactive	= 0xffffffff;
	regs->pwrdn	= 0xfffffffe;

	dev->regs = regs;
	dev->regs_name = regs_name;

	return dev->obj;
}
static exception_t free_omapl138_syscfg(conf_object_t* opaque) {
	omapl138_syscfg_device* dev = opaque->obj;

	skyeye_free(dev->regs);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

void omapl138_syscfg_register_attribute(conf_class_t* clss){
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	omapl138_syscfg_device *dev = obj->obj;
	if (index == 0) {
		dev->arm_signal.signal = obj2;
		dev->arm_signal.signal_iface = (general_signal_intf *)SKY_get_iface(dev->arm_signal.signal, GENERAL_SIGNAL_INTF_NAME);

		if (dev->arm_signal.signal_iface == NULL){
			if (dev->arm_signal.signal == NULL)
			{
				skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
			} else
				skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

			return Not_found_exp;
		}
	} else if (index == 1) {
		dev->c674x_signal.signal = obj2;
		dev->c674x_signal.signal_iface = (general_signal_intf *)SKY_get_iface(dev->c674x_signal.signal, GENERAL_SIGNAL_INTF_NAME);
		if (dev->c674x_signal.signal_iface == NULL){
			if (dev->c674x_signal.signal == NULL)
			{
				skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
			} else
				skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

			return Not_found_exp;
		}
	}
	return No_exp;
}
static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
#if 0
	omapl138_syscfg_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
#endif
	return No_exp;
}
void init_omapl138_syscfg(){
	static skyeye_class_t class_data = {
		.class_name = "omapl138_syscfg",
		.class_desc = "omapl138 syscfg",
		.new_instance = new_omapl138_syscfg,
		.free_instance = free_omapl138_syscfg,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_omapl138 SC_c6748),
	};
	conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = omapl138_syscfg_read,
		.write = omapl138_syscfg_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = omapl138_syscfg_get_regvalue_by_id,
		.get_regname_by_id = omapl138_syscfg_get_regname_by_id,
		.set_regvalue_by_id = omapl138_syscfg_set_regvalue_by_id,
		.get_regnum = omapl138_syscfg_get_regnum,
		.get_regid_by_name = omapl138_syscfg_get_regid_by_name,
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

	omapl138_syscfg_register_attribute(clss);
}

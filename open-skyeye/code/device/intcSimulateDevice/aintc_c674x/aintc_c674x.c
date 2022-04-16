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
* @file aintc_c674x.c
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

#include "aintc_c674x.h"

static int get_int_num(aintc_c674x_device *dev, int line)
{
	uint32_t intmux1 = dev->regs.intmux1;
	uint32_t intmux2 = dev->regs.intmux2;
	uint32_t intmux3 = dev->regs.intmux3;
	uint32_t i;

	/* get INT[15:4] number, this number only determine priority */
	for (i = 0; i < 4; i++) {
		if (((intmux1 >> i * 8) & 0x7f) == line) {
			return 4 + i;
		}
	}
	for (i = 0; i < 4; i++) {
		if (((intmux2 >> i * 8) & 0x7f) == line) {
			return 8 + i;
		}
	}
	for (i = 0; i < 4; i++) {
		if (((intmux3 >> i * 8) & 0x7f) == line) {
			return 12 + i;
		}
	}

	return -1;
}
static int aintc_c674x_raise_signal(conf_object_t *opaque, int line)
{
	aintc_c674x_device *dev = opaque->obj;
	interrupt_signal_t interrupt_signal;
	uint32_t index, num, res;
	uint32_t *pevtflag = dev->regs.evtflag;
	uint32_t *pevtmask = dev->regs.evtmask;
	uint32_t *pmevtflag = dev->regs.mevtflag;
	uint32_t *pexpmask = dev->regs.expmask;
	uint32_t *pmexpflag = dev->regs.mexpflag;
	uint32_t intdmask = dev->regs.intdmask;
	int evt0_3 = -1, evtnum, i;

	for (i = 0; i < 16; i++)
		interrupt_signal.c6k_signal.ifr[i] = Prev_level;

	index = line / 32;
	num = line % 32;
	res = 1 << num;

	/* EVT[127:4] */
	if (line < 4) return 0;

	pevtflag[index] |= res;
	pmevtflag[index] = pevtflag[index] & (~pevtmask[index]);

#define EXP_EVM 1
	if ((pexpmask[index] >> num & 0x1) == 0) {
		/* exception */
		pmexpflag[index] = pevtflag[index] & (~pexpmask[index]);
		interrupt_signal.c6k_signal.ifr[EXP_EVM] = High_level;
		if (dev->signal_iface)
			dev->signal_iface->signal(dev->signal, &interrupt_signal);

		return No_exp;
	}

	//evtnum = get_int_num(dev, line);
    evtnum = dev->EVT_intselect[line];
	if (evtnum != -1) {
		/* raise signal */
		dev->regs.intxstat = evtnum << 16 | line << 24;
		interrupt_signal.c6k_signal.ifr[evtnum] = High_level;
	}

	/* unmasked */
    
	if ((pevtmask[index] & res) == 0) {
		/* evt combiner number */
		evt0_3 = index;
		//evtnum = get_int_num(dev, evt0_3);
        evtnum = dev->EVT_intselect[evt0_3];

		if (evtnum != -1) {
			/* raise signal */
			dev->regs.intxstat = evtnum << 16 | evt0_3 << 24;
			interrupt_signal.c6k_signal.ifr[evtnum] = High_level;
		}
	}

	if (dev->signal_iface)
		dev->signal_iface->signal(dev->signal, &interrupt_signal);

	return No_exp;
}
static int aintc_c674x_lower_signal(conf_object_t *opaque, int line)
{
	struct aintc_c674x_device *dev = opaque->obj;
	return No_exp;
}

static exception_t aintc_c674x_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	aintc_c674x_device *dev = opaque->obj;
	uint32_t index;

	if (offset >= EVTFLAG && offset < EVTSET) {
		index = offset - EVTFLAG >> 2;
		*(uint32_t *)buf = dev->regs.evtflag[index];

		return No_exp;
	}
	if (offset >= EVTMASK && offset < MEVTFLAG) {
		index = offset - EVTMASK >> 2;
		*(uint32_t *)buf = dev->regs.evtmask[index];

		return No_exp;
	}
	if (offset >= MEVTFLAG && offset < EXPMASK) {
		index = offset - MEVTFLAG >> 2;
		*(uint32_t *)buf = dev->regs.mevtflag[index];

		return No_exp;
	}
	if (offset >= EXPMASK && offset < MEXPFLAG) {
		index = offset - EXPMASK >> 2;
		*(uint32_t *)buf = dev->regs.expmask[index];

		return No_exp;
	}
	if (offset >= MEXPFLAG && offset < INTMUX1) {
		index = offset - MEXPFLAG >> 2;
		*(uint32_t *)buf = dev->regs.mexpflag[index];

		return No_exp;
	}

	switch (offset) {
		case INTMUX1:
			*(uint32_t *)buf = dev->regs.intmux1;
			break;
		case INTMUX2:
			*(uint32_t *)buf = dev->regs.intmux2;
			break;
		case INTMUX3:
			*(uint32_t *)buf = dev->regs.intmux3;
			break;
		case AEGMUX0:
			*(uint32_t *)buf = dev->regs.aegmux0;
			break;
		case AEGMUX1:
			*(uint32_t *)buf = dev->regs.aegmux1;
			break;
		case INTXSTAT:
			*(uint32_t *)buf = dev->regs.intxstat;
			break;
		case INTXCLR:
			*(uint32_t *)buf = dev->regs.intxclr;
			break;
		case INTDMASK:
			*(uint32_t *)buf = dev->regs.intdmask;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in aintc_c674x\n", offset);
			break;
	}
	return No_exp;
}
static void check_INTSEL_changer(conf_object_t *obj,int INTMUXX,uint32_t val)
{
	aintc_c674x_device *dev = obj->obj;
    int i,j;
    uint32_t imtmux = 0;
    if(INTMUXX == 1){
        imtmux = dev->regs.intmux1;
    }else if(INTMUXX == 2){
        imtmux = dev->regs.intmux2;
    }else if(INTMUXX == 3){
        imtmux = dev->regs.intmux3;
    }else{
        skyeye_log(Error_log, __FUNCTION__, "Wrong parameters: %d\n", INTMUXX);
    }
    for(i = 0; i < 4; i++){
        dev->EVT_intselect[((imtmux >> i * 8) & 0x7f)] = INTMUXX * 4 + i;
    }
}
static exception_t aintc_c674x_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	aintc_c674x_device *dev = opaque->obj;
	uint32_t val, index;

	val = *(uint32_t *)buf;

	if (offset >= EVTSET && offset < EVTCLR) {
		index = offset - EVTSET >> 2;
		dev->regs.evtset[index] = val;
		dev->regs.evtflag[index] |= val;

		return No_exp;
	}
	if (offset >= EVTCLR && offset < EVTMASK) {
		index = offset - EVTCLR >> 2;
		dev->regs.evtclr[index] = val;
		dev->regs.evtflag[index] &= ~val;
		dev->regs.mevtflag[index] &= ~val;
		dev->regs.mexpflag[index] &= ~val;
		dev->regs.intxstat = 0;

		return No_exp;
	}
	if (offset >= EVTMASK && offset < MEVTFLAG) {
		index = offset - EVTMASK >> 2;
		dev->regs.evtmask[index] = val;

		return No_exp;
	}
	if (offset >= EXPMASK && offset < MEXPFLAG) {
		index = offset - EXPMASK >> 2;
		dev->regs.expmask[index] = val;

		return No_exp;
	}

	switch (offset) {
		case INTMUX1:
			dev->regs.intmux1 = val;
            check_INTSEL_changer(opaque,1,val);
 			break;
		case INTMUX2:
			dev->regs.intmux2 = val;
            check_INTSEL_changer(opaque,2,val);
			break;
		case INTMUX3:
			dev->regs.intmux3 = val;
            check_INTSEL_changer(opaque,3,val);
			break;
		case AEGMUX0:
			dev->regs.aegmux0 = val;
			break;
		case AEGMUX1:
			dev->regs.aegmux1 = val;
			break;
		case INTXSTAT:
			dev->regs.intxstat = val;
			break;
		case INTXCLR:
			dev->regs.intxclr = val;
			if (val & 1) {
				dev->regs.intxstat = 0;
				/* clear EVT96 */
				dev->regs.evtflag[3] &= ~1;
			}
			break;
		case INTDMASK:
			dev->regs.intdmask = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in aintc_c674x\n", offset);
			break;
	}
	return No_exp;
}

static char* aintc_c674x_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t aintc_c674x_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	aintc_c674x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t aintc_c674x_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t aintc_c674x_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers)/ 4;
	int i;

	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t aintc_c674x_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id) {
	aintc_c674x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static exception_t aintc_c674x_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	aintc_c674x_device *dev = opaque->obj;
}

static conf_object_t* new_aintc_c674x(char* obj_name){
	aintc_c674x_device* dev = skyeye_mm_zero(sizeof(aintc_c674x_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	memset(regs, 0, sizeof(struct registers));
    memset(dev->EVT_intselect,-1,sizeof(int) * 128);

	regs->evtmask[0]	= 0xf;
	regs->intmux1		= 0x4 | 0x5 << 8 | 0x6 << 16 | 0x7 << 24;
    dev->EVT_intselect[4] = 4;
    dev->EVT_intselect[5] = 5;
    dev->EVT_intselect[6] = 6;
    dev->EVT_intselect[7] = 7;
	regs->intmux2		= 0x8 | 0x9 << 8 | 0xa << 16 | 0xb << 24;
    dev->EVT_intselect[8] = 8;
    dev->EVT_intselect[9] = 9;
    dev->EVT_intselect[10] = 10;
    dev->EVT_intselect[11] = 11;
	regs->intmux3		= 0xc | 0xd << 8 | 0xe << 16 | 0xf << 24;
    dev->EVT_intselect[12] = 12;
    dev->EVT_intselect[13] = 13;
    dev->EVT_intselect[14] = 14;
    dev->EVT_intselect[15] = 15;
	regs->expmask[0]	= 0xffffffff;
	regs->expmask[1]	= 0xffffffff;
	regs->expmask[2]	= 0xffffffff;
	regs->expmask[4]	= 0xffffffff;
	regs->aegmux0		= 0x1 << 8 | 0x2 << 16 | 0x3 << 24;
	regs->aegmux1		= 0x4 | 0x5 << 8 | 0x6 << 16 | 0x7 << 24;

	return dev->obj;
}
static exception_t free_aintc_c674x(conf_object_t* opaque) {
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx) {
	aintc_c674x_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(0);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	aintc_c674x_device *dev = conf_obj->obj;
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	aintc_c674x_device *dev = obj->obj;
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
	aintc_c674x_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
void init_aintc_c674x(){
	static skyeye_class_t class_data = {
		.class_name = "c674x_aintc",
		.class_desc = "c674x_aintc",
		.new_instance = new_aintc_c674x,
		.free_instance = free_aintc_c674x,
		.get_attr = NULL,
		.set_attr = aintc_c674x_set_attr,
		.module_type = SKYML_ONCHIP(SC_c6748),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = aintc_c674x_read,
		.write = aintc_c674x_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = aintc_c674x_get_regvalue_by_id,
		.get_regname_by_id = aintc_c674x_get_regname_by_id,
		.set_regvalue_by_id = aintc_c674x_set_regvalue_by_id,
		.get_regnum = aintc_c674x_get_regnum,
		.get_regid_by_name = aintc_c674x_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const general_signal_intf signal_intf = {
		.raise_signal = aintc_c674x_raise_signal,
		.lower_signal = aintc_c674x_lower_signal,
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
}

/* Copyright (C) 
* 
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
* @file mcasp_omapl138.c
* @brief The implementation of system controller
* @author Kewei Yu keweihk@gmail.com
* @version 78.77
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_uart_ops.h>
#include <skyeye_iface.h>
#define DEBUG
#include <skyeye_log.h>

#include "mcasp_omapl138.h"

static exception_t mcasp_omapl138_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	mcasp_omapl138_device *dev = opaque->obj;
	mcasp_omapl138_reg_t* regs = dev->regs;
	int n;

	if (offset >= DITCSRA0 && offset <= DITCSRA5){
		n = (offset - DITCSRA0) / 4;
		*(uint32_t*)buf	= regs->ditcsra[n];
		return No_exp;
	}

	if (offset >= DITCSRB0 && offset <= DITCSRB5){
		n = (offset - DITCSRB0) / 4;
		*(uint32_t*)buf	= regs->ditcsrb[n];
		return No_exp;
	}
	if (offset >= DITUDRA0 && offset <= DITUDRA5){
		n = (offset - DITUDRA0) / 4;
		*(uint32_t*)buf	= regs->ditudra[n];
		return No_exp;
	}

	if (offset >= DITUDRB0 && offset <= DITUDRB5){
		n = (offset - DITUDRB0) / 4;
		*(uint32_t*)buf	= regs->ditudrb[n];
		return No_exp;
	}
	if (offset >= SRCTL0 && offset <= SRCTL15){
		n = (offset - SRCTL0) / 4;
		*(uint32_t*)buf	= regs->srctl[n];
		return No_exp;
	}
	if (offset >= XBUF0 && offset <= XBUF15){
		n = (offset - XBUF0) / 4;
		*(uint32_t*)buf	= regs->xbuf[n];
		return No_exp;
	}
	if (offset >= RBUF0 && offset <= RBUF15){
		n = (offset - RBUF0) / 4;
		if (dev->sound_iface != NULL)
			dev->sound_iface->read_sound(dev->sound_obj, buf, count);
		dev->transfer_type = READ_TYPE;
		return No_exp;
	}

	if (offset >= DATA_BASE_REG && offset <= DATA_END_REG){
		if (RX_RBUSEL == 0){
			offset -= DATA_BASE_REG;
			dev->transfer_type = READ_TYPE;
			if (dev->sound_iface != NULL)
				dev->sound_iface->read_sound(dev->sound_obj, buf, count);
		}	
		return No_exp;
	}
	switch(offset) {
		case REV:
			*(uint32_t*)buf = regs->rev;
			break;
		case PFUNC:
			*(uint32_t*)buf = regs->pfunc;
			break;
		case PDIR:
			*(uint32_t*)buf = regs->pdir;
			break;
		case PDOUT:
			*(uint32_t*)buf = regs->pdout;
			break;
		case PDIN:
			*(uint32_t*)buf = regs->pdin;
			break;
		case PDCLR:
			*(uint32_t*)buf = regs->pdclr;
			break;
		case GBLCTL:
			*(uint32_t*)buf = regs->gblctl;
			break;
		case AMUTE:
			*(uint32_t*)buf = regs->amute;
			break;
		case DLBCTL:
			*(uint32_t*)buf = regs->dlbctl;
			break;
		case DITCTL:
			*(uint32_t*)buf = regs->ditctl;
			break;
		case RGBLCTL:
			*(uint32_t*)buf = regs->rgblctl;
			break;
		case RMASK:
			*(uint32_t*)buf = regs->rmask;
			break;
		case RFMT:
			*(uint32_t*)buf = regs->rfmt;
			break;
		case AFSRCTL:
			*(uint32_t*)buf = regs->afsrctl;
			break;
		case ACLKRCTL:
			*(uint32_t*)buf = regs->aclkrctl;
			break;
		case AHCLKRCTL:
			*(uint32_t*)buf = regs->ahclkrctl;
			break;
		case RTDM:
			*(uint32_t*)buf = regs->rtdm;
			break;
		case RINTCTL:
			*(uint32_t*)buf = regs->rintctl;
			break;
		case RSTAT:
			*(uint32_t*)buf = regs->rstat;
			break;
		case RSLOT:
			*(uint32_t*)buf = regs->rslot;
			break;
		case RCLKCHK:
			*(uint32_t*)buf = regs->rclkchk;
			break;
		case REVTCTL:
			*(uint32_t*)buf = regs->revtctl;
			break;
		case XGBLCTL:
			*(uint32_t*)buf = regs->xgblctl;
			break;
		case XMASK:
			*(uint32_t*)buf = regs->xmask;
			break;
		case XFMT:
			*(uint32_t*)buf = regs->xfmt;
			break;
		case AFSXCTL:
			*(uint32_t*)buf = regs->afsxctl;
			break;
		case ACLKXCTL:
			*(uint32_t*)buf = regs->aclkxctl;
			break;
		case AHCLKXCTL:
			*(uint32_t*)buf = regs->ahclkxctl;
			break;
		case XTDM:
			*(uint32_t*)buf = regs->xtdm;
			break;
		case XINTCTL:
			*(uint32_t*)buf = regs->xintctl;
			break;
		case XSTAT:
			*(uint32_t*)buf = regs->xstat;
			break;
		case XSLOT:
			*(uint32_t*)buf = regs->xslot;
			break;
		case XCLKCHK:
			*(uint32_t*)buf = regs->xclkchk;
			break;
		case XEVTCTL:
			*(uint32_t*)buf = regs->xevtctl;
			break;
		case AFIFOREV:
			*(uint32_t*)buf = regs->afiforev;
			break;
		case WFIFOCTL:
			*(uint32_t*)buf = regs->wfifoctl;
			break;
		case WFIFOSTS:
			*(uint32_t*)buf = regs->wfifosts;
			break;
		case RFIFOCTL:
			*(uint32_t*)buf = regs->rfifoctl;
			break;
		case RFIFOSTS:
			*(uint32_t*)buf = regs->rfifosts;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__,"Can not read the register at 0x%x in mcasp_omapl138\n", offset);
			break;
	}
	return No_exp;
}

void check_pin(mcasp_omapl138_device* dev, int data){
	mcasp_omapl138_reg_t* regs = dev->regs;
	int i;
	for (i = 0; i < 16;i++){
		if ((data >> i) & 0x1){
			dev->transmit_pin = i;
		}
	}
}
static exception_t mcasp_omapl138_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	mcasp_omapl138_device *dev = opaque->obj;
	mcasp_omapl138_reg_t* regs = dev->regs;
	uint32_t data = *buf;
//	printf("In %s, offset:%x, buf:%x\n", __func__, offset, data);
	int n;

	if (offset >= DITCSRA0 && offset <= DITCSRA5){
		n = (offset - DITCSRA0) / 4;
		regs->ditcsra[n] = data;
		return No_exp;
	}

	if (offset >= DITCSRB0 && offset <= DITCSRB5){
		n = (offset - DITCSRB0) / 4;
		regs->ditcsrb[n] = data;
		return No_exp;
	}
	if (offset >= DITUDRA0 && offset <= DITUDRA5){
		n = (offset - DITUDRA0) / 4;
		regs->ditudra[n] = data;
		return No_exp;
	}

	if (offset >= DITUDRB0 && offset <= DITUDRB5){
		n = (offset - DITUDRB0) / 4;
		regs->ditudrb[n] = data;
		return No_exp;
	}
	if (offset >= SRCTL0 && offset <= SRCTL15){
		n = (offset - SRCTL0) / 4;
		regs->srctl[n] = data;
		if (SRMOD(n) == 1){
			dev->transmit_pin = n; 
		}else if (SRMOD(n) == 2){
			dev->receive_pin = n; 
		}

		return No_exp;
	}
	if (offset >= XBUF0 && offset <= XBUF15){
		n = (offset - XBUF0) / 4;
		regs->xbuf[n] = data;
		dev->transfer_type = WRITE_TYPE;
		return No_exp;
	}
	if (offset >= RBUF0 && offset <= RBUF15){
		n = (offset - RBUF0) / 4;
		regs->rbuf[n] = data;

		return No_exp;
	}

	if (offset >= DATA_BASE_REG && offset <= DATA_END_REG){
		if (TX_XBUSEL == 0){
			n = offset - DATA_BASE_REG;
			dev->xbuf[n] = data & 0xFF;
			dev->transfer_type = WRITE_TYPE;
		}	
		return No_exp; 
	}
	switch(offset) {
		case REV:
			regs->rev = data;
			break;
		case PFUNC:
			regs->pfunc = data;
			break;
		case PDIR:
			regs->pdir = data;
			check_pin(dev, data);
			break;
		case PDOUT:
			regs->pdout = data;
			break;
		case PDSET:
			regs->pdout |= data;
			break;
		case PDCLR:
			regs->pdout &= ~data;
			break;
		case GBLCTL:
			dev->old_gblctl = regs->gblctl; 
			regs->gblctl = data;
			if (((regs->gblctl >> 10) & 0x1) && 
				(((dev->old_gblctl >> 10) & 0x1) == 0)){ //transmit serializer enable 
				regs->xstat &= ~(0x1 << 5);
			}
			/*
			if (XSRCLR){
				regs->xstat &= ~(0x1 << 5);
			}
			*/
			break;
		case AMUTE:
			regs->amute = data;
			break;
		case DLBCTL:
			regs->dlbctl = data;
			break;
		case DITCTL:
			regs->ditctl = data;
			break;
		case RGBLCTL:
			regs->rgblctl = data;
			break;
		case RMASK:
			regs->rmask = data;
			break;
		case RFMT:
			regs->rfmt = data;
			break;
		case AFSRCTL:
			regs->afsrctl = data;
			break;
		case ACLKRCTL:
			regs->aclkrctl = data;
			break;
		case AHCLKRCTL:
			regs->ahclkrctl = data;
			break;
		case RTDM:
			regs->rtdm = data;
			break;
		case RINTCTL:
			regs->rintctl = data;
			break;
		case RSTAT:
			regs->rstat = data;
			break;
		case RSLOT:
			regs->rslot = data;
			break;
		case RCLKCHK:
			regs->rclkchk = data;
			break;
		case REVTCTL:
			regs->revtctl = data;
			break;
		case XGBLCTL:
			regs->xgblctl = data;
			break;
		case XMASK:
			regs->xmask = data;
			break;
		case XFMT:
			regs->xfmt = data;
			break;
		case AFSXCTL:
			regs->afsxctl = data;
			break;
		case ACLKXCTL:
			regs->aclkxctl = data;
			break;
		case AHCLKXCTL:
			regs->ahclkxctl = data;
			break;
		case XTDM:
			regs->xtdm = data;
			break;
		case XINTCTL:
			regs->xintctl = data;
			break;
		case XSTAT:
			regs->xstat = data;
			break;
		case XSLOT:
			regs->xslot = data;
			break;
		case XCLKCHK:
			regs->xclkchk = data;
			break;
		case XEVTCTL:
			regs->xevtctl = data;
			break;
		case AFIFOREV:
			regs->afiforev = data;
			break;
		case WFIFOCTL:
			regs->wfifoctl = data;
			dev->write_count = (data >> 8) & 0xff;
			break;
		case WFIFOSTS:
			regs->wfifosts = data;
			break;
		case RFIFOCTL:
			regs->rfifoctl = data;
			dev->read_count = (data >> 8) & 0xff;
			break;
		case RFIFOSTS:
			regs->rfifosts = data;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__,"Can not write the register at 0x%x in mcasp_omapl138\n", offset);
			break;
	}
	return No_exp;
}

exception_t mcasp_transmit_complete(conf_object_t* conf_obj, size_t length){
	mcasp_omapl138_device *dev= conf_obj->obj;

	if (dev->sound_iface != NULL && dev->transfer_type == WRITE_TYPE)
		dev->sound_iface->write_sound(dev->sound_obj, dev->xbuf, length);	
	
	return No_exp;
}

exception_t mcasp_strat_complete(conf_object_t* conf_obj)
{
	mcasp_omapl138_device *dev= conf_obj->obj;
    
    skyeye_log(Error_log, __FUNCTION__,"%s:This feature is not implemented \n",__func__);
    return No_exp;
}

static char * mcasp_omapl138_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	mcasp_omapl138_device *dev= conf_obj->obj;
	return dev->regs_name[id];
}

uint32_t mcasp_omapl138_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	mcasp_omapl138_device *dev= conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static char* mcasp_omapl138_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	mcasp_omapl138_device *dev = conf_obj->obj;
	mcasp_omapl138_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t mcasp_omapl138_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(mcasp_omapl138_reg_t) / 4;
}

static uint32_t mcasp_omapl138_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	mcasp_omapl138_device *dev = conf_obj->obj;
	mcasp_omapl138_reg_t *regs = (dev->regs);
	uint32_t regnum = sizeof(mcasp_omapl138_reg_t) / 4;
	int i;
	for(i =0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_mcasp_omapl138(char* obj_name) {
	mcasp_omapl138_device* dev = skyeye_mm_zero(sizeof(mcasp_omapl138_device));
	mcasp_omapl138_reg_t* regs =  skyeye_mm_zero(sizeof(mcasp_omapl138_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init mcasp_omapl138 regs */
	dev->regs = regs;
	dev->regs->rev = 0x44300a02;
	dev->regs->ahclkrctl = 0x8000;
	dev->regs->aclkxctl = 0x60;
	dev->regs->ahclkxctl = 0x8000;
	dev->regs->xslot = 0x17f;
	dev->regs->afiforev = 0x44311100;
	dev->regs->wfifoctl = 0x1004;
	dev->regs->rfifoctl = 0x1004;
	dev->regs_name = &regs_name;

	return dev->obj;
}

exception_t reset_mcasp_omapl138(conf_object_t* opaque, const char* parameters) {
	mcasp_omapl138_device *dev = opaque->obj;
	//mcasp_omapl138_reg_t* regs = dev->regs; 
	memset(dev->regs, 0, sizeof(mcasp_omapl138_reg_t));
	
	return No_exp;
}

exception_t free_mcasp_omapl138(conf_object_t* opaque) {
	mcasp_omapl138_device *dev = opaque->obj;

	skyeye_free(dev->regs);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	mcasp_omapl138_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf **)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

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
	mcasp_omapl138_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	mcasp_omapl138_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	mcasp_omapl138_device *dev = conf_obj->obj;
	dev->int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static exception_t get_attr_i2c_master(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	mcasp_omapl138_device *dev = obj->obj;
	*obj2 = dev->i2c_bus_obj;
	*port = NULL;
	return No_exp;
}
static exception_t set_attr_i2c_master(conf_object_t *obj, conf_object_t *obj2, const char *port, int index){
	mcasp_omapl138_device *dev = obj->obj;
	dev->i2c_bus_obj = obj2;
	dev->i2c_bus_iface = (i2c_bus_intf *)SKY_get_iface(dev->i2c_bus_obj, I2C_BUS_INTF_NAME);

	if (dev->i2c_bus_iface == NULL){
		if (dev->i2c_bus_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", I2C_BUS_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", I2C_BUS_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
static exception_t get_sound(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	mcasp_omapl138_device *dev = obj->obj;
	*obj2 = dev->sound_obj;
	*port = NULL;
	return No_exp;
}
static exception_t set_sound(conf_object_t *obj, conf_object_t *obj2, const char *port, int index){
	mcasp_omapl138_device *dev = obj->obj;
	dev->sound_obj = obj2;
	dev->sound_iface = (sound_intf *)SKY_get_iface(dev->sound_obj, SOUND_INTF);
	if (dev->sound_iface == NULL){
		if (dev->sound_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SOUND_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SOUND_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

void mcasp_omapl138_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", " mcasp_omapl138 interrupt number");
}


void init_mcasp_omapl138(){
	static skyeye_class_t class_data = {
		.class_name = "omapl138_mcasp",
		.class_desc = "omapl138_mcasp",
		.new_instance = new_mcasp_omapl138,
		.free_instance = free_mcasp_omapl138,
		.reset_instance = reset_mcasp_omapl138,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_omapl138),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = mcasp_omapl138_read,
		.write = mcasp_omapl138_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = mcasp_omapl138_get_regvalue_by_id,
		.get_regname_by_id = mcasp_omapl138_get_regname_by_id,
		.set_regvalue_by_id = mcasp_omapl138_set_regvalue_by_id,
		.get_regnum = mcasp_omapl138_get_regnum,
		.get_regid_by_name = mcasp_omapl138_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const edma_dev_intf edma_intf = {
		.transmit_complete = mcasp_transmit_complete,
        .start_dma_transfer = mcasp_strat_complete,
	};
	SKY_register_iface(clss, EDMA_DEV_INTF, &edma_intf);
	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF, 
			.iface = &reg_intf,
		},
		(struct InterfaceDescription) {
			.name = EDMA_DEV_INTF, 
			.iface = &edma_intf,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
		(struct ConnectDescription) {
			.name = I2C_BUS_INTF_NAME,
			.set = set_attr_i2c_master,
			.get = get_attr_i2c_master,
		},
		(struct ConnectDescription) {
			.name = SOUND_INTF,
			.set = set_sound,
			.get = get_sound,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	mcasp_omapl138_register_attribute(clss);
}

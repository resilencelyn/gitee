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
* @file mcbsp_c5510.c
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

#include "mcbsp_c5510.h"

static char* c5510_uart_attr[] = {"term", "signal"};

void uart_do_cycle(conf_object_t* opaque)
{
	mcbsp_c5510_device *dev = opaque->obj;
	mcbsp_c5510_reg_t* regs = dev->regs;
	exception_t ret = 0;
	unsigned char buf;
	if (regs->SPCR10.bit.RRST){

		if (dev->term_iface)
		{
			if ((ret = (dev->term_iface->read(dev->term, &buf, 1)) != 0)) {
				regs->SPCR10.bit.RRDY |= 0x10;
			}
		}
	}
}

static exception_t mcbsp_c5510_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	mcbsp_c5510_device *dev = opaque->obj;
	mcbsp_c5510_reg_t* regs = dev->regs;

	switch(offset) {
		case DRR2:
			*(uint16_t*)buf = regs->DRR20;
			break;
		case DRR1:
			*(uint16_t*)buf = regs->DRR10;
			break;
		case DXR2:
			*(uint16_t*)buf = regs->DXR20;
			break;
		case DXR1:
			*(uint16_t*)buf = regs->DXR10;
			break;
		case SPCR2:
			*(uint16_t*)buf = regs->SPCR20.all;
			break;
		case SPCR1:
			*(uint16_t*)buf = regs->SPCR10.all;
			break;
		case RCR2:
			*(uint16_t*)buf = regs->RCR20;
			break;
		case RCR1:
			*(uint16_t*)buf = regs->RCR10;
			break;
		case XCR2:
			*(uint16_t*)buf = regs->XCR20.all;
			break;
		case XCR1:
			*(uint16_t*)buf = regs->XCR10.all;
			break;
		case SRGR2:
			*(uint16_t*)buf = regs->SRGR20;
			break;
		case SRGR1:
			*(uint16_t*)buf = regs->SRGR10;
			break;
		case MCR2:
			*(uint16_t*)buf = regs->MCR20;
			break;
		case MCR1:
			*(uint16_t*)buf = regs->MCR10;
			break;
		case RCERA:
			*(uint16_t*)buf = regs->RCERA0;
			break;
		case RCERB:
			*(uint16_t*)buf = regs->RCERB0;
			break;
		case XCERA:
			*(uint16_t*)buf = regs->XCERA0;
			break;
		case XCERB:
			*(uint16_t*)buf = regs->XCERB0;
			break;
		case PCR:
			*(uint16_t*)buf = regs->PCR0;
			break;
		case RCERC:
			*(uint16_t*)buf = regs->RCERC0;
			break;
		case RCERD:
			*(uint16_t*)buf = regs->RCERD0;
			break;
		case XCERC:
			*(uint16_t*)buf = regs->XCERC0;
			break;
		case XCERD:
			*(uint16_t*)buf = regs->XCERD0;
			break;
		case RCERE:
			*(uint16_t*)buf = regs->RCERE0;
			break;
		case RCERF:
			*(uint16_t*)buf = regs->RCERF0;
			break;
		case XCERE:
			*(uint16_t*)buf = regs->XCERE0;
			break;
		case XCERF:
			*(uint16_t*)buf = regs->XCERF0;
			break;
		case RCERG:
			*(uint16_t*)buf = regs->RCERG0;
			break;
		case RCERH:
			*(uint16_t*)buf = regs->RCERH0;
			break;
		case XCERG:
			*(uint16_t*)buf = regs->XCERG0;
			break;
		case XCERH:
			*(uint16_t*)buf = regs->XCERH0;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__,"Can not read the register at 0x%x in mcbsp_c5510\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

void get_transmit_frame_word_length(mcbsp_c5510_device *dev, int flag){
	mcbsp_c5510_reg_t* regs = dev->regs;
	if (flag == 1){
		switch(regs->XCR10.bit.XWDLEN1){
		case 0:
			dev->send_word1_length = 8;
			break;
		case 1:
			dev->send_word1_length = 12;
			break;
		case 2:
			dev->send_word1_length = 16;
			break;
		case 3:
			dev->send_word1_length = 20;
			break;
		case 4:
			dev->send_word1_length = 24;
			break;
		case 5:
			dev->send_word1_length = 32;
			break;
		default:
			break;
		}
	}else {
		switch(regs->XCR20.bit.XWDLEN2){
		case 0:
			dev->send_word2_length = 8;
			break;
		case 1:
			dev->send_word2_length = 12;
			break;
		case 2:
			dev->send_word2_length = 16;
			break;
		case 3:
			dev->send_word2_length = 20;
			break;
		case 4:
			dev->send_word2_length = 24;
			break;
		case 5:
			dev->send_word2_length = 32;
			break;
		default:
			break;
		}
	}
	if (regs->XCR20.bit.XPHASE){
		dev->send_frame_length = regs->XCR20.bit.XFRLEN2 + regs->XCR10.bit.XFRLEN1 + 2;
	}else 
		dev->send_frame_length = regs->XCR10.bit.XFRLEN1 + 1;
}
void get_receive_frame_word_length(mcbsp_c5510_device *dev, int flag){
	mcbsp_c5510_reg_t* regs = dev->regs;
	if (flag == 1){
		switch(regs->XCR10.bit.XWDLEN1){
		case 0:
			dev->recv_word1_length = 8;
			break;
		case 1:
			dev->recv_word1_length = 12;
			break;
		case 2:
			dev->recv_word1_length = 16;
			break;
		case 3:
			dev->recv_word1_length = 20;
			break;
		case 4:
			dev->recv_word1_length = 24;
			break;
		case 5:
			dev->recv_word1_length = 32;
			break;
		default:
			break;
		}
	}else {
		switch(regs->XCR20.bit.XWDLEN2){
		case 0:
			dev->recv_word2_length = 8;
			break;
		case 1:
			dev->recv_word2_length = 12;
			break;
		case 2:
			dev->recv_word2_length = 16;
			break;
		case 3:
			dev->recv_word2_length = 20;
			break;
		case 4:
			dev->recv_word2_length = 24;
			break;
		case 5:
			dev->recv_word2_length = 32;
			break;
		default:
			break;
		}
	}
	if (regs->XCR20.bit.XPHASE){
		dev->recv_frame_length = regs->XCR20.bit.XFRLEN2 + regs->XCR10.bit.XFRLEN1 + 2;
	}else 
		dev->recv_frame_length = regs->XCR10.bit.XFRLEN1 + 1;
}
static exception_t mcbsp_c5510_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	mcbsp_c5510_device *dev = opaque->obj;
	mcbsp_c5510_reg_t* regs = dev->regs;
	uint16_t data = *buf;
	char c;

	switch(offset) {
		case DRR2:
			regs->DRR20 = data;
			break;
		case DRR1:
			regs->DRR10 = data;
			break;
		case DXR2:
			regs->DXR20 = data;
			if (regs->XCR20.bit.XCOMPAND){
				regs->DXR20 = data = (data >> 8) | (data << 8);
			}
			c = data;
			if (regs->SPCR20.bit.XRST){
				if(dev->term_iface != NULL){
					dev->term_iface->write(dev->term, &c, 1);
				}
			}
			break;
		case DXR1:
			regs->DXR10 = data;
			c = data;
			if (regs->SPCR20.bit.XRST){
				if(dev->term_iface != NULL){
					dev->term_iface->write(dev->term, &c, 1);
				}
			}
			break;
		case SPCR2:
			regs->SPCR20.all = data;
			break;
		case SPCR1:
			regs->SPCR10.all = data;
			break;
		case RCR2:
			regs->RCR20 = data;
			break;
		case RCR1:
			regs->RCR10 = data;
			break;
		case XCR2:
			regs->XCR20.all = data;
			get_transmit_frame_word_length(dev, 2);
			break;
		case XCR1:
			regs->XCR10.all = data;
			get_transmit_frame_word_length(dev, 1);
			break;
		case SRGR2:
			regs->SRGR20 = data;
			break;
		case SRGR1:
			regs->SRGR10 = data;
			break;
		case MCR2:
			regs->MCR20 = data;
			break;
		case MCR1:
			regs->MCR10 = data;
			break;
		case RCERA:
			regs->RCERA0 = data;
			break;
		case RCERB:
			regs->RCERB0 = data;
			break;
		case XCERA:
			regs->XCERA0 = data;
			break;
		case XCERB:
			regs->XCERB0 = data;
			break;
		case PCR:
			regs->PCR0 = data;
			break;
		case RCERC:
			regs->RCERC0 = data;
			break;
		case RCERD:
			regs->RCERD0 = data;
			break;
		case XCERC:
			regs->XCERC0 = data;
			break;
		case XCERD:
			regs->XCERD0 = data;
			break;
		case RCERE:
			regs->RCERE0 = data;
			break;
		case RCERF:
			regs->RCERF0 = data;
			break;
		case XCERE:
			regs->XCERE0 = data;
			break;
		case XCERF:
			regs->XCERF0 = data;
			break;
		case RCERG:
			regs->RCERG0 = data;
			break;
		case RCERH:
			regs->RCERH0 = data;
			break;
		case XCERG:
			regs->XCERG0 = data;
			break;
		case XCERH:
			regs->XCERH0 = data;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__,"Can not write the register at 0x%x in mcbsp_c5510\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}
static char * mcbsp_c5510_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	mcbsp_c5510_device *dev= conf_obj->obj;
	return dev->regs_name[id];
}

uint32_t mcbsp_c5510_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	mcbsp_c5510_device *dev= conf_obj->obj;
	uint16_t* regs_value = (uint16_t*)(dev->regs) + id;
	return *regs_value;
}

static char* mcbsp_c5510_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	mcbsp_c5510_device *dev = conf_obj->obj;
	mcbsp_c5510_reg_t *regs = (dev->regs);
	*((uint16_t*)regs + id) = value;

	return NULL;
}

static uint32_t mcbsp_c5510_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  REG_MAX_NUM;
}

static uint32_t mcbsp_c5510_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	mcbsp_c5510_device *dev = conf_obj->obj;
	mcbsp_c5510_reg_t *regs = (dev->regs);
	uint32_t regnum;
	regnum = REG_MAX_NUM;
	int i;
	for(i =0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t mcbsp_c5510_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static conf_object_t* new_mcbsp_c5510(char* obj_name) {
	mcbsp_c5510_device* dev = skyeye_mm_zero(sizeof(mcbsp_c5510_device));
	mcbsp_c5510_reg_t* regs =  skyeye_mm_zero(sizeof(mcbsp_c5510_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init mcbsp_c5510 regs */
	dev->regs = regs;
	dev->regs_name = &regs_name;
	int  pthread_id;
	create_thread_scheduler(1000, Periodic_sched, uart_do_cycle, dev->obj, &pthread_id);

	return dev->obj;
}

exception_t reset_mcbsp_c5510(conf_object_t* opaque, const char* parameters) {
	mcbsp_c5510_device *dev = opaque->obj;
	mcbsp_c5510_reg_t* regs = dev->regs; 
	
	return No_exp;
}

exception_t free_mcbsp_c5510(conf_object_t* opaque) {
	mcbsp_c5510_device *dev = opaque->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return No_exp;
}

static exception_t term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	mcbsp_c5510_device *dev = obj->obj;
	dev->term = obj2;
	dev->term_iface = (memory_space_intf*)SKY_get_iface(dev->term, SKYEYE_UART_INTF);
	if (dev->term_iface == NULL){
		if (dev->term == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_UART_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);

		return Not_found_exp;
	}
	
	return No_exp;
}

static exception_t term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	mcbsp_c5510_device *dev = obj->obj;
	*obj2 = dev->term;
	*port = NULL;
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	mcbsp_c5510_device *dev = obj->obj;
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
	mcbsp_c5510_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
/*
void mcbsp_c5510_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "rfifo", get_attr_rfifo, NULL, set_attr_rfifo, NULL, SKY_Attr_Optional, "uinteger", "fifo length for s3c2410x uart");
}
*/
void mcbsp_c5510_register_interface(conf_class_t* clss) {
	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = mcbsp_c5510_get_regvalue_by_id,
		.get_regname_by_id = mcbsp_c5510_get_regname_by_id,
		.set_regvalue_by_id = mcbsp_c5510_set_regvalue_by_id,
		.get_regnum = mcbsp_c5510_get_regnum,
		.get_regid_by_name = mcbsp_c5510_get_regid_by_name,
		.get_regoffset_by_id = mcbsp_c5510_get_regoffset_by_id,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
}
void init_mcbsp_c5510(){
	static skyeye_class_t class_data = {
		.class_name = "c5510_mcbsp",
		.class_desc = "c5510_mcbsp",
		.new_instance = new_mcbsp_c5510,
		.free_instance = free_mcbsp_c5510,
		.reset_instance = reset_mcbsp_c5510,
		.get_attr = NULL,
		.set_attr = NULL 
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = mcbsp_c5510_read,
		.write = mcbsp_c5510_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = mcbsp_c5510_get_regvalue_by_id,
		.get_regname_by_id = mcbsp_c5510_get_regname_by_id,
		.set_regvalue_by_id = mcbsp_c5510_set_regvalue_by_id,
		.get_regnum = mcbsp_c5510_get_regnum,
		.get_regid_by_name = mcbsp_c5510_get_regid_by_name,
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
		(struct ConnectDescription ){
			.name = SKYEYE_UART_INTF,
			.set = term_set,
			.get = term_get,
		},
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

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
* @file adk_rs422.c
* @brief The implementation of system controller
* @author 
* @version 78.77
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
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#include <skyeye_data_intf.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_uart.h>
#include <skyeye_emif.h>
#include "adk_rs422.h"

static int adk_rs422_set_state(conf_object_t* obj, char *buf, int Length)
{
	adk_rs422_device *dev = obj->obj;
	adk_rs422_reg_t* regs = dev->regs;
	
    /* 接收串口通道号和本地串口通道一致则开始接收 */
    if (dev->Rs422_ID == buf[0]){ 
        dev->fifo.recv_buf[0] = Length;
	    memcpy(&dev->fifo.recv_buf[1], &buf[1], Length);
	    regs->stu |= 1; //接收完毕
    }

	return OK;
}

static exception_t adk_rs422_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct adk_rs422_device *dev = opaque->obj;
	adk_rs422_reg_t* regs = dev->regs;
    if (offset >= RX_FIFO_REG && offset < TX_FIFO_REG){
        int m;
        m = (offset - RX_FIFO_REG) / 4;
		*(uint32_t *)buf = dev->fifo.recv_buf[m];
        return No_exp;
    }
	switch(offset) {
	case TX_FIFO_REG:
		*(uint32_t *)buf = regs->tx_fifo;
		break;
	case UCR_REG:
		*(uint32_t *)buf = regs->ucr;
		break;
	case MCR_REG:
		*(uint32_t *)buf = regs->mcr;
		break;
	case BRSR_REG:
		*(uint32_t *)buf = regs->brsr;
		break;
	case ICR_REG:
		*(uint32_t *)buf = regs->icr;
		break;
	case CMD_REG:
		*(uint32_t *)buf = regs->cmd;
		break;
	case STU_REG:
		*(uint32_t *)buf = regs->stu;
		break;
	case WRONG_STU_REG:
		*(uint32_t *)buf = regs->wrong_stu;
		break;
	case TX_FRAME_HEAD_2:
		*(uint32_t *)buf = regs->tx_frame_head2;
		break;
	case TX_FRAME_HEAD_1:
		*(uint32_t *)buf = regs->tx_frame_head1;
		break;
	case RX_FRAME_HEAD_2:
		*(uint32_t *)buf = regs->rx_frame_head2;
		break;
	case RX_FRAME_HEAD_1:
		*(uint32_t *)buf = regs->rx_frame_head1;
		break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in adk_rs422\n", offset);
		return Invarg_exp;
	}
	return No_exp;
}

static exception_t adk_rs422_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct adk_rs422_device *dev = opaque->obj;
	adk_rs422_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
    char send_buffer[64] = {0};
    int length;

    if (offset >= TX_FIFO_REG && offset < UCR_REG){
        int m = (offset - TX_FIFO_REG) / 4;
		dev->fifo.send_buf[m] = val;
        return No_exp;
    }
	switch(offset) {
	case RX_FIFO_REG:
		regs->rx_fifo = val;
		break;
	case UCR_REG:
		regs->ucr = val;
		break;
	case MCR_REG:
		regs->mcr = val;
		break;
	case BRSR_REG:
		regs->brsr = val;
		break;
	case ICR_REG:
		regs->icr = val;
		break;
	case CMD_REG:
		regs->cmd = val;
        if (regs->cmd == SEND_CMD){ //start send 
			if (dev->bus_iface != NULL){
                length = dev->fifo.send_buf[0];
                send_buffer[0] = dev->Rs422_ID; //本地串口通道号
                memcpy(&send_buffer[1], &dev->fifo.send_buf[1], length);
				dev->bus_iface->write_data(dev->bus_obj, opaque, send_buffer, length);
			}
		}else if (regs->cmd == RECV_CMD){ //clear recv
			regs->stu = 2; //接收完毕
		}
		break;
	case STU_REG:
		regs->stu = val;
		break;
	case WRONG_STU_REG:
		regs->wrong_stu = val;
		break;
	case TX_FRAME_HEAD_2:
		regs->tx_frame_head2 = val;
		break;
	case TX_FRAME_HEAD_1:
		regs->tx_frame_head1 = val;
		break;
	case RX_FRAME_HEAD_2:
		regs->rx_frame_head2 = val;
		break;
	case RX_FRAME_HEAD_1:
		regs->rx_frame_head1 = val;
		
		break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in adk_rs422\n", offset);
		return Invarg_exp;
	}
	return No_exp;
}

static char* adk_rs422_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t adk_rs422_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	adk_rs422_device *dev = conf_obj->obj;
	adk_rs422_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static char* adk_rs422_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	adk_rs422_device *dev = conf_obj->obj;
	adk_rs422_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t adk_rs422_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	adk_rs422_device *dev = conf_obj->obj;
	adk_rs422_reg_t* regs = dev->regs;
	return  sizeof(adk_rs422_reg_t) / 4;

}

static uint32_t adk_rs422_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	adk_rs422_device *dev = conf_obj->obj;
	adk_rs422_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(adk_rs422_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;

}

static conf_object_t* new_adk_rs422(char* obj_name){
	adk_rs422_device* dev = skyeye_mm_zero(sizeof(adk_rs422_device));
	adk_rs422_reg_t* regs =  skyeye_mm_zero(sizeof(adk_rs422_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init adk_rs422 regs */
	dev->regs = regs;

	return dev->obj;
}
static void free_adk_rs422(conf_object_t* conf_obj){
	adk_rs422_device *dev = conf_obj->obj;
    skyeye_free(dev->regs);
    skyeye_free(dev->obj);
    skyeye_free(dev);
	return;	
}

static exception_t adk_rs422_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	adk_rs422_device* dev = (adk_rs422_device*)(obj->obj);
	dev->bus_obj = obj2;
	dev->bus_iface = (Rs422_bus_intf*)SKY_get_iface(dev->bus_obj, RS422_BUS_INTF);

	if (dev->bus_iface == NULL){
		if (dev->bus_obj == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Get 'rs422_bus_iface' interface from NULL object fail.", obj2);
		else
			skyeye_log(Error_log, __FUNCTION__, "Get 'rs422_bus_iface' interface from %s fail.", obj2->objname);
	}
	if (Error == dev->bus_iface->register_device(obj2, obj)){
		printf("In %s, Register adk_rs422 device Error.\n", __FUNCTION__);
	}
	return No_exp;

}
 
static exception_t adk_rs422_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	adk_rs422_device* dev = (adk_rs422_device*)(obj->obj);
	*obj2 = dev->bus_obj;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_Rs422_ID(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	adk_rs422_device* dev = (adk_rs422_device*)(conf_obj->obj);
	attr_value_t Rs422_ID = SKY_make_attr_uinteger(dev->Rs422_ID);
	return Rs422_ID;
}

static exception_t set_attr_Rs422_ID(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	adk_rs422_device* dev = (adk_rs422_device*)(conf_obj->obj);
	dev->Rs422_ID = SKY_attr_uinteger(*value);
	return No_exp;
}

static adk_rs422_register_attribute(conf_class_t* clss){
    SKY_register_attribute(clss, "Rs422_ID", get_attr_Rs422_ID, NULL, set_attr_Rs422_ID, NULL, SKY_Attr_Optional, "uinteger", "ID of Rs422");
}

void init_adk_rs422(){
	static skyeye_class_t class_data = {
		.class_name = "adk_rs422",
		.class_desc = "adk_rs422",
		.new_instance = new_adk_rs422,
		.free_instance = free_adk_rs422,
		.get_attr = NULL,
		.set_attr = NULL 
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = adk_rs422_read,
		.write = adk_rs422_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const Rs422_emif_intf rs422_emif_interface = {
		.set_state = adk_rs422_set_state,
	};
	SKY_register_iface(clss, RS422_EMIF_INTF, &rs422_emif_interface);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = adk_rs422_get_regvalue_by_id,
		.get_regname_by_id = adk_rs422_get_regname_by_id,
		.set_regvalue_by_id = adk_rs422_set_regvalue_by_id,
		.get_regnum = adk_rs422_get_regnum,
		.get_regid_by_name = adk_rs422_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME, 
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = RS422_EMIF_INTF,
			.iface =  &rs422_emif_interface,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = RS422_BUS_INTF,
			.set = adk_rs422_set,
			.get = adk_rs422_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	adk_rs422_register_attribute(clss);
}

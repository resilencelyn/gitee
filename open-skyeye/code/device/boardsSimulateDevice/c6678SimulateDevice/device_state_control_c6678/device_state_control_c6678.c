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
* @file gpio_c6713.c
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
#define DEBUG
#include <skyeye_log.h>

#include "device_state_control_c6678.h"

static char* device_state_control_c6678_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t device_state_control_c6678_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	device_state_control_c6678_device *dev = conf_obj->obj;
	device_state_control_c6678_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t device_state_control_c6678_set_regvalue_by_id(conf_object_t* conf_obj, int32_t value, uint32_t id){
	device_state_control_c6678_device *dev = conf_obj->obj;
	device_state_control_c6678_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;
	return 0;
}

uint32_t device_state_control_c6678_get_regid_by_name(conf_object_t* opaque, char* name){
	uint32_t regnum = 0, i = 0;
	device_state_control_c6678_device *dev = opaque->obj;
	device_state_control_c6678_reg_t* regs = dev->regs;
	regnum = sizeof(device_state_control_c6678_reg_t)/ 4;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

uint32_t device_state_control_c6678_get_regnum(conf_object_t* opaque){
	device_state_control_c6678_device *dev = opaque->obj;
	device_state_control_c6678_reg_t* regs = dev->regs;
	return  sizeof(device_state_control_c6678_reg_t) / 4;
}

void call_signal_func(conf_object_t* opaque, int int_number,int core_number){
	device_state_control_c6678_device *dev = opaque->obj;
	device_state_control_c6678_reg_t* regs = dev->regs;
	general_signal_intf* general_signal = dev->general_signal;

	if (general_signal)
		general_signal->event_signal(dev->core, int_number,&core_number);

	return;
}
static exception_t device_state_control_c6678_read(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	device_state_control_c6678_device *dev = opaque->obj;
	device_state_control_c6678_reg_t* regs = dev->regs;

    //printf("read ipc %x\n",offset);
	switch (offset) {
		case 0x0:
            *buf = regs->ipcgr[0] | 0xfffffff0; 
			break;
		case 0x4:
            *buf = regs->ipcgr[1] | 0xfffffff0; 
			break;
		case 0x8:
            *buf = regs->ipcgr[2] | 0xfffffff0; 
			break;
		case 0xc:
            *buf = regs->ipcgr[3] | 0xfffffff0; 
			break;
		case 0x10:
            *buf = regs->ipcgr[4] | 0xfffffff0; 
			break;
		case 0x14:
            *buf = regs->ipcgr[5] | 0xfffffff0; 
			break;
		case 0x18:
            *buf = regs->ipcgr[6] | 0xfffffff0; 
			break;
		case 0x1c:
            *buf = regs->ipcgr[7] | 0xfffffff0; 
			break;
        case 0x40:
            *buf = regs->ipcar[0] | 0xfffffff0; 
			break;
		case 0x44:
            *buf = regs->ipcar[1] | 0xfffffff0; 
			break;
		case 0x48:
            *buf = regs->ipcar[2] | 0xfffffff0; 
			break;
		case 0x4c:
            *buf = regs->ipcar[3] | 0xfffffff0; 
			break;
		case 0x50:
            *buf = regs->ipcar[4] | 0xfffffff0; 
			break;
		case 0x54:
            *buf = regs->ipcar[5] | 0xfffffff0; 
			break;
		case 0x58:
            *buf = regs->ipcar[6] | 0xfffffff0; 
			break;
		case 0x5c:
            *buf = regs->ipcar[7] | 0xfffffff0; 
			break;
		default:
			printf("In %s, read error value from offset 0x%x\n", __FUNCTION__, offset);
			break;
	}

	return No_exp;
}

static exception_t device_state_control_c6678_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	device_state_control_c6678_device *dev = opaque->obj;
	device_state_control_c6678_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*) buf;
	int core_number = -1;
    int int_number = dev->interrupt_number;
 
    //printf("write ipc %x\n",offset);
	switch (offset) {
		case 0x0:
            regs->ipcgr[0] |= *(uint32_t *)buf ; 
            regs->ipcar[0] |= (*(uint32_t *)buf | 0xfffffff0); 
            core_number  = (regs->ipcgr[0] & 0x1) ? 0 : -1;
			break;
		case 0x4:
            regs->ipcgr[1] |= *(uint32_t *)buf ; 
            regs->ipcar[1] |= (*(uint32_t *)buf | 0xfffffff0); 
            core_number  = (regs->ipcgr[1] & 0x1) ? 1 : -1;
			break;
		case 0x8:
            regs->ipcgr[2] |= *(uint32_t *)buf ; 
            regs->ipcar[2] |= (*(uint32_t *)buf | 0xfffffff0); 
            core_number  = (regs->ipcgr[2] & 0x1) ? 2 : -1;
			break;
		case 0xc:
            regs->ipcgr[3] |= *(uint32_t *)buf ; 
            regs->ipcar[3] |= (*(uint32_t *)buf | 0xfffffff0); 
            core_number  = (regs->ipcgr[3] & 0x1) ? 3 : -1;
			break;
		case 0x10:
            regs->ipcgr[4] |= *(uint32_t *)buf ; 
            regs->ipcar[4] |= (*(uint32_t *)buf | 0xfffffff0); 
            core_number  = (regs->ipcgr[4] & 0x1) ? 4 : -1;
			break;
		case 0x14:
            regs->ipcgr[5] |= *(uint32_t *)buf ; 
            regs->ipcar[5] |= (*(uint32_t *)buf | 0xfffffff0); 
            core_number  = (regs->ipcgr[5] & 0x1) ? 5 : -1;
			break;
		case 0x18:
            regs->ipcgr[6] |= *(uint32_t *)buf ; 
            regs->ipcar[6] |= (*(uint32_t *)buf | 0xfffffff0); 
            core_number  = (regs->ipcgr[6] & 0x1) ? 6 : -1;
			break;
		case 0x1c:
            regs->ipcgr[7] |= *(uint32_t *)buf ; 
            regs->ipcar[7] |= (*(uint32_t *)buf | 0xfffffff0); 
            core_number  = (regs->ipcgr[7] & 0x1) ? 7 : -1;
			break;
        case 0x40:
            regs->ipcar[0] &= ~(*(uint32_t *)buf | 0xfffffff0); 
            regs->ipcgr[0] &= ~(*(uint32_t *)buf | 0xfffffff0); 
			break;
		case 0x44:
            regs->ipcar[1] &= ~(*(uint32_t *)buf | 0xfffffff0); 
            regs->ipcgr[1] &= ~(*(uint32_t *)buf | 0xfffffff0); 
			break;
		case 0x48:
            regs->ipcar[2] &= ~(*(uint32_t *)buf | 0xfffffff0); 
            regs->ipcgr[2] &= ~(*(uint32_t *)buf | 0xfffffff0); 
			break;
		case 0x4c:
            regs->ipcar[3] &= ~(*(uint32_t *)buf | 0xfffffff0); 
            regs->ipcgr[3] &= ~(*(uint32_t *)buf | 0xfffffff0); 
			break;
		case 0x50:
            regs->ipcar[4] &= ~(*(uint32_t *)buf | 0xfffffff0); 
            regs->ipcgr[4] &= ~(*(uint32_t *)buf | 0xfffffff0); 
			break;
		case 0x54:
            regs->ipcar[5] &= ~(*(uint32_t *)buf | 0xfffffff0); 
            regs->ipcgr[5] &= ~(*(uint32_t *)buf | 0xfffffff0); 
			break;
		case 0x58:
            regs->ipcar[6] &= ~(*(uint32_t *)buf | 0xfffffff0); 
            regs->ipcgr[6] &= ~(*(uint32_t *)buf | 0xfffffff0); 
			break;
		case 0x5c:
            regs->ipcar[7] &= ~(*(uint32_t *)buf | 0xfffffff0); 
            regs->ipcgr[7] &= ~(*(uint32_t *)buf | 0xfffffff0); 
			break;
		default:
			printf("In %s, read error value from offset 0x%x\n", __FUNCTION__, offset);
			break;
	}

    //when write ipcgr check interrupt  send event and clear interrupt bit
    if((offset >=0x0 && offset <=0x1c) && core_number != -1)
    {
        regs->ipcgr[core_number] |= 0xfffffff0;
        printf("do event_signal to core %d %d\n",core_number,int_number);
        call_signal_func(opaque,int_number,core_number);

    }
	return No_exp;
}

static conf_object_t* new_device_state_control_c6678(char* obj_name){
	device_state_control_c6678_device* dev = skyeye_mm_zero(sizeof(device_state_control_c6678_device));
	device_state_control_c6678_reg_t* regs =  skyeye_mm_zero(sizeof(device_state_control_c6678_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/*Init gpio regs*/
    int i =0;
    for(;i<8;i++)
    {
        regs->ipcgr[i] =0;
        regs->ipcar[i] = 0;
    }
     
    dev->regs = regs;

	return dev->obj;
}
static void free_device_state_control_c6678(conf_object_t* conf_obj){
	device_state_control_c6678_device* dev = conf_obj->obj;
	skyeye_free(dev->regs);
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return;	
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj,     attr_value_t* idx){
        device_state_control_c6678_device *dev = conf_obj->obj;
            attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
                return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj,      attr_value_t* value, attr_value_t* idx){
        device_state_control_c6678_device *dev = conf_obj->obj;
            dev->interrupt_number = SKY_attr_uinteger(*value);
                return No_exp;
}

static ipc_register_attribute(conf_class_t* clss){
        SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL,set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of  timer");
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	device_state_control_c6678_device *dev = obj->obj;
	dev->core = obj2;
	dev->general_signal = (general_signal_intf *)SKY_get_iface(dev->core, GENERAL_SIGNAL_INTF_NAME);

	return No_exp;
}
 
static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	device_state_control_c6678_device *dev = obj->obj;
	*obj2 = dev->core;
	*port = NULL;
	return No_exp;
}

void init_device_state_control_c6678(){
	static skyeye_class_t class_data = {
		.class_name = "c6678_device_state_control",
		.class_desc = "c6678_device_state_control",
		.new_instance = new_device_state_control_c6678,
		.free_instance = free_device_state_control_c6678,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_c6678),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = device_state_control_c6678_read,
		.write = device_state_control_c6678_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = device_state_control_c6678_get_regvalue_by_id,
		.get_regname_by_id = device_state_control_c6678_get_regname_by_id,
		.set_regvalue_by_id = device_state_control_c6678_set_regvalue_by_id,
		.get_regid_by_name = device_state_control_c6678_get_regid_by_name,
		.get_regnum = device_state_control_c6678_get_regnum
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
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

    ipc_register_attribute(clss);
}

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
* @file pie_c2812.c
* @brief The implementation of pie controller
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

#include "Pie_c2812.h"

void send_signal_thread(conf_object_t *opaque){
	pie_c2812_device *dev = opaque->obj;
    //irq_mesg_t irq_mesg;
	c28_signal_t c28_signal;
    int pie_x, pie_y, pie_group, type;

    if(CheckCanReadNum(dev->recv_fifo) == 0){
		return;
    }

    ReadFIFO(dev->recv_fifo, &dev->irq_mesg, 1);

    if (dev->irq_mesg.type == Non_Periph_Type){
        c28_signal.pie_group = dev->irq_mesg.pie_group;
        c28_signal.irq_type = dev->irq_mesg.type;
    }else if (dev->irq_mesg.type == Periph_Type){
        c28_signal.pie_x = dev->irq_mesg.pie_x;
        c28_signal.pie_y = dev->irq_mesg.pie_y;
        c28_signal.irq_type = dev->irq_mesg.type;
    }else {
        skyeye_log(Error_log, __FUNCTION__,"c28_signal type:%d not defined\n", c28_signal.irq_type);
    }

	if (dev->signal_iface){
		dev->signal_iface->signal(dev->signal, &c28_signal);
	}else{
		skyeye_log(Error_log, __FUNCTION__, "signal_iface Error.\n");
	}
}

static exception_t PieVect_Peripheral_c2812_raise_signal(conf_object_t *conf_obj, int irq_number)
{
	pie_c2812_device *dev = conf_obj->obj;
	pie_c2812_reg_t* regs = dev->regs;
	c28_signal_t c28_signal;
    //irq_mesg_t irq_mesg;
	int pie_x, pie_y;

    if (irq_number > 96){
        dev->irq_mesg.pie_group = irq_number / 8 - 1;
        dev->irq_mesg.type = Non_Periph_Type;
        RW_WRLOCK(dev->lock);
   	    WriteFIFO(dev->recv_fifo, &dev->irq_mesg, 1);
        RW_UNLOCK(dev->lock);
    }else {
	    if (irq_number == 8)
		    pie_x = irq_number / 8;
	    else 
		    pie_x = irq_number / 8 + 1;

	    pie_y = irq_number % 8;
	    if (irq_number > 0 && (irq_number % 8 == 0))
		    pie_y = 8;
	
	    regs->PIEIFR[pie_x].all |= (0x1 << (pie_y - 1));
	    if (regs->PIECTRL.bit.ENPIE){
		    if (regs->PIEIER[pie_x].all & (0x1 << (pie_y - 1))){
			    regs->PIEACK.all |= (0x1 << pie_x);
			    int i = 0, j;
			    for(; i < 12; i++){
				    for (j = 0; j < 8; j++){
					    c28_signal.intr[i][j] = Prev_level;
				    }
			    }

                dev->irq_mesg.pie_x = pie_x - 1;
                dev->irq_mesg.pie_y = pie_y - 1;
                dev->irq_mesg.type = Periph_Type;
                RW_WRLOCK(dev->lock);
   	            WriteFIFO(dev->recv_fifo, &dev->irq_mesg, 1);
                RW_UNLOCK(dev->lock);
		    }
	    }
    }
	return No_exp;
}

static exception_t PieVect_c2812_lower_signal(conf_object_t *conf_obj, int irq_number)
{
	pie_c2812_device *dev = conf_obj->obj;

	return No_exp;
}

static exception_t pie_c2812_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct pie_c2812_device *dev = opaque->obj;
	pie_c2812_reg_t* regs = dev->regs;
	switch(offset) {
		case 0x0:
			*(uint16_t *)buf = regs->PIECTRL.all;
			break;
		case 0x1:
			*(uint16_t *)buf = regs->PIEACK.all;
			break;
		case 0x2:
		case 0x4:
		case 0x6:
		case 0x8:
		case 0xa:
		case 0xc:
		case 0xe:
		case 0x10:
		case 0x12:
		case 0x14:
		case 0x16:
		case 0x18:
			*(uint16_t *)buf = regs->PIEIER[offset/2].all;
			break;
		case 0x3:
		case 0x5:
		case 0x7:
		case 0x9:
		case 0xb:
		case 0xd:
		case 0xf:
		case 0x11:
		case 0x13:
		case 0x15:
		case 0x17:
		case 0x19:
			*(uint16_t *)buf = regs->PIEIFR[(offset - 1)/2].all;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in pie_c2812\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}

static exception_t pie_c2812_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct pie_c2812_device *dev = opaque->obj;
	pie_c2812_reg_t* regs = dev->regs;
	uint16_t val = *(uint16_t*)buf;
	switch(offset) {
		case 0x0:
			regs->PIECTRL.all = val;
			break;
		case 0x1:
			regs->PIEACK.all = val;
//			regs->PIEACK.all &= ~val;
			break;
		case 0x2:
		case 0x4:
		case 0x6:
		case 0x8:
		case 0xa:
		case 0xc:
		case 0xe:
		case 0x10:
		case 0x12:
		case 0x14:
		case 0x16:
		case 0x18:
			regs->PIEIER[offset/2].all = val;
			break;
		case 0x3:
		case 0x5:
		case 0x7:
		case 0x9:
		case 0xb:
		case 0xd:
		case 0xf:
		case 0x11:
		case 0x13:
		case 0x15:
		case 0x17:
		case 0x19:
			regs->PIEIFR[(offset - 1)/2].all = val;
			break;

		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in pie_c2812\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}


static char* pie_c2812_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t pie_c2812_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	pie_c2812_device *dev = conf_obj->obj;
	pie_c2812_reg_t* regs = dev->regs;
	uint16_t* regs_value = (uint16_t*)regs + id;
	return *regs_value;
}

static char* pie_c2812_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	pie_c2812_device *dev = conf_obj->obj;
	pie_c2812_reg_t* regs = dev->regs;
	*((uint16_t*)regs + id) = value;

	return NULL;
}

static uint32_t pie_c2812_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	pie_c2812_device *dev = conf_obj->obj;
	pie_c2812_reg_t* regs = dev->regs;
	return  sizeof(pie_c2812_reg_t) / 2 - 2;
}

static uint32_t pie_c2812_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	pie_c2812_device *dev = conf_obj->obj;
	pie_c2812_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(pie_c2812_reg_t)/ 2 - 2;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}

	return regnum;
}

static uint32_t pie_c2812_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static conf_object_t* new_pie_c2812(char* obj_name){
	pie_c2812_device* dev = skyeye_mm_zero(sizeof(pie_c2812_device));
	pie_c2812_reg_t* regs =  skyeye_mm_zero(sizeof(pie_c2812_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init pie_c2812 regs */
	dev->regs = regs;
    dev->recv_fifo = CreateFIFO(4096);
	RWLOCK_INIT(dev->lock);

	return dev->obj;
}

void free_pie_c2812(conf_object_t* obj){
	pie_c2812_device *dev = obj->obj;
	skyeye_free(dev->regs);
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return;
}

void config_pie_c2812(conf_object_t* conf_obj){
    system_register_timer_handler(conf_obj, 500, (time_sched_t)send_signal_thread, SCHED_US|SCHED_NORMAL);
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct pie_c2812_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);
	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	struct pie_c2812_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void init_pie_c2812(){
	static skyeye_class_t class_data = {
		.class_name = "c2812_pie",
		.class_desc = "c2812_pie",
		.new_instance = new_pie_c2812,
		.free_instance = free_pie_c2812,
		.config_instance = config_pie_c2812,
		.get_attr = NULL,
		.set_attr = NULL
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = pie_c2812_read,
		.write = pie_c2812_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = pie_c2812_get_regvalue_by_id,
		.get_regname_by_id = pie_c2812_get_regname_by_id,
		.set_regvalue_by_id = pie_c2812_set_regvalue_by_id,
		.get_regnum = pie_c2812_get_regnum,
		.get_regid_by_name = pie_c2812_get_regid_by_name,
		.get_regoffset_by_id = pie_c2812_get_regoffset_by_id,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const general_signal_iface_t signal_iface = {
		.raise_signal = PieVect_Peripheral_c2812_raise_signal,
		.lower_signal = PieVect_c2812_lower_signal
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_iface);

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
			.name = GENERAL_SIGNAL_INTF_NAME, 
			.iface = &signal_iface,
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

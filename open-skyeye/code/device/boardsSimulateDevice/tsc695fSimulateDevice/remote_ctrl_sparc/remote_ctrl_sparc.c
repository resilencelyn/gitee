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
* @file remote_ctrl_sparc.c
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
#include <skyeye_core.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_swapendian.h>

#include "remote_ctrl_sparc.h"
#define CPU_INT2  10
#define SIZE 0x400000
#define FIFO_SIZE  22 
#define LENGTH  11 

static void data_recv_callback(conf_object_t *opaque){
	struct sparc_remote_ctrl_device *dev = opaque;
	remote_ctrl_reg_t* regs = dev->regs;
	if (dev->signal_iface){
			dev->signal_iface->raise_signal(dev->signal, CPU_INT2);
		}
	return ;
}
static void channel_data_recv_callback(conf_object_t *opaque){
	struct sparc_remote_ctrl_device *dev = opaque;
	remote_ctrl_reg_t* regs = dev->regs;
	if (regs->note_end != 0) //×¢ÊýÖÐ¶Ï½áÊø·ñ£¿
		return ;
	
	if (dev->signal_iface){
		dev->signal_iface->raise_signal(dev->signal, dev->int_number);
	}
	return ;
}
static void timer_callback(conf_object_t *opaque){
	struct sparc_remote_ctrl_device *dev = opaque;
	remote_ctrl_reg_t* regs = dev->regs;
	regs->timer_count += 1;
	if (regs->timer_count == 0xffff){
		regs->timer_count = 0;	
	}
	return ;
}
static exception_t sparc_remote_ctrl_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
//	skyeye_log(Error_log, __FUNCTION__, "offset :%x, buf :%x\n", offset, *(uint32_t *)buf);
	struct sparc_remote_ctrl_device *dev = opaque->obj;
	remote_ctrl_reg_t* regs = dev->regs;
	DBG("Jiachao ######################### In %s, offset=0x%x\n", __FUNCTION__, offset);
	switch(offset) {
		case 0x0:	
			*(uint32_t*)buf = regs->ycyk_state.value;
			break;			
	
	  	case 0x2:	
			*(uint32_t*)buf = regs->ycyk_state.value;
			break;			
	  	case 0x3:	
			*(uint32_t*)buf = regs->ycyk_state.value;
			break;			
	
		case 0x4:
			*(uint32_t*)buf = regs->channel_check;
			break;
		case 0x8:
			*(uint32_t*)buf = regs->channel_check;
			break;
		case 0x40:
			*(uint32_t*)buf = regs->dpsk_page;
			break;
		case 0x80:
			dev->channel_fifo[dev->length] = half_from_BE(dev->channel_fifo[dev->length]); 
			*(uint32_t*)buf = dev->channel_fifo[dev->length];
			dev->length++;
			if (dev->length == LENGTH){
				dev->length = 0;
			}
			break;
		case 0x84:
			dev->channel_fifo[dev->length] = half_from_BE(dev->channel_fifo[dev->length]); 
			*(uint32_t*)buf = dev->channel_fifo[dev->length];
			dev->length++;
			if (dev->length == LENGTH){
				dev->length = 0;
			}
			break;
		case 0x88:
			*(uint32_t*)buf = regs->note_end;
			break;
		case 0xc0:
			if(regs->start_recv == 0x55aa && (regs->ycyk_state.flag.tcu_data_status == 0))
				*(uint32_t*)buf = regs->TCUA_data_store;
			break;
		case 0xc4:
			if (regs->start_recv == 0x55aa && (regs->ycyk_state.flag.tcu_data_status == 0))
				*(uint32_t*)buf = regs->TCUB_data_store;
			break;
		case 0xc8:
			*(uint32_t*)buf = regs->start_recv;
			break;
		case 0xd0:
			*(uint32_t*)buf = regs->instr_send & 0xff;
			break;
		case 0xd4:
			*(uint32_t*)buf = regs->instr_send & 0xffff00 >> 8;
			break;
		case 0xd8:
			*(uint32_t*)buf = regs->instr_send;
			break;
		case 0x140:
			*(uint32_t*)buf = regs->timer_count;
			break;
		case 0x148:
			*(uint32_t*)buf = regs->exp_reg;
			break;
		default:
			if (offset >= 0x4000){
				offset -= 0x4000;
				memcpy(buf, dev->ptr + offset, 4);
			}else{ 
				skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in remote_ctrl\n", offset);
				*(uint32_t*)buf = 0;
				return No_exp;
			}
			//return Invarg_exp;
	}
	return No_exp;
}

static exception_t sparc_remote_ctrl_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{	
	//skyeye_log(Error_log, __FUNCTION__, "offset :%x, buf :%x\n", offset, *(uint32_t *)buf);
	struct sparc_remote_ctrl_device *dev = opaque->obj;
	remote_ctrl_reg_t* regs = dev->regs;
	DBG("Jiachao ######################### In %s, offset=0x%x\n", __FUNCTION__, offset);
	uint32_t val = *(uint32_t*)buf;
	switch(offset) {
		case 0x0:
			if (val == 1){
				regs->ycyk_state.flag.channel2_status = 1;
			}else 
				regs->ycyk_state.flag.channel1_status = 1;
			break;

		case 0x2:
			regs->ycyk_state.value = val;
			break;
		case 0x3:
			regs->ycyk_state.value = val;
			break;
		
		case 0x4:
			if (val == 1){
				regs->ycyk_state.flag.channel2_status = 1;
			}else 
				regs->ycyk_state.flag.channel1_status = 1;
			break;
		case 0x8:
			if (val == 1){
				regs->ycyk_state.flag.channel2_status = 1;
			}else 
				regs->ycyk_state.flag.channel1_status = 1;
			break;
		case 0x40:
			regs->dpsk_page = val;
			if (regs->ycyk_state.flag.cache){
				dev->dpsk_cache = 0x10004400;
			}else 
				dev->dpsk_cache = 0x10004000;
			break;
		case 0x80:
			regs->channel1_data = val;
			memcpy(dev->channel_fifo, buf, FIFO_SIZE);
			regs->note_end = 0;
			create_thread_scheduler((unsigned int)1000, Periodic_sched, channel_data_recv_callback, dev, &dev->timer_sched_id);
		case 0x84:
			regs->channel2_data = val;
			memcpy(dev->channel_fifo, buf, FIFO_SIZE);
			regs->note_end = 0;
			create_thread_scheduler((unsigned int)1000, Periodic_sched, channel_data_recv_callback, dev, &dev->timer_sched_id);
			break;
		case 0x88:
			regs->note_end = val;
			regs->ycyk_state.flag.channel1_status = 0;
			regs->ycyk_state.flag.channel2_status = 0;
			break;
		case 0xc0:
			regs->TCUA_data_store = val;
			break;
		case 0xc4:
			regs->TCUB_data_store = val;
			break;
		case 0xc8:
			regs->start_recv = val;
			break;
		case 0xd0:
			if (regs->ycyk_state.flag.tcu_instr_status == 0)
				regs->low_data = val;
			break;
		case 0xd4:
			if (regs->ycyk_state.flag.tcu_instr_status == 0)
				regs->high_data = val;
			break;
		case 0xd8:
			regs->instr_send = val;
			break;
		case 0x140:
			regs->timer_count = val;
			break;
		case 0x148:
			regs->exp_reg = val;
			break;
		default:
			if (offset >= 0x4000){
				offset -= 0x4000;
				if (offset == 0){
					dev->page_flag = 1;	
				}else if (offset == 0x400){
					dev->page_flag = 2;	
				}
				memcpy(dev->ptr + offset, buf, 4);
				if (dev->page_flag == 1){
					memcpy(dev->ptr + (0x2000 + dev->page0_cnt * 4 + 0x400 * (dev->off_cnt)), buf, 4);
					dev->page0_cnt += 1;
				}else if (dev->page_flag == 2){
					memcpy(dev->ptr + (0x2000 + dev->page1_cnt * 4 + 0x400 * (dev->off_cnt)), buf, 4);
					dev->page1_cnt += 1;
				}
				if (dev->page0_cnt == 0x100){
					dev->page0_cnt = 0;
					dev->off_cnt += 1;
					regs->ycyk_state.flag.cache = 0;
				}else if (dev->page1_cnt == 0x100){
					dev->page1_cnt = 0;
					dev->off_cnt += 1;
					regs->ycyk_state.flag.cache = 1;
				}

				if (dev->off_cnt == 0x800){
					dev->off_cnt = 0;
				}
			}else{ 
				skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in remote_ctrl\n", offset);
				return Invarg_exp;
			}
	}
	return No_exp;
}


char* sparc_remote_ctrl_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	sparc_remote_ctrl_device *dev = conf_obj->obj;
	return dev->regs_name[id];
}
	 

uint32_t sparc_remote_ctrl_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	sparc_remote_ctrl_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static char* sparc_remote_ctrl_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	sparc_remote_ctrl_device *dev = conf_obj->obj;
	remote_ctrl_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t sparc_remote_ctrl_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}
static uint32_t sparc_remote_ctrl_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	sparc_remote_ctrl_device *dev = conf_obj->obj;
	remote_ctrl_reg_t *regs = (dev->regs);
	return  13;
}

static uint32_t sparc_remote_ctrl_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	sparc_remote_ctrl_device *dev = conf_obj->obj;
	remote_ctrl_reg_t *regs = (dev->regs);
	uint32_t regnum;
	regnum = sizeof(remote_ctrl_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;


}

static conf_object_t* new_sparc_remote_ctrl(char* obj_name){
	sparc_remote_ctrl_device* dev = skyeye_mm_zero(sizeof(sparc_remote_ctrl_device));
	remote_ctrl_reg_t* regs =  skyeye_mm_zero(sizeof(remote_ctrl_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init remote_ctrl regs */
	dev->regs = regs;
	dev->regs_name = &regs_name;
	dev->timer_sched_id = -1;
	dev->regs->timer_count = 0;
	dev->regs->ycyk_state.flag.power_status = 1;
	regs->dpsk_page = 0x10004000;
	regs->ycyk_state.flag.page_num = 0;
	dev->page0_cnt = 0;
	dev->page1_cnt = 0;
	dev->page_flag = 0;
	dev->off_cnt = 0;
	dev->length = 0;
	dev->ptr = (char *)skyeye_mm_zero(SIZE);	
	dev->channel_fifo = (char *)skyeye_mm_zero(FIFO_SIZE);	

	if (dev->timer_sched_id <= 0){
		create_thread_scheduler((unsigned int)100, Periodic_sched, timer_callback, dev, &dev->timer_sched_id);
		create_thread_scheduler((unsigned int)500000, Periodic_sched, data_recv_callback, dev, &dev->timer_sched_id);
	}

	return dev->obj;
}
void free_sparc_remote_ctrl(conf_object_t* conf_object){
	sparc_remote_ctrl_device* dev = conf_object->obj;
	skyeye_free(dev->channel_fifo);
	skyeye_free(dev->ptr);
	skyeye_free(dev->obj);
	skyeye_free(dev->regs);
	skyeye_free(dev);
	return;
}


static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	sparc_remote_ctrl_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	sparc_remote_ctrl_device *dev = conf_obj->obj;
	dev->int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void sparc_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "irq_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number ");
	return;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sparc_remote_ctrl_device *dev = obj->obj;
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
	sparc_remote_ctrl_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void init_sparc_remote_ctrl(){
	static skyeye_class_t class_data = {
		.class_name = "sparc_remote_ctrl",
		.class_desc = "sparc remote_ctrl",
		.new_instance = new_sparc_remote_ctrl,
		.free_instance = free_sparc_remote_ctrl,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};
		
	conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = sparc_remote_ctrl_read,
		.write = sparc_remote_ctrl_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);	

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = sparc_remote_ctrl_get_regvalue_by_id,
		.get_regname_by_id = sparc_remote_ctrl_get_regname_by_id,
		.set_regvalue_by_id = sparc_remote_ctrl_set_regvalue_by_id,
		.get_regnum = sparc_remote_ctrl_get_regnum,
		.get_regid_by_name = sparc_remote_ctrl_get_regid_by_name,
		.get_regoffset_by_id = sparc_remote_ctrl_get_regoffset_by_id,
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

	sparc_register_attribute(clss);
}

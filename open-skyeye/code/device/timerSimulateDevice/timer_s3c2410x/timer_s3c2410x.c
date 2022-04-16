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
* @file timer_s3c2410x.c
* @brief The implementation of system controller
* @author
* @version 78.77
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

#include "timer_s3c2410x.h"

static void s3c2410xx_timer_callback(conf_object_t* obj)
{

	timer_s3c2410x_device *dev = obj->obj;
	RW_WRLOCK(dev->lock);
	struct timeval tv;

	if ((dev->regs.tcon & 0x100000) != 0) {
		/*tcntx is the orignal value we set, it equals tcntbx firstly*/
		dev->regs.tcnt[4] -= ((dev->regs.tcntb[4] / 10) + 1);
		if (dev->regs.tcnt[4] <= 0) {
			/* whe the tcntx is 0, reset the timer tcntx as the value of tcntb*/
			dev->regs.tcnt[4] = dev->regs.tcntb[4];
			dev->regs.tcnto[4] = dev->regs.tcntb[4];

			if(dev->signal_iface != NULL && dev->signal != NULL)
				 dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);
			goto exit_func;
		}
	}
exit_func:
	RW_UNLOCK(dev->lock);
}

static exception_t timer_s3c2410x_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	timer_s3c2410x_device *dev = opaque->obj;

	switch (offset) {
		case TCFG0:
			*(uint32_t *)buf = dev->regs.tcfg0;
			break;
		case TCFG1:
			*(uint32_t *)buf = dev->regs.tcfg1;
			break;
		case TCON:
			*(uint32_t *)buf = dev->regs.tcon;
			break;
		case TCNTB0:
		case TCNTB1:
		case TCNTB2:
		case TCNTB3:
		case TCNTB4:
			{
				int n = (offset - 0xC) / 0xC;
				*(uint32_t *)buf = dev->regs.tcntb[n];
			}
			break;
		case TCMPB0:
		case TCMPB1:
			{
				int n = (offset - 0x10) / 0xC;
				*(uint32_t *)buf = dev->regs.tcmpb[n];
			}
			break;
		case TCNTO0:
		case TCNTO1:
		case TCNTO2:
		case TCNTO3:
			{
				int n = (offset - 0x10) / 0xC;
				*(uint32_t *)buf = dev->regs.tcnto[n];
			}
			break;
		case TCNTO4:
			*(uint32_t *)buf = dev->regs.tcnt[4];
			break;
		default:
			break;
		}
	return No_exp;
}

static exception_t timer_s3c2410x_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	timer_s3c2410x_device *dev = opaque->obj;
	switch (offset) {
		case TCFG0:
			dev->regs.tcfg0 = *(uint32_t*)buf;
			break;
		case TCFG1:
			dev->regs.tcfg1 = *(uint32_t*)buf;
			break;
		case TCON:
			{
				dev->regs.tcon = *(uint32_t*)buf;

				/* timer4 */
				if ((dev->regs.tcon & 0x200000) != 0) {

					/* if timer4 is started */
					if(dev->scheduler_id != -1 ){
						/* prescaler for timer4 */
						int scaler = ((dev->regs.tcfg0 & 0xff00) >> 8);

						/* divider selection for timer4 frequency */
						int div = ((dev->regs.tcfg1 & 0xf0000) >> 16);

						/* get the divider */
						int mux = 1;		/* divider for timer4 */
						switch(div){
							case 0x0:
								mux = 2;
								break;
							case 0x1:
								mux = 4;
								break;
							case 0x2:
								mux = 8;
								break;
							case 0x3:
								mux = 16;
								break;
							default: /* external tclk */
								mux = 1;
								break;
						}

						/* timer4 frequency pclk is 120MHZ*/
						long long freq = ((120000000/(scaler + 1))/mux);
						/* get timer4 occur time */
						unsigned int us = (int)(dev->regs.tcntb[4] / (freq));
						/* get timer4 mode */
						int mode = (dev->regs.tcon & 0x400000)?Periodic_sched:Oneshot_sched;
						/* check if a proper value */
						if (us == 0 && dev->regs.tcntb[4])
							us = 1000;
						/* update timer4 */
						mod_thread_scheduler(dev->scheduler_id,(unsigned int)us,mode);
					}
				}

				/* timer4 start or stop */
				if ((dev->regs.tcon & 0x100000) != 0) {
					/* set internal timer */
					if(dev->scheduler_id == -1 ){
						/* prescaler for timer4 */
						int scaler = ((dev->regs.tcfg0 & 0xff00) >> 8);
						/* divider selection for timer4 frequency */
						int div = ((dev->regs.tcfg1 & 0xf0000) >> 16);

						int mux = 1;		/* divider for timer4 */
						/* get the divider */
						switch(div){
							case 0x0:  /* 1/2 */
								mux = 2;
								break;
							case 0x1:  /* 1/4 */
								mux = 4;
								break;
							case 0x2:  /* 1/8 */
								mux = 8;
								break;
							case 0x3:  /* 1/16 */
								mux = 16;
								break;
							default: //external tclk
								mux = 1;
								break;
						}

						/* timer4 frequency */
						long long freq = ((120000000/(scaler + 1))/mux);
						/* get timer4 occur time */
						unsigned int us = (int)(dev->regs.tcntb[4] / (freq));
						printf("[skyeye] tcntb[4] = %d,freq = %lld\n",dev->regs.tcntb[4],freq);
						/* get timer4 mode */
						int mode = (dev->regs.tcon & 0x400000)?Periodic_sched:Oneshot_sched;
						/* check if a proper value */
						if (us == 0 && dev->regs.tcntb[4])
							us = 1000;

						/* create a timer scheduler */
						create_thread_scheduler((unsigned int)us,mode, s3c2410xx_timer_callback, (void*)opaque, &dev->scheduler_id);
					}
				} else {
					if (dev->scheduler_id != -1) {
						del_thread_scheduler(dev->scheduler_id);
						dev->scheduler_id = -1;
					}
				}
				/* timer4 end */
			}

			break;
		case TCNTB0:
		case TCNTB1:
		case TCNTB2:
		case TCNTB3:
		case TCNTB4:
			{
				int n = (offset - 0xC) / 0xC;
				printf("write tcntb4[%d] %u\n",n,*(uint32_t*)buf);
				dev->regs.tcntb[n] = *(uint32_t*)buf;
			}
			break;
		case TCMPB0:
		case TCMPB1:
		case TCMPB2:
		case TCMPB3:
			{
				int n = (offset - 0x10) / 0xC;
				dev->regs.tcmpb[n] = *(uint32_t*)buf;
			}
			break;
		/* shenoubang 2012-4-19 */
		case TCNTO0:
		case TCNTO1:
		case TCNTO2:
		case TCNTO3:
			{
				int n = (offset - 0x14) / 0xc;
				dev->regs.tcnto[n] = *(uint32_t*)buf;
			}
			break;
		case TCNTO4:
			dev->regs.tcnto[4] = *(uint32_t*)buf;
			break;
		default:
			break;
	}
	return No_exp;
}

static char* timer_s3c2410x_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t timer_s3c2410x_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	timer_s3c2410x_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)&(dev->regs) + id;
	return *regs_value;
}

static exception_t timer_s3c2410x_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	timer_s3c2410x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t timer_s3c2410x_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t timer_s3c2410x_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	uint32_t regnum;
	regnum = sizeof(struct registers)/ 4;
	int i;

	for(i =0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static exception_t timer_s3c2410x_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	timer_s3c2410x_device *dev = opaque->obj;
}

static conf_object_t* new_timer_s3c2410x(char* obj_name){
	int i;
	timer_s3c2410x_device* dev = skyeye_mm_zero(sizeof(timer_s3c2410x_device));
	dev->obj = new_conf_object(obj_name, dev);
	/* init timer_s3c2410x regs */
	dev->scheduler_id = -1;
	dev->regs.tcfg0	= 0x00000000;
	dev->regs.tcfg1	= 0x00000000;
	dev->regs.tcon	= 0x00000000;

	for (i = 0; i < S3C2410x_TIMER_NUM; i++) {
		dev->regs.tcnt[i]	= 0x00000000;
		dev->regs.tcmp[i]	= 0x00000000;
		dev->regs.tcntb[i]	= 0x00000000;
		dev->regs.tcnto[i]	= 0x00000000;
		dev->regs.tcmpb[i]	= 0x00000000;
	}

	return dev->obj;
}
static exception_t free_timer_s3c2410x(conf_object_t* opaque) {
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	timer_s3c2410x_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	timer_s3c2410x_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	timer_s3c2410x_device *dev = obj->obj;
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
	timer_s3c2410x_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}


static timer_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");
}
void init_timer_s3c2410x(){
	static skyeye_class_t class_data = {
		.class_name = "s3c2410x_timer",
		.class_desc = "s3c2410x_timer",
		.new_instance = new_timer_s3c2410x,
		.free_instance = free_timer_s3c2410x,
		.get_attr = NULL,
		.set_attr = timer_s3c2410x_set_attr,
		.module_type = SKYML_ONCHIP(SC_s3c2410x1),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = timer_s3c2410x_read,
		.write = timer_s3c2410x_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = timer_s3c2410x_get_regvalue_by_id,
		.get_regname_by_id = timer_s3c2410x_get_regname_by_id,
		.set_regvalue_by_id = timer_s3c2410x_set_regvalue_by_id,
		.get_regnum = timer_s3c2410x_get_regnum,
		.get_regid_by_name = timer_s3c2410x_get_regid_by_name,
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

	timer_register_attribute(clss);
}

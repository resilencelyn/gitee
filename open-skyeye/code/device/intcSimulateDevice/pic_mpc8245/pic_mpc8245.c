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
* @file pic_mpc8245.c
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

#include "pic_mpc8245.h"

static inline uint32 ppc_bswap_word(uint32 data)
{
        return (data >> 24) | ((data >> 8) & 0xff00) | ((data << 8) & 0xff0000)
             | (data << 24);
 }

#define UART_IRQ 3 /* FIXME, just for mpc826x vxworks running*/
#define IPI0 (1 >> 0)
uint32_t get_cpu_register(conf_object_t *cpu, char *name){
	if(!SKY_has_attribute(cpu, name)){
		attr_value_t attr = SKY_get_attribute(cpu, name);
		return SKY_attr_uinteger(attr);
	}else{
		skyeye_log(Error_log, __FUNCTION__, "%s not has attribute %s\n", cpu->objname, name);
		return  0;
	}
}

void set_cpu_register(conf_object_t *cpu, char *name, uint32_t value){
	attr_value_t attr;
	attr = SKY_make_attr_uinteger(value);
	if(!SKY_has_attribute(cpu, name)){
		SKY_set_attribute(cpu, name, &attr);
	}else{
		skyeye_log(Error_log, __FUNCTION__, "%s not has attribute %s\n", cpu->objname, name);
	}
}

static exception_t pic_mpc8245_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct pic_mpc8245_device *dev = opaque->obj;
	pic_mpc8245_reg_t* regs = dev->regs;
	switch(offset) {
		case 0x1020:
			*(uint32_t *)buf = ppc_bswap_word(regs->gcr);
			break;
		case 0x10e0:
		case 0x1100:
		case 0x1140:
		case 0x1180:
		case 0x11c0:
			*(uint32_t *)buf = ppc_bswap_word(regs->gtccr[(offset - 0x1100)/0x40]);
			break;
		case 0x1110:
		case 0x1150:
		case 0x1190:
		case 0x11d0:
			*(uint32_t *)buf = ppc_bswap_word(regs->gtbcr[(offset - 0x1100)/0x40]);
			break;

		case 0x1120:
		case 0x1160:
		case 0x11A0:
		case 0x11E0:
			*(uint32_t *)buf = ppc_bswap_word(regs->gtvpr[(offset - 0x1120)/0x40]) ;
			break;		
		case 0x1130:
		case 0x1170:
		case 0x11b0:
		case 0x11f0:
			*(uint32_t *)buf = ppc_bswap_word(regs->gtdr[(offset - 0x1130)/ 0x40]);
			break;
		case 0x10200:
		case 0x10220:
		case 0x10240:
		case 0x10260:
		case 0x10280:
		case 0x102a0:
		case 0x102c0:
		case 0x102e0:
		case 0x10300:
		case 0x10320:
		case 0x10340:
		case 0x10360:
		case 0x10380:
		case 0x103a0:
		case 0x103c0:
		case 0x103e0:
			*(uint32_t *)buf = ppc_bswap_word(regs->svpr[(offset - 0x10200)/0x20]);
			break;
		case 0x200a0:{
			uint32_t ipr = get_cpu_register(dev->cpu, "ipr");
			if(ipr == 0)
				*(uint32_t *)buf = ppc_bswap_word(0xFF);
			else
				*(uint32_t *)buf = ppc_bswap_word(regs->iack);
			break;
			}
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in pic_mpc8245\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}

static exception_t pic_mpc8245_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct pic_mpc8245_device *dev = opaque->obj;
	pic_mpc8245_reg_t* regs = dev->regs;
	uint32_t val = ppc_bswap_word(*(uint32_t*)buf);
	switch(offset) {
		case 0x41020:
			regs->gcr = val;
			break;
		case 0x1100:
		case 0x1140:
		case 0x1180:
		case 0x11c0:
			regs->gtccr[(offset - 0x1100)/0x40] = val;
			break;
		case 0x1110:
		case 0x1150:
		case 0x1190:
		case 0x11d0:
			regs->gtbcr[(offset - 0x1100)/0x40] = val;
			break;

		case 0x1120:
		case 0x1160:
		case 0x11A0:
		case 0x11E0:
			regs->gtvpr[(offset - 0x1120)/0x40] = val;
			break;		
		case 0x1130:
		case 0x1170:
		case 0x11b0:
		case 0x11f0:
			regs->gtdr[(offset - 0x1130)/ 0x40] = val;
			break;
		case 0x10200:
		case 0x10220:
		case 0x10240:
		case 0x10260:
			//printf("in %s, offset = 0x%x, val=0x%x\n", __FUNCTION__, offset ,val);
		case 0x10280:
		case 0x102a0:
		case 0x102c0:
		case 0x102e0:
		case 0x10300:
		case 0x10320:
		case 0x10340:
		case 0x10360:
		case 0x10380:
		case 0x103a0:
		case 0x103c0:
		case 0x103e0:
			regs->svpr[(offset - 0x10200)/0x20] = val;
			break;
		case 0x200b0:
			regs->eoi = val;
			uint32_t pir = get_cpu_register(dev->cpu, "pir");
			uint32_t ipr = get_cpu_register(dev->cpu, "ipr");
			if (ipr & (1 << UART_IRQ)) {
				ipr &= ~(1 << UART_IRQ);
				set_cpu_register(dev->cpu, "ipr", ipr);
			}
			/* clear the interrupt with highest priority in ISR */
			if (ipr & IPI0) {
				ipr &= ~IPI0;
				set_cpu_register(dev->cpu, "ipr", ipr);
			}
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in pic_mpc8245\n", offset);
			printf("in %s, wrong offset=0x%x\n", __FUNCTION__, offset);
			return Invarg_exp;
	}
	return No_exp;

}

static char* pic_mpc8245_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t pic_mpc8245_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	pic_mpc8245_device *dev = conf_obj->obj;
	pic_mpc8245_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static char* pic_mpc8245_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	pic_mpc8245_device *dev = conf_obj->obj;
	pic_mpc8245_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t pic_mpc8245_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	pic_mpc8245_device *dev = conf_obj->obj;
	pic_mpc8245_reg_t* regs = dev->regs;
	return 0;
}

static uint32_t pic_mpc8245_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	pic_mpc8245_device *dev = conf_obj->obj;
	pic_mpc8245_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(pic_mpc8245_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static void io_do_event (conf_object_t *obj)
{
	const int core_id = 0; /* currently, we only send uart interrupt to cpu0 */
	pic_mpc8245_device *dev = obj->obj;

	uint32_t msr = get_cpu_register(dev->cpu, "msr");
	uint32_t ipr = get_cpu_register(dev->cpu, "ipr");
	if ((!(dev->regs->svpr[UART_IRQ] & 0x80000000)) 
		&& (msr & 0x8000) 
		&& (!(ipr & (1 << UART_IRQ)))){
		if(dev->uart_signal) {
			//RW_WRLOCK(core->ext_int_lock);
			/* set activity bit in vpr */
			dev->regs->svpr[UART_IRQ] |= 0x40000000;
			dev->regs->iack = dev->regs->svpr[UART_IRQ] & 0x000000ff;

			set_cpu_register(dev->cpu, "ipi_flag", 1);
			ipr |= (1 << UART_IRQ);
			set_cpu_register(dev->cpu, "ipr", ipr);
			dev->uart_signal = 0;
			//RW_UNLOCK(core->ext_int_lock);
		}
	}
}


static conf_object_t* new_pic_mpc8245(char* obj_name){
	pic_mpc8245_device* dev = skyeye_mm_zero(sizeof(pic_mpc8245_device));
	pic_mpc8245_reg_t* regs =  skyeye_mm_zero(sizeof(pic_mpc8245_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init pic_mpc8245 regs */
	dev->regs = regs;
	int i = 0;
	for(i = 0; i < SERIAL_VPR_NUM; i++)
		regs->svpr[i] = 0x80000000;
	regs->svr = 0xFF;
	dev->uart_signal = 0;
	int  pthread_id;
	create_thread_scheduler(1000, Periodic_sched, io_do_event, dev->obj, &pthread_id);

	return dev->obj;
}
void free_pic_mpc8245(conf_object_t* conf_obj){
    pic_mpc8245_device* dev = conf_obj->obj;
    skyeye_free(dev->regs);
    skyeye_free(dev);
	return;
}

static int uart_raise_signal(conf_object_t *conf_obj, int line)
{
    	pic_mpc8245_device* dev = conf_obj->obj;
	dev->uart_signal = 1;
	return 0;
}
static int uart_lower_signal(conf_object_t *conf_obj, int line)
{
    	pic_mpc8245_device* dev = conf_obj->obj;
	dev->uart_signal = 0;
	return 0;
}

static attr_value_t get_attr_cpu(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
    	pic_mpc8245_device* dev = conf_obj->obj;
	attr_value_t attr = SKY_make_attr_object(dev->cpu);
	return attr;
}
static exception_t set_attr_cpu(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
    	pic_mpc8245_device* dev = conf_obj->obj;
	dev->cpu = SKY_attr_object(*value);
	return No_exp;
}
static void intc_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "cpu", get_attr_cpu, NULL, set_attr_cpu, NULL, SKY_Attr_Optional, "object", "NULL");
	return;
}

void init_pic_mpc8245(){
	static skyeye_class_t class_data = {
		.class_name = "mpc8245_pic",
		.class_desc = "mpc8245_pic",
		.new_instance = new_pic_mpc8245,
		.free_instance = free_pic_mpc8245,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_mpc8245),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = pic_mpc8245_read,
		.write = pic_mpc8245_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = pic_mpc8245_get_regvalue_by_id,
		.get_regname_by_id = pic_mpc8245_get_regname_by_id,
		.set_regvalue_by_id = pic_mpc8245_set_regvalue_by_id,
		.get_regnum = pic_mpc8245_get_regnum,
		.get_regid_by_name = pic_mpc8245_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static general_signal_intf signal_intf = {
		.raise_signal = uart_raise_signal,
		.lower_signal = uart_lower_signal,
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_intf);

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
			.iface = &signal_intf,
		},
 	};
	static const struct ConnectDescription connects[] = {
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	intc_register_attribute(clss);
}

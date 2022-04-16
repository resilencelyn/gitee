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
#include <skyeye_system.h>
#include "pic_fifo.h"
#include "pic_T2080.h"

static void timer_intc_trriger(conf_object_t *opaque,uint32_t timer_index)
{
	pic_T2080_device *dev = opaque->obj;

	WriteFIFO(dev->int_fifo, &(dev->time_int_num[timer_index]), 1);
	dev->isInt = 1;
	dev->global_access_registers[0xa0] = dev->time_int_num[timer_index];
	dev->pic_global[timer_index * 0x40 + 0x1100] = dev->pic_global[timer_index * 0x40 + 0x1110] | 0x80000000;
	dev->timer_cnt[timer_index] = (dev->pic_global[timer_index * 0x40 + 0x1110] & 0x7FFFFFFF) / (TimeBaseRateUS * 100);
}

static void timer_func_0(conf_object_t *opaque, time_data_t * data)
{
	pic_T2080_device *dev = opaque->obj;
	uint32_t timer_index = 0;

	if ((dev->timer_cnt[timer_index] > 0) && (dev->time_int_enable[timer_index] == 0))//
	{
		dev->timer_cnt[timer_index]--;
		dev->pic_global[timer_index * 0x40 + 0x1100] = dev->timer_cnt[timer_index] * TimeBaseRateUS * 100;
		if ((dev->timer_cnt[timer_index]) == 0)
		{
			timer_intc_trriger(opaque,timer_index);
		}
	}
}

static void timer_func_1(conf_object_t *opaque, time_data_t * data)
{
	pic_T2080_device *dev = opaque->obj;
	uint32_t timer_index = 1;

	if ((dev->timer_cnt[timer_index] > 0) && (dev->time_int_enable[timer_index] == 0)) //
	{
		dev->timer_cnt[timer_index]--;
		dev->pic_global[timer_index * 0x40 + 0x1100] = dev->timer_cnt[timer_index] * TimeBaseRateUS * 100;
		if ((dev->timer_cnt[timer_index]) == 0)
		{
			timer_intc_trriger(opaque,timer_index);
		}
	}
}

static void timer_func_2(conf_object_t *opaque, time_data_t * data) 
{
	pic_T2080_device *dev = opaque->obj;
	uint32_t timer_index = 2;

	if (((dev->pic_global[timer_index * 0x40 + 0x1100] & 0x7FFFFFFF)!= 0) && (dev->time_int_enable[timer_index] == 0))
	{
		dev->timer_cnt[timer_index]--;
		dev->pic_global[timer_index * 0x40 + 0x1100] = dev->timer_cnt[timer_index] * TimeBaseRateUS * 100;
		if ((dev->timer_cnt[timer_index]) == 0)
		{
			timer_intc_trriger(opaque,timer_index);
		}
	}
}

static void timer_func_3(conf_object_t *opaque, time_data_t * data)
{
	pic_T2080_device *dev = opaque->obj;
	uint32_t timer_index = 3;

	if (((dev->pic_global[timer_index * 0x40 + 0x1100] & 0x7FFFFFFF)!= 0) && (dev->time_int_enable[timer_index] == 0))
	{
		dev->timer_cnt[timer_index]--;
		dev->pic_global[timer_index * 0x40 + 0x1100] = dev->timer_cnt[timer_index] * TimeBaseRateUS * 100;
		if ((dev->timer_cnt[timer_index]) == 0)
		{
			timer_intc_trriger(opaque,timer_index);
		}
	}
}

static void send_irq_callback(conf_object_t *opaque)
{
	pic_T2080_device *dev = opaque->obj;
	int irq_num, intNum;
	intNum = CheckIntReadNum(dev->int_fifo);

	if (intNum == 0 && dev->isInt)
	{
		return;
	}

	if (dev->isInt)
	{
		ReadFIFO(dev->int_fifo, &irq_num, 1);
		dev->isInt = 0;
		dev->pic_global[0xa0] = irq_num;//IACK
		if (dev->signal_iface)
		{
			dev->signal_iface->signal(dev->signal, NULL);
		}
	} else
	{
		if (dev->signal_iface)
		{
			dev->signal_iface->signal(dev->signal, NULL);
		}
	}
}

static exception_t pic_T2080_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	pic_T2080_device *dev = opaque->obj;
	if (offset >= 0 && offset <= 0xFFFF){
		if(offset == 0xa0)
		{//IACK
			if ((dev->pic_global[offset] & 0xffff) == 0) {
				*(uint32_t *)buf = dev->pic_global[offset];
				*(uint32_t *)buf |= dev->pic_global[0x10e0] & 0xffff;
			}else{
				*(uint32_t *)buf = dev->pic_global[offset];
			}
		}
		else
		{
			*(uint32_t *)buf = dev->pic_global[offset];
		}
	}
	else if (offset >= 0x10000 && offset <= 0x1FFFF)
	{
		*(uint32_t *)buf = dev->source_registers[offset - 0x10000];
	}
	else if (offset >= 0x20000 && offset <= 0x2FFFF)
	{
		*(uint32_t *)buf = dev->global_access_registers[offset - 0x20000];
		if (offset == 0x200a0)
		{
			dev->global_access_registers[offset - 0x20000] = 0;
		}
	}
	return 0;
}

static exception_t pic_T2080_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	pic_T2080_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	uint32_t timer_index = 0;
	int time_cur_flag;
	
	if (offset >= 0 && offset <= 0xFFFF){
		if (offset == 0x1020){//GCR
			if (val & (1 << 31)) {
				dev->pic_global[offset] = val;
				dev->pic_global[offset] &= ~(1 << 31);
			}
		}
		else if(offset == 0xB0){//EOI
			dev->pic_global[offset] = val;
			dev->isInt = 1;
			if(val == 0){
				dev->pic_global[0xa0] = dev->pic_global[0x10e0];
			}
		}
		else if (((offset - 0x1100) / 0x40 < 4) && (offset >= 0x1100))
		{
			dev->pic_global[offset] = val;

			if (((offset - 0x1110) % 0x40 == 0) && (offset >= 0x1110))
			{
				timer_index = (offset - 0x1110) / 0x40;
				time_cur_flag = (val >> 31) & 1;
				if (dev->time_flag[timer_index] - time_cur_flag == 1)
				{
					if (dev->first_flag[timer_index])
					{
						dev->timer_cnt[timer_index] = (val & 0x7FFFFFFF) / (TimeBaseRateUS * 100);
						dev->first_flag[timer_index] = 0;
					}
					dev->pic_global[offset - 0x10] = val & 0x7FFFFFFF;
					dev->time_flag[timer_index] = time_cur_flag;
					system_enable_timer(dev->handle[timer_index]);
				}
				else if (dev->time_flag[timer_index] - time_cur_flag == -1)
				{
					dev->time_flag[timer_index] = time_cur_flag;
					system_disable_timer(dev->handle[timer_index]);
				}
			}
			else if (((offset - 0x1120) % 0x40 == 0) && (offset >= 0x1120))
			{
				timer_index = (offset - 0x1110) / 0x40;
				dev->time_int_num[timer_index] = val & 0xFFFF;
				dev->time_int_enable[timer_index] = (val >> 31) & 1;
			}
		}
		else
			dev->pic_global[offset] = val;
	}
	else if (offset >= 0x10000 && offset <= 0x1FFFF){
		dev->source_registers[offset - 0x10000] = val;
	}else if (offset >= 0x20000 && offset <= 0x2FFFF){
		dev->global_access_registers[offset - 0x20000] = val;
	}
	return 0;
}

static char* pic_T2080_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t pic_T2080_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	return No_exp;
}

static uint32_t pic_T2080_get_regnum(conf_object_t *conf_obj)
{
	return 0;
}

static uint32_t pic_T2080_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	
	return 0;
}

static uint32_t pic_T2080_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	return 0;
}

static conf_object_t* new_pic_T2080(const char *obj_name)
{
	pic_T2080_device* dev = skyeye_mm_zero(sizeof(pic_T2080_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->pic_global[0x10e0] = 0x0000FFFF;//SVR
	dev->pic_global[0x80] = 0x0000000F;//CTPR
	dev->pic_global[0x1110] = 0x80000000;
	dev->pic_global[0x1120] = 0x80000000;
	dev->pic_global[0x1130] = 0x00000001;
	dev->pic_global[0x1150] = 0x80000000;
	dev->pic_global[0x1160] = 0x80000000;
	dev->pic_global[0x11a0] = 0x80000000;
	dev->pic_global[0x11e0] = 0x80000000;
	dev->pic_global[0x2120] = 0x80000000;
	dev->pic_global[0x2160] = 0x80000000;
	dev->pic_global[0x21a0] = 0x80000000;
	dev->pic_global[0x21e0] = 0x80000000;
	dev->pic_global[0x10a0] = 0x80000000;
	dev->pic_global[0x10b0] = 0x80000000;
	dev->pic_global[0x10c0] = 0x80000000;
	dev->pic_global[0x10d0] = 0x80000000;
	int i,j,k;
	for (i= 0;i<12;i++){
		dev->source_registers[0x10000+i*0x20] = 0x80000000;//EIVPR0-11
		dev->source_registers[0x10010+i*0x30] = 0x00000001;//EIDR0-11
	}
	for (j=0;j<160;j++){
		dev->source_registers[0x10200+j*0x20] = 0x80800000;//IIVPR0-159
		dev->source_registers[0x10210+j*0x30] = 0x00000001;//IIVPR0-159
	}
	for (k=0;k<15;k++){
		dev->source_registers[0x11C00+k*0x20] = 0x80000000;//MSIVPRA0-MSIVPRD15
		dev->source_registers[0x11C10+k*0x30] = 0x00000001;//MSIVPRA0-MSIVPRD15
	}
	for (j=160;j<256;j++){
		dev->source_registers[0x13000+j*0x20] = 0x80800000;//IIVPR160-255
		dev->source_registers[0x13010+j*0x30] = 0x00000001;//IIVPR160-255
	}
	for (i = 0;i < 4;i++)
	{
		dev->time_flag[i] = 1;
		dev->time_int_enable[i] = 1;
		dev->time_int_num[i] = 0;
		dev->timer_cnt[i] = 0;
		dev->first_flag[i] = 1;
	}
	dev->int_fifo = CreateFIFO(0x2000);
	dev->isInt = 1;
	return dev->obj;
}

static exception_t free_pic_T2080(conf_object_t* opaque)
{
	pic_T2080_device *dev = opaque->obj;

	skyeye_free(dev->int_fifo);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static int raise_signal(conf_object_t *obj, int irq_num){
	pic_T2080_device *dev = obj->obj;
	
	//dev->pic_global[0xa0] = irq_num;//IACK
	WriteFIFO(dev->int_fifo, &irq_num, 1);
	/*
	if(irq_num >= 0 && irq_num <= 31)//CISR1
		dev->pic_global[0x3B40] |= (1<< (irq_num-32)); 
	else if(irq_num >= 32 && irq_num <= 63)
		dev->pic_global[0x3B50] |= (1<< (irq_num-64)); 
	else if(irq_num >= 64 && irq_num <= 95)
		dev->pic_global[0x3B60] |= (1<< (irq_num-96)); 
	else if(irq_num >= 96 && irq_num <= 127)
		dev->pic_global[0x3B70] |= (1<< (irq_num-128)); 
	else if(irq_num >= 128 && irq_num <= 159)
		dev->pic_global[0x3B80] |= (1<< (irq_num-160)); 
	else if(irq_num >= 160 && irq_num <= 191)
		dev->pic_global[0x3B90] |= (1<< (irq_num-192)); 
	else if(irq_num >= 192 && irq_num <= 223)
		dev->pic_global[0x3BA0] |= (1<< (irq_num-224)); 
	else if(irq_num >= 224 && irq_num <= 255)
		dev->pic_global[0x3BB0] |= (1<< (irq_num-256)); 
	dev->signal_iface->signal(dev->signal, NULL);
	*/

	return No_exp;
}

static int lower_signal(conf_object_t *obj, int irq_num)
{
	pic_T2080_device *dev = obj->obj;

	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	pic_T2080_device *dev = obj->obj;
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
	pic_T2080_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static exception_t config_pic_T2080(conf_object_t* opaque)
{
	pic_T2080_device *dev =  opaque->obj;

	dev->handle[0] = system_register_timer_handler(opaque,100, (time_sched_t)timer_func_0, SCHED_US|SCHED_NORMAL);
	system_disable_timer(dev->handle[0]);
	dev->handle[1] = system_register_timer_handler(opaque,100, (time_sched_t)timer_func_1, SCHED_US|SCHED_NORMAL);
	system_disable_timer(dev->handle[1]);
	dev->handle[2] = system_register_timer_handler(opaque,100, (time_sched_t)timer_func_2, SCHED_US|SCHED_NORMAL);
	system_disable_timer(dev->handle[2]);
	dev->handle[3] = system_register_timer_handler(opaque,100, (time_sched_t)timer_func_3, SCHED_US|SCHED_NORMAL);
	system_disable_timer(dev->handle[3]);


	system_register_timer_handler(opaque, 1000, (time_sched_t)send_irq_callback, SCHED_US|SCHED_NORMAL);

	return No_exp;
}

void init_pic_T2080(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "pic_T2080",
		.class_desc = "pic_T2080",
		.new_instance = new_pic_T2080,
		.free_instance = free_pic_T2080,
		.config_instance = config_pic_T2080,
		.module_type = SKYML_ONCHIP(SC_t2080),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = pic_T2080_read,
		.write = pic_T2080_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	
	static const general_signal_iface_t signal_iface = {
		.raise_signal = raise_signal,
		.lower_signal = lower_signal
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_iface);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = pic_T2080_get_regvalue_by_id,
		.get_regname_by_id = pic_T2080_get_regname_by_id,
		.set_regvalue_by_id = pic_T2080_set_regvalue_by_id,
		.get_regnum = pic_T2080_get_regnum,
		.get_regid_by_name = pic_T2080_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
	static const struct InterfaceDescription ifaces[] = 
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription)
		{
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf
		},
		(struct InterfaceDescription)
		{
			.name = GENERAL_SIGNAL_INTF_NAME,
			.iface = &signal_iface
		}
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
				(struct ConnectDescription) {
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

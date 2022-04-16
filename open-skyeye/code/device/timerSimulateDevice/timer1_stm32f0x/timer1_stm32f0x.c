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
#include "timer1_stm32f0x.h"
static exception_t timer1_stm32f0x_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	timer1_stm32f0x_device *dev = opaque->obj;
	switch (offset)
	{
		case TIM1_CR1:
			*(uint32_t *)buf = dev->regs.tim1_cr1.all;
			break;
		case TIM1_CR2:
			*(uint32_t *)buf = dev->regs.tim1_cr2.all;
			break;
		case TIM1_SMCR:
			*(uint32_t *)buf = dev->regs.tim1_smcr;
			break;
		case TIM1_DIER:
			*(uint32_t *)buf = dev->regs.tim1_dier.all;
			break;
		case TIM1_SR:
			*(uint32_t *)buf = dev->regs.tim1_sr;
			break;
		case TIM1_EGR:
			*(uint32_t *)buf = dev->regs.tim1_egr.all;
			break;
		case TIM1_CCMR1:
			*(uint32_t *)buf = dev->regs.tim1_ccmr1;
			break;
		case TIM1_CCMR2:
			*(uint32_t *)buf = dev->regs.tim1_ccmr2;
			break;
		case TIM1_CCER:
			*(uint32_t *)buf = dev->regs.tim1_ccer;
			break;
		case TIM1_CNT:
			*(uint32_t *)buf = dev->regs.tim1_cnt;
			break;
		case TIM1_PSC:
			*(uint32_t *)buf = dev->regs.tim1_psc;
			break;
		case TIM1_ARR:
			*(uint32_t *)buf = dev->regs.tim1_arr;
			break;
		case TIM1_RCR:
			*(uint32_t *)buf = dev->regs.tim1_rcr;
			break;
		case TIM1_CCR1:
			*(uint32_t *)buf = dev->regs.tim1_ccr1;
			break;
		case TIM1_CCR2:
			*(uint32_t *)buf = dev->regs.tim1_ccr2;
			break;
		case TIM1_CCR3:
			*(uint32_t *)buf = dev->regs.tim1_ccr3;
			break;
		case TIM1_CCR4:
			*(uint32_t *)buf = dev->regs.tim1_ccr4;
			break;
		case TIM1_BDTR:
			*(uint32_t *)buf = dev->regs.tim1_bdtr;
			break;
		case TIM1_DCR:
			*(uint32_t *)buf = dev->regs.tim1_dcr;
			break;
		case TIM1_DMAR:
			*(uint32_t *)buf = dev->regs.tim1_dmar;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in timer1_stm32f0x\n", offset);
			break;
	}
	//printf("%d %s 0x%x %x\n",__LINE__,__func__,offset,*(uint32_t*)buf);
	return No_exp;
}

static void update_event(conf_object_t *obj){
	timer1_stm32f0x_device *dev = obj->obj;

	core_signal_intf* signal_iface = dev->signal_iface;
	interrupt_signal_t interrupt_signal;
	interrupt_signal.arm_signal.m0_irq = 15;
	if (signal_iface != NULL) {
		signal_iface->signal(dev->core, &interrupt_signal);
	}
}

static void timer_func(void *obj) {
	timer1_stm32f0x_device *dev = ((conf_object_t *)obj)->obj;
	dev->scale += 1;
	dev->regs.tim1_cnt = dev->scale;
	//if (dev->regs.cnt > dev->regs.prd)
	//	dev->regs.cnt = 0;
	//printf("%s %d\n", __func__, dev->regs.cnt); 
	//printf("scale = %d, arr = %d\n",dev->scale,dev->regs.tim1_arr);
	if(dev->scale >= dev->regs.tim1_arr){
		update_event(dev->obj); 
		dev->scale = 0;
		dev->regs.tim1_cnt = 0;
	}

}

static exception_t timer1_stm32f0x_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	timer1_stm32f0x_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf("%d %s 0x%x %x\n",__LINE__,__func__,offset,val);
	switch (offset)
	{
		case TIM1_CR1:
			dev->regs.tim1_cr1.all = val;
			if(dev->regs.tim1_cr1.bit.cen == 1){
				if(dev->sched_id == -1)
					create_thread_scheduler((unsigned int)1000, Periodic_sched, timer_func, (void*)dev->obj, &(dev->sched_id)); 
				/*********************REAL timing***************************/
				/* if(dev->psc != dev->regs.tim1_psc){
				   if(dev->handle != NULL)
				   system_disable_timer(dev->handle);
				   dev->handle = NULL;
				   dev->psc = dev->regs.tim1_psc;
				   }
				   if(dev->clock_frequency == NULL){
				   skyeye_log(Error_log, __FUNCTION__, "Can not get clock frequency_t in timer1_stm32f0x\n");
				   return Invarg_exp;
				   }
				   if(dev->handle != NULL){
				   system_enable_timer(dev->handle);
				   }else if(dev->handle == NULL){
				   double ck_psc = (double)dev->clock_frequency->read(dev->clock,SYSCLOCK);
				   double ck_cnt = ck_psc / (dev->regs.tim1_psc + 1);
				   double period_timer = 1 / ck_cnt;
				   printf("psc=%d,ck_psc=%0.16f,ck_cnt=%0.16f,f=%0.16f\n",dev->regs.tim1_psc,ck_psc,ck_cnt,1/ck_cnt);
				   if(period_timer >= 1.0 ) {
				   dev->handle = system_register_timer_handler(dev->obj, 5, (time_sched_t)timer_func, SCHED_MS|SCHED_NORMAL);
				   }else if(period_timer * 1000 >= 1.0) {
				   dev->handle = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000), (time_sched_t)timer_func, SCHED_MS|SCHED_NORMAL);
				   }else if(period_timer * 1000000 >= 1.0) {
				   dev->handle = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000), (time_sched_t)timer_func, SCHED_US|SCHED_NORMAL);
				   }else if(period_timer * 1000000000 >= 1.0) {
				   dev->handle = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000000), (time_sched_t)timer_func, SCHED_NS|SCHED_NORMAL);
				   }
				   }*/
			}
			else if(dev->regs.tim1_cr1.bit.cen == 0){
				if(dev->sched_id != -1)
					del_thread_scheduler(dev->sched_id);
				dev->sched_id = -1;
				/*if(dev->handle != NULL){
				  system_disable_timer(dev->handle);
				  }*/
			}
			break;
		case TIM1_CR2:
			dev->regs.tim1_cr2.all= val;
			break;
		case TIM1_SMCR:
			dev->regs.tim1_smcr = val;
			break;
		case TIM1_DIER:
			dev->regs.tim1_dier.all = val;
			break;
		case TIM1_SR:
			dev->regs.tim1_sr = val;
			break;
		case TIM1_EGR:
			dev->regs.tim1_egr.all = val;
			if(dev->regs.tim1_egr.bit.ug == 1){
				if(dev->regs.tim1_cr1.bit.dir == 0){
					dev->regs.tim1_cnt = 0;
				}else{
					dev->regs.tim1_cnt = dev->regs.tim1_arr; 
				}
			}
			break;
		case TIM1_CCMR1:
			dev->regs.tim1_ccmr1 = val;
			break;
		case TIM1_CCMR2:
			dev->regs.tim1_ccmr2 = val;
			break;
		case TIM1_CCER:
			dev->regs.tim1_ccer = val;
			break;
		case TIM1_CNT:
			dev->regs.tim1_cnt = val;
			break;
		case TIM1_PSC:
			dev->regs.tim1_psc = val;
			if(val >= 0x10000){
				dev->regs.tim1_psc = val & 0x0000ffff;
			}
			break;
		case TIM1_ARR:
			dev->regs.tim1_arr = val;
			break;
		case TIM1_RCR:
			dev->regs.tim1_rcr = val;
			break;
		case TIM1_CCR1:
			dev->regs.tim1_ccr1 = val;
			break;
		case TIM1_CCR2:
			dev->regs.tim1_ccr2 = val;
			break;
		case TIM1_CCR3:
			dev->regs.tim1_ccr3 = val;
			break;
		case TIM1_CCR4:
			dev->regs.tim1_ccr4 = val;
			break;
		case TIM1_BDTR:
			dev->regs.tim1_bdtr = val;
			break;
		case TIM1_DCR:
			dev->regs.tim1_dcr = val;
			break;
		case TIM1_DMAR:
			dev->regs.tim1_dmar = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in timer1_stm32f0x\n", offset);
			break;
	}
	return No_exp;
}
static char* timer1_stm32f0x_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t timer1_stm32f0x_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	timer1_stm32f0x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t timer1_stm32f0x_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t timer1_stm32f0x_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	timer1_stm32f0x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t timer1_stm32f0x_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static exception_t CK_PSC_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	timer1_stm32f0x_device *dev = obj->obj;
	dev->clock = obj2;
	dev->clock_frequency = (get_clock_frequency_t *)SKY_get_iface(dev->clock, CLOCK_FREQUENCY_INTERFACE);

	if (dev->clock_frequency == NULL){
		if (dev->clock == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CLOCK_FREQUENCY_INTERFACE);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CLOCK_FREQUENCY_INTERFACE, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t CK_PSC_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	timer1_stm32f0x_device *dev = obj->obj;
	*obj2 = dev->clock;
	*port = NULL;
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	timer1_stm32f0x_device *dev = obj->obj;
	dev->core = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->core, CORE_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->core == NULL)
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
	timer1_stm32f0x_device *dev = obj->obj;
	*obj2 = dev->core;
	*port = NULL;
	return No_exp;
}


static conf_object_t* new_timer1_stm32f0x(char *obj_name)
{
	timer1_stm32f0x_device* dev = skyeye_mm_zero(sizeof(timer1_stm32f0x_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->scale = 0;
	dev->sched_id = -1;
	return dev->obj;
}
static exception_t free_timer1_stm32f0x(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_timer1_stm32f0x(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "timer1_stm32f0x",
		.class_desc = "timer1_stm32f0x",
		.new_instance = new_timer1_stm32f0x,
		.free_instance = free_timer1_stm32f0x,
		.module_type = SKYML_ONCHIP(SC_stm32),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = timer1_stm32f0x_read,
		.write = timer1_stm32f0x_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = timer1_stm32f0x_get_regvalue_by_id,
		.get_regname_by_id = timer1_stm32f0x_get_regname_by_id,
		.set_regvalue_by_id = timer1_stm32f0x_set_regvalue_by_id,
		.get_regnum = timer1_stm32f0x_get_regnum,
		.get_regid_by_name = timer1_stm32f0x_get_regid_by_name
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
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription){
			.name = CLOCK_FREQUENCY_INTERFACE,
			.set = CK_PSC_set,
			.get = CK_PSC_get,
		},
		(struct ConnectDescription){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

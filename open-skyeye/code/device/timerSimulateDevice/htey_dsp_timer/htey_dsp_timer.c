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
#include "htey_dsp_timer.h"

#define DEFAULT_FREQ 50000000  // 200M/4

static exception_t htey_dsp_timer_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_dsp_timer_device *dev = opaque->obj;
	switch (offset)
	{
		case CTRL:
			*(uint32_t *)buf = dev->regs.ctrl;
			//printf("%s ctrl %d 0x%x\n", __func__, dev->regs.ctrl, dev->regs.ctrl);
			break;
		case PRD:
			*(uint32_t *)buf = dev->regs.prd;
			//printf("%s prd %d 0x%x\n", __func__, dev->regs.prd, dev->regs.prd);
			break;
		case CNT:
			*(uint32_t *)buf = dev->regs.cnt;
			//printf("%s cnt %d 0x%x\n", __func__, dev->regs.cnt, dev->regs.cnt);
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in htey_dsp_timer\n", offset);
			break;
	}
	return 0;
}

static void timer_func(void *obj) {
	htey_dsp_timer_device *dev = ((conf_object_t *)obj)->obj;
	dev->scale += 30000;
	dev->regs.cnt = dev->scale;
	//if (dev->regs.cnt > dev->regs.prd)
	//	dev->regs.cnt = 0;
	//printf("%s %d\n", __func__, dev->regs.cnt); 

	core_signal_intf* core_signal = dev->core_signal;
	interrupt_signal_t interrupt_signal;
	int i=0;
	for(; i < 16; i++)
		interrupt_signal.c6k_signal.ifr[i] = Prev_level;
	interrupt_signal.c6k_signal.ifr[dev->interrupt_number] = High_level;

	if (core_signal) {
		core_signal->signal(dev->core, &interrupt_signal);
	}
}

static exception_t htey_dsp_timer_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_dsp_timer_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case CTRL:
			dev->regs.ctrl = val;
			//printf("%s ctrl %d 0x%x\n", __func__, dev->regs.ctrl, dev->regs.ctrl);
			if ((val & 0xc0) == 0xc0) {
				//printf("re count\n");
				if (dev->handle != NULL) {
					system_enable_timer(dev->handle);
				} else if (dev->handle == NULL) {
					double period = (double)dev->regs.prd;
					if(dev->regs.ctrl & 0x100)
						period = period *2;

					double period_timer = period/ DEFAULT_FREQ;
					//printf("%lf period_timer\n",period_timer);
					//printf("0x%llx period_timer\n",period_timer);
					//printf("%lf period\n",period);
					if(period_timer >= 1.0 ) {
						//dev->handle = system_register_timer_handler(dev->obj, (uint32_t)period_timer, (time_sched_t)timer_func, SCHED_S|SCHED_NORMAL);
						dev->handle = system_register_timer_handler(dev->obj, 5, (time_sched_t)timer_func, SCHED_MS|SCHED_NORMAL);
					} else if(period_timer * 1000 >= 1.0) {
						dev->handle = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000), (time_sched_t)timer_func, SCHED_MS|SCHED_NORMAL);
					} else if(period_timer * 1000000 >= 1.0) {
						dev->handle = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000), (time_sched_t)timer_func, SCHED_US|SCHED_NORMAL);
					} else if(period_timer * 1000000000 >= 1.0) {
						dev->handle = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000000), (time_sched_t)timer_func, SCHED_NS|SCHED_NORMAL);
					}
				}
			} else if ((val & 0xc0) == 0x0) {
				//printf("stop\n");
				if (dev->handle != NULL)
					system_disable_timer(dev->handle);
			} else if ((val & 0xc0) == 0x40) {
				//printf("continue\n");
				if (dev->handle == NULL)
				{
					double period = (double)dev->regs.prd;
					if(dev->regs.ctrl & 0x100)
						period = period *2;

					double period_timer = period/ DEFAULT_FREQ;

					if(period_timer >= 1.0 ) {
						dev->handle = system_register_timer_handler(dev->obj, 5, (time_sched_t)timer_func, SCHED_MS|SCHED_NORMAL);
					} else if(period_timer * 1000 >= 1.0) {
						dev->handle = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000), (time_sched_t)timer_func, SCHED_MS|SCHED_NORMAL);
					} else if(period_timer * 1000000 >= 1.0) {
						dev->handle = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000), (time_sched_t)timer_func, SCHED_US|SCHED_NORMAL);
					} else if(period_timer * 1000000000 >= 1.0) {
						dev->handle = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000000), (time_sched_t)timer_func, SCHED_NS|SCHED_NORMAL);
					}
				}
			} else {
				//printf("unkown\n");
			}
			break;
		case PRD:
			dev->regs.prd = val;
			//printf("%s prd %d 0x%x\n", __func__, dev->regs.prd, dev->regs.prd);
			break;
		case CNT:
			dev->regs.cnt = val;
			//printf("%s cnt %d 0x%x\n", __func__, dev->regs.cnt, dev->regs.cnt);
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in htey_dsp_timer\n", offset);
			break;
	}
	return 0;
}
static char* htey_dsp_timer_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t htey_dsp_timer_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	htey_dsp_timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t htey_dsp_timer_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t htey_dsp_timer_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	htey_dsp_timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t htey_dsp_timer_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_htey_dsp_timer(char *obj_name)
{
	htey_dsp_timer_device* dev = skyeye_mm_zero(sizeof(htey_dsp_timer_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->core_signal = NULL;
	struct registers *regs = &(dev->regs);
	return dev->obj;
}
static exception_t free_htey_dsp_timer(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t htey_dsp_timer_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	htey_dsp_timer_device *dev = opaque->obj;

	if(!strncmp(attr_name, "cpu", strlen("cpu"))){
		dev->core_signal = (core_signal_intf *)SKY_get_iface(value->u.object,CORE_SIGNAL_INTF_NAME);
	}else if(!strncmp(attr_name, "int_number", strlen("int_number"))){
		dev->interrupt_number = value->u.uinteger;
		skyeye_log(Error_log, __FUNCTION__, "in %s, set interrupt_number %d\n",   __FUNCTION__, dev->interrupt_number);
	}
	else{
		skyeye_log(Error_log, __FUNCTION__, "No such attribute\n");
	}

}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	htey_dsp_timer_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj,attr_value_t* value, attr_value_t* idx){
	htey_dsp_timer_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static timer_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL,set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	htey_dsp_timer_device *dev = obj->obj;
	dev->core = obj2;
	dev->core_signal = (core_signal_intf *)SKY_get_iface(dev->core, CORE_SIGNAL_INTF_NAME);
	if (dev->core_signal == NULL){
		if (dev->core == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2,const char **port, int index)
{
	htey_dsp_timer_device *dev = obj->obj;
	*obj2 = dev->core;
	*port = NULL;

	return No_exp;
}


void init_htey_dsp_timer(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "htey_dsp_timer",
		.class_desc = "htey_dsp_timer",
		.new_instance = new_htey_dsp_timer,
		.free_instance = free_htey_dsp_timer,
		.get_attr = NULL,
		.set_attr = htey_dsp_timer_set_attr
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = htey_dsp_timer_read,
		.write = htey_dsp_timer_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = htey_dsp_timer_get_regvalue_by_id,
		.get_regname_by_id = htey_dsp_timer_get_regname_by_id,
		.set_regvalue_by_id = htey_dsp_timer_set_regvalue_by_id,
		.get_regnum = htey_dsp_timer_get_regnum,
		.get_regid_by_name = htey_dsp_timer_get_regid_by_name
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

		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	timer_register_attribute(clss);
}

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
#include "htey_hard_timer.h"
#include "skyeye_system.h"
static exception_t htey_hard_timer_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_hard_timer_device *dev = opaque->obj;
	switch (offset)
	{
		case TIMER_1S:
			*(uint32_t *)buf = dev->regs.timer_1s;
			break;
		case TIMER_2R5:
			*(uint32_t *)buf = dev->regs.timer_2r5;
	
			break;
		case TIMER_1S_AMOUNT:
			*(uint32_t *)buf = dev->regs.timer_1s_amount;
			break;
		case TIMER_1S1:
			*(uint32_t *)buf = dev->regs.timer_1s1;
			break;
		case TIMER_1S1_AMOUN:
			*(uint32_t *)buf = dev->regs.timer_1s1_amoun;
			break;
		case TIMER_2R5_LOCK:
			*(uint32_t *)buf = dev->regs.timer_2r5_lock;
			break;
		case TIMER_2R5_BYTE0:
			*(uint32_t *)buf = dev->regs.timer_2r5_byte0;
			break;
		case TIMER_2R5_BYTE1:
			*(uint32_t *)buf = dev->regs.timer_2r5_byte1;
			break;
		case TIMER_2R5_BYTE2:
			*(uint32_t *)buf = dev->regs.timer_2r5_byte2;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in htey_hard_timer\n", offset);
			break;
	}

	//printf("%s, %d, %d, value = %x\n", __func__, __LINE__, offset, *(uint32_t *)buf);
	return 0;
}
static exception_t htey_hard_timer_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_hard_timer_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf("%s, %d, %d, value = %x\n", __func__, __LINE__, offset, val);
	switch (offset)
	{
		case TIMER_1S:
			dev->regs.timer_1s = val;
			break;
		case TIMER_2R5:
			dev->regs.timer_2r5 = val;
			break;
		case TIMER_1S_AMOUNT:
			dev->regs.timer_1s_amount = val;
			break;
		case TIMER_1S1:
			dev->regs.timer_1s1 = val;
			break;
		case TIMER_1S1_AMOUN:
			dev->regs.timer_1s1_amoun = val;
			break;
		case TIMER_2R5_LOCK:
			dev->regs.timer_2r5_lock = val;
			break;
		case TIMER_2R5_BYTE0:
			dev->regs.timer_2r5_byte0 = val;
			break;
		case TIMER_2R5_BYTE1:
			dev->regs.timer_2r5_byte1 = val;
			break;
		case TIMER_2R5_BYTE2:
			dev->regs.timer_2r5_byte2 = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in htey_hard_timer\n", offset);
			break;
	}
	return 0;
}
static char* htey_hard_timer_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t htey_hard_timer_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	htey_hard_timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t htey_hard_timer_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t htey_hard_timer_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	htey_hard_timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t htey_hard_timer_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
void raise_signal(conf_object_t *obj){
	htey_hard_timer_device *dev = ((conf_object_t *)obj)->obj;
	interrupt_signal_t interrupt_signal;
	int i = 0;
	for(; i < 16; i++)
		interrupt_signal.c6k_signal.ifr[i] = Prev_level;
	//printf("%s, %d, irqnum = %d\n", __func__, __LINE__, dev->irq_num);
	interrupt_signal.c6k_signal.ifr[dev->irq_num] = High_level;
	if(dev->signal_iface)
		dev->signal_iface->signal(dev->signal, &interrupt_signal);
}

void timer_func(void *obj){
	htey_hard_timer_device *dev = ((conf_object_t *)obj)->obj;
	static int i = 0;
	if(dev->regs.timer_2r5 & 0x8000){
			i++;
			//if(i < 50000){
				//if ((i == 401) && (dev->kgl_iface != NULL))
					//dev->kgl_iface->kgl_ctrl(dev->kgl, TUOLUO, 0, NULL);
				raise_signal(obj);
			//}
	}

}
static conf_object_t* new_htey_hard_timer(char *obj_name)
{
	htey_hard_timer_device* dev = skyeye_mm_zero(sizeof(htey_hard_timer_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	system_register_timer_handler(dev->obj, 5, (time_sched_t)timer_func, SCHED_MS | SCHED_NORMAL);
	return dev->obj;
}
static exception_t free_htey_hard_timer(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	htey_hard_timer_device *dev = obj->obj;
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
	htey_hard_timer_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static exception_t kgl_intf_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	htey_hard_timer_device *dev = obj->obj;
	dev->kgl = obj2;
	dev->kgl_iface = (htey_kgl_intf *)SKY_get_iface(dev->kgl, HTEY_KGL_INTF);
	if (dev->kgl_iface == NULL){
		if (dev->kgl == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", HTEY_KGL_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", HTEY_KGL_INTF, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}
 
static exception_t kgl_intf_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	htey_hard_timer_device *dev = obj->obj;
	*obj2 = dev->kgl;
	*port = NULL;
	return No_exp;
}


static attr_value_t get_attr_int_number(void* arg, conf_object_t* obj, attr_value_t* idx){
	htey_hard_timer_device *dev = obj->obj;
	attr_value_t irq_num = SKY_make_attr_uinteger(dev->irq_num);
	return irq_num;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	htey_hard_timer_device *dev = obj->obj;
	dev->irq_num = SKY_attr_uinteger(*value);
	return No_exp;
}

static class_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");
}

void init_htey_hard_timer(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "htey_hard_timer",
		.class_desc = "htey_hard_timer",
		.new_instance = new_htey_hard_timer,
		.free_instance = free_htey_hard_timer
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = htey_hard_timer_read,
		.write = htey_hard_timer_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = htey_hard_timer_get_regvalue_by_id,
		.get_regname_by_id = htey_hard_timer_get_regname_by_id,
		.set_regvalue_by_id = htey_hard_timer_set_regvalue_by_id,
		.get_regnum = htey_hard_timer_get_regnum,
		.get_regid_by_name = htey_hard_timer_get_regid_by_name
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
		(struct ConnectDescription ){
			.name = HTEY_KGL_INTF,
			.set = kgl_intf_set,
			.get = kgl_intf_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	class_register_attribute(clss);
}

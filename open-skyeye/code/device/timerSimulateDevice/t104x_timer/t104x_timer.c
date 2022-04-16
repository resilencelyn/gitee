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
#include <skyeye_system.h>
#define DEBUG
#include <skyeye_log.h>
#include "t104x_timer.h"

static void timer1_callback(conf_object_t *opaque)
{
	timer_device *dev = opaque->obj;
	if (dev->regs.timer_ctrl & TIMER_ENABLE)
	{
		if (dev->regs.timer_intc & INT_ENABLE)
		{
			if(dev->signal.obj)
				dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.num[0]);
		}
	}
}

static void timer2_callback(conf_object_t *opaque)
{
	timer_device *dev = opaque->obj;
	if (dev->regs.timer_ctrl & TIMER_ENABLE)
	{
		if (dev->regs.timer_intc & INT_ENABLE)
		{
			if(dev->signal.obj)
				dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.num[1]);
		}
	}
}

static exception_t timer_device_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	timer_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	uint32_t value = *(uint32_t *)buf;

	switch (offset)
	{
		case TIMER_CTRL:
			regs->timer_ctrl = value;
			if (dev->regs.timer_ctrl & TIMER_ENABLE)
			{
				if (dev->handle[1])
					system_enable_timer(dev->handle[1]);
				if (dev->handle[0])
					system_enable_timer(dev->handle[0]);
			}
			break;
		case TIMER_INTC:
			regs->timer_intc = value;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in timer_device\n", offset);
			break;
	}
	return No_exp;
}

static exception_t timer_device_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	timer_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;

	switch (offset)
	{
		case TIMER_CTRL:
			*(uint32_t *)buf = regs->timer_ctrl;
			break;
		case TIMER_INTC:
			*(uint32_t *)buf = regs->timer_intc;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in timer_device\n", offset);
			break;
	}
	return No_exp;
}

static char* timer_device_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t timer_device_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t timer_device_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t timer_device_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t timer_device_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_timer_device(const char *obj_name)
{
	timer_device* dev = skyeye_mm_zero(sizeof(timer_device));
	dev->obj = new_conf_object(obj_name, dev);


	return dev->obj;
}

static exception_t config_timer_device(conf_object_t* opaque)
{
	timer_device *dev =  opaque->obj;

	if (dev->time[1] > 0)
	{
		dev->handle[1] = system_register_timer_handler(opaque, dev->time[1], (time_sched_t)timer2_callback, SCHED_US|SCHED_NORMAL);
		if (dev->handle[1] != NULL)
			system_disable_timer(dev->handle[1]);
	}

	if (dev->time[0] > 0)
	{
		dev->handle[0] = system_register_timer_handler(opaque, dev->time[0], (time_sched_t)timer1_callback, SCHED_US|SCHED_NORMAL);
		if (dev->handle[0] != NULL)
			system_disable_timer(dev->handle[0]);
	}
	return No_exp;
}

static exception_t free_timer_device(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	timer_device *dev =  obj->obj;
	dev->signal.obj = obj2;
	dev->signal.intf = (general_signal_intf *)SKY_get_iface(dev->signal.obj, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal.intf == NULL){
		if (dev->signal.obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	timer_device *dev =  obj->obj;
	*obj2 = dev->signal.obj;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_time1(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	timer_device *dev =  conf_obj->obj;
	attr_value_t time = SKY_make_attr_uinteger(dev->time[0]);
	return time;
}

static exception_t set_attr_time1(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	timer_device *dev =  conf_obj->obj;
	dev->time[0] = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_time2(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	timer_device *dev =  conf_obj->obj;
	attr_value_t time = SKY_make_attr_uinteger(dev->time[1]);
	return time;
}

static exception_t set_attr_time2(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	timer_device *dev =  conf_obj->obj;
	dev->time[1] = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_int_number1(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	timer_device *dev =  conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->signal.num[0]);
	return int_number;
}

static exception_t set_attr_int_number1(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	timer_device *dev =  conf_obj->obj;
	dev->signal.num[0] = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_int_number2(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	timer_device *dev =  conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->signal.num[1]);
	return int_number;
}

static exception_t set_attr_int_number2(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	timer_device *dev =  conf_obj->obj;
	dev->signal.num[1] = SKY_attr_uinteger(*value);
	return No_exp;
}

static void timer_device_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "time1", get_attr_time1, NULL, set_attr_time1, NULL, SKY_Attr_Optional, "uinteger", "set time1(us) to signal interrupt");
	SKY_register_attribute(clss, "time2", get_attr_time2, NULL, set_attr_time2, NULL, SKY_Attr_Optional, "uinteger", "set time2(us) to signal interrupt");
	SKY_register_attribute(clss, "int_number1", get_attr_int_number1, NULL, set_attr_int_number1, NULL, SKY_Attr_Optional, "uinteger", "set interrupt number 1");
	SKY_register_attribute(clss, "int_number2", get_attr_int_number2, NULL, set_attr_int_number2, NULL, SKY_Attr_Optional, "uinteger", "set interrupt number 2");
	return;
}

void init_timer_device(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "t104x_timer",
		.class_desc = "t104x_timer",
		.new_instance = new_timer_device,
		.free_instance = free_timer_device,
		.config_instance = config_timer_device
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = timer_device_read,
		.write = timer_device_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = timer_device_get_regvalue_by_id,
		.get_regname_by_id = timer_device_get_regname_by_id,
		.set_regvalue_by_id = timer_device_set_regvalue_by_id,
		.get_regnum = timer_device_get_regnum,
		.get_regid_by_name = timer_device_get_regid_by_name
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
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = signal_set,
			.get = signal_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	timer_device_register_attribute(clss);
}

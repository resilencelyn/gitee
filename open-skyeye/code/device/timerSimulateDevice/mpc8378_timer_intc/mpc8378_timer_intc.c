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
#include "mpc8378_timer_intc.h"

static timer_intr_callback(conf_object_t *opaque)
{
	mpc8378_timer_intc_device *dev = opaque->obj;
	if(dev->signal.obj)
		dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.num);
}

static exception_t mpc8378_timer_intc_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	mpc8378_timer_intc_device *dev = opaque->obj;
	return No_exp;
}

static exception_t mpc8378_timer_intc_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	mpc8378_timer_intc_device *dev = opaque->obj;

	return No_exp;
}

static conf_object_t* new_mpc8378_timer_intc(const char *obj_name)
{
	mpc8378_timer_intc_device* dev = skyeye_mm_zero(sizeof(mpc8378_timer_intc_device));
	dev->obj = new_conf_object(obj_name, dev);

	return dev->obj;
}

static exception_t config_mpc8378_timer_intc(conf_object_t* opaque)
{
	mpc8378_timer_intc_device *dev =  opaque->obj;
	if (dev->time > 0)
		system_register_timer_handler(opaque, dev->time, (time_sched_t)timer_intr_callback, SCHED_US|SCHED_NORMAL);

	return No_exp;
}

static exception_t free_mpc8378_timer_intc(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	mpc8378_timer_intc_device *dev =  obj->obj;
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
	mpc8378_timer_intc_device *dev =  obj->obj;
	*obj2 = dev->signal.obj;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_time(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	mpc8378_timer_intc_device *dev =  conf_obj->obj;
	attr_value_t time = SKY_make_attr_uinteger(dev->time);
	return time;
}

static exception_t set_attr_time(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	mpc8378_timer_intc_device *dev =  conf_obj->obj;
	dev->time = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	mpc8378_timer_intc_device *dev =  conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->signal.num);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	mpc8378_timer_intc_device *dev =  conf_obj->obj;
	dev->signal.num = SKY_attr_uinteger(*value);
	return No_exp;
}

static void mpc8378_timer_intc_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "time", get_attr_time, NULL, set_attr_time, NULL, SKY_Attr_Optional, "uinteger", "set time(us) to signal interrupt");
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "set interrupt number");
	return;
}

void init_mpc8378_timer_intc(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "mpc8378_timer_intc",
		.class_desc = "mpc8378_timer_intc",
		.new_instance = new_mpc8378_timer_intc,
		.free_instance = free_mpc8378_timer_intc,
		.config_instance = config_mpc8378_timer_intc
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = mpc8378_timer_intc_read,
		.write = mpc8378_timer_intc_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
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

	mpc8378_timer_intc_register_attribute(clss);
}

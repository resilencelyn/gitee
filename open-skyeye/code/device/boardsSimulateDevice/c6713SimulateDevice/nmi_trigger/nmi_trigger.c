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
#include "nmi_trigger.h"

static exception_t nmi_trigger_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	nmi_trigger_device *dev = opaque->obj;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in nmi_trigger\n", offset);
			break;
	}
	return 0;
}

static exception_t nmi_trigger_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	nmi_trigger_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in nmi_trigger\n", offset);
			break;
	}
	return 0;
}

static char* nmi_trigger_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t nmi_trigger_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	nmi_trigger_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t nmi_trigger_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t nmi_trigger_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	nmi_trigger_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t nmi_trigger_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static void timer_func(void *obj) {
	nmi_trigger_device *dev = ((conf_object_t *)obj)->obj;
	core_signal_intf* core_signal = dev->signal_iface;
	interrupt_signal_t interrupt_signal;

	int i=0;
	for(; i < 16; i++)
		interrupt_signal.c6k_signal.ifr[i] = Prev_level;

	// interrupt number : 1
	interrupt_signal.c6k_signal.ifr[1] = High_level;

	if (core_signal) {
		core_signal->signal(dev->signal, &interrupt_signal);
	}
}

static conf_object_t* new_nmi_trigger(char *obj_name)
{
	nmi_trigger_device* dev = skyeye_mm_zero(sizeof(nmi_trigger_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	dev->handle = system_register_timer_handler(dev->obj, (uint32_t)400, (time_sched_t)timer_func, SCHED_MS|SCHED_NORMAL);

	return dev->obj;
}

static exception_t free_nmi_trigger(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	nmi_trigger_device* dev = obj->obj;
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

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2,const char **port, int index)
{
	nmi_trigger_device* dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;

	return No_exp;
}

void init_nmi_trigger(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "nmi_trigger",
		.class_desc = "nmi_trigger",
		.new_instance = new_nmi_trigger,
		.free_instance = free_nmi_trigger
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = nmi_trigger_read,
		.write = nmi_trigger_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = nmi_trigger_get_regvalue_by_id,
		.get_regname_by_id = nmi_trigger_get_regname_by_id,
		.set_regvalue_by_id = nmi_trigger_set_regvalue_by_id,
		.get_regnum = nmi_trigger_get_regnum,
		.get_regid_by_name = nmi_trigger_get_regid_by_name
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
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

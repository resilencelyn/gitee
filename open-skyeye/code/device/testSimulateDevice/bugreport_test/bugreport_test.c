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
#include "bugreport_test.h"

#define TEST_1 0
#define TEST_2 0
#define TEST_3 0

static exception_t bugreport_test_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	bugreport_test_device *dev = ((conf_object_t *)opaque)->obj;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in bugreport_test\n", offset);
			break;
	}
	return 0;
}

static exception_t bugreport_test_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	bugreport_test_device *dev = ((conf_object_t *)opaque)->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in bugreport_test\n", offset);
			break;
	}
	return 0;
}

static char* bugreport_test_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t bugreport_test_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	bugreport_test_device *dev = ((conf_object_t *)conf_obj)->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t bugreport_test_get_regnum(conf_object_t *conf_obj)
{
	return 0;
}

static uint32_t bugreport_test_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	bugreport_test_device *dev = ((conf_object_t *)conf_obj)->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t bugreport_test_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if (strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static void timer_func(void *obj, time_data_t *data)
{
	bugreport_test_device *dev = ((conf_object_t *)obj)->obj;
#if TEST_1
	int * f = NULL;
	*f = 0;
#endif
}

static conf_object_t* new_bugreport_test(const char *obj_name)
{
	bugreport_test_device* dev = skyeye_mm_zero(sizeof(bugreport_test_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

#if TEST_2
	int * f = NULL;
	*f = 0;
#endif

	system_register_timer_handler(dev->obj, 50, (time_sched_t)timer_func, SCHED_MS | SCHED_NORMAL);

	return dev->obj;
}

static exception_t free_bugreport_test(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_bugreport_test(void)
{
#if TEST_3
	int * f = NULL;
	*f = 0;
#endif

	static skyeye_class_t class_data =
	{
		.class_name = "bugreport_test",
		.class_desc = "bugreport_test",
		.new_instance = new_bugreport_test,
		.free_instance = free_bugreport_test
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = bugreport_test_read,
		.write = bugreport_test_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = bugreport_test_get_regvalue_by_id,
		.get_regname_by_id = bugreport_test_get_regname_by_id,
		.set_regvalue_by_id = bugreport_test_set_regvalue_by_id,
		.get_regnum = bugreport_test_get_regnum,
		.get_regid_by_name = bugreport_test_get_regid_by_name
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
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

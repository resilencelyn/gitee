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
#include <skyeye_api.h>
#include "api_test_dev.h"

static exception_t api_test_dev_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	api_test_dev_device *dev = opaque->obj;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in api_test_dev\n", offset);
			break;
	}
	return 0;
}

static exception_t api_test_dev_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	api_test_dev_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in api_test_dev\n", offset);
			break;
	}
	return 0;
}

static char* api_test_dev_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t api_test_dev_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	api_test_dev_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t api_test_dev_get_regnum(conf_object_t *conf_obj)
{
	//return sizeof(struct registers) / 4;
	return 0;
}

static uint32_t api_test_dev_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	api_test_dev_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t api_test_dev_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if (strcmp(name, regs_name[i]) == 0) {
			return i;
		}
	}
	return regnum;
}

static void calc_timer_func(void *obj)
{
	api_test_dev_device *dev = ((conf_object_t *)obj)->obj;
	double curr_time = 0.0f;

	curr_time = WIN_get_simulation_time("c67x_core_0", NULL);
	printf(" WIN_get_simulation_time method 1: %f\n", curr_time);

	curr_time = WIN_get_simulation_time(NULL, dev->obj);
	printf(" WIN_get_simulation_time method 2: %f\n\n", curr_time);

	uint64_t cnt1, cnt2;
	cnt1 = get_simulation_insn_number("c67x_core_0", NULL);
	printf(" get_simulation_insn_number method 1: %lld\n", cnt1);

	cnt2 = get_simulation_insn_number(NULL, dev->obj);
	printf(" get_simulation_insn_number method 2: %lld\n\n", cnt2);

	printf(" WIN_get_work_full_path: %s\n", WIN_get_work_full_path());
	printf(" WIN_get_work_path: %s\n\n", WIN_get_work_path());

	uint32_t pc;
	pc = WIN_get_CPU_PC("c67x_core_0", NULL);
	printf(" WIN_get_CPU_PC method1 pc: 0x%x\n", pc);

	pc = WIN_get_CPU_PC(NULL, dev->obj);
	printf(" WIN_get_CPU_PC method2 pc: 0x%x\n\n", pc);

	uint32_t data;

	WIN_read_address_bus(NULL, dev->obj, 0x4, &data, 4);
	printf(" WIN_read_address_bus addr method 1: 0x4 data: 0x%x\n", data);
	WIN_read_address_bus("c67x_core_0", NULL, 0x4, &data, 4);
	printf(" WIN_read_address_bus addr method 2: 0x4 data: 0x%x\n\n", data);

	static uint32_t my_cnt = 2;
	WIN_write_address_bus(NULL, dev->obj, 0x8000, &my_cnt, 4);
	WIN_read_address_bus(NULL, dev->obj, 0x8000, &data, 4);
	printf(" WIN_write_address_bus write data: 0x%x\n\n", data);
	my_cnt++;

	printf("===========================================\n\n");
}

static exception_t config_api_test_dev(conf_object_t* conf_obj)
{
	api_test_dev_device *dev = conf_obj->obj;
	system_register_timer_handler(dev->obj, 100, (time_sched_t)calc_timer_func, SCHED_MS | SCHED_NORMAL);

	//WIN_set_script_path("e:\\1\\2\\abc.dat");
}

static conf_object_t* new_api_test_dev(const char *obj_name)
{
	api_test_dev_device* dev = skyeye_mm_zero(sizeof(api_test_dev_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	return dev->obj;
}

static exception_t free_api_test_dev(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_api_test_dev(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "api_test_dev",
		.class_desc = "api_test_dev",
		.new_instance = new_api_test_dev,
		.config_instance = config_api_test_dev,
		.free_instance = free_api_test_dev
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = api_test_dev_read,
		.write = api_test_dev_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = api_test_dev_get_regvalue_by_id,
		.get_regname_by_id = api_test_dev_get_regname_by_id,
		.set_regvalue_by_id = api_test_dev_set_regvalue_by_id,
		.get_regnum = api_test_dev_get_regnum,
		.get_regid_by_name = api_test_dev_get_regid_by_name
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

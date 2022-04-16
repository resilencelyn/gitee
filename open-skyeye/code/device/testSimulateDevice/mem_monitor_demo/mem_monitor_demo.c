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
#include <memory_monitor.h>
#include "mem_monitor_demo.h"

static exception_t mem_monitor_demo_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	mem_monitor_demo_device *dev = opaque->obj;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mem_monitor_demo\n", offset);
			break;
	}
	return 0;
}

static exception_t mem_monitor_demo_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	mem_monitor_demo_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in mem_monitor_demo\n", offset);
			break;
	}
	return 0;
}

static char* mem_monitor_demo_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t mem_monitor_demo_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	mem_monitor_demo_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t mem_monitor_demo_get_regnum(conf_object_t *conf_obj)
{
	return 0;
}

static uint32_t mem_monitor_demo_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	mem_monitor_demo_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t mem_monitor_demo_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_mem_monitor_demo(const char *obj_name)
{
	mem_monitor_demo_device* dev = skyeye_mm_zero(sizeof(mem_monitor_demo_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	return dev->obj;
}


void memory_callback(conf_object_t *conf_obj, uint32_t mm_type, uint32_t addr, void * val, size_t count)
{
	mem_monitor_demo_device *dev = conf_obj->obj;

	static uint32_t m_count = 0;

	m_count++;

	printf("m_count: %d\n", m_count);
	switch (mm_type) {
	case MO_READ_OP :
		printf("MO_READ_OP\n");
		printf("%s addr: 0x%x\n", __func__, addr);
		break;
	case MO_WRITE_OP:
		printf("MO_WRITE_OP\n");
		printf("%s addr: 0x%x val: %lf 0x%llx\n", __func__, addr, *(double *)val, *(uint64_t *)val);
		break;
	case MO_COND_OP :
		printf("MO_COND_OP\n");
		printf("%s addr: 0x%x val: %lf 0x%llx\n", __func__, addr, *(double *)val, *(uint64_t *)val);
		break;
	}

	if (m_count == 7) {
		printf("SkyEye_Memory_Monitor_Remove MO_READ_OP\n");
		SkyEye_Memory_Monitor_Remove("memory_space_0", MO_READ_OP, 0x69e0);
	}
	if (m_count == 12) {
		printf("SkyEye_Memory_Monitor MO_READ_OP\n");
		SkyEye_Memory_Monitor(conf_obj, "memory_space_0", MO_READ_OP, 0x69e0, OP_DOUBLE, No_Cond, NULL, &memory_callback);
	}
	if (m_count == 30) {
		printf("SkyEye_Memory_Monitor_Remove MO_COND_OP | MO_READ_OP\n");
		SkyEye_Memory_Monitor_Remove("memory_space_0", MO_COND_OP | MO_READ_OP, 0x69e0);
	}

	printf("\n");
}

static exception_t config_mem_monitor_demo(conf_object_t* opaque)
{
	mem_monitor_demo_device *dev = opaque->obj;

	printf("\nMemory Monitor Test\n");

	double cond_value = 123.359;
	printf("cond_value: %lf 0x%llx\n", cond_value, *(uint64_t *)&cond_value);

	printf("SkyEye_Memory_Monitor MO_READ_OP | MO_WRITE_OP | MO_COND_OP\n\n");
	SkyEye_Memory_Monitor(opaque, "memory_space_0", MO_READ_OP | MO_WRITE_OP | MO_COND_OP, 0x69e0, OP_DOUBLE, Cond_GTE, MO_VALUE(cond_value), &memory_callback);

	return No_exp;
}

static exception_t free_mem_monitor_demo(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_mem_monitor_demo(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "mem_monitor_demo",
		.class_desc = "mem_monitor_demo",
		.new_instance = new_mem_monitor_demo,
		.config_instance = config_mem_monitor_demo,
		.free_instance = free_mem_monitor_demo,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = mem_monitor_demo_read,
		.write = mem_monitor_demo_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = mem_monitor_demo_get_regvalue_by_id,
		.get_regname_by_id = mem_monitor_demo_get_regname_by_id,
		.set_regvalue_by_id = mem_monitor_demo_set_regvalue_by_id,
		.get_regnum = mem_monitor_demo_get_regnum,
		.get_regid_by_name = mem_monitor_demo_get_regid_by_name
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

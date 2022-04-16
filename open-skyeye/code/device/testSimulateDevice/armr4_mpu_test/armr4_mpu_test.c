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
#include "armr4_mpu_test.h"

static exception_t armr4_mpu_test_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	armr4_mpu_test_device *dev = opaque->obj;
	switch (offset)
	{
		case REG1:
			*(uint32_t *)buf = dev->regs.reg1;
			break;
		case REG2:
			*(uint32_t *)buf = dev->regs.reg2;
			break;
		case REG3:
			*(uint32_t *)buf = dev->regs.reg3;
			break;
		case REG4:
			*(uint32_t *)buf = dev->regs.reg4;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in armr4_mpu_test\n", offset);
			break;
	}

	//printf("%s  offset: 0x%x val: 0x%x\n", __func__, offset, *(uint32_t *)buf);
	return 0;
}

static exception_t armr4_mpu_test_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	armr4_mpu_test_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

	//printf("%s offset: 0x%x val: 0x%x\n", __func__, offset, *(uint32_t *)buf);

	switch (offset)
	{
		case REG1:
			dev->regs.reg1 = val;
			break;
		case REG2:
			dev->regs.reg2 = val;
			break;
		case REG3:
			dev->regs.reg3 = val;
			break;
		case REG4:
			dev->regs.reg4 = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in armr4_mpu_test\n", offset);
			break;
	}
	return 0;
}

static char* armr4_mpu_test_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t armr4_mpu_test_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	armr4_mpu_test_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t armr4_mpu_test_get_regnum(conf_object_t *conf_obj)
{
	return sizeof(struct registers) / 4;
}

static uint32_t armr4_mpu_test_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	armr4_mpu_test_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t armr4_mpu_test_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if (strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_armr4_mpu_test(const char *obj_name)
{
	armr4_mpu_test_device* dev = skyeye_mm_zero(sizeof(armr4_mpu_test_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	return dev->obj;
}

static exception_t free_armr4_mpu_test(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static attr_value_t get_attr_mode(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	armr4_mpu_test_device *dev = conf_obj->obj;
	attr_value_t mode = SKY_make_attr_uinteger(dev->test_mode);
	return mode;
}

static exception_t set_attr_mode(void* arg, conf_object_t* conf_obj,attr_value_t* value, attr_value_t* idx){
	armr4_mpu_test_device *dev = conf_obj->obj;
	dev->test_mode = SKY_attr_uinteger(*value);

	dev->regs.reg1 = dev->test_mode;
	return No_exp;
}

static class_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss,
			"mode",
			get_attr_mode, NULL,
			set_attr_mode, NULL,
			SKY_Attr_Optional,
			"uinteger",
			"test number of mpu test mode");
}

void init_armr4_mpu_test(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "armr4_mpu_test",
		.class_desc = "armr4_mpu_test",
		.new_instance = new_armr4_mpu_test,
		.free_instance = free_armr4_mpu_test
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = armr4_mpu_test_read,
		.write = armr4_mpu_test_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = armr4_mpu_test_get_regvalue_by_id,
		.get_regname_by_id = armr4_mpu_test_get_regname_by_id,
		.set_regvalue_by_id = armr4_mpu_test_set_regvalue_by_id,
		.get_regnum = armr4_mpu_test_get_regnum,
		.get_regid_by_name = armr4_mpu_test_get_regid_by_name
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

	class_register_attribute(clss);
}

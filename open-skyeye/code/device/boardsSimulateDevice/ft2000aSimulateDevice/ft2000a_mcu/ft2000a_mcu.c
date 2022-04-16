/*************************************************************************
	> File Name    : ft2000a_mcu.c
	> Author       : jiachao
	> Mail         : jiachao@163.com
	> Created Time : 2019/11/25 10:10:41
	> This is ft-2000A Memory Controller Unit(MCU) register device
************************************************************************/

#include <skyeye_lock.h>
#include <skyeye_types.h>
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
#include "ft2000a_mcu.h"

static exception_t ft2000a_mcu_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_mcu_device *dev = opaque->obj;
	struct mcu_registers *regs = &dev->regs;
	uint32_t *data = (uint32_t *)buf;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in ft2000a_mcu\n", offset);
			break;
	}

	return No_exp;
}

static exception_t ft2000a_mcu_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_mcu_device *dev = opaque->obj;
	struct mcu_registers *regs = &dev->regs;
	uint32_t data = *(uint32_t *)buf;

	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in ft2000a_mcu\n", offset);
			break;
	}
	return No_exp;
}

static char* ft2000a_mcu_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t ft2000a_mcu_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	ft2000a_mcu_device *dev = conf_obj->obj;
	struct mcu_registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t ft2000a_mcu_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct mcu_registers) / 4;
}

static uint32_t ft2000a_mcu_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	ft2000a_mcu_device *dev = conf_obj->obj;
	struct mcu_registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t ft2000a_mcu_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct mcu_registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t ft2000a_mcu_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

void init_mcu_register(struct mcu_registers *regs)
{
}

static conf_object_t* new_ft2000a_mcu(const char *obj_name)
{
	ft2000a_mcu_device* dev = skyeye_mm_zero(sizeof(ft2000a_mcu_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct mcu_registers *regs = &(dev->regs);
	init_mcu_register(regs);
	return dev->obj;
}

static exception_t free_ft2000a_mcu(conf_object_t* opaque)
{
	ft2000a_mcu_device *dev = opaque->obj;
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t config_ft2000a_mcu(conf_object_t* opaque)
{
	ft2000a_mcu_device *dev = opaque->obj;
	return No_exp;
}

static void ft2000a_mcu_register_attribute(conf_class_t* clss)
{
}

void init_ft2000a_mcu(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "ft2000a_mcu",
		.class_desc = "ft2000a_mcu",
		.new_instance = new_ft2000a_mcu,
		.free_instance = free_ft2000a_mcu,
		.config_instance = config_ft2000a_mcu
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = ft2000a_mcu_read,
		.write = ft2000a_mcu_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = ft2000a_mcu_get_regvalue_by_id,
		.get_regname_by_id = ft2000a_mcu_get_regname_by_id,
		.set_regvalue_by_id = ft2000a_mcu_set_regvalue_by_id,
		.get_regnum = ft2000a_mcu_get_regnum,
		.get_regid_by_name = ft2000a_mcu_get_regid_by_name,
		.get_regoffset_by_id = ft2000a_mcu_get_regoffset_by_id
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
	ft2000a_mcu_register_attribute(clss);
}

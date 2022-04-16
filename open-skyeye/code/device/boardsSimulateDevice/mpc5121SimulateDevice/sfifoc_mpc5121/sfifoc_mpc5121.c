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
#include "sfifoc_mpc5121.h"
static exception_t sfifoc_mpc5121_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sfifoc_mpc5121_device *dev = opaque->obj;
	switch (offset)
	{
		case FIFO_CMD:
			*(uint32_t *)buf = dev->regs.fifo_cmd;
			break;
		case FIFO_INT:
			*(uint32_t *)buf = dev->regs.fifo_int;
			break;
		case FIFO_DMA:
			*(uint32_t *)buf = dev->regs.fifo_dma;
			break;
		case FIFO_AXE:
			*(uint32_t *)buf = dev->regs.fifo_axe;
			break;
		case FIFO_DEBUG:
			*(uint32_t *)buf = dev->regs.fifo_debug;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sfifoc_mpc5121\n", offset);
			break;
	}
	return 0;
}
static exception_t sfifoc_mpc5121_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sfifoc_mpc5121_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case FIFO_CMD:
			dev->regs.fifo_cmd = val;
			break;
		case FIFO_INT:
			dev->regs.fifo_int = val;
			break;
		case FIFO_DMA:
			dev->regs.fifo_dma = val;
			break;
		case FIFO_AXE:
			dev->regs.fifo_axe = val;
			break;
		case FIFO_DEBUG:
			dev->regs.fifo_debug = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in sfifoc_mpc5121\n", offset);
			break;
	}
	return 0;
}
static char* sfifoc_mpc5121_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t sfifoc_mpc5121_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	sfifoc_mpc5121_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t sfifoc_mpc5121_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t sfifoc_mpc5121_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	sfifoc_mpc5121_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t sfifoc_mpc5121_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_sfifoc_mpc5121(char *obj_name)
{
	sfifoc_mpc5121_device* dev = skyeye_mm_zero(sizeof(sfifoc_mpc5121_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	return dev->obj;
}
static exception_t free_sfifoc_mpc5121(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void sfifoc_write_register(conf_object_t *obj, mpc5121_sfifoc_reg_t reg, uint32_t value, reg_op_t op){
	if(op == H_OP)
		sfifoc_mpc5121_write(obj, (generic_address_t)reg, &value, 4);
	else
		sfifoc_mpc5121_set_regvalue_by_id(obj, value, reg / 4);
}

void sfifoc_read_register(conf_object_t *obj, mpc5121_sfifoc_reg_t reg, uint32_t *value, reg_op_t op){
	if(op == H_OP)
		sfifoc_mpc5121_read(obj, (generic_address_t)reg, value, 4);
	else
		*value = sfifoc_mpc5121_get_regvalue_by_id(obj, reg / 4);
}

void init_sfifoc_mpc5121(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "mpc5121_sfifoc",
		.class_desc = "mpc5121_sfifoc",
		.new_instance = new_sfifoc_mpc5121,
		.free_instance = free_sfifoc_mpc5121,
		.module_type = SKYML_ONCHIP(SC_mpc5121),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = sfifoc_mpc5121_read,
		.write = sfifoc_mpc5121_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = sfifoc_mpc5121_get_regvalue_by_id,
		.get_regname_by_id = sfifoc_mpc5121_get_regname_by_id,
		.set_regvalue_by_id = sfifoc_mpc5121_set_regvalue_by_id,
		.get_regnum = sfifoc_mpc5121_get_regnum,
		.get_regid_by_name = sfifoc_mpc5121_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static mpc5121_sfifoc_intf sfifoc_intf =
	{
		.write_register = sfifoc_write_register,
		.read_register = sfifoc_read_register,
	};
	SKY_register_iface(clss, MPC5121_SFIFOC_INTF, &sfifoc_intf);

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
		(struct InterfaceDescription)
		{
			.name = MPC5121_SFIFOC_INTF,
			.iface = &sfifoc_intf
		},

	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

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
#include "rm48x_io.h"

static exception_t rm48x_io_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_io_device *dev = opaque->obj;
	
	if(offset >= 0x110 && offset <= 0x188){
		*(uint32_t *)buf = dev->regs.pinmmrnn[(offset - 0x110)/4];
		return No_exp;
	}
	
	switch (offset)
	{
		case REVISION:
			*(uint32_t *)buf = dev->regs.revision;
			break;
		case ENDIAN:
			*(uint32_t *)buf = dev->regs.endian;
			break;
		case KICK0:
			*(uint32_t *)buf = dev->regs.kick0;
			break;
		case KICK1:
			*(uint32_t *)buf = dev->regs.kick1;
			break;
		case ERR_RAW_STATUS:
			*(uint32_t *)buf = dev->regs.err_raw_status;
			break;
		case ERR_ENABLED_STATUS:
			*(uint32_t *)buf = dev->regs.err_enabled_status;
			break;
		case ERR_ENABLE_REG:
			*(uint32_t *)buf = dev->regs.err_enable_reg;
			break;
		case ERR_ENABLE_CLR:
			*(uint32_t *)buf = dev->regs.err_enable_clr;
			break;
		case FAULT_ADDRESS:
			*(uint32_t *)buf = dev->regs.fault_address;
			break;
		case FAULT_STATUS:
			*(uint32_t *)buf = dev->regs.fault_status;
			break;
		case FAULT_CLEAR:
			*(uint32_t *)buf = dev->regs.fault_clear;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rm48x_io\n", offset);
			break;
	}
	return 0;
}

static exception_t rm48x_io_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_io_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	
	if (offset >= 0x110 && offset <= 0x188) {
		dev->regs.pinmmrnn[(offset - 0x110)/4] = val;
		return No_exp;
	}

	switch (offset)
	{
		case REVISION:
			dev->regs.revision = val;
			break;
		case ENDIAN:
			dev->regs.endian = val;
			break;
		case KICK0:
			dev->regs.kick0 = val;
			break;
		case KICK1:
			dev->regs.kick1 = val;
			break;
		case ERR_RAW_STATUS:
			dev->regs.err_raw_status = val;
			break;
		case ERR_ENABLED_STATUS:
			dev->regs.err_enabled_status = val;
			break;
		case ERR_ENABLE_REG:
			dev->regs.err_enable_reg = val;
			break;
		case ERR_ENABLE_CLR:
			dev->regs.err_enable_clr = val;
			break;
		case FAULT_ADDRESS:
			dev->regs.fault_address = val;
			break;
		case FAULT_STATUS:
			dev->regs.fault_status = val;
			break;
		case FAULT_CLEAR:
			dev->regs.fault_clear = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rm48x_io\n", offset);
			break;
	}
	return 0;
}

static char* rm48x_io_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t rm48x_io_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	rm48x_io_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t rm48x_io_get_regnum(conf_object_t *conf_obj)
{
	return sizeof(struct registers) / 4;
}

static uint32_t rm48x_io_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	rm48x_io_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t rm48x_io_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if (strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_rm48x_io(const char *obj_name)
{
	int i;
	rm48x_io_device* dev = skyeye_mm_zero(sizeof(rm48x_io_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->regs.revision = 0x4e840102;
	for(i = 0;i < 124/4; i++)
	{
		dev->regs.pinmmrnn[i] = 0x01010101;
	}
	return dev->obj;
}

static exception_t free_rm48x_io(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_rm48x_io(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "rm48x_io",
		.class_desc = "rm48x_io",
		.new_instance = new_rm48x_io,
		.free_instance = free_rm48x_io
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = rm48x_io_read,
		.write = rm48x_io_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = rm48x_io_get_regvalue_by_id,
		.get_regname_by_id = rm48x_io_get_regname_by_id,
		.set_regvalue_by_id = rm48x_io_set_regvalue_by_id,
		.get_regnum = rm48x_io_get_regnum,
		.get_regid_by_name = rm48x_io_get_regid_by_name
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

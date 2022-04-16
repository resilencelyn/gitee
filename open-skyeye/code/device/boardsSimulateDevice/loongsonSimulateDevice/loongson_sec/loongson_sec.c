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
#include <skyeye_log.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>
#include <skyeye_uart_ops.h>
#include <skyeye_system.h>
#define DEBUG
#include "loongson_sec.h"


// Register read operation method implementation
static exception_t loongson_sec_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_sec_device *dev = (loongson_sec_device *)conf_obj->obj;
	switch (offset)
	{
		case WD_CFG:
			*(uint32_t*)buf = dev->regs.wd_cfg;
			break;
		case WD_FEED:
			*(uint32_t*)buf = dev->regs.wd_feed;
			break;
		case SEC_CMD:
			*(uint32_t*)buf = dev->regs.sec_cmd;
			break;
		case SEC_STS:
			*(uint32_t*)buf = dev->regs.sec_sts;
			break;
		case SEC_GPR0:
			*(uint32_t*)buf = dev->regs.sec_gpr0;
			break;
		case SEC_GPR1:
			*(uint32_t*)buf = dev->regs.sec_gpr1;
			break;
		case SEC_GATE_SRAM:
			*(uint32_t*)buf = dev->regs.sec_gate_sram;
			break;
		case SEC_GATE_FLASH:
			*(uint32_t*)buf = dev->regs.sec_gate_flash;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_sec\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_sec_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_sec_device *dev = (loongson_sec_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case WD_CFG:
			dev->regs.wd_cfg = val;
			break;
			case WD_FEED:
			dev->regs.wd_feed = val;
			break;
		case SEC_CMD:
			dev->regs.sec_cmd = val;
			break;
		case SEC_STS:
			dev->regs.sec_sts = val;
			break;
		case SEC_GPR0:
			dev->regs.sec_gpr0 = val;
			break;
		case SEC_GPR1:
			dev->regs.sec_gpr1 = val;
			break;
		case SEC_GATE_SRAM:
			dev->regs.sec_gate_sram = val;
			break;
		case SEC_GATE_FLASH:
			dev->regs.sec_gate_flash = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_sec\n", offset);
			break;
	}
	return No_exp;
}

// Register interface: gets the register name based on the register ID
static char* loongson_sec_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

// Register interface: set register value according to register ID
static exception_t loongson_sec_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	loongson_sec_device *dev = (loongson_sec_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

// Register interface: gets how many registers the device has
static uint32_t loongson_sec_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

// Register interface: gets the register value based on the register ID
static uint32_t loongson_sec_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	loongson_sec_device *dev = (loongson_sec_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

// Register interface: gets the register ID based on the register name
static uint32_t loongson_sec_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_loongson_sec(const char *obj_name)
{
	loongson_sec_device* dev = skyeye_mm_zero(sizeof(loongson_sec_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here
	// Register initialization
	struct registers *regs = &(dev->regs);
	dev->regs.wd_cfg = 0x3ffff;
	dev->regs.sec_gate_sram = 0xfffffffc;
	dev->regs.sec_gate_flash = 0xfffffffc;
	
	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_loongson_sec(conf_object_t* conf_obj)
{
	loongson_sec_device *dev = (loongson_sec_device *)conf_obj->obj;
	// TODO: place the code here


	No_exp;
}

// Free the device object to do some memory free
static exception_t free_loongson_sec(conf_object_t* conf_obj)
{
	loongson_sec_device *dev = (loongson_sec_device *)conf_obj->obj;
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_loongson_sec(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "loongson_sec",
		.class_desc      = "",
		.new_instance    = new_loongson_sec,
		.free_instance   = free_loongson_sec,
		.config_instance = config_loongson_sec,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);
	
	// sets the register address read-write method
	static memory_space_intf io_memory_interface =
	{
		.read  = loongson_sec_read,
		.write = loongson_sec_write
	};
	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);
	
	// register access interface (for SkyEye graphical control interface)
	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = loongson_sec_get_regvalue_by_id,
		.get_regname_by_id  = loongson_sec_get_regname_by_id,
		.set_regvalue_by_id = loongson_sec_set_regvalue_by_id,
		.get_regnum         = loongson_sec_get_regnum,
		.get_regid_by_name  = loongson_sec_get_regid_by_name
	};
	SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);
	
	// list of interface methods
	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name  = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory_interface
		},
		(struct InterfaceDescription)
		{
			.name  = SKYEYE_REG_INTF,
			.iface = &reg_interface
		},
	};
	
	// list of interface connection methods
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
}

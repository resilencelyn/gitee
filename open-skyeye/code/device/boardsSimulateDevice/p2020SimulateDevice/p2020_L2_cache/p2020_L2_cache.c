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
#include <skyeye_api.h>
#define DEBUG
#include "p2020_L2_cache.h"

// Register read operation method implementation
static exception_t p2020_L2_cache_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	p2020_L2_cache_device *dev = (p2020_L2_cache_device *)conf_obj->obj;
	switch (offset)
	{
		case L2_Cache_L2CTL :
			*(uint32_t *)buf = dev->regs.L2CTL;
			break;
		case L2_Cache_L2CEWAR0 :
			*(uint32_t *)buf = dev->regs.L2CEWAR0;
			break;
		case L2_Cache_L2CEWCR0 :
			*(uint32_t *)buf = dev->regs.L2CEWCR0;
			break;
		case L2_Cache_L2CEWAR1 :
			*(uint32_t *)buf = dev->regs.L2CEWAR1;
			break;
		case L2_Cache_L2CEWCR1 :
			*(uint32_t *)buf = dev->regs.L2CEWCR1;
			break;
		case L2_Cache_L2CEWAR2 :
			*(uint32_t *)buf = dev->regs.L2CEWAR2;
			break;
		case L2_Cache_L2CEWCR2 :
			*(uint32_t *)buf = dev->regs.L2CEWCR2;
			break;
		case L2_Cache_L2CEWCR3 :
			*(uint32_t *)buf = dev->regs.L2CEWCR3;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in p2020_L2_cache\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t p2020_L2_cache_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	p2020_L2_cache_device *dev = (p2020_L2_cache_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case L2_Cache_L2CTL :
			dev->regs.L2CTL = val;
			if (((val >> (L2_CTL_L2I)) & 1) == L2_Status_CLEAR)
			{
				//L2I置一需要对L2CTL进行复位
				dev->regs.L2CTL = L2_CTL_INIT;
			}
			break;
		case L2_Cache_L2CEWAR0 :
			dev->regs.L2CEWAR0 = val;
			break;
		case L2_Cache_L2CEWCR0 :
			dev->regs.L2CEWCR0 = val;
			break;
		case L2_Cache_L2CEWAR1 :
			dev->regs.L2CEWAR1 = val;
			break;
		case L2_Cache_L2CEWCR1 :
			dev->regs.L2CEWCR1 = val;
			break;
		case L2_Cache_L2CEWAR2 :
			dev->regs.L2CEWAR2 = val;
			break;
		case L2_Cache_L2CEWCR2 :
			dev->regs.L2CEWCR2 = val;
			break;
		case L2_Cache_L2CEWCR3 :
			dev->regs.L2CEWCR3 = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in p2020_L2_cache\n", offset);
			break;
	}
	return No_exp;
}

// Register interface: gets the register name based on the register ID
static char* p2020_L2_cache_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

// Register interface: set register value according to register ID
static exception_t p2020_L2_cache_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	p2020_L2_cache_device *dev = (p2020_L2_cache_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

// Register interface: gets how many registers the device has
static uint32_t p2020_L2_cache_get_regnum(conf_object_t *conf_obj)
{
	return sizeof(struct registers) / 4;
}

// Register interface: gets the register value based on the register ID
static uint32_t p2020_L2_cache_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	p2020_L2_cache_device *dev = (p2020_L2_cache_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

// Register interface: gets the register ID based on the register name
static uint32_t p2020_L2_cache_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if (strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_p2020_L2_cache(const char *obj_name)
{
	p2020_L2_cache_device* dev = skyeye_mm_zero(sizeof(p2020_L2_cache_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here
	dev->regs.L2CTL = 0x20000000;

	// Register initialization
	struct registers *regs = &(dev->regs);
	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_p2020_L2_cache(conf_object_t* conf_obj)
{
	p2020_L2_cache_device *dev = (p2020_L2_cache_device *)conf_obj->obj;
	// TODO: place the code here


	return No_exp;
}

// Free the device object to do some memory free
static exception_t free_p2020_L2_cache(conf_object_t* conf_obj)
{
	p2020_L2_cache_device *dev = (p2020_L2_cache_device *)conf_obj->obj;
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_p2020_L2_cache(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "p2020_L2_cache",
		.class_desc      = "p2020_L2_cache",
		.new_instance    = new_p2020_L2_cache,
		.free_instance   = free_p2020_L2_cache,
		.config_instance = config_p2020_L2_cache,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);
	
	// sets the register address read-write method
	static memory_space_intf io_memory_interface =
	{
		.read  = p2020_L2_cache_read,
		.write = p2020_L2_cache_write
	};
	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);
	
	// register access interface (for SkyEye graphical control interface)
	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = p2020_L2_cache_get_regvalue_by_id,
		.get_regname_by_id  = p2020_L2_cache_get_regname_by_id,
		.set_regvalue_by_id = p2020_L2_cache_set_regvalue_by_id,
		.get_regnum         = p2020_L2_cache_get_regnum,
		.get_regid_by_name  = p2020_L2_cache_get_regid_by_name
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

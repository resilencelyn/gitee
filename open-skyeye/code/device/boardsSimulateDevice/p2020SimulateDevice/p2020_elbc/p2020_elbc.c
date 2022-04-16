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
#include "p2020_elbc.h"

// Timer callback function
void timer_callback(conf_object_t *conf_obj, time_data_t *root_data)
{
	p2020_elbc_device *dev = (p2020_elbc_device *)conf_obj->obj;
	dev->general_signal.iface->raise_signal(dev->general_signal.obj, dev->int_number);
	// TODO: Place the code here
}

// Register read operation method implementation
static exception_t p2020_elbc_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	p2020_elbc_device *dev = (p2020_elbc_device *)conf_obj->obj;
	struct registers *regs = &dev->regs;

	switch (offset)
	{
		case ELBC_BR0:
			*(uint32_t *)buf = regs->elbc_br0;
			break;
		case ELBC_ORG0:
			*(uint32_t *)buf = regs->elbc_org0;
			break;
		case ELBC_BR1:
			*(uint32_t *)buf = regs->elbc_br1;
			break;
		case ELBC_ORG1:
			*(uint32_t *)buf = regs->elbc_org1;
			break;
		case ELBC_BR2:
			*(uint32_t *)buf = regs->elbc_br2;
			break;
		case ELBC_ORG2:
			*(uint32_t *)buf = regs->elbc_org2;
			break;
		case ELBC_BR3:
			*(uint32_t *)buf = regs->elbc_br3;
			break;
		case ELBC_ORG3:
			*(uint32_t *)buf = regs->elbc_org3;
			break;
		case ELBC_BR4:
			*(uint32_t *)buf = regs->elbc_br4;
			break;
		case ELBC_ORG4:
			*(uint32_t *)buf = regs->elbc_org4;
			break;
		case ELBC_LBCR:
			*(uint32_t *)buf = regs->elbc_lbcr;
			break;
		case ELBC_LCRR:
			*(uint32_t *)buf = regs->elbc_lcrr;
			break;
		case ELBC_FMR:
			*(uint32_t *)buf = regs->elbc_fmr;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in p2020_elbc\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t p2020_elbc_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	p2020_elbc_device *dev = (p2020_elbc_device *)conf_obj->obj;
	struct registers *regs = &dev->regs;
	uint32_t val = *(uint32_t *)buf;

	switch (offset)
	{
		case ELBC_BR0:
			regs->elbc_br0 = val;
			break;
		case ELBC_ORG0:
			regs->elbc_org0 = val;
			break;
		case ELBC_BR1:
			regs->elbc_br1 = val;
			break;
		case ELBC_ORG1:
			regs->elbc_org1 = val;
			break;
		case ELBC_BR2:
			regs->elbc_br2 = val;
			break;
		case ELBC_ORG2:
			regs->elbc_org2 = val;
			break;
		case ELBC_BR3:
			regs->elbc_br3 = val;
			break;
		case ELBC_ORG3:
			regs->elbc_org3 = val;
			break;
		case ELBC_BR4:
			regs->elbc_br4 = val;
			break;
		case ELBC_ORG4:
			regs->elbc_org4 = val;
			break;
		case ELBC_LBCR:
			regs->elbc_lbcr = val;
			break;
		case ELBC_LCRR:
			regs->elbc_lcrr = val;
			break;
		case ELBC_FMR:
			regs->elbc_fmr = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in p2020_elbc\n", offset);
			break;
	}
	return No_exp;
}

// Register interface: gets the register name based on the register ID
static char* p2020_elbc_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

// Register interface: set register value according to register ID
static exception_t p2020_elbc_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	p2020_elbc_device *dev = (p2020_elbc_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

// Register interface: gets how many registers the device has
static uint32_t p2020_elbc_get_regnum(conf_object_t *conf_obj)
{
	return sizeof(struct registers) / 4;
}

// Register interface: gets the register value based on the register ID
static uint32_t p2020_elbc_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	p2020_elbc_device *dev = (p2020_elbc_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

// Register interface: gets the register ID based on the register name
static uint32_t p2020_elbc_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if (strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

void init_regs(struct registers *regs)
{
	regs->elbc_lcrr = 0x80000000;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_p2020_elbc(const char *obj_name)
{
	p2020_elbc_device* dev = skyeye_mm_zero(sizeof(p2020_elbc_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here


	// Register initialization
	struct registers *regs = &(dev->regs);
	init_regs(regs);
	// Attribute initialization
	dev->int_number = 3;
	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_p2020_elbc(conf_object_t* conf_obj)
{
	p2020_elbc_device *dev = (p2020_elbc_device *)conf_obj->obj;
	// TODO: place the code here


	dev->timer_handle[0] = system_register_timer_handler(dev->obj, 1, (time_sched_t)timer_callback, SCHED_MS | SCHED_NORMAL);
	system_enable_timer(dev->timer_handle[0]);
	return No_exp;
}

// Free the device object to do some memory free
static exception_t free_p2020_elbc(conf_object_t* conf_obj)
{
	p2020_elbc_device *dev = (p2020_elbc_device *)conf_obj->obj;
	int i = 0;
	for (i = 0; i < 1; i++) {
		system_del_timer_handler(dev->timer_handle[i]);
	}
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

// system set device general_signal interface function
static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	p2020_elbc_device *dev = (p2020_elbc_device *)obj->obj;
	dev->general_signal.obj = obj2;
	if (obj2 == NULL) {
		skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", GENERAL_SIGNAL_INTF_NAME);
		return Not_found_exp;
	}
	dev->general_signal.iface = (general_signal_iface_t *)SKY_get_iface(obj2, GENERAL_SIGNAL_INTF_NAME);
	if (dev->general_signal.iface == NULL) {
		skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", GENERAL_SIGNAL_INTF_NAME, obj2->objname);
		return Not_found_exp;
	}
	return No_exp;
}

// system get device general_signal interface function
static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	p2020_elbc_device *dev = (p2020_elbc_device *)obj->obj;
	*obj2 = dev->general_signal.obj;
	*port = NULL;
	return No_exp;
}

// The system sets the value of the device int_number attribute
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	p2020_elbc_device *dev = (p2020_elbc_device *)conf_obj->obj;
	dev->int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

// The system gets the value of the device int_number attribute
static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	p2020_elbc_device *dev = (p2020_elbc_device *)conf_obj->obj;
	attr_value_t value = SKY_make_attr_uinteger(dev->int_number);
	return value;
}

// register device module properties
static void class_register_attribute(conf_class_t* dev_class)
{
	SKY_register_attribute(dev_class,
			"int_number",
			get_attr_int_number, NULL,
			set_attr_int_number, NULL,
			SKY_Attr_Optional,
			"uinteger",
			"this is int_number describe");
	return;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_p2020_elbc(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "p2020_elbc",
		.class_desc      = "p2020 elbc device",
		.new_instance    = new_p2020_elbc,
		.free_instance   = free_p2020_elbc,
		.config_instance = config_p2020_elbc
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);
	
	// sets the register address read-write method
	static memory_space_intf io_memory_interface =
	{
		.read  = p2020_elbc_read,
		.write = p2020_elbc_write
	};
	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);
	
	// register access interface (for SkyEye graphical control interface)
	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = p2020_elbc_get_regvalue_by_id,
		.get_regname_by_id  = p2020_elbc_get_regname_by_id,
		.set_regvalue_by_id = p2020_elbc_set_regvalue_by_id,
		.get_regnum         = p2020_elbc_get_regnum,
		.get_regid_by_name  = p2020_elbc_get_regid_by_name
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
		(struct ConnectDescription)
		{
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set  = general_signal_set,
			.get  = general_signal_get
		},
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
	class_register_attribute(dev_class);
}

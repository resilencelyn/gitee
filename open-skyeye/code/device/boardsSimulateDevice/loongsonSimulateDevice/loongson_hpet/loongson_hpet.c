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
#include "loongson_hpet.h"

void timer_count_handler(conf_object_t *opaque)
{
	loongson_hpet_device *dev = (loongson_hpet_device *)opaque->obj;
	dev->regs.hpet_cnt = dev->regs.hpet_cnt + 1000;
	//printf("in timer_count_handler, %x %x \n", dev->regs.hpet_cnt, dev->regs.hpet_cmp);
	if (dev->regs.hpet_cnt == dev->regs.hpet_cmp)
	{
		//interrupt
		//printf("in timer_count_handler, %x %x \n", dev->regs.hpet_cnt, dev->regs.hpet_cmp);
		if (dev->signal_iface)
			dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);
		dev->regs.hpet_cnt = 0;
	}
	
}

// Register read operation method implementation
static exception_t loongson_hpet_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_hpet_device *dev = (loongson_hpet_device *)conf_obj->obj;
	switch (offset)
	{
		case HPET_CFG:
			*(uint32_t*)buf = dev->regs.hpet_cfg;
			break;
		case HPET_CNT:
			*(uint32_t*)buf = dev->regs.hpet_cnt;
			break;
		case HPET_CMP:
			*(uint32_t*)buf = dev->regs.hpet_cmp;
			break;
		case HPET_STP:
			*(uint32_t*)buf = dev->regs.hpet_stp;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_hpet\n", offset);
			break;
	}
	//printf("in %s, offset: 0x%x, val: 0x%x \n", __FUNCTION__, offset, *(uint32_t*)buf);
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_hpet_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_hpet_device *dev = (loongson_hpet_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf("in %s, offset: 0x%x, val: 0x%x \n", __FUNCTION__, offset, val);
	switch (offset)
	{
		case HPET_CFG:
			dev->regs.hpet_cfg = val;
			if (val )
			break;
		case HPET_CNT:
			dev->regs.hpet_cnt = val;
			break;
		case HPET_CMP:
			dev->regs.hpet_cmp = val;
			break;
		case HPET_STP:
			dev->regs.hpet_stp = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_hpet\n", offset);
			break;
	}
	return No_exp;
}

// Register interface: gets the register name based on the register ID
static char* loongson_hpet_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

// Register interface: set register value according to register ID
static exception_t loongson_hpet_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	loongson_hpet_device *dev = (loongson_hpet_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

// Register interface: gets how many registers the device has
static uint32_t loongson_hpet_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

// Register interface: gets the register value based on the register ID
static uint32_t loongson_hpet_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	loongson_hpet_device *dev = (loongson_hpet_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

// Register interface: gets the register ID based on the register name
static uint32_t loongson_hpet_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_loongson_hpet(const char *obj_name)
{
	loongson_hpet_device* dev = skyeye_mm_zero(sizeof(loongson_hpet_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here
	// Register initialization
	struct registers *regs = &(dev->regs);
	dev->regs.hpet_cmp = 0xffffffff;
	dev->cpu_freq = 8;//MHz
	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_loongson_hpet(conf_object_t* conf_obj)
{
	loongson_hpet_device *dev = (loongson_hpet_device *)conf_obj->obj;
	// TODO: place the code here
	double cpu_cycle = 1 / (double)dev->cpu_freq;
	int period;

	if (cpu_cycle * 1000 > 0)
	{
		period = (int)(cpu_cycle * 1000);
	}
	system_register_timer_handler(conf_obj, period, (time_sched_t)timer_count_handler, SCHED_US|SCHED_NORMAL);
	No_exp;
}

// Free the device object to do some memory free
static exception_t free_loongson_hpet(conf_object_t* conf_obj)
{
	loongson_hpet_device *dev = (loongson_hpet_device *)conf_obj->obj;
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

static exception_t set_attr_general_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	loongson_hpet_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t get_attr_general_signal(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	loongson_hpet_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	loongson_hpet_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	loongson_hpet_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static timer_register_attribute(conf_class_t* dev_class){
	SKY_register_attribute(dev_class, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_loongson_hpet(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "loongson_hpet",
		.class_desc      = "",
		.new_instance    = new_loongson_hpet,
		.free_instance   = free_loongson_hpet,
		.config_instance = config_loongson_hpet,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);
	
	// sets the register address read-write method
	static memory_space_intf io_memory_interface =
	{
		.read  = loongson_hpet_read,
		.write = loongson_hpet_write
	};
	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);
	
	// register access interface (for SkyEye graphical control interface)
	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = loongson_hpet_get_regvalue_by_id,
		.get_regname_by_id  = loongson_hpet_get_regname_by_id,
		.set_regvalue_by_id = loongson_hpet_set_regvalue_by_id,
		.get_regnum         = loongson_hpet_get_regnum,
		.get_regid_by_name  = loongson_hpet_get_regid_by_name
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
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = set_attr_general_signal,
			.get = get_attr_general_signal,
		},
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
	timer_register_attribute(dev_class);
}

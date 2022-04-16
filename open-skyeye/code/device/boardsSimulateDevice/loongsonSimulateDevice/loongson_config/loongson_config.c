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
#include "loongson_config.h"

static int loongson_intc_raise_signal(conf_object_t *opaque, int line){
	loongson_config_device *dev = opaque->obj;
	dev->regs.int_out |= line;
	//printf("dev->regs.int_out:0x%x \n",dev->regs.int_out);
	if(dev->regs.int_en & line)
	{
		mips_signal_t mips_signal;
		mips_signal.HARD_IP = IP7;
		if(dev->signal_iface)
			dev->signal_iface->signal(dev->signal, &mips_signal);
	}

	return No_exp;
}

// Register read operation method implementation
static exception_t loongson_config_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_config_device *dev = (loongson_config_device *)conf_obj->obj;
	switch (offset)
	{
		case INT_EN:
			*(uint32_t*)buf = dev->regs.int_en;
			break;
		case INT_EDGE:
			*(uint32_t*)buf = dev->regs.int_edge;
			break;
		case INT_POL:
			*(uint32_t*)buf = dev->regs.int_pol;
			break;
		case INT_CLR:
			*(uint32_t*)buf = dev->regs.int_clr;
			break;
		case INT_SET:
			*(uint32_t*)buf = dev->regs.int_set;
			break;
		case INT_OUT:
			*(uint32_t*)buf = dev->regs.int_out;
			break;
		case SRAM_AF:
			*(uint32_t*)buf = dev->regs.sram_af;
			break;
		case GPIO_MUX:
			*(uint32_t*)buf = dev->regs.gpio_mux;
			break;
		case GPIO_O:
			*(uint32_t*)buf = dev->regs.gpio_o;
			break;
		case GPIO_E:
			*(uint32_t*)buf = dev->regs.gpio_e;
			break;
		case GPIO_I:
			*(uint32_t*)buf = dev->regs.gpio_i;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_config\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_config_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_config_device *dev = (loongson_config_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case INT_EN:
			dev->regs.int_en = val;
			break;
		case INT_EDGE:
			dev->regs.int_edge = val;
			break;
		case INT_POL:
			dev->regs.int_pol = val;
			break;
		case INT_CLR:
			dev->regs.int_clr &= ~val;
			break;
		case INT_SET:
			dev->regs.int_set &= ~val;
			break;
		case INT_OUT:
			dev->regs.int_out = val;
			break;
		case SRAM_AF:
			dev->regs.sram_af = val;
			break;
		case GPIO_MUX:
			dev->regs.gpio_mux = val;
			break;
		case GPIO_O:
			dev->regs.gpio_o = val;
			break;
		case GPIO_E:
			dev->regs.gpio_e = val;
			break;
		case GPIO_I:
			dev->regs.gpio_i = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_config\n", offset);
			break;
	}
	return No_exp;
}

// Register interface: gets the register name based on the register ID
static char* loongson_config_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

// Register interface: set register value according to register ID
static exception_t loongson_config_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	loongson_config_device *dev = (loongson_config_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

// Register interface: gets how many registers the device has
static uint32_t loongson_config_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

// Register interface: gets the register value based on the register ID
static uint32_t loongson_config_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	loongson_config_device *dev = (loongson_config_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

// Register interface: gets the register ID based on the register name
static uint32_t loongson_config_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_loongson_config(const char *obj_name)
{
	loongson_config_device* dev = skyeye_mm_zero(sizeof(loongson_config_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here
	// Register initialization
	struct registers *regs = &(dev->regs);
	dev->regs.int_edge = 0x7f481;
	dev->regs.int_pol = 0xffffffff;
	dev->regs.gpio_mux  = 0xffffffff;
	dev->regs.vr_cfg = 0x2f000000;
	dev->regs.bandgap = 0x1364;
	dev->regs.sram_af = 0xffffffff;

	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_loongson_config(conf_object_t* conf_obj)
{
	loongson_config_device *dev = (loongson_config_device *)conf_obj->obj;
	// TODO: place the code here


	No_exp;
}

// Free the device object to do some memory free
static exception_t free_loongson_config(conf_object_t* conf_obj)
{
	loongson_config_device *dev = (loongson_config_device *)conf_obj->obj;
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	loongson_config_device *dev =  obj->obj;

	dev->signal = obj2;
	if (dev->signal == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		return Not_found_exp;
	}

	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);
	}

	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	loongson_config_device *dev =  obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_loongson_config(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "loongson_config",
		.class_desc      = "",
		.new_instance    = new_loongson_config,
		.free_instance   = free_loongson_config,
		.config_instance = config_loongson_config,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);
	
	// sets the register address read-write method
	static memory_space_intf io_memory_interface =
	{
		.read  = loongson_config_read,
		.write = loongson_config_write
	};
	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);
	
	// register access interface (for SkyEye graphical control interface)
	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = loongson_config_get_regvalue_by_id,
		.get_regname_by_id  = loongson_config_get_regname_by_id,
		.set_regvalue_by_id = loongson_config_set_regvalue_by_id,
		.get_regnum         = loongson_config_get_regnum,
		.get_regid_by_name  = loongson_config_get_regid_by_name
	};
	SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);
	static const general_signal_intf signal = {
		.raise_signal = loongson_intc_raise_signal
	};
	SKY_register_iface(dev_class, GENERAL_SIGNAL_INTF_NAME, &signal);
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
		(struct InterfaceDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.iface = &signal,
		},
		
	};
	
	// list of interface connection methods
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		}
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
}

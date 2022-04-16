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
#include "i2c_c6678.h"

static void check_stop_condition(conf_object_t* opaque)
{
	c6678_I2C_device *dev = opaque->obj;
	if ((dev->regs.i2ccnt == 0) && (dev->regs.i2cmdr.bit.MST == 1))
	{
		//STP BIT = 1
		if (dev->regs.i2cmdr.bit.STP == 1)
		{
			dev->regs.i2cstr.bit.BB = 0;
			dev->regs.i2cstr.bit.ICXRDY = 0; //set bb and ICXRDY to 0;
			dev->regs.i2cmdr.bit.STP = 0; //set STP to 0; 
			dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
		}
	}
	//dev->regs.i2cstr &= ~(1 << 12);//set bb to 0   Bus free
}

static void check_start_condition(conf_object_t *opaque)
{
	c6678_I2C_device *dev = opaque->obj;
	int8_t s8_ret = -1;
	if (dev->regs.i2cmdr.bit.MST == 1)
	{	//master mode 
		if ((dev->regs.i2cmdr.bit.STT) && (dev->i2c_bus_iface != NULL))//STT = 1
		{
			dev->u32i2c_bus_flag = 1;
			if (dev->regs.i2cmdr.bit.TRX == 1)
			{
				//transmit data
				dev->regs.i2cstr.bit.ICXRDY = 1;//set ICXRDY
				s8_ret = dev->i2c_bus_iface->start(dev->i2c_bus_obj,(dev->slave_addr) & ~1);
				//printf("s8_ret = 0x%x\n",s8_ret);
				if (s8_ret == 0)
				{
					dev->regs.i2cmdr.bit.STT = 0;
					dev->regs.i2cstr.bit.BB = 1;
				}
				else
				{
					printf("[%s] : %d : start error ! ret = %d\n",__FUNCTION__,__LINE__,s8_ret);
				}
			}
			else
			{
				//receive data
				dev->regs.i2cstr.bit.ICRRDY = 1; //set ICRRDY
				s8_ret = dev->i2c_bus_iface->start(dev->i2c_bus_obj,(dev->slave_addr) | 1);
				if (s8_ret == 0)
				{
					dev->regs.i2cmdr.bit.STT = 0;
					dev->regs.i2cstr.bit.BB = 1;
				}
				else
				{
					printf("[%s] : %d : start error! ret = %d\n",__FUNCTION__,__LINE__,s8_ret);
				}
			}
			dev->regs.i2cmdr.bit.STT = 0;
		}
	}
	else
	{
		//slave mode
		if (dev->regs.i2cmdr.bit.STT == 1)
		{
			if (dev->i2c_bus_iface != NULL)
			{
				dev->regs.i2cdrr = dev->i2c_bus_iface->read_data(dev->i2c_bus_obj);
			}
			dev->regs.i2cstr.bit.BB = 1;//set bb to 1   Bus is busy
		}
	}
}

// Register read operation method implementation
static exception_t c6678_I2C_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	c6678_I2C_device *dev = (c6678_I2C_device *)conf_obj->obj;
	switch (offset)
	{
		case I2COAR:
			*(uint32_t *)buf = dev->regs.i2coar;
			break;
		case I2CIMR:
			*(uint32_t *)buf = dev->regs.i2cimr;
			break;
		case I2CSTR:
			*(uint32_t *)buf = dev->regs.i2cstr.all;
			break;
		case I2CCLKL:
			*(uint32_t *)buf = dev->regs.i2cclkl;
			break;
		case I2CCLKH:
			*(uint32_t *)buf = dev->regs.i2cclkh;
			break;
		case I2CCNT:
			*(uint32_t *)buf = dev->regs.i2ccnt;
			break;
		case I2CDRR:
			if ((dev->i2c_bus_iface != NULL) && (dev->regs.i2ccnt != 0))
			{
				*(uint32_t *)buf = (uint32_t)(dev->i2c_bus_iface->read_data(dev->i2c_bus_obj));
				dev->regs.i2cstr.bit.ICRRDY = 1;
				dev->regs.i2ccnt--;
			}
			break;
		case I2CSAR:
			*(uint32_t *)buf = dev->regs.i2csar;
			break;
		case I2CDXR:
			*(uint32_t *)buf = dev->regs.i2cdxr;
			break;
		case I2CMDR:
			*(uint32_t *)buf = dev->regs.i2cmdr.all;
			break;
		case I2CIVR:
			*(uint32_t *)buf = dev->regs.i2civr;
			break;
		case I2CEMDR:
			*(uint32_t *)buf = dev->regs.i2cemdr;
			break;
		case I2CPSC:
			*(uint32_t *)buf = dev->regs.i2cpsc;
			break;
		case I2CPID1:
			*(uint32_t *)buf = dev->regs.i2cpid1;
			break;
		case I2CPID2:
			*(uint32_t *)buf = dev->regs.i2cpid2;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in c6678_I2C\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t c6678_I2C_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	c6678_I2C_device *dev = (c6678_I2C_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case I2COAR:
			dev->regs.i2coar = val & 0x3ff;
			if (dev->regs.i2cmdr.bit.XA)
			{
				dev->own_slave_addr = val & 0x3ff;
			}
			else
			{
				dev->own_slave_addr = val & 0x7f;
			}
			break;
		case I2CIMR:
			dev->regs.i2cimr = val & 0xff;//只有第8位有效
			break;
		case I2CSTR:
			//参考寄存器手册，部分bit只读
			dev->regs.i2cstr.all = val & 0x743f;
			break;
		case I2CCLKL:
			dev->regs.i2cclkl = val & 0xffff;
			break;
		case I2CCLKH:
			dev->regs.i2cclkh = val & 0xffff;
			break;
		case I2CCNT:
			dev->regs.i2ccnt = val & 0xffff;
			break;
		case I2CSAR:
			dev->regs.i2csar = val & 0x3ff;
			if (dev->regs.i2cmdr.bit.XA)
			{
				dev->slave_addr = val & 0x3ff;
			}
			else
			{
				dev->slave_addr = val & 0x7f;
			}
			break;
		case I2CDXR:
			dev->regs.i2cdxr = (uint8_t)val;
			if ((dev->i2c_bus_iface != NULL) && (dev->regs.i2ccnt != 0))
			{
				dev->i2c_bus_iface->write_data(dev->i2c_bus_obj, val);
				dev->regs.i2cstr.bit.ICXRDY = 1;
				dev->regs.i2ccnt--;
			}
			break;
		case I2CMDR:
			//第12位只读
			dev->regs.i2cmdr.all = val & 0xefff;
			//
			if (dev->regs.i2cmdr.bit.FDF)
			{
				dev->regs.i2cstr.bit.AAS = 1;//AAS set to 1
			}
			if (dev->regs.i2cmdr.bit.STT == 1)
			{
				if (dev->i2c_bus_iface != NULL)
				{
					dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
				}
				else
				{
					printf("%s : %d ERROR!\n",__FUNCTION__,__LINE__);
				}
			}
			if (dev->regs.i2cmdr.bit.IRS == 1)//I2C使能
			{
				check_start_condition(conf_obj);
			}
			else
			{
				dev->regs.i2cstr.all = 0x403;
			}
			break;
		case I2CEMDR:
			dev->regs.i2cemdr = val & 0x3;
			break;
		case I2CPSC:
			dev->regs.i2cpsc = val & 0xff;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in c6678_I2C\n", offset);
			break;
	}
	return No_exp;
}

//i2c_bus_iface
static exception_t set_attr_i2c_bus(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	c6678_I2C_device *dev = obj->obj;
	dev->i2c_bus_obj = obj2;
	dev->i2c_bus_iface = (i2c_bus_intf *)SKY_get_iface(dev->i2c_bus_obj, I2C_BUS_INTF_NAME);
	if (dev->i2c_bus_iface == NULL)
	{
		if (dev->i2c_bus_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", I2C_BUS_INTF_NAME);
		}
		else
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", I2C_BUS_INTF_NAME, obj2->objname);
		}
		return Not_found_exp;
	}
	return No_exp;
}

static exception_t get_attr_i2c_bus(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	c6678_I2C_device *dev = obj->obj;
	*obj2 = dev->i2c_bus_obj;
	*port = NULL;
	return No_exp;
}

// Register interface: gets the register name based on the register ID
static char* c6678_I2C_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

// Register interface: set register value according to register ID
static exception_t c6678_I2C_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	c6678_I2C_device *dev = (c6678_I2C_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

// Register interface: gets how many registers the device has
static uint32_t c6678_I2C_get_regnum(conf_object_t *conf_obj)
{
	return sizeof(struct registers) / 4;
}

// Register interface: gets the register value based on the register ID
static uint32_t c6678_I2C_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	c6678_I2C_device *dev = (c6678_I2C_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

// Register interface: gets the register ID based on the register name
static uint32_t c6678_I2C_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++)
	{
		if (strcmp(name, regs_name[i]) == 0)
		{
			return i;
		}
	}
	return regnum;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_c6678_I2C(const char *obj_name)
{
	c6678_I2C_device* dev = skyeye_mm_zero(sizeof(c6678_I2C_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here
	// Register initialization
	struct registers *regs = &(dev->regs);
	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_c6678_I2C(conf_object_t* conf_obj)
{
	c6678_I2C_device *dev = (c6678_I2C_device *)conf_obj->obj;
	// TODO: place the code here
	return No_exp;
}

// Free the device object to do some memory free
static exception_t free_c6678_I2C(conf_object_t* conf_obj)
{
	c6678_I2C_device *dev = (c6678_I2C_device *)conf_obj->obj;
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_c6678_I2C(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "c6678_I2C",
		.class_desc      = "",
		.new_instance    = new_c6678_I2C,
		.free_instance   = free_c6678_I2C,
		.config_instance = config_c6678_I2C,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

	// sets the register address read-write method
	static memory_space_intf io_memory_interface =
	{
		.read  = c6678_I2C_read,
		.write = c6678_I2C_write
	};
	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

	// register access interface (for SkyEye graphical control interface)
	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = c6678_I2C_get_regvalue_by_id,
		.get_regname_by_id  = c6678_I2C_get_regname_by_id,
		.set_regvalue_by_id = c6678_I2C_set_regvalue_by_id,
		.get_regnum         = c6678_I2C_get_regnum,
		.get_regid_by_name  = c6678_I2C_get_regid_by_name
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
		(struct ConnectDescription)
		{
			.name = I2C_BUS_INTF_NAME,
			.set = set_attr_i2c_bus,
			.get = get_attr_i2c_bus,
		}
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
}

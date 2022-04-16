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
#include <skyeye_system.h>
#define DEBUG
#include <skyeye_log.h>
#include "sopc_gpio.h"

static exception_t sopc_gpio_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_gpio_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;

	switch (offset)
	{
		case GPIO_PORTA_DR:
			*(uint32_t *)buf = regs->gpio_porta_dr;
			break;
		case GPIO_PORTA_DDR:
			*(uint32_t *)buf = regs->gpio_porta_ddr;
			break;
		case GPIO_PORTB_DR:
			*(uint32_t *)buf = regs->gpio_portb_dr;
			break;
		case GPIO_PORTB_DDR:
			*(uint32_t *)buf = regs->gpio_portb_ddr;
			break;
		case GPIO_PORTC_DR:
			*(uint32_t *)buf = regs->gpio_portc_dr;
			break;
		case GPIO_PORTC_DDR:
			*(uint32_t *)buf = regs->gpio_portc_ddr;
			break;
		case GPIO_INTEN:
			*(uint32_t *)buf = regs->gpio_inten;
			break;
		case GPIO_INTMASK:
			*(uint32_t *)buf = regs->gpio_intmask;
			break;
		case GPIO_INTTYPE_LEVEL:
			*(uint32_t *)buf = regs->gpio_inttype_level;
			break;
		case GPIO_INT_POLARITY:
			*(uint32_t *)buf = regs->gpio_int_polarity;
			break;
		case GPIO_INT_STATUS:
			*(uint32_t *)buf = regs->gpio_intstatus;
			break;
		case GPIO_RAW_INT_STATUS:
			*(uint32_t *)buf = regs->gpio_raw_intstatus;
			break;
		case GPIO_DEBOUNCE:
			*(uint32_t *)buf = regs->gpio_debounce;
			break;
		case GPIO_EXT_PORTA:
			*(uint32_t *)buf = regs->gpio_ext_porta;
			break;
		case GPIO_EXT_PORTB:
			*(uint32_t *)buf = regs->gpio_ext_portb;
			break;
		case GPIO_EXT_PORTC:
			*(uint32_t *)buf = regs->gpio_ext_portc;
			break;
		case GPIO_LS_SYNC:
			*(uint32_t *)buf = regs->gpio_ls_sync;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sopc_gpio\n", offset);
			return Not_found_exp;
	}
	return No_exp;
}

void set_port_data(conf_object_t *opaque, uint32_t data, int portNum)
{
	sopc_gpio_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	int i;
	for (i = 0; i < 32; i++)
	{
		if ((data >> i) & 0x1)
		{
			if (portNum == 0) //PortA
			{
				if (GET_PORTA_DDR(i))
				{
					SET_PORTA(i);
				}
			} else if (portNum == 1) //PortB
			{
				if (GET_PORTB_DDR(i))
				{
					SET_PORTB(i);
				}
			} else if (portNum == 2) //PortC
			{
				if (GET_PORTC_DDR(i))
				{
					SET_PORTC(i);
				}
			}
		}
	}
}

static exception_t sopc_gpio_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_gpio_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	uint32_t val = *(uint32_t *)buf;

	switch (offset)
	{
		case GPIO_PORTA_DR:
			//regs->gpio_porta_dr = val;
			set_port_data(opaque, val, 0);
			if(val & IIC_ENABLE_MASK)
			{
				int tmp = (val & IIC_ENABLE_MASK);
				if(dev->iic_iface == NULL)
					if(dev->iic == NULL)
					{
						skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.\n", FPGA_GPIO_CONNECT);
						return Not_found_exp;
					}
				dev->iic_iface->send_data(dev->iic, &tmp, count);
			}

			break;
		case GPIO_PORTA_DDR:
			regs->gpio_porta_ddr = val;
			break;
		case GPIO_PORTB_DR:
			//regs->gpio_portb_dr = val;
			set_port_data(opaque, val, 1);
			break;
		case GPIO_PORTB_DDR:
			regs->gpio_portb_ddr = val;
			break;
		case GPIO_PORTC_DR:
			//regs->gpio_portc_dr = val;
			set_port_data(opaque, val, 2);
			break;
		case GPIO_PORTC_DDR:
			regs->gpio_portc_ddr = val;
			break;
		case GPIO_INTEN:
			regs->gpio_inten = val;
			break;
		case GPIO_INTMASK:
			regs->gpio_intmask = val;
			break;
		case GPIO_INTTYPE_LEVEL:
			regs->gpio_inttype_level = val;
			break;
		case GPIO_INT_POLARITY:
			regs->gpio_int_polarity = val;
			break;
		case GPIO_DEBOUNCE:
			regs->gpio_debounce = val;
			break;
		case GPIO_PORTA_EOI:
			regs->gpio_porta_eoi = val;
			break;
		case GPIO_LS_SYNC:
			regs->gpio_ls_sync = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sopc_gpio\n", offset);
			return Not_found_exp;
	}

	return No_exp;
}

static char* sopc_gpio_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t sopc_gpio_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	sopc_gpio_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t sopc_gpio_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t sopc_gpio_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	sopc_gpio_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t sopc_gpio_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t sopc_gpio_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static init_gpio_register(sopc_gpio_device *dev)
{
	struct registers *regs = &(dev->regs);
}

static conf_object_t* new_sopc_gpio(const char *obj_name)
{
	sopc_gpio_device* dev = skyeye_mm_zero(sizeof(sopc_gpio_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	init_gpio_register(dev);
	return dev->obj;
}

static exception_t config_sopc_gpio(conf_object_t* opaque)
{
	sopc_gpio_device *dev =  opaque->obj;
	return No_exp;
}

static exception_t free_sopc_gpio(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t iic_iface_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sopc_gpio_device *dev =  obj->obj;
	dev->iic = obj2;
	dev->iic_iface = (fpga_gpio_connect_intf *)SKY_get_iface(dev->iic, FPGA_GPIO_CONNECT);

	if (dev->iic_iface == NULL){
		if (dev->iic == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", FPGA_GPIO_CONNECT);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", FPGA_GPIO_CONNECT, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t iic_iface_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	sopc_gpio_device *dev =  obj->obj;
	*obj2 = dev->iic;
	*port = NULL;
	return No_exp;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sopc_gpio_device *dev =  obj->obj;
	dev->signal.obj = obj2;
	dev->signal.intf = (general_signal_intf *)SKY_get_iface(dev->signal.obj, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal.intf == NULL){
		if (dev->signal.obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	sopc_gpio_device *dev =  obj->obj;
	*obj2 = dev->signal.obj;
	*port = NULL;
	return No_exp;
}

static void sopc_gpio_register_attribute(conf_class_t* clss)
{
	return;
}

void init_sopc_gpio(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "sopc_gpio",
		.class_desc = "sopc_gpio",
		.new_instance = new_sopc_gpio,
		.free_instance = free_sopc_gpio,
		.config_instance = config_sopc_gpio
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = sopc_gpio_read,
		.write = sopc_gpio_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = sopc_gpio_get_regvalue_by_id,
		.get_regname_by_id = sopc_gpio_get_regname_by_id,
		.set_regvalue_by_id = sopc_gpio_set_regvalue_by_id,
		.get_regnum = sopc_gpio_get_regnum,
		.get_regid_by_name = sopc_gpio_get_regid_by_name,
		.get_regoffset_by_id = sopc_gpio_get_regoffset_by_id
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
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = signal_set,
			.get = signal_get,
		},
		(struct ConnectDescription ){
			.name = FPGA_GPIO_CONNECT,
			.set = iic_iface_set,
			.get = iic_iface_get,
		}

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	sopc_gpio_register_attribute(clss);
}

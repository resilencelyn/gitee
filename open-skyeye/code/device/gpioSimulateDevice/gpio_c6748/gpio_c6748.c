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
#include "gpio_c6748.h"

static exception_t gpio_c6748_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	gpio_c6748_device *dev = opaque->obj;
	switch (offset)
	{
		case CTRL_DATA:
			*(uint32_t *)buf = dev->regs.ctrl_data;
			break;
		default: {
			if ((offset >= GPIO_PIN0) && (offset <= GPIO_PIN7)) {
				int index = offset / 4 - 1;
				if (((dev->regs.ctrl_data >> index) & 0x1) == 1) {
					*(uint32_t *)buf = (dev->regs.gpio_pin.all >> index) & 0x1;
				} else {
					skyeye_log(Warning_log, "gpio%d is output mode\n", index);
					return 0;
				}
			} else {
				skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in gpio_c6748\n", offset);
			}
			break;
		}
	}
	//printf("%s, %d, offset = %x, buf = %x\n\n", __func__, __LINE__, offset, *(uint32_t *)buf);
	return 0;
}

static exception_t gpio_c6748_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	gpio_c6748_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf("\n%s, %d, offset = %x, buf = %x\n", __func__, __LINE__, offset, val);
	switch (offset) {
		case CTRL_DATA:
			dev->regs.ctrl_data = val;
			break;
		default: {
			if ((offset >= GPIO_PIN0) && (offset <= GPIO_PIN7)) {
				int index = offset / 4 - 1;
				if (((dev->regs.ctrl_data >> index) & 0x1) == 0) {
					if (val & 0x1) {
						dev->regs.gpio_pin.all |= (val & 0x1) << index;
					} else {
						dev->regs.gpio_pin.all &= ~((val & 0x1) << index);
					}
					if (dev->gpio_connector_iface) {
						dev->gpio_connector_iface->update(dev->gpio_connector, dev->regs.gpio_pin.all);
					}
				} else {
					skyeye_log(Warning_log, "gpio%d is input mode\n", index);
					return 0;
				}
			} else {
				skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in gpio_c6748\n", offset);
			}
			break;
		}
	}
	return 0;
}

static char* gpio_c6748_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t gpio_c6748_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	gpio_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t gpio_c6748_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t gpio_c6748_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	gpio_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t gpio_c6748_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_gpio_c6748(char *obj_name)
{
	gpio_c6748_device* dev = skyeye_mm_zero(sizeof(gpio_c6748_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	return dev->obj;
}

static exception_t free_gpio_c6748(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

exception_t update_gpio(conf_object_t *obj, uint32_t regvalue){
	gpio_c6748_device *dev = obj->obj;
	dev->regs.gpio_pin.all = regvalue;
	return No_exp;
}

static exception_t connector_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_c6748_device *dev = obj->obj;
	dev->gpio_connector = obj2;
	dev->gpio_connector_iface = (skyeye_gpio_connector_intf *)SKY_get_iface(dev->gpio_connector, SKYEYE_GPIO_CONNECTOR);
	if (dev->gpio_connector_iface == NULL){
		if (dev->gpio_connector == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_GPIO_CONNECTOR);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_GPIO_CONNECTOR, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
 
static exception_t connector_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	gpio_c6748_device *dev = obj->obj;
	*obj2 = dev->gpio_connector;
	*port = NULL;
	return No_exp;
}

void init_gpio_c6748(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6748_gpio",
		.class_desc = "c6748_gpio",
		.new_instance = new_gpio_c6748,
		.free_instance = free_gpio_c6748
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = gpio_c6748_read,
		.write = gpio_c6748_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = gpio_c6748_get_regvalue_by_id,
		.get_regname_by_id = gpio_c6748_get_regname_by_id,
		.set_regvalue_by_id = gpio_c6748_set_regvalue_by_id,
		.get_regnum = gpio_c6748_get_regnum,
		.get_regid_by_name = gpio_c6748_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const skyeye_gpio_connector_intf gpio_intf= {
		.update =  update_gpio
	};
	SKY_register_iface(clss, SKYEYE_GPIO_CONNECTOR, &gpio_intf);

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
		(struct InterfaceDescription) {
			.name = SKYEYE_GPIO_CONNECTOR,
			.iface = &gpio_intf
		},
	};

	static const struct ConnectDescription connects[] =
	{
		(struct ConnectDescription) {
			.name = SKYEYE_GPIO_CONNECTOR,
			.set = connector_set,
			.get = connector_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

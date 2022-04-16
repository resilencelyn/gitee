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
#include "gpio_c6678.h"

static exception_t gpio_c6678_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	gpio_c6678_device *dev = opaque->obj;
	switch (offset) {
	case BINTEN:
		*(uint32_t *)buf = dev->regs.binten;
		break;
	case DIR:
		*(uint32_t *)buf = dev->regs.dir;
		break;
	case OUT_DATA:
		*(uint32_t *)buf = dev->regs.out_data;
		break;
	case SET_DATA:
		*(uint32_t *)buf = dev->regs.set_data;
		break;
	case CLR_DATA:
		*(uint32_t *)buf = dev->regs.clr_data;
		break;
	case IN_DATA:
		*(uint32_t *)buf = dev->regs.out_data;
		break;
	case SET_RIS_TRIG:
		*(uint32_t *)buf = dev->ris_trig;
		break;
	case CLR_RIS_TRIG:
		*(uint32_t *)buf = dev->regs.clr_ris_trig;
		break;
	case SET_FAL_TRIG:
		*(uint32_t *)buf = dev->fal_trig;
		break;
	case CLR_FAL_TRIG:
		*(uint32_t *)buf = dev->regs.clr_fal_trig;
		break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in gpio_c6678\n", offset);
		break;
	}
	//printf("%s  offset: 0x%02x buf: 0x%x\n", __FUNCTION__, offset, *(uint32_t *)buf);
	return 0;
}

static exception_t gpio_c6678_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	gpio_c6678_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf("%s offset: 0x%02x buf: 0x%x\n", __FUNCTION__, offset, *(uint32_t *)buf);

	switch (offset) {
	case BINTEN:
		dev->regs.binten = val;
		break;
	case DIR:
		dev->regs.dir = val;
		break;
	case OUT_DATA:
		dev->regs.out_data = val;
		break;
	case SET_DATA: {
		dev->regs.set_data = val;
		int i;
		for (i = 0; i < 16; i++) {
			// 0 = No effect
			// 1 = Sets the correspongding bit in Output Data Register
			if (dev->regs.set_data & (0x1 << i)) {
				if ((dev->regs.dir & (0x1 << i)) == 0) {
					// GPn pin is Output pin
					dev->regs.out_data |= (0x1 << i);
				}
			}
		}
		break;
	}
	case CLR_DATA: {
		dev->regs.clr_data = val;
		int i;
		for (i = 0; i < 16; i++) {
			// 0 = No effect
			// 1 = Clears the correspongding bit in Output Data Register
			if (dev->regs.clr_data & (0x1 << i)) {
				if ((dev->regs.dir & (0x1 << i)) == 0) {
					// GPn pin is Output pin
					dev->regs.out_data &= ~(0x1 << i);
				}
			}
		}
		break;
	}
	case IN_DATA:
		dev->regs.in_data = val;
		break;
	case SET_RIS_TRIG: {
		dev->regs.set_ris_trig = val;
		int i;
		for (i = 0; i < 16; i++) {
			if (dev->regs.set_ris_trig & (0x1 << i)) {
				dev->ris_trig |= (0x1 << i);
			}
		}
		break;
	}
	case CLR_RIS_TRIG: {
		dev->regs.clr_ris_trig = val;
		int i;
		for (i = 0; i < 16; i++) {
			if (dev->regs.clr_ris_trig & (0x1 << i)) {
				dev->ris_trig &= ~(0x1 << i);
			}
		}
		break;
	}
	case SET_FAL_TRIG: {
		dev->regs.set_fal_trig = val;
		int i;
		for (i = 0; i < 16; i++) {
			if (dev->regs.set_fal_trig & (0x1 << i)) {
				dev->ris_trig |= (0x1 << i);
			}
		}
		break;
	}
	case CLR_FAL_TRIG: {
		dev->regs.clr_fal_trig = val;
		int i;
		for (i = 0; i < 16; i++) {
			if (dev->regs.clr_fal_trig & (0x1 << i)) {
				dev->fal_trig &= ~(0x1 << i);
			}
		}
		break;
	}
	default:
		skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in gpio_c6678\n", offset);
		break;
	}
	return 0;
}

static char* gpio_c6678_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t gpio_c6678_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	gpio_c6678_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t gpio_c6678_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t gpio_c6678_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	gpio_c6678_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t gpio_c6678_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if (strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_gpio_c6678(char *obj_name)
{
	gpio_c6678_device* dev = skyeye_mm_zero(sizeof(gpio_c6678_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	regs->dir = 0x0F;  // default input pin
	return dev->obj;
}

static exception_t free_gpio_c6678(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_gpio_c6678(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6678_gpio",
		.class_desc = "c6678_gpio",
		.new_instance = new_gpio_c6678,
		.free_instance = free_gpio_c6678
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = gpio_c6678_read,
		.write = gpio_c6678_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = gpio_c6678_get_regvalue_by_id,
		.get_regname_by_id = gpio_c6678_get_regname_by_id,
		.set_regvalue_by_id = gpio_c6678_set_regvalue_by_id,
		.get_regnum = gpio_c6678_get_regnum,
		.get_regid_by_name = gpio_c6678_get_regid_by_name
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

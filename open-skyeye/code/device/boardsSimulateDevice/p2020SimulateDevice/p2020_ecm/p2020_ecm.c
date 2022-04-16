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
#include "p2020_ecm.h"

static exception_t p2020_ecm_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	p2020_ecm_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;

	switch (offset)
	{
		case EEBACR:
			*(uint32_t *)buf = regs->eebacr;
			break;
		case EEBPCR:
			*(uint32_t *)buf = regs->eebpcr;
			break;
		case EIPBRR1:
			*(uint32_t *)buf = regs->eipbrr1;
			break;
		case EIPBRR2:
			*(uint32_t *)buf = regs->eipbrr2;
			break;
		case EEDR:
			*(uint32_t *)buf = regs->eedr;
			break;
		case EEER:
			*(uint32_t *)buf = regs->eeer;
			break;
		case EEATR:
			*(uint32_t *)buf = regs->eeatr;
			break;
		case EELADR:
			*(uint32_t *)buf = regs->eeladr;
			break;
		case EEHADR:
			*(uint32_t *)buf = regs->eehadr;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in p2020_ecm\n", offset);
			return Not_found_exp;
	}
	return No_exp;
}

static exception_t p2020_ecm_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	p2020_ecm_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	uint32_t val = *(uint32_t *)buf;

	switch (offset)
	{
		case EEBACR:
			regs->eebacr = val;
			break;
		case EEBPCR:
			regs->eebpcr = val;
			if (dev->core_iface[0].iface)
				dev->core_iface[0].iface->power(dev->core_iface[0].obj, (regs->eebpcr>>24) & 0x1);
			if (dev->core_iface[1].iface)
				dev->core_iface[1].iface->power(dev->core_iface[1].obj, (regs->eebpcr>>25) & 0x1);
			break;
		case EIPBRR1:
			regs->eipbrr1 = val;
			break;
		case EIPBRR2:
			regs->eipbrr2 = val;
			break;
		case EEDR:
			regs->eedr = val;
			break;
		case EEER:
			regs->eeer = val;
			break;
		case EEATR:
			regs->eeatr = val;
			break;
		case EELADR:
			regs->eeladr = val;
			break;
		case EEHADR:
			regs->eehadr = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in p2020_ecm\n", offset);
			return Not_found_exp;
	}

	return No_exp;
}

static char* p2020_ecm_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t p2020_ecm_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	p2020_ecm_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t p2020_ecm_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t p2020_ecm_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	p2020_ecm_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t p2020_ecm_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t p2020_ecm_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static int init_gpio_register(p2020_ecm_device *dev)
{
	struct registers *regs = &(dev->regs);
	regs->eebacr = 3;
}

static conf_object_t* new_p2020_ecm(const char *obj_name)
{
	p2020_ecm_device* dev = skyeye_mm_zero(sizeof(p2020_ecm_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	init_gpio_register(dev);
	return dev->obj;
}

static exception_t config_p2020_ecm(conf_object_t* opaque)
{
	p2020_ecm_device *dev =  opaque->obj;
	return No_exp;
}

static exception_t free_p2020_ecm(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static void p2020_ecm_register_attribute(conf_class_t* clss)
{
	return;
}

static exception_t core_interface_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	p2020_ecm_device *dev = obj->obj;
	assert(index < 2);
	dev->core_iface[index].obj = obj2;
	if (obj2 == NULL)
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface By core[%d] is NULL object.", CORE_EXEC_INTF_NAME, index);
	dev->core_iface[index].iface = (core_exec_intf *)SKY_get_iface(obj2, CORE_EXEC_INTF_NAME);
	if (dev->core_iface[index].iface == NULL)
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_EXEC_INTF_NAME, obj2->objname);
	return No_exp;
}

static exception_t core_interface_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	p2020_ecm_device *dev = obj->obj;
	*obj2 = dev->core_iface[index].obj;
	*port = NULL;
	return No_exp;
}

void init_p2020_ecm(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "p2020_ecm",
		.class_desc = "p2020_ecm",
		.new_instance = new_p2020_ecm,
		.free_instance = free_p2020_ecm,
		.config_instance = config_p2020_ecm
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = p2020_ecm_read,
		.write = p2020_ecm_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = p2020_ecm_get_regvalue_by_id,
		.get_regname_by_id = p2020_ecm_get_regname_by_id,
		.set_regvalue_by_id = p2020_ecm_set_regvalue_by_id,
		.get_regnum = p2020_ecm_get_regnum,
		.get_regid_by_name = p2020_ecm_get_regid_by_name,
		.get_regoffset_by_id = p2020_ecm_get_regoffset_by_id
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
		(struct ConnectDescription) {
			.name = CORE_EXEC_INTF_NAME,
			.set = core_interface_set,
			.get = core_interface_get
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	p2020_ecm_register_attribute(clss);
}

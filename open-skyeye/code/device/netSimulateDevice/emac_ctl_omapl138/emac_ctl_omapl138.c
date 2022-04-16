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
#include "emac_ctl_omapl138.h"
static exception_t emac_ctl_omapl138_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	emac_ctl_omapl138_device *dev = opaque->obj;
	switch (offset)
	{
		case REVID:
			*(uint32_t *)buf = dev->regs.revid;
			break;
		case SOFTRESET:
			*(uint32_t *)buf = dev->regs.softreset;
			break;
		case INTCONTROL:
			*(uint32_t *)buf = dev->regs.intcontrol;
			break;
		case C0RXTHRESHEN:
			*(uint32_t *)buf = dev->regs.c0rxthreshen;
			break;
		case C0RXEN:
			*(uint32_t *)buf = dev->regs.c0rxen;
			break;
		case C0TXEN:
			*(uint32_t *)buf = dev->regs.c0txen;
			break;
		case C0MISCEN:
			*(uint32_t *)buf = dev->regs.c0miscen;
			break;
		case C1RXTHRESHEN:
			*(uint32_t *)buf = dev->regs.c1rxthreshen;
			break;
		case C1RXEN:
			*(uint32_t *)buf = dev->regs.c1rxen;
			break;
		case C1TXEN:
			*(uint32_t *)buf = dev->regs.c1txen;
			break;
		case C1MISCEN:
			*(uint32_t *)buf = dev->regs.c1miscen;
			break;
		case C2RXTHRESHEN:
			*(uint32_t *)buf = dev->regs.c2rxthreshen;
			break;
		case C2RXEN:
			*(uint32_t *)buf = dev->regs.c2rxen;
			break;
		case C2TXEN:
			*(uint32_t *)buf = dev->regs.c2txen;
			break;
		case C2MISCEN:
			*(uint32_t *)buf = dev->regs.c2miscen;
			break;
		case C0RXTHRESHSTAT:
			*(uint32_t *)buf = dev->regs.c0rxthreshstat;
			break;
		case C0RXSTAT:
			*(uint32_t *)buf = dev->regs.c0rxstat;
			break;
		case C0TXSTAT:
			*(uint32_t *)buf = dev->regs.c0txstat;
			break;
		case C0MISCSTAT:
			*(uint32_t *)buf = dev->regs.c0miscstat;
			break;
		case C1RXTHRESHSTAT:
			*(uint32_t *)buf = dev->regs.c1rxthreshstat;
			break;
		case C1RXSTAT:
			*(uint32_t *)buf = dev->regs.c1rxstat;
			break;
		case C1TXSTAT:
			*(uint32_t *)buf = dev->regs.c1txstat;
			break;
		case C1MISCSTAT:
			*(uint32_t *)buf = dev->regs.c1miscstat;
			break;
		case C2RXTHRESHSTAT:
			*(uint32_t *)buf = dev->regs.c2rxthreshstat;
			break;
		case C2RXSTAT:
			*(uint32_t *)buf = dev->regs.c2rxstat;
			break;
		case C2TXSTAT:
			*(uint32_t *)buf = dev->regs.c2txstat;
			break;
		case C2MISCSTAT:
			*(uint32_t *)buf = dev->regs.c2miscstat;
			break;
		case C0RXIMAX:
			*(uint32_t *)buf = dev->regs.c0rximax;
			break;
		case C0TXIMAX:
			*(uint32_t *)buf = dev->regs.c0tximax;
			break;
		case C1RXIMAX:
			*(uint32_t *)buf = dev->regs.c1rximax;
			break;
		case C1TXIMAX:
			*(uint32_t *)buf = dev->regs.c1tximax;
			break;
		case C2RXIMAX:
			*(uint32_t *)buf = dev->regs.c2rximax;
			break;
		case C2TXIMAX:
			*(uint32_t *)buf = dev->regs.c2tximax;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in emac_ctl_omapl138\n", offset);
			break;
	}
	return 0;
}
static exception_t emac_ctl_omapl138_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	emac_ctl_omapl138_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case SOFTRESET:
			dev->regs.softreset = val;
			/* soft reset */
			if (BITS(val, 0, 0) == 1) {
				/* do something reset */
				memset(&dev->regs, 0, sizeof(dev->regs));
				dev->regs.revid = 0x4ec80100;
				/* FIXME: clear the interrupt status */
				/* FIXME: clear the control status */
				SETS(dev->regs.softreset, 0, 0, 0);
			}
			break;
		case INTCONTROL:
			dev->regs.intcontrol = val;
			break;
		case C0RXTHRESHEN:
			dev->regs.c0rxthreshen = val;
			break;
		case C0RXEN:
			dev->regs.c0rxen = val;
			break;
		case C0TXEN:
			dev->regs.c0txen = val;
			break;
		case C0MISCEN:
			dev->regs.c0miscen = val;
			break;
		case C1RXTHRESHEN:
			dev->regs.c1rxthreshen = val;
			break;
		case C1RXEN:
			dev->regs.c1rxen = val;
			break;
		case C1TXEN:
			dev->regs.c1txen = val;
			break;
		case C1MISCEN:
			dev->regs.c1miscen = val;
			break;
		case C2RXTHRESHEN:
			dev->regs.c2rxthreshen = val;
			break;
		case C2RXEN:
			dev->regs.c2rxen = val;
			break;
		case C2TXEN:
			dev->regs.c2txen = val;
			break;
		case C2MISCEN:
			dev->regs.c2miscen = val;
			break;
		case C0RXIMAX:
			dev->regs.c0rximax = val;
			break;
		case C0TXIMAX:
			dev->regs.c0tximax = val;
			break;
		case C1RXIMAX:
			dev->regs.c1rximax = val;
			break;
		case C1TXIMAX:
			dev->regs.c1tximax = val;
			break;
		case C2RXIMAX:
			dev->regs.c2rximax = val;
			break;
		case C2TXIMAX:
			dev->regs.c2tximax = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in emac_ctl_omapl138\n", offset);
			break;
	}
	return 0;
}
static char* emac_ctl_omapl138_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t emac_ctl_omapl138_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	emac_ctl_omapl138_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t emac_ctl_omapl138_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t emac_ctl_omapl138_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	emac_ctl_omapl138_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t emac_ctl_omapl138_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_emac_ctl_omapl138(char *obj_name)
{
	emac_ctl_omapl138_device* dev = skyeye_mm_zero(sizeof(emac_ctl_omapl138_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	regs->revid = 0x4ec80100;

	return dev->obj;
}
static exception_t free_emac_ctl_omapl138(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_emac_ctl_omapl138(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "omapl138_emac_ctl",
		.class_desc = "omapl138_emac_ctl",
		.new_instance = new_emac_ctl_omapl138,
		.free_instance = free_emac_ctl_omapl138,
		.module_type = SKYML_ONCHIP(SC_omapl138),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = emac_ctl_omapl138_read,
		.write = emac_ctl_omapl138_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = emac_ctl_omapl138_get_regvalue_by_id,
		.get_regname_by_id = emac_ctl_omapl138_get_regname_by_id,
		.set_regvalue_by_id = emac_ctl_omapl138_set_regvalue_by_id,
		.get_regnum = emac_ctl_omapl138_get_regnum,
		.get_regid_by_name = emac_ctl_omapl138_get_regid_by_name
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

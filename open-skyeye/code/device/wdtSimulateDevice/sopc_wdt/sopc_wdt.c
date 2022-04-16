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
#include "sopc_wdt.h"

static wdt_callback(conf_object_t *opaque)
{
	sopc_wdt_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;

	regs->wdt_ccvr --;
	if (regs->wdt_ccvr == 0)
	{
		regs->wdt_stat = 1;
		if(dev->signal.obj)
			dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.num);

	}
}

static exception_t sopc_wdt_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_wdt_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;

	switch (offset)
	{
		case WDT_CR:
			*(uint32_t *)buf = regs->wdt_cr;
			break;
		case WDT_TORR:
			*(uint32_t *)buf = regs->wdt_torr;
			break;
		case WDT_CCVR:
			*(uint32_t *)buf = regs->wdt_ccvr;
			break;
		case WDT_CRR:
			*(uint32_t *)buf = 0;
			break;
		case WDT_STAT:
			*(uint32_t *)buf = regs->wdt_stat;
			break;
		case WDT_EOI:
			*(uint32_t *)buf = regs->wdt_eoi;
			regs->wdt_stat = 0; //clear interrupt
			break;
		case WDT_CP5:
			*(uint32_t *)buf = regs->wdt_cp5;
			break;
		case WDT_CP4:
			*(uint32_t *)buf = regs->wdt_cp4;
			break;
		case WDT_CP3:
			*(uint32_t *)buf = regs->wdt_cp3;
			break;
		case WDT_CP2:
			*(uint32_t *)buf = regs->wdt_cp2;
			break;
		case WDT_CP1:
			*(uint32_t *)buf = regs->wdt_cp1;
			break;
		case WDT_VER:
			*(uint32_t *)buf = regs->wdt_ver;
			break;
		case WDT_TYPE:
			*(uint32_t *)buf = regs->wdt_type;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sopc_wdt\n", offset);
			return Not_found_exp;
	}
	return No_exp;
}

static exception_t sopc_wdt_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_wdt_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	uint32_t val = *(uint32_t *)buf;

	switch (offset)
	{
		case WDT_CR:
			regs->wdt_cr = val;
			break;
		case WDT_TORR:
			regs->wdt_torr = val;
			break;
		case WDT_CCVR:
			regs->wdt_ccvr = val;
			break;
		case WDT_CRR:
			regs->wdt_crr = val;
			if (regs->wdt_crr == 0x76)
				regs->wdt_ccvr = 0xffff;
			break;
		case WDT_STAT:
			regs->wdt_stat = val;
			break;
		case WDT_EOI:
			regs->wdt_eoi = val;
			break;
		case WDT_CP5:
			regs->wdt_cp5 = val;
			break;
		case WDT_CP4:
			regs->wdt_cp4 = val;
			break;
		case WDT_CP3:
			regs->wdt_cp3 = val;
			break;
		case WDT_CP2:
			regs->wdt_cp2 = val;
			break;
		case WDT_CP1:
			regs->wdt_cp1 = val;
			break;
		case WDT_VER:
			regs->wdt_ver = val;
			break;
		case WDT_TYPE:
			regs->wdt_type = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sopc_wdt\n", offset);
			return Not_found_exp;
	}

	return No_exp;
}

static char* sopc_wdt_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t sopc_wdt_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	sopc_wdt_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t sopc_wdt_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t sopc_wdt_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	sopc_wdt_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t sopc_wdt_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t sopc_wdt_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static init_wdt_register(sopc_wdt_device *dev)
{
	struct registers *regs = &(dev->regs);
	regs->wdt_ccvr = 0xffff;
	regs->wdt_cp2 = 0xffff;
	regs->wdt_cp1 = 0x240;
	regs->wdt_ver = 0x3130332a;
	regs->wdt_type = 0x44570120;
}

static conf_object_t* new_sopc_wdt(const char *obj_name)
{
	sopc_wdt_device* dev = skyeye_mm_zero(sizeof(sopc_wdt_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	init_wdt_register(dev);
	return dev->obj;
}

static exception_t config_sopc_wdt(conf_object_t* opaque)
{
	sopc_wdt_device *dev =  opaque->obj;
	system_register_timer_handler(opaque, 1000, (time_sched_t)wdt_callback, SCHED_US|SCHED_NORMAL);
	return No_exp;
}

static exception_t free_sopc_wdt(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sopc_wdt_device *dev =  obj->obj;
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
	sopc_wdt_device *dev =  obj->obj;
	*obj2 = dev->signal.obj;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	sopc_wdt_device *dev =  conf_obj->obj;
	attr_value_t int_number_attr = SKY_make_attr_uinteger(dev->signal.num);
	return int_number_attr;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	sopc_wdt_device *dev =  conf_obj->obj;
	dev->signal.num = SKY_attr_uinteger(*value);
	return No_exp;
}

static void sopc_wdt_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of sopc wdt");
	return;
}

void init_sopc_wdt(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "sopc_wdt",
		.class_desc = "sopc_wdt",
		.new_instance = new_sopc_wdt,
		.free_instance = free_sopc_wdt,
		.config_instance = config_sopc_wdt
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = sopc_wdt_read,
		.write = sopc_wdt_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = sopc_wdt_get_regvalue_by_id,
		.get_regname_by_id = sopc_wdt_get_regname_by_id,
		.set_regvalue_by_id = sopc_wdt_set_regvalue_by_id,
		.get_regnum = sopc_wdt_get_regnum,
		.get_regid_by_name = sopc_wdt_get_regid_by_name,
		.get_regoffset_by_id = sopc_wdt_get_regoffset_by_id
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
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	sopc_wdt_register_attribute(clss);
}

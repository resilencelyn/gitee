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
#include "htey_chk_timer.h"
static exception_t htey_chk_timer_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_chk_timer_device *dev = opaque->obj;
	switch (offset)
	{
		case CHKTIMCOUNTBYTE0:
			*(uint32_t *)buf = dev->regs.chktimcountbyte0;
			break;
		case CHKTIMCOUNTBYTE1:
			*(uint32_t *)buf = dev->regs.chktimcountbyte1;
			break;
		case CHKTIMCOUNTBYTE2:
			*(uint32_t *)buf = dev->regs.chktimcountbyte2;
			break;
		case CHKTIMCOUNTBYTE3:
			*(uint32_t *)buf = dev->regs.chktimcountbyte3;
			break;
		case GNDCHKTIMEN:
			*(uint32_t *)buf = dev->regs.gndchktimen;
			break;
		case GPSCHKTIMEN:
			*(uint32_t *)buf = dev->regs.gpschktimen;
			break;
		case CHKTIMCOUNTRDFLAG:
			*(uint32_t *)buf = dev->regs.chktimcountrdflag;
			break;
		case CHKTIMCOUNTCLRFLAG:
			*(uint32_t *)buf = dev->regs.chktimcountclrflag;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in htey_chk_timer\n", offset);
			break;
	}
	return 0;
}
static exception_t htey_chk_timer_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_chk_timer_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case CHKTIMCOUNTBYTE0:
			dev->regs.chktimcountbyte0 = val;
			break;
		case CHKTIMCOUNTBYTE1:
			dev->regs.chktimcountbyte1 = val;
			break;
		case CHKTIMCOUNTBYTE2:
			dev->regs.chktimcountbyte2 = val;
			break;
		case CHKTIMCOUNTBYTE3:
			dev->regs.chktimcountbyte3 = val;
			break;
		case GNDCHKTIMEN:
			dev->regs.gndchktimen = val;
			break;
		case GPSCHKTIMEN:
			dev->regs.gpschktimen = val;
			break;
		case CHKTIMCOUNTRDFLAG:
			dev->regs.chktimcountrdflag = val;
			break;
		case CHKTIMCOUNTCLRFLAG:
			dev->regs.chktimcountclrflag = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in htey_chk_timer\n", offset);
			break;
	}
	return 0;
}
static char* htey_chk_timer_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t htey_chk_timer_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	htey_chk_timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t htey_chk_timer_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t htey_chk_timer_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	htey_chk_timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t htey_chk_timer_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_htey_chk_timer(char *obj_name)
{
	htey_chk_timer_device* dev = skyeye_mm_zero(sizeof(htey_chk_timer_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	return dev->obj;
}
static exception_t free_htey_chk_timer(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_htey_chk_timer(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "htey_chk_timer",
		.class_desc = "htey_chk_timer",
		.new_instance = new_htey_chk_timer,
		.free_instance = free_htey_chk_timer
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = htey_chk_timer_read,
		.write = htey_chk_timer_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = htey_chk_timer_get_regvalue_by_id,
		.get_regname_by_id = htey_chk_timer_get_regname_by_id,
		.set_regvalue_by_id = htey_chk_timer_set_regvalue_by_id,
		.get_regnum = htey_chk_timer_get_regnum,
		.get_regid_by_name = htey_chk_timer_get_regid_by_name
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

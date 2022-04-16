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
#include "sopc_timer.h"

static timer_callback(conf_object_t *opaque)
{
	sopc_timer_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	int i;

	for (i = 0; i < 4; i++)
	{
		if (regs->timer[i].TimerControlReg & TIMER_ENABLE)
		{
			regs->timer[i].TimerCurrentValue -= (dev->stepCount - i* 0x14);
			if (regs->timer[i].TimerCurrentValue <= 0)
			{
				if ((regs->timer[i].TimerControlReg & TIMER_INT_MASK) == 0)
				{
					if(dev->signal.obj)
						dev->signal.intf->raise_signal(dev->signal.obj, TIMER_INT_NUM);
				}
			}
		}
	}
}

static exception_t sopc_timer_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_timer_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;

	if (offset < TIMER_END_ADDR)
	{
		int m = offset / 0x14; //timer device num
		int n = offset % 0x14; //timer reg id
		if (n == LOAD_COUNT_REG)
		{
			*(uint32_t *)buf = regs->timer[m].TimerLoadCount;
			return No_exp;
		}
		if (n == CURRENT_VALUE_REG)
		{
			*(uint32_t *)buf = regs->timer[m].TimerCurrentValue;
			return No_exp;
		}
		if (n == CONTROL_REG)
		{
			*(uint32_t *)buf = regs->timer[m].TimerControlReg;
			return No_exp;
		}
		if (n == EOI_REG)
		{
			*(uint32_t *)buf = 0;
			return No_exp;
		}
		if (n == INT_STATUS_REG)
		{
			*(uint32_t *)buf = regs->timer[m].TimerIntStatus;
			return No_exp;
		}
	}

	switch (offset)
	{
		case TIMERIntStatus:
			*(uint32_t *)buf = regs->TimersIntStatus;
			break;
		case TIMEREOI:
			*(uint32_t *)buf = 0;
			break;
		case TIMERRawIntStatus:
			*(uint32_t *)buf = regs->TimersRawIntStatus;
			break;
		case TIMERS_COMP_VERSION:
			*(uint32_t *)buf = regs->TimersCompVersion;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sopc_timer\n", offset);
			return Not_found_exp;
	}
//printf("%s offset: 0x%x buf: 0x%x\n", __func__, offset, *(uint32_t *)buf);	
	return No_exp;
}

static exception_t sopc_timer_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_timer_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	uint32_t val = *(uint32_t *)buf;
//printf("%s offset: 0x%x buf: 0x%x\n", __func__, offset, val);
	if (offset < TIMER_END_ADDR)
	{
		int m = offset / 0x14; //timer device num
		int n = offset % 0x14; //timer reg id
		if (n == LOAD_COUNT_REG)
		{
			regs->timer[m].TimerLoadCount = val;
			regs->timer[m].TimerCurrentValue = val;
			return No_exp;
		}
		if (n == CURRENT_VALUE_REG)
		{
			regs->timer[m].TimerCurrentValue = val;
			return No_exp;
		}
		if (n == CONTROL_REG)
		{
			regs->timer[m].TimerControlReg = val;
			return No_exp;
		}
		if (n == EOI_REG)
		{
			regs->timer[m].TimerEOI = val;
			return No_exp;
		}
		if (n == INT_STATUS_REG)
		{
			regs->timer[m].TimerIntStatus = val;
			return No_exp;
		}
	}
	switch (offset)
	{
		case TIMERIntStatus:
			regs->TimersIntStatus = val;
			break;
		case TIMEREOI:
			regs->TimersEOI = val;
			break;
		case TIMERRawIntStatus:
			regs->TimersRawIntStatus = val;
			break;
		case TIMERS_COMP_VERSION:
			regs->TimersCompVersion = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sopc_timer\n", offset);
			return Not_found_exp;
	}

	return No_exp;
}

static char* sopc_timer_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t sopc_timer_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	sopc_timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t sopc_timer_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t sopc_timer_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	sopc_timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t sopc_timer_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t sopc_timer_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static init_timer_register(sopc_timer_device *dev)
{
	struct registers *regs = &(dev->regs);
}

static conf_object_t* new_sopc_timer(const char *obj_name)
{
	sopc_timer_device* dev = skyeye_mm_zero(sizeof(sopc_timer_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	dev->stepCount = CONFIG_AHB_CLK / 1000000;
	init_timer_register(dev);
	return dev->obj;
}

static exception_t config_sopc_timer(conf_object_t* opaque)
{
	sopc_timer_device *dev =  opaque->obj;
	system_register_timer_handler(opaque, 1, (time_sched_t)timer_callback, SCHED_US|SCHED_NORMAL);

	return No_exp;
}

static exception_t free_sopc_timer(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sopc_timer_device *dev =  obj->obj;
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

static exception_t signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	sopc_timer_device *dev =  obj->obj;
	*obj2 = dev->signal.obj;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_time(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	sopc_timer_device *dev =  conf_obj->obj;
	attr_value_t time = SKY_make_attr_uinteger(dev->time);
	return time;
}

static exception_t set_attr_time(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	sopc_timer_device *dev =  conf_obj->obj;
	dev->time = SKY_attr_uinteger(*value);
	return No_exp;
}

static void sopc_timer_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "time", get_attr_time, NULL, set_attr_time, NULL, SKY_Attr_Optional, "uinteger", "set time(us) to signal interrupt");
	return;
}

void init_sopc_timer(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "sopc_timer",
		.class_desc = "sopc_timer",
		.new_instance = new_sopc_timer,
		.free_instance = free_sopc_timer,
		.config_instance = config_sopc_timer
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = sopc_timer_read,
		.write = sopc_timer_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = sopc_timer_get_regvalue_by_id,
		.get_regname_by_id = sopc_timer_get_regname_by_id,
		.set_regvalue_by_id = sopc_timer_set_regvalue_by_id,
		.get_regnum = sopc_timer_get_regnum,
		.get_regid_by_name = sopc_timer_get_regid_by_name,
		.get_regoffset_by_id = sopc_timer_get_regoffset_by_id
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

	sopc_timer_register_attribute(clss);
}

/*************************************************************************
  > File Name    : ft2000a_wdt.c
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/11/25 10:10:41
  > This is ft-2000A WDT(ø¥√≈π∑) register device
 ************************************************************************/

#include <skyeye_lock.h>
#include <skyeye_types.h>
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
#include "ft2000a_wdt.h"

void wdt_count_timer(conf_object_t *opaque)
{
	ft2000a_wdt_device *dev = opaque->obj;
	struct wdt_registers *regs = &dev->regs;

	regs->wdt_ccvr --;

	if (regs->wdt_ccvr == 0)
	{
		if (regs->wdt_cr.RMOD == 1)
		{
			dev->wdt_int_cnt ++;
			if (dev->wdt_int_cnt == 2 && ((regs->wdt_eoi & 1) == 0))
			{
				//reset system
				dev->wdt_int_cnt = 0;
			} else
			{
				regs->wdt_stat |= 0x1; //valid wdt interrupt
				if (dev->signal_iface)
					dev->signal_iface->raise_signal(dev->signal, dev->int_number);
			}
		} else
		{
			//reset system
		}
	}
}

static exception_t ft2000a_wdt_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_wdt_device *dev = opaque->obj;
	struct wdt_registers *regs = &dev->regs;
	uint32_t *data = (uint32_t *)buf;

	switch (offset)
	{
		case WDT_CR:
			*data = regs->wdt_cr.all;
			break;
		case WDT_TORR:
			*data = regs->wdt_torr.all;
			break;
		case WDT_CCVR:
			*data = regs->wdt_ccvr;
			break;
		case WDT_STAT:
			*data = regs->wdt_stat;
			break;
		case WDT_EOI:
			*data = regs->wdt_eoi;
			break;
		case WDT_COMP_PARAMS_5 ... WDT_COMP_PARAMS_1:
			{
				int n = (offset - WDT_COMP_PARAMS_5) / 4;
				*data = regs->wdt_comp_params[n];
			}
			break;
		case WDT_COMP_VERSION:
			*data = regs->wdt_comp_version;
			break;
		case WDT_COMP_TYPE:
			*data = regs->wdt_comp_type;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in ft2000a_wdt\n", offset);
			break;
	}

	return No_exp;
}

static exception_t ft2000a_wdt_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_wdt_device *dev = opaque->obj;
	struct wdt_registers *regs = &dev->regs;
	uint32_t data = *(uint32_t *)buf;

	switch (offset)
	{
		case WDT_CR:
			regs->wdt_cr.all = data;
			double period = DEFAULT_CLK / (1 << regs->wdt_cr.RPL);
			if (regs->wdt_cr.WDT_EN)
			{
				if (dev->handle == NULL)
					dev->handle = system_register_timer_handler(opaque, (int)period, (time_sched_t)wdt_count_timer, SCHED_US|SCHED_NORMAL);
				else
					system_enable_timer(dev->handle);
			} else
				system_disable_timer(dev->handle);
			break;
		case WDT_TORR:
			if (regs->wdt_cr.WDT_EN == 0)
			{
				regs->wdt_torr.all = data;
				regs->wdt_ccvr = data;
			}
			break;
		case WDT_CRR:
			regs->wdt_crr = data;
			if (regs->wdt_crr == 0x76)
			{
				regs->wdt_ccvr = 0;
				regs->wdt_eoi |= 1;
				//clear wdt interrupt
				if (dev->signal_iface)
					dev->signal_iface->lower_signal(dev->signal, dev->int_number);
			}
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in ft2000a_wdt\n", offset);
			break;
	}
	return No_exp;
}

static char* ft2000a_wdt_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t ft2000a_wdt_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	ft2000a_wdt_device *dev = conf_obj->obj;
	struct wdt_registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t ft2000a_wdt_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct wdt_registers) / 4;
}

static uint32_t ft2000a_wdt_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	ft2000a_wdt_device *dev = conf_obj->obj;
	struct wdt_registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t ft2000a_wdt_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct wdt_registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t ft2000a_wdt_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

void init_wdt_register(struct wdt_registers *regs)
{
	regs->wdt_comp_version = 0x3230312a;
	regs->wdt_comp_type = 0x44570120;
}

static conf_object_t* new_ft2000a_wdt(const char *obj_name)
{
	ft2000a_wdt_device* dev = skyeye_mm_zero(sizeof(ft2000a_wdt_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct wdt_registers *regs = &(dev->regs);
	init_wdt_register(regs);
	return dev->obj;
}

static exception_t free_ft2000a_wdt(conf_object_t* opaque)
{
	ft2000a_wdt_device *dev = opaque->obj;
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t config_ft2000a_wdt(conf_object_t* opaque)
{
	ft2000a_wdt_device *dev = opaque->obj;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	ft2000a_wdt_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	ft2000a_wdt_device *dev = conf_obj->obj;
	dev->int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void ft2000a_wdt_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of wdt.");
}

static exception_t set_general_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	ft2000a_wdt_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	return No_exp;
}

static exception_t get_general_signal(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	ft2000a_wdt_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void init_ft2000a_wdt(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "ft2000a_wdt",
		.class_desc = "ft2000a_wdt",
		.new_instance = new_ft2000a_wdt,
		.free_instance = free_ft2000a_wdt,
		.config_instance = config_ft2000a_wdt
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = ft2000a_wdt_read,
		.write = ft2000a_wdt_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = ft2000a_wdt_get_regvalue_by_id,
		.get_regname_by_id = ft2000a_wdt_get_regname_by_id,
		.set_regvalue_by_id = ft2000a_wdt_set_regvalue_by_id,
		.get_regnum = ft2000a_wdt_get_regnum,
		.get_regid_by_name = ft2000a_wdt_get_regid_by_name,
		.get_regoffset_by_id = ft2000a_wdt_get_regoffset_by_id
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
			.set = set_general_signal,
			.get = get_general_signal,
		},
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	ft2000a_wdt_register_attribute(clss);
}

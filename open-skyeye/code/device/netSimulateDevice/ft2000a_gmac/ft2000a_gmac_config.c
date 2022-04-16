/*************************************************************************
	> File Name    : ft2000a_gmac.c
	> Author       : jiachao
	> Mail         : jiachao@163.com
	> Created Time : 2019/11/27 10:10:41
	> This is ft-2000A GMAC(��̫��������) register device
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
#define DEBUG
#include <skyeye_log.h>
#include "ft2000a_gmac.h"

static exception_t ft2000a_gmac_config_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_gmac_device *dev = opaque->obj;
	struct gmac_registers *regs = &dev->regs;
	uint32_t *data = (uint32_t *)buf;

	switch (offset)
	{
		case GMAC0_AWCACHE:
			*data = regs->gmac_conf_regs.gmac0_awcache;
			break;
		case GMAC0_ARCACHE:
			*data = regs->gmac_conf_regs.gmac0_arcache;
			break;
		case GMAC1_AWCACHE:
			*data = regs->gmac_conf_regs.gmac1_awcache;
			break;
		case GMAC1_ARCACHE:
			*data = regs->gmac_conf_regs.gmac1_arcache;
			break;
		case GMAC_AWDOMAIN:
			*data = regs->gmac_conf_regs.gmac_awdomain;
			break;
		case GMAC_ARDOMAIN:
			*data = regs->gmac_conf_regs.gmac_ardomain;
			break;
		case GMAC_AWBAR:
			*data = regs->gmac_conf_regs.gmac_awbar;
			break;
		case GMAC_ARBAR:
			*data = regs->gmac_conf_regs.gmac_arbar;
			break;
		case GMAC_AWSNOOP:
			*data = regs->gmac_conf_regs.gmac_awsnoop;
			break;
		case GMAC_ARSNOOP:
			*data = regs->gmac_conf_regs.gmac_arsnoop;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in ft2000a_gmac\n", offset);
			break;
	}

	return 0;
}

static exception_t ft2000a_gmac_config_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_gmac_device *dev = opaque->obj;
	struct gmac_registers *regs = &dev->regs;
	uint32_t data = *(uint32_t *)buf;

	switch (offset)
	{
		case GMAC0_AWCACHE:
			regs->gmac_conf_regs.gmac0_awcache = data;
			break;
		case GMAC0_ARCACHE:
			regs->gmac_conf_regs.gmac0_arcache = data;
			break;
		case GMAC1_AWCACHE:
			regs->gmac_conf_regs.gmac1_awcache = data;
			break;
		case GMAC1_ARCACHE:
			regs->gmac_conf_regs.gmac1_arcache = data;
			break;
		case GMAC_AWDOMAIN:
			regs->gmac_conf_regs.gmac_awdomain = data;
			break;
		case GMAC_ARDOMAIN:
			regs->gmac_conf_regs.gmac_ardomain = data;
			break;
		case GMAC_AWBAR:
			regs->gmac_conf_regs.gmac_awbar = data;
			break;
		case GMAC_ARBAR:
			regs->gmac_conf_regs.gmac_arbar = data;
			break;
		case GMAC_AWSNOOP:
			regs->gmac_conf_regs.gmac_awsnoop = data;
			break;
		case GMAC_ARSNOOP:
			regs->gmac_conf_regs.gmac_arsnoop = data;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in ft2000a_gmac\n", offset);
			break;
	}
	return 0;
}

static char* gmac_config_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return config_regs_name[id];
}

static exception_t gmac_config_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	ft2000a_gmac_device *dev = conf_obj->obj;
	gmac_config_reg_t *regs = &(dev->regs.gmac_conf_regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t gmac_config_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(gmac_config_reg_t) / 4;
}

static uint32_t gmac_config_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	ft2000a_gmac_device *dev = conf_obj->obj;
	gmac_config_reg_t *regs = &(dev->regs.gmac_conf_regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t gmac_config_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(gmac_config_reg_t) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, config_regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t gmac_config_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return config_regs_offset[id];
}

void init_gmac_config_register(struct gmac_registers *regs)
{
	regs->gmac_conf_regs.gmac0_awcache = 0x2;
	regs->gmac_conf_regs.gmac0_arcache = 0x2;
	regs->gmac_conf_regs.gmac1_awcache = 0x2;
	regs->gmac_conf_regs.gmac1_arcache = 0x2;
	regs->gmac_conf_regs.gmac_awdomain = 0x1;
	regs->gmac_conf_regs.gmac_ardomain = 0x1;
	regs->gmac_conf_regs.gmac_awbar = 0;
	regs->gmac_conf_regs.gmac_arbar = 0;
	regs->gmac_conf_regs.gmac_awsnoop = 0;
	regs->gmac_conf_regs.gmac_arsnoop = 0;
}

static conf_object_t* new_ft2000a_gmac_config(const char *obj_name)
{
	ft2000a_gmac_device* dev = skyeye_mm_zero(sizeof(ft2000a_gmac_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct gmac_registers *regs = &(dev->regs);
	init_gmac_config_register(regs);
	return dev->obj;
}

static exception_t free_ft2000a_gmac_config(conf_object_t* opaque)
{
	ft2000a_gmac_device *dev = opaque->obj;
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t config_ft2000a_gmac_config(conf_object_t* opaque)
{
	ft2000a_gmac_device *dev = opaque->obj;
	return No_exp;
}

static void gmac_config_register_attribute(conf_class_t* clss){
}

void init_ft2000a_gmac_config(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "ft2000a_gmac_config",
		.class_desc = "ft2000a_gmac_config",
		.new_instance = new_ft2000a_gmac_config,
		.free_instance = free_ft2000a_gmac_config,
		.config_instance = config_ft2000a_gmac_config
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = ft2000a_gmac_config_read,
		.write = ft2000a_gmac_config_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = gmac_config_get_regvalue_by_id,
		.get_regname_by_id = gmac_config_get_regname_by_id,
		.set_regvalue_by_id = gmac_config_set_regvalue_by_id,
		.get_regnum = gmac_config_get_regnum,
		.get_regid_by_name = gmac_config_get_regid_by_name,
		.get_regoffset_by_id = gmac_config_get_regoffset_by_id
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
	gmac_config_register_attribute(clss);
}

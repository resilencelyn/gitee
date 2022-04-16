/*************************************************************************
	> File Name    : ft2000a_lpc.c
	> Author       : jiachao
	> Mail         : jiachao@163.com
	> Created Time : 2019/11/27 15:10:41
	> This is ft-2000A LPC register device
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
#include "ft2000a_lpc.h"

static exception_t ft2000a_lpc_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_lpc_device *dev = opaque->obj;
	struct lpc_registers *regs = &dev->regs;
	uint32_t *data = (uint32_t *)buf;
	exception_t ret;

	switch (offset)
	{
		case INT_MASK:
			*data = regs->int_mask;
			break;
		case DMA_DEVID_CONF:
			*data = regs->dma_devid_conf;
			break;
		case FIRMW_ID_CONF:
			*data = regs->firmware_id_conf;
			break;
		case ERROR_STATE:
			*data = regs->error_state;
			break;
		case INT_CONF:
			*data = regs->int_conf;
			break;
		case FIRMW_LEN_CONF:
			*data = regs->firmware_len_conf;
			break;
		case CLR_INT:
			*data = regs->clr_int;
			break;
		case INT_STATE:
			*data = regs->int_state;
			break;
		case LONG_TIMEOUT:
			*data = regs->long_timeout;
			break;
		case INT_APB_SPACE_CONF:
			*data = regs->apb_space_conf;
			break;
		default:
			ret = dev->image_space->read(dev->image, offset, data, count);
			if (ret != No_exp)
			{
				skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in ft2000a_lpc\n", offset);
				return Not_found_exp;
			}
	}

	return No_exp;
}

static exception_t ft2000a_lpc_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_lpc_device *dev = opaque->obj;
	struct lpc_registers *regs = &dev->regs;
	uint32_t data = *(uint32_t *)buf;
	exception_t ret;

	switch (offset)
	{
		case INT_MASK:
			regs->int_mask = data;
			break;
		case DMA_DEVID_CONF:
			regs->dma_devid_conf = data;
			break;
		case FIRMW_ID_CONF:
			regs->firmware_id_conf = data;
			break;
		case INT_CONF:
			regs->int_conf = data;
			break;
		case FIRMW_LEN_CONF:
			regs->firmware_len_conf = data;
			break;
		case CLR_INT:
			regs->clr_int = data;
			break;
		case LONG_TIMEOUT:
			regs->long_timeout = data;
			break;
		case INT_APB_SPACE_CONF:
			regs->apb_space_conf = data;
			break;
		default:
			ret = dev->image_space->write(dev->image, offset, &data, count);
			if(ret != No_exp)
			{
				skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in ft2000a_lpc\n", offset);
				return Not_found_exp;
			}
	}

	return No_exp;
}

static char* ft2000a_lpc_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t ft2000a_lpc_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	ft2000a_lpc_device *dev = conf_obj->obj;
	struct lpc_registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t ft2000a_lpc_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct lpc_registers) / 4;
}

static uint32_t ft2000a_lpc_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	ft2000a_lpc_device *dev = conf_obj->obj;
	struct lpc_registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t ft2000a_lpc_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct lpc_registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t ft2000a_lpc_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

void init_lpc_register(struct lpc_registers *regs)
{
	regs->apb_space_conf = 0xe4;
	regs->long_timeout = 0;
	regs->int_state = 0;
	regs->clr_int = 0;
	regs->firmware_len_conf = 0;
	regs->int_conf = 0;
	regs->error_state = 0;
	regs->firmware_id_conf = 0;
	regs->dma_devid_conf = 0x6;
	regs->int_mask = 0x3;
}

static conf_object_t* new_ft2000a_lpc(const char *obj_name)
{
	ft2000a_lpc_device* dev = skyeye_mm_zero(sizeof(ft2000a_lpc_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct lpc_registers *regs = &(dev->regs);
	init_lpc_register(regs);
	return dev->obj;
}

static exception_t free_ft2000a_lpc(conf_object_t* opaque)
{
	ft2000a_lpc_device *dev = opaque->obj;
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t config_ft2000a_lpc(conf_object_t* opaque)
{
	ft2000a_lpc_device *dev = opaque->obj;
	return No_exp;
}

static exception_t image_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	ft2000a_lpc_device* dev = (ft2000a_lpc_device*)obj->obj;
	dev->image = obj2;
	dev->image_space = (memory_space_intf*)SKY_get_iface(obj2, MEMORY_SPACE_INTF_NAME);

	return No_exp;
}

static exception_t image_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	ft2000a_lpc_device* dev = (ft2000a_lpc_device*)obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}

static void ft2000a_lpc_register_attribute(conf_class_t* clss)
{
}

void init_ft2000a_lpc(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "ft2000a_lpc",
		.class_desc = "ft2000a_lpc",
		.new_instance = new_ft2000a_lpc,
		.free_instance = free_ft2000a_lpc,
		.config_instance = config_ft2000a_lpc
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = ft2000a_lpc_read,
		.write = ft2000a_lpc_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = ft2000a_lpc_get_regvalue_by_id,
		.get_regname_by_id = ft2000a_lpc_get_regname_by_id,
		.set_regvalue_by_id = ft2000a_lpc_set_regvalue_by_id,
		.get_regnum = ft2000a_lpc_get_regnum,
		.get_regid_by_name = ft2000a_lpc_get_regid_by_name,
		.get_regoffset_by_id = ft2000a_lpc_get_regoffset_by_id
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
			.name = MEMORY_SPACE_INTF_NAME,
			.set = image_set,
			.get = image_get,
		}
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	ft2000a_lpc_register_attribute(clss);
}

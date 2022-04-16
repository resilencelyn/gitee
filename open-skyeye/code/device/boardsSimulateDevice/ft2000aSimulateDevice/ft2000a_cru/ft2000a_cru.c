/*************************************************************************
	> File Name    : ft2000a_cru.c
	> Author       : jiachao
	> Mail         : jiachao@163.com
	> Created Time : 2019/11/12 13:50:41
	> This is ft-2000A clock reset unit(CRU) control register device
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
#include "ft2000a_cru.h"

static exception_t ft2000a_cru_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_cru_device *dev = opaque->obj;
	struct cru_registers *regs = &dev->regs;
	uint32_t *data = (uint32_t *)buf;
	switch (offset)
	{
		case REG_FER_MASK:
			*data = regs->reg_fer_mask;
			break;
		case REG_SOFTRST_LOCAL:
			*data = regs->reg_softrst_local;
			break;
		case REG_RST_SOURCE:
			*data = regs->reg_rst_source;
			break;
		case REG_SOFTRST_CORE_SOURCE:
			*data = regs->reg_softrst_core_source;
			break;
		case REG_LPR_CORE_SOURCE:
			*data = regs->reg_lpr_core_source;
			break;
		case REG_LPR_MODULE_SOURCE:
			*data = regs->reg_lpr_module_source;
			break;
		case REG_SOFTRST_LOCAL_SOURCE:
			*data = regs->reg_softrst_local_source;
			break;
		case REG_WDT_RST_SOURCE:
			*data = regs->reg_wdt_rst_source;
			break;
		case REG_CORE_RST_SOURCE:
			*data = regs->reg_core_rst_source;
			break;
		case REG_HARD_WARMRST_CNT:
			*data = regs->reg_hard_warmrst_cnt;
			break;
		case REG_SOFT_WARMRST_CNT:
			*data = regs->reg_soft_warmrst_cnt;
			break;
		case REG_SOFTRST_CORE_CNT:
			*data = regs->reg_softrst_core_cnt;
			break;
		case REG_LPR_CNT:
			*data = regs->reg_lpr_cnt;
			break;
		case REG_FER_CNT:
			*data = regs->reg_fer_cnt;
			break;
		case REG_SOFTRST_LOCAL_CNT:
			*data = regs->reg_softrst_local_cnt;
			break;
		case REG_WDT_RST_CNT:
			*data = regs->reg_wdt_cnt;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in ft2000a_cru\n", offset);
			break;
	}

	return 0;
}

static exception_t ft2000a_cru_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_cru_device *dev = opaque->obj;
	struct cru_registers *regs = &dev->regs;
	uint32_t data = *(uint32_t *)buf;

	switch (offset)
	{
		case REG_SET_SOFT_WARMRST:
			regs->reg_set_soft_warmrst = data;
			break;
		case REG_FER_MASK:
			regs->reg_fer_mask = data;
			break;
		case REG_SOFTRST_LOCAL:
			regs->reg_softrst_local = data;
			break;
		case REG_SET_SOFTRST_LOCAL:
			regs->reg_set_softrst_local = data;
			//write 1 to trigger soft local reset
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in ft2000a_cru\n", offset);
			break;
	}
	return 0;
}

static char* ft2000a_cru_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t ft2000a_cru_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	ft2000a_cru_device *dev = conf_obj->obj;
	struct cru_registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t ft2000a_cru_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct cru_registers) / 4;
}

static uint32_t ft2000a_cru_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	ft2000a_cru_device *dev = conf_obj->obj;
	struct cru_registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t ft2000a_cru_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct cru_registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t ft2000a_cru_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

void init_cru_register(struct cru_registers *regs)
{
	regs->reg_set_soft_warmrst = 0;
	regs->reg_fer_mask = 1;
	regs->reg_softrst_local = 0;
	regs->reg_set_softrst_local = 0;
	regs->reg_rst_source = 1;
	regs->reg_softrst_core_source = 0;
	regs->reg_lpr_core_source = 0;
	regs->reg_lpr_module_source = 0;
	regs->reg_softrst_local_source = 0;
	regs->reg_wdt_rst_source = 0;
	regs->reg_core_rst_source = 0x101;
	regs->reg_hard_warmrst_cnt = 0;
	regs->reg_soft_warmrst_cnt = 0;
	regs->reg_softrst_core_cnt = 0;
	regs->reg_lpr_cnt = 0;
	regs->reg_fer_cnt = 0;
	regs->reg_softrst_local_cnt = 0;
	regs->reg_wdt_cnt = 0;
}

static conf_object_t* new_ft2000a_cru(const char *obj_name)
{
	ft2000a_cru_device* dev = skyeye_mm_zero(sizeof(ft2000a_cru_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct cru_registers *regs = &(dev->regs);
	init_cru_register(regs);
	return dev->obj;
}

static exception_t free_ft2000a_cru(conf_object_t* opaque)
{
	ft2000a_cru_device *dev = opaque->obj;
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t config_ft2000a_cru(conf_object_t* opaque)
{
	ft2000a_cru_device *dev = opaque->obj;
	return No_exp;
}

static attr_value_t get_poweron_reset_mode(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	ft2000a_cru_device *dev =  conf_obj->obj;
	attr_value_t poweron_reset_mode = SKY_make_attr_uinteger(dev->regs.reg_core_rst_source);
	return poweron_reset_mode;
}

static exception_t set_poweron_reset_mode(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	ft2000a_cru_device *dev =  conf_obj->obj;
	dev->regs.reg_core_rst_source = SKY_attr_uinteger(*value);
	return No_exp;
}

static void ft2000a_cru_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "poweron_reset_mode", get_poweron_reset_mode, NULL, set_poweron_reset_mode, NULL, SKY_Attr_Optional, "uinteger", "set poweron reset mode!");
}

void init_ft2000a_cru(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "ft2000a_cru",
		.class_desc = "ft2000a_cru",
		.new_instance = new_ft2000a_cru,
		.free_instance = free_ft2000a_cru,
		.config_instance = config_ft2000a_cru
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = ft2000a_cru_read,
		.write = ft2000a_cru_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = ft2000a_cru_get_regvalue_by_id,
		.get_regname_by_id = ft2000a_cru_get_regname_by_id,
		.set_regvalue_by_id = ft2000a_cru_set_regvalue_by_id,
		.get_regnum = ft2000a_cru_get_regnum,
		.get_regid_by_name = ft2000a_cru_get_regid_by_name,
		.get_regoffset_by_id = ft2000a_cru_get_regoffset_by_id
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
	ft2000a_cru_register_attribute(clss);
}

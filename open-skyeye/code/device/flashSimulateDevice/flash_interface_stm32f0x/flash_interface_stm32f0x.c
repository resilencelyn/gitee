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
#include "flash_interface_stm32f0x.h"
static exception_t flash_interface_stm32f0x_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	flash_interface_stm32f0x_device *dev = opaque->obj;
	switch (offset)
	{
		case FLASH_ACR:
			*(uint32_t *)buf = dev->regs.flash_acr;
			break;
		case FLASH_KEYR:
			*(uint32_t *)buf = dev->regs.flash_keyr;
			break;
		case FLASH_OPTKEYR:
			*(uint32_t *)buf = dev->regs.flash_optkeyr;
			break;
		case FLASH_SR:
			*(uint32_t *)buf = dev->regs.flash_sr;
			break;
		case FLASH_CR:
			*(uint32_t *)buf = dev->regs.flash_cr;
			break;
		case FLASH_AR:
			*(uint32_t *)buf = dev->regs.flash_ar;
			break;
		case FLASH_OBR:
			*(uint32_t *)buf = dev->regs.flash_obr;
			break;
		case FLASH_WRPR:
			*(uint32_t *)buf = dev->regs.flash_wrpr;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in flash_interface_stm32f0x\n", offset);
			break;
	}
    //printf("%d %s 0x%x %x\n",__LINE__,__func__,offset,*(uint32_t*)buf);
	return 0;
}
static exception_t flash_interface_stm32f0x_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	flash_interface_stm32f0x_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
    //printf("%d %s 0x%x %x\n",__LINE__,__func__,offset,val);
	switch (offset)
	{
		case FLASH_ACR:
			dev->regs.flash_acr = val;
			break;
		case FLASH_KEYR:
			dev->regs.flash_keyr = val;
			break;
		case FLASH_OPTKEYR:
			dev->regs.flash_optkeyr = val;
			break;
		case FLASH_SR:
			dev->regs.flash_sr = val;
			break;
		case FLASH_CR:
			dev->regs.flash_cr = val;
			break;
		case FLASH_AR:
			dev->regs.flash_ar = val;
			break;
		case FLASH_OBR:
			dev->regs.flash_obr = val;
			break;
		case FLASH_WRPR:
			dev->regs.flash_wrpr = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in flash_interface_stm32f0x\n", offset);
			break;
	}
	return 0;
}
static char* flash_interface_stm32f0x_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t flash_interface_stm32f0x_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	flash_interface_stm32f0x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t flash_interface_stm32f0x_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t flash_interface_stm32f0x_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	flash_interface_stm32f0x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t flash_interface_stm32f0x_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_flash_interface_stm32f0x(char *obj_name)
{
	flash_interface_stm32f0x_device* dev = skyeye_mm_zero(sizeof(flash_interface_stm32f0x_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	return dev->obj;
}
static exception_t free_flash_interface_stm32f0x(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_flash_interface_stm32f0x(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "flash_interface_stm32f0x",
		.class_desc = "flash_interface_stm32f0x",
		.new_instance = new_flash_interface_stm32f0x,
		.free_instance = free_flash_interface_stm32f0x,
		.module_type = SKYML_ONCHIP(SC_stm32),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = flash_interface_stm32f0x_read,
		.write = flash_interface_stm32f0x_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = flash_interface_stm32f0x_get_regvalue_by_id,
		.get_regname_by_id = flash_interface_stm32f0x_get_regname_by_id,
		.set_regvalue_by_id = flash_interface_stm32f0x_set_regvalue_by_id,
		.get_regnum = flash_interface_stm32f0x_get_regnum,
		.get_regid_by_name = flash_interface_stm32f0x_get_regid_by_name
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

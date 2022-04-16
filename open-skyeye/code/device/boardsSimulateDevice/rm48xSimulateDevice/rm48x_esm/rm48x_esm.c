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
#include "rm48x_esm.h"
static exception_t rm48x_esm_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_esm_device *dev = opaque->obj;
	switch (offset)
	{
		case ESMEEPAPR1:
			*(uint32_t *)buf = dev->regs.esmeepapr1;
			break;
		case ESMDEPAPR1:
			*(uint32_t *)buf = dev->regs.esmdepapr1;
			break;
		case ESMIESR1:
			*(uint32_t *)buf = dev->regs.esmiesr1;
			break;
		case ESMIECR1:
			*(uint32_t *)buf = dev->regs.esmiecr1;
			break;
		case ESMILSR1:
			*(uint32_t *)buf = dev->regs.esmilsr1;
			break;
		case ESMILCR1:
			*(uint32_t *)buf = dev->regs.esmilcr1;
			break;
		case ESMSR1:
			*(uint32_t *)buf = dev->regs.esmsr1;
			break;
		case ESMSR2:
			*(uint32_t *)buf = dev->regs.esmsr2;
			break;
		case ESMSR3:
			*(uint32_t *)buf = dev->regs.esmsr3;
			break;
		case ESMEPSR:
			*(uint32_t *)buf = dev->regs.esmepsr;
			break;
		case ESMIOFFHR:
			*(uint32_t *)buf = dev->regs.esmioffhr;
			dev->regs.esmsr2 |= 0;
			break;
		case ESMIOFFLR:
			*(uint32_t *)buf = dev->regs.esmiofflr;
			break;
		case ESMLTCR:
			*(uint32_t *)buf = dev->regs.esmltcr;
			break;
		case ESMLTCPR:
			*(uint32_t *)buf = dev->regs.esmltcpr;
			break;
		case ESMEKR:
			*(uint32_t *)buf = dev->regs.esmekr;
			break;
		case ESMSSR2:
			*(uint32_t *)buf = dev->regs.esmssr2;
			break;
		case ESMIEPSR4:
			*(uint32_t *)buf = dev->regs.esmiepsr4;
			break;
		case ESMIEPCR4:
			*(uint32_t *)buf = dev->regs.esmiepcr4;
			break;
		case ESMIESR4:
			*(uint32_t *)buf = dev->regs.esmiesr4;
			break;
		case ESMIECR4:
			*(uint32_t *)buf = dev->regs.esmiecr4;
			break;
		case ESMILSR4:
			*(uint32_t *)buf = dev->regs.esmilsr4;
			break;
		case ESMILCR4:
			*(uint32_t *)buf = dev->regs.esmilcr4;
			break;
		case ESMSR4:
			*(uint32_t *)buf = dev->regs.esmsr4;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rm48x_esm\n", offset);
			break;
	}
	return 0;
}
static exception_t rm48x_esm_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_esm_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case ESMEEPAPR1:
			dev->regs.esmeepapr1 = val;
			break;
		case ESMDEPAPR1:
			dev->regs.esmdepapr1 = val;
			break;
		case ESMIESR1:
			dev->regs.esmiesr1 = val;
			if (val != 0){
				dev->regs.esmiecr1 = val;
			}
			break;
		case ESMIECR1:
			dev->regs.esmiecr1 = val;
			if (val != 0){
				dev->regs.esmiesr1 &= ~val;
			}
			break;
		case ESMILSR1:
			dev->regs.esmilsr1 = val;
			if (val != 0){
				dev->regs.esmilcr1 = val;
			}
			break;
		case ESMILCR1:
			dev->regs.esmilcr1 = val;
			if (val != 0){
				dev->regs.esmilsr1 = val;
			}
			break;
		case ESMSR1:
			dev->regs.esmsr1 &= ~val;
			break;
		case ESMSR2:
			dev->regs.esmsr2 &= ~val;
			break;
		case ESMSR3:
			dev->regs.esmsr3 &= ~val;
			break;
		case ESMEPSR:
			dev->regs.esmepsr = val;
			break;
		case ESMIOFFHR:
			dev->regs.esmioffhr = val;
			break;
		case ESMIOFFLR:
			dev->regs.esmiofflr = val;
			break;
		case ESMLTCR:
			dev->regs.esmltcr = val;
			break;
		case ESMLTCPR:
			dev->regs.esmltcpr = val;
			break;
		case ESMEKR:
			dev->regs.esmekr = val;
			break;
		case ESMSSR2:
			dev->regs.esmssr2 &= ~val;
			break;
		case ESMIEPSR4:
			dev->regs.esmiepsr4 = val;
			if (val != 0){
				dev->regs.esmiepcr4 = val;
			}
			break;
		case ESMIEPCR4:
			dev->regs.esmiepcr4 = val;
			if (val != 0){
				dev->regs.esmiepsr4 &= ~val;
			}
			break;
		case ESMIESR4:
			dev->regs.esmiesr4 = val;
			if (val != 0){
				dev->regs.esmiecr4 = val;
			}
			break;
		case ESMIECR4:
			dev->regs.esmiecr4 = val;
			if (val != 0){
				dev->regs.esmiesr4 &= ~val;
			}
			break;
		case ESMILSR4:
			dev->regs.esmilsr4 = val;
			if (val != 0){
				dev->regs.esmilcr4 = val;
			}
			break;
		case ESMILCR4:
			dev->regs.esmilcr4 = val;
			if (val != 0){
				dev->regs.esmilsr4 &= ~val;
			}
			break;
		case ESMSR4:
			dev->regs.esmsr4 &= ~val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rm48x_esm\n", offset);
			break;
	}
	return 0;
}
static char* rm48x_esm_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t rm48x_esm_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	rm48x_esm_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t rm48x_esm_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t rm48x_esm_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	rm48x_esm_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t rm48x_esm_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_rm48x_esm(const char *obj_name)
{
	rm48x_esm_device* dev = skyeye_mm_zero(sizeof(rm48x_esm_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->regs.esmltcr = 0x3fff;
	dev->regs.esmltcpr = 0x3fff;
	dev->regs.esmepsr = 0x1;
	return dev->obj;
}
static exception_t free_rm48x_esm(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_rm48x_esm(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "rm48x_esm",
		.class_desc = "rm48x_esm",
		.new_instance = new_rm48x_esm,
		.free_instance = free_rm48x_esm
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = rm48x_esm_read,
		.write = rm48x_esm_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = rm48x_esm_get_regvalue_by_id,
		.get_regname_by_id = rm48x_esm_get_regname_by_id,
		.set_regvalue_by_id = rm48x_esm_set_regvalue_by_id,
		.get_regnum = rm48x_esm_get_regnum,
		.get_regid_by_name = rm48x_esm_get_regid_by_name
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

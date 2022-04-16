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
#include "rm48x_pcr.h"
static exception_t rm48x_pcr_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_pcr_device *dev = opaque->obj;
	switch (offset)
	{
		case PMPROTSET0:
			*(uint32_t *)buf = dev->regs.pmprotset0;
			break;
		case PMPROTSET1:
			*(uint32_t *)buf = dev->regs.pmprotset1;
			break;
		case PMPROTCLR0:
			*(uint32_t *)buf = dev->regs.pmprotclr0;
			break;
		case PMPROTCLR1:
			*(uint32_t *)buf = dev->regs.pmprotclr1;
			break;
		case PPROTSET0:
			*(uint32_t *)buf = dev->regs.pprotset0;
			break;
		case PPROTSET1:
			*(uint32_t *)buf = dev->regs.pprotset1;
			break;
		case PPROTSET2:
			*(uint32_t *)buf = dev->regs.pprotset2;
			break;
		case PPROTSET3:
			*(uint32_t *)buf = dev->regs.pprotset3;
			break;
		case PPROTCLR0:
			*(uint32_t *)buf = dev->regs.pprotclr0;
			break;
		case PPROTCLR1:
			*(uint32_t *)buf = dev->regs.pprotclr1;
			break;
		case PPROTCLR2:
			*(uint32_t *)buf = dev->regs.pprotclr2;
			break;
		case PPROTCLR3:
			*(uint32_t *)buf = dev->regs.pprotclr3;
			break;
		case PCSPWRDWNSET0:
			*(uint32_t *)buf = dev->regs.pcspwrdwnset0;
			break;
		case PCSPWRDWNSET1:
			*(uint32_t *)buf = dev->regs.pcspwrdwnset1;
			break;
		case PCSPWRDWNCLR0:
			*(uint32_t *)buf = dev->regs.pcspwrdwnclr0;
			break;
		case PCSPWRDWNCLR1:
			*(uint32_t *)buf = dev->regs.pcspwrdwnclr1;
			break;
		case PSPWRDWNSET0:
			*(uint32_t *)buf = dev->regs.pspwrdwnset0;
			break;
		case PSPWRDWNSET1:
			*(uint32_t *)buf = dev->regs.pspwrdwnset1;
			break;
		case PSPWRDWNSET2:
			*(uint32_t *)buf = dev->regs.pspwrdwnset2;
			break;
		case PSPWRDWNSET3:
			*(uint32_t *)buf = dev->regs.pspwrdwnset3;
			break;
		case PSPWRDWNCLR0:
			*(uint32_t *)buf = dev->regs.pspwrdwnclr0;
			break;
		case PSPWRDWNCLR1:
			*(uint32_t *)buf = dev->regs.pspwrdwnclr1;
			break;
		case PSPWRDWNCLR2:
			*(uint32_t *)buf = dev->regs.pspwrdwnclr2;
			break;
		case PSPWRDWNCLR3:
			*(uint32_t *)buf = dev->regs.pspwrdwnclr3;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rm48x_pcr\n", offset);
			break;
	}
	return 0;
}
static exception_t rm48x_pcr_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_pcr_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case PMPROTSET0:
			dev->regs.pmprotset0 = val;
			if (val != 0)
				dev->regs.pmprotclr0 = val;
			break;
		case PMPROTSET1:
			dev->regs.pmprotset1 = val;
			if (val != 0)
				dev->regs.pmprotclr1 = val;
			break;
		case PMPROTCLR0:
			dev->regs.pmprotclr0 = val;
			if (val != 0){
				dev->regs.pmprotclr0 &= ~val;
				dev->regs.pmprotset0 |= 0;
			}
			break;
		case PMPROTCLR1:
			dev->regs.pmprotclr1 = val;
			if (val != 0){
				dev->regs.pmprotclr1 &= ~val;
				dev->regs.pmprotset1 |= 0;
			}
			break;
		case PPROTSET0:
			dev->regs.pprotset0 = val;
			if (val != 0){
				dev->regs.pprotclr0 = val;
			}
			break;
		case PPROTSET1:
			dev->regs.pprotset1 = val;
			if (val != 0){
				dev->regs.pprotclr1 = val;
			}
			break;
		case PPROTSET2:
			dev->regs.pprotset2 = val;
			if (val != 0){
				dev->regs.pprotclr2 = val;
			}
			break;
		case PPROTSET3:
			dev->regs.pprotset3 = val;
			if (val != 0){
				dev->regs.pprotclr3 = val;
			}
			break;
		case PPROTCLR0:
			dev->regs.pprotclr0 = val;
			if (val != 0){
				dev->regs.pprotclr0 &= ~val;
				dev->regs.pprotset0 |= 0;
			}
			break;
		case PPROTCLR1:
			dev->regs.pprotclr1 = val;
			if (val != 0){
				dev->regs.pprotclr1 &= ~val;
				dev->regs.pprotset1 |= 0;
			}
			break;
		case PPROTCLR2:
			dev->regs.pprotclr2 = val;
			if (val != 0){
				dev->regs.pprotclr2 &= ~val;
				dev->regs.pprotset2 |= 0;
			}
			break;
		case PPROTCLR3:
			dev->regs.pprotclr3 = val;
			if (val != 0){
				dev->regs.pprotclr3 &= ~val;
				dev->regs.pprotset3 |= 0;
			}
			break;
		case PCSPWRDWNSET0:
			dev->regs.pcspwrdwnset0 = val;
			if (val != 0){
				dev->regs.pcspwrdwnclr0 = val;
			}
			break;
		case PCSPWRDWNSET1:
			dev->regs.pcspwrdwnset1 = val;
			if (val != 0){
				dev->regs.pcspwrdwnclr1 = val;
			}
			break;
		case PCSPWRDWNCLR0:
			dev->regs.pcspwrdwnclr0 = val;
			if (val != 0){
				dev->regs.pcspwrdwnclr0 &= ~val;
				dev->regs.pcspwrdwnset0 |= 0;
			}
			break;
		case PCSPWRDWNCLR1:
			dev->regs.pcspwrdwnclr1 = val;
			if (val != 0){
				dev->regs.pcspwrdwnclr1 &= ~val;
				dev->regs.pcspwrdwnset1 |= 0;
			}
			break;
		case PSPWRDWNSET0:
			dev->regs.pspwrdwnset0 = val;
			if (val != 0){
				dev->regs.pspwrdwnset0 = val;
			}
			break;
		case PSPWRDWNSET1:
			dev->regs.pspwrdwnset1 = val;
				if (val != 0){
				dev->regs.pspwrdwnset1 = val;
			}
			break;
		case PSPWRDWNSET2:
			dev->regs.pspwrdwnset2 = val;
				if (val != 0){
				dev->regs.pspwrdwnset2 = val;
			}
			break;
		case PSPWRDWNSET3:
			dev->regs.pspwrdwnset3 = val;
				if (val != 0){
				dev->regs.pspwrdwnset3 = val;
			}
			break;
		case PSPWRDWNCLR0:
			dev->regs.pspwrdwnclr0 = val;
			if (val != 0){
				dev->regs.pspwrdwnclr0 &= ~val;
				dev->regs.pspwrdwnset0 |= 0;
			}
			break;
		case PSPWRDWNCLR1:
			dev->regs.pspwrdwnclr1 = val;
			if (val != 0){
				dev->regs.pspwrdwnclr1 &= ~val;
				dev->regs.pspwrdwnset1 |= 0;
			}
			break;
		case PSPWRDWNCLR2:
			dev->regs.pspwrdwnclr2 = val;
			if (val != 0){
				dev->regs.pspwrdwnclr2 &= ~val;
				dev->regs.pspwrdwnset2 |= 0;
			}
			break;
		case PSPWRDWNCLR3:
			dev->regs.pspwrdwnclr3 = val;
			if (val != 0){
				dev->regs.pspwrdwnclr3 &= ~val;
				dev->regs.pspwrdwnset3 |= 0;
			}
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rm48x_pcr\n", offset);
			break;
	}
	return 0;
}
static char* rm48x_pcr_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t rm48x_pcr_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	rm48x_pcr_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t rm48x_pcr_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t rm48x_pcr_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	rm48x_pcr_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t rm48x_pcr_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_rm48x_pcr(const char *obj_name)
{
	rm48x_pcr_device* dev = skyeye_mm_zero(sizeof(rm48x_pcr_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	return dev->obj;
}
static exception_t free_rm48x_pcr(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_rm48x_pcr(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "rm48x_pcr",
		.class_desc = "rm48x_pcr",
		.new_instance = new_rm48x_pcr,
		.free_instance = free_rm48x_pcr
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = rm48x_pcr_read,
		.write = rm48x_pcr_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = rm48x_pcr_get_regvalue_by_id,
		.get_regname_by_id = rm48x_pcr_get_regname_by_id,
		.set_regvalue_by_id = rm48x_pcr_set_regvalue_by_id,
		.get_regnum = rm48x_pcr_get_regnum,
		.get_regid_by_name = rm48x_pcr_get_regid_by_name
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

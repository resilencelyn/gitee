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
#include "pic_p1010.h"

static exception_t pic_p1010_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	pic_p1010_device *dev = opaque->obj;

	switch (offset)
	{
		case BRR1:
			*(uint32_t *)buf = dev->regs.brr1;
			break;
		case BRR2:
			*(uint32_t *)buf = dev->regs.brr2;
			break;
		case IPIDR0:
			*(uint32_t *)buf = dev->regs.ipidr0;
			break;
		case IPIDR1:
			*(uint32_t *)buf = dev->regs.ipidr1;
			break;
		case IPIDR2:
			*(uint32_t *)buf = dev->regs.ipidr2;
			break;
		case IPIDR3:
			*(uint32_t *)buf = dev->regs.ipidr3;
			break;
		case CTPR:
			*(uint32_t *)buf = dev->regs.ctpr;
			break;
		case WHOAMI:
			*(uint32_t *)buf = dev->regs.whoami;
			break;
		case IACK:
			if ((dev->regs.iack & 0xffff) == 0) {
				*(uint32_t *)buf = dev->regs.iack;
				*(uint32_t *)buf |= dev->regs.svr & 0xffff;
			}else{
				*(uint32_t *)buf = dev->regs.iack;
			}

			break;
		case EOI:
			*(uint32_t *)buf = dev->regs.eoi;
			break;
		case FRR:
			*(uint32_t *)buf = dev->regs.frr;
			break;
		case GCR:
			*(uint32_t *)buf = dev->regs.gcr;
			break;
		case SVR:
			*(uint32_t *)buf = dev->regs.svr;
			break;
		default:
			if((offset >= 0x10200) &&(offset < 0x11600)) // PIC IIVPR && PIC IIDR
			{
				skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in pic_p1010\n", offset);
			}

			break;
	}

	//printf("%s, %d, offset = %x, value = %x\n", __func__, __LINE__, offset, *(uint32_t *)buf);
	return No_exp;
}

static exception_t pic_p1010_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	pic_p1010_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf("%s, %d, offset = %x, value = %x\n", __func__, __LINE__, offset, *(uint32_t *)buf);

	switch (offset)
	{
		case BRR1:
			dev->regs.brr1 = val;
			break;
		case BRR2:
			dev->regs.brr2 = val;
			break;
		case IPIDR0:
			dev->regs.ipidr0 = val;
			break;
		case IPIDR1:
			dev->regs.ipidr1 = val;
			break;
		case IPIDR2:
			dev->regs.ipidr2 = val;
			break;
		case IPIDR3:
			dev->regs.ipidr3 = val;
			break;
		case CTPR:
			dev->regs.ctpr = val;
			break;
		case WHOAMI:
			dev->regs.whoami = val;
			break;
		case IACK:
			dev->regs.iack = val;
			break;
		case EOI:
			dev->regs.eoi = val;
			if(val == 0){
				dev->regs.iack = dev->regs.svr;
			}
			break;
		case FRR:
			dev->regs.frr = val;
			break;
		case GCR:
			if (val & (1 << 31)) {
				dev->regs.gcr = val;
				dev->regs.gcr &= ~(1 << 31);
			}
			break;
		case SVR:
			dev->regs.svr = val;
			break;
		default:
			if((offset >= 0x10200) &&(offset < 0x11600)) // PIC IIVPR && PIC IIDR
			{
				skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in pic_p1010\n", offset);
			}
			break;
	}
	return No_exp;
}

static char* pic_p1010_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t pic_p1010_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	pic_p1010_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t pic_p1010_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t pic_p1010_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	pic_p1010_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t pic_p1010_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_pic_p1010(char *obj_name)
{
	pic_p1010_device* dev = skyeye_mm_zero(sizeof(pic_p1010_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	regs->brr1 = 0x00400301;
	regs->brr2 = 0x00000001;
	regs->ctpr = 0x0000000f;
	regs->svr = 0x0000ffff;
	return dev->obj;
}

static exception_t free_pic_p1010(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static int raise_signal(conf_object_t *obj, int line){
	pic_p1010_device *dev = obj->obj;

	dev->regs.iack = line;

	if (dev->signal_iface)
		dev->signal_iface->signal(dev->signal, NULL);
	return No_exp;
}

static int lower_signal(conf_object_t *obj, int line)
{
	pic_p1010_device *dev = obj->obj;

	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	pic_p1010_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	pic_p1010_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void init_pic_p1010(void)
{
	static skyeye_class_t class_data = {
		.class_name = "p1010_pic",
		.class_desc = "p1010_pic",
		.new_instance = new_pic_p1010,
		.free_instance = free_pic_p1010,
		.module_type = SKYML_ONCHIP(SC_p1010),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory = {
		.read = pic_p1010_read,
		.write = pic_p1010_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = pic_p1010_get_regvalue_by_id,
		.get_regname_by_id = pic_p1010_get_regname_by_id,
		.set_regvalue_by_id = pic_p1010_set_regvalue_by_id,
		.get_regnum = pic_p1010_get_regnum,
		.get_regid_by_name = pic_p1010_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const general_signal_iface_t signal_iface = {
		.raise_signal = raise_signal,
		.lower_signal = lower_signal
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_iface);

	static const struct InterfaceDescription ifaces[] = {
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
		(struct InterfaceDescription)
		{
			.name = GENERAL_SIGNAL_INTF_NAME,
			.iface = &signal_iface
		}
	};
	static const struct ConnectDescription connects[] = {
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		}
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

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
#include "msmc_c6678.h"

static exception_t msmc_c6678_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	msmc_c6678_device *dev = opaque->obj;
	switch (offset) {
	case PID:
		*(uint32_t *)buf = dev->regs.pid;
		break;
	case SMCERRAR:
		*(uint32_t *)buf = dev->regs.smcerrar;
		break;
	case SMCERRXR:
		*(uint32_t *)buf = dev->regs.smcerrxr;
		break;
	case SMEDCC:
		*(uint32_t *)buf = dev->regs.smedcc;
		break;
	case SMCEA:
		*(uint32_t *)buf = dev->regs.smcea;
		break;
	case SMSECC:
		*(uint32_t *)buf = dev->regs.smsecc;
		break;
	case SMPFAR:
		*(uint32_t *)buf = dev->regs.smpfar;
		break;
	case SMPFXR:
		*(uint32_t *)buf = dev->regs.smpfxr;
		break;
	case SMPFR:
		*(uint32_t *)buf = dev->regs.smpfr;
		break;
	case SMPFCR:
		*(uint32_t *)buf = dev->regs.smpfcr;
		break;
	case SBNDC0:
		*(uint32_t *)buf = dev->regs.sbndc0;
		break;
	case SBNDC1:
		*(uint32_t *)buf = dev->regs.sbndc1;
		break;
	case SBNDC2:
		*(uint32_t *)buf = dev->regs.sbndc2;
		break;
	case SBNDC3:
		*(uint32_t *)buf = dev->regs.sbndc3;
		break;
	case SBNDC4:
		*(uint32_t *)buf = dev->regs.sbndc4;
		break;
	case SBNDC5:
		*(uint32_t *)buf = dev->regs.sbndc5;
		break;
	case SBNDC6:
		*(uint32_t *)buf = dev->regs.sbndc6;
		break;
	case SBNDC7:
		*(uint32_t *)buf = dev->regs.sbndc7;
		break;
	case SBNDM:
		*(uint32_t *)buf = dev->regs.sbndm;
		break;
	case SBNDE:
		*(uint32_t *)buf = dev->regs.sbnde;
		break;
	case CFGLCK:
		*(uint32_t *)buf = dev->regs.cfglck;
		break;
	case CFGULCK:
		*(uint32_t *)buf = dev->regs.cfgulck;
		break;
	case CFGLCKSTAT:
		*(uint32_t *)buf = dev->regs.cfglckstat;
		break;
	case SMS_MPAX_LCK:
		*(uint32_t *)buf = dev->regs.sms_mpax_lck;
		break;
	case SMS_MPAX_ULCK:
		*(uint32_t *)buf = dev->regs.sms_mpax_ulck;
		break;
	case SMS_MPAX_LCKSTAT:
		*(uint32_t *)buf = dev->regs.sms_mpax_lckstat;
		break;
	case SES_MPAX_LCK:
		*(uint32_t *)buf = dev->regs.ses_mpax_lck;
		break;
	case SES_MPAX_ULCK:
		*(uint32_t *)buf = dev->regs.ses_mpax_ulck;
		break;
	case SES_MPAX_LCKSTAT:
		*(uint32_t *)buf = dev->regs.ses_mpax_lckstat;
		break;
	case SMESTAT:
		*(uint32_t *)buf = dev->regs.smestat;
		break;
	case SMIRSTAT:
		*(uint32_t *)buf = dev->regs.smirstat;
		break;
	case SMIRC:
		*(uint32_t *)buf = dev->regs.smirc;
		break;
	case SMIESTAT:
		*(uint32_t *)buf = dev->regs.smiestat;
		break;
	case SMIEC:
		*(uint32_t *)buf = dev->regs.smiec;
		break;
	case SMNCERRAR:
		*(uint32_t *)buf = dev->regs.smncerrar;
		break;
	case SMNCERRXR:
		*(uint32_t *)buf = dev->regs.smncerrxr;
		break;
	case SMNCEA:
		*(uint32_t *)buf = dev->regs.smncea;
		break;
	default: {
		if ((offset >= 0x200) && (offset < 0x600)) {
		} else if ((offset >= 0x600) && (offset < 0xA00)) {
		} else {
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in msmc_c6678\n", offset);
		}
		break;
	}
	}
	//printf("In %s  offset: 0x%X buf: 0x%x\n", __func__, offset, *(uint32_t *)buf);
	return 0;
}

static exception_t msmc_c6678_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	msmc_c6678_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf("In %s offset: 0x%X buf: 0x%x\n", __func__, offset, *(uint32_t *)buf);

	switch (offset) {
	case PID:
		dev->regs.pid = val;
		break;
	case SMCERRAR:
		dev->regs.smcerrar = val;
		break;
	case SMCERRXR:
		dev->regs.smcerrxr = val;
		break;
	case SMEDCC:
		dev->regs.smedcc = val;
		break;
	case SMCEA:
		dev->regs.smcea = val;
		break;
	case SMSECC:
		dev->regs.smsecc = val;
		break;
	case SMPFAR:
		dev->regs.smpfar = val;
		break;
	case SMPFXR:
		dev->regs.smpfxr = val;
		break;
	case SMPFR:
		dev->regs.smpfr = val;
		break;
	case SMPFCR:
		dev->regs.smpfcr = val;
		break;
	case SBNDC0:
		dev->regs.sbndc0 = val;
		break;
	case SBNDC1:
		dev->regs.sbndc1 = val;
		break;
	case SBNDC2:
		dev->regs.sbndc2 = val;
		break;
	case SBNDC3:
		dev->regs.sbndc3 = val;
		break;
	case SBNDC4:
		dev->regs.sbndc4 = val;
		break;
	case SBNDC5:
		dev->regs.sbndc5 = val;
		break;
	case SBNDC6:
		dev->regs.sbndc6 = val;
		break;
	case SBNDC7:
		dev->regs.sbndc7 = val;
		break;
	case SBNDM:
		dev->regs.sbndm = val;
		break;
	case SBNDE:
		dev->regs.sbnde = val;
		break;
	case CFGLCK:
		dev->regs.cfglck = val;
		break;
	case CFGULCK:
		dev->regs.cfgulck = val;
		break;
	case CFGLCKSTAT:
		dev->regs.cfglckstat = val;
		break;
	case SMS_MPAX_LCK:
		dev->regs.sms_mpax_lck = val;
		break;
	case SMS_MPAX_ULCK:
		dev->regs.sms_mpax_ulck = val;
		break;
	case SMS_MPAX_LCKSTAT:
		dev->regs.sms_mpax_lckstat = val;
		break;
	case SES_MPAX_LCK:
		dev->regs.ses_mpax_lck = val;
		break;
	case SES_MPAX_ULCK:
		dev->regs.ses_mpax_ulck = val;
		break;
	case SES_MPAX_LCKSTAT:
		dev->regs.ses_mpax_lckstat = val;
		break;
	case SMESTAT:
		dev->regs.smestat = val;
		break;
	case SMIRSTAT:
		dev->regs.smirstat = val;
		break;
	case SMIRC:
		dev->regs.smirc = val;
		break;
	case SMIESTAT:
		dev->regs.smiestat = val;
		break;
	case SMIEC:
		dev->regs.smiec = val;
		break;
	case SMNCERRAR:
		dev->regs.smncerrar = val;
		break;
	case SMNCERRXR:
		dev->regs.smncerrxr = val;
		break;
	case SMNCEA:
		dev->regs.smncea = val;
		break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in msmc_c6678\n", offset);
		break;
	}
	return 0;
}

static char* msmc_c6678_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t msmc_c6678_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	msmc_c6678_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t msmc_c6678_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t msmc_c6678_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	msmc_c6678_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;

	return *regs_value;
}

static uint32_t msmc_c6678_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if (strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_msmc_c6678(char *obj_name)
{
	msmc_c6678_device* dev = skyeye_mm_zero(sizeof(msmc_c6678_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	regs->smedcc |= 0x4000000;

	return dev->obj;
}

static exception_t free_msmc_c6678(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_msmc_c6678(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6678_msmc",
		.class_desc = "c6678_msmc",
		.new_instance = new_msmc_c6678,
		.free_instance = free_msmc_c6678
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = msmc_c6678_read,
		.write = msmc_c6678_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = msmc_c6678_get_regvalue_by_id,
		.get_regname_by_id = msmc_c6678_get_regname_by_id,
		.set_regvalue_by_id = msmc_c6678_set_regvalue_by_id,
		.get_regnum = msmc_c6678_get_regnum,
		.get_regid_by_name = msmc_c6678_get_regid_by_name
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

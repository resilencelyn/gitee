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
#include "rm48x_pmm.h"

static exception_t rm48x_pmm_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_pmm_device *dev = opaque->obj;
	if (offset >= 0x50 && offset <= 0x7f){
		*(uint32_t *)buf = dev->regs.reserved3[(offset - 0x50)/4];
		return 0;
	}
	else if (offset >= 0x8c && offset <= 0x9f){
		*(uint32_t *)buf = dev->regs.reserved4[(offset - 0x8c)/4];
		return 0;
	}
	switch (offset)
	{
		case LOGICPDPWRCTRL0:
			*(uint32_t *)buf = dev->regs.logicpdpwrctrl0;
			break;
		case RESERVED0:
			*(uint32_t *)buf = dev->regs.reserved0[0];
			break;
		case (RESERVED0 + 4):
			*(uint32_t *)buf = dev->regs.reserved0[1];
			break;
		case (RESERVED0 + 8):
			*(uint32_t *)buf = dev->regs.reserved0[2];
			break;
		case MEMPDPWRCTRL0:
			*(uint32_t *)buf = dev->regs.mempdpwrctrl0;
			break;
		case RESERVED1:
			*(uint32_t *)buf = dev->regs.reserved1[0];
			break;
		case (RESERVED1 + 4):
			*(uint32_t *)buf = dev->regs.reserved1[1];
			break;
		case (RESERVED1 + 8):
			*(uint32_t *)buf = dev->regs.reserved1[2];
			break;
		case PDCLKDISREG:
			*(uint32_t *)buf = dev->regs.pdclkdisreg;
			break;
		case PDCLKDISSETREG:
			*(uint32_t *)buf = dev->regs.pdclkdissetreg;
			break;
		case PDCLKDISCLRREG:
			*(uint32_t *)buf = dev->regs.pdclkdisclrreg;
			break;
		case RESERVED2:
			*(uint32_t *)buf = dev->regs.reserved2[0];
			break;
		case (RESERVED2 + 4):
			*(uint32_t *)buf = dev->regs.reserved2[1];
			break;
		case (RESERVED2 + 8):
			*(uint32_t *)buf = dev->regs.reserved2[2];
			break;
		case (RESERVED2 + 0xc):
			*(uint32_t *)buf = dev->regs.reserved2[3];
			break;
		case LOGICPDPWRSTAT0:
			*(uint32_t *)buf = dev->regs.logicpdpwrstat0;
			break;
		case LOGICPDPWRSTAT1:
			*(uint32_t *)buf = dev->regs.logicpdpwrstat1;
			break;
		case LOGICPDPWRSTAT2:
			*(uint32_t *)buf = dev->regs.logicpdpwrstat2;
			break;
		case LOGICPDPWRSTAT3:
			*(uint32_t *)buf = dev->regs.logicpdpwrstat3;
			break;
		case MEMPDPWRSTAT0:
			*(uint32_t *)buf = dev->regs.mempdpwrstat0;
			break;
		case MEMPDPWRSTAT1:
			*(uint32_t *)buf = dev->regs.mempdpwrstat1;
			break;
		case MEMPDPWRSTAT2:
			*(uint32_t *)buf = dev->regs.mempdpwrstat2;
			break;
		case GLOBALCTRL1:
			*(uint32_t *)buf = dev->regs.globalctrl1;
			break;
		case GLOBALSTAT:
			*(uint32_t *)buf = dev->regs.globalstat;
			break;
		case PRCKEYREG:
			*(uint32_t *)buf = dev->regs.prckeyreg;
			break;
		case LPDDCSTAT1:
			*(uint32_t *)buf = dev->regs.lpddcstat1;
			break;
		case LPDDCSTAT2:
			*(uint32_t *)buf = dev->regs.lpddcstat2;
			break;
		case MPDDCSTAT1:
			*(uint32_t *)buf = dev->regs.mpddcstat1;
			break;
		case MPDDCSTAT2:
			*(uint32_t *)buf = dev->regs.mpddcstat2;
			break;
		case ISODIAGSTAT:
			*(uint32_t *)buf = dev->regs.isodiagstat;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rm48x_pmm\n", offset);
			break;
	}

	//printf("%s  offset: 0x%x val: 0x%x\n", __func__, offset, *(uint32_t *)buf);
	return 0;
}

static exception_t rm48x_pmm_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_pmm_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

	//printf("%s offset: 0x%x val: 0x%x\n", __func__, offset, *(uint32_t *)buf);

	if (offset >= 0x50 && offset <= 0x7f){
		dev->regs.reserved3[(offset - 0x50)/4] = val;
		return 0;
	}
	else if (offset >= 0x8c && offset <= 0x9f){
		dev->regs.reserved4[(offset - 0x8c)/4] = val;
		return 0;
	}
	switch (offset)
	{
		case LOGICPDPWRCTRL0:
			dev->regs.logicpdpwrctrl0 = val;
			break;
		case RESERVED0:
			dev->regs.reserved0[0] = val;
			break;
		case (RESERVED0 + 4):
			dev->regs.reserved0[1] = val;
			break;
		case (RESERVED0 + 8):
			dev->regs.reserved0[2] = val;
			break;
		case MEMPDPWRCTRL0:
			dev->regs.mempdpwrctrl0 = val;
			break;
		case RESERVED1:
			dev->regs.reserved1[0] = val;
			break;
		case (RESERVED1 + 4):
			dev->regs.reserved1[1] = val;
			break;
		case (RESERVED1 + 8):
			dev->regs.reserved1[2] = val;
			break;
		case PDCLKDISREG:
			dev->regs.pdclkdisreg = val;
			break;
		case PDCLKDISSETREG:
			dev->regs.pdclkdissetreg = val;
			break;
		case PDCLKDISCLRREG:
			dev->regs.pdclkdisclrreg = val;
			break;
		case RESERVED2:
			dev->regs.reserved2[0] = val;
			break;
		case (RESERVED2 + 4):
			dev->regs.reserved2[1] = val;
			break;
		case (RESERVED2 + 8):
			dev->regs.reserved2[2] = val;
			break;
		case (RESERVED2 + 0xc):
			dev->regs.reserved2[3] = val;
			break;
		case LOGICPDPWRSTAT0:
			dev->regs.logicpdpwrstat0 = val;
			break;
		case LOGICPDPWRSTAT1:
			dev->regs.logicpdpwrstat1 = val;
			break;
		case LOGICPDPWRSTAT2:
			dev->regs.logicpdpwrstat2 = val;
			break;
		case LOGICPDPWRSTAT3:
			dev->regs.logicpdpwrstat3 = val;
			break;
		case MEMPDPWRSTAT0:
			dev->regs.mempdpwrstat0 = val;
			break;
		case MEMPDPWRSTAT1:
			dev->regs.mempdpwrstat1 = val;
			break;
		case MEMPDPWRSTAT2:
			dev->regs.mempdpwrstat2 = val;
			break;
		case GLOBALCTRL1:
			dev->regs.globalctrl1 = val;
			break;
		case GLOBALSTAT:
			dev->regs.globalstat = val;
			break;
		case PRCKEYREG:
		{
			dev->regs.prckeyreg = val;

			// Check MKEY (PRCKEYREG[3-0 bit])
			switch(dev->regs.prckeyreg & 0xF) {
			case 0x0:  // Lock Step mode
				break;
			case 0x6:  // Self-test mode
				dev->regs.lpddcstat1 |= 0xF;
				dev->regs.mpddcstat1 |= 0x7;
				break;
			case 0x9:  // Error Forcing mode
				break;
			case 0xf:  // Self-test Error Forcing Mode
				break;
			}
			break;
		}
		case LPDDCSTAT1:
			dev->regs.lpddcstat1 = val;
			if ((dev->regs.lpddcstat1 & 0xf0000) != 0){
				dev->regs.lpddcstat1 |= 0xfff0ffff;
			}
			break;
		case LPDDCSTAT2:
			dev->regs.lpddcstat2 = val;
			break;
		case MPDDCSTAT1:
			dev->regs.mpddcstat1 = val;
			if ((dev->regs.mpddcstat1 & 0x70000) != 0){
				dev->regs.mpddcstat1 |= 0xfff8ffff;
			}
			break;
		case MPDDCSTAT2:
			dev->regs.mpddcstat2 = val;
			break;
		case ISODIAGSTAT:
			dev->regs.isodiagstat = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rm48x_pmm\n", offset);
			break;
	}
	return 0;
}

static char* rm48x_pmm_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t rm48x_pmm_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	rm48x_pmm_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t rm48x_pmm_get_regnum(conf_object_t *conf_obj)
{
	return sizeof(struct registers) / 4;
}

static uint32_t rm48x_pmm_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	rm48x_pmm_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t rm48x_pmm_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if (strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_rm48x_pmm(const char *obj_name)
{
	rm48x_pmm_device* dev = skyeye_mm_zero(sizeof(rm48x_pmm_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->regs.globalstat = 0x1;
	return dev->obj;
}

static exception_t free_rm48x_pmm(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_rm48x_pmm(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "rm48x_pmm",
		.class_desc = "rm48x_pmm",
		.new_instance = new_rm48x_pmm,
		.free_instance = free_rm48x_pmm
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = rm48x_pmm_read,
		.write = rm48x_pmm_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = rm48x_pmm_get_regvalue_by_id,
		.get_regname_by_id = rm48x_pmm_get_regname_by_id,
		.set_regvalue_by_id = rm48x_pmm_set_regvalue_by_id,
		.get_regnum = rm48x_pmm_get_regnum,
		.get_regid_by_name = rm48x_pmm_get_regid_by_name
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

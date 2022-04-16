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
#include "ccm_imx6q.h"
static exception_t ccm_imx6q_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ccm_imx6q_device *dev = opaque->obj;
	switch (offset)
	{
		case CCM_CCR:
			*(uint32_t *)buf = dev->regs.ccm_ccr;
			break;
		case CCM_CCDR:
			*(uint32_t *)buf = dev->regs.ccm_ccdr;
			break;
		case CCM_CSR:
			*(uint32_t *)buf = dev->regs.ccm_csr;
			break;
		case CCM_CCSR:
			*(uint32_t *)buf = dev->regs.ccm_ccsr;
			break;
		case CCM_CACRR:
			*(uint32_t *)buf = dev->regs.ccm_cacrr;
			break;
		case CCM_CBCDR:
			*(uint32_t *)buf = dev->regs.ccm_cbcdr;
			break;
		case CCM_CBCMR:
			*(uint32_t *)buf = dev->regs.ccm_cbcmr;
			break;
		case CCM_CSCMR1:
			*(uint32_t *)buf = dev->regs.ccm_cscmr1;
			break;
		case CCM_CSCMR2:
			*(uint32_t *)buf = dev->regs.ccm_cscmr2;
			break;
		case CCM_CSCDR1:
			*(uint32_t *)buf = dev->regs.ccm_cscdr1;
			break;
		case CCM_CS1CDR:
			*(uint32_t *)buf = dev->regs.ccm_cs1cdr;
			break;
		case CCM_CS2CDR:
			*(uint32_t *)buf = dev->regs.ccm_cs2cdr;
			break;
		case CCM_CDCDR:
			*(uint32_t *)buf = dev->regs.ccm_cdcdr;
			break;
		case CCM_CHSCCDR:
			*(uint32_t *)buf = dev->regs.ccm_chsccdr;
			break;
		case CCM_CSCDR2:
			*(uint32_t *)buf = dev->regs.ccm_cscdr2;
			break;
		case CCM_CSCDR3:
			*(uint32_t *)buf = dev->regs.ccm_cscdr3;
			break;
		case CCM_CWDR:
			*(uint32_t *)buf = dev->regs.ccm_cwdr;
			break;
		case CCM_CDHIPR:
			*(uint32_t *)buf = dev->regs.ccm_cdhipr;
			break;
		case CCM_CLPCR:
			*(uint32_t *)buf = dev->regs.ccm_clpcr;
			break;
		case CCM_CISR:
			*(uint32_t *)buf = dev->regs.ccm_cisr;
			break;
		case CCM_CIMR:
			*(uint32_t *)buf = dev->regs.ccm_cimr;
			break;
		case CCM_CCOSR:
			*(uint32_t *)buf = dev->regs.ccm_ccosr;
			break;
		case CCM_CGPR:
			*(uint32_t *)buf = dev->regs.ccm_cgpr;
			break;
		case CCM_CCGR0:
			*(uint32_t *)buf = dev->regs.ccm_ccgr0;
			break;
		case CCM_CCGR1:
			*(uint32_t *)buf = dev->regs.ccm_ccgr1;
			break;
		case CCM_CCGR2:
			*(uint32_t *)buf = dev->regs.ccm_ccgr2;
			break;
		case CCM_CCGR3:
			*(uint32_t *)buf = dev->regs.ccm_ccgr3;
			break;
		case CCM_CCGR4:
			*(uint32_t *)buf = dev->regs.ccm_ccgr4;
			break;
		case CCM_CCGR5:
			*(uint32_t *)buf = dev->regs.ccm_ccgr5;
			break;
		case CCM_CCGR6:
			*(uint32_t *)buf = dev->regs.ccm_ccgr6;
			break;
		case CCM_CMEOR:
			*(uint32_t *)buf = dev->regs.ccm_cmeor;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in ccm_imx6q\n", offset);
			break;
	}

	return 0;
}
static exception_t ccm_imx6q_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ccm_imx6q_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case CCM_CCR:
			dev->regs.ccm_ccr = val;
			break;
		case CCM_CCDR:
			dev->regs.ccm_ccdr = val;
			break;
		case CCM_CSR:
			dev->regs.ccm_csr = val;
			break;
		case CCM_CCSR:
			dev->regs.ccm_ccsr = val;
			break;
		case CCM_CACRR:
			dev->regs.ccm_cacrr = val;
			break;
		case CCM_CBCDR:
			dev->regs.ccm_cbcdr = val;
			break;
		case CCM_CBCMR:
			dev->regs.ccm_cbcmr = val;
			break;
		case CCM_CSCMR1:
			dev->regs.ccm_cscmr1 = val;
			break;
		case CCM_CSCMR2:
			dev->regs.ccm_cscmr2 = val;
			break;
		case CCM_CSCDR1:
			dev->regs.ccm_cscdr1 = val;
			break;
		case CCM_CS1CDR:
			dev->regs.ccm_cs1cdr = val;
			break;
		case CCM_CS2CDR:
			dev->regs.ccm_cs2cdr = val;
			break;
		case CCM_CDCDR:
			dev->regs.ccm_cdcdr = val;
			break;
		case CCM_CHSCCDR:
			dev->regs.ccm_chsccdr = val;
			break;
		case CCM_CSCDR2:
			dev->regs.ccm_cscdr2 = val;
			break;
		case CCM_CSCDR3:
			dev->regs.ccm_cscdr3 = val;
			break;
		case CCM_CWDR:
			dev->regs.ccm_cwdr = val;
			break;
		case CCM_CDHIPR:
			dev->regs.ccm_cdhipr = val;
			break;
		case CCM_CLPCR:
			dev->regs.ccm_clpcr = val;
			break;
		case CCM_CISR:
			dev->regs.ccm_cisr = val;
			break;
		case CCM_CIMR:
			dev->regs.ccm_cimr = val;
			break;
		case CCM_CCOSR:
			dev->regs.ccm_ccosr = val;
			break;
		case CCM_CGPR:
			dev->regs.ccm_cgpr = val;
			break;
		case CCM_CCGR0:
			dev->regs.ccm_ccgr0 = val;
			break;
		case CCM_CCGR1:
			dev->regs.ccm_ccgr1 = val;
			break;
		case CCM_CCGR2:
			dev->regs.ccm_ccgr2 = val;
			break;
		case CCM_CCGR3:
			dev->regs.ccm_ccgr3 = val;
			break;
		case CCM_CCGR4:
			dev->regs.ccm_ccgr4 = val;
			break;
		case CCM_CCGR5:
			dev->regs.ccm_ccgr5 = val;
			break;
		case CCM_CCGR6:
			dev->regs.ccm_ccgr6 = val;
			break;
		case CCM_CMEOR:
			dev->regs.ccm_cmeor = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in ccm_imx6q\n", offset);
			break;
	}
	return 0;
}
static char* ccm_imx6q_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t ccm_imx6q_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	ccm_imx6q_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t ccm_imx6q_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t ccm_imx6q_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	ccm_imx6q_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t ccm_imx6q_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_ccm_imx6q(char *obj_name)
{
	ccm_imx6q_device* dev = skyeye_mm_zero(sizeof(ccm_imx6q_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	return dev->obj;
}
static exception_t free_ccm_imx6q(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t config_ccm_imx6q(conf_object_t* opaque)
{
	ccm_imx6q_device *dev = opaque->obj;
	dev->regs.ccm_cscmr1 = 0x1900000;
	dev->regs.ccm_cscdr1 = 0x490b00;
	return No_exp;
}

void init_ccm_imx6q(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "ccm_imx6q",
		.class_desc = "ccm_imx6q",
		.new_instance = new_ccm_imx6q,
		.free_instance = free_ccm_imx6q,
		.config_instance = config_ccm_imx6q,
		.module_type = SKYML_ONCHIP(SC_imx6),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = ccm_imx6q_read,
		.write = ccm_imx6q_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = ccm_imx6q_get_regvalue_by_id,
		.get_regname_by_id = ccm_imx6q_get_regname_by_id,
		.set_regvalue_by_id = ccm_imx6q_set_regvalue_by_id,
		.get_regnum = ccm_imx6q_get_regnum,
		.get_regid_by_name = ccm_imx6q_get_regid_by_name
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

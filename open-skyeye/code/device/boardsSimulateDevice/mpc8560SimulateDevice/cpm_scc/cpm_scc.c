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
#include "cpm_scc.h"
static exception_t cpm_scc_read(conf_object_t *opaque,
		generic_address_t offset, void *buf, size_t count)
{
	cpm_scc_device *dev = opaque->obj;
	switch (offset)
	{
		case GSMR_L:
			*(uint32_t *)buf = dev->regs.gsmr_l;
			break;
		case GSMR_H:
			*(uint32_t *)buf = dev->regs.gsmr_h;
			break;
		case PSMR:
			*(uint32_t *)buf = dev->regs.psmr;
			break;
		case TODR:
			*(uint32_t *)buf = dev->regs.todr;
			break;
		case DSR:
			*(uint32_t *)buf = dev->regs.dsr;
			break;
		case SCCE:
			*(uint32_t *)buf = dev->regs.scce;
			break;
		case SCCM:
			*(uint32_t *)buf = dev->regs.sccm;
			break;
		case SCCS:
			*(uint32_t *)buf = dev->regs.sccs;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at "
					"0x%x in cpm_scc\n", offset);
			break;
	}
	return 0;
}
static exception_t cpm_scc_write(conf_object_t *opaque,
		generic_address_t offset, void *buf, size_t count)
{
	cpm_scc_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case GSMR_L:
			dev->regs.gsmr_l = val;
			break;
		case GSMR_H:
			dev->regs.gsmr_h = val;
			break;
		case PSMR:
			dev->regs.psmr = val;
			break;
		case TODR:
			dev->regs.todr = val;
			break;
		case DSR:
			dev->regs.dsr = val;
			break;
		case SCCE:
			dev->regs.scce = val;
			break;
		case SCCM:
			dev->regs.sccm = val;
			break;
		case SCCS:
			dev->regs.sccs = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the "
					"register at 0x%x in cpm_scc\n", offset);
			break;
	}
	return 0;
}
static char* cpm_scc_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t cpm_scc_set_regvalue_by_id(conf_object_t *conf_obj,
		uint32_t value, uint32_t id)
{
	cpm_scc_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t cpm_scc_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t cpm_scc_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	cpm_scc_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t cpm_scc_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_cpm_scc(char *obj_name)
{
	cpm_scc_device* dev = skyeye_mm_zero(sizeof(cpm_scc_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	return dev->obj;
}
static exception_t free_cpm_scc(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_cpm_scc(void)
{
	static skyeye_class_t class_data = {
		.class_name = "cpm_scc",
		.class_desc = "cpm_scc",
		.new_instance = new_cpm_scc,
		.free_instance = free_cpm_scc,
		.module_type = SKYML_ONCHIP(SC_mpc8560),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory = {
		.read = cpm_scc_read,
		.write = cpm_scc_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = cpm_scc_get_regvalue_by_id,
		.get_regname_by_id = cpm_scc_get_regname_by_id,
		.set_regvalue_by_id = cpm_scc_set_regvalue_by_id,
		.get_regnum = cpm_scc_get_regnum,
		.get_regid_by_name = cpm_scc_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf
		},
	};
	static const struct ConnectDescription connects[] = {
		/* ConnectDescription */
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

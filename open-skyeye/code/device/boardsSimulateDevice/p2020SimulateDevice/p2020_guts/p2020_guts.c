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
#include "p2020_guts.h"

static exception_t guts_p2020_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	guts_p2020_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;

	switch (offset)
	{
		case PORPLLSR:
			*(uint32_t *)buf = regs->porpllsr;
			break;
		case PORBMSR:
			*(uint32_t *)buf = regs->porbmsr;
			break;
		case PORDEVSR:
			*(uint32_t *)buf = regs->pordevsr;
			break;
		case PORDBGMSR:
			*(uint32_t *)buf = regs->pordbgmsr;
			break;
		case PORDEVSR2:
			*(uint32_t *)buf = regs->pordevsr2;
			break;
		case GPPORCR:
			*(uint32_t *)buf = regs->gpporcr;
			break;
		case PMUXCR:
			*(uint32_t *)buf = regs->pmuxcr;
			break;
		case DEVDISR:
			*(uint32_t *)buf = regs->devdisr;
			break;
		case POWMGTCSR:
			*(uint32_t *)buf = regs->powmgtcsr;
			break;
		case PMCDR:
			*(uint32_t *)buf = regs->pmcdr;
			break;
		case MCPSUMR:
			*(uint32_t *)buf = regs->mcpsumr;
			break;
		case RSTRSCR:
			*(uint32_t *)buf = regs->rstrscr;
			break;
		case ECTRSTCR:
			*(uint32_t *)buf = regs->ectrstcr;
			break;
		case AUTORSTSR:
			*(uint32_t *)buf = regs->autorstsr;
			break;
		case PVR:
			*(uint32_t *)buf = regs->pvr;
			break;
		case SVR:
			*(uint32_t *)buf = regs->svr;
			break;
		case RSTCR:
			*(uint32_t *)buf = regs->rstcr;
			break;
		case IOVSELSR:
			*(uint32_t *)buf = regs->iovselsr;
			break;
		case DDRCLKDR:
			*(uint32_t *)buf = regs->ddrclkdr;
			break;
		case CLKOCR:
			*(uint32_t *)buf = regs->clkocr;
			break;
		case ECMCR:
			*(uint32_t *)buf = regs->ecmcr;
			break;
		case SDHCDCR:
			*(uint32_t *)buf = regs->sdhcdcr;
			break;
		case SRDSCR0:
			*(uint32_t *)buf = regs->srdscr0;
			break;
		case SRDSCR1:
			*(uint32_t *)buf = regs->srdscr1;
			break;
		case SRDSCR2:
			*(uint32_t *)buf = regs->srdscr2;
			break;
		case SRDSCR4:
			*(uint32_t *)buf = regs->srdscr4;
			break;
		case SRDSCR5:
			*(uint32_t *)buf = regs->srdscr5;
			break;
		case SRDSCR6:
			*(uint32_t *)buf = regs->srdscr6;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in p2020_guts\n", offset);
			break;
	}

	//printf("%s, %d, offset = %x, value = %x\n", __func__, __LINE__, offset, *(uint32_t *)buf);
	return No_exp;
}

static exception_t guts_p2020_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	guts_p2020_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	uint32_t val = *(uint32_t *)buf;

	switch (offset)
	{
		case PORPLLSR:
			regs->porpllsr = val;
			break;
		case PORBMSR:
			//00 CPU boot holdoff mode for both cores. The e500 cores are prevented from booting until configured by an external master.
			//01 e500 core 1 is allowed to boot without waiting for configuration by an external master, while e500 core 0 is prevented from booting until configured by an external master or the other core.
			//10 e500 core 0 is allowed to boot without waiting for configuration by an external master, while e500 core 1 is prevented from booting until configured by an external master or the other core.
			//11 Both e500 cores are allowed to boot without waiting for configuration by an external maste
			regs->porbmsr = val;
			break;
		case PORDEVSR:
			regs->pordevsr = val;
			//send_core_signal(dev->core0, regs->pordevsr & E500_CORE0_DISABLE, regs->pordevsr & TB0_DISABLE);
			//send_core_signal(dev->core1, regs->pordevsr & E500_CORE1_DISABLE, regs->pordevsr & TB1_DISABLE);
			break;
		case PORDBGMSR:
			//bit8 Enable machine check to core 0 in response to check stop from core 1
			//bit9 Enable machine check to core 1 in response to check stop from core 0
			//bit24 Machine check to core 1 in response to checkstop from core 0
			//bit25 Machine check to core 0 in response to checkstop from core 1
			//bit26 Core 1 watchdog timer machine check
			//bit29 Core 0 watchdog timer machine check
			break;
		case PORDEVSR2:
			regs->pordevsr2 = val;
			break;
		case GPPORCR:
			regs->gpporcr = val;
			break;
		case PMUXCR:
			regs->pmuxcr = val;
			break;
		case DEVDISR:
			regs->devdisr = val;
			break;
		case POWMGTCSR:
			regs->powmgtcsr = val;
			break;
		case PMCDR:
			regs->pmcdr = val;
			break;
		case MCPSUMR:
			regs->mcpsumr = val;
			break;
		case RSTRSCR:
			regs->rstrscr = val;
			break;
		case ECTRSTCR:
			regs->ectrstcr = val;
			break;
		case AUTORSTSR:
			regs->autorstsr = val;
			//bit16 Core 0 ready pin
			//bit17 Core 1 ready pin
			break;
		case PVR:
			regs->pvr = val;
			break;
		case SVR:
			regs->svr = val;
			break;
		case RSTCR:
			regs->rstcr = val;
			break;
		case IOVSELSR:
			regs->iovselsr = val;
			break;
		case DDRCLKDR:
			regs->ddrclkdr = val;
			break;
		case CLKOCR:
			regs->clkocr = val;
			break;
		case ECMCR:
			regs->ecmcr = val;
			break;
		case SDHCDCR:
			regs->sdhcdcr = val;
			break;
		case SRDSCR0:
			regs->srdscr0 = val;
			break;
		case SRDSCR1:
			regs->srdscr1 = val;
			break;
		case SRDSCR2:
			regs->srdscr2 = val;
			break;
		case SRDSCR4:
			regs->srdscr4 = val;
			break;
		case SRDSCR5:
			regs->srdscr5 = val;
			break;
		case SRDSCR6:
			regs->srdscr6 = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in p2020_guts\n", offset);
			break;
	}
	return No_exp;
}

static char* guts_p2020_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t guts_p2020_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	guts_p2020_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t guts_p2020_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t guts_p2020_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	guts_p2020_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t guts_p2020_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_guts_p2020(char *obj_name)
{
	guts_p2020_device* dev = skyeye_mm_zero(sizeof(guts_p2020_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	regs->porpllsr = 0x303060c;
	regs->porbmsr = 0x80070000;
	return dev->obj;
}

static exception_t free_guts_p2020(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static attr_value_t get_attr_svr(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	guts_p2020_device *dev =  conf_obj->obj;
	attr_value_t svr = SKY_make_attr_uinteger(dev->regs.svr);

	return svr;
}

static exception_t set_attr_svr(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	guts_p2020_device *dev =  conf_obj->obj;
	dev->regs.svr = SKY_attr_uinteger(*value);

	return No_exp;
}

static attr_value_t get_attr_pvr(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	guts_p2020_device *dev =  conf_obj->obj;
	attr_value_t pvr = SKY_make_attr_uinteger(dev->regs.pvr);

	return pvr;
}

static exception_t set_attr_pvr(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	guts_p2020_device *dev =  conf_obj->obj;
	dev->regs.pvr = SKY_attr_uinteger(*value);

	return No_exp;
}

static void guts_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "svr", get_attr_svr, NULL, set_attr_svr, NULL, SKY_Attr_Optional, "uinteger", "cpu board svr");
	SKY_register_attribute(clss, "pvr", get_attr_pvr, NULL, set_attr_pvr, NULL, SKY_Attr_Optional, "uinteger", "cpu board pvr");
}

static exception_t core_interface_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	guts_p2020_device *dev = obj->obj;
	assert(index < 2);
	dev->core_iface[index].obj = obj2;
	if (obj2 == NULL)
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface By core[%d] is NULL object.", CORE_EXEC_INTF_NAME, index);
	dev->core_iface[index].iface = (core_exec_intf *)SKY_get_iface(obj2, CORE_EXEC_INTF_NAME);
	if (dev->core_iface[index].iface == NULL)
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_EXEC_INTF_NAME, obj2->objname);
	return No_exp;
}

static exception_t core_interface_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	guts_p2020_device *dev = obj->obj;
	*obj2 = dev->core_iface[index].obj;
	*port = NULL;
	return No_exp;
}

void init_guts_p2020(void)
{
	static skyeye_class_t class_data = {
		.class_name = "p2020_guts",
		.class_desc = "p2020_guts",
		.new_instance = new_guts_p2020,
		.free_instance = free_guts_p2020,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory = {
		.read = guts_p2020_read,
		.write = guts_p2020_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = guts_p2020_get_regvalue_by_id,
		.get_regname_by_id = guts_p2020_get_regname_by_id,
		.set_regvalue_by_id = guts_p2020_set_regvalue_by_id,
		.get_regnum = guts_p2020_get_regnum,
		.get_regid_by_name = guts_p2020_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

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
	};
	static const struct ConnectDescription connects[] = {
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = CORE_EXEC_INTF_NAME,
			.set = core_interface_set,
			.get = core_interface_get
		},
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	guts_register_attribute(clss);
}

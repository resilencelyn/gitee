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
#include "ipic_mpc5121.h"
static exception_t ipic_mpc5121_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ipic_mpc5121_device *dev = opaque->obj;
	switch (offset)
	{
		case SICFR:
			*(uint32_t *)buf = dev->regs.sicfr;
			break;
		case SIVCR:
			*(uint32_t *)buf = dev->regs.sivcr;
			break;
		case SIPNR_H:
			*(uint32_t *)buf = dev->regs.sipnr_h;
			break;
		case SIPNR_L:
			*(uint32_t *)buf = dev->regs.sipnr_l;
			break;
		case SIPRR_A:
			*(uint32_t *)buf = dev->regs.siprr_a;
			break;
		case SIPRR_B:
			*(uint32_t *)buf = dev->regs.siprr_b;
			break;
		case SIPRR_C:
			*(uint32_t *)buf = dev->regs.siprr_c;
			break;
		case SIPRR_D:
			*(uint32_t *)buf = dev->regs.siprr_d;
			break;
		case SIMSR_H:
			*(uint32_t *)buf = dev->regs.simsr_h;
			break;
		case SIMSR_L:
			*(uint32_t *)buf = dev->regs.simsr_l;
			break;
		case SICNR:
			*(uint32_t *)buf = dev->regs.sicnr;
			break;
		case SEPNR:
			*(uint32_t *)buf = dev->regs.sepnr;
			break;
		case SMPRR_A:
			*(uint32_t *)buf = dev->regs.smprr_a;
			break;
		case SMPRR_B:
			*(uint32_t *)buf = dev->regs.smprr_b;
			break;
		case SEMSR:
			*(uint32_t *)buf = dev->regs.semsr;
			break;
		case SECNR:
			*(uint32_t *)buf = dev->regs.secnr;
			break;
		case SERSR:
			*(uint32_t *)buf = dev->regs.sersr;
			break;
		case SERMR:
			*(uint32_t *)buf = dev->regs.sermr;
			break;
		case SIFCR_H:
			*(uint32_t *)buf = dev->regs.sifcr_h;
			break;
		case SIFCR_L:
			*(uint32_t *)buf = dev->regs.sifcr_l;
			break;
		case SEFCR:
			*(uint32_t *)buf = dev->regs.sefcr;
			break;
		case SERFR:
			*(uint32_t *)buf = dev->regs.serfr;
			break;
		case SCVCR:
			*(uint32_t *)buf = dev->regs.scvcr;
			break;
		case SMVCR:
			*(uint32_t *)buf = dev->regs.smvcr;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in ipic_mpc5121\n", offset);
			break;
	}

	return No_exp;
}
static exception_t ipic_mpc5121_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ipic_mpc5121_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case SICFR:
			dev->regs.sicfr = val;
			break;
		case SIVCR:
			dev->regs.sivcr = val;
			break;
		case SIPNR_H:
			dev->regs.sipnr_h = val;
			break;
		case SIPNR_L:
			dev->regs.sipnr_l = val;
			break;
		case SIPRR_A:
			dev->regs.siprr_a = val;
			break;
		case SIPRR_B:
			dev->regs.siprr_b = val;
			break;
		case SIPRR_C:
			dev->regs.siprr_c = val;
			break;
		case SIPRR_D:
			dev->regs.siprr_d = val;
			break;
		case SIMSR_H:
			dev->regs.simsr_h = val;
			break;
		case SIMSR_L:
			dev->regs.simsr_l = val;
			break;
		case SICNR:
			dev->regs.sicnr = val;
			break;
		case SEPNR:
			dev->regs.sepnr = val;
			break;
		case SMPRR_A:
			dev->regs.smprr_a = val;
			break;
		case SMPRR_B:
			dev->regs.smprr_b = val;
			break;
		case SEMSR:
			dev->regs.semsr = val;
			break;
		case SECNR:
			dev->regs.secnr = val;
			break;
		case SERSR:
			dev->regs.sersr = val;
			break;
		case SERMR:
			dev->regs.sermr = val;
			break;
		case SIFCR_H:
			dev->regs.sifcr_h = val;
			break;
		case SIFCR_L:
			dev->regs.sifcr_l = val;
			break;
		case SEFCR:
			dev->regs.sefcr = val;
			break;
		case SERFR:
			dev->regs.serfr = val;
			break;
		case SCVCR:
			dev->regs.scvcr = val;
			break;
		case SMVCR:
			dev->regs.smvcr = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in ipic_mpc5121\n", offset);
			break;
	}
	return 0;
}
static char* ipic_mpc5121_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t ipic_mpc5121_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	ipic_mpc5121_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t ipic_mpc5121_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t ipic_mpc5121_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	ipic_mpc5121_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t ipic_mpc5121_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_ipic_mpc5121(char *obj_name)
{
	ipic_mpc5121_device* dev = skyeye_mm_zero(sizeof(ipic_mpc5121_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	return dev->obj;
}
static exception_t free_ipic_mpc5121(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

int if_simsr_enable(conf_object_t *obj, int line){
	uint32_t offset;
	ipic_mpc5121_device *dev = obj->obj;
	offset = simsr_offset_table[line];
	return line < 46 ? (dev->regs.simsr_h >> offset) & 1: (dev->regs.simsr_l >> offset ) & 1;
}
static int raise_signal(conf_object_t *obj, int line){
	ipic_mpc5121_device *dev = obj->obj;
	if(if_simsr_enable(obj, line)){
		dev->regs.sivcr = line;
		if (dev->signal_iface)
			dev->signal_iface->signal(dev->signal, line);
	}else{
		//interrupt mask is disableed
	}
	return No_exp;
}

static int lower_signal(conf_object_t *obj, int line)
{
	ipic_mpc5121_device *dev = obj->obj;

	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	ipic_mpc5121_device *dev = obj->obj;
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
	ipic_mpc5121_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
void init_ipic_mpc5121(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "mpc5121_ipic",
		.class_desc = "mpc5121_ipic",
		.new_instance = new_ipic_mpc5121,
		.free_instance = free_ipic_mpc5121,
		.module_type = SKYML_ONCHIP(SC_mpc5121),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = ipic_mpc5121_read,
		.write = ipic_mpc5121_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = ipic_mpc5121_get_regvalue_by_id,
		.get_regname_by_id = ipic_mpc5121_get_regname_by_id,
		.set_regvalue_by_id = ipic_mpc5121_set_regvalue_by_id,
		.get_regnum = ipic_mpc5121_get_regnum,
		.get_regid_by_name = ipic_mpc5121_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const general_signal_iface_t signal_iface = {
		.raise_signal = raise_signal,
		.lower_signal = lower_signal
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_iface);


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
		(struct InterfaceDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME, 
			.iface = &signal_iface,
		},

	};
	static const struct ConnectDescription connects[] = 
	{
	(struct ConnectDescription) {
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		}

		/* ConnectDescription */
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

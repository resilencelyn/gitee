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
#include "htey_GPS.h"
static exception_t htey_GPS_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_GPS_device *dev = opaque->obj;
	switch (offset)
	{
		case GPS_TX_RX:
			*(uint32_t *)buf = dev->regs.gps_tx_rx;
			break;
		case GPS_IER:
			*(uint32_t *)buf = dev->regs.gps_ier;
			break;
		case GPS_IFR:
			*(uint32_t *)buf = dev->regs.gps_ifr;
			break;
		case GPS_LCR:
			*(uint32_t *)buf = dev->regs.gps_lcr;
			break;

		case GPS_FCR:
			*(uint32_t *)buf = dev->regs.gps_fcr;
			break;
		case GPS_LSR:
			*(uint32_t *)buf = dev->regs.gps_lsr;
			break;
		case GPS_BDRH:
			*(uint32_t *)buf = dev->regs.gps_bdrh;
			break;
		case GPS_BDRL:
			*(uint32_t *)buf = dev->regs.gps_bdrl;
			break;
		case GPS_EN:
			*(uint32_t *)buf = dev->regs.gps_en;
			break;
		case GPS_CNT:
			*(uint32_t *)buf = dev->regs.gps_cnt;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in htey_GPS\n", offset);
			break;
	}
	return 0;
}
static exception_t htey_GPS_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_GPS_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case GPS_TX_RX:
			dev->regs.gps_tx_rx = val;
			break;
		case GPS_IER:
			dev->regs.gps_ier = val;
			break;
		case GPS_IFR:
			dev->regs.gps_ifr = val;
			break;
		case GPS_LCR:
			dev->regs.gps_lcr = val;
			break;
		case GPS_FCR:
			dev->regs.gps_fcr = val;
			break;
		case GPS_LSR:
			dev->regs.gps_lsr = val;
			break;
		case GPS_BDRH:
			dev->regs.gps_bdrh = val;
			break;
		case GPS_BDRL:
			dev->regs.gps_bdrl = val;
			break;
		case GPS_EN:
			dev->regs.gps_en = val;
			break;
		case GPS_CNT:
			dev->regs.gps_cnt = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in htey_GPS\n", offset);
			break;
	}
	return 0;
}
static char* htey_GPS_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t htey_GPS_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	htey_GPS_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t htey_GPS_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t htey_GPS_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	htey_GPS_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t htey_GPS_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_htey_GPS(char *obj_name)
{
	htey_GPS_device* dev = skyeye_mm_zero(sizeof(htey_GPS_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	return dev->obj;
}
static exception_t free_htey_GPS(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_htey_GPS(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "htey_GPS",
		.class_desc = "htey_GPS",
		.new_instance = new_htey_GPS,
		.free_instance = free_htey_GPS
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = htey_GPS_read,
		.write = htey_GPS_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = htey_GPS_get_regvalue_by_id,
		.get_regname_by_id = htey_GPS_get_regname_by_id,
		.set_regvalue_by_id = htey_GPS_set_regvalue_by_id,
		.get_regnum = htey_GPS_get_regnum,
		.get_regid_by_name = htey_GPS_get_regid_by_name
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

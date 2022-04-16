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
#include "dcfg_t2080.h"

static exception_t dcfg_t2080_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	dcfg_t2080_device *dev = opaque->obj;

    uint32_t *data = (uint32_t *)buf;
	switch (offset)
	{
		case PORSR1:
			*data = dev->regs.porsr1;
			break;
		case PVR:
			*data = dev->regs.pvr;
			break;
		case SVR:
			*data = dev->regs.svr;
			break;
		case RCWSR0:
			*data = dev->regs.rcwsr[0];
			break;
		case RCWSR1:
			*data = dev->regs.rcwsr[1];
			break;
		case RCWSR2:
			*data = dev->regs.rcwsr[2];
			break;
		case RCWSR3:
			*data = dev->regs.rcwsr[3];
			break;
		case RCWSR4:
			*data = dev->regs.rcwsr[4];
			break;
		case RCWSR5:
			*data = dev->regs.rcwsr[5];
			break;
		case RCWSR6:
			*data = dev->regs.rcwsr[6];
			break;
		case RCWSR7:
			*data = dev->regs.rcwsr[7];
			break;
		case RCWSR8:
			*data = dev->regs.rcwsr[8];
			break;
		case RCWSR9:
			*data = dev->regs.rcwsr[9];
			break;
		case RCWSR10:
			*data = dev->regs.rcwsr[10];
			break;
		case RCWSR11:
			*data = dev->regs.rcwsr[11];
			break;
		case RCWSR12:
			*data = dev->regs.rcwsr[12];
			break;
		case RCWSR13:
			*data = dev->regs.rcwsr[13];
			break;
		case RCWSR14:
			*data = dev->regs.rcwsr[14];
			break;
		case RCWSR15:
			*data = dev->regs.rcwsr[15];
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot read the register at 0x%x in dcfg_t2080\n", offset);
			break;
	}

	return 0;
}

static exception_t dcfg_t2080_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	dcfg_t2080_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in dcfg_t2080\n", offset);
			break;
	}
	return 0;
}

static char* dcfg_t2080_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t dcfg_t2080_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	dcfg_t2080_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t dcfg_t2080_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t dcfg_t2080_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	dcfg_t2080_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t dcfg_t2080_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static void reset_registers(struct registers *regs)
{
    regs->pvr = 0x80400120;    
    regs->svr = 0x85300000; //T2080 without security
}

static conf_object_t* new_dcfg_t2080(char *obj_name)
{
	dcfg_t2080_device* dev = skyeye_mm_zero(sizeof(dcfg_t2080_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

    reset_registers(regs);
	dev->regs.porsr1 = 0x9e7ffffb13;
	dev->regs.rcwsr[0] = 0x0c18000e;
	dev->regs.rcwsr[1] = 0x0;
	dev->regs.rcwsr[2] = 0x0;
	dev->regs.rcwsr[3] = 0x0;
	dev->regs.rcwsr[4] = 0x86000002;
	dev->regs.rcwsr[5] = 0x40000002;
	dev->regs.rcwsr[6] = 0xec027000;
	dev->regs.rcwsr[7] = 0x01000000;
	dev->regs.rcwsr[8] = 0x0;
	dev->regs.rcwsr[9] = 0x0;
	dev->regs.rcwsr[10] = 0x0;
	dev->regs.rcwsr[11] = 0x00030810;
	dev->regs.rcwsr[12] = 0x00;
	dev->regs.rcwsr[13] = 0x01fe580f;
	dev->regs.rcwsr[14] = 0x00;
	dev->regs.rcwsr[15] = 0x00;
	return dev->obj;
}

static exception_t free_dcfg_t2080(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static attr_value_t get_attr_svr(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	dcfg_t2080_device *dev =  conf_obj->obj;
	attr_value_t svr = SKY_make_attr_uinteger(dev->regs.svr);

	return svr;
}

static exception_t set_attr_svr(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	dcfg_t2080_device *dev =  conf_obj->obj;
	dev->regs.svr = SKY_attr_uinteger(*value);

	return No_exp;
}

static attr_value_t get_attr_pvr(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	dcfg_t2080_device *dev =  conf_obj->obj;
	attr_value_t pvr = SKY_make_attr_uinteger(dev->regs.pvr);

	return pvr;
}

static exception_t set_attr_pvr(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	dcfg_t2080_device *dev =  conf_obj->obj;
	dev->regs.pvr = SKY_attr_uinteger(*value);

	return No_exp;
}

static void dcfg_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "svr", get_attr_svr, NULL, set_attr_svr, NULL, SKY_Attr_Optional, "uinteger", "cpu board svr");
	SKY_register_attribute(clss, "pvr", get_attr_pvr, NULL, set_attr_pvr, NULL, SKY_Attr_Optional, "uinteger", "cpu board pvr");
}

void init_dcfg_t2080(void)
{
	static skyeye_class_t class_data = {
		.class_name = "t2080_dcfg",
		.class_desc = "t2080_dcfg",
		.new_instance = new_dcfg_t2080,
		.free_instance = free_dcfg_t2080,
		.module_type = SKYML_ONCHIP(SC_t2080),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory = {
		.read = dcfg_t2080_read,
		.write = dcfg_t2080_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = dcfg_t2080_get_regvalue_by_id,
		.get_regname_by_id = dcfg_t2080_get_regname_by_id,
		.set_regvalue_by_id = dcfg_t2080_set_regvalue_by_id,
		.get_regnum = dcfg_t2080_get_regnum,
		.get_regid_by_name = dcfg_t2080_get_regid_by_name
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
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	dcfg_register_attribute(clss);
}

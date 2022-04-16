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
#include "rm48x_pbist.h"

static exception_t rm48x_pbist_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_pbist_device *dev = opaque->obj;
	if (offset >= 0x0 && offset <=0x15c){
		uint32_t off = offset/4;
		*(uint32_t *)buf = dev->regs.reserve0[off];
		return 0;
	}
	else if(offset >= 0x168 && offset <=0x17c){
		uint32_t off = (offset - 0x168)/4;
		*(uint32_t *)buf = dev->regs.reserve1[off];
		return 0;
	}
	else{
		switch (offset)
		{
			case RAMT:
				*(uint32_t *)buf = dev->regs.ramt;
				break;
			case DLR:
				*(uint32_t *)buf = dev->regs.dlr;
				break;
			case PACT:
				*(uint32_t *)buf = dev->regs.pact;
				break;
			case PBISTID:
				*(uint32_t *)buf = dev->regs.pbistid;
				break;
			case OVER:
				*(uint32_t *)buf = dev->regs.over;
				break;
			case RESERVE2:
				*(uint32_t *)buf = dev->regs.reserve2;
				break;
			case FSRF0:
				*(uint32_t *)buf = dev->regs.fsrf0;
				break;
			case RESERVE5:
				*(uint32_t *)buf = dev->regs.reserve5;
				break;
			case FSRC0:
				*(uint32_t *)buf = dev->regs.fsrc0;
				break;
			case FSRC1:
				*(uint32_t *)buf = dev->regs.fsrc1;
				break;
			case FSRA0:
				*(uint32_t *)buf = dev->regs.fsra0;
				break;
			case FSRA1:
				*(uint32_t *)buf = dev->regs.fsra1;
				break;
			case FSRDL0:
				*(uint32_t *)buf = dev->regs.fsrdl0;
				break;
			case RESERVE3:
				*(uint32_t *)buf = dev->regs.reserve3;
				break;
			case FSRDL1:
				*(uint32_t *)buf = dev->regs.fsrdl1;
				break;
			case RESERVE4:
				*(uint32_t *)buf = dev->regs.reserve4[0];
				break;
			case (RESERVE4 + 4):
				*(uint32_t *)buf = dev->regs.reserve4[1];
				break;
			case (RESERVE4 + 8):
				*(uint32_t *)buf = dev->regs.reserve4[2];
				break;
			case ROM:
				*(uint32_t *)buf = dev->regs.rom;
				break;
			case ALGO:
				*(uint32_t *)buf = dev->regs.algo;
				break;
			case RINFOL:
				*(uint32_t *)buf = dev->regs.rinfol;
				break;
			case RINFOU:
				*(uint32_t *)buf = dev->regs.rinfou;
				break;
			default:
				skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rm48x_pbist\n", offset);
				break;
		}
	}
	return 0;
}

static exception_t rm48x_pbist_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_pbist_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf(" %s\t offset = 0x%x\t value = 0x%x\n",__FUNCTION__,offset,val);
	if (offset >= 0x0 && offset <=0x15c){
		uint32_t off = offset/4;
		dev->regs.reserve0[off] = val;
		//return 0;
	}
	else if(offset >= 0x168 && offset <=0x17c){
		uint32_t off = (offset - 0x168)/4;
		dev->regs.reserve1[off] = val;
		//return 0;
	}
	else {
		switch (offset)
		{
			case RAMT:
				dev->regs.ramt = val;
				break;
			case DLR:
				dev->regs.dlr = val;
				break;
			case PACT:
				dev->regs.pact = val;
				break;
			case PBISTID:
				dev->regs.pbistid = val;
				break;
			case OVER:
				dev->regs.over = val;
				break;
			case RESERVE2:
				dev->regs.reserve2 = val;
				break;
			case FSRF0:
				dev->regs.fsrf0 = val;
				break;
			case RESERVE5:
				dev->regs.reserve5 = val;
				break;
			case FSRC0:
				dev->regs.fsrc0 = val;
				break;
			case FSRC1:
				dev->regs.fsrc1 = val;
				break;
			case FSRA0:
				dev->regs.fsra0 = val;
				break;
			case FSRA1:
				dev->regs.fsra1 = val;
				break;
			case FSRDL0:
				dev->regs.fsrdl0 = val;
				break;
			case RESERVE3:
				dev->regs.reserve3 = val;
				break;
			case FSRDL1:
				dev->regs.fsrdl1 = val;
				break;
			case RESERVE4:
				dev->regs.reserve4[0] = val;
				break;
			case (RESERVE4 + 4):
				dev->regs.reserve4[1] = val;
				break;
			case (RESERVE4 + 8):
				dev->regs.reserve4[2] = val;
				break;
			case ROM:
				dev->regs.rom = val;
				break;
			case ALGO:
				dev->regs.algo = val;
				break;
			case RINFOL:
				dev->regs.rinfol = val;
				break;
			case RINFOU:
				dev->regs.rinfou = val;
				break;
			default:
				skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rm48x_pbist\n", offset);
				break;
		}
	}
	return 0;
}

static char* rm48x_pbist_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t rm48x_pbist_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	rm48x_pbist_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t rm48x_pbist_get_regnum(conf_object_t *conf_obj)
{
	//return sizeof(struct registers) / 4;
	return 0;
}

static uint32_t rm48x_pbist_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	rm48x_pbist_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t rm48x_pbist_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_rm48x_pbist(const char *obj_name)
{
	rm48x_pbist_device* dev = skyeye_mm_zero(sizeof(rm48x_pbist_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->regs.over = 0x1;
	dev->regs.fsrdl0 = dev->regs.fsrdl1 = 0xaaaaaaaa;
	dev->regs.rom = 0x3;
	dev->regs.algo = 0xffffffff;
	dev->regs.rinfol = 0xffffffff;
	dev->regs.rinfou = 0xffffffff;
	dev->regs.dlr = 0x4;

	return dev->obj;
}

static exception_t free_rm48x_pbist(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_rm48x_pbist(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "rm48x_pbist",
		.class_desc = "rm48x_pbist",
		.new_instance = new_rm48x_pbist,
		.free_instance = free_rm48x_pbist
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = rm48x_pbist_read,
		.write = rm48x_pbist_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = rm48x_pbist_get_regvalue_by_id,
		.get_regname_by_id = rm48x_pbist_get_regname_by_id,
		.set_regvalue_by_id = rm48x_pbist_set_regvalue_by_id,
		.get_regnum = rm48x_pbist_get_regnum,
		.get_regid_by_name = rm48x_pbist_get_regid_by_name
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

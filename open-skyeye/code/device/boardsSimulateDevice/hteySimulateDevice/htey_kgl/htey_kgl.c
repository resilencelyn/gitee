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
#include "htey_kgl.h"
#include <skyeye_system.h>
static exception_t htey_kgl_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_kgl_device *dev = opaque->obj;
	switch (offset)
	{
		case KG_1SWIDTH:
			*(uint32_t *)buf = dev->regs.kg_1swidth;
			break;
		case KG_TONGBU:
			*(uint32_t *)buf = dev->regs.kg_tongbu;
			break;
		case KG_PAOZHAO:
			*(uint32_t *)buf = dev->regs.kg_paozhao;
			break;
		case KG_FENLI:
			*(uint32_t *)buf = dev->regs.kg_fenli;
			break;
		case KG_TIANXIAN1:
			*(uint32_t *)buf = dev->regs.kg_tianxian1;
			break;
		case KG_TIANXIAN2:
			*(uint32_t *)buf = dev->regs.kg_tianxian2;
			break;
		case KG_ZIKONG:
			*(uint32_t *)buf = dev->regs.kg_zikong;
			break;
		case KG_GUIKONG:
			*(uint32_t *)buf = dev->regs.kg_guikong;
			break;
		case KG_ANQUAN:
			*(uint32_t *)buf = dev->regs.kg_anquan;
			break;
		case KG_RSTWIDTH:
			*(uint32_t *)buf = dev->regs.kg_rstwidth;
			break;
		case KG_TLWIDTH:
			*(uint32_t *)buf = dev->regs.kg_tlwidth;
			break;
		case KG_PZWIDTH:
			*(uint32_t *)buf = dev->regs.kg_pzwidth;
			break;
		case KG_FLWIDTH:
			*(uint32_t *)buf = dev->regs.kg_flwidth;
			break;
		case KG_PULSEINPUT:
			*(uint32_t *)buf = dev->regs.kg_pulseinput;
			break;
		case KG_INPUT:
			*(uint32_t *)buf = dev->regs.kg_input;
			break;
		case KG_ZIGUI:
			*(uint32_t *)buf = dev->regs.kg_zigui;
			break;
		case KG_RSTLOCK:
			*(uint32_t *)buf = dev->regs.kg_rstlock;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in htey_kgl\n", offset);
			break;
	}
	return 0;
}
static exception_t htey_kgl_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_kgl_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case KG_1SWIDTH:
			dev->regs.kg_1swidth = val;
			break;
		case KG_TONGBU:
			dev->regs.kg_tongbu = val;
			break;
		case KG_PAOZHAO:
			dev->regs.kg_paozhao = val;
			break;
		case KG_FENLI:
			dev->regs.kg_fenli = val;
			break;
		case KG_TIANXIAN1:
			dev->regs.kg_tianxian1 = val;
			break;
		case KG_TIANXIAN2:
			dev->regs.kg_tianxian2 = val;
			break;
		case KG_ZIKONG:
			dev->regs.kg_zikong = val;
			break;
		case KG_GUIKONG:
			dev->regs.kg_guikong = val;
			break;
		case KG_ANQUAN:
			dev->regs.kg_anquan = val;
			break;
		case KG_RSTWIDTH:
			dev->regs.kg_rstwidth = val;
			break;
		case KG_TLWIDTH:
			dev->regs.kg_tlwidth = val;
			break;
		case KG_PZWIDTH:
			dev->regs.kg_pzwidth = val;
			break;
		case KG_FLWIDTH:
			dev->regs.kg_flwidth = val;
			break;
		case KG_TB_TST:
			dev->regs.kg_tb_tst = val;
			break;
		case KG_PZ_TST:
			dev->regs.kg_pz_tst = val;
			break;
		case KG_FL_TST:
			dev->regs.kg_fl_tst = val;
			break;
		case KG_RSTLOCK:
			dev->regs.kg_rstlock = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in htey_kgl\n", offset);
			break;
	}
	return 0;
}
static char* htey_kgl_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t htey_kgl_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	htey_kgl_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t htey_kgl_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t htey_kgl_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	htey_kgl_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t htey_kgl_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

int hety_kgl_ctrl(conf_object_t *conf_obj, int cmd, int argc, int *argv){
	htey_kgl_device *dev = conf_obj->obj;
	switch(cmd){
		case TUOLUO:
			dev->regs.kg_pulseinput |= 0x2;
			break;
		default:
				return 0;
	}
	
	return 0;
}

static conf_object_t* new_htey_kgl(char *obj_name)
{
	htey_kgl_device* dev = skyeye_mm_zero(sizeof(htey_kgl_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);


	return dev->obj;
}
static exception_t free_htey_kgl(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_htey_kgl(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "htey_kgl",
		.class_desc = "htey_kgl",
		.new_instance = new_htey_kgl,
		.free_instance = free_htey_kgl
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = htey_kgl_read,
		.write = htey_kgl_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = htey_kgl_get_regvalue_by_id,
		.get_regname_by_id = htey_kgl_get_regname_by_id,
		.set_regvalue_by_id = htey_kgl_set_regvalue_by_id,
		.get_regnum = htey_kgl_get_regnum,
		.get_regid_by_name = htey_kgl_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
	static htey_kgl_intf kgl_intf =
	{
			.kgl_ctrl = hety_kgl_ctrl,
	};
	SKY_register_iface(clss, HTEY_KGL_INTF, &kgl_intf);
	
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
		(struct InterfaceDescription)
		{
			.name = HTEY_KGL_INTF,
			.iface = &kgl_intf
		},
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

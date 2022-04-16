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
#include "BMan_T2080.h"

static int register_read(conf_object_t *opaque, generic_address_t offset, uint32_t *value, size_t count)
{
	BMan_T2080_device *dev = opaque->obj;

	switch (count) {
	case 1:
	{
		uint8_t *this_regs = &(dev->sp_regs[offset]);
		uint8_t data;
		data = *this_regs;
		*value = data;
		break;
	}
	case 2:
	{
		uint16_t *this_regs = &(dev->sp_regs[offset]);
		uint16_t data;
		data = *this_regs;
		*value = data;
		break;
	}
	case 4:
	{
		uint32_t *this_regs = &(dev->sp_regs[offset]);
		*value = *this_regs;
		break;
	}
	default:
		printf("read error\n");
		return 0;
	}

	return 1;
}

static int register_write(conf_object_t *opaque, generic_address_t offset, uint32_t value, size_t count)
{
	BMan_T2080_device *dev = opaque->obj;

	switch (count) {
	case 1:
	{
		uint8_t *this_regs = &(dev->sp_regs[offset]);
		*this_regs = value & 0xFF;
		break;
	}
	case 2:
	{
		uint16_t *this_regs = &(dev->sp_regs[offset]);
		*this_regs = value & 0xFFFF;
		break;
	}
	case 4:
	{
		uint32_t *this_regs = &(dev->sp_regs[offset]);
		*this_regs = value;
		break;
	}
	default:
		printf("write error\n");
		return 0;
	}

	return 1;
}

static exception_t BMan_T2080_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	BMan_T2080_device *dev = opaque->obj;
	uint32_t val = dev->regs[offset];
	if(offset>=0x100 && offset<=0xF04)
		*(uint32_t *)buf = val;
	else{
		skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in BMan_T2080\n", offset);
	}
	return 0;
}
static exception_t BMan_T2080_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	BMan_T2080_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	if(offset>=0x100 && offset<=0xF04)
		dev->regs[offset] = val;
	
	else{
		skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in BMan_T2080\n", offset);
	}
	
	return 0;
}

static exception_t BMan_T2080_0xF8_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	BMan_T2080_device *dev = opaque->obj;
	uint32_t val;
	register_read(opaque, offset, &val, count);
	if (offset == 0x01000004){
		uint8_t CI = dev->sp_regs[0x1000004] & 0x7;
		if(CI == 0)
			register_write(opaque, offset, 7, 1);
		else {
			CI--;
			register_write(opaque, offset, CI, 1);
		}
		
	}
	else 
		*(uint32_t *)buf = val;
	 // printf("%s  offset: 0x%06x val: 0x%x\n", __func__, offset, *(uint32_t *)buf);
}
static exception_t BMan_T2080_0xF8_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	BMan_T2080_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

	register_write(opaque, offset, val, count);
}


static char* BMan_T2080_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t BMan_T2080_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	BMan_T2080_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t BMan_T2080_get_regnum(conf_object_t *conf_obj)
{

	return 0;
}
static uint32_t BMan_T2080_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	BMan_T2080_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t BMan_T2080_get_regid_by_name(conf_object_t *conf_obj, char *name)
{

	return 0;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	BMan_T2080_device *dev =  obj->obj;
	dev->signal= obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);
	if(dev->signal_iface == NULL){
		if (dev->signal == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from NULL object fail.");
		else
			skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from %s fail.", obj2->objname);
	}

	return No_exp;
}
 
static exception_t signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	BMan_T2080_device *dev =  obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}


static conf_object_t* new_BMan_T2080(const char *obj_name)
{
	BMan_T2080_device* dev = skyeye_mm_zero(sizeof(BMan_T2080_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->regs[0xbf8] = 0x0A020201;
	dev->regs[0xBFC] = 0x00000102;
	dev->regs[0x960] = 0x00000003;
	
	//Software Portal
	dev->sp_regs[0x003000] = 0x00000008;
	dev->sp_regs[0x003100] = 0x00008008;
	dev->sp_regs[0x1000000] = 0x00000008;
	dev->sp_regs[0x1000004] = 0x00008008;
	dev->sp_regs[0x1000200] = 0xFFFFFFFF;
	dev->sp_regs[0x1000204] = 0xFFFFFFFF;
	
	return dev->obj;
}
static exception_t free_BMan_T2080(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t config_BMan_T2080(conf_object_t* conf_obj)
{
	BMan_T2080_device *dev = conf_obj->obj;

	static memory_space_intf io_memory =
	{
		.read  = BMan_T2080_0xF8_read,
		.write = BMan_T2080_0xF8_write
	};

	if (dev->image != NULL) {
		exception_t ret;
		ret = SKY_add_map(dev->image->obj, conf_obj, 0xF8000000, 0x2000000, 0, &io_memory, 1, 1);
	} else {
		skyeye_log(Error_log, __FUNCTION__, "Cannot get dev->image in QMan_T2080\n");
	}

	return No_exp;
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	BMan_T2080_device *dev = obj->obj;
	dev->image = obj2;
	dev->image_space = (memory_space_intf *)SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);
	return No_exp;
}
 
static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	BMan_T2080_device *dev = obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}


void init_BMan_T2080(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "BMan_T2080",
		.class_desc = "BMan_T2080",
		.new_instance = new_BMan_T2080,
		.free_instance = free_BMan_T2080,
		.config_instance = config_BMan_T2080,
		.module_type = SKYML_ONCHIP(SC_t2080),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	
	static memory_space_intf io_memory =
	{
		.read = BMan_T2080_read,
		.write = BMan_T2080_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = BMan_T2080_get_regvalue_by_id,
		.get_regname_by_id = BMan_T2080_get_regname_by_id,
		.set_regvalue_by_id = BMan_T2080_set_regvalue_by_id,
		.get_regnum = BMan_T2080_get_regnum,
		.get_regid_by_name = BMan_T2080_get_regid_by_name
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
		/* Connect */
		(struct ConnectDescription ){
			.name = MEMORY_SPACE_INTF_NAME,
			.set = memory_set,
			.get = memory_get,
		},
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = signal_set,
			.get = signal_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

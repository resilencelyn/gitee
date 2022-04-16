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
#include "mpc8378_i2c.h"


#define SET_BIT(reg, mask) ((reg) |= (mask))
#define CLR_BIT(reg, mask) ((reg) &= ~(mask))
#define CHK_BIT(reg, mask) ((reg) & (mask))

static exception_t mpc8378_i2c_regs_init(mpc8378_i2c_device *dev)
{	
	dev->regs.adr = 0x00;
	dev->regs.fdr = 0x00;
	dev->regs.cr = 0x00;
	dev->regs.sr = 0x81;
	dev->regs.dr = 0x00;
	dev->regs.dfsrr = 0x10;

	dev->deviceAddress = 0xFFFFFFFF;
	dev->blockNumber   = 0xFFFFFFFF;
}

static exception_t mpc8378_i2c_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	mpc8378_i2c_device *dev = opaque->obj;
	switch (offset)
	{
		case ADR:
			*(uint8_t *)buf = dev->regs.adr;
			break;
		case FDR:
			*(uint8_t *)buf = dev->regs.fdr;
			break;
		case CR:
			*(uint8_t *)buf = dev->regs.cr;
			break;
		case SR:
			*(uint8_t *)buf = dev->regs.sr;
			break;
		case DR:

			if (dev->i2c_bus_iface != NULL){
				*(uint8_t*)buf = dev->i2c_bus_iface->read_data(dev->i2c_bus_obj);
				
			}
			if (dev->read_flag % 2 != 0) {
				*(uint8_t*)buf = dev->rxbuf[dev->rx_index++];
			} else {
				*(uint8_t*)buf = 0;
			}
			dev->read_flag++;
			break;
		case DFSRR:
			*(uint8_t *)buf = dev->regs.dfsrr;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc8378_i2c\n", offset);
			break;
	}
	//printf("%s  offset: 0x%03x val: 0x%x\n",__func__, offset, *(uint8_t *)buf);
	return 0;
}
static exception_t mpc8378_i2c_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	mpc8378_i2c_device *dev = opaque->obj;
	uint8_t val = *(uint8_t *)buf;

	//printf("%s offset: 0x%03x val: 0x%x\n",__func__, offset, *(uint8_t *)buf);
	switch (offset)
	{
		case ADR:
			dev->regs.adr = val;
			break;
		case FDR:
			dev->regs.fdr=val;
			break;
		case CR:
			dev->regs.cr=val;
			if (CHK_BIT(dev->regs.cr, CR_RSTA)){
				mpc8378_i2c_regs_init(dev);
				
			}
				
			break;
		case SR:
			
			dev->regs.sr = val;
			break;
		case DR:
			SET_BIT(dev->regs.sr, SR_MIF);
			SET_BIT(dev->regs.cr, CR_MSTA);
			SET_BIT(dev->regs.cr, CR_MTX);
			
			dev->regs.dr = val;

			if (dev->deviceAddress == 0xFFFFFFFF) {
				dev->deviceAddress = val >> 1;
			} else if (dev->blockNumber == 0xFFFFFFFF) {
				dev->blockNumber   = val;
			} else {
				printf("%c", val);
				SET_BIT(dev->regs.cr, CR_RSTA);
			}
			break;
		case DFSRR:
			dev->regs.dfsrr = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in mpc8378_i2c\n", offset);
			break;
		}
	return 0;
	
}
static char* mpc8378_i2c_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t mpc8378_i2c_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	mpc8378_i2c_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t mpc8378_i2c_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t mpc8378_i2c_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	mpc8378_i2c_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t mpc8378_i2c_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static exception_t i2c_bus_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	mpc8378_i2c_device* dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal,CORE_SIGNAL_INTF_NAME);
	
	return No_exp;
}

static exception_t i2c_bus_get(conf_object_t* obj,conf_object_t** obj2,const char **port,int index)
{
	mpc8378_i2c_device* dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	mpc8378_i2c_device* dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal,CORE_SIGNAL_INTF_NAME);

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

static exception_t core_signal_get(conf_object_t* obj,conf_object_t** obj2,const char **port,int index)
{
	mpc8378_i2c_device* dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static conf_object_t* new_mpc8378_i2c(const char *obj_name)
{
	mpc8378_i2c_device* dev = skyeye_mm_zero(sizeof(mpc8378_i2c_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	mpc8378_i2c_regs_init(dev);

	dev->rxbuf[0] = 'a';
	dev->rxbuf[1] = 'b';
	dev->rxbuf[2] = 'c';
	dev->rxbuf[3] = 'd';
	dev->rxbuf[4] = 'e';
	dev->rxbuf[5] = 'f';
	
	dev->rx_index = 0;
	return dev->obj;
}


static exception_t free_mpc8378_i2c(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_mpc8378_i2c(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "mpc8378_i2c",
		.class_desc = "mpc8378_i2c",
		.new_instance = new_mpc8378_i2c,
		.free_instance = free_mpc8378_i2c,
		.module_type = SKYML_ONCHIP(SC_mpc8378),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = mpc8378_i2c_read,
		.write = mpc8378_i2c_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = mpc8378_i2c_get_regvalue_by_id,
		.get_regname_by_id = mpc8378_i2c_get_regname_by_id,
		.set_regvalue_by_id = mpc8378_i2c_set_regvalue_by_id,
		.get_regnum = mpc8378_i2c_get_regnum,
		.get_regid_by_name = mpc8378_i2c_get_regid_by_name
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
		
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		}
		
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

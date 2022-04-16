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
#include <skyeye_system.h>
#include "mpc8378_pit.h"

#define DEFAULT_FREQ 200000000  //counter = 时间/(1/(主频/分频值))

static exception_t mpc8378_pit_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	mpc8378_pit_device *dev = opaque->obj;
	switch (offset)
	{
		case PIT_PTCNR:
			*(uint32_t *)buf = dev->regs.ptcnr;
			//printf("ptcnr buf = 0x%x\n",buf);
				
			break;
		case PIT_PTLDR:
			*(uint32_t *)buf = dev->regs.ptldr;
			//printf("ptldr buf = 0x%x\n",buf);
			break;
		case PIT_PTPSR:
			*(uint32_t *)buf = dev->regs.ptpsr;
			//printf("ptpsr buf = 0x%x\n",buf);
			break;
		case PIT_PTCTR:
			*(uint32_t *)buf = dev->regs.ptctr;
			//printf("ptctr buf = 0x%x\n",buf);
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc8378_pit\n", offset);
			break;
	}
	return 0;
}

static void timer_func(void *obj, time_data_t * data) {
	mpc8378_pit_device *dev = ((conf_object_t *)obj)->obj;
		dev->regs.ptevr  = 0x01;//PTEVR[PIF]置位
		if((dev->regs.ptevr == 0x01)&&((dev->regs.ptcnr & 0x01) == 0x01))
		{
			if(dev->signal){
				dev->signal_iface->raise_signal(dev->signal,IRQ_NUM);
			}
		}
		dev->scale = dev->regs.ptldr;//重新开始计数
	return 0;
}

static exception_t mpc8378_pit_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	mpc8378_pit_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case PIT_PTCNR:
			dev->regs.ptcnr = val;
			//printf("%s ptcnr 0x%x\n", __func__, dev->regs.ptcnr);
			if ((val&0x80)==0x80) {
				if (dev->handle != NULL) {
					system_enable_timer(dev->handle);
					printf("system_enable_timer\n");
				}
				else if(dev->handle == NULL){
					double period =((double)(dev->regs.ptldr)*((double)dev->regs.ptpsr/(double)DEFAULT_FREQ));//总次数*(1/(主频/分频值))
					uint32_t time_cnt = (uint32_t)(period * 1000000000);
					//printf("time_cnt: %d\n", time_cnt);
					dev->handle = system_register_timer_handler(dev->obj, time_cnt, (time_sched_t)timer_func, SCHED_NS|SCHED_NORMAL);
				}
			}
			else if ((val&0x80)==0x0)
			{
				if (dev->handle != NULL)
					system_disable_timer(dev->handle);
			}
			break;
		case PIT_PTLDR:
			dev->regs.ptldr = val;
			dev->scale = dev->regs.ptldr;
			//printf("%s ptldr %d\n", __func__, dev->regs.ptldr);
			//printf("%s scale %d\n", __func__, dev->scale);
			break;
		case PIT_PTPSR:
			dev->regs.ptpsr = val;
			//printf("%s ptpsr 0x%x\n", __func__, dev->regs.ptpsr);
			break;
		case PIT_PTEVR:
			dev->regs.ptevr = val;
			if(val & 0x01 == 0x01)
				dev->regs.ptevr = 0;
			//printf("%s ptevr 0x%x\n", __func__,  dev->regs.ptevr);
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in mpc8378_pit\n", offset);
			break;
	}
	return 0;
}
static char* mpc8378_pit_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t mpc8378_pit_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	mpc8378_pit_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t mpc8378_pit_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t mpc8378_pit_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	mpc8378_pit_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t mpc8378_pit_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_mpc8378_pit(const char *obj_name)
{
	mpc8378_pit_device* dev = skyeye_mm_zero(sizeof(mpc8378_pit_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	return dev->obj;
}
static exception_t free_mpc8378_pit(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	mpc8378_pit_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2,const char **port, int index)
{
        mpc8378_pit_device *dev = obj->obj;
        *obj2 = dev->signal;
        *port = NULL;

        return No_exp;
}


void init_mpc8378_pit(void)
{

	static skyeye_class_t class_data =
	{
		.class_name = "mpc8378_pit",
		.class_desc = "mpc8378_pit",
		.new_instance = new_mpc8378_pit,
		.free_instance = free_mpc8378_pit,
		.module_type = SKYML_ONCHIP(SC_mpc8378),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = mpc8378_pit_read,
		.write = mpc8378_pit_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = mpc8378_pit_get_regvalue_by_id,
		.get_regname_by_id = mpc8378_pit_get_regname_by_id,
		.set_regvalue_by_id = mpc8378_pit_set_regvalue_by_id,
		.get_regnum = mpc8378_pit_get_regnum,
		.get_regid_by_name = mpc8378_pit_get_regid_by_name
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
			.set = general_signal_set,
			.get = general_signal_get
		},
	};
	
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

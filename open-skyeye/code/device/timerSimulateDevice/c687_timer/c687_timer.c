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
#include "c687_timer.h"
static exception_t c687_timer_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	c687_timer_device *dev = opaque->obj;
	switch (offset)
	{
		case T_COUNT:
			*(uint32_t *)buf = dev->regs.t_count;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in c687_timer\n", offset);
			break; 
	}
	return 0;
}

static timer_func(void *obj, time_data_t * data) {
	c687_timer_device *dev = ((conf_object_t *)obj)->obj;
	if(dev->flag == 1)//设置初始值后开始计时
	{
		dev->ir_num = dev->ir_num -1;
		dev->regs.t_count = dev->ir_num;//计数值回读寄存器
		if(dev->ir_num == 0)
		{
			if(dev->signal){
						dev->signal_iface->raise_signal(dev->signal,dev->int_number);
					}
			dev->ir_num = dev->regs.ir;
		}
	}
	return 0;
}

static exception_t c687_timer_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	int times = 0;
	c687_timer_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case IR:
			dev->regs.ir = val;
			dev->ir_num = dev->regs.ir;//初始值
			dev->flag = 1;
			break;
		case MR:
			dev->regs.mr = val;
			dev->time_cnt = 0xF2 ;//每计数一次所需时间242ns =定时长度/定时器初值=倍率16*15.15us;
			if(val == 0xF0)//11110000，即16分频，工作方式0
			{
				if (dev->handle != NULL) {
					system_enable_timer(dev->handle);
				}
				else if(dev->handle == NULL){
					dev->handle = system_register_timer_handler(dev->obj, dev->time_cnt, (time_sched_t)timer_func, SCHED_NS|SCHED_NORMAL);//每242纳秒调用一次处理函数timer_func
				}
				
			}
			// else if(val = 0xF1)//方式1硬件测试有问题
			// {
				
			// }
			break;
		case T_PAUSE:
			dev->regs.t_pause = val;
			if (dev->regs.t_pause == 0x55aa)
			{
				dev->flag = 0;
				if (dev->handle != NULL)
					system_disable_timer(dev->handle);
			}
			else if(dev->regs.t_pause == 0xaa55)
			{
				dev->flag = 1;
				if (dev->handle != NULL)
					system_enable_timer(dev->handle);
			}
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in c687_timer\n", offset);
			break;
	}
	return 0;
}
static char* c687_timer_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t c687_timer_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	c687_timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t c687_timer_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t c687_timer_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	c687_timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t c687_timer_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
} 
static conf_object_t* new_c687_timer(const char *obj_name)
{
	c687_timer_device* dev = skyeye_mm_zero(sizeof(c687_timer_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->flag = 0;
	return dev->obj;
}
static exception_t free_c687_timer(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
        c687_timer_device *dev = obj->obj;
        dev->signal = obj2;
        dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);
        return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2,const char **port, int index)
{
        c687_timer_device *dev = obj->obj;
        *obj2 = dev->signal;
        *port = NULL;

        return No_exp;
}

// tx,rx,err interrupt number attribute
static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	c687_timer_device *dev =  conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	c687_timer_device *dev =  conf_obj->obj;
	dev->int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void class_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "tx interrupt number of timer");
	return;
 }

void init_c687_timer(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c687_timer",
		.class_desc = "c687_timer",
		.new_instance = new_c687_timer,
		.free_instance = free_c687_timer,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = c687_timer_read,
		.write = c687_timer_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = c687_timer_get_regvalue_by_id,
		.get_regname_by_id = c687_timer_get_regname_by_id,
		.set_regvalue_by_id = c687_timer_set_regvalue_by_id,
		.get_regnum = c687_timer_get_regnum,
		.get_regid_by_name = c687_timer_get_regid_by_name
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
			.get = core_signal_get
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	class_register_attribute(clss);
}

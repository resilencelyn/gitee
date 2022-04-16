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
#include "c687_epic.h"

#define SET_BIT(reg, mask) ((reg) |= (mask))
#define CLR_BIT(reg, mask) ((reg) &= ~(mask))
#define CHK_BIT(reg, mask) ((reg) & (mask))

uint32_t get_cpu_register(conf_object_t *cpu, char *name){
	if(!SKY_has_attribute(cpu, name)){
		attr_value_t attr = SKY_get_attribute(cpu, name);
		return SKY_attr_uinteger(attr);
	}else{
		skyeye_log(Error_log, __FUNCTION__, "%s not has attribute %s\n", cpu->objname, name);
		return  0;
	}
}

void set_cpu_register(conf_object_t *cpu, char *name, uint32_t value){
	attr_value_t attr;
	attr = SKY_make_attr_uinteger(value);
	if(!SKY_has_attribute(cpu, name)){
		SKY_set_attribute(cpu, name, &attr);
	}else{
		skyeye_log(Error_log, __FUNCTION__, "%s not has attribute %s\n", cpu->objname, name);
	}
}

static int32_t send_irq_signal(conf_object_t *opaque, uint32_t int_num)
{
	c687_epic_device *dev = opaque->obj;

	if (dev->irq_map[int_num] == M_INT_VECTOR_ERROR) {
		// 没有此中断号映射
		return -1;
	}

		//printf("int_mask: 0x%x irq_map: 0x%x\n", dev->regs.int_mask, dev->irq_map[int_num]);
	if (!CHK_BIT(dev->regs.int_mask, 1 << dev->irq_map[int_num])) {
		dev->regs.int_ack = int_num;
		if (dev->signal_obj)
		{
			dev->signal_iface->signal(dev->signal_obj, NULL);
			return 1;
		}
		if (dev->pic_mpc8641d_signal_obj)
		{
			dev->pic_mpc8641d_signal_iface->raise_signal(dev->pic_mpc8641d_signal_obj, dev->regs.int_ack);
			return 1;
		}
	}

	return 0;
}

static void time_callback(conf_object_t *opaque, time_data_t *root_data)
{
	c687_epic_device *dev = opaque->obj;

	//printf("time_callback\n");
	static uint32_t l_uin32_cunt = 0;
	l_uin32_cunt++;
	if (l_uin32_cunt == 30) {
		send_irq_signal(opaque, 51);
	}
}

static exception_t c687_epic_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	c687_epic_device *dev = opaque->obj;
	switch (offset)
	{
		case INT_MASK:
			*(uint32_t *)buf = dev->regs.int_mask;
			break;
		case INT_EOI:
			// write onlly
			//*(uint32_t *)buf = dev->regs.int_eoi;
			skyeye_log(Warning_log, __FUNCTION__, "INT_EOI register is write onlly at 0x%x in c687_epic\n", offset);
			break;
		case INT_ACK:
			*(uint32_t *)buf = dev->regs.int_ack;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in c687_epic\n", offset);
			break;
	}

	//printf("%s  offset: 0x%04x buf: 0x%x\n", __func__, offset, *(uint32_t *)buf);
	return 0;
}

static exception_t c687_epic_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	c687_epic_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

	//printf("%s offset: 0x%04x buf: 0x%x\n", __func__, offset, *(uint32_t *)buf);
	switch (offset)
	{
		case INT_MASK:
			dev->regs.int_mask = val;
			//printf("int_mask = 0x%x\n",val);
			break;
		case INT_EOI:
			// 退出中断服务子程序前发送EOI命令字20H
			dev->regs.int_eoi  = val;
			break;
		case INT_ACK:
			//dev->regs.int_ack  = val;
			skyeye_log(Warning_log, __FUNCTION__, "INT_ACK register is read onlly at 0x%x in c687_epic\n", offset);
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in c687_epic\n", offset);
			break;
	}
	return 0;
}

static char* c687_epic_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t c687_epic_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	c687_epic_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t c687_epic_get_regnum(conf_object_t *conf_obj)
{
	return 0;
}

static uint32_t c687_epic_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	c687_epic_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t c687_epic_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_c687_epic(const char *obj_name)
{
	c687_epic_device* dev = skyeye_mm_zero(sizeof(c687_epic_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	// 上电默认屏蔽
	regs->int_mask = 0xFF;

	// 默认中断映射表
	int l_uint32_i = 0;
	for (l_uint32_i = 0; l_uint32_i < M_INT_VECTOR_MAP_SIZE; l_uint32_i++) {
		dev->irq_map[l_uint32_i] = M_INT_VECTOR_ERROR;
	}
	dev->irq_map[M_INT_VECTOR_COM_DEBUG]   = 0;
	dev->irq_map[M_INT_VECTOR_COM_DISPLAY] = 1;
	dev->irq_map[M_INT_VECTOR_TIMER1]      = 2;
	dev->irq_map[M_INT_VECTOR_TIMER2]      = 3;

	return dev->obj;
}

static exception_t config_c687_epic(conf_object_t *opaque)
{
	c687_epic_device *dev = (c687_epic_device *)opaque->obj;

	// 发送中断测试
	//system_register_timer_handler(dev->obj, 50, (time_sched_t)time_callback, SCHED_MS | SCHED_NORMAL);

}

static exception_t free_c687_epic(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

int raise_signal(conf_object_t *obj, int int_irq_num)
{
	c687_epic_device *dev = obj->obj;
	send_irq_signal(obj, int_irq_num);
	return 0;
}

int lower_signal(conf_object_t *obj, int line)
{
	c687_epic_device *dev = obj->obj;
	return 0;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	c687_epic_device *dev =  obj->obj;
	dev->signal_obj  = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal_obj, CORE_SIGNAL_INTF_NAME);

	if(dev->signal_iface == NULL){
		if (dev->signal_obj == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Get 'core_signal_intf' interface from NULL object fail.");
		else
			skyeye_log(Error_log, __FUNCTION__, "Get 'core_signal_intf' interface from %s fail.", obj2->objname);
	}

	return No_exp;
}

static exception_t signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	c687_epic_device *dev =  obj->obj;
	*obj2 = dev->signal_obj;
	*port = NULL;
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	c687_epic_device *dev = obj->obj;
	dev->pic_mpc8641d_signal_obj = obj2;
	dev->pic_mpc8641d_signal_iface = (general_signal_intf *)SKY_get_iface(dev->pic_mpc8641d_signal_obj, GENERAL_SIGNAL_INTF_NAME);

	if (dev->pic_mpc8641d_signal_iface == NULL)
	{
		if (dev->pic_mpc8641d_signal_obj == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from NULL object fail.");
		else
			skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from %s fail.", obj2->objname);
	}

	return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	c687_epic_device *dev = obj->obj;
	*obj2 = dev->pic_mpc8641d_signal_obj;
	*port = NULL;
	return No_exp;
}

static void class_register_attribute(conf_class_t* clss)
{
	return;
}

void init_c687_epic(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c687_epic",
		.class_desc = "c687_epic",
		.new_instance = new_c687_epic,
		.config_instance = config_c687_epic,
		.free_instance = free_c687_epic,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = c687_epic_read,
		.write = c687_epic_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = c687_epic_get_regvalue_by_id,
		.get_regname_by_id = c687_epic_get_regname_by_id,
		.set_regvalue_by_id = c687_epic_set_regvalue_by_id,
		.get_regnum = c687_epic_get_regnum,
		.get_regid_by_name = c687_epic_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static general_signal_intf signal_intf = {
		.raise_signal = raise_signal,
		.lower_signal = lower_signal,
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_intf);

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
			.name = GENERAL_SIGNAL_INTF_NAME,
			.iface = &signal_intf
		},
	};
	static const struct ConnectDescription connects[] =
	{
		// 临时添加, 使用mpc8641d_pic触发中断
		(struct ConnectDescription )
		{
			.name = CORE_SIGNAL_INTF_NAME,
			.set = signal_set,
			.get = signal_get,
		},
		(struct ConnectDescription){
				.name = GENERAL_SIGNAL_INTF_NAME,
				.set = general_signal_set,
				.get = general_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	class_register_attribute(clss);
}

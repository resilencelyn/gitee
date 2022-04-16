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
#include <skyeye_system.h>
#define DEBUG
#include <skyeye_log.h>
#include "gpt_imx6q.h"
static exception_t gpt_imx6q_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	gpt_imx6q_device *dev = opaque->obj;
	switch (offset)
	{
		case GPT_CR:
			*(uint32_t *)buf = dev->regs.gpt_cr;
			break;
		case GPT_PR:
			*(uint32_t *)buf = dev->regs.gpt_pr;
			break;
		case GPT_SR:
			*(uint32_t *)buf = dev->regs.gpt_sr;
			break;
		case GPT_IR:
			*(uint32_t *)buf = dev->regs.gpt_ir;
			break;
		case GPT_OCR1:
			*(uint32_t *)buf = dev->regs.gpt_ocr1;
			break;
		case GPT_OCR2:
			*(uint32_t *)buf = dev->regs.gpt_ocr2;
			break;
		case GPT_OCR3:
			*(uint32_t *)buf = dev->regs.gpt_ocr3;
			break;
		case GPT_ICR1:
			*(uint32_t *)buf = dev->regs.gpt_icr1;
			break;
		case GPT_ICR2:
			*(uint32_t *)buf = dev->regs.gpt_icr2;
			break;
		case GPT_CNT:
			*(uint32_t *)buf = dev->regs.gpt_cnt;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in gpt_imx6q\n", offset);
			break;
	}
	//printf("%s, %d, offset:%x, value = %x\n", __func__, __LINE__, offset, *(uint32_t *)buf);
	return 0;
}

void timer_cnt_thread(void *obj, time_data_t *data)
{
	gpt_imx6q_device *dev = ((conf_object_t *)obj)->obj;
	dev->regs.gpt_cnt++;
	if(dev->regs.gpt_cnt == 0xFFFFFFFF)
		dev->regs.gpt_cnt = 0;

	if (dev->signal_iface != NULL) {
		// int numver : 87
		interrupt_signal_t interrupt_signal;
		interrupt_signal.arm_signal.firq = Prev_level;
		interrupt_signal.arm_signal.irq = Low_level;
		interrupt_signal.arm_signal.reset = Prev_level;
		dev->signal_iface->signal(dev->signal, &interrupt_signal);
	}
	if (dev->general_signal) {
		general_signal_intf* general_signal = dev->general_signal;
		general_signal->event_signal(dev->general_core, 87, 0);
	}
}

static exception_t gpt_imx6q_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	gpt_imx6q_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf("%s, %d, offset:%x, value = %x\n", __func__, __LINE__, offset, *(uint32_t *)buf);
	switch (offset)
	{
		case GPT_CR:
			dev->regs.gpt_cr = val;
			if(dev->regs.gpt_cr & 0x1){
				if(dev->cnt_timer >= 0)
					system_disable_timer(dev->cnt_timer);
				dev->cnt_timer = system_register_timer_handler(opaque, 10000, timer_cnt_thread, SCHED_NORMAL|SCHED_NS);
			}else{
				if(dev->cnt_timer >=0)
					system_disable_timer(dev->cnt_timer);
			}
			break;
		case GPT_PR:
			dev->regs.gpt_pr = val;
			break;
		case GPT_SR:
			dev->regs.gpt_sr = val;
			break;
		case GPT_IR:
			dev->regs.gpt_ir = val;
			break;
		case GPT_OCR1:
			dev->regs.gpt_ocr1 = val;
			break;
		case GPT_OCR2:
			dev->regs.gpt_ocr2 = val;
			break;
		case GPT_OCR3:
			dev->regs.gpt_ocr3 = val;
			break;
		case GPT_ICR1:
			dev->regs.gpt_icr1 = val;
			break;
		case GPT_ICR2:
			dev->regs.gpt_icr2 = val;
			break;
		case GPT_CNT:
			dev->regs.gpt_cnt = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in gpt_imx6q\n", offset);
			break;
	}
	return 0;
}
static char* gpt_imx6q_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t gpt_imx6q_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	gpt_imx6q_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t gpt_imx6q_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t gpt_imx6q_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	gpt_imx6q_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t gpt_imx6q_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_gpt_imx6q(char *obj_name)
{
	gpt_imx6q_device* dev = skyeye_mm_zero(sizeof(gpt_imx6q_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	return dev->obj;
}
static exception_t free_gpt_imx6q(conf_object_t* opaque)
{
	gpt_imx6q_device *dev = opaque->obj;
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t config_gpt_imx6q(conf_object_t* opaque)
{
	gpt_imx6q_device *dev = opaque->obj;
	dev->cnt_timer = -1;
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	gpt_imx6q_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf **)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);
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
	gpt_imx6q_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    gpt_imx6q_device *dev = obj->obj;
    dev->general_core = obj2;
    dev->general_signal = (general_signal_intf *)SKY_get_iface(dev->general_core, GENERAL_SIGNAL_INTF_NAME);
	if (dev->general_signal == NULL){
		if (dev->general_core == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
    return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    gpt_imx6q_device *dev = obj->obj;
    *obj2 = dev->general_core;
    *port = NULL;
    return No_exp;
}

void init_gpt_imx6q(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "gpt_imx6q",
		.class_desc = "gpt_imx6q",
		.new_instance = new_gpt_imx6q,
		.free_instance = free_gpt_imx6q,
		.config_instance = config_gpt_imx6q,
		.module_type = SKYML_ONCHIP(SC_imx6),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = gpt_imx6q_read,
		.write = gpt_imx6q_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = gpt_imx6q_get_regvalue_by_id,
		.get_regname_by_id = gpt_imx6q_get_regname_by_id,
		.set_regvalue_by_id = gpt_imx6q_set_regvalue_by_id,
		.get_regnum = gpt_imx6q_get_regnum,
		.get_regid_by_name = gpt_imx6q_get_regid_by_name
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
		(struct ConnectDescription) {
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

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
#include "ictl_fifo.h"
#include "sopc_ictl.h"

static exception_t sopc_ictl_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_ictl_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;

	switch (offset)
	{
		case INT_ENB:
			*(uint32_t *)buf = regs->int_enb;
			break;
		case INT_ENB_EX:
			*(uint32_t *)buf = regs->int_enb_ex;
			break;
		case INT_STA:
			*(uint32_t *)buf = regs->int_sta;
			break;
		case INT_STA_EX:
			*(uint32_t *)buf = regs->int_sta_ex;
			break;
		case INT_MSK:
			*(uint32_t *)buf = regs->int_msk;
			break;
		case INT_MSK_EX:
			*(uint32_t *)buf = regs->int_msk_ex;
			break;
		case INT_PR:
			*(uint32_t *)buf = regs->int_pr;
			break;
		case INT_PR_EX:
			*(uint32_t *)buf = regs->int_pr_ex;
			break;
		case INT_POL:
			*(uint32_t *)buf = regs->int_pol;
			break;
		case INT_EDG:
			*(uint32_t *)buf = regs->int_edg;
			break;
		case IRQ_VEC:
			*(uint32_t *)buf = regs->irq_vec;
			/*
			if (regs->irq_vec == 0x20)
				printf("read irq cnt1 = %d\n", cnt1++);
			if (regs->irq_vec == 0x24)
				printf("read irq cnt2 = %d\n", cnt2++);
				*/
			break;
		case FIQ_VEC:
			*(uint32_t *)buf = regs->fiq_vec;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sopc_ictl\n", offset);
			return Not_found_exp;
	}
	return No_exp;
}

static exception_t sopc_ictl_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_ictl_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	uint32_t val = *(uint32_t *)buf;

	switch (offset)
	{
		case INT_ENB:
			regs->int_enb = val;
			break;
		case INT_ENB_EX:
			regs->int_enb_ex = val;
			break;
		case INT_MSK:
			regs->int_msk = val;
			break;
		case INT_MSK_EX:
			regs->int_msk_ex = val;
			break;
		case INT_PR:
			regs->int_pr = val;
			break;
		case INT_PR_EX:
			regs->int_pr_ex = val;
			break;
		case INT_POL:
			regs->int_pol = val;
			break;
		case INT_EDG:
			regs->int_edg = val;
			break;
		case INTS_STA:
			regs->ints_sta = val;
			regs->int_sta |= val;
			break;
		case INTS_STA_EX:
			regs->ints_sta_ex = val;
			regs->int_sta_ex |= val;
			break;
		case INTR_STA:
			regs->intr_sta = val;
			regs->int_sta &= ~val;
			break;
		case INTR_STA_EX:
			regs->intr_sta_ex = val;
			regs->int_sta_ex &= ~val;
			break;
		case IRQ_VEC:
			//regs->irq_vec = val;
			break;
		case FIQ_VEC:
			//regs->fiq_vec = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sopc_ictl\n", offset);
			return Not_found_exp;
	}

	return No_exp;
}

static void raise_irq(sopc_ictl_device *dev)
{
	interrupt_signal_t interrupt_signal;

	interrupt_signal.arm_signal.firq = Prev_level;
	interrupt_signal.arm_signal.irq = Low_level;
	interrupt_signal.arm_signal.reset = Prev_level;

	if (dev->signal.intf)
		dev->signal.intf->signal(dev->signal.obj, &interrupt_signal);
}

static void raise_fiq(sopc_ictl_device *dev)
{
	interrupt_signal_t interrupt_signal;

	interrupt_signal.arm_signal.firq = Low_level;
	interrupt_signal.arm_signal.irq = Prev_level;
	interrupt_signal.arm_signal.reset = Prev_level;

	if (dev->signal.intf)
		dev->signal.intf->signal(dev->signal.obj, &interrupt_signal);
}

void send_ictl_irq(conf_object_t *obj)
{
	sopc_ictl_device *dev = obj->obj;
	struct registers *regs = &(dev->regs);
	struct intc_type_t intc_t;

	if (CheckIntReadNum(dev->ictl_fifo) == 0)
		return;

	ReadFIFO(dev->ictl_fifo, &intc_t, 1);
	if (intc_t.isIrq)
	{
		regs->irq_vec = (intc_t.irq_num << 2);
		raise_irq(dev);
	} else
	{
		regs->fiq_vec = (intc_t.irq_num << 2);
		raise_fiq(dev);
	}

	return;
}

static int raise_signal(conf_object_t *obj, int int_num)
{
	sopc_ictl_device *dev = obj->obj;
	struct registers *regs = &(dev->regs);
	struct intc_type_t intc_t;

	if (int_num < 32)
	{
		regs->int_sta |= (1 << int_num);
		if ((regs->int_enb & (1 << int_num))
				&& (regs->int_msk & (1 << int_num)) == 0)
		{
			if (regs->int_pr >> int_num)
			{
				/* FIQ Interrupt Mode */
				intc_t.irq_num = int_num;
				intc_t.isIrq = 0;
			} else
			{
				/* IRQ Interrupt Mode */
				intc_t.irq_num = int_num;
				intc_t.isIrq = 1;
			}
			WriteFIFO(dev->ictl_fifo, &intc_t, 1);
		}
	} else
	{
		int_num -= 32;
		regs->int_sta_ex |= (1 << int_num);
		if ((regs->int_enb_ex & (1 << int_num))
				&& (regs->int_msk_ex & (1 << int_num)) == 0)
		{
			if (regs->int_pr_ex >> int_num)
			{
				/* FIQ Interrupt Mode */
				intc_t.irq_num = int_num;
				intc_t.isIrq = 0;
			} else
			{
				/* IRQ Interrupt Mode */
				intc_t.irq_num = int_num;
				intc_t.isIrq = 1;
			}
			WriteFIFO(dev->ictl_fifo, &intc_t, 1);
		}
	}
	return No_exp;
}

static int lower_signal(conf_object_t *obj, int line)
{
	sopc_ictl_device *dev = obj->obj;

	return No_exp;
}

static char* sopc_ictl_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t sopc_ictl_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	sopc_ictl_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t sopc_ictl_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t sopc_ictl_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	sopc_ictl_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t sopc_ictl_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t sopc_ictl_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static void init_ictl_register(sopc_ictl_device *dev)
{
	struct registers *regs = &(dev->regs);
	regs->int_msk = 0xffffffff;
	regs->int_msk_ex = 0xff;
	regs->int_pol = 0x3ff;
	regs->int_edg = 0x3ff;
	regs->irq_vec = 0xffff00;
	regs->fiq_vec = 0xfffe00;
}

static conf_object_t* new_sopc_ictl(const char *obj_name)
{
	sopc_ictl_device* dev = skyeye_mm_zero(sizeof(sopc_ictl_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	init_ictl_register(dev);
	dev->ictl_fifo = CreateFIFO(0x4000);
	return dev->obj;
}

static exception_t config_sopc_ictl(conf_object_t* opaque)
{
	sopc_ictl_device *dev =  opaque->obj;
	system_register_timer_handler(dev->obj, 2000, (time_sched_t)send_ictl_irq, SCHED_US|SCHED_NORMAL);
	return No_exp;
}

static exception_t free_sopc_ictl(conf_object_t* opaque)
{
	sopc_ictl_device *dev = opaque->obj;

	FreeFIFO(dev->ictl_fifo);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sopc_ictl_device *dev =  obj->obj;
	dev->signal.obj = obj2;
	dev->signal.intf = (core_signal_intf *)SKY_get_iface(dev->signal.obj, CORE_SIGNAL_INTF_NAME);

	if (dev->signal.intf == NULL){
		if (dev->signal.obj == NULL)
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
	sopc_ictl_device *dev =  obj->obj;
	*obj2 = dev->signal.obj;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	sopc_ictl_device *dev =  conf_obj->obj;
	attr_value_t int_number_attr = SKY_make_attr_uinteger(dev->signal.num);
	return int_number_attr;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	sopc_ictl_device *dev =  conf_obj->obj;
	dev->signal.num = SKY_attr_uinteger(*value);
	return No_exp;
}

static void sopc_ictl_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of sopc ictl");
	return;
}

void init_sopc_ictl(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "sopc_ictl",
		.class_desc = "sopc_ictl",
		.new_instance = new_sopc_ictl,
		.free_instance = free_sopc_ictl,
		.config_instance = config_sopc_ictl
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = sopc_ictl_read,
		.write = sopc_ictl_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = sopc_ictl_get_regvalue_by_id,
		.get_regname_by_id = sopc_ictl_get_regname_by_id,
		.set_regvalue_by_id = sopc_ictl_set_regvalue_by_id,
		.get_regnum = sopc_ictl_get_regnum,
		.get_regid_by_name = sopc_ictl_get_regid_by_name,
		.get_regoffset_by_id = sopc_ictl_get_regoffset_by_id
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const general_signal_iface_t signal_iface = {
		.raise_signal = raise_signal,
		.lower_signal = lower_signal
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_iface);

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
			.iface = &signal_iface
		}
	};

	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	sopc_ictl_register_attribute(clss);
}

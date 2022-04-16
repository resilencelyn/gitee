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
#include "sopc_sysctrl.h"

static exception_t sopc_sysctrl_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_sysctrl_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;

	switch (offset)
	{
		case GCK_CON:
			*(uint32_t *)buf = regs->gck_con;
			break;
		case SRST_CON:
			*(uint32_t *)buf = regs->srst_con;
			break;
		case PLLM_CFG:
			*(uint32_t *)buf = regs->pllm_cfg;
			break;
		case FPGAIF_CON:
			*(uint32_t *)buf = regs->fpgaif_con;
			break;
		case TIMER_DIV_CON:
			*(uint32_t *)buf = regs->timer_div_con;
			break;
		case BRM1553_CON:
			*(uint32_t *)buf = regs->brm1553_con;
			break;
		case PAD_DUPLI_CTRL0:
			*(uint32_t *)buf = regs->pad_ctrl0;
			break;
		case PAD_DUPLI_CTRL1:
			*(uint32_t *)buf = regs->pad_ctrl1;
			break;
		case VER_SEL:
			*(uint32_t *)buf = regs->ver_sel;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sopc_sysctrl\n", offset);
			return Not_found_exp;
	}
	return No_exp;
}

static exception_t sopc_sysctrl_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_sysctrl_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	uint32_t val = *(uint32_t *)buf;

	switch (offset)
	{
		case GCK_CON:
			regs->gck_con = val;
			break;
		case SRST_CON:
			regs->srst_con = val;
			if ((regs->srst_con & (1 << 7)) == 0) //FPGAIF reset
			{
				regs->srst_con |= (1 << 7); //reset ok
			}
			break;
		case PLLM_CFG:
			regs->pllm_cfg = val;
			break;
		case FPGAIF_CON:
			regs->fpgaif_con = val;
			break;
		case TIMER_DIV_CON:
			regs->timer_div_con = val;
			break;
		case BRM1553_CON:
			regs->brm1553_con = val;
			break;
		case PAD_DUPLI_CTRL0:
			regs->pad_ctrl0 = val;
			//bit18 set 1 to SPI INIT DONE
			break;
		case PAD_DUPLI_CTRL1:
			regs->pad_ctrl1 = val;
			break;
		case VER_SEL:
			regs->ver_sel = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in sopc_sysctrl\n", offset);
			return Not_found_exp;
	}

	return No_exp;
}

static char* sopc_sysctrl_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t sopc_sysctrl_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	sopc_sysctrl_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t sopc_sysctrl_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t sopc_sysctrl_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	sopc_sysctrl_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t sopc_sysctrl_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t sopc_sysctrl_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

void init_sysctrl_register(sopc_sysctrl_device *dev)
{
	struct registers *regs = &(dev->regs);
	regs->gck_con = 0xffffffff;
	regs->srst_con = 0xffffffff;
	regs->timer_div_con = 0;
	regs->pllm_cfg = 0x198; //M(4:11) = 25, N(0:3) = 8
	regs->fpgaif_con = 0x300;
	regs->brm1553_con = 0x3;
	regs->pad_ctrl0 = 0xc1800000;
	regs->pad_ctrl1 = 0;
	regs->ver_sel = 0;
}

static conf_object_t* new_sopc_sysctrl(const char *obj_name)
{
	sopc_sysctrl_device* dev = skyeye_mm_zero(sizeof(sopc_sysctrl_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	init_sysctrl_register(dev);
	return dev->obj;
}

static exception_t free_sopc_sysctrl(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_sopc_sysctrl(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "sopc_sysctrl",
		.class_desc = "sopc_sysctrl",
		.new_instance = new_sopc_sysctrl,
		.free_instance = free_sopc_sysctrl
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = sopc_sysctrl_read,
		.write = sopc_sysctrl_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = sopc_sysctrl_get_regvalue_by_id,
		.get_regname_by_id = sopc_sysctrl_get_regname_by_id,
		.set_regvalue_by_id = sopc_sysctrl_set_regvalue_by_id,
		.get_regnum = sopc_sysctrl_get_regnum,
		.get_regid_by_name = sopc_sysctrl_get_regid_by_name,
		.get_regoffset_by_id = sopc_sysctrl_get_regoffset_by_id
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

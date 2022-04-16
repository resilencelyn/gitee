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
#include "GIC_PL390_imx6.h"

extern GIC_PL390_imx6_device* global_dev;

static void raise_irq(GIC_PL390_imx6_device *dev)
{
	interrupt_signal_t interrupt_signal;

	interrupt_signal.arm_signal.firq = Prev_level;
	interrupt_signal.arm_signal.irq = Low_level;
	interrupt_signal.arm_signal.reset = Prev_level;

	dev->signal_iface->signal(dev->signal, &interrupt_signal);
}

static void raise_fiq(GIC_PL390_imx6_device *dev)
{
	interrupt_signal_t interrupt_signal;

	interrupt_signal.arm_signal.firq = Low_level;
	interrupt_signal.arm_signal.irq = Prev_level;
	interrupt_signal.arm_signal.reset = Prev_level;

	dev->signal_iface->signal(dev->signal, &interrupt_signal);
}

static exception_t GIC_PL390_CPU_Interface_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	GIC_PL390_imx6_device *dev = opaque->obj;

	if (offset == ICCICR) {
		*(uint32_t *)buf = dev->cpu_regs.iccicr[0].all;
	} else if (offset == ICCPMR) {
		*(uint32_t *)buf = dev->cpu_regs.iccpmr[0].all;
	} else if (offset == ICCBPR) {
		*(uint32_t *)buf = dev->cpu_regs.iccbpr[0].all;
	} else if (offset == ICCIAR) {
		*(uint32_t *)buf = dev->cpu_regs.icciar.all;
	} else if (offset == ICCEOIR) {
		*(uint32_t *)buf = dev->cpu_regs.icceoir.all;
	} else if (offset == ICCRPR) {
		*(uint32_t *)buf = dev->cpu_regs.iccrpr;
	} else if (offset == ICCHPIR) {
		*(uint32_t *)buf = dev->cpu_regs.icchpir;
	} else if (offset == ICCABPR) {
		*(uint32_t *)buf = dev->cpu_regs.iccabpr;
	} else if (offset == INTEG_EN_C) {
		*(uint32_t *)buf = dev->cpu_regs.integ_en_c[0];
	} else if (offset == INTERRUPT_OUT) {
		*(uint32_t *)buf = dev->cpu_regs.interrupt_out[0];
	} else if (offset == MATCH_C) {
		*(uint32_t *)buf = dev->cpu_regs.match_c[0];
	} else if (offset == ENABLE_C) {
		*(uint32_t *)buf = dev->cpu_regs.enable_c[0];
	} else if (offset == ICCIIDR) {
		*(uint32_t *)buf = dev->cpu_regs.icciidr;
	} else if (offset == PERIPH_ID_8) {
		*(uint32_t *)buf = dev->cpu_regs.periph_id_8;
	} else if (offset == PERIPH_ID_7) {
		*(uint32_t *)buf = dev->cpu_regs.periph_id_7;
	} else if (offset == PERIPH_ID_6) {
		*(uint32_t *)buf = dev->cpu_regs.periph_id_6;
	} else if (offset == PERIPH_ID_5) {
		*(uint32_t *)buf = dev->cpu_regs.periph_id_5;
	} else if (offset == PERIPH_ID_4) {
		*(uint32_t *)buf = dev->cpu_regs.periph_id_4;
	} else if (offset == PERIPH_ID_3) {
		*(uint32_t *)buf = dev->cpu_regs.periph_id_3;
	} else if (offset == PERIPH_ID_2) {
		*(uint32_t *)buf = dev->cpu_regs.periph_id_2;
	} else if (offset == PERIPH_ID_1) {
		*(uint32_t *)buf = dev->cpu_regs.periph_id_1;
	} else if (offset == PERIPH_ID_0) {
		*(uint32_t *)buf = dev->cpu_regs.periph_id_0;
	} else if (offset == COMPONENT_ID_3) {
		*(uint32_t *)buf = dev->cpu_regs.component_id_3;
	} else if (offset == COMPONENT_ID_2) {
		*(uint32_t *)buf = dev->cpu_regs.component_id_2;
	} else if (offset == COMPONENT_ID_1) {
		*(uint32_t *)buf = dev->cpu_regs.component_id_1;
	} else if (offset == COMPONENT_ID_0) {
		*(uint32_t *)buf = dev->cpu_regs.component_id_0;
    } else {
		skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in GIC_PL390_CPU_Interface\n", offset);
	}
    //printf("%s offset: 0x%X value: 0x%X\n", __func__, offset, *(uint32_t *)buf);
	return 0;
}
static exception_t GIC_PL390_CPU_Interface_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	GIC_PL390_imx6_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

    //printf("%s offset: 0x%X value: 0x%X\n", __func__, offset, val);

	if (offset == ICCICR) {
		dev->cpu_regs.iccicr[0].all = val;
	} else if (offset == ICCPMR) {
		dev->cpu_regs.iccpmr[0].all = val;
	} else if (offset == ICCBPR) {
		dev->cpu_regs.iccbpr[0].all = val;
	} else if (offset == ICCIAR) {
		dev->cpu_regs.icciar.all = val;
	} else if (offset == ICCEOIR) {
		dev->cpu_regs.icceoir.all = val;
        // End interrupt
        if ((dev->FIFO != 0x3FF) && (dev->FIFO != dev->cpu_regs.icciar.Interrupt_ID)) {
            dev->cpu_regs.icciar.Interrupt_ID = dev->FIFO;
            dev->cpu_regs.icciar.CPUID = 0;
            raise_irq(dev);
        } else {
            dev->FIFO = 0x3FF;
            dev->cpu_regs.icciar.all = 0x000003FF;
        }
	} else if (offset == ICCRPR) {
		dev->cpu_regs.iccrpr = val;
	} else if (offset == ICCHPIR) {
		dev->cpu_regs.icchpir = val;
	} else if (offset == ICCABPR) {
		dev->cpu_regs.iccabpr = val;
	} else if (offset == INTEG_EN_C) {
		dev->cpu_regs.integ_en_c[0] = val;
	} else if (offset == INTERRUPT_OUT) {
		dev->cpu_regs.interrupt_out[0] = val;
	} else if (offset == MATCH_C) {
		dev->cpu_regs.match_c[0] = val;
	} else if (offset == ENABLE_C) {
		dev->cpu_regs.enable_c[0] = val;
	} else if (offset == ICCIIDR) {
		dev->cpu_regs.icciidr = val;
	} else if (offset == PERIPH_ID_8) {
		dev->cpu_regs.periph_id_8 = val;
	} else if (offset == PERIPH_ID_7) {
		dev->cpu_regs.periph_id_7 = val;
	} else if (offset == PERIPH_ID_6) {
		dev->cpu_regs.periph_id_6 = val;
	} else if (offset == PERIPH_ID_5) {
		dev->cpu_regs.periph_id_5 = val;
	} else if (offset == PERIPH_ID_4) {
		dev->cpu_regs.periph_id_4 = val;
	} else if (offset == PERIPH_ID_3) {
		dev->cpu_regs.periph_id_3 = val;
	} else if (offset == PERIPH_ID_2) {
		dev->cpu_regs.periph_id_2 = val;
	} else if (offset == PERIPH_ID_1) {
		dev->cpu_regs.periph_id_1 = val;
	} else if (offset == PERIPH_ID_0) {
		dev->cpu_regs.periph_id_0 = val;
	} else if (offset == COMPONENT_ID_3) {
		dev->cpu_regs.component_id_3 = val;
	} else if (offset == COMPONENT_ID_2) {
		dev->cpu_regs.component_id_2 = val;
	} else if (offset == COMPONENT_ID_1) {
		dev->cpu_regs.component_id_1 = val;
	} else if (offset == COMPONENT_ID_0) {
		dev->cpu_regs.component_id_0 = val;
	} else {
		skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in GIC_PL390_CPU_Interface\n", offset);
	}
	return 0;
}

static char* GIC_PL390_CPU_Interface_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return cpu_regs_name[id];
}

static exception_t GIC_PL390_CPU_Interface_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	GIC_PL390_imx6_device *dev = conf_obj->obj;
	struct CPU_Interface_registers *regs = &(dev->cpu_regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t GIC_PL390_CPU_Interface_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	//return  sizeof(struct CPU_Interface_registers) / 4;
	return 26;
}

static uint32_t GIC_PL390_CPU_Interface_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	GIC_PL390_imx6_device *dev = conf_obj->obj;
	struct CPU_Interface_registers *regs = &(dev->cpu_regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t GIC_PL390_CPU_Interface_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct CPU_Interface_registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if(strcmp(name, cpu_regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_GIC_PL390_CPU_Interface(char *obj_name)
{

    if (global_dev == NULL) {
	    global_dev = skyeye_mm_zero(sizeof(GIC_PL390_imx6_device));
    }

    global_dev->core_number = 0;

    global_dev->FIFO = 0x3FF;

	global_dev->cpu_obj = new_conf_object(obj_name, global_dev);
	struct CPU_Interface_registers *regs = &(global_dev->cpu_regs);
	return global_dev->cpu_obj;

}

static exception_t free_GIC_PL390_CPU_Interface(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_GIC_PL390_CPU_Interface(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "GIC_PL390_CPU_Interface",
		.class_desc = "GIC_PL390_CPU_Interface",
		.new_instance = new_GIC_PL390_CPU_Interface,
		.free_instance = free_GIC_PL390_CPU_Interface
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = GIC_PL390_CPU_Interface_read,
		.write = GIC_PL390_CPU_Interface_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = GIC_PL390_CPU_Interface_get_regvalue_by_id,
		.get_regname_by_id = GIC_PL390_CPU_Interface_get_regname_by_id,
		.set_regvalue_by_id = GIC_PL390_CPU_Interface_set_regvalue_by_id,
		.get_regnum = GIC_PL390_CPU_Interface_get_regnum,
		.get_regid_by_name = GIC_PL390_CPU_Interface_get_regid_by_name
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


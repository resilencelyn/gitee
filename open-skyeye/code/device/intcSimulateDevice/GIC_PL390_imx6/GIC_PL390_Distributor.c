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

GIC_PL390_imx6_device* global_dev;

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

static exception_t GIC_PL390_Distributor_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	GIC_PL390_imx6_device *dev = opaque->obj;

	if (offset == ICDDCR) {
		*(uint32_t *)buf = dev->dist_regs.icddcr.all;
	} else if (offset == ICDICTR) {
		*(uint32_t *)buf = dev->dist_regs.icdictr.all;
	} else if (offset == ICDIIDR) {
		*(uint32_t *)buf = dev->dist_regs.icdiidr.all;
	} else if (offset == ICDISR) {
		*(uint32_t *)buf = dev->dist_regs.icdisr[0].all;
	} else if (offset == ICDISER) {
		*(uint32_t *)buf = dev->dist_regs.icdiser[0].all;
	} else if (offset == ICDICER) {
		*(uint32_t *)buf = dev->dist_regs.icdicer[0].all;
	} else if (offset == ICDISPR) {
		*(uint32_t *)buf = dev->dist_regs.icdispr[0].all;
	} else if (offset >= ICDICPR && offset < ICDABR) {
        int v = offset - ICDICPR;
        if (v == 0) {
		    *(uint32_t *)buf = dev->dist_regs.icdicpr[0].all;
        }
	} else if (offset == ICDABR) {
		*(uint32_t *)buf = dev->dist_regs.icdabr[0].all;
    } else if (offset >= ICDIPR && offset <= 0x41F) {
        /*
        int index = (offset - 0x400) / 4;
        if (index < 4) {
		    dev->dist_regs.icdipr[0].priority_sgi_INTID_if[index] = val;
        } else {
		    dev->dist_regs.icdipr[0].priority_ppi_INTID_if[index - 4] = val;
        }
        */
    } else if (offset >= 0x420 && offset <= 0x7FB) {
        int index = (offset - 0x420) / 4;
		*(uint32_t *)buf = dev->dist_regs.icdipr[0].priority_spi_INTID[index];
	} else if (offset == ICDIPTR) {
		*(uint32_t *)buf = dev->dist_regs.icdiptr[0].all;
	} else if (offset == ICDICR) {
		*(uint32_t *)buf = dev->dist_regs.icdicr[0].all;
	} else if (offset == PPI) {
		*(uint32_t *)buf = dev->dist_regs.ppi;
	} else if (offset == SPI) {
		*(uint32_t *)buf = dev->dist_regs.spi;
	} else if (offset == LEGACY_INT) {
		*(uint32_t *)buf = dev->dist_regs.legacy_int[0];
	} else if (offset == MATCH_D) {
		*(uint32_t *)buf = dev->dist_regs.match_d[0];
	} else if (offset == ENABLE_D) {
		*(uint32_t *)buf = dev->dist_regs.enable_d[0];
	} else if (offset == ICDSGIR) {
		*(uint32_t *)buf = dev->dist_regs.icdsgir;
	} else if (offset == PERIPH_ID_8) {
		*(uint32_t *)buf = dev->dist_regs.periph_id_8;
	} else if (offset == PERIPH_ID_7) {
		*(uint32_t *)buf = dev->dist_regs.periph_id_7;
	} else if (offset == PERIPH_ID_6) {
		*(uint32_t *)buf = dev->dist_regs.periph_id_6;
	} else if (offset == PERIPH_ID_5) {
		*(uint32_t *)buf = dev->dist_regs.periph_id_5;
	} else if (offset == PERIPH_ID_4) {
		*(uint32_t *)buf = dev->dist_regs.periph_id_4;
	} else if (offset == PERIPH_ID_3) {
		*(uint32_t *)buf = dev->dist_regs.periph_id_3;
	} else if (offset == PERIPH_ID_2) {
		*(uint32_t *)buf = dev->dist_regs.periph_id_2;
	} else if (offset == PERIPH_ID_1) {
		*(uint32_t *)buf = dev->dist_regs.periph_id_1;
	} else if (offset == PERIPH_ID_0) {
		*(uint32_t *)buf = dev->dist_regs.periph_id_0;
	} else if (offset == COMPONENT_ID_3) {
		*(uint32_t *)buf = dev->dist_regs.component_id_3;
	} else if (offset == COMPONENT_ID_2) {
		*(uint32_t *)buf = dev->dist_regs.component_id_2;
	} else if (offset == COMPONENT_ID_1) {
		*(uint32_t *)buf = dev->dist_regs.component_id_1;
	} else if (offset == COMPONENT_ID_0) {
		*(uint32_t *)buf = dev->dist_regs.component_id_0;
	} else {
		skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in GIC_PL390_Distributor\n", offset);
	}
    //printf("%s offset: 0x%X value: 0x%X\n", __func__, offset, *(uint32_t *)buf);
	return 0;
}

static exception_t GIC_PL390_Distributor_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	GIC_PL390_imx6_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

    //printf("%s offset: 0x%X value: 0x%X\n", __func__, offset, val);

    if (offset == ICDDCR) {
        dev->dist_regs.icddcr.all = val;
    } else if (offset == ICDICTR) {
		dev->dist_regs.icdictr.all = val;
    } else if (offset == ICDIIDR) {
		dev->dist_regs.icdiidr.all = val;
    } else if (offset == ICDISR) {
		dev->dist_regs.icdisr[0].all = val;
    } else if (offset == ICDISER) {
		dev->dist_regs.icdiser[0].all = val;
	} else if (offset >= 0x104 && offset <= 0x17C) {

        dev->dist_regs.spi_enable[(offset - 0x104) / 4] = val;

    } else if (offset == ICDICER) {
		dev->dist_regs.icdicer[0].all = val;
	} else if (offset >= 0x184 && offset <= 0x1FC) {

        dev->dist_regs.spi_enable[(offset - 0x184) / 4] = val;

    } else if (offset == ICDISPR) {
		dev->dist_regs.icdispr[0].all = val;
    } else if (offset == ICDICPR) {
		dev->dist_regs.icdicpr[0].all = val;
	} else if (offset >= 0x284 && offset <= 0x2FC) {

        dev->dist_regs.spi_pending[(offset - 0x284) / 4] = val;

    } else if (offset == ICDABR) {
        dev->dist_regs.icdabr[0].all = val;
    } else if (offset >= ICDIPR && offset <= 0x41F) {
        int index = (offset - 0x400) / 4;
        if (index < 4) {
		    dev->dist_regs.icdipr[0].priority_sgi_INTID_if[index] = val;
        } else {
		    dev->dist_regs.icdipr[0].priority_ppi_INTID_if[index - 4] = val;
        }
    } else if (offset >= 0x420 && offset <= 0x7FB) {
        int index = (offset - 0x420) / 4;
		dev->dist_regs.icdipr[0].priority_spi_INTID[index] = val;
    } else if (offset == ICDIPTR) {
		dev->dist_regs.icdiptr[0].all = val;
    } else if (offset >= 0x820 && offset <= 0xBFB) {

        dev->dist_regs.icdiptr[0].targets_spi_INTID[(offset - 0x820) / 4] = val;

	} else if (offset == ICDICR) {
		dev->dist_regs.icdicr[0].all = val;
	} else if (offset >= 0xC08 && offset <= 0xCFC) {

        dev->dist_regs.int_config[(offset - 0xC08) / 4] = val;

	} else if (offset == PPI) {
		dev->dist_regs.ppi = val;
	} else if (offset == SPI) {
		dev->dist_regs.spi = val;
	} else if (offset == LEGACY_INT) {
		dev->dist_regs.legacy_int[0] = val;
	} else if (offset == MATCH_D) {
		dev->dist_regs.match_d[0] = val;
	} else if (offset == ENABLE_D) {
		dev->dist_regs.enable_d[0] = val;
	} else if (offset == ICDSGIR) {
		dev->dist_regs.icdsgir = val;
	} else if (offset == PERIPH_ID_8) {
		dev->dist_regs.periph_id_8 = val;
	} else if (offset == PERIPH_ID_7) {
		dev->dist_regs.periph_id_7 = val;
	} else if (offset == PERIPH_ID_6) {
		dev->dist_regs.periph_id_6 = val;
	} else if (offset == PERIPH_ID_5) {
		dev->dist_regs.periph_id_5 = val;
	} else if (offset == PERIPH_ID_4) {
		dev->dist_regs.periph_id_4 = val;
	} else if (offset == PERIPH_ID_3) {
		dev->dist_regs.periph_id_3 = val;
	} else if (offset == PERIPH_ID_2) {
		dev->dist_regs.periph_id_2 = val;
	} else if (offset == PERIPH_ID_1) {
		dev->dist_regs.periph_id_1 = val;
	} else if (offset == PERIPH_ID_0) {
		dev->dist_regs.periph_id_0 = val;
	} else if (offset == COMPONENT_ID_3) {
		dev->dist_regs.component_id_3 = val;
	} else if (offset == COMPONENT_ID_2) {
		dev->dist_regs.component_id_2 = val;
	} else if (offset == COMPONENT_ID_1) {
		dev->dist_regs.component_id_1 = val;
	} else if (offset == COMPONENT_ID_0) {
		dev->dist_regs.component_id_0 = val;
	} else {
		skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in GIC_PL390_Distributor\n", offset);
	}
	return 0;
}

static char* GIC_PL390_Distributor_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return dist_regs_name[id];
}

static exception_t GIC_PL390_Distributor_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	GIC_PL390_imx6_device *dev = conf_obj->obj;
	struct Distributor_registers *regs = &(dev->dist_regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t GIC_PL390_Distributor_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return 114;
}

static uint32_t GIC_PL390_Distributor_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	GIC_PL390_imx6_device *dev = conf_obj->obj;
	struct Distributor_registers *regs = &(dev->dist_regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t GIC_PL390_Distributor_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct Distributor_registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if(strcmp(name, dist_regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_GIC_PL390_Distributor(char *obj_name)
{
    if (global_dev == NULL) {
	    global_dev = skyeye_mm_zero(sizeof(GIC_PL390_imx6_device));
    }

    global_dev->core_number = 0;

    global_dev->FIFO = 0x3FF;

    global_dev->cpu_regs.icciar.Interrupt_ID = 0x3FF;

	global_dev->dis_obj = new_conf_object(obj_name, global_dev);
	struct Distributor_registers *regs = &(global_dev->dist_regs);
	return global_dev->dis_obj;

}

static exception_t free_GIC_PL390_Distributor(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

exception_t interrupt_signal(conf_object_t* opaque, int line, void* args) {
    /* called in another thread */
    GIC_PL390_imx6_device *dev = opaque->obj;
    int group;

    // Interrupt queue
    if (dev->cpu_regs.icciar.Interrupt_ID != 0x3FF) {
        dev->FIFO = line;
        return No_exp;
    }

    if (line >= 0 && line <= 15) {            // SGI
        // No implement
		skyeye_log(Warning_log, __FUNCTION__, "GIC_PL390_Distributor SGI No implement\n");
    } else if (line >= 16 && line <= 31) {    // PPI
        // No implement
		skyeye_log(Warning_log, __FUNCTION__, "GIC_PL390_Distributor PPI No implement\n");
    } else if (line >= 32 && line <= 1019) {  // SPI
        if (dev->cpu_regs.iccicr[0].EnableGrp0 || dev->cpu_regs.iccicr[0].EnableGrp1) {
            if (dev->dist_regs.icddcr.Enable) {
                group = dev->dist_regs.spi_security[line - 32];
                if (group == 0) {
                    dev->dist_regs.spi_pending[line - 32] = 0x1;
                    if (dev->cpu_regs.iccicr[0].FIQEn == 0x1) {
                        // Incomplete implementatio
                        skyeye_log(Warning_log, __FUNCTION__, "GIC_PL390_Distributor SPI group 0 fiq Incomplete implementatio\n");
                        raise_fiq(dev);
                    } else if (dev->cpu_regs.iccicr[0].FIQEn == 0x0) {

                        dev->cpu_regs.icciar.Interrupt_ID = line;
                        dev->cpu_regs.icciar.CPUID = 0;
                        raise_irq(dev);
                    }
                } else if (group == 0) {
                    // Incomplete implementatio

                    skyeye_log(Warning_log, __FUNCTION__, "GIC_PL390_Distributor SPI group 1 irq Incomplete implementatio\n");

                    dev->dist_regs.spi_pending[line - 32] = 0x1;
                    raise_irq(dev);
                }
            }
        }
    }
    return No_exp;
}

int lower_signal(conf_object_t* opaque, int line) {
	GIC_PL390_imx6_device *dev = opaque->obj;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	GIC_PL390_imx6_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);

	return No_exp;
}
 
static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	GIC_PL390_imx6_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void init_GIC_PL390_Distributor(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "GIC_PL390_Distributor",
		.class_desc = "GIC_PL390_Distributor",
		.new_instance = new_GIC_PL390_Distributor,
		.free_instance = free_GIC_PL390_Distributor
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = GIC_PL390_Distributor_read,
		.write = GIC_PL390_Distributor_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = GIC_PL390_Distributor_get_regvalue_by_id,
		.get_regname_by_id = GIC_PL390_Distributor_get_regname_by_id,
		.set_regvalue_by_id = GIC_PL390_Distributor_set_regvalue_by_id,
		.get_regnum = GIC_PL390_Distributor_get_regnum,
		.get_regid_by_name = GIC_PL390_Distributor_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static const general_signal_intf general_signal = {
        .event_signal = interrupt_signal,
        .lower_signal = Low_level,
    };
    SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &general_signal);

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
        (struct InterfaceDescription) {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .iface = &general_signal,
        }
	};
	static const struct ConnectDescription connects[] = 
	{
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

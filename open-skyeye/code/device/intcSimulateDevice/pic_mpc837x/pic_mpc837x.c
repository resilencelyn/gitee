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
#include "pic_mpc837x_fifo.h"
#include "pic_mpc837x.h"

#define CLR_BIT(reg, mask) ((reg) &= ~(mask))
static exception_t pic_mpc837x_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	pic_mpc837x_device *dev = opaque->obj;

    uint32_t *data = (uint32_t *)buf;
	switch (offset)
	{
		case SICFR:
			*data = dev->regs.sicfr;
			break;
		case SIVCR:
			*data = dev->regs.sivcr;
			break;
		case SIPNR_H:
			*data = dev->regs.sipnr_h;
			break;
		case SIPNR_L:
			*data = dev->regs.sipnr_l;
			break;
		case SIPRR_A:
			*data = dev->regs.siprr_a;
			break;
		case SIPRR_B:
			*data = dev->regs.siprr_b;
			break;
		case SIPRR_C:
			*data = dev->regs.siprr_c;
			break;
		case SIPRR_D:
			*data = dev->regs.siprr_d;
			break;
		case SIMSR_H:
			*data = dev->regs.simsr_h;
			break;
		case SIMSR_L:
			*data = dev->regs.simsr_l;
			break;
		case SICNR:
			*data = dev->regs.sicnr;
			break;
		case SEPNR:
			*data = dev->regs.sepnr;
			break;
		case SMPRR_A:
			*data = dev->regs.smprr_a;
			break;
		case SMPRR_B:
			*data = dev->regs.smprr_b;
			break;
		case SEMSR:
			*data = dev->regs.semsr;
			break;
		case SECNR:
			*data = dev->regs.secnr;
			break;
		case SERSR:
			*data = dev->regs.sersr;
			break;
		case SERMR:
			*data = dev->regs.sermr;
			break;
		case SERCR:
			*data = dev->regs.sercr;
			break;
		case SEPCR:
			*data = dev->regs.sepcr;
			break;
		case SIFCR_H:
			*data = dev->regs.sifcr_h;
			break;
		case SIFCR_L:
			*data = dev->regs.sifcr_l;
			break;
		case SEFCR:
			*data = dev->regs.sefcr;
			break;
		case SERFR:
			*data = dev->regs.serfr;
			break;
		case SCVCR:
			*data = dev->regs.scvcr;
			break;
		case SMVCR:
			*data = dev->regs.smvcr;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in pic_mpc837x\n", offset);
			break;
	}

//	printf("%s, %d, offset = %x, value = %x\n", __func__, __LINE__, offset, *(uint32_t *)buf);
	return 0;
}
static exception_t pic_mpc837x_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	pic_mpc837x_device *dev = opaque->obj;

	uint32_t val = *(uint32_t *)buf;
//	printf("%s, %d, offset = %x, value = %x\n", __func__, __LINE__, offset, *(uint32_t *)buf);
	switch (offset)
	{
		case SICFR:
			dev->regs.sicfr = val;
			break;
		case SIPRR_A:
			dev->regs.siprr_a = val;
			break;
		case SIPRR_B:
			dev->regs.siprr_b = val;
			break;
		case SIPRR_C:
			dev->regs.siprr_c = val;
			break;
		case SIPRR_D:
			dev->regs.siprr_d = val;
			break;
		case SIMSR_H:
			dev->regs.simsr_h &= ~val;
			break;
		case SIMSR_L:
			dev->regs.simsr_l &= ~val;
			break;
		case SICNR:
			dev->regs.sicnr = val;
			break;
		case SEPNR:
			dev->regs.sepnr = val;
			break;
		case SMPRR_A:
			dev->regs.smprr_a = val;
			break;
		case SMPRR_B:
			dev->regs.smprr_b = val;
			break;
		case SEMSR:
			dev->regs.semsr = val;
			break;
		case SECNR:
			dev->regs.secnr = val;
			break;
		case SERSR:
			dev->regs.sersr = val;
			break;
		case SERMR:
			dev->regs.sermr = val;
			break;
		case SERCR:
			dev->regs.sercr = val;
			break;
		case SEPCR:
			dev->regs.sepcr = val;
			break;
		case SIFCR_H:
			dev->regs.sifcr_h = val;
			break;
		case SIFCR_L:
			dev->regs.sifcr_l = val;
			break;
		case SEFCR:
			dev->regs.sefcr = val;
			break;
		case SERFR:
			dev->regs.serfr = val;
			break;
		case SCVCR:
			dev->regs.scvcr = val;
			break;
		case SMVCR:
			dev->regs.smvcr = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in pic_mpc837x\n", offset);
			break;
	}
	return 0;
}

static char* pic_mpc837x_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t pic_mpc837x_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	pic_mpc837x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t pic_mpc837x_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t pic_mpc837x_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	pic_mpc837x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t pic_mpc837x_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static void reset_registers(struct registers *regs)
{
    regs->siprr_a = 0x05309770;    
    regs->siprr_b = 0x05309770;    
    regs->siprr_c = 0x05309770;    
    regs->siprr_d = 0x05309770;    

    regs->smprr_a = 0x05309770;
    regs->smprr_b = 0x05309770;
}

static void send_pic(void *opaque)
{
	pic_mpc837x_device *dev = ((conf_object_t *)opaque)->obj;
	struct registers *regs = &(dev->regs);
	unsigned char buf;
	if (CheckFIFOReadNum(dev->pic_fifo) > 0)
	{
		ReadFIFO(dev->pic_fifo, &buf, 1);
		int line = buf;
		dev->regs.sivcr = line;

		if (line < 17){
			dev->regs.simsr_h |= (1 << (line+15));
		}else if (line > 31 && line < 48){
			dev->regs.simsr_h |= (1 << (line-32));
		}else if (line >= 64 && line < 92){
			dev->regs.simsr_l |= (1 << (line-64));
		}else if ( (line >= 17) && (line <= 23) ){
			uint32_t line_bit = 30 - (line -17);
			dev->regs.semsr |= (1 << line_bit);
		}else if (line == 48){
			CLR_BIT(dev->regs.semsr,0x8000);
			dev->regs.semsr |= (1 << 31);
		 }

		if (dev->signal_iface)
			dev->signal_iface->signal(dev->signal, NULL);
	}	

}

static conf_object_t* new_pic_mpc837x(char *obj_name)
{
	pic_mpc837x_device* dev = skyeye_mm_zero(sizeof(pic_mpc837x_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

    reset_registers(regs);
	dev->pic_fifo = CreateFIFO(0x40000);
	system_register_timer_handler(dev->obj, 1000, (time_sched_t)send_pic, SCHED_US|SCHED_NORMAL);
	return dev->obj;
}

static exception_t free_pic_mpc837x(conf_object_t* opaque)
{
	pic_mpc837x_device *dev =  opaque->obj;
	FreeFIFO(dev->pic_fifo);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}


static int raise_signal(conf_object_t *obj, int line){
	pic_mpc837x_device *dev = obj->obj;
	WriteFIFO(dev->pic_fifo, &line, 1);
//	printf("int_num : %x\n", line);
	return No_exp;
}

static int lower_signal(conf_object_t *obj, int line)
{
	pic_mpc837x_device *dev = obj->obj;

	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	pic_mpc837x_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);

	if(dev->signal_iface == NULL){
		if (dev->signal == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Can't get 'core_signal_intf' interface from NULL obj.");
		else
			skyeye_log(Error_log, __FUNCTION__, "Can't get 'core_signal_intf' interface from %s.", obj2->objname);
	}

	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	pic_mpc837x_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void init_pic_mpc837x(void)
{
	static skyeye_class_t class_data = {
		.class_name = "mpc837x_pic",
		.class_desc = "mpc837x_pic",
		.new_instance = new_pic_mpc837x,
		.free_instance = free_pic_mpc837x,
		.module_type = SKYML_ONCHIP(SC_mpc8378),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory = {
		.read = pic_mpc837x_read,
		.write = pic_mpc837x_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = pic_mpc837x_get_regvalue_by_id,
		.get_regname_by_id = pic_mpc837x_get_regname_by_id,
		.set_regvalue_by_id = pic_mpc837x_set_regvalue_by_id,
		.get_regnum = pic_mpc837x_get_regnum,
		.get_regid_by_name = pic_mpc837x_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const general_signal_iface_t signal_iface = {
		.raise_signal = raise_signal,
		.lower_signal = lower_signal
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_iface);

	static const struct InterfaceDescription ifaces[] = {
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
	static const struct ConnectDescription connects[] = {
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		}
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

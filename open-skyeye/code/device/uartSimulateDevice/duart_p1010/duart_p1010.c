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
#include <skyeye_uart_ops.h>
#define DEBUG
#include <skyeye_log.h>
#include "duart_p1010.h"

void raise_signal(conf_object_t *obj){
	duart_p1010_device *dev = obj->obj;
	if(dev->signal_iface&&dev->signal){
		printf("%s, %s, %d\n", obj->objname,__func__, __LINE__);
		dev->signal_iface->raise_signal(dev->signal, dev->irq_num);
	}
}
static exception_t duart_p1010_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	duart_p1010_device *dev = opaque->obj;
	switch (offset) {
		case 0:
			if (BITS(dev->regs.ulcr1, 7, 7) == 0) {
				*(uint8_t *)buf = dev->regs.urbr1;
			} else {
				*(uint8_t *)buf = dev->regs.udlb1;
			}
			break;
		case 1:
			if (BITS(dev->regs.ulcr1, 7, 7) == 0) {
				*(uint8_t *)buf = dev->regs.uier1;
			} else {
				*(uint8_t *)buf = dev->regs.udmb1;
			}
			break;
		case 2:
			if (BITS(dev->regs.ulcr1, 7, 7) == 0) {
				*(uint8_t *)buf = dev->regs.uiir1;
			} else {
				*(uint8_t *)buf = dev->regs.uafr1;
			}
			break;
		case ULCR1:
			*(uint8_t *)buf = dev->regs.ulcr1;
			break;
		case UMCR1:
			*(uint8_t *)buf = dev->regs.umcr1;
			break;
		case ULSR1:
			*(uint8_t *)buf = dev->regs.ulsr1;
			break;
		case UMSR1:
			*(uint8_t *)buf = dev->regs.umsr1;
			break;
		case USCR1:
			*(uint8_t *)buf = dev->regs.uscr1;
			break;
		case UDSR1:
			*(uint8_t *)buf = dev->regs.udsr1;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in duart_p1010\n", offset);
			break;
	}
	printf("%s, %s, %d, offset = %x, value = %x\n", opaque->objname ,__func__, __LINE__, offset, *(char *)buf);
	return 0;
}
static exception_t duart_p1010_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	duart_p1010_device *dev = opaque->obj;
	printf("%s, %s, %d, offset = %x, value = %x\n", opaque->objname, __func__, __LINE__, offset, *(uint32_t *)buf);
	uint8_t val = *(uint8_t *)buf;
	switch (offset) {
		case 0:
			if (BITS(dev->regs.ulcr1, 7, 7) == 0) {
				dev->fifo_empty = F_FALSE;
				dev->regs.uthr1 = val;
				if (dev->term && dev->term_iface) {
					dev->term_iface->write(dev->term, &val, 1);
				}
				dev->fifo_empty = F_TRUE;
			} else {
				dev->regs.udlb1 = val;
			}
			break;
		case 1:
			if (BITS(dev->regs.ulcr1, 7, 7) == 0) {
				dev->regs.uier1 = val;
			} else {
				dev->regs.udmb1 = val;
			}
			break;
		case 2:
			if (BITS(dev->regs.ulcr1, 7, 7) == 0) {
				dev->regs.ufcr1 = val;
			} else {
				dev->regs.uafr1 = val;
			}
			break;
		case ULCR1:
			dev->regs.ulcr1 = val;
			break;
		case UMCR1:
			dev->regs.umcr1 = val;
			break;
		case ULSR1:
			dev->regs.ulsr1 = val;
			break;
		case UMSR1:
			dev->regs.umsr1 = val;
			break;
		case USCR1:
			dev->regs.uscr1 = val;
			break;
		case UDSR1:
			dev->regs.udsr1 = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in duart_p1010\n", offset);
			break;
	}
	return 0;
}
static char* duart_p1010_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t duart_p1010_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	duart_p1010_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}
static uint32_t duart_p1010_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t duart_p1010_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	duart_p1010_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;

	return *regs_value;
}
static uint32_t duart_p1010_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}

	return regnum;
}


void fifo_check(conf_object_t  *obj){
	duart_p1010_device *dev = obj->obj;

	if (dev->dev_enable == 0)
		return;
	if((dev->regs.uier1 & ETHREI) &&(dev->fifo_empty)){ //if transmitler interrupt is enabled ?
	//		printf("%s, %s, %d\n", obj->objname,__func__, __LINE__);
			dev->regs.uiir1 = 0x2;
			raise_signal(obj);
	}
}

static exception_t term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	duart_p1010_device *dev = obj->obj;
	dev->term = obj2;
	dev->term_iface = (skyeye_uart_intf *)SKY_get_iface(dev->term, SKYEYE_UART_INTF);

	if (dev->term_iface == NULL){
		if (dev->term == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_UART_INTF);
			dev->dev_enable = 0;
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);

		return Not_found_exp;
	}

	dev->dev_enable = 1;
	return No_exp;
}
static exception_t term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	duart_p1010_device *dev = obj->obj;

	*obj2 = dev->term;
	*port = NULL;

	return No_exp;
}

static exception_t uart_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	duart_p1010_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);
	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
			dev->dev_enable = 0;
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	dev->dev_enable = 1;

	return No_exp;
}

static exception_t uart_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	duart_p1010_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* obj, attr_value_t* idx){
	duart_p1010_device *dev = obj->obj;
	attr_value_t irq_num = SKY_make_attr_uinteger(dev->irq_num);
	return irq_num;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	duart_p1010_device *dev = obj->obj;
	dev->irq_num = SKY_attr_uinteger(*value);
	return No_exp;
}

static void class_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "irq_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number");
	return;
}



static conf_object_t* new_duart_p1010(char *obj_name)
{
	duart_p1010_device* dev = skyeye_mm_zero(sizeof(duart_p1010_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	regs->uiir1 = 0x01;
	regs->ulsr1 = 0x60;
	regs->udsr1 = 0x01;
	dev->fifo_empty = F_TRUE;

	uint32_t id;
	create_thread_scheduler(100000, Periodic_sched, fifo_check, dev->obj, &id);
	dev->dev_enable = 1;
	return dev->obj;
}
static exception_t free_duart_p1010(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}
void init_duart_p1010(void)
{
	static skyeye_class_t class_data = {
		.class_name = "p1010_duart",
		.class_desc = "p1010_duart",
		.new_instance = new_duart_p1010,
		.free_instance = free_duart_p1010
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory = {
		.read = duart_p1010_read,
		.write = duart_p1010_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = duart_p1010_get_regvalue_by_id,
		.get_regname_by_id = duart_p1010_get_regname_by_id,
		.set_regvalue_by_id = duart_p1010_set_regvalue_by_id,
		.get_regnum = duart_p1010_get_regnum,
		.get_regid_by_name = duart_p1010_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf
		},
	};
	static const struct ConnectDescription connects[] = {
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = SKYEYE_UART_INTF,
			.set = term_set,
			.get = term_get,
		},
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = uart_signal_set,
			.get = uart_signal_get,
		},

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	class_register_attribute(clss);
}

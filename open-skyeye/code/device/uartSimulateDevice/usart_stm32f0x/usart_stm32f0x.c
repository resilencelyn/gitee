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
#include "skyeye_uart_ops.h"
#include "usart_stm32f0x.h"
static exception_t usart_stm32f0x_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	usart_stm32f0x_device *dev = opaque->obj;
	switch (offset)
	{
		case USART_CR1:
			*(uint32_t *)buf = dev->regs.usart_cr1.all;
			break;
		case USART_CR2:
			*(uint32_t *)buf = dev->regs.usart_cr2;
			break;
		case USART_CR3:
			*(uint32_t *)buf = dev->regs.usart_cr3;
			break;
		case USART_BRR:
			*(uint32_t *)buf = dev->regs.usart_brr;
			break;
		case USART_GTPR:
			*(uint32_t *)buf = dev->regs.usart_gtpr;
			break;
		case USART_RTOR:
			*(uint32_t *)buf = dev->regs.usart_rtor;
			break;
		case USART_RQR:
			*(uint32_t *)buf = dev->regs.usart_rqr;
			break;
		case USART_ISR:
			*(uint32_t *)buf = dev->regs.usart_isr.all;
			break;
		case USART_ICR:
			*(uint32_t *)buf = dev->regs.usart_icr;
			break;
		case USART_RDR:
			*(uint32_t *)buf = dev->regs.usart_rdr;
			break;
		case USART_TDR:
			*(uint32_t *)buf = dev->regs.usart_tdr;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in usart_stm32f0x\n", offset);
			break;
	}
    //printf("%d %s 0x%x %x\n",__LINE__,__func__,offset,*(uint32_t*)buf);
	return 0;
}
static exception_t usart_stm32f0x_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	usart_stm32f0x_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
    //printf("%d %s 0x%x %x\n",__LINE__,__func__,offset,val);
	switch (offset)
	{
		case USART_CR1:
			dev->regs.usart_cr1.all = val;
            if(dev->regs.usart_cr1.bit.ue == 1){
                dev->regs.usart_isr.bit.teack = 1;
                dev->regs.usart_isr.bit.reack = 1;
                if(dev->regs.usart_tdr == 0){
                    dev->regs.usart_isr.bit.txe = 1;
                }
            }
           
			break;
		case USART_CR2:
			dev->regs.usart_cr2 = val;
			break;
		case USART_CR3:
			dev->regs.usart_cr3 = val;
			break;
		case USART_BRR:
			dev->regs.usart_brr = val;
			break;
		case USART_GTPR:
			dev->regs.usart_gtpr = val;
			break;
		case USART_RTOR:
			dev->regs.usart_rtor = val;
			break;
		case USART_RQR:
			dev->regs.usart_rqr = val;
			break;
		case USART_ISR:
			dev->regs.usart_isr.all = val;
			break;
		case USART_ICR:
			dev->regs.usart_icr = val;
			break;
		case USART_RDR:
			dev->regs.usart_rdr = val;
			break;
		case USART_TDR:
			dev->regs.usart_tdr = val;
            char data = dev->regs.usart_tdr & 0xff;
			dev->regs.usart_tdr = 1;
            if(data != 0){
                if(dev->term_iface != NULL){
                    dev->term_iface->write(dev->term,&data,1);
                }
                if(dev->uart_file_iface != NULL){
                    dev->uart_file_iface->write(dev->uart_file, &data, 1);
                }
                data = 0;
            }
            dev->regs.usart_isr.bit.tc = 1;
			dev->regs.usart_tdr = 0;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in usart_stm32f0x\n", offset);
			break;
	}
	return 0;
}
static char* usart_stm32f0x_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t usart_stm32f0x_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	usart_stm32f0x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t usart_stm32f0x_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t usart_stm32f0x_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	usart_stm32f0x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t usart_stm32f0x_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static exception_t term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	usart_stm32f0x_device *dev = obj->obj;
	dev->term = obj2;
	dev->term_iface = (skyeye_uart_intf*)SKY_get_iface(dev->term, SKYEYE_UART_INTF);
	if (dev->term_iface == NULL){
		if (dev->term == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can't Get 'skyeye_uart_intf' interface from NULL object.");
			dev->dev_enable = 0;
		}
		else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.", SKYEYE_UART_INTF, obj2->objname);

		return Not_found_exp;
	}
	dev->dev_enable = 1;
	return No_exp;
}

static exception_t term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	usart_stm32f0x_device *dev = obj->obj;
	*obj2 = dev->term;
	*port = NULL;
	return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	usart_stm32f0x_device *dev = obj->obj;
	dev->uart_file = obj2;
	dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
	if (dev->uart_file_iface == NULL){
		if (dev->uart_file == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can't Get 'skyeye_uart_intf' interface from NULL object.");
			dev->dev_enable = 0;
		}
		else
			skyeye_log(Error_log, __FUNCTION__, "Can't Get 'skyeye_uart_intf' interface from %s.", obj2->objname);
		return Not_found_exp;
	}

	dev->dev_enable = 1;
	return No_exp;
}
 
static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	usart_stm32f0x_device *dev = obj->obj;
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}


static void uart_sched_func(conf_object_t *obj)
{
	usart_stm32f0x_device *dev = obj->obj;
    if(dev->regs.usart_tdr != 0 && dev->dev_enable){
        char data = dev->regs.usart_tdr & 0xff;
        if(dev->term_iface != NULL){
            //dev->term_iface->write(dev->term,&data,1);
        }
        dev->regs.usart_isr.bit.txe = 1;
    }
    //dev->regs.usart_tdr = 0;
    //dev->regs.usart_isr.bit.tc = 1;
}

static conf_object_t* new_usart_stm32f0x(char *obj_name)
{
	usart_stm32f0x_device* dev = skyeye_mm_zero(sizeof(usart_stm32f0x_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
    create_thread_scheduler((unsigned int)10, Periodic_sched, uart_sched_func, dev->obj, &(dev->sched_id));
	dev->dev_enable = 1;
	return dev->obj;
}
static exception_t free_usart_stm32f0x(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static attr_value_t get_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	usart_stm32f0x_device *dev = conf_obj->obj;
	attr_value_t rfifo = SKY_make_attr_uinteger(dev->rfifo);
	return rfifo;
}
static exception_t set_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	usart_stm32f0x_device *dev = conf_obj->obj;
	dev->rfifo = SKY_attr_uinteger(*value);
	return No_exp;
}

static usart_stm32f0x_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "rfifo", get_attr_rfifo, NULL, set_attr_rfifo, NULL, SKY_Attr_Optional, "uinteger", "usart rfifo length");
}

void init_usart_stm32f0x(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "usart_stm32f0x",
		.class_desc = "usart_stm32f0x",
		.new_instance = new_usart_stm32f0x,
		.free_instance = free_usart_stm32f0x,
		.module_type = SKYML_ONCHIP(SC_stm32),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = usart_stm32f0x_read,
		.write = usart_stm32f0x_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = usart_stm32f0x_get_regvalue_by_id,
		.get_regname_by_id = usart_stm32f0x_get_regname_by_id,
		.set_regvalue_by_id = usart_stm32f0x_set_regvalue_by_id,
		.get_regnum = usart_stm32f0x_get_regnum,
		.get_regid_by_name = usart_stm32f0x_get_regid_by_name
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
			.name = SKYEYE_UART_INTF,
			.set = term_set,
			.get = term_get,
		},
		(struct ConnectDescription ){
			.name = UART_FILE_INTF,
			.set = uart_file_set,
			.get = uart_file_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	usart_stm32f0x_register_attribute(clss);
}

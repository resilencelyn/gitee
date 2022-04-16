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
#include "c687_uart.h"
static exception_t c687_uart_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	c687_uart_device *dev = opaque->obj;
	switch (offset)
	{
		case UART_REV_SEND_REG:
			*(uint32_t *)buf = dev->regs.uart_rev_send_reg;
			break;
		case UART_COMM_CTR_REG:
			*(uint32_t *)buf = dev->regs.uart_comm_ctr_reg;
			break;
		case UART_COMM_STA_REG:
			*(uint32_t *)buf = dev->regs.uart_comm_sta_reg;
			break;
		case UART_INT_CTR_REG:
			*(uint32_t *)buf = dev->regs.uart_int_ctr_reg;
			break;
		case UART_INT_FLG_REG:
			*(uint32_t *)buf = dev->regs.uart_int_flg_reg;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in c687_uart\n", offset);
			break;
	}
	return 0;
}

static exception_t serial_write(conf_object_t *opaque, uint32_t value)
{
	c687_uart_device *dev = opaque->obj;
	unsigned char buf = value;
	if ((dev->regs.uart_comm_sta_reg & 0x61) == 0x60)//判断接收寄存器是否为非满或者发送寄存器是否空，满足的话进行接收数据---避免数据同时收发
	 {
		if (buf == 127)
		buf = '\b';
		dev->regs.uart_rev_send_reg = buf;//先接收数据然后在中断处理函数中发送数据实现回显
		dev->regs.uart_comm_sta_reg |= 0x01;//置位，接收寄存器满
		dev->regs.uart_int_flg_reg = 0x04; //接收寄存器满中断
		if(dev->regs.uart_int_ctr_reg & 0x01)//允许接收寄存器满中断
		{
		//printf("dev->num = %d\n",dev->num);
			if(dev->signal){
				dev->signal_iface->raise_signal(dev->signal,dev->num);
			}
		}
	}
	return 0;
}

static exception_t c687_uart_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	c687_uart_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case UART_REV_SEND_REG:
			dev->regs.uart_rev_send_reg = val;
			//printf("dev->regs.uart_rev_send_reg = 0x%x\n",dev->regs.uart_rev_send_reg);
			if(!(dev->regs.uart_comm_ctr_reg & 0x80))
			{
				if(dev->term){
					dev->term_iface->write(dev->term,&(dev->regs.uart_rev_send_reg),1);
				}
					dev->regs.uart_comm_sta_reg = 0x60;//发送完之后，发送寄存器空，接收寄存器空
					//dev->regs.uart_int_flg_reg = 0x02;//发送寄存器空中断
					if(dev->signal){
						dev->signal_iface->raise_signal(dev->signal,dev->num);
					}
			}
			break;
		case UART_COMM_CTR_REG:
			dev->regs.uart_comm_ctr_reg = val;
			//printf("dev->regs.uart_comm_ctr_reg = 0x%x\n",dev->regs.uart_comm_ctr_reg);
			break;
		case UART_COMM_STA_REG:
			dev->regs.uart_comm_sta_reg = val;
			//printf("dev->regs.uart_comm_sta_reg = 0x%x\n",dev->regs.uart_comm_sta_reg);
			break;
		case UART_INT_CTR_REG:
			dev->regs.uart_int_ctr_reg = val;
			//printf("dev->regs.uart_int_ctr_reg = 0x%x\n",dev->regs.uart_int_ctr_reg);
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in c687_uart\n", offset);
			break;
	}
	return 0;
}
static char* c687_uart_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t c687_uart_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	c687_uart_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t c687_uart_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t c687_uart_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	c687_uart_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t c687_uart_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_c687_uart(const char *obj_name)
{
	c687_uart_device* dev = skyeye_mm_zero(sizeof(c687_uart_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->regs.uart_comm_sta_reg = 0x60;//初始化通信状态寄存器：发送数据寄存器空，发送移位寄存器空
	dev->regs.uart_int_flg_reg = 0x01;//初始化中断标志寄存器为无中断
	return dev->obj;
}
static exception_t free_c687_uart(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	c687_uart_device *dev =  conf_obj->obj;
	attr_value_t int_number_attr = SKY_make_attr_uinteger(dev->num);
	return int_number_attr;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	c687_uart_device *dev =  conf_obj->obj;
	dev->num = SKY_attr_uinteger(*value);
	return No_exp;
}

static void c687_uart_2_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of c687 uart");
	return;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
        c687_uart_device *dev = obj->obj;
        dev->signal = obj2;
        dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);
        return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2,const char **port, int index)
{
        c687_uart_device *dev = obj->obj;
        *obj2 = dev->signal;
        *port = NULL;

        return No_exp;
}

static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	c687_uart_device *dev = obj->obj;
	dev->term = obj2;
	dev->term_iface = (skyeye_uart_intf*)SKY_get_iface(obj2, SKYEYE_UART_INTF);
	if (dev->term_iface == NULL){
		if (dev->term == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.", SKYEYE_UART_INTF);
		else
			skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
 
static exception_t uart_term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	c687_uart_device *dev = obj->obj;
	*obj2 = dev->term;
	*port = NULL;
	return No_exp;
}


void init_c687_uart(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c687_uart",
		.class_desc = "c687_uart",
		.new_instance = new_c687_uart,
		.free_instance = free_c687_uart,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = c687_uart_read,
		.write = c687_uart_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	
	static const skyeye_serial_device_t serial_device = {
		.write = serial_write,
		.receive_ready = NULL,
	};
	SKY_register_iface(clss, SERIAL_DEVICE_INTERFACE, &serial_device);
	
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = c687_uart_get_regvalue_by_id,
		.get_regname_by_id = c687_uart_get_regname_by_id,
		.set_regvalue_by_id = c687_uart_set_regvalue_by_id,
		.get_regnum = c687_uart_get_regnum,
		.get_regid_by_name = c687_uart_get_regid_by_name
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
		(struct InterfaceDescription) {
			.name = SERIAL_DEVICE_INTERFACE,
			.iface = &serial_device,
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
				(struct ConnectDescription ){
			.name = SKYEYE_UART_INTF,
			.set = uart_term_set,
			.get = uart_term_get
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	c687_uart_2_register_attribute(clss);
}

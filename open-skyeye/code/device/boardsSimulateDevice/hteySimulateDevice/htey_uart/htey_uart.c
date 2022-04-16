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
#include <skyeye_uart_ops.h>
#define DEBUG
#include <skyeye_log.h>
#include "htey_uart.h"
static exception_t htey_uart_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_uart_device *dev = opaque->obj;
	switch (offset)
	{
		case UART_TX_RX: {
					 char pbuf;
					 //*(uint32_t *)buf = dev->regs.uart_tx_rx;
					 ReadFIFO(dev->read_fifo, &pbuf, 1);
					 *(uint32_t *)buf = pbuf &0xff;
					 if (CheckCanReadNum(dev->read_fifo) == 0) {
						 dev->regs.uart_lsr &= ~0x1;	
					 }
					 break;
				 }
		case UART_IER:
				 *(uint32_t *)buf = dev->regs.uart_ier;
				 break;
		case UART_IFR:
				 *(uint32_t *)buf = dev->regs.uart_ifr;
				 break;
		case UART_LCR:
				 *(uint32_t *)buf = dev->regs.uart_lcr;
				 break;
		case UART_FCR:
				 *(uint32_t *)buf = dev->regs.uart_fcr;
				 break;
		case UART_LSR:
				 *(uint32_t *)buf = dev->regs.uart_lsr;
				 break;
		case UART_BDRH:
				 *(uint32_t *)buf = dev->regs.uart_bdrh;
				 break;
		case UART_BDRL:
				 *(uint32_t *)buf = dev->regs.uart_bdrl;
				 break;
		default:
				 skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in htey_uart\n", offset);
				 break;
	}

	static int icount = 0;
	//if (icount <= 20){
		//printf("%s, %d, 0ffset = %x, value = %x\n", __func__, __LINE__, offset, *(char *)buf);
	//	icount++;
	//}

	return 0;
}
static exception_t htey_uart_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_uart_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf("%s, %d, 0ffset = %x, value = %x\n", __func__, __LINE__, offset, val);
	switch (offset)
	{
		case UART_TX_RX:
			//dev->regs.uart_tx_rx = val;
			dev->regs.uart_lsr &= ~0x10;
			if(dev->serial)
				dev->serial_iface->write(dev->serial, val&0xff);
			if(dev->client) {
				dev->client_iface->client_send(dev->client, val&0xff, 1);
			}
				
			dev->regs.uart_lsr |= 0x10;
			break;
		case UART_IER:
			dev->regs.uart_ier = val;
			break;
		case UART_IFR:
			dev->regs.uart_ifr = val;
			break;
		case UART_LCR:
			dev->regs.uart_lcr = val;
			break;
		case UART_FCR:
			dev->regs.uart_fcr = val;
			break;
		case UART_LSR:
			dev->regs.uart_lsr = val;
			break;
		case UART_BDRH:
			dev->regs.uart_bdrh = val;
			break;
		case UART_BDRL:
			dev->regs.uart_bdrl = val;

			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in htey_uart\n", offset);
			break;
	}
	return 0;
}
static exception_t serial_write(conf_object_t *obj, uint32_t value)
{
	htey_uart_device *dev = obj->obj;

	return No_exp;
}	

static exception_t serial_write_chars(conf_object_t *obj, void *buf, uint32_t cnt)
{
	htey_uart_device *dev = obj->obj;
	WriteFIFO(dev->read_fifo, buf, cnt);
	if (CheckCanReadNum(dev->read_fifo) >= 1) {
		dev->regs.uart_lsr |= 0x1;	
	}
	/*
	   char *str = (char *)buf;
	   int i = 0;
	   for (i = 0; i < cnt; i++)
	   {
	   printf("0x%x  ", *str++);
	//dev->serial_iface->write(dev->serial, *str++);
	}
	printf("\n");
	*/
	return No_exp;
}	

static char* htey_uart_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t htey_uart_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	htey_uart_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t htey_uart_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t htey_uart_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	htey_uart_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t htey_uart_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_htey_uart(char *obj_name)
{
	htey_uart_device* dev = skyeye_mm_zero(sizeof(htey_uart_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->read_fifo = CreateFIFO(256);
	dev->regs.uart_lsr |= 0x10;
	return dev->obj;
}
static exception_t free_htey_uart(conf_object_t* opaque)
{
	htey_uart_device *dev = opaque->obj;
	if(dev->read_fifo) {
		FreeFIFO(dev->read_fifo);
	}
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
static exception_t serial_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	htey_uart_device *dev = obj->obj;
	dev->serial = obj2;
	dev->serial_iface = (skyeye_serial_device_t*)SKY_get_iface(obj2, SERIAL_DEVICE_INTERFACE);
	if (dev->serial_iface == NULL){
		if (dev->serial == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SERIAL_DEVICE_INTERFACE);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SERIAL_DEVICE_INTERFACE, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t serial_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	htey_uart_device *dev = obj->obj;
	*obj2 = dev->serial;
	*port = NULL;
	return No_exp;
}

static exception_t client_intf_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	htey_uart_device *dev = obj->obj;
	dev->client = obj2;
	dev->client_iface = (htey_client_intf *)SKY_get_iface(dev->client, HTEY_CLIENT_INTF);
	if (dev->client_iface == NULL){
		if (dev->client == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", HTEY_CLIENT_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", HTEY_CLIENT_INTF, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t client_intf_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	htey_uart_device *dev = obj->obj;
	dev->client = obj2;
	*obj2 = dev->client;
	*port = NULL;
	return No_exp;
}


void init_htey_uart(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "htey_uart",
		.class_desc = "htey_uart",
		.new_instance = new_htey_uart,
		.free_instance = free_htey_uart
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = htey_uart_read,
		.write = htey_uart_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_serial_device_t serial_device = {
		.write = serial_write,
		.write_chars = serial_write_chars,
		.receive_ready = NULL,
	};
	SKY_register_iface(clss, SERIAL_DEVICE_INTERFACE, &serial_device);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = htey_uart_get_regvalue_by_id,
		.get_regname_by_id = htey_uart_get_regname_by_id,
		.set_regvalue_by_id = htey_uart_set_regvalue_by_id,
		.get_regnum = htey_uart_get_regnum,
		.get_regid_by_name = htey_uart_get_regid_by_name
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
			.name = SERIAL_DEVICE_INTERFACE,
			.set = serial_set,
			.get = serial_get,
		},
		(struct ConnectDescription ){
			.name = HTEY_CLIENT_INTF,
			.set = client_intf_set,
			.get = client_intf_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

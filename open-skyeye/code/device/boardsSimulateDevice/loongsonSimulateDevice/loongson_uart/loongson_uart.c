#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_system.h>
#include <skyeye_iface.h>
#include "skyeye_uart_ops.h"
#include "loongson_uart.h"

static int Interrupt_handling(conf_object_t *opaque)
{
	loongson_uart_device *dev = opaque->obj;
	loongson_uart_reg_t *regs= dev->regs;
	uint8_t b_num;

	switch (dev->FCR_T.bits.trigger)
	{
		case 0:
			b_num = 1;
		case 1:
			b_num = 4;
		case 2:
			b_num = 8;
		case 3:
			b_num = 14;
		default:
			//skyeye_log(Error_log, __FUNCTION__, "loongson_uart regs->HR_FCR_DL_D.FCR_T.trigger value = %d failed !\n",regs->HR_FCR_DL_D.FCR_T.trigger);
			break;
	}
	if(regs->IER_T.bits.IRE && b_num == dev->recv_data_num)		//up a recv interrupt
	{
		regs->HR_T.bits.H = 2;
		regs->HR_T.bits.INTPn = 0;
		dev->recv_data_num = 0;
	}
	return 0;
}

static void move_DAT_read(conf_object_t * opaque)
{
	loongson_uart_device *dev = opaque->obj;
	loongson_uart_reg_t *regs= dev->regs;
	if(dev->FCR_T.bits.rxreset)
	{
		dev->recv_data_num++;
		if(regs->HR_T.bits.H == 2 && regs->IER_T.bits.IRE)			//clear recv interrupt
			regs->HR_T.bits.INTPn = 1;
		Interrupt_handling(opaque);
		regs->LSR_T.bits.DR = 0;
	}
}

static exception_t loongson_uart_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	loongson_uart_device *dev = opaque->obj;
	loongson_uart_reg_t *regs= dev->regs;

	switch (offset)
	{
		case DL_L:
		{
			*(uint8_t *)buf = regs->DAT_T.all;
			if(! regs->LCR_T.bits.dlab)
				move_DAT_read(opaque);
			else
				*(uint8_t *)buf = dev->DL.DL_l;
			break;
		}
		case DL_H:
			if(! regs->LCR_T.bits.dlab)
				*(uint8_t *)buf = regs->HR_T.all;
			else
				*(uint8_t *)buf = dev->DL.DL_h;
			break;
		case DL_D:
		{
			if(! regs->LCR_T.bits.dlab)
				*(uint8_t *)buf = regs->HR_T.all;
			else
				*(uint8_t *)buf = dev->DL.DL_d;
			if(! regs->LCR_T.bits.dlab && regs->IER_T.bits.ITE && regs->HR_T.bits.H == 1) //clear send state interrupt
				regs->HR_T.bits.INTPn = 1;
			break;
		}
		case LCR:
			*(uint8_t *)buf = regs->LCR_T.all;
			break;
		case MCR:
			*(uint8_t *)buf = regs->MCR_T.all;
			regs->LSR_T.all &= 0x60;
			break;
		case LSR:
			*(uint8_t *)buf = regs->LSR_T.all;
			break;
		case MSR:
			*(uint8_t *)buf = regs->MSR_T.all;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in c687_uart\n", offset);
			break;
	}
	return 0;
}

static exception_t serial_write(conf_object_t *opaque, uint32_t value)
{
	loongson_uart_device *dev = opaque->obj;
	loongson_uart_reg_t *regs= dev->regs;
	regs->DAT_T.all = (uint8_t)value;
	if(dev->FCR_T.bits.rxreset)
		regs->LSR_T.bits.DR = 1;
	return 0;
}

static void send_to_term(conf_object_t *opaque)
{
	loongson_uart_device *dev = opaque->obj;
	loongson_uart_reg_t *regs= dev->regs;

	if (dev->term_iface != NULL)
		dev->term_iface->write(dev->term, &(regs->DAT_T.all), 1);
	else
		printf("[WARING]:In %s (line:%d), Please Register SKYEYE_UART_INTF!\n", __func__, __LINE__);

	if (dev->uart_file_iface != NULL)
		dev->uart_file_iface->write(dev->uart_file, &(regs->DAT_T.all), 1);
	else
		printf("[WARING]:In %s (line:%d), Please Register UART_FILE_INTF!\n", __func__, __LINE__);

}

static void move_DAT_write(conf_object_t *opaque)
{
	loongson_uart_device *dev = opaque->obj;
	loongson_uart_reg_t *regs = dev->regs;
	if(dev->FCR_T.bits.txreset)
	{		
		regs->LSR_T.bits.TE = 0;
		regs->LSR_T.bits.TFE = 0;
		if(regs->IER_T.bits.ITE && regs->HR_T.bits.H == 1) //clear send state interrupt
			regs->HR_T.bits.INTPn = 1;
		send_to_term(opaque);
		regs->LSR_T.bits.TE = 1;
		regs->LSR_T.bits.TFE = 1;
		if(regs->IER_T.bits.ITE) //up a send interrupt
		{
			regs->HR_T.bits.H == 1;
			regs->HR_T.bits.INTPn = 0;
		}
	}

}

static exception_t loongson_uart_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	loongson_uart_device *dev = opaque->obj;
	loongson_uart_reg_t *regs = dev->regs;
	uint8_t val = *(uint8_t *)buf;
	switch (offset)
	{
		case 0x0:
		{
			
			if(! regs->LCR_T.bits.dlab)
			{
				move_DAT_write(opaque);		//write uart send data
				regs->DAT_T.all = val;
			}
			else
				dev->DL.DL_l = val;
			break;
		}
		case 0x1:
			if(! regs->LCR_T.bits.dlab)
				regs->IER_T.all = val;
			else
				dev->DL.DL_h = val;
			break;
		case 0x2:
		{
			if(! regs->LCR_T.bits.dlab)
			{
				regs->HR_T.all = val;
				dev->FCR_T.all = val;
				dev->recv_data_num = dev->FCR_T.bits.trigger;
			}
			else
				dev->DL.DL_d = val;
			break;
		}
		case 0x3:
			regs->LCR_T.all = val;
			break;
		case 0x4:
			regs->LSR_T.all = val;
			break;
		case 0x5:
			regs->MSR_T.all = val;
		case 0x6:
			regs->MSR_T.all = val;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in c687_uart\n", offset);
			break;
	}
	return 0;
}

static char* loongson_uart_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t loongson_uart_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	loongson_uart_device *dev = conf_obj->obj;
	loongson_uart_reg_t *regs = dev->regs;
	if(id == 0 || id == 1)	id = 0;
	if(id == 2 || id == 3)	id = 1;
	if(4 <= id <= 6)		id = 2;
	if(7 <= id <= 10)		id -= 4;
	*((uint8_t*)regs + id) = value;

	return No_exp;
}

static uint32_t loongson_uart_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(loongson_uart_reg_t);
}

static uint32_t loongson_uart_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static uint32_t loongson_uart_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	loongson_uart_device *dev = conf_obj->obj;
	loongson_uart_reg_t *regs = dev->regs;
	if(id == 0 || id == 1)	id = 0;
	if(id == 2 || id == 3)	id = 1;
	if(4 <= id <= 6)		id = 2;
	if(7 <= id <= 10)		id -= 4;
	uint8_t* regs_value = (uint8_t*)regs + id;
	return *regs_value;
}

static uint32_t loongson_uart_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(loongson_uart_reg_t);
	uint32_t i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_loongson_uart(const char *obj_name)
{
	loongson_uart_device* dev = skyeye_mm_zero(sizeof(loongson_uart_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->regs = skyeye_mm_zero(sizeof(loongson_uart_reg_t));
	loongson_uart_reg_t *regs = dev->regs;
	regs->HR_T.bits.INTPn = 1;
	regs->LSR_T.bits.TE = 1;
	regs->LSR_T.bits.TFE = 1;
	return dev->obj;
}

static exception_t free_loongson_uart(conf_object_t* opaque)
{
	loongson_uart_device *dev = opaque->obj;

	skyeye_free(opaque->objname);
	skyeye_free(dev->regs);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	loongson_uart_device *dev = obj->obj;
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

static exception_t uart_term_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	loongson_uart_device *dev = obj->obj;
	*obj2 = dev->term;
	*port = NULL;
	return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	loongson_uart_device *dev =  obj->obj;
	dev->uart_file = obj2;
	dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
	if (dev->uart_file_iface == NULL)
	{
		if (dev->uart_file == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", UART_FILE_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", UART_FILE_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	loongson_uart_device *dev =  obj->obj;
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}

void init_loongson_uart(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "loongson_uart",
		.class_desc = "loongson_uart",
		.new_instance = new_loongson_uart,
		.free_instance = free_loongson_uart,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = loongson_uart_read,
		.write = loongson_uart_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	
	static const skyeye_serial_device_t serial_device = {
		.write = serial_write,
		.receive_ready = NULL,
	};
	SKY_register_iface(clss, SERIAL_DEVICE_INTERFACE, &serial_device);
	
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = loongson_uart_get_regvalue_by_id,
		.get_regname_by_id = loongson_uart_get_regname_by_id,
		.set_regvalue_by_id = loongson_uart_set_regvalue_by_id,
		.get_regnum = loongson_uart_get_regnum,
		.get_regid_by_name = loongson_uart_get_regid_by_name,
		.get_regoffset_by_id = loongson_uart_get_regoffset_by_id
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription)
		{
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
		(struct InterfaceDescription) {
			.name = SERIAL_DEVICE_INTERFACE,
			.iface = &serial_device,
		},
	};
	static const struct ConnectDescription connects[] = 
	{
		(struct ConnectDescription ){
			.name = SKYEYE_UART_INTF,
			.set = uart_term_set,
			.get = uart_term_get,
		},
		(struct ConnectDescription ){
			.name = UART_FILE_INTF,
			.set = uart_file_set,
			.get = uart_file_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

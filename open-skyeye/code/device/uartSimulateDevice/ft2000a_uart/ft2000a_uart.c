/*************************************************************************
	> File Name    : ft2000a_uart.c
	> Author       : jiachao
	> Mail         : jiachao@163.com
	> Created Time : 2019/11/12 15:50:41
	> This is ft-2000A uart device
************************************************************************/

#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_attr.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#include <skyeye_system.h>
#include <skyeye_log.h>
#include "ft2000a_uart.h"

#ifdef __MINGW32__
#include <windows.h>
#endif

static void check_uart_interrupt(ft2000a_uart_device *dev)
{
	struct uart_registers *regs = &dev->regs;

	if(CHECK_BIT(regs->iir, IIR_NINT))
	{
		if(regs->ier.bit.ETBEI)
		{
			/* set iir with 0b0010 meaning uthr is empty */
			CLEAR_BIT(regs->iir, IIR_NON);
			SET_BIT(regs->iir, IIR_TE);
			/* thansmitter fifo and thr is empty */
			regs->lsr.bit.THRE = 1;
			regs->lsr.bit.TEMT = 1;
		}
	}

	if(!CHECK_BIT(regs->iir, IIR_NINT))
	{
		if(regs->ier.bit.ETBEI)
		{
			if(dev->signal.intf)
			{
				dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.num);
			}
		} else
		{
			if(dev->signal.intf)
			{
				dev->signal.intf->lower_signal(dev->signal.obj, dev->signal.num);
			}
		}
	} else
	{
		if(dev->signal.intf)
		{
			dev->signal.intf->lower_signal(dev->signal.obj, dev->signal.num);
		}
	}
}

static exception_t serial_write(conf_object_t *opaque, uint32_t value)
{
	ft2000a_uart_device *dev = (ft2000a_uart_device *)(opaque->obj);
	struct uart_registers *regs = &dev->regs;
	unsigned char buf = value;

	CLEAR_BIT(regs->iir, IIR_NON);
	SET_BIT(regs->iir, IIR_RDA);
	regs->lsr.bit.DR = 1;
	if (regs->fcr & FIFO_ENABLE)
	{
		if (buf == 127)
		{
			buf = '\b';
		} else if(buf == 10 || buf == 13)
		{
			buf = '\r';
		} else
		{
			//do nothing
		}
		regs->rbr = buf;
		if(dev->signal.intf)
		{
			dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.num);
		}
	}
	while (dev->isLock)
	{
#ifdef __MINGW32__
		Sleep(10);
#else
		usleep(10);
#endif
	}

	dev->isLock = 1;
}

void uart_tx_callback(conf_object_t* opaque)
{
	ft2000a_uart_device *dev = opaque->obj;
	struct uart_registers *regs = &dev->regs;

	if(regs->lsr.bit.TEMT)
	{
		check_uart_interrupt(dev);
    }
}

static exception_t ft2000a_uart_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_uart_device *dev = opaque->obj;
	struct uart_registers *regs = &dev->regs;
	uint32_t *data = (uint32_t *)buf;

	switch (offset)
	{
		case RBR:
			if (regs->lcr.bit.DLAB && regs->usr.bit.BUSY == 0)
				*data = regs->dll;
			else
			{
				*data = regs->rbr;
				/* clear lsr receive data ready */
				regs->lsr.bit.DR = 0;
				/* set iir no interrupt pending */
				CLEAR_BIT(regs->iir, IIR_NON);
				SET_BIT(regs->iir, IIR_NINT);
				dev->isLock = 0;
			}
			break;
		case DLH:
			if (regs->lcr.bit.DLAB && regs->usr.bit.BUSY == 0)
				*data = regs->dlh;
			else
				*data = regs->ier.all;
			break;
		case IIR:
			*data = regs->iir;
			if (regs->iir & IIR_TE)
			{
				/* set iir no interrupt pending */
				CLEAR_BIT(regs->iir, IIR_NON);
				SET_BIT(regs->iir, IIR_NINT);
			}
			break;
		case LCR:
			*data = regs->lcr.all;
			break;
		case LSR:
			*data = regs->lsr.all;
			break;
		case USR:
			*data = regs->usr.all;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in ft2000a_uart\n", offset);
			break;
	}

	return No_exp;
}

static exception_t ft2000a_uart_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_uart_device *dev = opaque->obj;
	struct uart_registers *regs = &dev->regs;
	uint8_t data = *(uint8_t *)buf;

	switch (offset)
	{
		case THR:
			if (regs->lcr.bit.DLAB && regs->usr.bit.BUSY == 0)
				regs->dll = data;
			else
			{
				dev->send_buf[dev->send_buf_index++] = data;
				if(dev->send_buf_index == 1 || data == '\n')
				{
					if(dev->term.obj)
					{
						dev->term.intf->write(dev->term.obj, dev->send_buf, dev->send_buf_index);
					}

					if (dev->uart_file)
					{
						dev->uart_file_iface->write(dev->uart_file, dev->send_buf, dev->send_buf_index);
					}
					memset(dev->send_buf, 0, 1024);
					dev->send_buf_index = 0;	
				}
				/* set TEMT and THRE bit */
				regs->lsr.bit.THRE = 1;
				regs->lsr.bit.TEMT = 1;
				//uart_tx_callback(opaque);
			}
			break;
		case IER:
			if (regs->lcr.bit.DLAB && regs->usr.bit.BUSY == 0)
				regs->dlh = data;
			else
				regs->ier.all = data;
			break;
		case FCR:
			regs->fcr = data;
			break;
		case LCR:
			regs->lcr.all = data;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in ft2000a_uart\n", offset);
			break;
	}
	return No_exp;
}

static char* ft2000a_uart_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t ft2000a_uart_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	ft2000a_uart_device *dev = conf_obj->obj;
	struct uart_registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t ft2000a_uart_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct uart_registers) / 4;
}

static uint32_t ft2000a_uart_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	ft2000a_uart_device *dev = conf_obj->obj;
	struct uart_registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t ft2000a_uart_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct uart_registers) / 4, i;
	for(i = 0; i < regnum; i++)
	{
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t ft2000a_uart_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

void init_uart_register(struct uart_registers *regs)
{
	regs->rbr = 0;
	regs->thr = 0;
	regs->dll = 0;
	regs->dlh = 0;
	regs->ier.all = 0;
	regs->iir = 1;
	regs->fcr = 0;
	regs->lcr.all = 0;
	regs->lsr.all = 0x60;
	regs->usr.all = 0x6;
}

static conf_object_t* new_ft2000a_uart(const char *obj_name)
{
	ft2000a_uart_device* dev = skyeye_mm_zero(sizeof(ft2000a_uart_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct uart_registers *regs = &(dev->regs);
	init_uart_register(regs);

	dev->isLock = 1;
	return dev->obj;
}

static exception_t free_ft2000a_uart(conf_object_t* opaque)
{
	ft2000a_uart_device *dev = opaque->obj;
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t config_ft2000a_uart(conf_object_t* opaque)
{
	ft2000a_uart_device *dev = opaque->obj;
	memset(dev->send_buf, 0, 1024);
	dev->send_buf_index = 0;
	system_register_timer_handler(opaque, 20, (time_sched_t)uart_tx_callback, SCHED_US|SCHED_NORMAL);
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	ft2000a_uart_device *dev =  conf_obj->obj;
	attr_value_t int_number_attr = SKY_make_attr_uinteger(dev->signal.num);
	return int_number_attr;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	ft2000a_uart_device *dev =  conf_obj->obj;
	dev->signal.num = SKY_attr_uinteger(*value);
	return No_exp;
}

static void ft2000a_uart_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of ft2000a uart");
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	ft2000a_uart_device *dev =  obj->obj;

	dev->signal.obj = obj2;
	dev->signal.intf = (general_signal_intf *)SKY_get_iface(dev->signal.obj, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal.intf == NULL)
	{
		if (dev->signal.obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);
		}

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	ft2000a_uart_device *dev =  obj->obj;
	*obj2 = dev->signal.obj;
	*port = NULL;
	return No_exp;
}

static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	ft2000a_uart_device *dev =  obj->obj;

	dev->term.obj = obj2;
	dev->term.intf = (skyeye_uart_intf *)SKY_get_iface(dev->term.obj, SKYEYE_UART_INTF);

	if (dev->term.intf == NULL)
	{
		if (dev->term.obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_UART_INTF);
		} else
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);
		}

		return Not_found_exp;
 	}

	return No_exp;
}

static exception_t uart_term_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	ft2000a_uart_device *dev =  obj->obj;
	*obj2 = dev->term.obj;
	*port = NULL;
	return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	ft2000a_uart_device *dev =  obj->obj;

	dev->uart_file = obj2;
	dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);

	if (dev->uart_file_iface == NULL)
	{
		if (dev->uart_file == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", UART_FILE_INTF);
		} else
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", UART_FILE_INTF, obj2->objname);
		}

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	ft2000a_uart_device *dev =  obj->obj;
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}

void init_ft2000a_uart(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "ft2000a_uart",
		.class_desc = "ft2000a_uart",
		.new_instance = new_ft2000a_uart,
		.free_instance = free_ft2000a_uart,
		.config_instance = config_ft2000a_uart
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = ft2000a_uart_read,
		.write = ft2000a_uart_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = ft2000a_uart_get_regvalue_by_id,
		.get_regname_by_id = ft2000a_uart_get_regname_by_id,
		.set_regvalue_by_id = ft2000a_uart_set_regvalue_by_id,
		.get_regnum = ft2000a_uart_get_regnum,
		.get_regid_by_name = ft2000a_uart_get_regid_by_name,
		.get_regoffset_by_id = ft2000a_uart_get_regoffset_by_id
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const skyeye_serial_device_t serial_device =
	{
		.write = serial_write,
		.receive_ready = NULL,
	};
	SKY_register_iface(clss, SERIAL_DEVICE_INTERFACE, &serial_device);

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
			.name = SERIAL_DEVICE_INTERFACE,
			.iface = &serial_device,
		}
	};

	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription )
		{
			.name = SKYEYE_UART_INTF,
			.set = uart_term_set,
			.get = uart_term_get,
		},
		(struct ConnectDescription )
		{
			.name = UART_FILE_INTF,
			.set = uart_file_set,
			.get = uart_file_get,
		},
		(struct ConnectDescription )
		{
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = signal_set,
			.get = signal_get,
		}
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	ft2000a_uart_register_attribute(clss);
}

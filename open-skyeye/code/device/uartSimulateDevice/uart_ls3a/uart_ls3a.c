/* Copyright (C) 
 * 2011 - Michael.Kang blackfin.kang@gmail.com
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */
/**
 * @file uart_ls3a.c
 * @brief ls3a uart module
 * @author Michael.Kang blackfin.kang@gmail.com
 * @version 0.1
 * @date 2011-07-13
 */
/* Description: update uart improvement from mpc8641d
 * author eJim Lee
 * date 2014-02-27
 */

#include <stdlib.h>
#include <assert.h>
#include <sched.h>

#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_mm.h>
#include <skyeye_attr.h>
#include <skyeye_signal.h>
#include <skyeye_core.h>
#include <skyeye_lock.h>
#include <skyeye_system.h>
#include <skyeye_sched.h>
#include <skyeye_system.h>

#include "uart_ls3a.h"

#if 0
#define UART_DBG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define UART_DBG(fmt, ...)
#endif

#define offset_err(info, offset, ...) \
	fprintf(stderr, "%s: Error offset 0x%x (" info ")\n", __func__, offset, ##__VA_ARGS__)

#define SET_BIT(reg, mask) ((reg) |= (mask))
#define CLR_BIT(reg, mask) ((reg) &= ~(mask))
#define CHK_BIT(reg, mask) ((reg) & (mask))

#define UART_IRQ 4

static void update_signal(uart_ls3a_t *dev)
{
	reg_ls3a_t *reg = &dev->reg;

	if(CHK_BIT(reg->ier, IER_ETHREI)) /* Check if transmit data interrupt is enabled */
	{
		if(CHK_BIT(reg->lsr, (LSR_THRE)))
		{
			/* set iir with 0b0010 meaning uthr is empty */
			SET_BIT(reg->iir, IIR_TE);
			CLR_BIT(reg->iir, IIR_NINT);
		}
	}

	if(CHK_BIT(reg->ier, IER_ERDAI))
	{
		//Check if receiv data interrupt is enabled
		if(CHK_BIT(reg->lsr, LSR_DR))
		{
			// if some data is already in RBR
			CLR_BIT(reg->iir, IIR_NINT); /* set uart interrupt flag */
			SET_BIT(reg->iir, IIR_RDA); /* set recei interrupt flag */
		}
	}
	/* some uart interrupt is happened */
	if(!CHK_BIT(reg->iir, IIR_NINT))
	{
		if(dev->signal.intf)
		{
			dev->signal.intf->signal(dev->signal.obj, NULL);
		}
	}
	return;
}

static exception_t serial_write(conf_object_t *opaque, uint32_t value)
{
	uart_ls3a_t *uart = (uart_ls3a_t *)(opaque->obj);
	reg_ls3a_t *reg = &uart->reg;
	exception_t ret = 0;
	unsigned char buf = value;

	SET_BIT(reg->lsr, LSR_DR);

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
	reg->rbr = buf;
	if(CHK_BIT(uart->reg.iir, IIR_NINT))
	{
		update_signal(uart);
	}

	return Access_exp;
}

static exception_t serial_ready(conf_object_t *obj)
{
	uart_ls3a_t *dev = obj->obj;

	return Access_exp;
}

void uart_tx_callback(conf_object_t* opaque)
{
	uart_ls3a_t *uart = (uart_ls3a_t *)(opaque->obj);
	reg_ls3a_t *reg = &uart->reg;

	if(CHK_BIT(reg->lsr, IIR_NON))
	{
		update_signal(uart);
	}
}

static exception_t uart_ls3a_read(conf_object_t *obj,generic_address_t offset, void* buf, size_t count)
{
	if(offset >= UART_ls3a_BANK_LEN)
	{
		offset_err("over bounds", offset);
		return Invarg_exp;
	}

	uart_ls3a_t *uart = (uart_ls3a_t *)(obj->obj);
	reg_ls3a_t *reg = &uart->reg;
	uint8 retval;

#define CASE_REG_OP(r) \
	case r##_off: \
				  retval = reg->r; \
	UART_DBG("[read] " #r ": 0x%hhx\n", retval); \
	break;

	switch(offset)
	{
		case rtd_off:
			if(CHK_BIT(reg->lcr, LCR_DLAB))
			{
				retval = reg->dlb;
				UART_DBG("[read] dlb: 0x%hhx\n", retval);
			} else
			{
				retval = reg->rbr;
				UART_DBG("[read] rbr: 0x%hhx\n", retval);
				/* clear lsr receive data ready */
				CLR_BIT(reg->lsr, LSR_DR);
			}
			break;
		case id_off:
			if(CHK_BIT(reg->lcr, LCR_DLAB))
			{
				retval = reg->dmb;
				UART_DBG("[read] dmb: 0x%hhx\n", retval);
			} else
			{
				retval = reg->ier;
				UART_DBG("[read] ier: 0x%hhx\n", retval);
			}
			break;
		case ifa_off:
			if(CHK_BIT(reg->lcr, LCR_DLAB))
			{
				retval = reg->afr;
				UART_DBG("[read] afr: 0x%hhx\n", retval);
			} else
			{
				retval = reg->iir;
				UART_DBG("[read] iir: 0x%hhx\n", retval);
				/* set iir no interrupt pending */
				CLR_BIT(reg->iir, IIR_NON);
				SET_BIT(reg->iir, IIR_NINT);
			}
			break;
		CASE_REG_OP(lcr)
		CASE_REG_OP(mcr)
		CASE_REG_OP(lsr)
		CASE_REG_OP(msr)
		CASE_REG_OP(scr)
		CASE_REG_OP(dsr)
		default:
			offset_err("over bounds", offset);
			return Invarg_exp;
	}
	*((uint8*) buf) = retval;

	return No_exp;
#undef CASE_REG_OP
}

static exception_t uart_ls3a_write(conf_object_t *obj, generic_address_t offset, const void *buf, size_t count)
{
	if(offset >= UART_ls3a_BANK_LEN)
	{
		offset_err("over bounds", offset);
		return Invarg_exp;
	}

	uart_ls3a_t *uart = (uart_ls3a_t *)(obj->obj);
	reg_ls3a_t *reg = &uart->reg;
	uint8 data = *((uint8*) buf);

#define CASE_REG_OP(r) \
	case r##_off: \
				  reg->r = data; \
	UART_DBG("[write] " #r ": 0x%hhx\n", data); \
	break;

	switch(offset)
	{
		case rtd_off:
			if(CHK_BIT(reg->lcr, LCR_DLAB))
			{
				reg->dlb = data;
				UART_DBG("[write] dlb: 0x%hhx\n", data);
			} else
			{
				UART_DBG("[write] thr: 0x%hhx\n", data);
				if(uart->term.obj)
				{
					uart->term.intf->write(uart->term.obj, &data, 1);
				}

				if (uart->uart_file)
				{
					uart->uart_file_iface->write(uart->uart_file, &data,1);
				}

				/* set TEMT and THRE bit */
				SET_BIT(reg->lsr, (LSR_TEMT | LSR_THRE));
			}
			break;
		case id_off:
			if(CHK_BIT(reg->lcr, LCR_DLAB))
			{
				reg->dmb = data;
				UART_DBG("[write] dmb: 0x%hhx\n", data);
			} else
			{
				reg->ier = data;
				UART_DBG("[write] ier: 0x%hhx\n", data);
			}
			break;
		case ifa_off:
			if(CHK_BIT(reg->lcr, LCR_DLAB))
			{
				reg->afr = data;
				UART_DBG("[write] afr: 0x%hhx\n", data);
			} else
			{
				reg->fcr = data;
				UART_DBG("[write] fcr: 0x%hhx\n", data);
			}
			break;
		CASE_REG_OP(lcr)
		CASE_REG_OP(mcr)
		CASE_REG_OP(lsr)
		CASE_REG_OP(msr)
		CASE_REG_OP(scr)
		CASE_REG_OP(dsr)
		default:
			offset_err("over bounds", offset);
			return Invarg_exp;
	}
	return No_exp;
#undef CASE_REG_OP
}

exception_t reset_uart_ls3a(conf_object_t* obj, const char *parameter)
{
	uart_ls3a_t *dev = (uart_ls3a_t*) obj->obj;
	reg_ls3a_t  *reg = &dev->reg;
	reg->rbr = 0x0;
	reg->thr = 0x0;
	/* FIXME: UDLB maybe should be 0x0 according to docs,
	 * but linux driver need a non-zero value */
	reg->dlb = 0x1;
	reg->ier = 0x0;
	reg->dmb = 0x0;
	reg->iir = 0x1;
	reg->fcr = 0x0;
	reg->afr = 0x0;
	reg->lcr = 0x0;
	reg->mcr = 0x0;
	reg->lsr = 0x60;
	reg->msr = 0x0;
	reg->scr = 0x0;
	reg->dsr = 0x1;

	// 设置默认中断号
	dev->signal.num = UART_IRQ;
	return No_exp;
}

static conf_object_t *create_ls3a_uart(const char *obj_name)
{
	uart_ls3a_t* uart = (uart_ls3a_t *)skyeye_mm_zero(sizeof(uart_ls3a_t));
	uart->obj = new_conf_object(obj_name, (void*)uart);
	reset_uart_ls3a(uart->obj, NULL);
	return uart->obj;
}

static exception_t config_ls3a_uart(conf_object_t *obj)
{
	int  pthread_id;
	int  pthread_id1;
	system_register_timer_handler(obj, 1000, (time_sched_t)uart_tx_callback, SCHED_US|SCHED_NORMAL);
	return No_exp;
}

static exception_t free_ls3a_uart(conf_object_t *obj)
{
	skyeye_free(obj->objname);
	skyeye_free(obj->obj);
	skyeye_free(obj);
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	uart_ls3a_t *dev =  conf_obj->obj;
	attr_value_t int_number_attr = SKY_make_attr_uinteger(dev->signal.num);
	return int_number_attr;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	uart_ls3a_t *dev =  conf_obj->obj;
	dev->signal.num = SKY_attr_uinteger(*value);
	return No_exp;
}

static void uart_ls3a_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of ls3a uart");
	return;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	uart_ls3a_t *dev =  obj->obj;
	dev->signal.obj = obj2;

	if (dev->signal.obj == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Get object failed!\n");
		return Not_found_exp;
	}

	dev->signal.intf = (core_signal_intf *)SKY_get_iface(dev->signal.obj, CORE_SIGNAL_INTF_NAME);
	if(dev->signal.intf == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	uart_ls3a_t *dev =  obj->obj;
	*obj2 = dev->signal.obj;
	*port = NULL;
	return No_exp;
}

static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	uart_ls3a_t *dev =  obj->obj;
	
	dev->term.obj = obj2;
	if (dev->term.obj == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Get object failed!\n");
		return Not_found_exp;
	}

	dev->term.intf = (skyeye_uart_intf *)SKY_get_iface(dev->term.obj, SKYEYE_UART_INTF);
	if(dev->term.intf == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from %s fail.", obj2->objname);
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t uart_term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	uart_ls3a_t *dev =  obj->obj;
	*obj2 = dev->term.obj;
	*port = NULL;
	return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	uart_ls3a_t *dev =  obj->obj;
	dev->uart_file = obj2;
	if (dev->uart_file == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Get object failed!\n");
		return Not_found_exp;
	}

	dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
	if (dev->uart_file_iface == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Get 'skyeye_uart_intf' interface from %s fail.", obj2->objname);
		return Not_found_exp;
	}
	return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	uart_ls3a_t *dev =  obj->obj;
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}

void init_ls3a_uart(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "ls3a_uart",
		.class_desc = "ls3a_uart",
		.new_instance = create_ls3a_uart,
		.free_instance = free_ls3a_uart,
		.reset_instance = reset_uart_ls3a,
		.config_instance = config_ls3a_uart,
		.set_attr = NULL,
		.get_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_mpc8378 SC_mpc8641d_mach SC_t2080),
	};
	REGS_NAME_INIT(UART_ls3a_REGS_LIST, uart_ls3a_reg_name);
	conf_class_t* class = SKY_register_device_class(class_data.class_name,&class_data);

	static const memory_space_intf io_memory =
	{
		.read = uart_ls3a_read,
		.write = uart_ls3a_write,
	};
	SKY_register_iface(class, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_serial_device_t serial_device =
	{
		.write = serial_write,
		.receive_ready = NULL,
	};
	SKY_register_iface(class, SERIAL_DEVICE_INTERFACE, &serial_device);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription)
		{
			.name = SERIAL_DEVICE_INTERFACE,
			.iface = &serial_device,
		}
	};
	static const struct ConnectDescription connects[] =
	{

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
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		}
	};
	class_register_ifaces(class, ifaces);
	class_register_connects(class, connects);

	uart_ls3a_register_attribute(class);
}


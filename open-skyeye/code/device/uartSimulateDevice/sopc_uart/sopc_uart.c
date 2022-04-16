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
#define DEBUG
#include <skyeye_log.h>
#include "sopc_fifo.h"
#include "sopc_uart.h"

static void update_signal(sopc_uart_device *uart)
{
	struct registers *reg = &uart->regs;

	if(CHK_BIT(reg->iir, IIR_NINT)) {
		if(CHK_BIT(reg->ier, IER_ETHREI)) {
			/* set iir with 0b0010 meaning uthr is empty */
			CLR_BIT(reg->iir, IIR_NON);
			SET_BIT(reg->iir, IIR_TE);
			/* thansmitter fifo and thr is empty */
			SET_BIT(reg->lsr, (LSR_TEMT | LSR_THRE));
		}
	}
	if(!CHK_BIT(reg->iir, IIR_NINT)) {
		if(CHK_BIT(reg->ier, IER_ETHREI)) {
			if(uart->signal.obj)
				uart->signal.intf->raise_signal(uart->signal.obj, uart->signal.num);
		}else{
			if(uart->signal.obj)
				uart->signal.intf->lower_signal(uart->signal.obj, uart->signal.num);
		}
	}else{
		if(uart->signal.obj)
			uart->signal.intf->lower_signal(uart->signal.obj, uart->signal.num);
	}
}

void uart_tx_callback(conf_object_t* opaque){
	sopc_uart_device *uart = (sopc_uart_device *)(opaque->obj);
	struct registers *regs = &uart->regs;

	if(CHK_BIT(regs->lsr, LSR_TEMT)){
		update_signal(uart);
	}
}

void uart_rx_callback(conf_object_t* opaque)
{
	sopc_uart_device *uart = (sopc_uart_device *)(opaque->obj);
	struct registers *reg = &uart->regs;
	exception_t ret = 0;
	unsigned char buf;
	if (uart->term.intf != NULL){
		if ((ret = (uart->term.intf->read(uart->term.obj, &buf, 1)) != 0)) {
			CLR_BIT(reg->iir, IIR_NON);
			SET_BIT(reg->iir, IIR_RDA);
			SET_BIT(reg->lsr, LSR_DR);
			if (reg->fcr & 1){
				if (buf == 127)
					buf = '\b';
				reg->rbr = buf;
				if(uart->signal.obj)
					uart->signal.intf->raise_signal(uart->signal.obj, uart->signal.num);
			}
		}
	}
}

static exception_t serial_write(conf_object_t *opaque, uint32_t value)
{
	sopc_uart_device *uart = (sopc_uart_device *)(opaque->obj);
	struct registers *reg = &uart->regs;
	exception_t ret = 0;
	unsigned char buf = value;
	CLR_BIT(reg->iir, IIR_NON);
	SET_BIT(reg->iir, IIR_RDA);
	SET_BIT(reg->lsr, LSR_DR);
	if (reg->fcr & 1) {
		if (buf == 127)
			buf = '\b';
		else if (buf == 10 || buf == 13) //new line
		{
			buf = '\r'; // carriage return
		}
		reg->rbr = buf;
		reg->lsr |= LSR_DR;
		if(uart->signal.obj) {
			uart->signal.intf->raise_signal(uart->signal.obj, uart->signal.num);
		}
	} else
	{
		if (reg->uart_ctl.rx_en)
		{
			uart->rx_fifo_cnt ++;
			if (buf == 127)
				buf = '\b'; //new line
			else if (buf == 10)
			{
				buf = '\r'; // carriage return
			}
			WriteFIFO(uart->recv_fifo, &buf, 1);
			reg->lsr |= LSR_DR;
			if (uart->rx_fifo_cnt > reg->uart_ctl.rx_threshold)
			{
				uart->rx_fifo_cnt = 0;
				if (reg->uart_ctl.rx_int_en)
				{
					if(uart->signal.obj)
						uart->signal.intf->raise_signal(uart->signal.obj, uart->signal.num);
				}
			}
		}
	}

	return Access_exp;
}

static exception_t sopc_uart_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_uart_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;

	switch (offset)
	{
		case RBR:
			if(CHK_BIT(regs->lcr, LCR_DLAB))
			{
				*(uint32_t *)buf = regs->dll;
			} else
			{
				/* clear lsr receive data ready */
				CLR_BIT(regs->lsr, LSR_DR);
				if (CheckFIFOReadNum(dev->recv_fifo) > 0)
					ReadFIFO(dev->recv_fifo, buf, 1);
				else
					*(uint32_t *)buf = regs->rbr;
				/* set iir no interrupt pending */
				CLR_BIT(regs->iir, IIR_NON);
				SET_BIT(regs->iir, IIR_NINT);
			}
			break;
		case IER:
			if(CHK_BIT(regs->lcr, LCR_DLAB))
			{
				*(uint32_t *)buf = regs->dlh;
			} else
			{
				*(uint32_t *)buf = regs->ier;
			}
			break;
		case IIR:
			*(uint32_t *)buf = regs->iir;
			if(CHK_BIT(regs->iir, IIR_TE)) {
				/* set iir no interrupt pending */
				CLR_BIT(regs->iir, IIR_NON);
				SET_BIT(regs->iir, IIR_NINT);
			}
			break;
		case LCR:
			*(uint32_t *)buf = regs->lcr;
			break;
		case LSR:
			*(uint32_t *)buf = regs->lsr;
			break;
		case USR:
			*(uint32_t *)buf = regs->usr;
			break;
		case TFL:
			*(uint32_t *)buf = regs->tfl;
			break;
		case RFL:
			*(uint32_t *)buf = regs->rfl;
			break;
		case UART_RB:
			ReadFIFO(dev->recv_fifo, buf, 1);
			break;
		case UART_ST:
			*(uint32_t *)buf = regs->uart_st;
			break;
		case UART_CTL:
			*(uint32_t *)buf = regs->uart_ctl.all;
			break;
		case UART_BAUD:
			*(uint32_t *)buf = regs->uart_baud;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sopc_uart\n", offset);
			return Not_found_exp;
	}
	return No_exp;
}

void tx_interrupt_check(conf_object_t *opaque)
{
	sopc_uart_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;

	if (dev->tx_fifo_cnt > regs->uart_ctl.tx_threshold)
	{
		dev->tx_fifo_cnt = 0;
		if (regs->uart_ctl.tx_int_en)
		{
			if(dev->signal.obj)
				dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.num);
		}
	}
}

static exception_t sopc_uart_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_uart_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	uint32_t val = *(uint32_t *)buf;

	switch (offset)
	{
		case THR:
			if (CHK_BIT(regs->lcr, LCR_DLAB))
			{
				regs->dll = val;
			} else
			{
				regs->thr = val;
				if (dev->term.intf)
				{
					dev->term.intf->write(dev->term.obj, &val, 1);
				}
				if (dev->uart_file.intf)
				{
					dev->uart_file.intf->write(dev->uart_file.obj, &val, 1);
				}
			}
			break;
		case IER:
			if (CHK_BIT(regs->lcr, LCR_DLAB))
			{
				regs->dlh = val;
			} else
			{
				regs->ier = val;
			}
			break;
		case FCR:
			regs->fcr = val;
			break;
		case LCR:
			regs->lcr = val;
			break;
		case LSR:
			regs->lsr = val;
			break;
		case USR:
			regs->usr = val;
			break;
		case TFL:
			regs->tfl = val;
			break;
		case RFL:
			regs->rfl = val;
			break;
		case UART_TB:
			dev->tx_fifo_cnt ++;
			if (regs->uart_ctl.tx_en)
			{
				tx_interrupt_check(opaque);
				if (dev->term.intf)
				{
					dev->term.intf->write(dev->term.obj, &val, 1);
				}
				if (dev->uart_file.intf)
				{
					dev->uart_file.intf->write(dev->uart_file.obj, &val, 1);
				}
			}
			break;
		case UART_ST:
			regs->uart_st = val;
			break;
		case UART_CTL:
			regs->uart_ctl.all = val;
			break;
		case UART_BAUD:
			regs->uart_baud = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in sopc_uart\n", offset);
			return Not_found_exp;
	}

	return No_exp;
}

static char* sopc_uart_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t sopc_uart_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	sopc_uart_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t sopc_uart_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t sopc_uart_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	sopc_uart_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t sopc_uart_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t sopc_uart_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static init_uart_register(sopc_uart_device *dev)
{
	struct registers *regs = &(dev->regs);
	regs->rbr = 0;
	regs->thr = 0;
	regs->dll = 0;
	regs->dlh = 0;
	regs->iir = 1;
	regs->fcr = 0;
	regs->lcr = 0;
	regs->lsr = 0x60;
	regs->usr = 6;
	regs->tfl = 0;
	regs->rfl = 0;
}

static conf_object_t* new_sopc_uart(const char *obj_name)
{
	sopc_uart_device* dev = skyeye_mm_zero(sizeof(sopc_uart_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	init_uart_register(dev);
	dev->recv_fifo = CreateFIFO(2048);
	return dev->obj;
}

static exception_t config_sopc_uart(conf_object_t* opaque)
{
	system_register_timer_handler(opaque, 1000, (time_sched_t)uart_tx_callback, SCHED_US|SCHED_NORMAL);
	//system_register_timer_handler(opaque, 2000, (time_sched_t)uart_rx_callback, SCHED_US|SCHED_NORMAL);

	return No_exp;
}

static exception_t free_sopc_uart(conf_object_t* opaque)
{
	sopc_uart_device *dev =  opaque->obj;
	skyeye_free(opaque->objname);
	skyeye_free(dev->recv_fifo);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sopc_uart_device *dev =  obj->obj;
	dev->term.obj = obj2;
	dev->term.intf = (skyeye_uart_intf *)SKY_get_iface(dev->term.obj, SKYEYE_UART_INTF);

	if (dev->term.intf == NULL){
		if (dev->term.obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_UART_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t uart_term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	sopc_uart_device *dev =  obj->obj;
	*obj2 = dev->term.obj;
	*port = NULL;
	return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sopc_uart_device *dev =  obj->obj;
	dev->uart_file.obj = obj2;
	dev->uart_file.intf = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
	if (dev->uart_file.intf == NULL){
		if (dev->uart_file.obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", UART_FILE_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", UART_FILE_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	sopc_uart_device *dev =  obj->obj;
	*obj2 = dev->uart_file.obj;
	*port = NULL;
	return No_exp;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sopc_uart_device *dev =  obj->obj;
	dev->signal.obj = obj2;
	dev->signal.intf = (general_signal_intf *)SKY_get_iface(dev->signal.obj, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal.intf == NULL){
		if (dev->signal.obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	sopc_uart_device *dev =  obj->obj;
	*obj2 = dev->signal.obj;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	sopc_uart_device *dev =  conf_obj->obj;
	attr_value_t int_number_attr = SKY_make_attr_uinteger(dev->signal.num);
	return int_number_attr;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	sopc_uart_device *dev =  conf_obj->obj;
	dev->signal.num = SKY_attr_uinteger(*value);
	return No_exp;
}

static void sopc_uart_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of sopc uart");
	return;
}

void init_sopc_uart(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "sopc_uart",
		.class_desc = "sopc_uart",
		.new_instance = new_sopc_uart,
		.free_instance = free_sopc_uart,
		.config_instance = config_sopc_uart
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = sopc_uart_read,
		.write = sopc_uart_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = sopc_uart_get_regvalue_by_id,
		.get_regname_by_id = sopc_uart_get_regname_by_id,
		.set_regvalue_by_id = sopc_uart_set_regvalue_by_id,
		.get_regnum = sopc_uart_get_regnum,
		.get_regid_by_name = sopc_uart_get_regid_by_name,
		.get_regoffset_by_id = sopc_uart_get_regoffset_by_id
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const skyeye_serial_device_t serial_device = {
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
		(struct InterfaceDescription) {
			.name = SERIAL_DEVICE_INTERFACE,
			.iface = &serial_device,
		}
	};

	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
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
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = signal_set,
			.get = signal_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	sopc_uart_register_attribute(clss);
}

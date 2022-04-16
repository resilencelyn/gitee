/* Copyright (C)
* 2014 - Michael.Kang blackfin.kang@gmail.com
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
* @file uart_am3359.c
* @brief
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-12-04
*/

#include <stdio.h>
#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"

#include "uart_am3359.h"
#include "am3359_fifo.h"


static void init_regs(conf_object_t *opaque);

static exception_t serial_write(conf_object_t *obj, uint32_t value)
{
	am3359_uart_dev *dev = obj->obj;
	struct registers* regs = &dev->regs;
	unsigned char buf = (uint8_t)value;

	if ((regs->lsr & 0x1) == 0x0)
	{
		if(dev->read_fifo)
			WriteFIFO(dev->read_fifo, &buf, 1);
		else
			regs->rhr = buf;

		regs->lsr |= 1;
		regs->iir &= ~0x3e;
		regs->iir |= 2 << 1;
		regs->iir &= ~1;

		if(dev->signal_iface != NULL) {
			dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);
		}
	}
	return No_exp;
}

static exception_t uart_am3359_read(conf_object_t* opaque, generic_address_t offset,
		void *buf,  size_t count)
{
	am3359_uart_dev *dev = opaque->obj;
	uint16_t *val = (uint16_t *)buf;
	uint8_t ch;

	switch(offset) {
		case RHR:
			if (IS_OP_MODE) {
				if (dev->read_fifo) {
					if (CheckCanReadNum(dev->read_fifo) > 0) {
						ReadFIFO(dev->read_fifo, &ch, 1);
						*val = ch;
					}
				} else {
					*val = dev->regs.rhr;
				}
				dev->regs.iir |= 1;
				dev->regs.iir &= ~0x3e;
				dev->regs.lsr = SET_BIT(0, dev->regs.lsr, 0);

				if (dev->regs.ier & 0x2) {
					dev->regs.iir &= ~0x3e;
					dev->regs.iir |= 1 << 1;
					dev->regs.iir &= ~1;
					dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);
				}
			} else if (IS_MODE_A) {

			} else if (IS_MODE_B) {

			}
			break;
		case IER:
			if (IS_OP_MODE) {
				*val = dev->regs.ier;
			} else if (IS_MODE_A) {
				*val = dev->regs.dlh;
			} else if (IS_MODE_B) {
				*val = dev->regs.dlh;
			}
			break;
		case IIR:
			if (IS_OP_MODE) {
				/* operational mode UART_IIR register read */
				*val = dev->regs.iir;
			} else if (IS_MODE_A) {
				/* MODE A UART_IIR register read */
				*val = dev->regs.iir;
			} else if (IS_MODE_B) {
				/* MODE B UART_EFR register read */
				*val = dev->regs.efr;
			}
			dev->regs.iir &= ~0x3e;
			dev->regs.iir |= 1;
		       	break;
		case LCR:
			*val = dev->regs.lcr;
		       	break;
		case MCR:
			*val = dev->regs.mcr;
                       	break;
		case LSR:
			*val = dev->regs.lsr;
                       	break;
                case MSR:
			/* Transmission control register (TCR) are accessible only when EFR[4] = 1 and MCR[6] = 1 */
			if (TCR_TLR_ENABLE) {
				/* all mode UART_TCR read */
				*val = dev->regs.tcr;
			} else {
				if (IS_OP_MODE | IS_MODE_A) {
					/* operational mode and MODE A UART_MSR read */
					*val = dev->regs.msr;
				} else if (IS_MODE_B) {
					/* MODE B UART_XOFF1 read */
					*val = dev->regs.xoff1;
				}
			}
                       	break;
		case SPR:
			/* trigger level register (TLR) are accessible only when EFR[4] = 1 and MCR[6] = 1 */
			if (TCR_TLR_ENABLE) {
				/* all mode UART_TLR read */
				*val = dev->regs.tlr;
			} else {
				if (IS_OP_MODE | IS_MODE_A) {
					/* operational mode and MODE A UART_SPR read */
					*val = dev->regs.spr;
				} else if(IS_MODE_B) {
					/* MODE B UART_XOFF2 read */
					*val = dev->regs.xoff2;
				}
			}
                       	break;
                case MDR1:
			*val = dev->regs.mdr1;
                       	break;
                case MDR2:
			*val = dev->regs.mdr2;
                       	break;
                case SFLSR:
			*val = dev->regs.sflsr;
                       	break;
		case RESUME:
			*val = dev->regs.resume;
	               	break;
                case SFREGL:
			*val = dev->regs.sfregl;
                       	break;
                case SFREGH:
			*val = dev->regs.sfregh;
                       	break;
                case BLR:
			*val = dev->regs.sfregh;
                       	break;
                case ACREG:
			*val = dev->regs.acreg;
                       	break;
                case SCR:
			*val = dev->regs.scr;
                       	break;
                case SSR:
			*val = dev->regs.ssr;
                       	break;
                case EBLR:
			*val = dev->regs.eblr;
                       	break;
                case MVR:
			*val = dev->regs.mvr;
                       	break;
		case SYSC:
			*val = dev->regs.sysc;
                       	break;
		case SYSS:
			*val = dev->regs.syss;
                       	break;
		case WER:
			*val = dev->regs.wer;
                       	break;
                case CFPS:
			*val = dev->regs.cfps;
                       	break;
                case RXFIFO_LVL:
			*val = dev->regs.rxfifo_lvl;
                       	break;
		case TXFIFO_LVL:
			*val = dev->regs.txfifo_lvl;
                       	break;
                case IER2:
			*val = dev->regs.ier2;
                       	break;
                case ISR2:
			*val = dev->regs.isr2;
                       	break;
                case MDR3:
			*val = dev->regs.mdr3;
                       	break;
                case TXDMA:
			*val = dev->regs.txdma;
                       	break;
		default:
			skyeye_log(Warning_log, __FUNCTION__, "Can not read the register at 0x%x in %s\n", offset, __FILE__);
			break;
	}

	return  No_exp;
}
static exception_t uart_am3359_write(conf_object_t* opaque, generic_address_t offset,
		void *buf,  size_t count)
{
	am3359_uart_dev *dev = opaque->obj;
	uint16_t val = *(uint16_t *)buf, c;

	switch(offset) {
		case THR:
			dev->regs.thr = val;
			dev->regs.lsr &= ~0x60;
			dev->regs.lsr |= 0x60;

			c = *(uint16_t *)buf;
			if (dev->term_iface != NULL)
				dev->term_iface->write(dev->term, &c, 2);
			if (dev->uart_file !=NULL)
				dev->uart_file_iface->write(dev->uart_file, &c, 2);
#if 0
			if (dev->regs.ier & 0x2) {
				dev->regs.iir &= ~0x3e;
				dev->regs.iir |= 1 << 1;
				dev->regs.iir &= ~1;
				dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);
			}
#endif
			break;
		case IER:/* IER, DLH, DLL */
			/* IER[7:4] can only be written when EFR[4] = 1 */
			if ((dev->regs.efr >> 4) & 1) {
				/* operational mode ier write */
				dev->regs.ier = val & (~0xf);
			} else {
				/* operational mode ier write */
				dev->regs.ier = val & (~(0xf << 4));
			}
			break;
		case LCR:
			dev->regs.lcr = val;
		       	break;
		case MCR:
			dev->regs.mcr = val;
                       	break;
		case LSR:
			dev->regs.lsr = val;
                       	break;
                case MSR:/* MSR, TCR, XOFF1 */
			/* Transmission control register (TCR) are accessible only when EFR[4] = 1 and MCR[6] = 1 */
			if (TCR_TLR_ENABLE) {
				/* all mode UART_TCR write */
				dev->regs.tcr = val;
			} else {
				if (IS_MODE_B) {
					/* MODE B UART_xoff1 write */
					dev->regs.xoff1 = val;
				}
			}
                       	break;
		case SPR: /* SPR, TLR, XOFF2 */
			/* trigger level register (TLR) are accessible only when EFR[4] = 1 and MCR[6] = 1 */
			if (TCR_TLR_ENABLE) {
				/* all mode UART_TLR write */
				dev->regs.tlr = val;
			} else {
				if (IS_OP_MODE | IS_MODE_A) {
					/* operational mode UART_SPR write */
					dev->regs.spr = val;
				} else if (IS_MODE_B) {
					/* MODE B UART_xoff2 write */
					dev->regs.xoff2 = val;
				}
			}
                       	break;
                case MDR1:
			dev->regs.mdr1 = val;
                       	break;
                case MDR2:
			dev->regs.mdr2 = val;
                       	break;
                case SFLSR:
			dev->regs.sflsr = val;
                       	break;
		case RESUME:
			dev->regs.resume = val;
	               	break;
                case SFREGL:
			dev->regs.sfregl = val;
                       	break;
                case SFREGH:
			dev->regs.sfregh = val;
                       	break;
                case BLR:
			dev->regs.sfregh = val;
                       	break;
                case ACREG:
			dev->regs.acreg = val;
                       	break;
                case SCR:
			dev->regs.scr = val;
                       	break;
                case SSR:
			dev->regs.ssr = val;
                       	break;
                case EBLR:
			dev->regs.eblr = val;
                       	break;
                case MVR:
			dev->regs.mvr = val;
                       	break;
		case SYSC:
			dev->regs.sysc = val;
			if (dev->regs.sysc & 0x2) {
				init_regs(opaque);
				dev->regs.syss |= 0x1;
			}
                       	break;
		case WER:
			dev->regs.wer = val;
                       	break;
                case CFPS:
			dev->regs.cfps = val;
                       	break;
                case RXFIFO_LVL:
			dev->regs.rxfifo_lvl = val;
                       	break;
		case TXFIFO_LVL:
			dev->regs.txfifo_lvl = val;
                       	break;
                case IER2:
			dev->regs.ier2 = val;
                       	break;
                case ISR2:
			dev->regs.isr2 = val;
                       	break;
                case MDR3:
			dev->regs.mdr3 = val;
                       	break;
                case TXDMA:
			dev->regs.txdma = val;
                       	break;
		case IIR:
			/////////not fix///////////////////
			dev->regs.iir = val;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__, "Can not write the register at 0x%x in %s\n", offset, __FILE__);
			break;
	}
	return  No_exp;
}
const char *uart_am3359_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	am3359_uart_dev *dev= conf_obj->obj;
	return regs_name[id];
}

uint32_t uart_am3359_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	am3359_uart_dev *dev= conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(&dev->regs) + id;
	return *regs_value;
}

static exception_t uart_am3359_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	am3359_uart_dev *dev = conf_obj->obj;
	struct registers *regs = &dev->regs;
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t uart_am3359_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	am3359_uart_dev *dev = conf_obj->obj;
	return  sizeof(dev->regs) / 4;
}

static int32_t uart_am3359_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	am3359_uart_dev *dev = conf_obj->obj;
	uint32_t regnum = sizeof(dev->regs) / 4;
	int i;
	for(i =0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static exception_t am3359_uart_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	am3359_uart_dev *dev = opaque->obj;

	if (!strncmp(attr_name, "term", strlen("term"))) {
		dev->term = (skyeye_uart_intf *)SKY_get_iface(value->u.object, SKYEYE_UART_INTF);
	} else if (!strncmp(attr_name, "signal", strlen("signal"))) {
		dev->signal = (general_signal_intf *)SKY_get_iface(value->u.object,
				GENERAL_SIGNAL_INTF_NAME);
	} else {
		printf("parameter error\n");
		return Invarg_exp;
	}
	return No_exp;
}
static exception_t term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	am3359_uart_dev *dev = obj->obj;
	dev->term = obj2;
	dev->term_iface = (memory_space_intf*)SKY_get_iface(dev->term, SKYEYE_UART_INTF);

	if (dev->term_iface == NULL){
		if (dev->term == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_UART_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	am3359_uart_dev *dev = obj->obj;
	*obj2 = dev->term;
	*port = NULL;
	return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	am3359_uart_dev *dev = obj->obj;
	dev->uart_file = obj2;
	dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
	if (dev->uart_file_iface == NULL){
		if (dev->uart_file == NULL)
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
	am3359_uart_dev *dev = obj->obj;
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	am3359_uart_dev *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf **)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	am3359_uart_dev *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	am3359_uart_dev *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	am3359_uart_dev *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* idx) {
	am3359_uart_dev *dev = conf_obj->obj;
	return SKY_make_attr_object(dev->signal);
}

static exception_t set_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx) {
	unsigned int length;
	am3359_uart_dev *dev = conf_obj->obj;
	length = SKY_attr_uinteger(*value);
	dev->read_fifo = CreateFIFO(length);
	if(dev->read_fifo == NULL)
		skyeye_log(Error_log, __FUNCTION__, "Create Read Fifo Fail.");
	return No_exp;
}

static void am3359_uart_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "rfifo", get_attr_rfifo, NULL, set_attr_rfifo, NULL, SKY_Attr_Optional, "uinteger", "fifo length for am3359 uart");
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of systimer");
}

static void init_regs(conf_object_t *opaque)
{
	am3359_uart_dev *dev = opaque->obj;

	memset(&dev->regs, 0, sizeof(dev->regs));

	dev->regs.iir 	= 0x01;
	dev->regs.lsr	= 0x60;
	dev->regs.mdr1 	= 0x07;
	dev->regs.blr 	= 0x40;
	dev->regs.ssr 	= 0x04;
	dev->regs.wer 	= 0xff;
	dev->regs.cfps 	= 0x69;
}
static conf_object_t* new_am3359_uart(char* obj_name)
{
	am3359_uart_dev* dev = skyeye_mm_zero(sizeof(am3359_uart_dev));
	dev->obj = new_conf_object(obj_name, dev);

	dev->regs.iir 	= 0x01;
	dev->regs.lsr	= 0x60;
	dev->regs.mdr1 	= 0x07;
	dev->regs.blr 	= 0x40;
	dev->regs.ssr 	= 0x04;
	dev->regs.wer 	= 0xff;
	dev->regs.cfps 	= 0x69;

	return dev->obj;
}
static exception_t reset_am3359_uart(conf_object_t* opaque, const char* args)
{
	init_regs(opaque);
	return No_exp;
}
static exception_t free_am3359_uart(conf_object_t *opaque) {
	am3359_uart_dev *dev = opaque->obj;

	if (dev->read_fifo)
		FreeFIFO(dev->read_fifo);

	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}
void init_am3359_uart()
{
	static skyeye_class_t class_data = {
		.class_name = "am3359_uart",
		.class_desc = "am3359_uart",
		.new_instance = new_am3359_uart,
		.reset_instance = reset_am3359_uart,
		.free_instance = free_am3359_uart,
		.get_attr = NULL,
		.set_attr = am3359_uart_set_attr,
		.module_type = SKYML_ONCHIP(SC_am335x SC_am3359),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = uart_am3359_read,
		.write = uart_am3359_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = uart_am3359_get_regvalue_by_id,
		.get_regname_by_id = uart_am3359_get_regname_by_id,
		.set_regvalue_by_id = uart_am3359_set_regvalue_by_id,
		.get_regnum = uart_am3359_get_regnum,
		.get_regid_by_name = uart_am3359_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const skyeye_serial_device_t serial_device = {
		.write = serial_write,
		.receive_ready = NULL,
		.write_chars = NULL,
	};
	SKY_register_iface(clss, SERIAL_DEVICE_INTERFACE, &serial_device);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
		(struct InterfaceDescription) {
			.name = SERIAL_DEVICE_INTERFACE,
			.iface = &serial_device,
		},
 	};
	static const struct ConnectDescription connects[] = {
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
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	am3359_uart_register_attribute(clss);
}

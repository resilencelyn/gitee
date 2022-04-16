/* Copyright (C) 
 * 
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
 * @file sci_28335.c
 * @brief The implementation of system controller
 * @author Kewei Yu keweihk@gmail.com
 * @version 78.77
 */

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_uart_ops.h>
#include <skyeye_iface.h>
#include <skyeye_emif.h>
#define DEBUG
#include <skyeye_log.h>

#include "sci_28335.h"
#include "sci_fifo.h"

void send_data_to_stimulate(sci_28335_device *dev, int length);

static void sci_interrupt_callback(conf_object_t *obj){
	sci_28335_device *dev = obj->obj;
	sci_28335_reg_t* regs = dev->regs;

	if (TX_CNT <= regs->scifftx.bit.TXFFIL && regs->scifftx.bit.SCIFFENA && regs->scifftx.bit.TXFFIENA){
		//raise spitxfifo interrupt
		if (dev->signal_iface){
			dev->signal_iface->raise_signal(dev->signal, dev->Tx_int_number);
		}
	}	
}

static void dmc_receive(conf_object_t *obj, void *buf){
	sci_28335_device *dev = obj->obj;
	sci_28335_reg_t* regs = dev->regs;
	unsigned char rx_buf = *(unsigned char *)buf;
	regs->scirxbuf.bit.RXDT = rx_buf;
	WriteFIFO(dev->recv_fifo, &rx_buf, 1);


	regs->scirxst.bit.RXRDY = 1;
	if ((rx_buf == 'a' || rx_buf == 'A') && dev->first_data == 0){
		regs->sciffct.bit.ABD = 1;
	}
	dev->first_data ++;
	//raise an receive interrupt  
	if (dev->signal_iface)
		dev->signal_iface->raise_signal(dev->signal, dev->Rx_int_number);

	return 0;
}

static void dmc_raise_signal(conf_object_t *obj, int irq_number){
	sci_28335_device *dev = obj->obj;
	if (dev->signal_iface){
		dev->signal_iface->raise_signal(dev->signal, irq_number);
	}
}

static exception_t sci_28335_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct sci_28335_device *dev = opaque->obj;
	sci_28335_reg_t* regs = dev->regs;
	int tmp_cnt, rxffil;
	switch(offset)
	{
		case SCICCR:
			*(uint16_t *)buf = regs->sciccr.all;
			break;
		case SCICTL1:
			*(uint16_t *)buf = regs->scictl1.all;
			break;
		case SCIHBAUD:
			*(uint16_t *)buf = regs->scihbaud;
			break;
		case SCILBAUD:
			*(uint16_t *)buf = regs->scilbaud;
			break;
		case SCICTL2:
			*(uint16_t *)buf = regs->scictl2.all;
			break;
		case SCIRXST:
			*(uint16_t *)buf = regs->scirxst.all;
			break;
		case SCIRXEMU:
			*(uint16_t *)buf = regs->scirxemu;
			break;
		case SCIRXBUF:
			rxffil = regs->sciffrx.bit.RXFFIL & 0xF;
			unsigned char recv_ch;
			if ((regs->sciffrx.bit.RXFFIL > 0) && (rxffil != 0xF)){
				tmp_cnt = regs->sciffrx.bit.RXFFIL;
				//*(uint16_t *)buf = dev->rx_fifo[tmp_cnt - RX_CNT];
				RW_RDLOCK(dev->lock);
				ReadFIFO(dev->recv_fifo, &recv_ch, 1);
				*(uint16_t *)buf = recv_ch;
				RX_CNT -= 1;
				RW_UNLOCK(dev->lock);
			}else { 
				//*(uint16_t *)buf = regs->scirxbuf.all;
				RW_RDLOCK(dev->lock);
				ReadFIFO(dev->recv_fifo, &recv_ch, 1);
				*(uint16_t *)buf = recv_ch;
				RW_UNLOCK(dev->lock);
			}

			regs->scirxst.bit.RXRDY = 0;
			break;
		case SCITXBUF:
			*(uint16_t *)buf = regs->scitxbuf;
			break;
		case SCIFFTX:
			*(uint16_t *)buf = regs->scifftx.all;
			break;
		case SCIFFRX:
			*(uint16_t *)buf = regs->sciffrx.all;
			break;
		case SCIFFCT:
			*(uint16_t *)buf = regs->sciffct.all;
			break;
		case SCIPRI:
			*(uint16_t *)buf = regs->scipri.all;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "read error register offset %d\n", offset);
			break;
	}

	return No_exp;
}

static exception_t sci_28335_read_hl(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count, int hl_flag)
{
	exception_t ret;
	ret = sci_28335_read(opaque, offset, buf, count);
	uint16_t data = *(uint16_t*)buf;
	if (hl_flag){
		*(uint8_t *)buf = data >> 8;
	}else {
		*(uint8_t *)buf = data & 0xFF;
	}

	return ret;
}

static int recv_data_from_stimulate(conf_object_t *obj, unsigned char *buf, int length){
	sci_28335_device *dev = obj->obj;
	sci_28335_reg_t* regs = dev->regs;
	int i;

	if (buf[0] == dev->Sci_ID){
		for(i = 0; i < length; i++){
			RW_WRLOCK(dev->lock);
			WriteFIFO(dev->recv_fifo, &buf[i+1], 1);
			regs->scirxst.bit.RXRDY = 1;
			if (dev->signal_iface)
				dev->signal_iface->raise_signal(dev->signal, dev->Rx_int_number);
			RW_UNLOCK(dev->lock);
		}
	}
	return 0;
}

void send_data_to_stimulate(sci_28335_device* dev, int length){
	unsigned char buf[48] = {0};
	buf[0] = dev->Sci_ID;
	memcpy(&buf[1], dev->tx_fifo, length);

	if(dev->bus_iface != NULL){
		dev->bus_iface->write_data(dev->bus_obj, dev->obj, buf, length);
	}
}

void start_transmit_scififo_data(sci_28335_device* dev, int mode){
	sci_28335_reg_t* regs = dev->regs;
	int i;
	if (CHAR_LENGTH == 8){ 
		//memcpy(&dev->rx_fifo[TX_CNT], &dev->tx_fifo[TX_CNT], 1);
		WriteFIFO(dev->recv_fifo, &dev->tx_fifo[TX_CNT], 1);
	}else {
		printf("In %s, line:%d, Not implment char length %d...\n", __func__, __LINE__, CHAR_LENGTH);
		return ;
	}
	TX_CNT++;

	RX_CNT = TX_CNT; //receive fifo number
	if (RX_CNT >= regs->sciffrx.bit.RXFFIL && regs->scifftx.bit.SCIFFENA 
			&& regs->sciffrx.bit.RXFFIENA){
		//raise spirxfifo interrupt
		if (mode == LOOPBK_MODE){
			if (dev->signal_iface){
				dev->signal_iface->raise_signal(dev->signal, dev->Rx_int_number);
			}
		}
		send_data_to_stimulate(dev, RX_CNT);
	}	
}

void start_transmit_scidat(sci_28335_device* dev){
	sci_28335_reg_t* regs = dev->regs;
	if (regs->scictl1.bit.TXENA){
		if (CHAR_LENGTH == 8){
			regs->scirxbuf.all = regs->scitxbuf;
			WriteFIFO(dev->recv_fifo, &regs->scirxbuf.all, 1);
		} else {
			regs->scirxbuf.all = regs->scitxbuf << (CHAR_LENGTH);
			WriteFIFO(dev->recv_fifo, &regs->scirxbuf.all, 1);
			return ;
		}

		TX_CNT += 1;
		regs->scirxst.bit.RXRDY = 1;
		RX_CNT = TX_CNT; //receive fifo number
		TX_CNT = 0; //clear transmit fifo number
		if (RX_CNT >= regs->sciffrx.bit.RXFFIL && regs->scifftx.bit.SCIFFENA  
				&& regs->sciffrx.bit.RXFFIENA){
			//raise spirxfifo interrupt
			if (dev->signal_iface){
				dev->signal_iface->raise_signal(dev->signal, dev->Rx_int_number);
			}
			send_data_to_stimulate(dev, RX_CNT);
		}	
	}
}
static exception_t sci_28335_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	sci_28335_device *dev = opaque->obj;
	sci_28335_reg_t* regs = dev->regs;
	uint16_t val = *(uint16_t *)buf;
	switch(offset)
	{
		case SCICCR:
			regs->sciccr.all = val;
			break;
		case SCICTL1:
			regs->scictl1.all = val;
			if (regs->scictl1.bit.SWRESET){
				double period = CPU_FREQ * 1000 / (4 * (regs->scilbaud + 1)); 
				if (dev->handle == NULL)
					dev->handle = system_register_timer_handler(opaque, (int)2000, (time_sched_t)sci_interrupt_callback, SCHED_US|SCHED_NORMAL);
			}
			break;
		case SCIHBAUD:
			regs->scihbaud = val;
			break;
		case SCILBAUD:
			regs->scilbaud = val;
			break;
		case SCICTL2:
			regs->scictl2.all = val;
			break;
		case SCIRXST:
			regs->scirxst.all = val;
			break;
		case SCIRXEMU:
			regs->scirxemu = val;
			break;
		case SCIRXBUF:
			regs->scirxbuf.all = val;
			break;
		case SCITXBUF:
			if (regs->scifftx.bit.TXFFIL > 0){
				dev->tx_fifo[TX_CNT] = val;	

				if (regs->scictl1.bit.TXENA){ //loop-back mode
					if (regs->sciccr.bit.LOOPBKENA){
						start_transmit_scififo_data(dev, LOOPBK_MODE);
					}else {
						start_transmit_scififo_data(dev, NOLOOPBK_MODE);
					}
					if (regs->scifftx.bit.TXFFIL == TX_CNT)
						TX_CNT = 0;
				} else {
					//printf("In %s, line:%d, Not implement Not loop-back mode transmit.", __func__, __LINE__);
				} 
			}else {
				if (regs->sciccr.bit.LOOPBKENA){
					regs->scitxbuf = val;
					start_transmit_scidat(dev);
				}else {
					regs->scitxbuf = val;
					dev->tx_fifo[0] = val;
					regs->scictl2.bit.TXRDY = 1;
					send_data_to_stimulate(dev, 1);
					if(dev->dmc_linker_iface){
						dev->dmc_linker_iface->send_data(dev->dmc_linker, opaque, buf);
					}
				}
			}
			if (regs->scictl2.bit.TXINTENA && regs->scifftx.bit.SCIFFENA == 0){
				//raise 
				if (dev->signal_iface)
					dev->signal_iface->raise_signal(dev->signal, dev->Tx_int_number);
			}

			break;
		case SCIFFTX:
			regs->scifftx.all = val;
			if (regs->scifftx.bit.TXFFINTCLR)
				regs->scifftx.bit.TXFFINT = 0;
			break;
		case SCIFFRX:
			regs->sciffrx.all = val;
			if (regs->sciffrx.bit.RXFFINTCLR)
				regs->sciffrx.bit.RXFFINT = 0;
			break;
		case SCIFFCT:
			regs->sciffct.all = val;
			if (regs->sciffct.bit.ABDCLR)
				regs->sciffct.bit.ABD = 0;
			break;
		case SCIPRI:
			regs->scipri.all = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "write error register offset %d\n", offset);
			break;
	}
	return No_exp;
}

static exception_t sci_28335_write_hl(conf_object_t *opaque, generic_address_t offset, uint8_t* buf, size_t count, int hl_flag)
{
	exception_t ret;

	ret = sci_28335_write(opaque, offset, buf, count);

	return ret;
}

static char * sci_28335_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

uint32_t sci_28335_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	sci_28335_device *dev= conf_obj->obj;
	sci_28335_reg_t* regs = dev->regs;
	uint16_t* regs_value = (uint16_t*)regs + id;
	return *regs_value;
}

static char* sci_28335_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	sci_28335_device *dev = conf_obj->obj;
	sci_28335_reg_t* regs = dev->regs;
	*((uint16_t*)regs + id) = value;

	return NULL;
}

static uint32_t sci_28335_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	sci_28335_device *dev = conf_obj->obj;
	sci_28335_reg_t* regs = dev->regs;
	return  sizeof(sci_28335_reg_t) / 2;
}

static uint32_t sci_28335_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	sci_28335_device *dev = conf_obj->obj;
	sci_28335_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(sci_28335_reg_t)/ 2;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}

	return regnum;
}

static uint32_t sci_28335_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static exception_t rs422_iface_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sci_28335_device *dev = obj->obj;
	dev->bus_obj = obj2;
	dev->bus_iface = (Rs422_bus_intf*)SKY_get_iface(dev->bus_obj, RS422_BUS_INTF);

	if (dev->bus_iface == NULL){
		if (dev->bus_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", RS422_BUS_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", RS422_BUS_INTF, obj2->objname);

		return Not_found_exp;
	}

	if (Error == dev->bus_iface->register_device(obj2, obj)){
		printf("In %s, Register rs422_iface device Error.\n", __FUNCTION__);
	}

	return No_exp;
}

static exception_t rs422_iface_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	sci_28335_device *dev = obj->obj;
	*obj2 = dev->bus_obj;
	*port = NULL;
	return No_exp;
}

static conf_object_t* new_sci_28335(char* obj_name){
	sci_28335_device* dev = skyeye_mm_zero(sizeof(sci_28335_device));
	dev->obj = new_conf_object(obj_name, dev);
	/* init sci_28335 regs */
	sci_28335_reg_t* regs =  skyeye_mm_zero(sizeof(sci_28335_reg_t));
	dev->regs = regs;
	dev->timer_sched_id = -1;
	dev->first_data = 0;
	dev->recv_fifo = CreateFIFO(2048);
	RWLOCK_INIT(dev->lock);

	return dev->obj;
}

exception_t reset_sci_28335(conf_object_t* opaque, const char* parameters){

	return No_exp;
}

exception_t free_sci_28335(conf_object_t* opaque){
	sci_28335_device *dev = opaque->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev->regs);
	skyeye_free(dev->recv_fifo);
	RWLOCK_DESTROY(dev->lock);
	skyeye_free(dev);

	return No_exp;
}

static attr_value_t get_attr_Tx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	sci_28335_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->Tx_int_number);
	return int_number;
}

static exception_t set_attr_Tx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	sci_28335_device *dev = conf_obj->obj;
	dev->Tx_int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_Rx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	sci_28335_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->Rx_int_number);
	return int_number;
}

static exception_t set_attr_Rx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	sci_28335_device *dev = conf_obj->obj;
	dev->Rx_int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_Sci_ID(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	sci_28335_device *dev = conf_obj->obj;
	attr_value_t Sci_ID = SKY_make_attr_uinteger(dev->Sci_ID);
	return Sci_ID;
}

static exception_t set_attr_Sci_ID(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	sci_28335_device *dev = conf_obj->obj;
	dev->Sci_ID = SKY_attr_uinteger(*value);
	return No_exp;
}

static sci_28335_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "Tx_int_number", get_attr_Tx_int_number, NULL, set_attr_Tx_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of sci Tx");
	SKY_register_attribute(clss, "Rx_int_number", get_attr_Rx_int_number, NULL, set_attr_Rx_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of sci Rx");
	SKY_register_attribute(clss, "Sci_ID", get_attr_Sci_ID, NULL, set_attr_Sci_ID, NULL, SKY_Attr_Optional, "uinteger", "ID of Sci");
}

static exception_t set_attr_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sci_28335_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

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

static exception_t get_attr_signal(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	sci_28335_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct sci_28335_device *dev = obj->obj;
	dev->term_dev.obj = obj2;
	dev->term_dev.term = (skyeye_uart_intf*)SKY_get_iface(dev->term_dev.obj, SKYEYE_UART_INTF);

	if (dev->term_dev.term == NULL){
		if (dev->term_dev.obj == NULL)
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
	struct sci_28335_device *dev = obj->obj;
	*obj2 = dev->term_dev.obj;
	*port = NULL;
	return No_exp;
}

static exception_t dmc_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sci_28335_device *dev =  obj->obj;
	dev->dmc_linker = obj2;
	dev->dmc_linker_iface = (dmc_linker_intf *)SKY_get_iface(dev->dmc_linker, DMC_LINKER_INTF);

	if (dev->dmc_linker_iface == NULL){
		if (dev->dmc_linker == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", DMC_LINKER_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", DMC_LINKER_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t dmc_linker_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	sci_28335_device *dev =  obj->obj;
	*obj2 = dev->dmc_linker;
	*port = NULL;
	return No_exp;
}

void init_sci_28335(){
	static skyeye_class_t class_data = {
		.class_name = "f28335_sci",
		.class_desc = "f28335_sci",
		.new_instance = new_sci_28335,
		.free_instance = free_sci_28335,
		.reset_instance = reset_sci_28335,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_f28335),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = sci_28335_read,
		.write = sci_28335_write,
		.read_hl = sci_28335_read_hl,
		.write = sci_28335_write_hl
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = sci_28335_get_regvalue_by_id,
		.get_regname_by_id = sci_28335_get_regname_by_id,
		.set_regvalue_by_id = sci_28335_set_regvalue_by_id,
		.get_regnum = sci_28335_get_regnum,
		.get_regid_by_name = sci_28335_get_regid_by_name,
		.get_regoffset_by_id = sci_28335_get_regoffset_by_id,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const dmc_dev_intf dmc_intf = {
		.receive_data = dmc_receive,
		.raise_signal = dmc_raise_signal,
	};
	SKY_register_iface(clss, DMC_DEV_INTF, &dmc_intf);

	static const Rs422_emif_intf rs422_emif_interface = {
		.set_state = recv_data_from_stimulate,
	};
	SKY_register_iface(clss, RS422_EMIF_INTF, &rs422_emif_interface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME, 
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = DMC_DEV_INTF, 
			.iface = &dmc_intf,
		},
	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = SKYEYE_UART_INTF,
			.set = uart_term_set,
			.get = uart_term_get,
		},
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = set_attr_signal,
			.get = get_attr_signal,
		},
		(struct ConnectDescription) {
			.name = RS422_BUS_INTF,
			.set = rs422_iface_set,
			.get = rs422_iface_get,
		},
		(struct ConnectDescription) {
			.name = DMC_LINKER_INTF,
			.set = dmc_linker_set,
			.get = dmc_linker_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	sci_28335_register_attribute(clss);
}

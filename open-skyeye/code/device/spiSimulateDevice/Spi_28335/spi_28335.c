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
* @file spi_28335.c
* @brief The implementation of system controller
* @author 
* @version 78.77
*/

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

#include "spi_28335.h"

static void spi_interrupt_callback(conf_object_t *opaque){
	spi_28335_device *dev = opaque->obj;
	spi_28335_reg_t* regs = dev->regs;

	if (TX_CNT <= regs->spifftx.bit.TXFFIL && regs->spictl.bit.SPIINTENA && regs->spifftx.bit.TXFFIENA){
		//raise spitxfifo interrupt
		if (dev->signal_iface){
			dev->signal_iface->raise_signal(dev->signal, dev->Tx_int_number);
		}
	}	
}

void start_transmit_spififo_data(spi_28335_device* dev){
	spi_28335_reg_t* regs = dev->regs;
	if (regs->spictl.bit.TALK){
		if (CHAR_LENGTH > 8)
			memcpy(dev->rx_fifo, dev->tx_fifo, TX_CNT * 2);
		else if (CHAR_LENGTH == 8) 
			memcpy(dev->rx_fifo, dev->tx_fifo, TX_CNT);
		else {
			printf("In %s, line:%d, Not implment char length %d...\n", __func__, __LINE__, CHAR_LENGTH);
			return ;
		}
		
		regs->spists.bit.INT_FLAG = 1;
		regs->spists.bit.BUFFULL_FLAG = 0;
		RX_CNT = TX_CNT; //receive fifo number
		if (RX_CNT >= regs->spiffrx.bit.RXFFIL && regs->spictl.bit.SPIINTENA 
				&& regs->spiffrx.bit.RXFFIENA){
			//raise spirxfifo interrupt
			if (dev->signal_iface){
				dev->signal_iface->raise_signal(dev->signal, dev->Rx_int_number);
			}
		}	

		if (regs->spists.bit.INT_FLAG && regs->spictl.bit.SPIINTENA 
				&& (regs->spiffrx.bit.RXFFIENA == 0)){
			//raise spirx interrupt
			if (dev->signal_iface){
				dev->signal_iface->raise_signal(dev->signal, dev->Rx_int_number);
			}
		}	
	}
}

void start_transmit_spidat(spi_28335_device* dev){
	spi_28335_reg_t* regs = dev->regs;
	if (regs->spictl.bit.TALK){
		if (CHAR_LENGTH == 16){
			regs->spirxbuf = regs->spidat;
		} else {
			regs->spirxbuf = regs->spidat << (CHAR_LENGTH);
			printf("In %s, line:%d, Not implment char length %d...\n", __func__, __LINE__, CHAR_LENGTH);
			return ;
		}
		
		regs->spists.bit.INT_FLAG = 1;
		regs->spists.bit.BUFFULL_FLAG = 0;
		RX_CNT = TX_CNT; //receive fifo number
		TX_CNT = 0; //clear transmit fifo number
		if (RX_CNT >= regs->spiffrx.bit.RXFFIL && regs->spictl.bit.SPIINTENA 
				&& regs->spiffrx.bit.RXFFIENA){
			//raise spirxfifo interrupt
			if (dev->signal_iface){
				dev->signal_iface->raise_signal(dev->signal, dev->Rx_int_number);
			}
		}	
	}
}

static exception_t spi_28335_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	spi_28335_device *dev = opaque->obj;
	spi_28335_reg_t* regs = dev->regs;
	int tmp_cnt, rxffil;
	switch(offset) {
		case SPICCR:
			*(uint16_t *)buf = regs->spiccr.all;
			break;
		case SPICTL:
			*(uint16_t *)buf = regs->spictl.all;
			break;
		case SPISTS:
			*(uint16_t *)buf = regs->spists.all;
			break;
		case SPIBRR:
			*(uint16_t *)buf = regs->spibrr;
			break;
		case SPIRXEMU:
			*(uint16_t *)buf = regs->spirxemu;
			break;
		case SPIRXBUF:
			rxffil = regs->spiffrx.bit.RXFFIL & 0xF;
			if ((regs->spiffrx.bit.RXFFIL > 0) && (rxffil != 0xF)){
				tmp_cnt = regs->spiffrx.bit.RXFFIL;
				*(uint16_t *)buf = dev->rx_fifo[tmp_cnt - RX_CNT];
				RX_CNT -= 1;
			}else 
				*(uint16_t *)buf = regs->spirxbuf;
			regs->spists.bit.INT_FLAG = 0;
			break;
		case SPITXBUF:
			*(uint16_t *)buf = regs->spitxbuf;
			break;
		case SPIDAT:
			*(uint16_t *)buf = regs->spidat;
			break;
		case SPIFFTX:
			*(uint16_t *)buf = regs->spifftx.all;
			break;
		case SPIFFRX:
			*(uint16_t *)buf = regs->spiffrx.all;
			break;
		case SPIFFCT:
			*(uint16_t *)buf = regs->spiffct.all;
			break;
		case SPIPRI:
			*(uint16_t *)buf = regs->spipri.all;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in spi_28335\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}

static exception_t spi_28335_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	spi_28335_device *dev = opaque->obj;
	spi_28335_reg_t* regs = dev->regs;
	uint16_t val = *(uint16_t*)buf;
	switch(offset) {
		
            case SPICCR:
			regs->spiccr.all = val;
			if (regs->spiccr.bit.SPILBK){
				//loop-back mode
			}
			if (regs->spiccr.bit.SPISWRESET == 0){
				regs->spists.bit.INT_FLAG = 0;
				regs->spists.bit.OVERRUN_FLAG = 0;
				regs->spists.bit.BUFFULL_FLAG = 0;
			}else {
                double period = CPU_FREQ * 1000 / (4 * (regs->spibrr + 1)); 
                if (dev->handle == NULL)
                    dev->handle = system_register_timer_handler(opaque, (int)period, (time_sched_t)spi_interrupt_callback, SCHED_MS|SCHED_NORMAL);
            }
			break;
		case SPICTL:
			regs->spictl.all = val;
			if (regs->spictl.bit.OVERRUNINTENA){
				regs->spists.bit.OVERRUN_FLAG = 1;
			}
			break;
		case SPISTS:
			regs->spists.all = val;
			if (regs->spists.bit.OVERRUN_FLAG)
				regs->spists.bit.OVERRUN_FLAG = 0;
			break;
		case SPIBRR:
			regs->spibrr = val;
			break;
		case SPIRXEMU:
			regs->spirxemu = val;
			break;
		case SPIRXBUF:
			regs->spirxbuf = val;
			break;
		case SPITXBUF:
			regs->spists.bit.BUFFULL_FLAG = 1;
			if (regs->spifftx.bit.TXFFIL > 0){ //FIFO mode
				dev->tx_fifo[TX_CNT] = val;
				TX_CNT += 1;

				if (regs->spictl.bit.MASTER_SLAVE){
					//if no transmission is current active, start transmission
					if (regs->spiccr.bit.SPILBK){ //loop-back mode
						start_transmit_spififo_data(dev);
						if (regs->spifftx.bit.TXFFIL == TX_CNT)
							TX_CNT = 0;
					} else 
						printf("In %s, line:%d, Not implement Not loop-back mode transmit.", __func__, __LINE__);
				}
			}else {
				regs->spitxbuf = val;
				regs->spidat = val;
				TX_CNT += 1;
				if (regs->spictl.bit.MASTER_SLAVE){
					start_transmit_spidat(dev);
				}
			}
			break;
		case SPIDAT:
			regs->spidat = val;
			TX_CNT += 1;
			if (regs->spictl.bit.MASTER_SLAVE)
				start_transmit_spidat(dev);
			break;
		case SPIFFTX:
			regs->spifftx.all = val;
			if (regs->spifftx.bit.TXFFINTCLR)
				regs->spifftx.bit.TXFFINT = 0; //clear INT

			if (regs->spifftx.bit.TXFIFO)
				TX_CNT = 0;
			break;
		case SPIFFRX:
			regs->spiffrx.all = val;
			if (regs->spiffrx.bit.RXFFINTCLR)
				regs->spiffrx.bit.RXFFINT = 0; //clear INT

			if (regs->spiffrx.bit.RXFIFORESET)
				RX_CNT = 0;
			break;
		case SPIFFCT:
			regs->spiffct.all = val;
			break;
		case SPIPRI:
			regs->spipri.all = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in spi_28335\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}

static char* spi_28335_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t spi_28335_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	spi_28335_device *dev = conf_obj->obj;
	spi_28335_reg_t* regs = dev->regs;
	uint16_t* regs_value = (uint16_t*)regs + id;
	return *regs_value;
}

static char* spi_28335_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	spi_28335_device *dev = conf_obj->obj;
	spi_28335_reg_t* regs = dev->regs;
	*((uint16_t*)regs + id) = value;

	return NULL;
}

static uint32_t spi_28335_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static uint32_t spi_28335_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	spi_28335_device *dev = conf_obj->obj;
	spi_28335_reg_t* regs = dev->regs;
	return  sizeof(spi_28335_reg_t) / 2;
}

static uint32_t spi_28335_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	spi_28335_device *dev = conf_obj->obj;
	spi_28335_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(spi_28335_reg_t)/ 2;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}

	return regnum;
}

static conf_object_t* new_spi_28335(char* obj_name){
	spi_28335_device* dev = skyeye_mm_zero(sizeof(spi_28335_device));
	spi_28335_reg_t* regs =  skyeye_mm_zero(sizeof(spi_28335_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init spi_28335 regs */
	dev->regs = regs;
	dev->sched_id = -1;
//	create_thread_scheduler((unsigned int )1000, Periodic_sched, spi_interrupt_callback,  (void *)dev, &dev->sched_id);

	return dev->obj;
}

void free_spi_28335(conf_object_t* conf_obj){
	spi_28335_device* dev = (spi_28335_device *)conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev->regs);
	skyeye_free(dev);

	return;
}

static attr_value_t get_attr_Tx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	spi_28335_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->Tx_int_number);
	return int_number;
}

static exception_t set_attr_Tx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	spi_28335_device *dev = conf_obj->obj;
	dev->Tx_int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_Rx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	spi_28335_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->Rx_int_number);
	return int_number;
}

static exception_t set_attr_Rx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	spi_28335_device *dev = conf_obj->obj;
	dev->Rx_int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static spi_28335_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "Tx_int_number", get_attr_Tx_int_number, NULL, set_attr_Tx_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of spi Tx");
	SKY_register_attribute(clss, "Rx_int_number", get_attr_Rx_int_number, NULL, set_attr_Rx_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of spi Rx");
}

static exception_t set_attr_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	spi_28335_device *dev = obj->obj;
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
	spi_28335_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void init_spi_28335(){
	static skyeye_class_t class_data = {
		.class_name = "f28335_spi",
		.class_desc = "f28335_spi",
		.new_instance = new_spi_28335,
		.free_instance = free_spi_28335,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_f28335),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = spi_28335_read,
		.write = spi_28335_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = spi_28335_get_regvalue_by_id,
		.get_regname_by_id = spi_28335_get_regname_by_id,
		.set_regvalue_by_id = spi_28335_set_regvalue_by_id,
		.get_regnum = spi_28335_get_regnum,
		.get_regid_by_name = spi_28335_get_regid_by_name,
		.get_regoffset_by_id = spi_28335_get_regoffset_by_id,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF, 
			.iface = &reg_intf,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = set_attr_signal,
			.get = get_attr_signal,
		},
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	spi_28335_register_attribute(clss);
}

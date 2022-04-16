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
* @file spi_am335x.c
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
#include <skyeye_dev_intf.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"

#include "spi_am335x.h"


void CreateFIFO(FIFO *fifo, uint32 FIFOLength)
{
    uint8 *pfifo;
    pfifo = (uint8 *)malloc(FIFOLength);
    fifo->pFirst = pfifo;
    fifo->pLast = pfifo + FIFOLength-1;
    fifo->Length = FIFOLength;
    fifo->pIn     = fifo->pFirst;
    fifo->pOut    = fifo->pFirst;
    fifo->Enteres = 0;  
}

uint32 WriteFIFO(FIFO *fifo, uint8 *pSource,uint32 WriteLength)
{
    uint32 i;
    
	for (i = 0; i < WriteLength; i++){
		if (fifo->Enteres >= fifo->Length){
			return i;//如果已经写入FIFO的数据两超过或者等于FIFO的长度，就返回实际写入FIFO的数据个数
		}
		*(fifo->pIn) = *(pSource ++);
		if (fifo->pIn == fifo->pLast)
		{
			fifo->pIn = fifo->pFirst;
		}
		else
		{
			fifo->pIn ++;
		}
		fifo->Enteres ++;
	}
    return i;
}

uint32 ReadFIFO(FIFO *fifo, uint8 *pAim,uint32 ReadLength)
{
    uint32 i;
	for (i = 0; i < ReadLength; i++){
		if (fifo->Enteres <= 0){
			return i;//返回从FIFO中读取到的数据个数
		}
		*(pAim ++) = *(fifo->pOut);
		if (fifo->pOut == fifo->pLast)
		{
			fifo->pOut = fifo->pFirst;
		}
		else
		{
			fifo->pOut ++;
		}
		fifo->Enteres -- ;
	} 
	return i;
}

uint32 CheckFIFOLength(FIFO *fifo)
{
    return fifo->Length;
}


uint8* CheckCurrentWritePoint(FIFO *fifo)
{
    return (fifo->pIn);
}

uint8* CheckCurrentReadPoint(FIFO *fifo)
{
    return (fifo->pOut);
}


void FreeFIFO(FIFO *fifo)
{
    free(fifo->pFirst);
}

uint32 CheckCanWriteNum(FIFO *fifo)
{
    return (fifo->Length - fifo->Enteres);
}

uint32 CheckCanReadNum(FIFO *fifo)
{
    return fifo->Enteres;
}

static void *hardware_trans(void *spi_dev)
{
	int buf;
	am335x_spi_dev *dev= spi_dev;
	return NULL;
}


void am335x_spi_request(conf_object_t *opaque, uint8_t value)
{

}

static exception_t am335x_spi_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	am335x_spi_dev *dev = opaque->obj;
	switch(offset)
	{
		case MCSPI_REVISION  : 
			*(uint32_t*)buf = dev->regs.mcspi_revision;
			break;
                case MCSPI_SYSCONFIG : 
			*(uint32_t*)buf = dev->regs.mcspi_sysconfig;
			break;
                case MCSPI_SYSSTATUS : 
                	*(uint32_t*)buf = dev->regs.mcspi_sysstatus;
			if((dev->regs.mcspi_sysstatus) & MCSPI_SYSSTATUS_RESETDONE_COMPLETED)
				dev->regs.mcspi_sysstatus &= ~MCSPI_SYSSTATUS_RESETDONE_COMPLETED;
			break;
                case MCSPI_IRQSTATUS : 
			*(uint32_t*)buf = dev->regs.mcspi_irqstatus;
			
			break;
                case MCSPI_IRQENABLE : 
			*(uint32_t*)buf = dev->regs.mcspi_irqenable;
			break;
                case MCSPI_SYST      : 
			*(uint32_t*)buf = dev->regs.mcspi_syst;
			break;
                case MCSPI_MODULCTRL : 
			*(uint32_t*)buf = dev->regs.mcspi_modulctrl;
			break;
                case MCSPI_CH0CONF   : 
			*(uint32_t*)buf = dev->regs.mcspi_ch0conf;
			break;
                case MCSPI_CH0STAT   : 
			*(uint32_t*)buf = dev->regs.mcspi_ch0stat;
			break;
                case MCSPI_CH0CTRL   : 
			*(uint32_t*)buf = dev->regs.mcspi_ch0ctrl;
			break;
                case MCSPI_TX0       : 
			*(uint32_t*)buf = dev->regs.mcspi_tx0;
			break;
                case MCSPI_RX0       : 
			*(uint32_t*)buf = dev->regs.mcspi_rx0;
			dev->read_num -= 1;
			break;
                case MCSPI_CH1CONF   : 
			*(uint32_t*)buf = dev->regs.mcspi_ch1conf;
			break;
                case MCSPI_CH1STAT   : 
			*(uint32_t*)buf = dev->regs.mcspi_ch1stat;
			break;
                case MCSPI_CH1CTRL   : 
			*(uint32_t*)buf = dev->regs.mcspi_ch1ctrl;
			break;
                case MCSPI_TX1       : 
			*(uint32_t*)buf = dev->regs.mcspi_tx1;
			break;
                case MCSPI_RX1       : 
			*(uint32_t*)buf = dev->regs.mcspi_rx1;
			break;
                case MCSPI_CH2CONF   : 
			*(uint32_t*)buf = dev->regs.mcspi_ch2conf;
			break;
                case MCSPI_CH2STAT   : 
			*(uint32_t*)buf = dev->regs.mcspi_ch2stat;
			break;
                case MCSPI_CH2CTRL   : 
			*(uint32_t*)buf = dev->regs.mcspi_ch2ctrl;
			break;
                case MCSPI_TX2       : 
			*(uint32_t*)buf = dev->regs.mcspi_tx2;
			break;
                case MCSPI_RX2       : 
			*(uint32_t*)buf = dev->regs.mcspi_rx2;
			break;
                case MCSPI_CH3CONF   : 
			*(uint32_t*)buf = dev->regs.mcspi_ch3conf;
			break;
                case MCSPI_CH3STAT   : 
			*(uint32_t*)buf = dev->regs.mcspi_ch3stat;
			break;
                case MCSPI_CH3CTRL   : 
			*(uint32_t*)buf = dev->regs.mcspi_ch3ctrl;
			break;
                case MCSPI_TX3       : 
			*(uint32_t*)buf = dev->regs.mcspi_tx3;
			break;
                case MCSPI_RX3       : 
			*(uint32_t*)buf = dev->regs.mcspi_rx3;
			break;
                case MCSPI_XFERLEVEL : 
			*(uint32_t*)buf = dev->regs.mcspi_xferlevel;
			break;
                case MCSPI_DAFTX     : 
			*(uint32_t*)buf = dev->regs.mcspi_daftx;
			break;
                case MCSPI_DAFRX     : 
			*(uint32_t*)buf = dev->regs.mcspi_dafrx;
			break;
	}       
	return  No_exp;
}

static exception_t am335x_spi_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	am335x_spi_dev *dev = opaque->obj;
	switch(offset)
	{
		case MCSPI_REVISION  : 
			dev->regs.mcspi_revision = *(uint32_t*)buf;
			break;
                case MCSPI_SYSCONFIG : 
			dev->regs.mcspi_sysconfig = *(uint32_t*)buf;
			if(dev->regs.mcspi_sysconfig & MCSPI_SYSCONFIG_SOFTRESET)
				dev->regs.mcspi_sysstatus |= MCSPI_SYSSTATUS_RESETDONE_COMPLETED;
			break;
                case MCSPI_SYSSTATUS : 
                	dev->regs.mcspi_sysstatus = *(uint32_t*)buf;
			break;
                case MCSPI_IRQSTATUS : 
			dev->regs.mcspi_irqstatus &= ~(*(uint32_t*)buf);
			if(dev->ReadFlag == 1 && dev->read_num > 0){
				dev->regs.mcspi_irqstatus |= 0x4;
			}
			break;
                case MCSPI_IRQENABLE : 
			{
			uint32_t tmp_irqenable =  dev->regs.mcspi_irqenable;
			dev->regs.mcspi_irqenable = *(uint32_t*)buf;
			if(*(uint32_t *)buf & 0x1 || *(uint32_t *)buf & 0x4){
				if(dev->regs.mcspi_irqenable & 0x1)
					dev->regs.mcspi_irqstatus |=  0x1;
				if(dev->regs.mcspi_irqenable & 0x4)
					dev->regs.mcspi_irqstatus |=  0x4;
				if (dev->signal)
					dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
			}
			if(*(uint32_t*)buf & 0x4 == 0){
				if(tmp_irqenable & 0x4)
					dev->ReadFlag = 0;
			}
			}
			break;
                case MCSPI_SYST      : 
			dev->regs.mcspi_syst = *(uint32_t*)buf;
			break;
                case MCSPI_MODULCTRL : 
			dev->regs.mcspi_modulctrl = *(uint32_t*)buf;
			break;
                case MCSPI_CH0CONF   : 
			{
			uint32_t tmp_ch0_conf =  dev->regs.mcspi_ch0conf;
			dev->regs.mcspi_ch0conf = *(uint32_t*)buf;
			if(*(uint32_t *)buf & 0x100000){
				dev->FirstFlag = 1;
				if (dev->spi_bus_iface)
					dev->spi_bus_iface->connect_slave(dev->spi_bus_obj, "max1253_0");
			}else{
				if(tmp_ch0_conf& 0x100000){
					if (dev->spi_bus_iface)
						dev->spi_bus_iface->disconnect_slave(dev->spi_bus_obj, "max1253_0");
				}
			}
			}
			break;
                case MCSPI_CH0STAT   : 
			dev->regs.mcspi_ch0stat = *(uint32_t*)buf;
			break;
                case MCSPI_CH0CTRL   : 
			dev->regs.mcspi_ch0ctrl = *(uint32_t*)buf;
			break;
                case MCSPI_TX0       : 
			{
				int first = 0;
				char feedback;
				dev->regs.mcspi_tx0 = *(uint32_t*)buf;
				if(dev->FirstFlag == 1){
					dev->FirstFlag = 0;
					first = 1;
				}
				if (dev->spi_bus_iface)
					dev->spi_bus_iface->spi_slave_request(dev->spi_bus_obj, first, 0, (uint8_t *)buf, 1, &feedback);
				if(feedback > 0){
					dev->read_num = feedback + 1;
					dev->ReadFlag = 1;
				}
					
				if(dev->regs.mcspi_irqenable & 0x1 ){
					if(dev->ReadFlag == 1){
						dev->regs.mcspi_rx0 = *(uint8_t *)buf;
						dev->regs.mcspi_irqstatus |= 0x4;
					}
					dev->regs.mcspi_irqstatus |= 0x1;
					if (dev->signal)
						dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
				}
			}
			break;
                case MCSPI_RX0       : 
			dev->regs.mcspi_rx0 = *(uint32_t*)buf;
			break;
                case MCSPI_CH1CONF   : 
			dev->regs.mcspi_ch1conf = *(uint32_t*)buf;
			break;
                case MCSPI_CH1STAT   : 
			dev->regs.mcspi_ch1stat = *(uint32_t*)buf;
			break;
                case MCSPI_CH1CTRL   : 
			dev->regs.mcspi_ch1ctrl = *(uint32_t*)buf;
			break;
                case MCSPI_TX1       : 
			dev->regs.mcspi_tx1 = *(uint32_t*)buf;
			break;
                case MCSPI_RX1       : 
			dev->regs.mcspi_rx1 = *(uint32_t*)buf;
			break;
                case MCSPI_CH2CONF   : 
			dev->regs.mcspi_ch2conf = *(uint32_t*)buf;
			break;
                case MCSPI_CH2STAT   : 
			dev->regs.mcspi_ch2stat = *(uint32_t*)buf;
			break;
                case MCSPI_CH2CTRL   : 
			dev->regs.mcspi_ch2ctrl = *(uint32_t*)buf;
			break;
                case MCSPI_TX2       : 
			dev->regs.mcspi_tx2 = *(uint32_t*)buf;
			break;
                case MCSPI_RX2       : 
			dev->regs.mcspi_rx2 = *(uint32_t*)buf;
			break;
                case MCSPI_CH3CONF   : 
			dev->regs.mcspi_ch3conf = *(uint32_t*)buf;
			break;
                case MCSPI_CH3STAT   : 
			dev->regs.mcspi_ch3stat = *(uint32_t*)buf;
			break;
                case MCSPI_CH3CTRL   : 
			dev->regs.mcspi_ch3ctrl = *(uint32_t*)buf;
			break;
                case MCSPI_TX3       : 
			dev->regs.mcspi_tx3 = *(uint32_t*)buf;
			break;
                case MCSPI_RX3       : 
			dev->regs.mcspi_rx3 = *(uint32_t*)buf;
			break;
                case MCSPI_XFERLEVEL : 
			dev->regs.mcspi_xferlevel = *(uint32_t*)buf;
			break;
                case MCSPI_DAFTX     : 
			dev->regs.mcspi_daftx = *(uint32_t*)buf;
			break;
                case MCSPI_DAFRX     : 
			dev->regs.mcspi_dafrx = *(uint32_t*)buf;
			break;
	}
	return  No_exp;
}

static conf_object_t* new_am335x_spi(char* obj_name)
{
	am335x_spi_dev* am335x_spi = skyeye_mm_zero(sizeof(am335x_spi_dev));

	am335x_spi->obj = new_conf_object(obj_name, am335x_spi);
#if 0
	memory_space_intf* io_memory = skyeye_mm_zero(sizeof(memory_space_intf));
	io_memory->conf_obj = am335x_spi->obj;
	io_memory->read = am335x_spi_read;
	io_memory->write = am335x_spi_write;
	SKY_register_interface((void*)io_memory, obj_name, MEMORY_SPACE_INTF_NAME);
#endif
	am335x_spi->FirstFlag = 0;
	am335x_spi->ReceiveLen = 0;
	am335x_spi->ReadFlag = 0;
	pthread_t pthread_id;
	
	create_thread_scheduler(1000000, Periodic_sched, hardware_trans, am335x_spi, &pthread_id);
	return am335x_spi->obj;
}

static exception_t reset_am335x_spi(conf_object_t* opaque, const char* args)
{
#if 1
	am335x_spi_dev *dev = opaque->obj;
	memset(&(dev->regs), 0, sizeof(dev->regs));
#endif
	return No_exp;        
}

static exception_t free_am335x_spi(conf_object_t* conf_obj)
{
	am335x_spi_dev *dev = conf_obj->obj;

	skyeye_free(dev->obj);
	skyeye_free(dev);
	return No_exp;
}

static exception_t get_attr_signal(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	am335x_spi_dev *dev = obj->obj;

	*obj2 = dev->signal_obj;
	*port = NULL;
	return No_exp;
}

static exception_t set_attr_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index){
	am335x_spi_dev *dev = obj->obj;

	dev->signal_obj = obj2;

	if (dev->signal_obj == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Connect iface name: %s object is NULL.", GENERAL_SIGNAL_INTF_NAME);
		return Not_found_exp;
	}
	dev->signal = (general_signal_intf *)SKY_get_iface(dev->signal_obj, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SPI_BUS_INTF_NAME, obj2->objname);
		return Not_found_exp;
	}
	return No_exp;
}

static exception_t get_attr_spi_master(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	am335x_spi_dev *dev = obj->obj;

	*obj2 = dev->spi_bus_obj;
	*port = NULL;
	return No_exp;
}

static exception_t set_attr_spi_master(conf_object_t *obj, conf_object_t *obj2, const char *port, int index){
	am335x_spi_dev *dev = obj->obj;
	dev->spi_bus_obj = obj2;

	if (dev->spi_bus_obj == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Connect iface name: %s object is NULL.", SPI_BUS_INTF_NAME);
		return Not_found_exp;
	}

	dev->spi_bus_iface = (spi_bus_intf *)SKY_get_iface(dev->spi_bus_obj, SPI_BUS_INTF_NAME);

	if (dev->spi_bus_iface == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SPI_BUS_INTF_NAME, obj2->objname);
		return Not_found_exp;
	}
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	am335x_spi_dev *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	am335x_spi_dev *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void am335x_spi_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");

}

static void am335x_spi_register_interface(conf_class_t* clss){
	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = am335x_spi_read,
		.write = am335x_spi_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		}
 	};
	class_register_ifaces(clss, ifaces);

	return;
}

static void am335x_spi_register_connect(conf_class_t* clss){
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = set_attr_signal,
			.get = get_attr_signal,
		},
		(struct ConnectDescription ){
			.name = SPI_BUS_INTF_NAME,
			.set = set_attr_spi_master,
			.get = get_attr_spi_master,
		},
	};
	class_register_connects(clss, connects);

	return;
}

void init_am335x_spi(){
	static skyeye_class_t class_data = {
		.class_name = "am335x_spi",
		.class_desc = "am335x_spi",
		.new_instance = new_am335x_spi,
		.reset_instance = reset_am335x_spi,
		.free_instance = free_am335x_spi,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_am335x SC_am3359),
	};

	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);

	am335x_spi_register_interface(class);
	am335x_spi_register_connect(class);
	am335x_spi_register_attribute(class);
}


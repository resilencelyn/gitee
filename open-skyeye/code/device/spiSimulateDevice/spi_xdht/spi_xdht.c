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
* @file spi_xdht.c
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-12-04
*/

#include <stdio.h>
#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_exec.h>
#include <skyeye_cell.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <skyeye_dev_bus_intf.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"
#include "spi_xdht.h"

static void CreateFIFO(FIFO *fifo, uint32 FIFOLength)
{
    uint8 *pfifo;
    pfifo = (uint8 *)malloc(FIFOLength * sizeof(uint8));
    fifo->pFirst = pfifo;
    fifo->pLast = pfifo + FIFOLength-1;
    fifo->Length = FIFOLength;
    fifo->pIn     = fifo->pFirst;
    fifo->pOut    = fifo->pFirst;
    fifo->Enteres = 0;  
}
static uint32 WriteFIFO(FIFO *fifo, uint8 *pSource,uint32 WriteLength)
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

static uint32 ReadFIFO(FIFO *fifo, uint8 *pAim,uint32 ReadLength)
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

static uint32 CheckFIFOLength(FIFO *fifo)
{
    return fifo->Length;
}


static uint8 *CheckCurrentWritePoint(FIFO *fifo)
{
    return (fifo->pIn);
}

static uint8 *CheckCurrentReadPoint(FIFO *fifo)
{
    return (fifo->pOut);
}


static void FreeFIFO(FIFO *fifo)
{
    free(fifo->pFirst);
}

static void ResetFIFO(FIFO *fifo)
{
	fifo->pIn  = fifo->pFirst;
	fifo->pOut = fifo->pFirst;
	fifo->Enteres = 0;  
}

static uint32 CheckCanWriteNum(FIFO *fifo)
{
    return (fifo->Length - fifo->Enteres);
}

static uint32 CheckCanReadNum(FIFO *fifo)
{
    return fifo->Enteres;
}


static void *hardware_trans(void *spi_dev)
{
	xdht_spi_dev * dev = spi_dev;
	int first = 0;
	uint8_t  buf;
	char feedback;
	 if(dev->MASTER_SEND_FLAG == 1){
		 if(CheckCanReadNum(&dev->tx_fifo) > 0)
			ReadFIFO(&dev->tx_fifo, &buf, 1);
		else
			buf = 0x0;
		if(dev->StartFlag == 1){
			first = 1;
			dev->StartFlag = 0;
		}
		dev->spi_bus_iface->spi_slave_request(dev->spi_bus_obj, first, 1, &buf, 1, &feedback);
		WriteFIFO(&dev->rx_fifo, &buf, 1);
		dev->regs.spi_rfor =  CheckCanReadNum(&dev->rx_fifo);
		dev->write_num++;
		if(dev->write_num == dev->regs.spi_dlr){
			dev->MASTER_SEND_FLAG = 0;
			dev->spi_bus_iface->disconnect_slave(dev->spi_bus_obj, "max1253_1");
			dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
		}
	 }
	 return NULL;
}
#if 0
static exception_t xdht_spi_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	xdht_spi_dev *dev = opaque->obj;
	if(!strncmp(attr_name, "signal", strlen("signal"))){
		dev->signal = (general_signal_intf *)SKY_get_iface(value->u.object, GENERAL_SIGNAL_INTF_NAME);
	}else if(!strncmp(attr_name, "i2c_master", strlen("i2c_master"))){
		dev->spi_bus_iface =  (spi_bus_intf* )SKY_get_iface(value->u.object, SPI_BUS_INTF_NAME);
		return Invarg_exp;
	}else if(!strncmp(attr_name, "int_number", strlen("int_number"))){
		dev->interrupt_number = value->u.uinteger;
	}else{
		printf("parameter error\n");
		return Invarg_exp;
	}
	return No_exp;
}
#endif
static exception_t xdht_spi_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	xdht_spi_dev *dev = opaque->obj;
	switch(offset)
	{
		case SPI_CR:
			*(uint8 *)buf = dev->regs.spi_cr;
			break;
		case SPI_SR:
			*(uint8 *)buf = dev->regs.spi_sr;
			break;
		case SPI_RFOR:
			*(uint8 *)buf = dev->regs.spi_rfor;
			break;
		case SPI_TFOR:
			*(uint8 *)buf = dev->regs.spi_tfor;
			break;
		case SPI_RF:
			{
				uint8_t ch;
			 ReadFIFO(&dev->rx_fifo, &ch, 1);
			dev->regs.spi_rf = ch;
			*(uint8 *)buf = ch;
			}
			break;
                default:

			printf("read %s error offset %d : 0x%x\n",dev->obj->objname, offset, *(uint16*)buf);
			break;
	}       

	return  No_exp;
}

static exception_t xdht_spi_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	xdht_spi_dev *dev = opaque->obj;
	switch(offset)
	{
		case SPI_CR:
			dev->regs.spi_cr = *(uint8 *)buf;
			break;
		case SPI_RSR:
			dev->regs.spi_rsr = *(uint8 *)buf;
			break;
		case SPI_CSR:
			dev->regs.spi_csr = *(uint8 *)buf;
			break;
		case SPI_STWR:
			dev->regs.spi_stwr = *(uint8 *)buf;
			if(*(uint8 *)buf & 0x1){
				dev->spi_bus_iface->connect_slave(dev->spi_bus_obj, "max1253_1");
				dev->StartFlag = 1;
				dev->MASTER_SEND_FLAG = 1;
				dev->write_num = 0;
			}
			break;
		case SPI_MRR:
			dev->regs.spi_mrr = *(uint8 *)buf;
			break;
		case SPI_CRFR:
			dev->regs.spi_crfr = *(uint8 *)buf;
			if(*(uint8 *)buf & 0x1){
				ResetFIFO(&dev->rx_fifo);
			}
			break;
		case SPI_DLR:
			dev->regs.spi_dlr = *(uint8 *)buf;
			break;
		case SPI_TF:
			dev->regs.spi_tf = *(uint8 *)buf;
			WriteFIFO(&dev->tx_fifo, (uint8 *)buf, 1);
			break;
		default:
			printf("read %s error offset %d : 0x%x\n",dev->obj->objname, offset, *(uint16*)buf);
			break;
	}
	return  No_exp;
}

static conf_object_t* new_xdht_spi(char* obj_name)
{
	xdht_spi_dev* xdht_spi = skyeye_mm_zero(sizeof(xdht_spi_dev));

	xdht_spi->obj = new_conf_object(obj_name, xdht_spi);
#if 0
	memory_space_intf* io_memory = skyeye_mm_zero(sizeof(memory_space_intf));
	io_memory->conf_obj = xdht_spi->obj;
	io_memory->read = xdht_spi_read;
	io_memory->write = xdht_spi_write;
	SKY_register_interface((void*)io_memory, obj_name, MEMORY_SPACE_INTF_NAME);
#endif
	CreateFIFO(&xdht_spi->rx_fifo, 256);
	CreateFIFO(&xdht_spi->tx_fifo, 256);

	int  pthread_id;
	create_thread_scheduler(1000, Periodic_sched, hardware_trans, xdht_spi, &pthread_id);
	
	return xdht_spi->obj;
}

static exception_t reset_xdht_spi(conf_object_t* opaque, const char* args)
{
#if 1
	xdht_spi_dev *dev = opaque->obj;
	memset(&(dev->regs), 0, sizeof(dev->regs));

	FreeFIFO(&dev->rx_fifo);
	FreeFIFO(&dev->tx_fifo);
	CreateFIFO(&dev->rx_fifo, 256);
	CreateFIFO(&dev->tx_fifo, 256);
#endif
	return No_exp;        
}


static exception_t free_xdht_spi(conf_object_t* conf_obj)
{
	return No_exp;
}
static attr_value_t get_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	xdht_spi_dev *dev = conf_obj->obj;
	attr_value_t signal_attr = SKY_make_attr_object(dev->signal_obj);
	return signal_attr;
}
static exception_t set_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	xdht_spi_dev *dev = conf_obj->obj;
	dev->signal_obj = SKY_attr_object(*value);
	dev->signal = (general_signal_intf *)SKY_get_iface(dev->signal_obj, GENERAL_SIGNAL_INTF_NAME);
	return No_exp;
}
static attr_value_t get_attr_i2c_master(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	xdht_spi_dev *dev = conf_obj->obj;
	attr_value_t spi_bus_attr = SKY_make_attr_object(dev->spi_bus_obj);
	return spi_bus_attr;
}
static exception_t set_attr_i2c_master(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	xdht_spi_dev *dev = conf_obj->obj;
	dev->spi_bus_obj = SKY_attr_object(*value);
	dev->spi_bus_iface = (spi_bus_intf *)SKY_get_iface(dev->spi_bus_obj, SPI_BUS_INTF_NAME);
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	xdht_spi_dev *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	xdht_spi_dev *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void xdht_spi_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "signal", get_attr_signal, NULL, set_attr_signal, NULL, SKY_Attr_Optional, "object", " interrupt controler  connected");
	SKY_register_attribute(clss, "i2c_master", get_attr_i2c_master, NULL, set_attr_i2c_master, NULL, SKY_Attr_Optional, "object", "i2c master");
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");

}
static void xdht_spi_register_interface(conf_class_t* clss){
	static const memory_space_intf io_memory = {
		.read = xdht_spi_read,
		.write = xdht_spi_write,
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
void init_xdht_spi(){
	static skyeye_class_t class_data = {
		.class_name = "xdht_spi",
		.class_desc = "xdht_spi",
		.new_instance = new_xdht_spi,
		.reset_instance = reset_xdht_spi,
		.free_instance = free_xdht_spi,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	xdht_spi_register_interface(class);
	xdht_spi_register_attribute(class);
}


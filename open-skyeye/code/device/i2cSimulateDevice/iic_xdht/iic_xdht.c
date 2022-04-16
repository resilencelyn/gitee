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
* @file iic_xdht.c
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
#include <skyeye_dev_bus_intf.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"
#include "iic_xdht.h"

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


static void *hardware_trans(void *iic_dev)
{
	xdht_iic_dev * dev = iic_dev;
	uint8_t ch;
#if 0
	if(dev->MASTER_SEND_FLAG == 1){
		ReadFIFO(&dev->tx_fifo, &ch, 1);
		dev->regs.iic_tfor = CheckCanWriteNum(&dev->tx_fifo);
		//printf("write data:%d\n", ch);
		dev->i2c_bus_iface->write_data(dev->i2c_bus_iface->conf_obj, ch);
		dev->write_count--;
		if(dev->write_count <= 0){
			dev->MASTER_SEND_FLAG = 0;
			dev->i2c_bus_iface->stop(dev->i2c_bus_iface->conf_obj);
		}
	}
	if(dev->MASTER_RECEIVE_FLAG == 1){
		ch = dev->i2c_bus_iface->read_data(dev->i2c_bus_iface->conf_obj);
		WriteFIFO(&dev->rx_fifo, &ch, 1);
		dev->regs.iic_rfor = CheckCanReadNum(&dev->rx_fifo);
		dev->read_count--;
		if(dev->read_count <= 0){
			dev->MASTER_RECEIVE_FLAG = 0;
			dev->i2c_bus_iface->stop(dev->i2c_bus_iface->conf_obj);
		}
	}
	dev->i2c_bus_iface->start(dev->i2c_bus_iface->conf_obj, 0x50);
	dev->i2c_bus_iface->write_data(dev->i2c_bus_iface->conf_obj, 0);
	dev->i2c_bus_iface->write_data(dev->i2c_bus_iface->conf_obj, 0);
	dev->i2c_bus_iface->write_data(dev->i2c_bus_iface->conf_obj, 11);
	dev->i2c_bus_iface->write_data(dev->i2c_bus_iface->conf_obj, 22);
	dev->i2c_bus_iface->stop(dev->i2c_bus_iface->conf_obj);
	dev->i2c_bus_iface->start(dev->i2c_bus_iface->conf_obj, (0x50));
	dev->i2c_bus_iface->write_data(dev->i2c_bus_iface->conf_obj, 0);
	dev->i2c_bus_iface->write_data(dev->i2c_bus_iface->conf_obj, 0);
	dev->i2c_bus_iface->stop(dev->i2c_bus_iface->conf_obj);
	dev->i2c_bus_iface->start(dev->i2c_bus_iface->conf_obj, 0x50 | 0x1);
	ch = dev->i2c_bus_iface->read_data(dev->i2c_bus_iface->conf_obj);
	ch = dev->i2c_bus_iface->read_data(dev->i2c_bus_iface->conf_obj);
#endif


	return NULL;
}



#if 0
static exception_t xdht_iic_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	xdht_iic_dev *dev=  opaque->obj;
	i2c_bus_intf * i2c_bus_iface;
	if(!strncmp(attr_name, "signal", strlen("signal"))){
		dev->signal = (general_signal_intf *)SKY_get_iface(value->u.object, GENERAL_SIGNAL_INTF_NAME);
	}else if(!strncmp(attr_name, "i2c_master", strlen("i2c_master"))){
		dev->i2c_bus_iface =  (i2c_bus_intf* )SKY_get_iface(value->u.object, I2C_BUS_INTF_NAME);
	}else{
		printf("parameter error\n");
		return Invarg_exp;
	}
	return No_exp;
}
#endif
static exception_t xdht_iic_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	xdht_iic_dev *dev = opaque->obj;
	switch(offset)
	{
		case IIC_IER:
			*(uint8 *)buf = dev->regs.iic_ier;
			break;
		case IIC_SR:
			*(uint8 *)buf = dev->regs.iic_sr;
			break;
		case IIC_RFOR:
			*(uint8 *)buf = dev->regs.iic_rfor;
			break;
		case IIC_TFOR:
			*(uint8 *)buf = dev->regs.iic_tfor;
			break;
		case IIC_RF:
			ReadFIFO(&dev->rx_fifo, &(dev->regs.iic_rf), 1);
			*(uint8 *)buf = dev->regs.iic_rf;
			break;
                default:

			printf("read %s error offset %d : 0x%x\n",dev->obj->objname, offset, *(uint16*)buf);
			break;
	}       

	return  No_exp;
}

static exception_t xdht_iic_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	xdht_iic_dev *dev = opaque->obj;
	switch(offset)
	{
		case IIC_IER:
			dev->regs.iic_ier = *(uint8 *)buf;
			break;
		case IIC_DLL:
			dev->regs.iic_dll = *(uint8 *)buf;
			break;
		case IIC_DLM:
			dev->regs.iic_dlm = *(uint8 *)buf;
			break;
		case IIC_STWR:
			dev->regs.iic_stwr = *(uint8 *)buf;
			if(dev->regs.iic_stwr & 0x1){
				if(dev->regs.iic_sar & 0x1){       //read start
					dev->i2c_bus_iface->start(dev->i2c_bus_obj, (dev->regs.iic_sar >> 1) | 0x1);
					dev->read_count = dev->regs.iic_dlr;
					dev->i2c_bus_iface->start(dev->i2c_bus_obj, dev->regs.iic_sar >> 1);
					dev->write_count = dev->regs.iic_dlr;

					while(dev->read_count > 0){
						unsigned char ch;
						ch = dev->i2c_bus_iface->read_data(dev->i2c_bus_obj);
						WriteFIFO(&dev->rx_fifo, &ch, 1);
						dev->regs.iic_rfor = CheckCanReadNum(&dev->rx_fifo);
						dev->read_count--;
					}
					dev->i2c_bus_iface->stop(dev->i2c_bus_obj);

					//dev->MASTER_RECEIVE_FLAG = 1;
				}else{  //write start 
					dev->i2c_bus_iface->start(dev->i2c_bus_obj, dev->regs.iic_sar >> 1);
					dev->write_count = dev->regs.iic_dlr;
					while(dev->write_count > 0){
						unsigned char ch;
						ReadFIFO(&dev->tx_fifo, &ch, 1);
						dev->regs.iic_tfor = CheckCanWriteNum(&dev->tx_fifo);
						dev->i2c_bus_iface->write_data(dev->i2c_bus_obj, ch);
						dev->write_count--;
					}
					dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
				}
			}
			break;
		case IIC_MRR:
			dev->regs.iic_mrr = *(uint8 *)buf;
			break;
		case IIC_DLR:
			dev->regs.iic_dlr = *(uint8 *)buf;
			break;
		case IIC_SAR:
			dev->regs.iic_sar = *(uint8 *)buf;
			break;
		case IIC_TF:
			dev->regs.iic_tf = *(uint8 *)buf;
			WriteFIFO(&dev->tx_fifo, (uint8 *)buf, 1);
			break;
		default:
			printf("read %s error offset %d : 0x%x\n",dev->obj->objname, offset, *(uint16*)buf);
			break;
	}
	return  No_exp;
}

static conf_object_t* new_xdht_iic(char* obj_name)
{
	xdht_iic_dev* xdht_iic = skyeye_mm_zero(sizeof(xdht_iic_dev));

	xdht_iic->MASTER_SEND_FLAG = 0;
	xdht_iic->MASTER_RECEIVE_FLAG = 0;
	xdht_iic->write_count = 0;
	xdht_iic->read_count = 0;

	xdht_iic->obj = new_conf_object(obj_name, xdht_iic);
#if 0
	memory_space_intf* io_memory = skyeye_mm_zero(sizeof(memory_space_intf));
	io_memory->conf_obj = xdht_iic->obj;
	io_memory->read = xdht_iic_read;
	io_memory->write = xdht_iic_write;

	SKY_register_interface((void*)io_memory, obj_name, MEMORY_SPACE_INTF_NAME);
#endif
	CreateFIFO(&xdht_iic->rx_fifo, 128);
	CreateFIFO(&xdht_iic->tx_fifo, 128);

	//int  pthread_id;
	//create_thread_scheduler(1000, Periodic_sched, hardware_trans, xdht_iic, &pthread_id);
	
	return xdht_iic->obj;
}

static exception_t reset_xdht_iic(conf_object_t* opaque, const char* args)
{
#if 1
	xdht_iic_dev *dev = opaque->obj;
	memset(&(dev->regs), 0, sizeof(dev->regs));
	ResetFIFO(&dev->rx_fifo);
	ResetFIFO(&dev->tx_fifo);
	dev->MASTER_SEND_FLAG = 0;
	dev->MASTER_RECEIVE_FLAG = 0;
	dev->write_count = 0;
	dev->read_count = 0;

#endif
	return No_exp;        
}


static exception_t free_xdht_iic(conf_object_t* conf_obj)
{

	return No_exp;
}
static void xdht_iic_register_interface(conf_class_t* clss){
	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = xdht_iic_read,
		.write = xdht_iic_write,
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
static attr_value_t get_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	xdht_iic_dev *dev = conf_obj->obj;
	attr_value_t signal_attr = SKY_make_attr_object(dev->signal_obj);
	return signal_attr;
}
static exception_t set_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	xdht_iic_dev *dev = conf_obj->obj;
	dev->signal_obj = SKY_attr_object(*value);
	dev->signal = (general_signal_intf *)SKY_get_iface(dev->signal_obj, GENERAL_SIGNAL_INTF_NAME);
	return No_exp;
}
static attr_value_t get_attr_i2c_master(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	xdht_iic_dev *dev = conf_obj->obj;
	attr_value_t i2c_bus_attr = SKY_make_attr_object(dev->i2c_bus_obj);
	return i2c_bus_attr;
}
static exception_t set_attr_i2c_master(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	xdht_iic_dev *dev = conf_obj->obj;
	dev->i2c_bus_obj = SKY_attr_object(*value);
	dev->i2c_bus_iface = (i2c_bus_intf *)SKY_get_iface(dev->i2c_bus_obj, I2C_BUS_INTF_NAME);
	return No_exp;
}

static void xdht_iic_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "signal", get_attr_signal, NULL, set_attr_signal, NULL, SKY_Attr_Optional, "object", " interrupt controler  connected");
	SKY_register_attribute(clss, "i2c_master", get_attr_i2c_master, NULL, set_attr_i2c_master, NULL, SKY_Attr_Optional, "object", "i2c master");
	return;
}
void init_xdht_iic(){
	static skyeye_class_t class_data = {
		.class_name = "xdht_iic",
		.class_desc = "xdht_iic",
		.new_instance = new_xdht_iic,
		.reset_instance = reset_xdht_iic,
		.free_instance = free_xdht_iic,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	xdht_iic_register_interface(class);
	xdht_iic_register_attribute(class);
}

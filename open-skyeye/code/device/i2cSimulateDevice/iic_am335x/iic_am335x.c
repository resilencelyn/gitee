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
* @file iic_am335x.c
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
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_dev_intf.h>
#include <skyeye_dev_bus_intf.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"
#include "iic_am335x.h"

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
	am335x_iic_dev * dev = iic_dev;
	if(dev->i2c_bus_iface){
	}
	return NULL;
}

static int am335x_i2c_set_state(conf_object_t* opaque, i2c_device_state_t state, uint8_t address)
{
	am335x_iic_dev *dev=  (am335x_iic_dev *)opaque->obj;
	return 0;
}

static uint8_t am335x_i2c_read_data(conf_object_t *opaque){
	am335x_iic_dev *dev=  (am335x_iic_dev *)opaque->obj;
	return 0;
}

static uint8_t am335x_i2c_write_data(conf_object_t *opaque, uint8_t value){
	am335x_iic_dev *dev=  (am335x_iic_dev *)opaque->obj;
	return 0;
}

static uint8_t am335x_i2c_get_address(conf_object_t *opaque)
{
	am335x_iic_dev *dev=  (am335x_iic_dev *)opaque->obj;
	return 0;
}
#if 0
static exception_t am335x_iic_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	am335x_iic_dev *dev=  (am335x_iic_dev *)opaque->obj;
	i2c_bus_intf * i2c_bus_iface;
	if(!strncmp(attr_name, "signal", strlen("signal"))){
		dev->signal = (general_signal_intf *)SKY_get_iface(value->u.object, GENERAL_SIGNAL_INTF_NAME);
	}else if(!strncmp(attr_name, "i2c_master", strlen("i2c_master"))){
		dev->i2c_bus_iface =  (i2c_bus_intf* )SKY_get_iface(value->u.object, I2C_BUS_INTF_NAME);
	}else if(!strncmp(attr_name, "int_number", strlen("int_number"))){
		dev->interrupt_number = value->u.uinteger;
	}else{
		printf("parameter error\n");
		return Invarg_exp;
	}
	
	return No_exp;
}
#endif
static exception_t am335x_iic_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	am335x_iic_dev *dev=  (am335x_iic_dev *)opaque->obj;
	switch(offset)
	{
		case I2C_REVNB_LO:          
			*(uint32_t*)buf = dev->regs.i2c_revnb_lo;
			break;
		case I2C_REVNB_HI :           
			*(uint32_t*)buf = dev->regs.i2c_revnb_hi;            
			break;
		case I2C_SYSC :               
			*(uint32_t*)buf = dev->regs.i2c_sysc;                
			break;
		case I2C_IRQSTATUS_RAW :      
			*(uint32_t*)buf = dev->regs.i2c_irqstatus_raw;       
			break;
		case I2C_IRQSTATUS:
			*(uint32_t*)buf = dev->regs.i2c_irqstatus;       
			break;
		case I2C_IRQENABLE_SET :      
			*(uint32_t*)buf = dev->regs.i2c_irqenable_set;      
			break;
		case I2C_IRQENABLE_CLR :      
			*(uint32_t*)buf = dev->regs.i2c_irqenable_clr;      
			break;
		case I2C_WE:           
			*(uint32_t*)buf = dev->regs.i2c_we;                 
			break;
		case I2C_DMARXENABLE_SET :    
			*(uint32_t*)buf = dev->regs.i2c_dmarxenable_set;    
			break;
		case I2C_DMATXENABLE_SET  :   
			*(uint32_t*)buf = dev->regs.i2c_dmatxenable_set;    
			break;
		case I2C_DMARXENABLE_CLR  :   
			*(uint32_t*)buf = dev->regs.i2c_dmarxenable_clr;    
			break;
		case I2C_DMATXENABLE_CLR :    
			*(uint32_t*)buf = dev->regs.i2c_dmatxenable_clr;    
			break;
		case I2C_DMARXWAKE_EN    :    
			*(uint32_t*)buf = dev->regs.i2c_dmarxwake_en;       
			break;
		case I2C_DMATXWAKE_EN    :    
			*(uint32_t*)buf = dev->regs.i2c_dmatxwake_en;       
			break;
		case I2C_SYSS :    
			*(uint32_t*)buf = dev->regs.i2c_syss;                
			break;
		case I2C_BUF:    
			*(uint32_t*)buf = dev->regs.i2c_buf;         
			break;
		case I2C_CNT             :    
			*(uint32_t*)buf = dev->regs.i2c_cnt;                
			break;
		case I2C_DATA            :    
			*(uint32_t*)buf = dev->regs.i2c_data;                
			dev->regs.i2c_data = dev->i2c_bus_iface->read_data(dev->i2c_bus_obj);
			if((dev->regs.i2c_irqenable_set & 0x8 )&& (dev->read_irq_count < dev->regs.i2c_cnt)){
				dev->regs.i2c_irqstatus |= 0x8;
				dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
				dev->read_irq_count++;
			}
			break;
		case I2C_CON             :    
			*(uint32_t*)buf = dev->regs.i2c_con;                
			break;
		case I2C_OA 		 :
			*(uint32_t*)buf = dev->regs.i2c_oa;
			break;
		case I2C_SA 		 :
			*(uint32_t*)buf = dev->regs.i2c_sa;		 
			break;
		case I2C_PSC 		 :
			*(uint32_t*)buf = dev->regs.i2c_psc;		 
			break;
		case I2C_SCLL 		 :
			*(uint32_t*)buf = dev->regs.i2c_scll;		 
			break;
		case I2C_SCLH 		 :
			*(uint32_t*)buf = dev->regs.i2c_sclh;		 
			break;
		case I2C_SYSTEST 	 :        
			*(uint32_t*)buf = dev->regs.i2c_systest;		 
			break;
		case I2C_BUFSTAT 	 :        
			*(uint32_t*)buf = dev->regs.i2c_bufstat;		 
			break;
		case I2C_OA1 		 :
			*(uint32_t*)buf = dev->regs.i2c_oa1;		 
			break;
		case I2C_OA2  		 :
			*(uint32_t*)buf = dev->regs.i2c_oa2; 		 
			break;
		case I2C_OA3 		 :
			*(uint32_t*)buf = dev->regs.i2c_oa3;		 
			break;
		case I2C_ACTOA 		 :
			*(uint32_t*)buf = dev->regs.i2c_actoa;		 
			break;
		case I2C_SBLOCK      	 :
			*(uint32_t*)buf = dev->regs.i2c_sblock;     	 
			break;
	}       
	return  No_exp;
}

static exception_t am335x_iic_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{

	am335x_iic_dev *dev=  (am335x_iic_dev *)opaque->obj;
	switch(offset)
	{
		case I2C_REVNB_LO:          
			dev->regs.i2c_revnb_lo = *(uint32_t*)buf;
			break;
		case I2C_REVNB_HI :           
			dev->regs.i2c_revnb_hi = *(uint32_t*)buf;            
			break;
		case I2C_SYSC :               
			dev->regs.i2c_sysc = *(uint32_t*)buf;                
			break;
		case I2C_IRQSTATUS_RAW :      
			dev->regs.i2c_irqstatus_raw = *(uint32_t*)buf;       
			break;
		case I2C_IRQSTATUS:
			dev->regs.i2c_irqstatus &= ~(*(uint32_t*)buf);       
			break;
		case I2C_IRQENABLE_SET :      
			dev->regs.i2c_irqenable_set = *(uint32_t*)buf;      
			break;
		case I2C_IRQENABLE_CLR :      
			dev->regs.i2c_irqenable_clr = *(uint32_t*)buf;      
			dev->regs.i2c_irqenable_set &= ~(dev->regs.i2c_irqenable_clr);
			break;
		case I2C_WE       :           
			dev->regs.i2c_we = *(uint32_t*)buf;                 
			break;
		case I2C_DMARXENABLE_SET :    
			dev->regs.i2c_dmarxenable_set = *(uint32_t*)buf;    
			break;
		case I2C_DMATXENABLE_SET  :   
			dev->regs.i2c_dmatxenable_set = *(uint32_t*)buf;    
			break;
		case I2C_DMARXENABLE_CLR  :   
			dev->regs.i2c_dmarxenable_clr = *(uint32_t*)buf;    
			break;
		case I2C_DMATXENABLE_CLR :    
			dev->regs.i2c_dmatxenable_clr = *(uint32_t*)buf;    
			break;
		case I2C_DMARXWAKE_EN    :    
			dev->regs.i2c_dmarxwake_en = *(uint32_t*)buf;       
			break;
		case I2C_DMATXWAKE_EN    :    
			dev->regs.i2c_dmatxwake_en = *(uint32_t*)buf;       
			break;
		case I2C_SYSS             :    
			dev->regs.i2c_syss = *(uint32_t*)buf;                
			break;
		case I2C_BUF:    
			dev->regs.i2c_buf = *(uint32_t*)buf;         
			break;
		case I2C_CNT             :    
			dev->regs.i2c_cnt = *(uint32_t*)buf;                
			break;
		case I2C_DATA            :    
			{
			uint32_t ch;
			dev->regs.i2c_data = *(uint32_t*)buf;                
			ch = dev->regs.i2c_data & 0xff;
			dev->i2c_bus_iface->write_data(dev->i2c_bus_obj, ch);
			if(dev->regs.i2c_irqenable_set & 0x10 && dev->write_irq_count < dev->regs.i2c_cnt){
				dev->regs.i2c_irqstatus |= 0x10;
				dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
				dev->write_irq_count++;
			}
			break;
			}
		case I2C_CON             :    
			dev->regs.i2c_con = *(uint32_t*)buf;                
			if((*(uint32_t*)buf) & 0x1){
				int rw, slave_addr;
				dev->regs.i2c_con &= ~0x1;
				slave_addr = dev->regs.i2c_sa;
				rw = ((dev->regs.i2c_con >> 9) & 0x1);
				dev->regs.i2c_irqstatus_raw |= 0x1000;
				if(rw & 0x1){
					dev->write_irq_count = 0;
					dev->i2c_bus_iface->start(dev->i2c_bus_obj, dev->regs.i2c_sa & (~0x1));
					if(dev->regs.i2c_irqenable_set & 0x10){
						dev->regs.i2c_irqstatus |= 0x10;
						dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
						dev->write_irq_count++;
					}
				}else{
					dev->read_irq_count = 0;
					dev->i2c_bus_iface->start(dev->i2c_bus_obj, dev->regs.i2c_sa | 0x1);
					if(dev->regs.i2c_irqenable_set & 0x8){
						dev->regs.i2c_irqstatus |= 0x8;
						dev->regs.i2c_data = dev->i2c_bus_iface->read_data(dev->i2c_bus_obj);
						dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
						dev->read_irq_count++;
					}
				}
				break;
			}

			if((*(uint32_t*)buf) & 0x2){
				dev->regs.i2c_con &= ~0x2;
				dev->regs.i2c_irqstatus_raw |= 0x4;
				dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
				if(dev->regs.i2c_irqenable_set & 0x100){
					dev->regs.i2c_irqstatus |= 0x100;
					dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
				}
				break;
			}
			break;
		case I2C_OA 		 :
			dev->regs.i2c_oa = *(uint32_t*)buf;
			break;
		case I2C_SA 		 :
			dev->regs.i2c_sa = *(uint32_t*)buf;		 
			break;
		case I2C_PSC 		 :
			dev->regs.i2c_psc = *(uint32_t*)buf;		 
			break;
		case I2C_SCLL 		 :
			dev->regs.i2c_scll = *(uint32_t*)buf;		 
			break;
		case I2C_SCLH 		 :
			dev->regs.i2c_sclh = *(uint32_t*)buf;		 
			break;
		case I2C_SYSTEST 	 :        
			dev->regs.i2c_systest = *(uint32_t*)buf;		 
			break;
		case I2C_BUFSTAT 	 :        
			dev->regs.i2c_bufstat = *(uint32_t*)buf;		 
			break;
		case I2C_OA1:
			dev->regs.i2c_oa1 = *(uint32_t*)buf;		 
			break;
		case I2C_OA2:
			dev->regs.i2c_oa2 = *(uint32_t*)buf; 		 
			break;
		case I2C_OA3 		 :
			dev->regs.i2c_oa3 = *(uint32_t*)buf;		 
			break;
		case I2C_ACTOA 		 :
			dev->regs.i2c_actoa = *(uint32_t*)buf;		 
			break;
		case I2C_SBLOCK      	 :
			dev->regs.i2c_sblock = *(uint32_t*)buf;     	 
			break;
	}
	return  No_exp;
}

static conf_object_t* new_am335x_iic(char* obj_name)
{
	am335x_iic_dev* am335x_iic = skyeye_mm_zero(sizeof(am335x_iic_dev));

	am335x_iic->obj = new_conf_object(obj_name, am335x_iic);
#if 0
	memory_space_intf* io_memory = skyeye_mm_zero(sizeof(memory_space_intf));
	io_memory->conf_obj = am335x_iic->obj;
	io_memory->read = am335x_iic_read;
	io_memory->write = am335x_iic_write;
	SKY_register_interface((void*)io_memory, obj_name, MEMORY_SPACE_INTF_NAME);
#endif

#if 0
	i2c_device_intf *i2c_dev_iface = skyeye_mm_zero(sizeof(i2c_device_intf));
	i2c_dev_iface->set_state = am335x_i2c_set_state;
	i2c_dev_iface->write_data = am335x_i2c_write_data;
	i2c_dev_iface->read_data = am335x_i2c_read_data;
	i2c_dev_iface->get_address = am335x_i2c_get_address;
	SKY_register_interface((void*)i2c_dev_iface, obj_name, I2C_DEVICE_INTF_NAME);
#endif
	int  pthread_id;
	create_thread_scheduler(1000000, Periodic_sched, hardware_trans, am335x_iic, &pthread_id);

	return am335x_iic->obj;
}

static exception_t reset_am335x_iic(conf_object_t* opaque, const char* args)
{
	am335x_iic_dev *dev = opaque->obj;
	memset(&(dev->regs), 0, sizeof(dev->regs));
	return No_exp;        
}


static exception_t free_am335x_iic(conf_object_t* conf_obj)
{

	return No_exp;
}

static void am335x_iic_register_interface(conf_class_t* clss){
	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = am335x_iic_read,
		.write = am335x_iic_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static const i2c_device_intf i2c_dev_iface = {
		.set_state = am335x_i2c_set_state,
		.write_data = am335x_i2c_write_data,
		.read_data = am335x_i2c_read_data,
		.get_address = am335x_i2c_get_address,
	};
	SKY_register_iface(clss, I2C_DEVICE_INTF_NAME, &i2c_dev_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = I2C_DEVICE_INTF_NAME,
			.iface = &i2c_dev_iface,
		},
 	};
	class_register_ifaces(clss, ifaces);
	return;
}
static attr_value_t get_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	am335x_iic_dev *dev = conf_obj->obj;
	attr_value_t signal_attr = SKY_make_attr_object(dev->signal_obj);
	return signal_attr;
}
static exception_t set_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	am335x_iic_dev *dev = conf_obj->obj;
	dev->signal_obj = SKY_attr_object(*value);
	dev->signal = (general_signal_intf *)SKY_get_iface(dev->signal_obj, GENERAL_SIGNAL_INTF_NAME);
	return No_exp;
}
static attr_value_t get_attr_i2c_master(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	am335x_iic_dev *dev = conf_obj->obj;
	attr_value_t i2c_bus_attr = SKY_make_attr_object(dev->i2c_bus_obj);
	return i2c_bus_attr;
}
static exception_t set_attr_i2c_master(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	am335x_iic_dev *dev = conf_obj->obj;
	dev->i2c_bus_obj = SKY_attr_object(*value);
	dev->i2c_bus_iface = (i2c_bus_intf *)SKY_get_iface(dev->i2c_bus_obj, I2C_BUS_INTF_NAME);
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	am335x_iic_dev *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	am335x_iic_dev *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void am335x_iic_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "signal", get_attr_signal, NULL, set_attr_signal, NULL, SKY_Attr_Optional, "objecto", " interrupt controler  connected");
	SKY_register_attribute(clss, "i2c_master", get_attr_i2c_master, NULL, set_attr_i2c_master, NULL, SKY_Attr_Optional, "object", "i2c master");
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");

}

void init_am335x_iic(){
	static skyeye_class_t class_data = {
		.class_name = "am335x_iic",
		.class_desc = "am335x_iic",
		.new_instance = new_am335x_iic,
		.reset_instance = reset_am335x_iic,
		.free_instance = free_am335x_iic,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_am335x SC_am3359),
	};

	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	am335x_iic_register_interface(class);
	am335x_iic_register_attribute(class);
}


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
* @file iic_omapl138.c
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
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_dev_intf.h>
#include <skyeye_dev_bus_intf.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"
#include "iic_omapl138.h"

static FIFO* CreateFIFO(uint32 FIFOLength)
{
	FIFO *fifo = malloc(sizeof(FIFO));
    if(fifo == NULL)
	    return NULL;
    unsigned char  *pfifo;
    pfifo = (unsigned char  *)malloc(FIFOLength);
    if(pfifo == NULL)
	    return NULL;
	fifo->pFirst = pfifo;
	fifo->pLast = pfifo + FIFOLength-1;
	fifo->Length = FIFOLength;
	fifo->pIn     = fifo->pFirst;
	fifo->pOut    = fifo->pFirst;
	fifo->Enteres = 0;  

	return fifo;
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

static int omapl138_i2c_set_state(conf_object_t* opaque, i2c_device_state_t state, uint8_t address)
{
	omapl138_iic_dev *dev=  (omapl138_iic_dev *)opaque->obj;
	return 0;
}

static uint8_t omapl138_i2c_read_data(conf_object_t *opaque){
	omapl138_iic_dev *dev=  (omapl138_iic_dev *)opaque->obj;
	char ch;

	dev->data_count -= 1;
	ReadFIFO(dev->i2c_fifo, &ch, 1);
	if (dev->data_count == 0){
		if (ICMDR_STP) {
			//send STOP condition
			if (dev->i2c_bus_iface != NULL)
				dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
			dev->regs.i2c_icstr &= ~(0x1 << 12); //Bus is free 
			dev->regs.i2c_icivr = I2C_INTCODE_STOP;
			dev->regs.i2c_icmdr &= ~(0x1 << 11); //clear stop bit
			dev->regs.i2c_icstr |= (0x1 << 5); //set stop condition 
			dev->regs.i2c_icstr &= ~(0x1 << 3);//clear ICRRDY

			if (SCD){
				if (dev->signal != NULL)
					dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
			}
		}
	}
	
	return ch;
}

static uint8_t omapl138_i2c_write_data(conf_object_t *opaque, uint8_t value){
	omapl138_iic_dev *dev=  (omapl138_iic_dev *)opaque->obj;

	dev->data_count -= 1;
	WriteFIFO(dev->i2c_fifo, &value, 1);

	if (dev->data_count == 0){
		if (ICMDR_STP) {
			//send STOP condition
			if (dev->i2c_bus_iface != NULL)
				dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
			dev->regs.i2c_icstr &= ~(0x1 << 12); //Bus is free 
			dev->regs.i2c_icivr = I2C_INTCODE_STOP;
			dev->regs.i2c_icmdr &= ~(0x1 << 11); //clear stop bit
			dev->regs.i2c_icstr |= (0x1 << 5); //set stop condition 
			dev->regs.i2c_icstr &= ~(0x1 << 4);//clear ICXRDY

			if (SCD){
				if (dev->signal != NULL)
					dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
			}
		}
	} 

	return 0;
}
static void check_transmit_stop_condition(omapl138_iic_dev *dev){
	if (dev->send_count == dev->regs.i2c_iccnt){
		dev->send_count = 0;
		if (ICMDR_STP) {
			//send STOP condition
			if (dev->i2c_bus_iface != NULL)
				dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
			dev->regs.i2c_icstr &= ~(0x1 << 12); //Bus is free 
			dev->regs.i2c_icivr = I2C_INTCODE_STOP;
			dev->regs.i2c_icmdr &= ~(0x1 << 11); //clear stop bit
			dev->regs.i2c_icstr |= (0x1 << 5); //set stop condition 
			dev->regs.i2c_icstr &= ~(0x1 << 4);//clear ICXRDY

			if (SCD){
				if (dev->signal != NULL)
					dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
			}
		}
	}else {
		if (ICXRDY || ICDMAC_TX){
			dev->regs.i2c_icivr = I2C_INTCODE_ICXRDY;
			if (dev->signal != NULL)
				dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
		}
	}
}

static void check_receive_stop_condition(omapl138_iic_dev *dev){
	dev->data_count -= 1;

	if (dev->data_count == 0){
		if (ICMDR_STP) {
			//send STOP condition
			if (dev->i2c_bus_iface != NULL)
				dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
			dev->regs.i2c_icstr &= ~(0x1 << 12); //Bus is free 
			dev->regs.i2c_icivr = I2C_INTCODE_STOP;
			dev->regs.i2c_icmdr &= ~(0x1 << 11); //clear stop bit
			dev->regs.i2c_icstr |= (0x1 << 5); //set stop condition 
			dev->regs.i2c_icstr &= ~(0x1 << 3);//clear ICRRDY

			if (SCD){
				if (dev->signal != NULL)
					dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
			}
		}
	}
}
static uint8_t omapl138_i2c_get_address(conf_object_t *opaque)
{
	omapl138_iic_dev *dev=  (omapl138_iic_dev *)opaque->obj;
	return 0;
}

static exception_t omapl138_iic_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	omapl138_iic_dev *dev=  (omapl138_iic_dev *)opaque->obj;
	
	switch(offset)
	{
		case I2C_ICOAR:          
			*(uint32_t*)buf = dev->regs.i2c_icoar;
			break;
		case I2C_ICIMR :           
			*(uint32_t*)buf = dev->regs.i2c_icimr;            
			break;
		case I2C_ICSTR :               
			*(uint32_t*)buf = dev->regs.i2c_icstr;                
			break;
		case I2C_ICCLKL :      
			*(uint32_t*)buf = dev->regs.i2c_icclkl;       
			break;
		case I2C_ICCLKH:
			*(uint32_t*)buf = dev->regs.i2c_icclkl;       
			break;
		case I2C_ICCNT :      
			*(uint32_t*)buf = dev->regs.i2c_iccnt;       
			break;
		case I2C_ICDRR :      
			if (dev->i2c_bus_iface != NULL)
				*(uint32_t*)buf = dev->i2c_bus_iface->read_data(dev->i2c_bus_obj);
			check_receive_stop_condition(dev);
			break;
		case I2C_ICSAR :           
			*(uint32_t*)buf = dev->regs.i2c_icsar;      
			break;
		case I2C_ICDXR :    
			*(uint32_t*)buf = dev->regs.i2c_icdxr;    
			break;
		case I2C_ICMDR  :   
			*(uint32_t*)buf = dev->regs.i2c_icmdr;    
			break;
		case I2C_ICIVR  :   
			*(uint32_t*)buf = dev->regs.i2c_icivr;    
			break;
		case I2C_ICEMDR :    
			*(uint32_t*)buf = dev->regs.i2c_icemdr;    
			break;
		case I2C_ICPSC  :    
			*(uint32_t*)buf = dev->regs.i2c_icpsc;    
			break;
		case I2C_REVID1  :    
			*(uint32_t*)buf = dev->regs.i2c_revid1;    
			break;
		case I2C_REVID2  :    
			*(uint32_t*)buf = dev->regs.i2c_revid2;    
			break;
		case I2C_ICDMAC:    
			*(uint32_t*)buf = dev->regs.i2c_icdmac;    
			break;
		case I2C_ICPFUNC :    
			*(uint32_t*)buf = dev->regs.i2c_icpfunc;    
			break;
		case I2C_ICPDIR  :    
			*(uint32_t*)buf = dev->regs.i2c_icpdir;    
			break;
		case I2C_ICPDIN   :    
			*(uint32_t*)buf = dev->regs.i2c_icpdin;    
			break;
		case I2C_ICPDOUT :
			*(uint32_t*)buf = dev->regs.i2c_icpdout;    
			break;
		case I2C_ICPDSET :
			*(uint32_t*)buf = dev->regs.i2c_icpdset;		 
			break;
		case I2C_ICPDCLR :
			*(uint32_t*)buf = dev->regs.i2c_icpdclr;		 
			break;
		default:
			break;
	}       
	return  No_exp;
}

static exception_t omapl138_iic_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{

	omapl138_iic_dev *dev=  (omapl138_iic_dev *)opaque->obj;
	uint32_t val = *(uint32_t*)buf;
	//printf("In %s, offset:%x, val:%x\n", __func__, offset, val);
	switch(offset)
	{
		case I2C_ICOAR:          
			dev->regs.i2c_icoar = val;
			if (ICMDR_XA == 0)
				dev->own_slave_addr = val & 0x7f;
			else 
				dev->own_slave_addr = val & 0x3ff;

			break;
		case I2C_ICIMR :           
			dev->regs.i2c_icimr = val;            
			if ((val & 0x5f) == 0)
				dev->regs.i2c_icivr = 0;
			break;
		case I2C_ICSTR :               
			dev->regs.i2c_icstr = val;                
			break;
		case I2C_ICCLKL :      
			dev->regs.i2c_icclkl = val;       
			break;
		case I2C_ICCLKH:
			dev->regs.i2c_icclkl = val;       
			break;
		case I2C_ICCNT :      
			dev->regs.i2c_iccnt = val;       
			if (ICMDR_RM == 0)
				dev->data_count = val & 0xFFFF;
			break;
		case I2C_ICDRR :      
			dev->regs.i2c_icdrr = val;      
			break;
		case I2C_ICSAR :           
			dev->regs.i2c_icsar = val;      
			if (ICMDR_XA == 0)
				dev->slave_addr = val & 0x7f;
			else 
				dev->slave_addr = val & 0x3ff;
			
			break;
		case I2C_ICDXR :    
			dev->regs.i2c_icdxr = val;    
			{
				dev->send_count += 1;
				if (dev->i2c_bus_iface != NULL)
					dev->i2c_bus_iface->write_data(dev->i2c_bus_obj, val & 0xFF);
				check_transmit_stop_condition(dev);
			}
			break;
		case I2C_ICMDR  :   
			dev->regs.i2c_icmdr = val;    
			if (ICMDR_FDF)
				dev->regs.i2c_icstr |= (0x1 << 9); //AAS set to 1

			if (ICMDR_IRS){
				if (ICMDR_MST){
				//Master mode
					if (ICMDR_STT){ //general a start condition
						if (dev->i2c_bus_iface != NULL)
							dev->i2c_bus_iface->start(dev->i2c_bus_obj, dev->slave_addr & (~0x1));
						if (ICMDR_TRX){
						//transmit data 
							dev->regs.i2c_icstr |= (0x1 << 4);//set ICXRDY
							check_transmit_stop_condition(dev);
						}else{
						//receive data 
							dev->regs.i2c_icstr |= (0x1 << 3);//set ICRRDY
							if (ICRRDY || ICDMAC_RX){
								dev->regs.i2c_icivr = I2C_INTCODE_ICRRDY;
								if (dev->signal != NULL)
									dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
							}
						} 

						dev->regs.i2c_icmdr &= ~(0x1 << 13); //clear start bit 
						dev->regs.i2c_icstr |= (0x1 << 12); //Bus is busy
					}
				}else {
				//Slave mode
					if (ICMDR_STT){ //transmit/receive data from master
						if (dev->i2c_bus_iface != NULL)
							dev->regs.i2c_icdrr = dev->i2c_bus_iface->read_data(dev->i2c_bus_obj);
						if (dev->signal != NULL)
							dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
						dev->read_irq_count++;
						dev->regs.i2c_icstr |= (0x1 << 12); //Bus is busy
					}
				}
			}

			break;
		case I2C_ICIVR  :   
			dev->regs.i2c_icivr = val;    
			break;
		case I2C_ICEMDR :    
			dev->regs.i2c_icemdr = val;    
			break;
		case I2C_ICPSC  :    
			dev->regs.i2c_icpsc = val;    
			break;
		case I2C_REVID1  :    
			dev->regs.i2c_revid1 = val;    
			break;
		case I2C_REVID2  :    
			dev->regs.i2c_revid2 = val;    
			break;
		case I2C_ICDMAC:    
			dev->regs.i2c_icdmac = val;    
			break;
		case I2C_ICPFUNC :    
			dev->regs.i2c_icpfunc = val;    
			break;
		case I2C_ICPDIR  :    
			dev->regs.i2c_icpdir = val;    
			break;
		case I2C_ICPDIN   :    
			dev->regs.i2c_icpdin = val;    
			break;
		case I2C_ICPDOUT :
			dev->regs.i2c_icpdout = val;    
			break;
		case I2C_ICPDSET :
			dev->regs.i2c_icpdset = val;		 
			break;
		case I2C_ICPDCLR :
			dev->regs.i2c_icpdclr = val;		 
			break;
		default:
			break;
	}
	return  No_exp;
}

static char* iic_omapl138_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t iic_omapl138_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	omapl138_iic_dev *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t iic_omapl138_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t iic_omapl138_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	omapl138_iic_dev *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t iic_omapl138_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_omapl138_iic(char* obj_name)
{
	omapl138_iic_dev* omapl138_iic = skyeye_mm_zero(sizeof(omapl138_iic_dev));

	omapl138_iic->obj = new_conf_object(obj_name, omapl138_iic);
	omapl138_iic->regs.i2c_icstr = 0x410;
	omapl138_iic->regs.i2c_revid1 = 0x4415;
	omapl138_iic->regs.i2c_revid2 = 0x6;
	omapl138_iic->regs.i2c_icdmac = 0x3;
	omapl138_iic->i2c_fifo = CreateFIFO(1024);

	return omapl138_iic->obj;
}

static exception_t reset_omapl138_iic(conf_object_t* opaque, const char* args)
{
	omapl138_iic_dev *dev = opaque->obj;
	memset(&(dev->regs), 0, sizeof(dev->regs));
	return No_exp;        
}


static exception_t free_omapl138_iic(conf_object_t* opaque)
{
	omapl138_iic_dev *dev = opaque->obj;
	FreeFIFO(dev->i2c_fifo);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	omapl138_iic_dev *dev = obj->obj;
	dev->signal_obj = obj2;
	dev->signal = (general_signal_intf *)SKY_get_iface(dev->signal_obj, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal == NULL){
		if (dev->signal_obj == NULL)
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
	omapl138_iic_dev *dev = obj->obj;
	*obj2 = dev->signal_obj;
	*port = NULL;
	return No_exp;
}

static exception_t get_attr_i2c_master(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	omapl138_iic_dev *dev = obj->obj;
	*obj2 = dev->i2c_bus_obj;
	*port = NULL;
	return No_exp;
}
static exception_t set_attr_i2c_master(conf_object_t *obj, conf_object_t *obj2, const char *port, int index){
	omapl138_iic_dev *dev = obj->obj;
	dev->i2c_bus_obj = obj2;
	dev->i2c_bus_iface = (i2c_bus_intf *)SKY_get_iface(dev->i2c_bus_obj, I2C_BUS_INTF_NAME);
	if (dev->i2c_bus_iface == NULL){
		if (dev->i2c_bus_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", I2C_BUS_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", I2C_BUS_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	omapl138_iic_dev *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	omapl138_iic_dev *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void omapl138_iic_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");

}

void init_omapl138_iic(){
	static skyeye_class_t class_data = {
		.class_name = "omapl138_iic",
		.class_desc = "omapl138_iic",
		.new_instance = new_omapl138_iic,
		.reset_instance = reset_omapl138_iic,
		.free_instance = free_omapl138_iic,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_omapl138 SC_c6748),
	};
	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = omapl138_iic_read,
		.write = omapl138_iic_write,
	};
	SKY_register_iface(class, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const i2c_device_intf i2c_dev_iface = {
		.set_state = omapl138_i2c_set_state,
		.write_data = omapl138_i2c_write_data,
		.read_data = omapl138_i2c_read_data,
		.get_address = omapl138_i2c_get_address,
	};
	SKY_register_iface(class, I2C_DEVICE_INTF_NAME, &i2c_dev_iface);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = iic_omapl138_get_regvalue_by_id,
		.get_regname_by_id = iic_omapl138_get_regname_by_id,
		.set_regvalue_by_id = iic_omapl138_set_regvalue_by_id,
		.get_regnum = iic_omapl138_get_regnum,
		.get_regid_by_name = iic_omapl138_get_regid_by_name
	};
	SKY_register_iface(class, SKYEYE_REG_INTF, &reg_intf);

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
			.name = I2C_DEVICE_INTF_NAME,
			.iface = &i2c_dev_iface
		},
	};
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = I2C_BUS_INTF_NAME,
			.set = set_attr_i2c_master,
			.get = get_attr_i2c_master,
		},
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		}
	};
	class_register_ifaces(class, ifaces);
	class_register_connects(class, connects);
	omapl138_iic_register_attribute(class);
}


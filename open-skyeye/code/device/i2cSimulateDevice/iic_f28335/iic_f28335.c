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
* @file iic_f28335.c
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
#include "iic_f28335.h"

static void check_transmit_stop_condition(f28335_iic_dev *dev){
	char addr;
	int i;
	if (ICMDR_STP) {
		//send STOP condition
		dev->regs.i2c_icstr.bit.BB = 0; //Bus is free 
		dev->regs.i2c_icisrc.all = I2C_INTCODE_STOP;
		dev->regs.i2c_icmdr.bit.STP = 0; //clear stop bit
		dev->regs.i2c_icstr.bit.SCD = 0x1; //set stop condition 
		dev->regs.i2c_icstr.bit.XRDY = 0;//clear ICXRDY

		if (dev->i2c_bus_iface != NULL){
			for (i = 0; i < dev->w_cnt;i++){
				dev->i2c_bus_iface->write_data(dev->i2c_bus_obj, dev->fifo_buffer[i]);
			}
		}
		if (ICSCD){
			if (dev->signal != NULL)
				dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
		}
	}else {
		dev->regs.i2c_icstr.bit.BB = 0; //Bus is free 
		dev->regs.i2c_icisrc.all = I2C_INTCODE_ARDY;
		dev->regs.i2c_icstr.bit.XRDY = 0;//clear ICXRDY

		if (ICARDY){
			if (dev->signal != NULL)
				dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
		}
	}
}

static void check_receive_stop_condition(f28335_iic_dev *dev){
	if (ICMDR_STP) {
		//send STOP condition
		dev->regs.i2c_icstr.bit.BB = 0; //Bus is free 
		dev->regs.i2c_icisrc.all = I2C_INTCODE_STOP;
		dev->regs.i2c_icmdr.bit.STP = 0; //clear stop bit
		dev->regs.i2c_icstr.bit.SCD = 0x1; //set stop condition 
		dev->regs.i2c_icstr.bit.RRDY = 0;//clear ICRRDY

		if (ICSCD){
			if (dev->signal != NULL)
				dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
		}
	}
}

static exception_t f28335_iic_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	f28335_iic_dev *dev=  (f28335_iic_dev *)opaque->obj;
	char ch;
	
	switch(offset)
	{
		case I2COAR:          
			*(uint16_t*)buf = dev->regs.i2c_icoar;
			break;
		case I2CIER :           
			*(uint16_t*)buf = dev->regs.i2c_icier.all;            
			break;
		case I2CSTR :               
			*(uint16_t*)buf = dev->regs.i2c_icstr.all;                
			break;
		case I2CCLKL :      
			*(uint16_t*)buf = dev->regs.i2c_icclkl;       
			break;
		case I2CCLKH:
			*(uint16_t*)buf = dev->regs.i2c_icclkh;       
			break;
		case I2CCNT :      
			*(uint16_t*)buf = dev->regs.i2c_iccnt;       
			break;
		case I2CDRR :      
			if (dev->i2c_bus_iface != NULL){
				*(uint16_t*)buf = dev->i2c_bus_iface->read_data(dev->i2c_bus_obj);
			}
			break;
		case I2CSAR :           
			*(uint16_t*)buf = dev->regs.i2c_icsar;      
			break;
		case I2CDXR :    
			*(uint16_t*)buf = dev->regs.i2c_icdxr;    
			break;
		case I2CMDR  :   
			*(uint16_t*)buf = dev->regs.i2c_icmdr.all;    
			break;
		case I2CISRC  :   
			*(uint16_t*)buf = dev->regs.i2c_icisrc.all;    
			break;
		case I2CPSC  :    
			*(uint16_t*)buf = dev->regs.i2c_icpsc;    
			break;
		case I2CFFTX  :    
			*(uint16_t*)buf = dev->regs.i2c_icfftx.all;    
			break;
		case I2CFFRX  :    
			*(uint16_t*)buf = dev->regs.i2c_icffrx.all;    
			break;
		default:
			printf("In %s, %d, can't read register offset is %x\n", __func__, __LINE__, offset);
			break;
	}       
	return  No_exp;
}

static exception_t f28335_iic_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{

	f28335_iic_dev *dev=  (f28335_iic_dev *)opaque->obj;
	uint16_t val = *(uint16_t*)buf;
	switch(offset)
	{
		case I2COAR:          
			dev->regs.i2c_icoar = val;
			if (ICMDR_XA == 0)
				dev->own_slave_addr = val & 0x7f;
			else 
				dev->own_slave_addr = val & 0x3ff;

			break;
		case I2CIER :           
			dev->regs.i2c_icier.all = val;            
			if ((val & 0x5f) == 0)
				dev->regs.i2c_icier.all = 0;
			break;
		case I2CSTR :               
			dev->regs.i2c_icstr.all = val;                
			break;
		case I2CCLKL :      
			dev->regs.i2c_icclkl = val;       
			break;
		case I2CCLKH:
			dev->regs.i2c_icclkh = val;       
			break;
		case I2CCNT :      
			dev->regs.i2c_iccnt = val;       
			if (ICMDR_RM == 0)
				dev->data_count = val & 0xFFFF;
			break;
		case I2CDRR :      
			dev->regs.i2c_icdrr = val;      
			break;
		case I2CSAR :           
			dev->regs.i2c_icsar = val;      
			if (ICMDR_XA == 0)
				dev->slave_addr = val & 0x7f;
			else 
				dev->slave_addr = val & 0x3ff;
			
			break;
		case I2CDXR :    
			if (dev->regs.i2c_icfftx.bit.I2CFFEN){
				dev->fifo_buffer[dev->w_cnt] = val;
				dev->w_cnt += 1;
			}
			else 
				dev->regs.i2c_icdxr = val;    
			break;
		case I2CMDR  :   
			dev->regs.i2c_icmdr.all = val;    
			if (ICMDR_FDF)
				dev->regs.i2c_icstr.bit.AAS = 0x1; //AAS set to 1

			if (ICMDR_IRS){
				if (ICMDR_MST){ //general a start condition 
						if (dev->i2c_bus_iface != NULL)
							dev->i2c_bus_iface->start(dev->i2c_bus_obj, dev->slave_addr & (~0x1));
						if (ICMDR_TRX){
						//transmit data 
							dev->regs.i2c_icstr.bit.XRDY = 0x1;//set ICXRDY
							if (ICXRDY){
								dev->regs.i2c_icisrc.all = I2C_INTCODE_ICXRDY;
								dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
							}
							check_transmit_stop_condition(dev);
						}else{
						//receive data 
							dev->regs.i2c_icstr.bit.RRDY = 0x1;//set ICRRDY
							if (ICRRDY){
								dev->regs.i2c_icisrc.all = I2C_INTCODE_ICRRDY;
								dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
							}
							check_receive_stop_condition(dev);
						} 

						dev->regs.i2c_icmdr.bit.STT = 0; //clear start bit 
				}else {
				//Slave mode
					if (ICMDR_STT){ //transmit/receive data from master
						if (dev->i2c_bus_iface != NULL)
							dev->regs.i2c_icdrr = dev->i2c_bus_iface->read_data(dev->i2c_bus_obj);
						dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
						dev->regs.i2c_icstr.bit.BB = 0x1; //Bus is busy
					}
				}
			}
			break;
		case I2CISRC  :   
			dev->regs.i2c_icisrc.all = val;    
			break;
		case I2CPSC  :    
			dev->regs.i2c_icpsc = val;    
			break;
		case I2CFFTX  :    
			dev->regs.i2c_icfftx.all = val;    
			break;
		case I2CFFRX  :    
			dev->regs.i2c_icffrx.all = val;    
			break;
		default:
			break;
	}
	return  No_exp;
}

static char* iic_f28335_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static uint32_t iic_f28335_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}
static exception_t iic_f28335_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	f28335_iic_dev *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint16_t*)regs + id) = value;
	return No_exp;
}
static uint32_t iic_f28335_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 2;
}
static uint32_t iic_f28335_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	f28335_iic_dev *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint16_t* regs_value = (uint16_t*)regs + id;
	return *regs_value;
}
static uint32_t iic_f28335_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 2, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_f28335_iic(char* obj_name)
{
	f28335_iic_dev* f28335_iic = skyeye_mm_zero(sizeof(f28335_iic_dev));

	f28335_iic->obj = new_conf_object(obj_name, f28335_iic);
	f28335_iic->w_cnt = 0;
	f28335_iic->r_cnt = 0;

	return f28335_iic->obj;
}

static exception_t reset_f28335_iic(conf_object_t* opaque, const char* args)
{
	f28335_iic_dev *dev = opaque->obj;
	memset(&(dev->regs), 0, sizeof(dev->regs));
	return No_exp;        
}


static exception_t free_f28335_iic(conf_object_t* opaque)
{
	f28335_iic_dev *dev = opaque->obj;
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	f28335_iic_dev *dev = obj->obj;
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
	f28335_iic_dev *dev = obj->obj;
	*obj2 = dev->signal_obj;
	*port = NULL;
	return No_exp;
}

static exception_t get_attr_i2c_bus(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	f28335_iic_dev *dev = obj->obj;
	*obj2 = dev->i2c_bus_obj;
	*port = NULL;
	return No_exp;
}
static exception_t set_attr_i2c_bus(conf_object_t *obj, conf_object_t *obj2, const char *port, int index){
	f28335_iic_dev *dev = obj->obj;
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
	f28335_iic_dev *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	f28335_iic_dev *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void f28335_iic_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");

}

void init_f28335_iic(){
	static skyeye_class_t class_data = {
		.class_name = "f28335_iic",
		.class_desc = "f28335_iic",
		.new_instance = new_f28335_iic,
		.reset_instance = reset_f28335_iic,
		.free_instance = free_f28335_iic,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_f28335),
	};
	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = f28335_iic_read,
		.write = f28335_iic_write,
	};
	SKY_register_iface(class, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = iic_f28335_get_regvalue_by_id,
		.get_regname_by_id = iic_f28335_get_regname_by_id,
		.set_regvalue_by_id = iic_f28335_set_regvalue_by_id,
		.get_regnum = iic_f28335_get_regnum,
		.get_regid_by_name = iic_f28335_get_regid_by_name,
		.get_regoffset_by_id = iic_f28335_get_regoffset_by_id
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
	};
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = I2C_BUS_INTF_NAME,
			.set = set_attr_i2c_bus,
			.get = get_attr_i2c_bus,
		},
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		}
	};
	class_register_ifaces(class, ifaces);
	class_register_connects(class, connects);
	f28335_iic_register_attribute(class);
}


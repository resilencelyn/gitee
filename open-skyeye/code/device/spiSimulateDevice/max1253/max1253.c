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
* @file max1253.c
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-12-06
*/

#include <stdio.h>
#include <stdlib.h>
#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_exec.h>
#include <skyeye_cell.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_log.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_dev_intf.h>
#include "skyeye_thread.h"
#include "max1253.h"


int reset(conf_object_t *opaque)
{
	int i;
	max1253_dev *dev=  (max1253_dev *)opaque->obj;
	for(i = 0; i < 10; i++){
		dev->regs.ain[i] = 0x80;
	}
	return 0;
}

void max1253_spi_request(conf_object_t *opaque, int first, int last, char *buf, int len, char *feedback)
{
	max1253_dev *dev=  (max1253_dev *)opaque->obj;
	if(first == 1){
		dev->command = ((*buf) & 0xf0) >> 4;
		dev->addr = ((*buf) & 0xf);
		switch(dev->command){
			case 0x0:     //Manually Trigged Conversion
				break;
			case 0x1:      //Read Alarm Register
				break;
			case 0x2:      //Read Current Data Register for Selected Channel
				*feedback = 5;
				dev->read_num = 5;
				dev->read_flag = 1;
				break;
			case 0x3:      //Read Current Data Register for All Channel
				*feedback = 20;
				dev->read_num = 20;
				dev->read_flag = 1;
				int i;
				for(i = 0; i < 20; i += 2){
					dev->all_channel_data[i] =  (dev->regs.ain[i/2] & 0xf0) >> 4;
					dev->all_channel_data[i + 1] = dev->regs.ain[i/2] & 0xf ;
				}
				break;
			case 0x4:      //Read Configuration Register for Selected Channel
				*feedback = 5;
				dev->read_num = 5;
				*buf = 0x0;
				break;
			case 0x5:      //Read Global Configuration Registers break;
				break;
			case 0x6:      //Reseverd
				break;
			case 0x7:      //Reset 
				reset(opaque);
				break;
			case 0x8:      //Clear Alarm/Fault for Selected Channel
				break;
			case 0x9:      //Clear Alarm/Fault for All Channel
				break;
			case 0xa:      //Write Current Data Registers for Selected Channel
				break;
			case 0xb:      //Write Current Data Registers for All Channel
				break;
			case 0xc:      //Write Configuration Register for Selected Channel
				dev->write_num = 5;
				dev->read_flag = 0;
				break;
			case 0xd:   //Write Global Configuration Registers
				dev->write_num = 5;
				dev->read_flag = 0;
				break;
			case 0xe:
				//Reserved
				break;
			case 0xf:
				//Reserved
				break;
		}

	}else{
		switch(dev->command){
			case 0x2:      //Read Current Data Register for Selected Channel
				switch(dev->write_num){
					case 5:
						*buf = dev->regs.chanel_upper_thr[dev->addr] >> 4;
						break;
					case 4:
						*buf = dev->regs.chanel_upper_thr[dev->addr]& 0xf;
						break;
					case 3:
						*buf = dev->regs.chanel_lower_thr[dev->addr] >> 4;
						break;
					case 2:
						*buf = dev->regs.chanel_lower_thr[dev->addr] & 0xf;
						break;
					case 1:
						*buf = dev->regs.chanel_conf;
						break;
				}
				dev->write_num -= 0;
				break;
			case 0x3:      //Read Current Data Register for All Channel
				*buf = dev->all_channel_data[20 - dev->read_num] & 0xff;
				dev->read_num -= 1;
					break;
			case 0x4:   //Read Global Configuration Registers
				switch(dev->read_num){
					case 5:
						*buf = (dev->regs.chanel_upper_thr[dev->addr] & 0xff00) >> 8;
					//	printf("read buf :%x\n", *buf & 0xff);
						break;
					case 4:
						*buf = dev->regs.chanel_upper_thr[dev->addr] & 0xff;
					//	printf("read buf :%x\n", *buf & 0xff);
						break;
					case 3:
						*buf = (dev->regs.chanel_lower_thr[dev->addr] & 0xff00) >> 8;
					//	printf("read buf :%x\n", *buf & 0xff);
						break;
					case 2:
						*buf = dev->regs.chanel_lower_thr[dev->addr] & 0xff;
					//	printf("read buf :%x\n", *buf & 0xff);
						break;
					case 1:
						*buf = dev->regs.chanel_conf[dev->addr];
					//	printf("read buf :%x\n", *buf);
						break;
				}
				dev->read_num -= 1;
				break;
			case 0xd:   //Write Global Configuration Registers
				switch(dev->write_num){
					case 5:
						dev->regs.ch_en &= ~0xff00;
						dev->regs.ch_en |= *buf << 8;
						break;
					case 4:
						dev->regs.ch_en &= ~0x00ff;
						dev->regs.ch_en |= (*buf)&0xff;
						break;
					case 3:
						dev->regs.input_conf &= ~0xff00;
						dev->regs.input_conf |= *buf << 8;
						break;
					case 2:
						dev->regs.input_conf &= ~0x00ff;
						dev->regs.input_conf |= (*buf)&0xff;
						break;
					case 1:
						dev->regs.step = *buf;
						break;
				}
				dev->write_num -= 1;
				break;
			case 0xc:
				switch(dev->write_num){
					case 5:
						dev->regs.chanel_upper_thr[dev->addr] &= ~0xff00;
						dev->regs.chanel_upper_thr[dev->addr] |= *buf << 8;
						break;
					case 4:
						dev->regs.chanel_upper_thr[dev->addr] &= ~0x00ff;
						dev->regs.chanel_upper_thr[dev->addr] |= (*buf) & 0xff;
						break;
					case 3:
						dev->regs.chanel_lower_thr[dev->addr] &= ~0xff00;
						dev->regs.chanel_lower_thr[dev->addr] |= *buf << 8;
						break;
					case 2:
						dev->regs.chanel_lower_thr[dev->addr] &= ~0x00ff;
						dev->regs.chanel_lower_thr[dev->addr] |= (*buf) & 0xff;
						break;
					case 1:
							dev->regs.chanel_conf[dev->addr] = *buf;
						
						break;
				}
				dev->write_num -= 1;
				break;
		}
	}

}

int max1253_connect_master(conf_object_t *opaque)
{
	max1253_dev *dev=  (max1253_dev *)opaque->obj;
	return 0;

}
#if 0
static exception_t max1253_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	max1253_dev *dev=  (max1253_dev *)opaque->obj;
	return No_exp;
}
#endif
static conf_object_t* new_max1253(char* obj_name)
{
	max1253_dev* dev = skyeye_mm_zero(sizeof(max1253_dev));
	dev->obj = new_conf_object(obj_name, dev);
#if 0
	spi_device_intf *spi_dev_iface = skyeye_mm_zero(sizeof(spi_device_intf));
	spi_dev_iface->spi_request = max1253_spi_request;
	spi_dev_iface->connect_master = max1253_connect_master;
	SKY_register_interface((void*)spi_dev_iface, obj_name, SPI_DEVICE_INTF_NAME);
#endif
	return dev->obj;
}

static exception_t reset_max1253(conf_object_t* opaque, const char* args)
{
	max1253_dev *dev = opaque->obj;
	return No_exp;        
}


static exception_t free_max1253(conf_object_t* conf_obj)
{
	return No_exp;
}
static void max1253_register_interface(conf_class_t* class){
	static const spi_device_intf spi_dev_iface = {
		.spi_request = max1253_spi_request,
		.connect_master = max1253_connect_master,
	};
	SKY_register_iface(class, SPI_DEVICE_INTF_NAME, &spi_dev_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SPI_DEVICE_INTF_NAME,
			.iface = &spi_dev_iface,
		}
 	};
	class_register_ifaces(class, ifaces);

	return;
}
void init_max1253(){
	static skyeye_class_t class_data = {
		.class_name = "max1253",
		.class_desc = "max1253",
		.new_instance = new_max1253,
		.reset_instance = reset_max1253,
		.free_instance = free_max1253,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	max1253_register_interface(class);
}

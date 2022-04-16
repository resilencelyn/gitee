/* Copyright (C) 
* 2011 - Michael.Kang blackfin.kang@gmail.com
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
* @file gpio_f28335.c
* @brief The implementation of system controller
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_attribute.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h> 
#include <skyeye_core.h>
#include <memory_space.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>

#include "skyeye_lock.h"
#include "gpio_f28335.h"

#define RISING_EDGE  1
#define FALLING_EDGE 0

#define WAKE_UP_INT 8

static exception_t gpio_pin_set_data(conf_object_t *obj, uint32_t pin_num, int status)
{
	f28335_gpio_device *dev = obj->obj;
	gpio_reg_t *regs = dev->regs;
	int mode;

	mode = dev->gpio_group[pin_num].mode;
	RW_WRLOCK(dev->lock);
	if (pin_num < 32){
		if (mode == INPUT_MODE){
			GPIOA_WR(&status, GPADAT, pin_num);
		}
	}else if (pin_num < 64){
		if (mode == INPUT_MODE){
			GPIOB_WR(&status, GPBDAT, pin_num);
		}
	}else if (pin_num < MAX_GPIO_NUM){
		if (mode == INPUT_MODE){
			GPIOC_WR(&status, GPCDAT, pin_num);
		}
	}else {
		printf("In %s, Pin_num=%d is too larger than MAX_PIN_NUM=%d\n", __func__, pin_num, MAX_GPIO_NUM);
	}
	RW_UNLOCK(dev->lock);

    	return No_exp;
}

int connect_gpio(conf_object_t *device, conf_object_t *gpio, uint32_t value){
}

void get_wake_up_pin(conf_object_t *obj, int gpio_val){
	f28335_gpio_device *dev = obj->obj;
	int i;
	for (i = 0; i < 32; i++){
		if ((gpio_val >> i) & 0x1){
			dev->gpio_group[i].wake_up = 1;
		}
	}
}

void set_output_io_value(conf_object_t *obj, int gpio_group, int gpio_val){
	f28335_gpio_device *dev = obj->obj;
	gpio_reg_t *regs = dev->regs;
	int i, io_id, io_state;
	
	for (i = 0; i < 16; i++){
		io_id = gpio_group * 16 + i;
		io_state = (gpio_val >> i) & 0x1;
		if (dev->gpio_group[io_id].mode == OUTPUT_MODE){
			if (io_id < 32){
				GPIOA_WR(&io_state, GPADAT, io_id);
			}else if (io_id < 64){
				GPIOB_WR(&io_state, GPBDAT, io_id);
			}else if (io_id < MAX_GPIO_NUM){
				GPIOC_WR(&io_state, GPCDAT, io_id);
			}else {
				printf("In %s, Set Gpio pin=%d Error!\n", __func__, io_id);
				return;
			}
		}
	}
}

void get_io_high_pin(conf_object_t *obj, int gpio_group, int gpio_val){
	f28335_gpio_device *dev = obj->obj;
	int i, j, k, io_id;
	int dev_num = dev->dev_list.dev_num;
	for (i = 0; i < 16; i++){
		if ((gpio_val >> i) & 0x1){
			io_id = gpio_group * 16 + i;
			dev->gpio_group[io_id].high_pin = HIGH;
			if (dev->gpio_group[io_id].low_pin == HIGH && dev->gpio_group[io_id].mode == INPUT_MODE){
			//if (dev->gpio_group[io_id].low_pin == HIGH){
				dev->gpio_group[io_id].high_pin = LOW;
				dev->gpio_group[io_id].low_pin = LOW;

				if (dev->gpio_group[io_id].wake_up){
					if (dev->signal_iface){
						dev->signal_iface->raise_signal(dev->signal, WAKE_UP_INT);
					}
				}
				for (j=0;j<dev_num;j++){
					if (dev->dev_list.device[j].connector_device_iface){
						for (k=0;k<dev->cnt;k++)
							dev->dev_list.device[j].connector_device_iface->connect_device(obj, dev->dev_list.device[j].dev, dev->channel[k], RISING_EDGE);
					}
				}
				//rising edge
			}
		}
	}
}

void get_io_low_pin(conf_object_t *obj, int gpio_group, int gpio_val){
	f28335_gpio_device *dev = obj->obj;
	int i, j, k, io_id;
	int dev_num = dev->dev_list.dev_num;
	for (i = 0; i < 16; i++){
		if ((gpio_val >> i) & 0x1){
			io_id = gpio_group * 16 + i;
			dev->gpio_group[io_id].low_pin = HIGH;
			if (dev->gpio_group[io_id].high_pin == HIGH && dev->gpio_group[io_id].mode == INPUT_MODE){
			//if (dev->gpio_group[io_id].high_pin == HIGH){
				dev->gpio_group[io_id].low_pin = LOW;
				dev->gpio_group[io_id].high_pin = LOW;

				if (dev->gpio_group[io_id].wake_up){
					if (dev->signal_iface){
						dev->signal_iface->raise_signal(dev->signal, WAKE_UP_INT);
					}
				}

				for (j=0;j<dev_num;j++){
					if (dev->dev_list.device[j].connector_device_iface){
						for (k=0;k<dev->cnt;k++)
						dev->dev_list.device[j].connector_device_iface->connect_device(obj, dev->dev_list.device[j].dev, dev->channel[k],FALLING_EDGE);
					}
				}
				//falling edge
			}
		}
	}
}

void get_io_output_pin(conf_object_t *obj, int gpio_group, int gpio_val){
	f28335_gpio_device *dev = obj->obj;
	int i, io_id;
	for (i = 0; i < 16; i++){
		if ((gpio_val >> i) & 0x1){
			io_id = gpio_group * 16 + i;
			dev->gpio_group[io_id].mode = OUTPUT_MODE;
		}else {
            		io_id = gpio_group * 16 + i;
            		dev->gpio_group[io_id].mode = INPUT_MODE;
        	}
	}
}



exception_t update_gpio(conf_object_t *obj, uint32_t regvalue){
	f28335_gpio_device *dev = obj->obj;
	gpio_reg_t* regs = dev->regs;
	regs->GPADAT.all = regvalue;
	return No_exp;
}

exception_t update_gpio_on_line(conf_object_t *obj, uint32_t regvalue){
	f28335_gpio_device *dev = obj->obj;
	gpio_reg_t* regs = dev->regs;
	if(dev->gpio_connector && dev->connector_iface){
		dev->connector_iface->update(dev->gpio_connector, regvalue);
	}
	if(dev->gpio_gate_a && dev->gate_a_iface){
		dev->gate_a_iface->update(dev->gpio_gate_a, regvalue);
	}
	if(dev->gpio_gate_b && dev->gate_b_iface){
		dev->gate_b_iface->update(dev->gpio_gate_b, regvalue);
	}
	return No_exp;
}

static exception_t f28335_gpio_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct f28335_gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	
	RW_RDLOCK(dev->lock);
	switch(offset) {
		case 0x0:
			*(uint16_t*)buf = regs->GPACTRL.GPA_LSB; 
			break;
		case 0x1:
			*(uint16_t*)buf = regs->GPACTRL.GPA_MSB; 
			break;
		case 0x2:
			*(uint16_t*)buf = regs->GPAQSEL1.GPA1_LSB;
			break;
		case 0x3:
			*(uint16_t*)buf = regs->GPAQSEL1.GPA1_MSB;
			break;
		case 0x4:
			*(uint16_t*)buf = regs->GPAQSEL2.GPA2_LSB;
			break;
		case 0x5:
			*(uint16_t*)buf = regs->GPAQSEL2.GPA2_MSB;
			break;
		case 0x6:
			*(uint16_t*)buf = regs->GPAMUX1.GPA1_LSB;
			break;
		case 0x7:
			*(uint16_t*)buf = regs->GPAMUX1.GPA1_MSB;
			break;
		case 0x8:
			*(uint16_t*)buf = regs->GPAMUX2.GPA2_LSB;
			break;
		case 0x9:
			*(uint16_t*)buf = regs->GPAMUX2.GPA2_MSB;
			break;
		case 0xa:
			*(uint16_t*)buf = regs->GPADIR.GPA_LSB;
			break;
		case 0xb:
			*(uint16_t*)buf = regs->GPADIR.GPA_MSB;
			break;
		case 0xc:
			*(uint16_t*)buf = regs->GPAPUD.GPA_LSB;
			break;
		case 0xd:
			*(uint16_t*)buf = regs->GPAPUD.GPA_MSB;
			break;
		case 0x10:
			*(uint16_t*)buf = regs->GPBCTRL.GPB_LSB;
			break;
		case 0x11:
			*(uint16_t*)buf = regs->GPBCTRL.GPB_MSB;
			break;
		case 0x12:
			*(uint16_t*)buf = regs->GPBQSEL1.GPB1_LSB;
			break;
		case 0x13:
			*(uint16_t*)buf = regs->GPBQSEL1.GPB1_MSB;
			break;
		case 0x14:
			*(uint16_t*)buf = regs->GPBQSEL2.GPB2_LSB;
			break;
		case 0x15:
			*(uint16_t*)buf = regs->GPBQSEL2.GPB2_MSB;
			break;
		case 0x16:
			*(uint16_t*)buf = regs->GPBMUX1.GPB1_LSB;
			break;
		case 0x17:
			*(uint16_t*)buf = regs->GPBMUX1.GPB1_MSB;
			break;
		case 0x18:
			*(uint16_t*)buf = regs->GPBMUX2.GPB2_LSB;
			break;
		case 0x19:
			*(uint16_t*)buf = regs->GPBMUX2.GPB2_MSB;
			break;
		case 0x1a:
			*(uint16_t*)buf = regs->GPBDIR.GPB_LSB;
			break;
		case 0x1b:
			*(uint16_t*)buf = regs->GPBDIR.GPB_MSB;
			break;
		case 0x1c:
			*(uint16_t*)buf = regs->GPBPUD.GPB_LSB;
			break;
		case 0x1d:
			*(uint16_t*)buf = regs->GPBPUD.GPB_MSB;
			break;
		case 0x26:
			*(uint16_t*)buf = regs->GPCMUX1.GPC1_LSB;
			break;
		case 0x27:
			*(uint16_t*)buf = regs->GPCMUX1.GPC1_MSB;
			break;
		case 0x28:
			*(uint16_t*)buf = regs->GPCMUX2.GPC2_LSB;
			break;
		case 0x29:
			*(uint16_t*)buf = regs->GPCMUX2.GPC2_MSB;
			break;
		case 0x2a:
			*(uint16_t*)buf = regs->GPCDIR.GPC_LSB;
			break;
		case 0x2b:
			*(uint16_t*)buf = regs->GPCDIR.GPC_MSB;
			break;
		case 0x2c:
			*(uint16_t*)buf = regs->GPCPUD.GPC_LSB;
			break;
		case 0x2d:
			*(uint16_t*)buf = regs->GPCPUD.GPC_MSB;
			break;
		case 0x40:
			*(uint16_t*)buf = regs->GPADAT.GPA_LSB;
			break;
		case 0x41:
			*(uint16_t*)buf = regs->GPADAT.GPA_MSB;
			break;
		case 0x42:
			*(uint16_t*)buf = regs->GPASET.GPA_LSB;
			break;
		case 0x43:
			*(uint16_t*)buf = regs->GPASET.GPA_MSB;
			break;
		case 0x44:
			*(uint16_t*)buf = regs->GPACLEAR.GPA_LSB;
			break;
		case 0x45:
			*(uint16_t*)buf = regs->GPACLEAR.GPA_MSB;
			break;
		case 0x46:
			*(uint16_t*)buf = regs->GPATOGGLE.GPA_LSB;
			break;
		case 0x47:
			*(uint16_t*)buf = regs->GPATOGGLE.GPA_MSB;
			break;
		case 0x48:
			*(uint16_t*)buf = regs->GPBDAT.GPB_LSB;
			break;
		case 0x49:
			*(uint16_t*)buf = regs->GPBDAT.GPB_MSB;
			break;
		case 0x4a:
			*(uint16_t*)buf = regs->GPBSET.GPB_LSB;
			break;
		case 0x4b:
			*(uint16_t*)buf = regs->GPBSET.GPB_MSB;
			break;
		case 0x4c:
			*(uint16_t*)buf = regs->GPBCLEAR.GPB_LSB;
			break;
		case 0x4d:
			*(uint16_t*)buf = regs->GPBCLEAR.GPB_MSB;
			break;
		case 0x4e:
			*(uint16_t*)buf = regs->GPBTOGGLE.GPB_LSB;
			break;
		case 0x4f:
			*(uint16_t*)buf = regs->GPBTOGGLE.GPB_MSB;
			break;
		case 0x50:
			*(uint16_t*)buf = regs->GPCDAT.GPC_LSB;
			break;
		case 0x51:
			*(uint16_t*)buf = regs->GPCDAT.GPC_MSB;
			break;
		case 0x52:
			*(uint16_t*)buf = regs->GPCSET.GPC_LSB;
			break;
		case 0x53:
			*(uint16_t*)buf = regs->GPCSET.GPC_MSB;
			break;
		case 0x54:
			*(uint16_t*)buf = regs->GPCCLEAR.GPC_LSB;
			break;
		case 0x55:
			*(uint16_t*)buf = regs->GPCCLEAR.GPC_MSB;
			break;
		case 0x56:
			*(uint16_t*)buf = regs->GPCTOGGLE.GPC_LSB;
			break;
		case 0x57:
			*(uint16_t*)buf = regs->GPCTOGGLE.GPC_MSB;
			break;
		case 0x60:
			*(uint16_t*)buf = regs->GPIOXINT1SEL.all;
			break;
		case 0x61:
			*(uint16_t*)buf = regs->GPIOXINT2SEL.all;
			break;
		case 0x62:
			*(uint16_t*)buf = regs->GPIOXNMISEL.all;
			break;
		case 0x63:
			*(uint16_t*)buf = regs->GPIOXINT3SEL.all;
			break;
		case 0x64:
			*(uint16_t*)buf = regs->GPIOXINT4SEL.all;
			break;
		case 0x65:
			*(uint16_t*)buf = regs->GPIOXINT5SEL.all;
			break;
		case 0x66:
			*(uint16_t*)buf = regs->GPIOXINT6SEL.all;
			break;
		case 0x67:
			*(uint16_t*)buf = regs->GPIOXINT7SEL.all;
			break;
		case 0x68:
			*(uint16_t*)buf = regs->GPIOLPMSEL.all;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__,"Can not read the register at 0x%x in gpio\n", offset);
			RW_UNLOCK(dev->lock);
			return Invarg_exp;
	}
	RW_UNLOCK(dev->lock);
	return No_exp;
}

static exception_t f28335_gpio_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct f28335_gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	RW_WRLOCK(dev->lock);
	uint16_t val16 = *(uint16_t*)buf;
	//printf("In %s, offset:%x, , LSB:%x, MSB:%x, val16:%x\n", __func__, offset, regs->GPADAT.GPA_LSB, regs->GPADAT.GPA_MSB, val16);
	switch(offset) {
		case 0x0:
			regs->GPACTRL.GPA_LSB = val16; 
			break;
		case 0x1:
			regs->GPACTRL.GPA_MSB = val16; 
			break;
		case 0x2:
			regs->GPAQSEL1.GPA1_LSB = val16;
			break;
		case 0x3:
			regs->GPAQSEL1.GPA1_MSB = val16;
			break;
		case 0x4:
			regs->GPAQSEL2.GPA2_LSB = val16;
			break;
		case 0x5:
			regs->GPAQSEL2.GPA2_MSB = val16;
			break;
		case 0x6:
			regs->GPAMUX1.GPA1_LSB = val16;
			break;
		case 0x7:
			regs->GPAMUX1.GPA1_MSB = val16;
			break;
		case 0x8:
			regs->GPAMUX2.GPA2_LSB = val16;
			break;
		case 0x9:
			regs->GPAMUX2.GPA2_MSB = val16;
			break;
		case 0xa:
			regs->GPADIR.GPA_LSB = val16;
			get_io_output_pin(opaque, 0, val16);
			break;
		case 0xb:
			regs->GPADIR.GPA_MSB = val16;
			get_io_output_pin(opaque, 1, val16);
			break;
		case 0xc:
			regs->GPAPUD.GPA_LSB = val16;
			break;
		case 0xd:
			regs->GPAPUD.GPA_MSB = val16;
			break;
		case 0x10:
			regs->GPBCTRL.GPB_LSB = val16;
			break;
		case 0x11:
			regs->GPBCTRL.GPB_MSB = val16;
			break;
		case 0x12:
			regs->GPBQSEL1.GPB1_LSB = val16;
			break;
		case 0x13:
			regs->GPBQSEL1.GPB1_MSB = val16;
			break;
		case 0x14:
			regs->GPBQSEL2.GPB2_LSB = val16;
			break;
		case 0x15:
			regs->GPBQSEL2.GPB2_MSB = val16;
			break;
		case 0x16:
			regs->GPBMUX1.GPB1_LSB = val16;
			break;
		case 0x17:
			regs->GPBMUX1.GPB1_MSB = val16;
			break;
		case 0x18:
			regs->GPBMUX2.GPB2_LSB = val16;
			break;
		case 0x19:
			regs->GPBMUX2.GPB2_MSB = val16;
			break;
		case 0x1a:
			regs->GPBDIR.GPB_LSB = val16;
			get_io_output_pin(opaque, 2, val16);
			break;
		case 0x1b:
			regs->GPBDIR.GPB_MSB = val16;
			get_io_output_pin(opaque, 3, val16);
			break;
		case 0x1c:
			regs->GPBPUD.GPB_LSB = val16;
			break;
		case 0x1d:
			regs->GPBPUD.GPB_MSB = val16;
			break;
		case 0x26:
			regs->GPCMUX1.GPC1_LSB = val16;
			break;
		case 0x27:
			regs->GPCMUX1.GPC1_MSB = val16;
			break;
		case 0x28:
			regs->GPCMUX2.GPC2_LSB = val16;
			break;
		case 0x29:
			regs->GPCMUX2.GPC2_MSB = val16;
			break;
		case 0x2a:
			regs->GPCDIR.GPC_LSB = val16;
			get_io_output_pin(opaque, 4, val16);
			break;
		case 0x2b:
			regs->GPCDIR.GPC_MSB = val16;
			get_io_output_pin(opaque, 5, val16);
			break;
		case 0x2c:
			regs->GPCPUD.GPC_LSB = val16;
			break;
		case 0x2d:
			regs->GPCPUD.GPC_MSB = val16;
			break;
		case 0x40:
			set_output_io_value(opaque, 0, val16);
			update_gpio_on_line(opaque, regs->GPADAT.GPA_LSB);
			break;
		case 0x41:
			set_output_io_value(opaque, 1, val16);
			update_gpio_on_line(opaque, regs->GPADAT.GPA_MSB);
			break;
		case 0x42:
		case 0x43:
			if (offset == 0x42){
				regs->GPADAT.GPA_LSB |= val16;  //GpioA set 
				get_io_high_pin(opaque, 0, val16);
			}else {
				regs->GPADAT.GPA_MSB |= val16;  
				get_io_high_pin(opaque, 1, val16);
			}
			if(dev->gpio_linker_intf != NULL){
				int i, pin_num, mode, pin_state;
				for (i = 0; i < dev->connect_quanaity; i++){
					pin_num = dev->connect_pin[i];
					if (pin_num < 32){
						mode = dev->gpio_group[pin_num].mode;
						GPIOA(&pin_state, GPADAT, pin_num);
						if (mode == OUTPUT_MODE){
							dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, pin_num, pin_state);	
						}
					}
				}
			}
			break;
		case 0x44:
		case 0x45:
			if (offset == 0x44){
				regs->GPADAT.GPA_LSB &= (~val16); //GpioA clear
				get_io_low_pin(opaque, 0, val16);
			
			}else {
				regs->GPADAT.GPA_MSB &= (~val16);
				get_io_low_pin(opaque, 1, val16);
			}
			if(dev->gpio_linker_intf != NULL){
				int i, pin_num, mode, pin_state;
				for (i = 0; i < dev->connect_quanaity; i++){
					pin_num = dev->connect_pin[i];
					if (pin_num < 32){
						mode = dev->gpio_group[pin_num].mode;
						GPIOA(&pin_state, GPADAT, pin_num);
						if (mode == OUTPUT_MODE){
							dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, pin_num, pin_state);	
						}
					}
				}
			}
			break;
		case 0x46:
		case 0x47:
			if (offset == 0x46){
				regs->GPADAT.GPA_LSB ^= val16; //GpioA toggle
			}else {
				regs->GPADAT.GPA_MSB ^= val16;//GpioA toggle
			}
			if(dev->gpio_linker_intf != NULL){
				int i, pin_num, mode, pin_state;
				for (i = 0; i < dev->connect_quanaity; i++){
					pin_num = dev->connect_pin[i];
					if (pin_num < 32){
						mode = dev->gpio_group[pin_num].mode;
						GPIOA(&pin_state, GPADAT, pin_num);
						if (mode == OUTPUT_MODE){
							dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, pin_num, pin_state);	
						}
					}
				}
			}
			break;
		case 0x48:
			set_output_io_value(opaque, 2, val16);
			update_gpio_on_line(opaque, regs->GPBDAT.GPB_LSB);
			break;
		case 0x49:
			set_output_io_value(opaque, 3, val16);
			update_gpio_on_line(opaque, regs->GPBDAT.GPB_MSB);
			break;
		case 0x4a:
		case 0x4b:
			if (offset == 0x4a){
				regs->GPBDAT.GPB_LSB |= val16; //GpioB set 
				//get_io_high_pin(opaque, 2, val16);
			}else {
				regs->GPBDAT.GPB_MSB |= val16;
				//get_io_high_pin(opaque, 3, val16);
			
			}
			if(dev->gpio_linker_intf != NULL){
				int i, pin_num, mode, pin_state;
				for (i = 0; i < dev->connect_quanaity; i++){
					pin_num = dev->connect_pin[i];
					if (pin_num >= 32 && pin_num < 64){
						mode = dev->gpio_group[pin_num].mode;
						GPIOB(&pin_state, GPBDAT, pin_num);
						if (mode == OUTPUT_MODE){
							dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, pin_num, pin_state);	
						}
					}
				}
			}
			break;
		case 0x4c:
		case 0x4d:
			if (offset == 0x4c){
				regs->GPBDAT.GPB_LSB &= (~val16); //GpioB clear
				//get_io_low_pin(opaque, 2, val16);
			}else {
				regs->GPBDAT.GPB_MSB &= ~(val16); 
				//get_io_low_pin(opaque, 3, val16);
			}
			if(dev->gpio_linker_intf != NULL){
				int i, pin_num, mode, pin_state;
				for (i = 0; i < dev->connect_quanaity; i++){
					pin_num = dev->connect_pin[i];
					if (pin_num >= 32 && pin_num < 64){
						mode = dev->gpio_group[pin_num].mode;
						GPIOB(&pin_state, GPBDAT, pin_num);
						if (mode == OUTPUT_MODE){
							dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, pin_num, pin_state);	
						}
					}
				}
			}
			break;
		case 0x4e:
		case 0x4f:
			if (offset == 0x4e){
				regs->GPBDAT.GPB_LSB ^= val16; //GpioB toggle
			}else {
				regs->GPBDAT.GPB_MSB ^= val16; //GpioB toggle 
			}
			if(dev->gpio_linker_intf != NULL){
				int i, pin_num, mode, pin_state;
				for (i = 0; i < dev->connect_quanaity; i++){
					pin_num = dev->connect_pin[i];
					if (pin_num >= 32 && pin_num < 64){
						mode = dev->gpio_group[pin_num].mode;
						GPIOB(&pin_state, GPBDAT, pin_num);
						if (mode == OUTPUT_MODE){
							dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, pin_num, pin_state);	
						}
					}
				}
			}
			break;
		case 0x50:
			set_output_io_value(opaque, 4, val16);
			update_gpio_on_line(opaque, regs->GPCDAT.GPC_LSB);
			break;
		case 0x51:
			set_output_io_value(opaque, 5, val16);
			update_gpio_on_line(opaque, regs->GPCDAT.GPC_MSB);
			break;
		case 0x52:
		case 0x53:
			if (offset == 0x52){
				regs->GPCDAT.GPC_LSB |= val16; //GpioC set 
				//get_io_high_pin(opaque, 4, val16);
			}else {
				regs->GPCDAT.GPC_MSB |= val16;
				//get_io_high_pin(opaque, 5, val16);
			}
			if(dev->gpio_linker_intf != NULL){
				int i, pin_num, mode, pin_state;
				for (i = 0; i < dev->connect_quanaity; i++){
					pin_num = dev->connect_pin[i];
					if (pin_num >= 64 && pin_num < MAX_GPIO_NUM){
						mode = dev->gpio_group[pin_num].mode;
						GPIOC(&pin_state, GPCDAT, pin_num);
						if (mode == OUTPUT_MODE){
							dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, pin_num, pin_state);	
						}
					}
				}
			}
			break;
		case 0x54:
		case 0x55:
			if (offset == 0x54){
				regs->GPCDAT.GPC_LSB &= (~val16); //GpioC clear
				//get_io_low_pin(opaque, 4, val16);
			}else {
				regs->GPCDAT.GPC_MSB &= (~val16);
				//get_io_low_pin(opaque, 5, val16);
			}
			if(dev->gpio_linker_intf != NULL){
				int i, pin_num, mode, pin_state;
				for (i = 0; i < dev->connect_quanaity; i++){
					pin_num = dev->connect_pin[i];
					if (pin_num >= 64 && pin_num < MAX_GPIO_NUM){
						mode = dev->gpio_group[pin_num].mode;
						GPIOC(&pin_state, GPCDAT, pin_num);
						if (mode == OUTPUT_MODE){
							dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, pin_num, pin_state);	
						}
					}
				}
			}
			break;
		case 0x56:
		case 0x57:
			if (offset == 0x56){
				regs->GPCDAT.GPC_LSB ^= val16; //GpioC toggle
			}else {
				regs->GPCDAT.GPC_MSB ^= val16;
			}
			if(dev->gpio_linker_intf != NULL){
				int i, pin_num, mode, pin_state;
				for (i = 0; i < dev->connect_quanaity; i++){
					pin_num = dev->connect_pin[i];
					if (pin_num >= 64 && pin_num < MAX_GPIO_NUM){
						mode = dev->gpio_group[pin_num].mode;
						GPIOC(&pin_state, GPCDAT, pin_num);
						if (mode == OUTPUT_MODE){
							dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, pin_num, pin_state);	
						}
					}
				}
			}
			break;
		case 0x60:
			regs->GPIOXINT1SEL.all = val16;
			//XINT1 connect gpio[val16]
			dev->channel[dev->cnt] = 1;
			dev->cnt ++;
			if (dev->cnt == 8)
				dev->cnt = 0;
			break;
		case 0x61:
			regs->GPIOXINT2SEL.all = val16;
			//XINT2 connect gpio[val16]
			dev->channel[dev->cnt] = 2;
			dev->cnt ++;
			if (dev->cnt == 8)
				dev->cnt = 0;
			break;
		case 0x62:
			regs->GPIOXNMISEL.all = val16;
			break;
		case 0x63:
			regs->GPIOXINT3SEL.all = val16;
			//XINT3 connect gpio[val16]
			dev->channel[dev->cnt] = 3;
			dev->cnt ++;
			if (dev->cnt == 8)
				dev->cnt = 0;
			break;
		case 0x64:
			regs->GPIOXINT4SEL.all = val16;
			//XINT4 connect gpio[val16]
			dev->channel[dev->cnt] = 4;
			dev->cnt ++;
			if (dev->cnt == 8)
				dev->cnt = 0;
			break;
		case 0x65:
			regs->GPIOXINT5SEL.all = val16;
			//XINT5 connect gpio[val16]
			dev->channel[dev->cnt] = 5;
			dev->cnt ++;
			if (dev->cnt == 8)
				dev->cnt = 0;
			break;
		case 0x66:
			regs->GPIOXINT6SEL.all = val16;
			//XINT6 connect gpio[val16]
			dev->channel[dev->cnt] = 6;
			dev->cnt ++;
			if (dev->cnt == 8)
				dev->cnt = 0;
			break;
		case 0x67:
			regs->GPIOXINT7SEL.all = val16;
			//XINT7 connect gpio[val16]
			dev->channel[dev->cnt] = 7;
			dev->cnt ++;
			if (dev->cnt == 8)
				dev->cnt = 0;
			break;
		case 0x68:
			regs->GPIOLPMSEL.all = val16;
			get_wake_up_pin(opaque, val16);
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__, "Can not write the register at 0x%x in gpio\n", offset);
			RW_UNLOCK(dev->lock);
			return Invarg_exp;
	}
	RW_UNLOCK(dev->lock);
	return No_exp;
}

char* f28335_gpio_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	f28335_gpio_device *dev = conf_obj->obj;
	return regs_name[id];
}

uint32_t f28335_gpio_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	f28335_gpio_device *dev = conf_obj->obj;
	uint16_t* regs_value = (uint16_t*)(dev->regs) + id;
	return *regs_value;
}

static char* f28335_gpio_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	f28335_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = (dev->regs);
	*((uint16_t*)regs + id) = value;
    	int offset = regs_offset[id];
	//printf("%s %d, offset :%x, val16:%x\n", __func__, __LINE__, offset, value);
	switch(offset){
	case GPA_SET_LOW:
			regs->GPADAT.GPA_LSB |= value;
			regs->GPASET.GPA_LSB = 0;
			get_io_high_pin(conf_obj, 0, value);
			break;
        case GPA_SET_HIGH:
			regs->GPADAT.GPA_MSB |= value;
			regs->GPASET.GPA_MSB = 0;
			get_io_high_pin(conf_obj, 1, value);
			break;
	case GPA_CLEAR_LOW:
			regs->GPADAT.GPA_LSB &= ~value;
			regs->GPACLEAR.GPA_LSB = 0;
	    		get_io_low_pin(conf_obj, 0, value);
			break;
        case GPA_CLEAR_HIGH:
			regs->GPADAT.GPA_MSB &= ~value;
			regs->GPACLEAR.GPA_MSB = 0;
	    		get_io_low_pin(conf_obj, 1, value);
			break;
	case GPA_TOGGLE_LOW:
			regs->GPADAT.GPA_LSB ^= value;
			regs->GPATOGGLE.GPA_LSB = 0;
			break;
        case GPA_TOGGLE_HIGH:
			regs->GPADAT.GPA_MSB ^= value;
			regs->GPATOGGLE.GPA_MSB = 0;
			break;
	case GPB_SET_LOW:
			regs->GPBDAT.GPB_LSB |= value;
			regs->GPBSET.GPB_LSB = 0;
			break;
        case GPB_SET_HIGH:
			regs->GPBDAT.GPB_MSB |= value;
			regs->GPBSET.GPB_MSB = 0;
			break;
	case GPB_CLEAR_LOW:
			regs->GPBDAT.GPB_LSB &= ~value;
			regs->GPBCLEAR.GPB_LSB = 0;
			break;
        case GPB_CLEAR_HIGH:
			regs->GPBDAT.GPB_MSB &= ~value;
			regs->GPBCLEAR.GPB_MSB = 0;
			break;
	case GPB_TOGGLE_LOW:
			regs->GPBDAT.GPB_LSB ^= value;
			regs->GPBTOGGLE.GPB_LSB = 0;
			break;
        case GPB_TOGGLE_HIGH:
			regs->GPBDAT.GPB_MSB ^= value;
			regs->GPBTOGGLE.GPB_MSB = 0;
			break;
        case GPC_SET_LOW:
			regs->GPCDAT.GPC_LSB |= value;
			regs->GPCSET.GPC_LSB = 0;
			break;
        case GPC_SET_HIGH:
			regs->GPCDAT.GPC_MSB |= value;
			regs->GPCSET.GPC_MSB = 0;
			break;
	case GPC_CLEAR_LOW:
			regs->GPCDAT.GPC_LSB &= ~value;
			regs->GPCCLEAR.GPC_LSB = 0;
			break;
        case GPC_CLEAR_HIGH:
			regs->GPCDAT.GPC_MSB &= ~value;
			regs->GPCCLEAR.GPC_MSB = 0;
			break;
	case GPC_TOGGLE_LOW:
			regs->GPCDAT.GPC_LSB ^= value;
			regs->GPCTOGGLE.GPC_LSB = 0;
			break;
        case GPC_TOGGLE_HIGH:
			regs->GPCDAT.GPC_MSB ^= value;
			regs->GPCTOGGLE.GPC_MSB = 0;
			break;
		default:
			break;
	}

	return NULL;
}

static uint32_t f28335_gpio_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	f28335_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = (dev->regs);
	return  REG_NUM;
}

static uint32_t f28335_gpio_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	f28335_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = (dev->regs);
	uint32_t regnum;
	regnum = REG_NUM;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return REG_NUM;
}

static uint32_t gpio_28335_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static uint32_t f28335_gpio_get_pin_num(conf_object_t *conf_obj)
{
	f28335_gpio_device* dev = (f28335_gpio_device*)conf_obj->obj;
	return dev->pin_num;
}

exception_t f28335_gpio_get_pin_type(conf_object_t *conf_obj, uint32_t index, void *type)
{
	f28335_gpio_device* dev = (f28335_gpio_device*)conf_obj->obj;
	gpio_reg_t* regs = dev->regs;

	if(index < 0 || index > dev->pin_num)
	{
		skyeye_log(Error_log, __FUNCTION__, "f28335 gpio pin index %d is out of range, please confirm.\n", index);
		type = NULL;
		return OUTOFRANGE;
	}

	if (index < 32) // GPIO0 - GPIO31 in GPADIR
	{
		GPIOA((int*)type, GPADIR, index);
	}
	else if (index < 63) // GPIO32 - GPIO63 in GPBDIR
	{
		GPIOB((int*)type, GPBDIR, index);
	}
	else // GPIO64 - GPIO87 in GPCDIR
	{
		GPIOC((int*)type, GPCDIR, index);
	}

	return No_exp;
}

exception_t f28335_gpio_read_pin_state(conf_object_t *conf_obj, uint32_t index, void *state)
{
	f28335_gpio_device* dev = (f28335_gpio_device*)conf_obj->obj;
	gpio_reg_t* regs = dev->regs;

	if(index < 0 || index > dev->pin_num)
	{
		skyeye_log(Error_log, __FUNCTION__, "f28335 gpio pin index %d is out of range, please confirm.\n", index);
		state = NULL;
		return OUTOFRANGE;
	}
	
	RW_RDLOCK(dev->lock);
	// ignore pin type
	if (index < 32) // GPIO0 - GPIO31 in GPADAT
	{
		GPIOA((float*)state, GPADAT, index);
	}
	else if (index < 63) // GPIO32 - GPIO63 in GPBDAT
	{
		GPIOB((float*)state, GPBDAT, index);
	}
	else // GPIO64 - GPIO87 in GPCDAT
	{
		GPIOC((float*)state, GPCDAT, index);
	}
	RW_UNLOCK(dev->lock);

	return No_exp;
}

exception_t f28335_gpio_write_pin_state(conf_object_t *conf_obj, uint32_t index, void *state)
{
	int pin_type = 0;
	int value = *(int *)state;

	f28335_gpio_device* dev = (f28335_gpio_device*)conf_obj->obj;
	gpio_reg_t* regs = dev->regs;

	if(index < 0 || index > dev->pin_num)
	{
		skyeye_log(Error_log, __FUNCTION__, "f28335 gpio pin index %d is out of range, please confirm.\n", index);
		return OUTOFRANGE;
	}

	RW_WRLOCK(dev->lock);
	if (value > 0){
		value = 1;
	}else {
		value = 0;
	}

	if (index < 32) // GPIO0 - GPIO31 in GPADAT
	{
		// maybe error here
		GPIOA_WR(&value, GPADAT, index);
	}
	else if (index < 63) // GPIO32 - GPIO63 in GPBDAT
	{
		GPIOB_WR(&value, GPBDAT, index);
	}
	else // GPIO64 - GPIO87 in GPCDAT
	{
		GPIOC_WR(&value, GPCDAT, index);
	}
	RW_UNLOCK(dev->lock);

	return No_exp;
}

static conf_object_t* new_f28335_gpio(char* obj_name){
	f28335_gpio_device* dev = skyeye_mm_zero(sizeof(f28335_gpio_device));
	gpio_reg_t* regs =  skyeye_mm_zero(sizeof(gpio_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init gpio regs */

	dev->regs = regs;
	dev->regs_name = &regs_name;
	dev->pin_num = MAX_GPIO_NUM;
	dev->circuit_display = NULL;
	RWLOCK_INIT(dev->lock);

	return dev->obj;
}

static exception_t reset_f28335_gpio (conf_object_t *conf_obj, const char* args)
{
	f28335_gpio_device* dev = conf_obj->obj;
	memset(dev->regs, 0, sizeof(dev->regs));

	return No_exp;
}

void free_f28335_gpio(conf_object_t* conf_obj){
	f28335_gpio_device* dev = conf_obj->obj;	

	skyeye_free(dev->obj);
	skyeye_free(dev->regs);
	if(dev->circuit_display)
		skyeye_free(dev->circuit_display);
	//printf("%s, %d\n", __func__, __LINE__);
	RWLOCK_DESTROY(dev->lock);
	skyeye_free(dev);
	//printf("%s, %d\n", __func__, __LINE__);
	return;
}

static exception_t connector_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	f28335_gpio_device *dev = obj->obj;
	dev->gpio_connector = obj2;
	dev->connector_iface = (skyeye_gpio_connector_intf *)SKY_get_iface(dev->gpio_connector, SKYEYE_GPIO_CONNECTOR);

	if (dev->connector_iface == NULL){
		if (dev->gpio_connector == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_GPIO_CONNECTOR);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_GPIO_CONNECTOR, obj2->objname);

		return Not_found_exp;
	}
//	update_gpio_on_line(obj, dev->regs->gpaqsel1);
	return No_exp;
}
 
static exception_t connector_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	f28335_gpio_device *dev = obj->obj;
	*obj2 = dev->gpio_connector;
	*port = NULL;
	return No_exp;
}

static exception_t gate_b_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	f28335_gpio_device *dev = obj->obj;
	dev->gpio_gate_b = obj2;
	dev->gate_b_iface = (skyeye_gate_b_intf*)SKY_get_iface(dev->gpio_gate_b, LOGIC_GATE_B);
	if (dev->gate_b_iface == NULL){
		if (dev->gpio_gate_b == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", LOGIC_GATE_B);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", LOGIC_GATE_B, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
 
static exception_t gate_b_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	f28335_gpio_device *dev = obj->obj;
	*obj2 = dev->gpio_gate_b;
	*port = NULL;
	return No_exp;
}


static exception_t gate_a_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	f28335_gpio_device *dev = obj->obj;
	dev->gpio_gate_a = obj2;
	dev->gate_a_iface = (skyeye_gate_a_intf*)SKY_get_iface(dev->gpio_gate_a, LOGIC_GATE_A);
	if (dev->gate_a_iface == NULL){
		if (dev->gpio_gate_a == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", LOGIC_GATE_A);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", LOGIC_GATE_A, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
 
static exception_t gate_a_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	f28335_gpio_device *dev = obj->obj;
	*obj2 = dev->gpio_gate_a;
	*port = NULL;
	return No_exp;
}

static exception_t connect_device_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	f28335_gpio_device *dev = obj->obj;
	gpio_connect_dev_list_t* list = &(dev->dev_list);
	list->device[list->dev_num].dev = obj2;
	list->device[list->dev_num].connector_device_iface =  (gpio_connect_device_intf *)SKY_get_iface(obj2, GPIO_CONNECT_DEVICE_INTF);
	if (list->device[list->dev_num].connector_device_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GPIO_CONNECT_DEVICE_INTF, obj2->objname);
	}
	list->dev_num ++;
	return No_exp;
}
 
static exception_t connect_device_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	f28335_gpio_device *dev = obj->obj;
	gpio_connect_dev_list_t* list = &(dev->dev_list);

	list->dev_num --;
	*obj2 = list->device[list->dev_num].dev;
	*port = NULL;
	return No_exp;
}

static exception_t set_attr_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	f28335_gpio_device *dev = obj->obj;
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
	f28335_gpio_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_connect_pin(void *arg, conf_object_t *conf_obj, attr_value_t *idx)
{
	f28335_gpio_device* dev = conf_obj->obj;
	attr_value_t connect_list = SKY_make_attr_string(dev->connect_list);

	return connect_list;
}

static exception_t set_attr_connect_pin(void *arg, conf_object_t *conf_obj, attr_value_t *value, attr_value_t *idx)
{
	f28335_gpio_device* dev = conf_obj->obj;
	dev->connect_list =SKY_attr_string(*value);

    	int i = 0;
    	char tranf[128] = "";
    	char *delim_pin = ".";
    	char *swap;

    	strcpy(tranf, dev->connect_list);
    	dev->connect_pin[i] = atoi(strtok(tranf, delim_pin));
    	while(swap = strtok(NULL, delim_pin)){
        	i++;
        	dev->connect_pin[i] = atoi(swap);
    	}
    	dev->connect_quanaity = i + 1;

	return No_exp;
}

static attr_value_t get_attr_circuit_display (void *arg, conf_object_t *conf_obj, attr_value_t *idx)
{
	f28335_gpio_device *dev = conf_obj->obj;
	attr_value_t circuit_display = SKY_make_attr_string(dev->circuit_display);
	return circuit_display;
}

static exception_t set_attr_circuit_display (void *arg, conf_object_t *conf_obj, attr_value_t *value, attr_value_t *idx)
{
	f28335_gpio_device *dev = conf_obj->obj;
	char *circuit_string = SKY_attr_string(*value);
	dev->circuit_display = skyeye_mm_zero(strlen(circuit_string) + 1);
	strcpy(dev->circuit_display, circuit_string);
	return No_exp;
}

static exception_t gpio_link_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	f28335_gpio_device *dev = obj->obj;
	dev->gpio_link = obj2;
	dev->gpio_linker_intf = (gpio_linker_pin_intf **)SKY_get_iface(dev->gpio_link, GPIO_LINKER_PIN_INTF);

	if (dev->gpio_linker_intf == NULL){
		if (dev->gpio_link == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GPIO_LINKER_PIN_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GPIO_LINKER_PIN_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t gpio_link_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	f28335_gpio_device *dev = obj->obj;
	*obj2 = dev->gpio_link;
	*port = NULL;
	return No_exp;
}

static void f28335_gpio_register_attribute (conf_class_t *clss)
{
	SKY_register_attribute(clss,"connect_pin",
            get_attr_connect_pin,NULL,
            set_attr_connect_pin,NULL,
            SKY_Attr_Optional,"string",
            "Connected pin, string format: PIN1.PIN2 ... PINn");
	SKY_register_attribute(clss, "circuit_display", get_attr_circuit_display, NULL, set_attr_circuit_display, NULL, SKY_Attr_Optional, "string", "if or not display in MultiSim");
	return;
}

void init_f28335_gpio(){
	static skyeye_class_t class_data = {
		.class_name = "f28335_gpio",
		.class_desc = "f28335 gpio",
		.new_instance = new_f28335_gpio,
		.reset_instance = reset_f28335_gpio,
		.free_instance = free_f28335_gpio,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_f28335 SC_c2812),
	};
		
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const skyeye_gpio_connector_intf iface = {
		.update =  update_gpio
	};
	SKY_register_iface(clss, SKYEYE_GPIO_CONNECTOR, &iface);

	static const memory_space_intf io_memory = {
		.read = f28335_gpio_read,
		.write = f28335_gpio_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);	
	
	static const gpio_connect_device_intf gpio_intf = {
		.connect_gpio = connect_gpio,
	};

	SKY_register_iface(clss, GPIO_CONNECT_DEVICE_INTF, &gpio_intf);
	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = f28335_gpio_get_regvalue_by_id,
		.get_regname_by_id = f28335_gpio_get_regname_by_id,
		.set_regvalue_by_id = f28335_gpio_set_regvalue_by_id,
		.get_regnum = f28335_gpio_get_regnum,
		.get_regid_by_name = f28335_gpio_get_regid_by_name,
		.get_regoffset_by_id = gpio_28335_get_regoffset_by_id,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const skyeye_pin_info_intf pin_intf = {
		.get_pin_num = f28335_gpio_get_pin_num,
		.get_pin_type = f28335_gpio_get_pin_type,
		.read_pin_state = f28335_gpio_read_pin_state,
		.write_pin_state = f28335_gpio_write_pin_state,
	};
	SKY_register_iface(clss, SKYEYE_PIN_INFO_INTF, &pin_intf);

	static const gpio_pin_set gpio_pin_intf = {
        	.set_gpio = gpio_pin_set_data,
    	};
    	SKY_register_iface(clss, GPIO_PIN_SET, &gpio_pin_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SKYEYE_GPIO_CONNECTOR,
			.iface = &iface,
		},
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF, 
			.iface = &reg_intf,
		},
		(struct InterfaceDescription) {
			.name = GPIO_CONNECT_DEVICE_INTF, 
			.iface = &gpio_intf,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_PIN_INFO_INTF,
			.iface = &pin_intf,
		},
		(struct InterfaceDescription) {
            		.name = GPIO_PIN_SET,
            		.iface = &gpio_pin_intf,
        	},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = LOGIC_GATE_A,
			.set = gate_a_set,
			.get = gate_a_get,
		},
		(struct ConnectDescription) {
			.name = LOGIC_GATE_B,
			.set = gate_b_set,
			.get = gate_b_get,
		},
		(struct ConnectDescription) {
			.name = GPIO_CONNECT_DEVICE_INTF,
			.set = connect_device_set,
			.get = connect_device_get,
		},
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = set_attr_signal,
			.get = get_attr_signal,
		},
        	(struct ConnectDescription) {
			.name = SKYEYE_GPIO_CONNECTOR,
			.set = connector_set,
			.get = connector_get,
		},
		(struct ConnectDescription) {
            		.name = GPIO_LINKER_PIN_INTF,
            		.set = gpio_link_set,
            		.get = gpio_link_get,
        	},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	f28335_gpio_register_attribute(clss);
}

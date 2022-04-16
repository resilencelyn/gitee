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
* @file gpio_omapl138.c
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
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_config.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_dev_intf.h>

#include "gpio_omapl138.h"

#define GPIO_B0INT 42

/*******Determine if the connected pin level has changed********/
#define JUDGE_PIN_STATUS(val,pin_num,dir) (((val >> pin_num) & 1) && ((~dir) >> pin_num) & 1)


static void gpio_raise_signal(conf_object_t *opaque, int bank){
	struct omapl138_gpio_device *dev = opaque->obj;
	int int_number = GPIO_B0INT + bank;

	if (dev->signal_iface != NULL)
		dev->signal_iface->raise_signal(dev->signal, int_number);
}

static void	rising_edge_signal_mode(conf_object_t *opaque, int bank, int val){
	struct omapl138_gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	int pin = 0;

	while (((1 << pin++) & val) == 0);

	if ((regs->gpio_set_ris_trig[bank] & (1 << pin)) 
			&& INT_ENABLE(pin)){
		bank = bank << 1 + pin / 16;
		gpio_raise_signal(opaque, bank);
	}
}

static void	falling_edge_signal_mode(conf_object_t *opaque, int bank, int val){
	struct omapl138_gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	int pin = 0;

	while (((1 << pin++) & val) == 0);

	if ((regs->gpio_set_fal_trig[bank] & (1 << pin))
			&& INT_ENABLE(pin)){
		bank = bank << 1 + pin / 16;
		gpio_raise_signal(opaque, bank);
	}
	
}
static exception_t omapl138_gpio_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{

	struct omapl138_gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	int n = 0;
	switch(offset) {
		case 0x00:
			*(uint32_t *)buf = regs->gpio_revid;
			break;
		case 0x08:
			*(uint32_t *)buf = regs->gpio_binten;
			break;
		case 0x10:
		case 0x38:
		case 0x60:
		case 0x88:
		case 0xb0:
			n = (offset - 0x10) / 0x28;
			*(uint32_t *)buf = regs->gpio_dir[n];
			break;
		case 0x14:
		case 0x3c:
		case 0x64:
		case 0x8c:
		case 0xb4:
			n = (offset - 0x14) / 0x28;
			*(uint32_t *)buf = regs->gpio_out_data[n];
			break;
		case 0x18:
		case 0x40:
		case 0x68:
		case 0x90:
		case 0xb8:
			n = (offset - 0x18) / 0x28;
			*(uint32_t *)buf = regs->gpio_set_data[n];
			break;
		case 0x1c:
		case 0x44:
		case 0x6c:
		case 0x94:
		case 0xbc:
			n = (offset - 0x1c) / 0x28;
			*(uint32_t *)buf = regs->gpio_clr_data[n];
			break;
		case 0x20:
		case 0x48:
		case 0x70:
		case 0x98:
		case 0xc0:
			n = (offset - 0x20) / 0x28;
			*(uint32_t *)buf = regs->gpio_in_data[n];
			if (offset == 0x20) {
				*(uint32_t *)buf |= dev->gpio0_px_data & 0xffff;
			}
			break;
		case 0x24:
		case 0x4c:
		case 0x74:
		case 0x9c:
		case 0xc4:
			n = (offset - 0x24) / 0x28;
			*(uint32_t *)buf = regs->gpio_set_ris_trig[n];
			break;
		case 0x28:
		case 0x50:
		case 0x78:
		case 0xa0:
		case 0xc8:
			n = (offset - 0x28) / 0x28;
			*(uint32_t *)buf = regs->gpio_clr_ris_trig[n];
			break;
		case 0x2c:
		case 0x54:
		case 0x7c:
		case 0xa4:
		case 0xcc:
			n = (offset - 0x2c) / 0x28;
			*(uint32_t *)buf = regs->gpio_set_fal_trig[n];
			break;
		case 0x30:
		case 0x58:
		case 0x80:
		case 0xa8:
		case 0xd0:
			n = (offset - 0x30) / 0x28;
			*(uint32_t *)buf = regs->gpio_clr_fal_trig[n];
			break;
		case 0x34:
		case 0x5c:
		case 0x84:
		case 0xac:
		case 0xd4:
			n = (offset - 0x34) / 0x28;
			*(uint32_t *)buf = regs->gpio_intstat[n];
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__,"Can not read the register at 0x%x in gpio\n", offset);
			return Invarg_exp;
	}
    //printf("%d\t %s\t offset = 0x%x\t value = 0x%x\t %p\n",__LINE__,__FUNCTION__,offset,*(uint32_t*)buf,opaque);
	return No_exp;
}

static exception_t omapl138_gpio_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct omapl138_gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	int n = 0;
   // printf("%d\t %s offset = 0x%x\t value = 0x%x\t %p\n",__LINE__,__FUNCTION__,offset,val,opaque);
	switch(offset) {
		case 0x00:
			regs->gpio_revid = val;
			break;
		case 0x08:
			regs->gpio_binten = val;
			break;
		case 0x10:
		case 0x38:
		case 0x60:
		case 0x88:
		case 0xb0:
			n = (offset - 0x10) / 0x28;
			regs->gpio_dir[n] = val;
			break;
		case 0x14:
		case 0x3c:
		case 0x64:
		case 0x8c:
		case 0xb4:
			n = (offset - 0x14) / 0x28;
			regs->gpio_in_data[n] |= (val & (~regs->gpio_dir[n]));
			break;
		case 0x18:
		case 0x40:
		case 0x68:
		case 0x90:
		case 0xb8:
            {
                n = (offset - 0x18) / 0x28;
                regs->gpio_set_data[n] = val;
                regs->gpio_out_data[n] = val & ~(regs->gpio_dir[n]);
                if (val != 0)
                    rising_edge_signal_mode(opaque, n , val);

                int i = 0;
                for(i = 0; i <= dev->connect_quanaity; i++){
                    if(JUDGE_PIN_STATUS(val, dev->connect_pin[i] % 31, regs->gpio_dir[n])){
                        if(dev->gpio_linker_intf != NULL){
                            dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, dev->connect_pin[i], PIN_HIGH); 
                        }
                    }
                }
            }
			break;
		case 0x1c:
		case 0x44:
		case 0x6c:
		case 0x94:
		case 0xbc:
            {
                n = (offset - 0x1c) / 0x28;
                regs->gpio_clr_data[n] = val;
                regs->gpio_out_data[n] &= ~(val & (~regs->gpio_dir[n]));
                if (val != 0)
                    falling_edge_signal_mode(opaque, n , val);
                int i = 0;
                for(i = 0; i <= dev->connect_quanaity; i++){
                    if(JUDGE_PIN_STATUS(val, dev->connect_pin[i] % 31, regs->gpio_dir[n])){
                        if(dev->gpio_linker_intf != NULL){
                            dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, dev->connect_pin[i], PIN_LOW); 
                        }
                    }
                }
            }
			break;
		case 0x20:
		case 0x48:
		case 0x70:
		case 0x98:
		case 0xc0:
			n = (offset - 0x20) / 0x28;
			//regs->gpio_in_data[n] = val;
			break;
		case 0x24:
		case 0x4c:
		case 0x74:
		case 0x9c:
		case 0xc4:
			n = (offset - 0x24) / 0x28;
			regs->gpio_set_ris_trig[n] = val;
			break;
		case 0x28:
		case 0x50:
		case 0x78:
		case 0xa0:
		case 0xc8:
			n = (offset - 0x28) / 0x28;
			regs->gpio_clr_ris_trig[n] = val;
			regs->gpio_set_ris_trig[n] &= ~val;
			break;
		case 0x2c:
		case 0x54:
		case 0x7c:
		case 0xa4:
		case 0xcc:
			n = (offset - 0x2c) / 0x28;
			regs->gpio_set_fal_trig[n] = val;
			break;
		case 0x30:
		case 0x58:
		case 0x80:
		case 0xa8:
		case 0xd0:
			n = (offset - 0x30) / 0x28;
			regs->gpio_clr_fal_trig[n] = val;
			regs->gpio_set_fal_trig[n] &= ~val;
			break;
		case 0x34:
		case 0x5c:
		case 0x84:
		case 0xac:
		case 0xd4:
			n = (offset - 0x34) / 0x28;
			regs->gpio_intstat[n] = val;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__, "Can not write the register at 0x%x in gpio\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static char* omapl138_gpio_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	omapl138_gpio_device *dev = conf_obj->obj;
	return dev->regs_name[id];
}

static uint32_t omapl138_gpio_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	omapl138_gpio_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static exception_t omapl138_gpio_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	omapl138_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t omapl138_gpio_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	omapl138_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = (dev->regs);
	return  sizeof(gpio_reg_t) / 4;
}

static uint32_t omapl138_gpio_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	omapl138_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = (dev->regs);
	uint32_t regnum;
	regnum = sizeof(gpio_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t omapl138_gpio_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static uint32_t omapl138_gpio_get_pin_num (conf_object_t *conf_obj)
{
	struct omapl138_gpio_device *dev = conf_obj->obj;
	return dev->pin_num;
}

static exception_t omapl138_gpio_get_pin_type (conf_object_t *conf_obj, uint32_t index, void *type)
{
	int gpx = 0;
	int suffix = 0;
	uint32_t temp = 0x1;

	struct omapl138_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = dev->regs;
	if(index < 0 || index > dev->pin_num)
	{
		skyeye_log(Error_log, __FUNCTION__, "omapl138 gpio pin index %d is out of range, please confirm.\n", index);
		type = NULL;
		return OUTOFRANGE;
	}

	// index 0 - 133  total 144 pins
	// register GP0 - GP4 total 5 registers
	// bank0 - bank8 total 8 banks
	// index 0 - 15 in GPO01 - GP015
	// index 16 - 31 in GP016 - GP031
	// and so on.
	gpx = index / 32; // confirm which bank
	suffix = index % 32; // confirm which suffix
	*(uint32_t*)type = regs->gpio_dir[gpx] >> suffix & temp;

	return No_exp;
}

static exception_t omapl138_gpio_read_pin_state (conf_object_t *conf_obj, uint32_t index, void *state)
{
	int gpx = 0;
	int suffix = 0;
	int pin_type = 0;
	int ret = 0;
	int mask = 1;

	struct omapl138_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = dev->regs;
	if(index < 0 || index > dev->pin_num)
	{
		skyeye_log(Error_log, __FUNCTION__, "omapl138 gpio pin index %d is out of range, please confirm.\n", index);
		state = NULL;
		return OUTOFRANGE;
	}

	omapl138_gpio_get_pin_type(conf_obj, index, &pin_type);
	if(-1 == ret)
	{
		skyeye_log(Error_log, __FUNCTION__, "omapl138 gpio pin index %d get pin type is error, please confirm.\n", index);
		return OUTOFRANGE;
	}

	gpx = index / 32; // confirm which bank
	suffix = index % 32; // confirm which suffix
	if(INPUT == pin_type) // when type is input  IN_DATA register
	{
		*(uint32_t*)state = regs->gpio_in_data[gpx] >> suffix & mask;
	}
	else  // when type is output OUT_DATA register
	{
		*(uint32_t*)state = regs->gpio_out_data[gpx] >> suffix & mask;
	}

	return No_exp;
}

static exception_t omapl138_gpio_write_pin_state (conf_object_t *conf_obj, uint32_t index, void*state)
{
	int pin_type = 0; // 0 = OUTPUT; 1 = INPUT
	int gpx = 0;
	int suffix = 0;
	int value = 0;
	int ret = 0;

	struct omapl138_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = dev->regs;
	if(index < 0 || index > dev->pin_num)
	{
		skyeye_log(Error_log, __FUNCTION__, "omapl138 gpio pin index %d is out of range, please confirm.\n", index);
		return OUTOFRANGE;
	}

	omapl138_gpio_get_pin_type(conf_obj, index, &pin_type);
	if(-1 == ret)
	{
		skyeye_log(Error_log, __FUNCTION__, "omapl138 gpio pin index %d get pin type is error, please confirm.\n", index);
		return OUTOFRANGE;
	}

	gpx = index / 32; // confirm which bank
	suffix = index % 32; // confirm which suffix
	if(INPUT == pin_type)
	{
		value = *(int*)state;
		// clear value in that index bit
		regs->gpio_in_data[gpx] &= ~(1 << suffix);
		regs->gpio_in_data[gpx] |= value << suffix;
		// todo here
	}
	else // OUTPUT type
	{
		value = *(int*)state;
		if(0 == value)
		{
			regs->gpio_clr_data[gpx] |= 1 << suffix;
			regs->gpio_out_data[gpx] &= ~(1 << suffix);
			regs->gpio_out_data[gpx] |= value << suffix;
		}
		else if(1 == value)
		{
			regs->gpio_set_data[gpx] |= 1 << suffix;
			regs->gpio_out_data[gpx] |= value << suffix;
		}
	}

	return No_exp;
}

static conf_object_t* new_omapl138_gpio(char* obj_name){
	omapl138_gpio_device* dev = skyeye_mm_zero(sizeof(omapl138_gpio_device));
	gpio_reg_t* regs =  skyeye_mm_zero(sizeof(gpio_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	dev->regs = regs;
	int i;
	for (i = 0; i < 5;i++)
		dev->regs->gpio_dir[i] = 0xffffffff;
	dev->regs_name = regs_name;

	dev->gpio0_px_data = 0;
	dev->pin_num  = 144;
	dev->circuit_display = NULL;

	return dev->obj;
}

static exception_t reset_omapl138_gpio (conf_object_t *opaque, const char* args)
{
	int i = 0;
	omapl138_gpio_device* dev = opaque->obj;

	memset(dev->regs, 0, sizeof(gpio_reg_t));
	dev->regs->gpio_revid = 0x44830105;

	for (i = 0; i < 5;i++)
		dev->regs->gpio_dir[i] = 0xffffffff;

	dev->gpio0_px_data = 0;

	return No_exp;
}

static exception_t free_omapl138_gpio(conf_object_t* opaque) {
	omapl138_gpio_device* dev = opaque->obj;

	skyeye_free(dev->regs);
	if(dev->circuit_display);
		skyeye_free(dev->circuit_display);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

static attr_value_t get_attr_gpio0_px_data(void *arg, conf_object_t *conf_obj, attr_value_t *idx)
{
	omapl138_gpio_device* dev = conf_obj->obj;
	attr_value_t gpio0_px = SKY_make_attr_uinteger(dev->gpio0_px_data);
	return gpio0_px;
}
static exception_t  set_attr_gpio0_px_data(void *arg, conf_object_t *conf_obj, attr_value_t *value, attr_value_t *idx)
{
	omapl138_gpio_device* dev = conf_obj->obj;
	dev->gpio0_px_data =SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_connect_pin(void *arg, conf_object_t *conf_obj, attr_value_t *idx)
{
	omapl138_gpio_device* dev = conf_obj->obj;
	attr_value_t connect_list = SKY_make_attr_string(dev->connect_list);
	return connect_list;
}
static exception_t  set_attr_connect_pin(void *arg, conf_object_t *conf_obj, attr_value_t *value, attr_value_t *idx)
{
	omapl138_gpio_device* dev = conf_obj->obj;
	dev->connect_list =SKY_attr_string(*value);
    char tranf[128] = "";
    char *delim_pin = ".";
    char *swap;
    strcpy(tranf,dev->connect_list);
    int i = 0;
    dev->connect_pin[i] = atoi(strtok(tranf,delim_pin));
    while(swap = strtok(NULL,delim_pin)){
        i++;
        dev->connect_pin[i] = atoi(swap);
    }
    dev->connect_quanaity = i+1;
	return No_exp;
}

static attr_value_t get_attr_circuit_display (void *arg, conf_object_t *conf_obj, attr_value_t *idx)
{
	omapl138_gpio_device* dev = conf_obj->obj;
	attr_value_t circuit_display = SKY_make_attr_string(dev->circuit_display);
	return circuit_display;
}

static exception_t set_attr_circuit_display (void *arg, conf_object_t *conf_obj, attr_value_t *value, attr_value_t *idx)
{
	omapl138_gpio_device* dev = conf_obj->obj;
	char *circuit_string = SKY_attr_string(*value);
	dev->circuit_display = skyeye_mm_zero(strlen(circuit_string) + 1);
	strcpy(dev->circuit_display, circuit_string);
	return No_exp;
}

void omapl138_gpio_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "gpio0_px_data",
			get_attr_gpio0_px_data, NULL,
			set_attr_gpio0_px_data, NULL,
			SKY_Attr_Optional, "uinteger",
			"gpio0_px_data_val");
    SKY_register_attribute(clss,"connect_pin",
            get_attr_connect_pin,NULL,
            set_attr_connect_pin,NULL,
            SKY_Attr_Optional,"string",
            "Connected pin, string format: PIN1.PIN2 ... PINn");
	SKY_register_attribute(clss, "circuit_display", get_attr_circuit_display, NULL, set_attr_circuit_display, NULL, SKY_Attr_Optional, "string", "if or not display in MultiSim");
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct omapl138_gpio_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf **)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);
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

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	struct omapl138_gpio_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static exception_t gpio_link_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct omapl138_gpio_device *dev = obj->obj;
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
	struct omapl138_gpio_device *dev = obj->obj;
	*obj2 = dev->gpio_link;
	*port = NULL;
	return No_exp;
}

static exception_t gpio_pin_set_data(conf_object_t *obj, uint32_t pin_num, int status)
{
	struct omapl138_gpio_device *dev = obj->obj;
	gpio_reg_t* regs = dev->regs;
    int group = pin_num / 31;
    int pin = pin_num % 31;
    if(status == PIN_LOW){
        dev->regs->gpio_in_data[group] &= ~(1 << pin);
    }
    else if(status == PIN_HIGH){
        dev->regs->gpio_in_data[group] |= (1 << pin);
    }else{
		skyeye_log(Error_log, __FUNCTION__, "Unexpected value:%d", status);
    }
    return No_exp;
}

void init_omapl138_gpio(){
	static skyeye_class_t class_data = {
		.class_name = "omapl138_gpio",
		.class_desc = "omapl138 gpio",
		.new_instance = new_omapl138_gpio,
		.reset_instance = reset_omapl138_gpio,
		.free_instance = free_omapl138_gpio,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_omapl138 SC_c6748),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = omapl138_gpio_read,
		.write = omapl138_gpio_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = omapl138_gpio_get_regvalue_by_id,
		.get_regname_by_id = omapl138_gpio_get_regname_by_id,
		.set_regvalue_by_id = omapl138_gpio_set_regvalue_by_id,
		.get_regnum = omapl138_gpio_get_regnum,
		.get_regid_by_name = omapl138_gpio_get_regid_by_name,
		.get_regoffset_by_id = omapl138_gpio_get_regoffset_by_id,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static const gpio_pin_set gpio_pin_intf = {
        .set_gpio = gpio_pin_set_data,
    };
    SKY_register_iface(clss, GPIO_PIN_SET, &gpio_pin_intf);

	static const skyeye_pin_info_intf pin_intf = {
		.get_pin_num = omapl138_gpio_get_pin_num,
		.get_pin_type = omapl138_gpio_get_pin_type,
		.read_pin_state = omapl138_gpio_read_pin_state,
		.write_pin_state = omapl138_gpio_write_pin_state,
	};
	SKY_register_iface(clss, SKYEYE_PIN_INFO_INTF, &pin_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
        (struct InterfaceDescription) {
            .name = GPIO_PIN_SET,
            .iface = &gpio_pin_intf,
        },
		(struct InterfaceDescription) {
			.name = SKYEYE_PIN_INFO_INTF,
			.iface = &pin_intf,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
        (struct ConnectDescription) {
            .name = GPIO_LINKER_PIN_INTF,
            .set = gpio_link_set,
            .get = gpio_link_get,
        },
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	omapl138_gpio_register_attribute(clss);
}

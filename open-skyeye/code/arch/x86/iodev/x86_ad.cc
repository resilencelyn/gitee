/* Copyright (C) 
* 2015 - Michael.Kang blackfin.kang@gmail.com
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
* @file x86_ad.cc
* @brief 
* @author zyuming zyumingfit@163.com
* @version 7849
* @date 2015-07-23
*/

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_AD 

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
#include <skyeye_core.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"



#include "cpu/cpu.h"
#include "x86_ad.h"


bx_ad_c *adDevice = NULL;
#define LOG_THIS adDevice->
#define BX_AD_THIS this->


static exception_t x86_ad_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count);
static exception_t x86_ad_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count);


int libad_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  adDevice = new bx_ad_c();
  bx_devices.pluginad = adDevice;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, adDevice, BX_PLUGIN_AD);

  return(0); // Success
}

void libad_LTX_plugin_fini(void)
{
  delete adDevice;
}

struct bx_ad_s_type {
  Bit16u ad_registers[8]; /* different gpio value */
} bx_ad_s;


bx_ad_c::bx_ad_c()
{
  put("IODBG");
}

void bx_ad_c::init(void)
{
  DEV_register_ioread_handler_range(this, read_handler, AD_BASE, AD_BASE + AD_LEN, "BOCHS AD", 4);
  DEV_register_iowrite_handler_range(this, write_handler, AD_BASE, AD_BASE + AD_LEN, "BOCHS AD", 4);
}

Bit32u bx_ad_c::read_handler(void *this_ptr, Bit32u addr, unsigned io_len)
{
  bx_ad_c *bx_ad_ptr = (bx_ad_c *) this_ptr;
  return bx_ad_ptr->read(addr, io_len);
}

Bit32u bx_ad_c::read(Bit32u addr, unsigned io_len)
{
	Bit32u data;
	x86_ad_read((conf_object_t*)adDevice->obj, addr - AD_BASE, &data,  io_len);
	return data;
}

void bx_ad_c::write_handler(void *this_ptr, Bit32u addr, Bit32u dvalue, unsigned io_len)
{
  bx_ad_c *bx_ad_ptr = (bx_ad_c *) this_ptr;
  bx_ad_ptr->write(addr, dvalue, io_len);
}

void bx_ad_c::write(Bit32u addr, Bit32u dvalue, unsigned io_len)
{
	conf_object_t *obj = adDevice->obj;
	x86_ad_write(adDevice->obj, addr - AD_BASE, &dvalue,  io_len);
	
	return;
}

void bx_ad_c::mem_read(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data){
	return;
}

void bx_ad_c::mem_write(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data){
}


static void x86_ad_raise_signal(conf_object_t *obj)
{
	x86_ad_dev *dev = obj->obj;
}

static void start_ad_sample(x86_ad_dev *dev, void *ad_buffer, int length){
	ad_reg_t* ad_regs = dev->ad_regs;
	double ad_data[AD_CH_NUM] = {0.0};
	int i;
	memcpy(ad_data, ad_buffer, AD_CH_NUM * 8);

	for(i = 0; i < AD_CH_NUM; i++){
		ad_regs->ad_data[i] = ((int)(ad_data[i] / 0.01465 + 0x800));
		if ((ad_regs->ad_data[i] >> 31) & 0x1){ //-30v
			ad_regs->ad_data[i] = 0;
		}
		if (ad_regs->ad_data[i] > 0xFFF){ //+30v
			ad_regs->ad_data[i] = 0xFFF;
		}
	}
}
static exception_t x86_ad_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	x86_ad_dev *dev = opaque->obj;
	ad_reg_t* ad_regs = dev->ad_regs;
	int m = 0;
	if (offset >= 4 && offset <= 0x10){
		m = (offset - 4) / 4;
		*(uint32_t *)buf = ad_regs->ad_data[m];
		return No_exp;
	}
	switch(offset) {
		case 0x0:
			*(uint32_t *)buf = ad_regs->ad_mode;
			break;
		default:
			printf("Can not read the register at 0x%x in ad_read\n", offset);
			return Invarg_exp;
	}

	return  No_exp;
}

static exception_t x86_ad_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	x86_ad_dev *dev = opaque->obj;
	ad_reg_t* regs = dev->ad_regs;
	uint32_t val = *(uint32_t*)buf;
	//printf("%s, %d, offset = %x, buf = %x, %s\n", __func__, __LINE__, offset, *(uint32_t *)buf, opaque->objname);
	switch(offset) {
		case 0x0:
			regs->ad_mode = val;
			break;
		default:
			printf("Can not write the register at 0x%x in ad_write\n", offset);
		break;
	}

	return  No_exp;
}

static void ad_register_attribute(conf_class_t* clss) {
	return;
}

static conf_object_t* new_x86_ad(char* obj_name)
{
	x86_ad_dev* dev = skyeye_mm_zero(sizeof(x86_ad_dev));
	double ad_buffer[AD_CH_NUM] = {20.0, 24.5, 22.8, 20.4}; 
	int i;
	if(dev == NULL)
		printf("alloc memmory for obj in %x fail\n", obj_name);
	ad_reg_t* ad_regs = skyeye_mm_zero(sizeof(ad_reg_t));
	dev->ad_regs = ad_regs;
	if(ad_regs == NULL)
		printf("alloc memmory for regs in %x fail\n", obj_name);

	for(i = 0; i < 4; i++){
		dev->ad_regs->ad_mode = 0;
		dev->ad_regs->ad_data[i] = 5;
	}
	dev->obj = new_conf_object(obj_name, dev);
	adDevice->obj = dev->obj;
	start_ad_sample(dev, ad_buffer, AD_CH_NUM);

	return dev->obj;
}

static exception_t reset_x86_ad(conf_object_t* opaque, const char* args)
{
	x86_ad_dev *dev = opaque->obj;

	return No_exp;        
}

static exception_t free_x86_ad(conf_object_t* conf_obj)
{
	x86_ad_dev *dev = conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return No_exp;
}
char* x86_ad_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	x86_ad_dev *dev = conf_obj->obj;
	return regs_name[id];
}

uint32_t x86_ad_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	x86_ad_dev *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)&(dev->ad_regs) + id;
	return *regs_value;
}


static char* x86_ad_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	x86_ad_dev *dev = conf_obj->obj;
	ad_reg_t* regs = dev->ad_regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t x86_ad_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	x86_ad_dev *dev = conf_obj->obj;
	ad_reg_t* regs = dev->ad_regs;
	return  17;
}

static uint32_t x86_ad_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	x86_ad_dev *dev = conf_obj->obj;
	ad_reg_t* regs = dev->ad_regs;
	uint32_t regnum;
	regnum = sizeof(ad_reg_t) / 4;;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static int x86_ad_receive(conf_object_t *obj, can_msg_t *msg){
	x86_ad_device *dev = obj->obj;

	return 0;
}

extern "C" void init_x86_ad();
void init_x86_ad(){
	static skyeye_class_t class_data = {
		class_name : "x86_ad",
		class_desc : "x86_ad",
		new_instance : new_x86_ad,
		free_instance : free_x86_ad,
		reset_instance : reset_x86_ad,
		get_attr : NULL,
		set_attr : NULL
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
		/* Register io function to the object */
		static const memory_space_intf io_memory = {
			conf_obj:NULL,
			read : x86_ad_read,
			write : x86_ad_write,
		};
		SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
		static const skyeye_reg_intf reg_intf = {
			conf_obj:NULL,
			get_regvalue_by_id : x86_ad_get_regvalue_by_id,
			get_regname_by_id : x86_ad_get_regname_by_id,
			set_regvalue_by_id : x86_ad_set_regvalue_by_id,
			get_regid_by_name : x86_ad_get_regid_by_name,
			get_regnum : x86_ad_get_regnum,
		};
		SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
		static const can_dev_intf can_intf = {
			receive_msg:x86_ad_receive,
		};
		SKY_register_iface(clss, CAN_DEV_INTF, &can_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			name : SKYEYE_REG_INTF,
			iface : &reg_intf,
		},
		(struct InterfaceDescription) {
			name : MEMORY_SPACE_INTF_NAME, 
			iface : &io_memory,
		},
		
 	};
	static const struct ConnectDescription connects[] = {
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	ad_register_attribute(clss);
}

#endif 


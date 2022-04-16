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
* @file bu_61580.cc
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

#if BX_SUPPORT_BU_61580 

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
#include "bu_61580.h"


bx_bu_61580_c *theBu_61580Device = NULL;
#define LOG_THIS theBu_61580Device->
#define BX_BU_61580_THIS this->


static exception_t x86_bu61580_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count);
static exception_t x86_bu61580_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count);


int libbu_61580_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theBu_61580Device = new bx_bu_61580_c();
	//BX_INFO(("KIO plugin #################\n"));
	//fprintf(stderr, "KIO plugin #################\n");
  bx_devices.pluginBu_61580 = theBu_61580Device;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theBu_61580Device, BX_PLUGIN_KIO);

  return(0); // Success
}

void libbu_61580_LTX_plugin_fini(void)
{
  delete theBu_61580Device;
}

struct bx_bu_61580_s_type {
  Bit16u bu_61580_registers[8]; /* different gpio value */
} bx_bu_61580_s;


bx_bu_61580_c::bx_bu_61580_c()
{
  put("IODBG");
}

void bx_bu_61580_c::init(void)
{
  DEV_register_ioread_handler_range(this, read_handler, 0x9000, 0x9000+ 0x1840, "BOCHS BU_61580", 4);
  DEV_register_iowrite_handler_range(this, write_handler, 0x9000, 0x9000+ 0x1840, "BOCHS BU_61580", 4);
}

Bit32u bx_bu_61580_c::read_handler(void *this_ptr, Bit32u addr, unsigned io_len)
{
  bx_bu_61580_c *bx_bu_61580_ptr = (bx_bu_61580_c *) this_ptr;
  return bx_bu_61580_ptr->read(addr, io_len);
}

Bit32u bx_bu_61580_c::read(Bit32u addr, unsigned io_len)
{
	Bit32u data;
		
	x86_bu61580_read((conf_object_t*)theBu_61580Device->obj, addr - PointBase, &data,  io_len);
	return data;
}

void bx_bu_61580_c::write_handler(void *this_ptr, Bit32u addr, Bit32u dvalue, unsigned io_len)
{
  bx_bu_61580_c *bx_bu_61580_ptr = (bx_bu_61580_c *) this_ptr;
  bx_bu_61580_ptr->write(addr, dvalue, io_len);
}

void bx_bu_61580_c::write(Bit32u addr, Bit32u dvalue, unsigned io_len)
{
	conf_object_t *obj = theBu_61580Device->obj;
	x86_bu61580_write(theBu_61580Device->obj, addr - PointBase, &dvalue,  io_len);
	
	return;
}

void bx_bu_61580_c::mem_read(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data){
	return;
}

void bx_bu_61580_c::mem_write(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data){
}



/*skyeye moudle*/
#define Memory_Size 0x2000
#define OK      0 
#define Error  -1 

static int x86_bu61580_read_data(conf_object_t* opaque, void* buf, int length)
{
	x86_bu61580_dev *dev = (x86_bu61580_dev *)opaque->obj;
	int i;
	memcpy(buf, dev->send_buf, length * 2);
	return OK;
}

static int x86_bu61580_write_data(conf_object_t* opaque, void* recv_buf, int length, int sub_address)
{
	x86_bu61580_dev *dev =  (x86_bu61580_dev *)opaque->obj;
	x86_bu61580_reg_t* regs = dev->regs;

	if (dev->std_1553b_bus_iface != NULL){
		dev->std_1553b_bus_iface->read_data(dev->std_1553b_bus_obj, opaque, dev->recv_buf, length, sub_address);
	}else{
		printf("In %s, std_1553b_bus_iface Failed.\n", __FUNCTION__);
		return Error;
	}

	if ((regs->config_1 >> 13) & 0x1){
		dev->DataBlock = dev->DataBlockB_offset;
	}else
		dev->DataBlock = dev->DataBlockA_offset;
	memcpy((dev->DataBlockA + dev->DataBlock + dev->cmd_reg.flag.sub_address * 32), dev->recv_buf, length);    
	return OK;
}
static int x86_bu61580_set_state(conf_object_t* obj, int length)
{
	x86_bu61580_dev *dev = obj->obj;
	dev->recv_length = length;

	return OK;
}
static int x86_bu61580_get_address(conf_object_t* opaque)
{
	x86_bu61580_dev *dev =  (x86_bu61580_dev *)opaque->obj;
	return OK;
}


static exception_t x86_bu61580_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	x86_bu61580_dev *dev = opaque->obj;
	x86_bu61580_reg_t* regs = dev->regs;
	switch(offset)
	{
		case 0x100:
			*(uint16 *)buf = dev->stkptrA;
			break;
		case 0x104:
			*(uint16 *)buf = dev->stkptrB;
			break;
		case 0x800:
			*(uint16 *)buf = regs->intrpt_mask;
			break;
		case 0x804:
			*(uint16 *)buf = regs->config_1;
			break;
		case 0x808:
			*(uint16 *)buf = regs->config_2;
			break;
		case 0x80c:
			*(uint16 *)buf = regs->start_reset;
			break;
		case 0x810:
			*(uint16 *)buf = regs->stackptr;
			break;
		case 0x814:
			*(uint16 *)buf = regs->ctrl_word;
			break;
		case 0x818:
			*(uint16 *)buf = regs->timetag;
			break;
		case 0x81c:
			*(uint16 *)buf = regs->intrpt_sta;
			break;
		case 0x820:
			*(uint16 *)buf = regs->config_3;
			break;
		case 0x824:
			*(uint16 *)buf = regs->config_4;
			break;
		case 0x828:
			*(uint16 *)buf = regs->config_5;
			break;
		case 0x82c:
			*(uint16 *)buf = regs->mtd_data_stk;
			break;
		case 0x830:
			*(uint16 *)buf = regs->bc_fmetimer;
			break;
		case 0x834:
			*(uint16 *)buf = regs->bc_msgtimer;
			break;
		case 0x838:
			*(uint16 *)buf = regs->bc_frmtime;
			break;
		case 0x83c:
			*(uint16 *)buf = regs->rt_stawd;
			break;
		case 0x840:
			*(uint16 *)buf = regs->rt_bitword;
			break;
        default:{
			if (offset >= 0 && offset <= 0xff){
				memcpy(buf, dev->stkptr + offset , 2);
			}else
			if (offset >= 0x140 && offset <= 0x15f){
				memcpy(buf, dev->TabASaRx + offset , 2);
			}else
			if (offset >= 0x160 && offset <= 0x17f){
				memcpy(buf, dev->TabASaTx + offset , 2);
			}else
			if (offset >= 0x180 && offset <= 0x19f){
				memcpy(buf, dev->TabASaBcst + offset , 2);
			}else
			if (offset >= 0x1a0 && offset <= 0x1bf){
				memcpy(buf, dev->TabASaCtrl_Word + offset , 2);
			}else
			if (offset >= 0x1c0 && offset <= 0x1df){
				memcpy(buf, dev->TabBSaRx + offset , 2);
			}else
			if (offset >= 0x1e0 && offset <= 0x1ff){
				memcpy(buf, dev->TabBSaTx + offset , 2);
			}else
			if (offset >= 0x200 && offset <= 0x21f){
				memcpy(buf, dev->TabBSaBcst + offset , 2);
			}else
			if (offset >= 0x220 && offset <= 0x23f){
				memcpy(buf, dev->TabBSaCtrl_Word + offset , 2);
			}else
			if (offset >= 0x240 && offset <= 0x247){
				memcpy(buf, dev->BusyLookTab + offset , 2);
			}else
			if (offset >= 0x260 && offset <= 0x6ff){
				memcpy(buf, dev->DataBlockA + offset , 1);
			}else
			if (offset >= 0x700 && offset <= 0xfff){
				memcpy(buf, dev->DataBlockB + offset , 1);
			}else{
				printf("read %s error offset %d : 0x%x\n",dev->obj->objname, offset, *(uint16*)buf);
				break;
			}
		}
	}       

	return  No_exp;
}

static exception_t x86_bu61580_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	x86_bu61580_dev *dev = opaque->obj;
	x86_bu61580_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t *)buf;


	switch(offset)
	{
		case 0x100:
			dev->stkptrA = val;
			break;
		case 0x104:
			dev->stkptrB = val;
			break;
		case 0x800:
			regs->intrpt_mask = val;
			break;
		case 0x804:
			regs->config_1 = val;
			break;
		case 0x808:
			regs->config_2 = val;
			break;
		case 0x80c:
			regs->start_reset = val;
			break;
		case 0x810:
			regs->stackptr = val;
			break;
		case 0x814:
			regs->ctrl_word = val;
			break;
		case 0x818:
			regs->timetag = val;
			break;
		case 0x81c:
			regs->intrpt_sta = val;
			break;
		case 0x820:
			regs->config_3 = val;
			break;
		case 0x824:
			regs->config_4 = val;
			break;
		case 0x828:
			regs->config_5 = val;
			break;
		case 0x82c:
			regs->mtd_data_stk = val;
			break;
		case 0x830:
			regs->bc_fmetimer = val;
			break;
		case 0x834:
			regs->bc_msgtimer = val;
			break;
		case 0x838:
			regs->bc_frmtime = val;
			break;
		case 0x83c:
			regs->rt_stawd = val;
			break;
		case 0x840:
			regs->rt_bitword = val;
			break;
		default:{
			if (offset >= 0 && offset <= 0xff){
				memcpy(dev->stkptr + offset , buf, 2);
				if (((*(dev->stkptr) >> 8) & 0x1) && ((*(dev->stkptr) >> 1) & 0x1)){
					dev->cmd_word = *(dev->stkptr); 
					if (dev->std_1553b_bus_iface != NULL){
						dev->std_1553b_bus_iface->write_data(dev->std_1553b_bus_obj, opaque, dev->send_buf, dev->data_cnt, dev->dev_id);
						dev->data_cnt = 0;
					}else{ 
						printf("In %s, std_1553b_bus_iface Send data Failed.\n", __FUNCTION__);
						break;
					}
				}else if (!((*(dev->stkptr) >> 8) & 0x1) && ((*(dev->stkptr) >> 1) & 0x1)){
					dev->cmd_word = *(dev->stkptr); 
					regs->ctrl_word |= 0x1 << 9;
					dev->cmd_reg.flag.sub_address = (dev->cmd_word >> 9) & 0x1f;
					if (dev->std_1553b_bus_iface != NULL){
						if (Error == x86_bu61580_write_data(opaque, dev->recv_buf, dev->recv_length, dev->cmd_reg.flag.sub_address)){
							printf("In %s, x86_bu61580 recv data error.\n", __FUNCTION__);
							break;
						}
					}else{
						printf("In %s, std_1553b_bus_iface not Connect.\n", __FUNCTION__);
					}
					break;
				}
			}else
			if (offset >= 0x140 && offset <= 0x15f){
				memcpy(dev->TabASaRx + offset , buf, 2);
					break;
			}else
			if (offset >= 0x160 && offset <= 0x17f){
				memcpy(dev->TabASaTx + offset , buf, 2);
					break;
			}else
			if (offset >= 0x180 && offset <= 0x19f){
				memcpy(dev->TabASaBcst + offset , buf, 2);
					break;
			}else
			if (offset >= 0x1a0 && offset <= 0x1bf){
				memcpy(dev->TabASaCtrl_Word + offset , buf, 2);
					break;
			}else
			if (offset >= 0x1c0 && offset <= 0x1df){
				memcpy(dev->TabBSaRx + offset , buf, 2);
					break;
			}else
			if (offset >= 0x1e0 && offset <= 0x1ff){
				memcpy(dev->TabBSaTx + offset , buf, 2);
					break;
			}else
			if (offset >= 0x200 && offset <= 0x21f){
				memcpy(dev->TabBSaBcst + offset , buf, 2);
					break;
			}else
			if (offset >= 0x220 && offset <= 0x23f){
				memcpy(dev->TabBSaCtrl_Word + offset , buf, 2);
					break;
			}else
			if (offset >= 0x240 && offset <= 0x247){
				memcpy(dev->BusyLookTab + offset , buf, 2);
					break;
			}else
			if (offset >= 0x260 && offset <= 0x6ff){
				memcpy(dev->DataBlockA + offset , buf, 1);
				if ((regs->config_1 >> 15) & 0x1)
				{
					memcpy(dev->send_buf + dev->data_cnt, buf, 1);
					dev->data_cnt += 1;
				}
					break;
			}else
			if (offset >= 0x700 && offset <= 0xfff){
				memcpy(dev->DataBlockB + offset , buf, 1);
				if ((regs->config_1 >> 15) & 0x1)
				{
					memcpy(dev->send_buf + dev->data_cnt, buf, 1);
					dev->data_cnt += 1;
				}
					break;
			}else{
				printf("read %s error offset %d : 0x%x\n", __FUNCTION__, offset, *(uint16*)buf);
				break;
			}
		}
	}
	return  No_exp;
}

static conf_object_t* new_x86_bu61580(char* obj_name)
{
	x86_bu61580_dev* dev = skyeye_mm_zero(sizeof(x86_bu61580_dev));
	x86_bu61580_reg_t* regs =  skyeye_mm_zero(sizeof(x86_bu61580_reg_t));

	dev->obj = new_conf_object(obj_name, dev);
	dev->ptr = (uint16_t *)skyeye_mm_zero(Memory_Size);
	dev->DataBlockA = (char *)skyeye_mm_zero(Memory_Size);
	dev->DataBlockB = (char *)skyeye_mm_zero(Memory_Size);
	dev->recv_buf = (char *)skyeye_mm_zero(32 * sizeof(int));
	dev->send_buf = (char *)skyeye_mm_zero(32 * sizeof(int));
	dev->stkptr = dev->ptr;
	dev->stkptrA = 0;
	dev->stkptrB = 0;
	dev->TabASaRx = dev->ptr;
	dev->TabASaTx = dev->ptr;
	dev->TabASaBcst = dev->ptr;
	dev->TabASaCtrl_Word = dev->ptr;
	dev->TabBSaRx = dev->ptr;
	dev->TabBSaTx = dev->ptr;
	dev->TabBSaBcst = dev->ptr;
	dev->TabBSaCtrl_Word = dev->ptr;
	dev->BusyLookTab = dev->ptr;
	dev->data_cnt = 0;
	dev->DataBlockA_offset = 0x260;
	dev->DataBlockB_offset = 0x700;
	dev->DataBlock = 0;
	dev->regs = regs;
	theBu_61580Device->obj = dev->obj;
	return dev->obj;
}

static exception_t reset_x86_bu61580(conf_object_t* opaque, const char* args)
{
	x86_bu61580_dev *dev = opaque->obj;

	return No_exp;        
}

static exception_t free_x86_bu61580(conf_object_t* conf_obj)
{
	x86_bu61580_dev *dev = conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return No_exp;
}
char* x86_bu61580_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	x86_bu61580_dev *dev = conf_obj->obj;
	return dev->regs_name[id];
}

uint32_t leon2_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	x86_bu61580_dev *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)&(dev->regs) + id;
	return *regs_value;
}


static char* x86_bu61580_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	x86_bu61580_dev *dev = conf_obj->obj;
	x86_bu61580_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t x86_bu61580_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	x86_bu61580_dev *dev = conf_obj->obj;
	x86_bu61580_reg_t* regs = dev->regs;
	return  17;
}

static uint32_t x86_bu61580_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	x86_bu61580_dev *dev = conf_obj->obj;
	x86_bu61580_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(x86_bu61580_reg_t) / 4;;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static attr_value_t get_attr_dev_id(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	x86_bu61580_dev *dev = conf_obj->obj;
	attr_value_t dev_id = SKY_make_attr_uinteger(dev->dev_id);
	return dev_id;
}

static exception_t set_attr_dev_id(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	x86_bu61580_dev *dev = conf_obj->obj;
	dev->dev_id = SKY_attr_uinteger(*value);

	return No_exp;
}
static void x86_bu61580_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "dev_id", get_attr_dev_id, NULL, set_attr_dev_id, NULL, SKY_Attr_Optional, "uinteger", "device number ");
	return;
}
static exception_t std_1553b_bus_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	x86_bu61580_dev *dev = obj->obj;
	*obj2 = dev->std_1553b_bus_obj;
	*port = NULL;
	return No_exp;
}
static exception_t std_1553b_bus_set(conf_object_t* obj, conf_object_t* obj2,const char *port, int index){
	x86_bu61580_dev *dev = obj->obj;
	dev->std_1553b_bus_obj = obj2;
	dev->std_1553b_bus_iface = (std_1553b_bus_intf *)SKY_get_iface(dev->std_1553b_bus_obj, STD_1553B_BUS_INTF_NAME);
	if (dev->std_1553b_bus_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Get 'std_1553b_bus_iface' interface from %s fail.", obj2->objname);
	}
	return No_exp;
}
extern "C" void init_x86_bu61580();
void init_x86_bu61580(){
	static skyeye_class_t class_data = {
		class_name : "x86_bu61580",
		class_desc : "x86_bu61580",
		new_instance : new_x86_bu61580,
		free_instance : free_x86_bu61580,
		reset_instance : reset_x86_bu61580,
		get_attr : NULL,
		set_attr : NULL
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
		/* Register io function to the object */
		static const memory_space_intf io_memory = {
			conf_obj:NULL,
			read : x86_bu61580_read,
			write : x86_bu61580_write,
		};
		SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
		static const bu61580_dev_intf x86_bu61580_dev_iface = {
			conf_obj:NULL,
			read : x86_bu61580_read_data,
			write : x86_bu61580_write_data,
			set_state : x86_bu61580_set_state,
		};
		SKY_register_iface(clss, BU61580_DEV_INTF_NAME, &x86_bu61580_dev_iface);
		static const skyeye_reg_intf reg_intf = {
			conf_obj:NULL,
			get_regvalue_by_id : leon2_get_regvalue_by_id,
			get_regname_by_id : x86_bu61580_get_regname_by_id,
			set_regvalue_by_id : x86_bu61580_set_regvalue_by_id,
			get_regid_by_name : x86_bu61580_get_regid_by_name,
			get_regnum : x86_bu61580_get_regnum,
		};
		SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			name : BU61580_DEV_INTF_NAME,
			iface : &x86_bu61580_dev_iface,
		},
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
		(struct ConnectDescription ){
			name : STD_1553B_BUS_INTF_NAME,
			set : std_1553b_bus_set,
			get : std_1553b_bus_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	x86_bu61580_register_attribute(clss);
}

#endif 


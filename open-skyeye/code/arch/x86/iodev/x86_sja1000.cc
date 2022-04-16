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
* @file x86_sja1000.cc
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

#if BX_SUPPORT_SJA1000 

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
#include "x86_sja1000.h"
#include "virt_timer.h"

bx_sja1000_c *sja1000Device = NULL;
#define LOG_THIS sja1000Device->
#define BX_SJA1000_THIS this->

#define RAISE_SIGNAL 1
#define LOWER_SIGNAL 0

static exception_t x86_sja1000_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count);
static exception_t x86_sja1000_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count);
static void can_sja1000_set_signal(conf_object_t *obj, int level);

int libsja1000_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  sja1000Device = new bx_sja1000_c();
  bx_devices.pluginsja1000 = sja1000Device;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, sja1000Device, BX_PLUGIN_SJA1000);

  return(0); // Success
}

void libsja1000_LTX_plugin_fini(void)
{
  delete sja1000Device;
}

struct bx_sja1000_s_type {
  Bit16u sja1000_registers[8]; /* different gpio value */
} bx_sja1000_s;


bx_sja1000_c::bx_sja1000_c()
{
  put("IODBG");
}

void bx_sja1000_c::init(void)
{
  DEV_register_ioread_handler_range(this, read_handler, SJA1000_BASE, SJA1000_BASE + SJA1000_LEN, "BOCHS SJA1000", 4);
  DEV_register_iowrite_handler_range(this, write_handler, SJA1000_BASE, SJA1000_BASE + SJA1000_LEN, "BOCHS SJA1000", 4);
}

Bit32u bx_sja1000_c::read_handler(void *this_ptr, Bit32u addr, unsigned io_len)
{
  bx_sja1000_c *bx_sja1000_ptr = (bx_sja1000_c *) this_ptr;
  return bx_sja1000_ptr->read(addr, io_len);
}

Bit32u bx_sja1000_c::read(Bit32u addr, unsigned io_len)
{
	Bit32u data;
	x86_sja1000_read((conf_object_t*)sja1000Device->obj, addr - SJA1000_BASE, &data,  io_len);
	return data;
}

void bx_sja1000_c::write_handler(void *this_ptr, Bit32u addr, Bit32u dvalue, unsigned io_len)
{
  bx_sja1000_c *bx_sja1000_ptr = (bx_sja1000_c *) this_ptr;
  bx_sja1000_ptr->write(addr, dvalue, io_len);
}

void bx_sja1000_c::write(Bit32u addr, Bit32u dvalue, unsigned io_len)
{
	conf_object_t *obj = sja1000Device->obj;
	x86_sja1000_write(sja1000Device->obj, addr - SJA1000_BASE, &dvalue,  io_len);
	
	return;
}

void bx_sja1000_c::mem_read(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data){
	return;
}

void bx_sja1000_c::mem_write(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data){
}


static void x86_sja1000_raise_signal(conf_object_t *obj)
{
	x86_sja1000_dev *dev = obj->obj;
}


static exception_t can_sja1000_transmit(conf_object_t *opaque, void *buf, uint32_t nbytes){
	int i;
	x86_sja1000_device *dev = opaque->obj;
	can_msg_t msg;
	msg.ide = 1;
	msg.id = 0;
	msg.id |= ((dev->regs->tx_id[0]) & 0xff) << 24;
	msg.id |= ((dev->regs->tx_id[1]) & 0xff) << 16;
	msg.id |= ((dev->regs->tx_id[2]) & 0xff) << 8;
	msg.id |= ((dev->regs->tx_id[3]) & 0xff);
	msg.id = msg.id >> 3;
	msg.dlc = dev->regs->tx_fid & 0xf;
	msg.channel_id = dev->regs->channel_id;
	memcpy(msg.data, buf, 8);
#if 0
	printf("identifier:%x, dlc:%x, send data:", msg.id, msg.dlc);
	for(i = 0; i < msg.dlc; i++)
		printf("%x ", msg.data[i]);
	printf("\n");
#endif
	
	if(dev->can_linker_iface != NULL){
		dev->can_linker_iface->send_msg(dev->can_linker, opaque, &msg);
	}
	return No_exp;

}
static exception_t x86_sja1000_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	x86_sja1000_dev *dev = opaque->obj;
	x86_sja1000_reg_t* regs = dev->regs;
	cpld_reg_t* cpld_regs = dev->cpld_regs;
	switch(offset) {
	case 0x0:
		*(uint32_t *)buf = cpld_regs->offset;
		break;
	case 0x4:
		switch(cpld_regs->offset){
		case 0x0:
			*(uint8_t *)buf = regs->mode;
			break;
		case 0x1:
			*(uint8_t *)buf = regs->cmd;
			break;
		case 0x2:
			*(uint8_t *)buf = regs->sr;
			break;
		case 0x3:
			*(uint8_t *)buf = regs->ir;
			break;
		case 0x4:
			*(uint8_t *)buf = regs->ier;
			break;
		case 0x6:
			*(uint8_t *)buf = regs->btr0;
			break;
		case 0x7:
			*(uint8_t *)buf = regs->btr1;
			break;
		case 0x8:
			*(uint8_t *)buf = regs->output_ctrl;
			break;
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
			if(regs->mode & 0x8){ /* normal mode */
				if(cpld_regs->offset == 0x10){
					*(uint8_t *)buf = regs->rx_fid;
				}
				else if(cpld_regs->offset >= 0x11 && cpld_regs->offset < 0x15){
					*(uint8_t *)buf = regs->rx_id[cpld_regs->offset - 0x11];
				}else{
					*(uint8_t *)buf = regs->rx_data[cpld_regs->offset - 0x15];
				}
			}
			else{ /* reset mode */
				if(cpld_regs->offset < 0x14)
					*(uint8_t *)buf = regs->acr[cpld_regs->offset - 0x10];
				else
					*(uint8_t *)buf = regs->amr[cpld_regs->offset - 0x14];
			}
			break;
		case 0x18:
		case 0x19:
                case 0x1a:
                case 0x1b:
                case 0x1c:
			*(uint8_t *)buf = regs->rx_data[cpld_regs->offset - 0x15];
                        break;

		case 0x1f:
			*(uint8_t *)buf = regs->cdr;
			break;
		case 0x20:
			*(uint8_t *)buf = regs->rcv_cnt;
			break;
		case 0x24:
			*(uint8_t *)buf = dev->can_id; /*CAN¿¨ID*/
			break;
		case 0x28:
			*(uint8_t *)buf = regs->channel_id; /*Can Í¨µÀIDºÅ*/
			break;
		default:
			
			printf("Can not read the register at 0x%x in can_sja1000\n", cpld_regs->offset);
			break;
		}
		break;
	default:
		printf("Can not read the register at 0x%x in cpld\n", offset);
		return Invarg_exp;
	}

	//printf("%s, %d, offset = %x, buf = %x\n", __func__, __LINE__, offset, *(uint32_t *)buf);
	return  No_exp;
}

static exception_t x86_sja1000_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	x86_sja1000_dev *dev = opaque->obj;
	x86_sja1000_reg_t* regs = dev->regs;
	cpld_reg_t* cpld_regs = dev->cpld_regs;
	uint32_t val = *(uint32_t*)buf;
	//printf("%s, %d, offset = %x, buf = %x, %s\n", __func__, __LINE__, offset, *(uint32_t *)buf, opaque->objname);
	switch(offset) {
		case 0x0:
			cpld_regs->offset = val;
			break;
		case 0x4:
			switch(cpld_regs->offset){
			case 0x0:
				regs->mode = val;
				break;
			case 0x1:
				regs->cmd = val;
				if(regs->cmd & 0x1){ /* the send request */
					
					char msg_buf[8];
					memcpy(msg_buf, regs->tx_data, 8);
					can_sja1000_transmit(opaque, msg_buf, 8);

					regs->sr |= 0x4;

				}
				if(regs->cmd & 0x4){ /* release receive buffer */
					dev->now_msg_out = 1;
					regs->ier = 1;

					regs->sr &= ~0x1;
					if (regs->rcv_cnt > 0)
						regs->rcv_cnt --;

					if (CanFIFOCheckCanReadNum(dev->rec_fifo) == 0){ //fix me
						dev->regs->ir &= ~0x1; 
					}
				}
				break;
			case 0x2:
				regs->sr = val;
				break;
			case 0x3:
				regs->ir = val;
				break;
			case 0x4:
				regs->ier = val;
				if(regs->ier & PELI_IR_TI){
				}
				if (regs->ier == 0){
					can_sja1000_set_signal(opaque, LOWER_SIGNAL);
				}
				break;
			case 0x6:
				regs->btr0 = val;
				break;
			case 0x7:
				regs->btr1 = val;
				break;
			case 0x8:
				regs->output_ctrl = val;
				break;
			case 0x10:
			case 0x11:
			case 0x12:
			case 0x13:
			case 0x14:
			case 0x15:
			case 0x16:
			case 0x17:
				if(regs->mode & 0x8){ /* normal mode */
					if(cpld_regs->offset == 0x10)
						regs->tx_fid = val;
					else if(cpld_regs->offset >= 0x11 && cpld_regs->offset < 0x15)
						regs->tx_id[cpld_regs->offset - 0x11] = val;
					else{
						regs->tx_data[cpld_regs->offset - 0x15] = val;
					}
				}
				else{ /* reset mode */
					if(cpld_regs->offset < 0x14)
						regs->acr[cpld_regs->offset - 0x10] = val;
					else
						regs->amr[cpld_regs->offset - 0x14] = val;
				}
				break;
			case 0x18:
			case 0x19:
			case 0x1a:
			case 0x1b:
			case 0x1c:
				regs->tx_data[cpld_regs->offset - 0x15] = val;
				break;
			case 0x1f:
				regs->cdr = val;
				break;
			case 0x20:
				regs->rcv_cnt = val;
				break;
			case 0x24:
				dev->can_id = val;
				break;
			case 0x28:
				regs->channel_id = val;
				break;
			default:
				printf("Can not write the register at 0x%x in can_sja1000\n", cpld_regs->offset);
				break;
			}

			break;
		
		default:
			printf("Can not write the register at 0x%x in cpld\n", offset);
			return Invarg_exp;
	}


	return  No_exp;
}


static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	x86_sja1000_dev *dev =  conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	x86_sja1000_dev *dev =  conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_can_id(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	x86_sja1000_dev *dev =  conf_obj->obj;
	attr_value_t can_id = SKY_make_attr_uinteger(dev->can_id);
	return can_id;
}

static exception_t set_attr_can_id(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	x86_sja1000_dev *dev =  conf_obj->obj;
	dev->can_id = SKY_attr_uinteger(*value);
	return No_exp;
}

static void sja1000_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of can device");
	SKY_register_attribute(clss, "can_id", get_attr_can_id, NULL, set_attr_can_id, NULL, SKY_Attr_Optional, "uinteger", "id of can device");
	return;
}

static void can_sja1000_set_signal(conf_object_t *obj, int level)
{
	x86_sja1000_dev *dev = obj->obj;
	
	if (level == RAISE_SIGNAL)
		DEV_pic_raise_irq(dev->interrupt_number);
	else 
		DEV_pic_lower_irq(dev->interrupt_number);
}

static void can_recv_callback(conf_object_t *opaque){
	struct x86_sja1000_device *dev = opaque->obj;
	x86_sja1000_reg_t* regs = dev->regs;
	int nbytes = 8;
	
	if(CanFIFOCheckCanReadNum(dev->rec_fifo) == 0 && (dev->now_msg_out == 1)){
		return;
	}
	uint32_t id;
	can_msg_t *msg;
	if(dev->now_msg_out == 1){
		ReadCanFIFO(dev->rec_fifo, &(dev->now_msg), 1);
		dev->now_msg_out = 0;
		msg = &(dev->now_msg);	
		dev->regs->rx_fid = msg->dlc;
		dev->regs->rx_fid |= 0x80;//À©Õ¹Ö¡
		id = msg->id;
		id = id << 3;
		dev->regs->rx_id[0] = (id & 0xff000000) >> 24;
		dev->regs->rx_id[1] = (id & 0xff0000) >> 16;
		dev->regs->rx_id[2] = (id & 0xff00) >> 8;
		dev->regs->rx_id[3] =  id & 0xff;
		dev->regs->channel_id = msg->channel_id;
		memcpy(dev->regs->rx_data, msg->data, 8);
		dev->regs->sr |= 0x1;
		can_sja1000_set_signal(opaque, RAISE_SIGNAL);
	}else {
		if(regs->ier & PELI_IR_RI){
			can_sja1000_set_signal(opaque, RAISE_SIGNAL);
		}
	}
}

static conf_object_t* new_x86_sja1000(char* obj_name)
{
	x86_sja1000_dev* dev = skyeye_mm_zero(sizeof(x86_sja1000_dev));
	if(dev == NULL)
		printf("alloc memmory for obj in %x fail\n", obj_name);
	x86_sja1000_reg_t* regs =  skyeye_mm_zero(sizeof(x86_sja1000_reg_t));
	cpld_reg_t* cpld_regs = skyeye_mm_zero(sizeof(cpld_reg_t));
	if(cpld_regs== NULL)
		printf("alloc memmory for cpld_regs in %x fail\n", obj_name);
	dev->cpld_regs = cpld_regs;
	if(regs == NULL)
		printf("alloc memmory for regs in %x fail\n", obj_name);
	dev->obj = new_conf_object(obj_name, dev);
	sja1000Device->obj = dev->obj;
	dev->regs = regs;
	dev->regs->rcv_cnt = 0;

	regs->sr = 0x3c;
	regs->ier = 1;
	dev->rec_fifo = CreateCanFIFO(1000);
	dev->rec_fifo->name = obj_name;

	int id;
	//create_thread_scheduler((unsigned int)500000, Periodic_sched, can_recv_callback, dev->obj, &id);
	int clock_mode = SIM->get_param_enum(BXPN_CLOCK_SYNC)->get();
	bool is_realtime = (clock_mode == BX_CLOCK_SYNC_REALTIME) ||
      		(clock_mode == BX_CLOCK_SYNC_BOTH);
	bx_virt_timer.register_timer(dev->obj, can_recv_callback, 1000, 1, 1, is_realtime, "x86_sja1000");
	dev->now_msg_out = 1;
	return dev->obj;
}

static exception_t can_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	x86_sja1000_device *dev =  obj->obj;
	dev->can_linker = obj2;
	dev->can_linker_iface = (can_linker_intf *)SKY_get_iface(dev->can_linker, CAN_LINKER_INTF);
	return No_exp;

}
 
static exception_t can_linker_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	x86_sja1000_device *dev =  obj->obj;
	*obj2 = dev->can_linker;
	*port = NULL;
	return No_exp;
}

static exception_t reset_x86_sja1000(conf_object_t* opaque, const char* args)
{
	x86_sja1000_dev *dev = opaque->obj;

	return No_exp;        
}

static exception_t free_x86_sja1000(conf_object_t* conf_obj)
{
	x86_sja1000_dev *dev = conf_obj->obj;
	FreeCanFIFO(dev->rec_fifo);
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return No_exp;
}
char* x86_sja1000_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	x86_sja1000_dev *dev = conf_obj->obj;
	return regs_name[id];
}

uint32_t x86_sja1000_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	x86_sja1000_dev *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)&(dev->regs) + id;
	return *regs_value;
}


static char* x86_sja1000_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	x86_sja1000_dev *dev = conf_obj->obj;
	x86_sja1000_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t x86_sja1000_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	x86_sja1000_dev *dev = conf_obj->obj;
	x86_sja1000_reg_t* regs = dev->regs;
	return  17;
}

static uint32_t x86_sja1000_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	x86_sja1000_dev *dev = conf_obj->obj;
	x86_sja1000_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(x86_sja1000_reg_t) / 4;;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static int x86_sja1000_receive(conf_object_t *obj, can_msg_t *msg){
	x86_sja1000_device *dev = obj->obj;
	WriteCanFIFO(dev->rec_fifo, msg, 1);
	dev->regs->rcv_cnt ++;
	
	if (CanFIFOCheckCanReadNum(dev->rec_fifo) != 0){
		dev->regs->ir |= 1;
	}

	return 0;
}

extern "C" void init_x86_sja1000();
void init_x86_sja1000(){
	static skyeye_class_t class_data = {
		class_name : "x86_sja1000",
		class_desc : "x86_sja1000",
		new_instance : new_x86_sja1000,
		free_instance : free_x86_sja1000,
		reset_instance : reset_x86_sja1000,
		get_attr : NULL,
		set_attr : NULL
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
		/* Register io function to the object */
		static const memory_space_intf io_memory = {
			conf_obj:NULL,
			read : x86_sja1000_read,
			write : x86_sja1000_write,
		};
		SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
		static const skyeye_reg_intf reg_intf = {
			conf_obj:NULL,
			get_regvalue_by_id : x86_sja1000_get_regvalue_by_id,
			get_regname_by_id : x86_sja1000_get_regname_by_id,
			set_regvalue_by_id : x86_sja1000_set_regvalue_by_id,
			get_regid_by_name : x86_sja1000_get_regid_by_name,
			get_regnum : x86_sja1000_get_regnum,
		};
		SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
		static const can_dev_intf can_intf = {
			receive_msg:x86_sja1000_receive,
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
		(struct InterfaceDescription) {
			name : CAN_DEV_INTF,
			iface : &can_intf,
		},
		
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			name : CAN_LINKER_INTF,
			set : can_linker_set,
			get : can_linker_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	sja1000_register_attribute(clss);
}

#endif 


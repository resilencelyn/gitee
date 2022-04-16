/*
 * =====================================================================================
 *
 *       Filename:  zn122_atm.c
 *
 *    Description:  This file implements the LEON2 UART on-chip device.
 *
 *        Version:  1.0
 *        Created:  24/06/08 10:51:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_dev_intf.h>
#include <skyeye_attr.h>
#include <simics/base_types.h>
#include "skyeye_uart.h"

#include "atm_zn122.h"
typedef struct uPD98401{
	uint32_t gmr;
	uint32_t gsr;
	uint32_t imr;
	uint32_t rqu;
	uint32_t rqa;
	uint32_t addr;
	uint32_t ver;
	uint32_t swr;
	uint32_t cmr;
	uint32_t cmr_l;
	uint32_t cer;
	uint32_t cer_l;
	uint32_t data;
}uPD98401_t;

typedef struct zn122_atm_dev{
	conf_object_t* conf_obj;
	uPD98401_t upd_regs;
	int irq_number;
	char* regs_name;
	atm_msg_t recv;
	conf_object_t* atm_linker;
	atm_linker_intf *atm_linker_iface;
}zn122_atm_dev_t;

static const char* regs_name[] = {
	"GMR",
	"GSR",
	"IMR",
	"RQU",
	"RQA",
	"ADDR",
	"VER",
	"SWR",
	"CMR",
	"CMR_L",
	"CER",
	"CER_L",
};
#if 0
static exception_t atm_zn122_transmit(conf_object_t *opaque, void *buf, uint32_t nbytes){
	struct zn122_atm_dev_t *dev = opaque->obj;
	int count;
	if(dev->atm_linker_iface != NULL){
			can_msg_t msg;
			msg.ide = 1;
			msg.id = 0;
			msg.id |= ((dev->regs->tx_id[0]) & 0xff) << 24;
			msg.id |= ((dev->regs->tx_id[1]) & 0xff) << 16;
			msg.id |= ((dev->regs->tx_id[2]) & 0xff) << 8;
			msg.id |= ((dev->regs->tx_id[3]) & 0xff);
			msg.id = msg.id >> 3;
			msg.dlc = dev->regs->tx_fid & 0xf;
			memcpy(msg.data, buf, 8);
			dev->atm_linker_iface->send_msg(dev->atm_linker, opaque, &msg);
		}
	}else{
		printf("No can transfer bus\n");
		return Unknown_exp;

	}
	return No_exp;

}
#endif
static exception_t zn122_atm_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	zn122_atm_dev_t *dev = opaque->obj;
	
	switch(offset / 4){
		case uPD98401_GMR:
			dev->upd_regs.gmr = *buf;
			break;
		case uPD98401_GSR:
			dev->upd_regs.gsr = *buf;
			break;
		case uPD98401_IMR:
			dev->upd_regs.imr = *buf;
			break;
		case uPD98401_RQU:
			dev->upd_regs.rqu = *buf;
			break;
		case uPD98401_RQA:
			dev->upd_regs.rqa = *buf;
			break;
		case uPD98401_ADDR:
			dev->upd_regs.addr = *buf;
			break;
		case uPD98401_VER:
			dev->upd_regs.ver = *buf;
			break;	
		case uPD98401_SWR:
			dev->upd_regs.swr = *buf;
			break;
		case uPD98401_CMR:
			dev->upd_regs.cmr = *buf;
			//printf("write cmr to 0x%x in %s\n", dev->upd_regs.cmr, __FUNCTION__);
			if(dev->upd_regs.cmr & uPD98401_TX_READY){
				/* begin send the packet */
				uint32_t addr = dev->upd_regs.cmr & uPD98401_CHAN_ADDR;
				if(dev->atm_linker_iface != NULL){
					atm_msg_t msg;
					msg.desc[4] = dev->upd_regs.data;
					//printf("send data in %s\n",  __FUNCTION__);
					if(dev->atm_linker_iface)
					{
						if(dev->atm_linker_iface->send_msg)
							dev->atm_linker_iface->send_msg(dev->atm_linker, opaque, &msg);
					}
				}

			}
			break;
		case uPD98401_CMR_L:
			dev->upd_regs.cmr_l = *buf;
			break;
		case uPD98401_CER:
			dev->upd_regs.cer = *buf;
			break;
		case uPD98401_CER_L: 
			dev->upd_regs.cer_l = *buf;
			break;
		case uPD98401_DATA:
			dev->upd_regs.data = *buf;
			break;

		default:
			break;
	}
	return No_exp;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  zn122_atm_read
 *  Description:  This function performs the reading in the UART device.
 * =====================================================================================
 */
static exception_t zn122_atm_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	zn122_atm_dev_t *dev = opaque->obj;
	switch(offset / 4)
	{
		case uPD98401_GMR:
			*(uint32*)buf = dev->upd_regs.gmr;
			break;
		case uPD98401_GSR:
			*(uint32*)buf = dev->upd_regs.gsr;
			break;
		case uPD98401_IMR:
			*(uint32*)buf = dev->upd_regs.imr;
			break;
		case uPD98401_RQU:
			*(uint32*)buf = dev->upd_regs.rqu;
			break;
		case uPD98401_RQA:
			*(uint32*)buf = dev->upd_regs.rqa;
			break;
		case uPD98401_ADDR:
			*(uint32*)buf = dev->upd_regs.addr;
			break;
		case uPD98401_VER:
			*(uint32*)buf = dev->upd_regs.ver;
			break;	
		case uPD98401_SWR:
			*(uint32*)buf = dev->upd_regs.swr;
			break;
		case uPD98401_CMR:
			*(uint32*)buf = dev->upd_regs.cmr;
			break;
		case uPD98401_CMR_L:
			*(uint32*)buf = dev->upd_regs.cmr_l;
			break;
		case uPD98401_CER:
			*(uint32*)buf = dev->upd_regs.cer;
			break;
		case uPD98401_CER_L: 
			*(uint32*)buf = dev->upd_regs.cer_l;
			break;
		case uPD98401_DATA:
			*(uint32*)buf = dev->upd_regs.data;
			dev->upd_regs.gsr &= ~0x1; /* data is read, clear the bit */
			break;
		default: // the wrong offset
			break; 
	}

	return No_exp;
}

char* zn122_atm_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	//zn122_atm_dev_t *dev = conf_obj->obj;
	//return dev->regs_name[id];
	return regs_name[id];
}

uint32_t tsc695f_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	zn122_atm_dev_t *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)&(dev->upd_regs) + id;
	return *regs_value;
}


static char* zn122_atm_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	zn122_atm_dev_t *dev = conf_obj->obj;
	uPD98401_t* regs = &(dev->upd_regs);
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t zn122_atm_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	zn122_atm_dev_t *dev = conf_obj->obj;
	return  sizeof(dev->upd_regs)/sizeof(uint32);
}

static uint32_t zn122_atm_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	zn122_atm_dev_t *dev = conf_obj->obj;
	uPD98401_t* regs = &(dev->upd_regs);
	uint32_t regnum;
	regnum = sizeof(dev->upd_regs) / 4;;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static int atm_zn122_receive(conf_object_t *obj, atm_msg_t *msg){
	zn122_atm_dev_t *dev = obj->obj;
	dev->upd_regs.data = msg->desc[4];
	dev->upd_regs.gsr |= 0x1; /* data received */
	return 0;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  new_zn122_atm
 *  Description:  instance zn122_atm.
 *
 *  The function returns base class of point which point to the zn122_atm.
 * =====================================================================================
 */
static conf_object_t* new_zn122_atm(char* obj_name)
{
	zn122_atm_dev_t* zn122_atm = skyeye_mm_zero(sizeof(zn122_atm_dev_t));
	zn122_atm->regs_name = &regs_name;
	/*  Clear the UART register   */
	zn122_atm->conf_obj = new_conf_object(obj_name, zn122_atm);
	zn122_atm->upd_regs.gsr |= uPD98401_INT_IND;
	return zn122_atm->conf_obj;
}

static exception_t reset_zn122_atm(conf_object_t* opaque, const char* args)
{
	zn122_atm_dev_t *dev = opaque->obj;
	return No_exp;
}

static void free_zn122_atm(conf_object_t* conf_obj)
{
	zn122_atm_dev_t* dev = conf_obj->obj;
	skyeye_free(dev->conf_obj);
	skyeye_free(dev);
}

static exception_t atm_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	zn122_atm_dev_t *dev =  obj->obj;
	dev->atm_linker = obj2;
	dev->atm_linker_iface = (atm_linker_intf *)SKY_get_iface(dev->atm_linker, ATM_LINKER_INTF);
	//printf("in %s, linker_iface=0x%x\n", __FUNCTION__, dev->atm_linker_iface);
	return No_exp;

}
 
static exception_t atm_linker_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	zn122_atm_dev_t *dev =  obj->obj;
	*obj2 = dev->atm_linker;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_irq_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	zn122_atm_dev_t* dev = (zn122_atm_dev_t*)(conf_obj->obj);
	attr_value_t irq_number = SKY_make_attr_uinteger(dev->irq_number);
	return irq_number;
}
static exception_t set_attr_irq_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	zn122_atm_dev_t* dev = (zn122_atm_dev_t*)(conf_obj->obj);
	dev->irq_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void zn122_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "irq_number", get_attr_irq_number, NULL, set_attr_irq_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of zn122 ATM");
	return;
}

void init_zn122_atm(){
	static skyeye_class_t class_data = {
		.class_name = "zn122_atm",
		.class_desc = "zn122 ATM",
		.new_instance = new_zn122_atm,
		.reset_instance = reset_zn122_atm,
		.free_instance = free_zn122_atm,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = zn122_atm_read,
		.write = zn122_atm_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = tsc695f_get_regvalue_by_id,
		.get_regname_by_id = zn122_atm_get_regname_by_id,
		.set_regvalue_by_id = zn122_atm_set_regvalue_by_id,
		.get_regnum = zn122_atm_get_regnum,
		.get_regid_by_name = zn122_atm_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const atm_dev_intf atm_intf = {
		.receive_msg = atm_zn122_receive,
	};
	SKY_register_iface(clss, ATM_DEV_INTF, &atm_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME, 
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = ATM_DEV_INTF, 
			.iface = &atm_intf,
		},
	};

	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			.name = ATM_LINKER_INTF,
			.set = atm_linker_set,
			.get = atm_linker_get,
		},

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	zn122_register_attribute(clss);
	return;
}

/* Copyright (C) 
* 
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
* @file HDLC_Rs485.c
* @brief The implementation of system controller
* @author 
* @version 78.77
*/

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
#include <skyeye_core.h>
#include <skyeye_signal.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_uart.h>
#include <skyeye_emif.h>
#include <skyeye_dev_bus_intf.h>

#include "HDLC_Rs485.h"

#define Memory_Size 0x400
static exception_t HDLC_Rs485_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct HDLC_Rs485_device *dev = opaque->obj;
	HDLC_Rs485_reg_t* regs = dev->regs;
	switch(offset) {
	case 0x0:
		*(uint16_t *)buf = regs->recv_length_reg;
		break;
	case 0x1:
		*(uint16_t *)buf = regs->status_reg;
		break;
	case 0x2:
		*(uint16_t *)buf = regs->cmd_reg;
		break;
	case 0x3:
		*(uint16_t *)buf = regs->addr_reg;
		break;
	case 0x4:
		*(uint16_t *)buf = regs->send_length_reg;
		break;
	case 0x4000:
		*(uint16_t *)buf = regs->intr_status_reg;
		break;
	default:
		if (offset >= 0xc00 && offset <= 0x1000){
			if (dev->bus_iface != NULL){
				dev->bus_iface->read_data(dev->bus_obj, opaque, buf, 1);
			}else{
				printf("In %s, bus_iface Failed.\n", __FUNCTION__);
				return Error;
			}
		}else{
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in HDLC_Rs485\n", offset);
			return Invarg_exp;
		}
	}
	return No_exp;
}

static uint8_t CRC4(char* pData, int Length){

	uint8_t  tempResult;
	uint8_t tempRegs;
	uint8_t midval;
	int i, j;

	midval = 0x13;
	tempRegs = *pData++;
	tempRegs ^= 0xff;
	i = 0;

	while(i < Length)
	{
		for(j = 0; j < 8; j++)
		{
			if (tempRegs & 1)
			{
				tempRegs >>= 1;
				tempRegs ^= midval;
			}
			else
			{
				tempRegs >>= 1;
			}
		}

		if (i++ == (Length - 1)) return tempRegs;
		tempResult = *pData++;
		tempRegs ^= tempResult;
	}

	return 0;

}


uint16_t Check_sum(char *buf, int len)
{
   int i=0,Sum=0;
   for (i=0;i<len;i++)
   {
    Sum+=*buf++;
   }
   return Sum;
}

uint8_t Check_xor(char *buf, int len)
{
   unsigned char i=0,Sum=0;
   for (i=0;i<len;i++)
   {
    Sum ^=*buf++;
   }
   return Sum;
}



// --------------------------------------------------------------
//      CRC16???㷽??1:ʹ??2??256???ȵ?У????
// --------------------------------------------------------------
const char chCRCHTalbe[] =                                 // CRC ??λ?ֽ?ֵ??
{
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40
};
const char chCRCLTalbe[] =                                 // CRC ??λ?ֽ?ֵ??
{
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
0x41, 0x81, 0x80, 0x40
};

uint16_t CRC16(char* pchMsg, int wDataLen)
{
        char chCRCHi = 0xFF; // ??CRC?ֽڳ?ʼ??
        char chCRCLo = 0xFF; // ??CRC?ֽڳ?ʼ??
        int  wIndex;            // CRCѭ???е?????
        while (wDataLen--)
        {
                // ????CRC
                wIndex = chCRCLo ^ *pchMsg++ ;
                chCRCLo = chCRCHi ^ chCRCHTalbe[wIndex];
                chCRCHi = chCRCLTalbe[wIndex] ;
        }
        return ((chCRCHi << 8) | chCRCLo) ;
}

static int HDLC_Rs485_set_state(conf_object_t* obj, char *buf, int Length, int dev_id)
{
	HDLC_Rs485_device *dev = obj->obj;
	HDLC_Rs485_reg_t* regs = dev->regs;
	int crc_ret, crc_sum1, crc_sum2;
	int data_length;

	data_length = Length - 4;

	if (buf[0] == 0x7E && buf[Length - 1] == 0x7E){
	//CRC16 Check
		crc_ret = CRC16(&buf[1], data_length);
		crc_sum1 = buf[data_length + 1] & 0xff;
		crc_sum2 = buf[data_length + 2] & 0xff;
		if ((crc_ret >> 8 & 0xff) == crc_sum1 && ((crc_ret & 0xff) == crc_sum2)){
			// data Length more than recv_cache_length ?
			if (data_length > 0x200)   
				printf("In %s, recv %d byte data over the recv_cache_length.\n", __FUNCTION__,data_length);
			else{
				regs->status_reg |= 0x1;	
				regs->recv_length_reg = data_length;
			}
		}else {
			printf("In %s, frame CRC16 checksum not correct.\n", __FUNCTION__);
		}
	}else{
		printf("In %s, Recv error frame.\n", __FUNCTION__);	
	}
	regs->addr_reg = dev_id;
	return OK;
}
static exception_t HDLC_Rs485_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
//	skyeye_log(Error_log, __FUNCTION__, "offset :%x, buf :%x\n", offset, *(uint32_t *)buf);
	struct HDLC_Rs485_device *dev = opaque->obj;
	HDLC_Rs485_reg_t* regs = dev->regs;
	uint16_t val = *(uint16_t*)buf;
	switch(offset) {
	case 0x0:
		regs->recv_length_reg = val;
		break;
	case 0x1:
		regs->status_reg = val;
		break;
	case 0x2:
		regs->cmd_reg = val;
		if (regs->cmd_reg == 0xFF){
			if (dev->bus_iface != NULL){
				dev->bus_iface->write_data(dev->bus_obj, opaque, dev->stkptr, dev->data_length);
			}
		}
		break;
	case 0x3:
		regs->addr_reg = val;
		break;
	case 0x4:
		regs->send_length_reg = val;
		break;
	case 0x4000:
		regs->intr_status_reg = val;
		break;
	default:
		if (offset >= 0x800 && offset <= 0xc00){
			offset = offset - 0x800;
			memcpy(dev->stkptr + offset, buf, 1);
			dev->data_cnt += 1;
			if (dev->data_cnt == regs->send_length_reg){
				uint16_t crc_ret;
				crc_ret = CRC16(&dev->stkptr[1], dev->data_cnt);
				dev->stkptr[0] = 0x7E;
				dev->stkptr[dev->data_cnt + 1] = (crc_ret >> 8) & 0xff;
				dev->stkptr[dev->data_cnt + 2] = crc_ret & 0xff;
				dev->stkptr[dev->data_cnt + 3] = 0x7E;
				dev->data_length = dev->data_cnt + 4;
			}
		}else{
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in HDLC_Rs485\n", offset);
			return Invarg_exp;
		}
	}
	return No_exp;
}

static char* HDLC_Rs485_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t HDLC_Rs485_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	HDLC_Rs485_device *dev = conf_obj->obj;
	HDLC_Rs485_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static char* HDLC_Rs485_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	HDLC_Rs485_device *dev = conf_obj->obj;
	HDLC_Rs485_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t HDLC_Rs485_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	HDLC_Rs485_device *dev = conf_obj->obj;
	HDLC_Rs485_reg_t* regs = dev->regs;
	return  sizeof(HDLC_Rs485_reg_t)/1;

}

static uint32_t HDLC_Rs485_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	HDLC_Rs485_device *dev = conf_obj->obj;
	HDLC_Rs485_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(HDLC_Rs485_reg_t)/ 1;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;

}

static attr_value_t get_attr_dev_id(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	HDLC_Rs485_device *dev = conf_obj->obj;
	attr_value_t dev_id = SKY_make_attr_uinteger(dev->dev_id);
	return dev_id;
}

static exception_t set_attr_dev_id(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	HDLC_Rs485_device *dev = conf_obj->obj;
	dev->dev_id = SKY_attr_uinteger(*value);

	return No_exp;
}

static void HDLC_Rs485_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "dev_id", get_attr_dev_id, NULL, set_attr_dev_id, NULL, SKY_Attr_Optional, "uinteger", "device number ");
	return;
}
static conf_object_t* new_HDLC_Rs485(char* obj_name){
	HDLC_Rs485_device* dev = skyeye_mm_zero(sizeof(HDLC_Rs485_device));
	HDLC_Rs485_reg_t* regs =  skyeye_mm_zero(sizeof(HDLC_Rs485_reg_t));

	dev->obj = new_conf_object(obj_name, dev);
	dev->ptr = (char *)skyeye_mm_zero(Memory_Size);
	dev->send_buffer = (char *)skyeye_mm_zero(Memory_Size);
	/* init HDLC_Rs485 regs */
	dev->regs = regs;
	dev->stkptr = dev->ptr;
	dev->data_cnt = 0;
	dev->data_length = 0;

	return dev->obj;
}

static void free_HDLC_Rs485(conf_object_t* conf_obj){
	HDLC_Rs485_device* dev = conf_obj->obj;
	skyeye_free(dev->ptr);
	skyeye_free(dev->send_buffer);
	skyeye_free(dev->obj);
	skyeye_free(dev->regs);
	skyeye_free(dev);
	return;	
}

static exception_t HDLC_Rs485_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	HDLC_Rs485_device* dev = (HDLC_Rs485_device*)(obj->obj);
	dev->bus_obj = obj2;
	dev->bus_iface = (Rs485_bus_intf*)SKY_get_iface(dev->bus_obj, RS485_BUS_INTF);

	if (dev->bus_iface == NULL){
		if (dev->bus_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", RS485_BUS_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", RS485_BUS_INTF, obj2->objname);

		return Not_found_exp;
	}
	if (Error == dev->bus_iface->register_device(obj2, obj)){
		printf("In %s, Register HDLC_Rs485 device Error.\n", __FUNCTION__);
	}
	return No_exp;

}
 
static exception_t HDLC_Rs485_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	HDLC_Rs485_device* dev = (HDLC_Rs485_device*)(obj->obj);
	*obj2 = dev->bus_obj;
	*port = NULL;
	return No_exp;
}
void init_HDLC_Rs485(){
	static skyeye_class_t class_data = {
		.class_name = "Rs485_HDLC",
		.class_desc = "Rs485_HDLC",
		.new_instance = new_HDLC_Rs485,
		.free_instance = free_HDLC_Rs485,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = HDLC_Rs485_read,
		.write = HDLC_Rs485_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const Rs485_emif_intf Rs485_emif_interface = {
		.set_state = HDLC_Rs485_set_state,
	};
	SKY_register_iface(clss, RS485_EMIF_INTF, &Rs485_emif_interface);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = HDLC_Rs485_get_regvalue_by_id,
		.get_regname_by_id = HDLC_Rs485_get_regname_by_id,
		.set_regvalue_by_id = HDLC_Rs485_set_regvalue_by_id,
		.get_regnum = HDLC_Rs485_get_regnum,
		.get_regid_by_name = HDLC_Rs485_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = RS485_EMIF_INTF,
			.iface =  &Rs485_emif_interface,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME, 
			.iface = &io_memory,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = RS485_BUS_INTF,
			.set = HDLC_Rs485_set,
			.get = HDLC_Rs485_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	HDLC_Rs485_register_attribute(clss);
}

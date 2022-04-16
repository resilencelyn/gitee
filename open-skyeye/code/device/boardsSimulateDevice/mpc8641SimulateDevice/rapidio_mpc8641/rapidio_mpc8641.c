/*
 * =====================================================================================
 *
 *       Filename:  leon2_uart.c
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
#include <skyeye_attr.h>
#include <simics/base_types.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>
#include "skyeye_uart.h"

#include "rapidio_mpc8641.h"


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  mpc8641_rapidio_write
 *  Description:  This function perform the writing in the UART device
 * =====================================================================================
 */

static exception_t mpc8641_rapidio_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	mpc8641_rapidio_dev_t *dev = opaque->obj;
	switch(offset)
	{
		case ARCH_DIDCAR:
			dev->arch_regs.didcar = *buf;
			break;
		case ARCH_DICAR:
			dev->arch_regs.dicar = *buf;
			break;
		case ARCH_AIDCAR:
			dev->arch_regs.aidcar = *buf;
			break;
		case ARCH_AICAR:
			dev->arch_regs.aicar = *buf;
			break;
		case ARCH_PEFCAR:
			dev->arch_regs.pefcar = *buf;
			break;
		case ARCH_SOCAR:
			dev->arch_regs.socar = *buf;
			break;
		case ARCH_DOCAR:
			dev->arch_regs.docar = *buf;
			break;
		case ARCH_MCSR:
			dev->arch_regs.mcsr = *buf;
			break;
		case ARCH_PWDCSR:
			dev->arch_regs.pwdcsr = *buf;
			break;
		case ARCH_PELLCCSR:
			dev->arch_regs.pellccsr = *buf;
			break;
		case ARCH_LCSBA1CSR:
			dev->arch_regs.lcsba1csr = *buf;
			break;
		case ARCH_BDIDCSR:
			dev->arch_regs.bdidcsr = *buf;
			break;
		case ARCH_HBDIDLCSR:
			dev->arch_regs.hbdidlcsr = *buf;
			break;
		case ARCH_CTCSR:
			dev->arch_regs.ctcsr = *buf;
			break;
		case MU_OM0SR:
			dev->arch_regs.om0sr = *buf;
			break;
		case MU_IM0SR:
			dev->arch_regs.im0sr = *buf;
			if(0 == (dev->arch_regs.im0sr & 0x4)){
				if(dev->tsi578_linker_iface != NULL){
					tsi578_msg_t msg;
					msg.data[4] = dev->arch_regs.msg_data;
					dev->tsi578_linker_iface->send_msg(dev->tsi578_linker, opaque, &msg);
					dev->arch_regs.im0sr = 0x4;
				}
			}
			break;
		case MSG_DATA:
			dev->arch_regs.msg_data = *buf;
			break;
		default:
			break;
	}
	return No_exp;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  mpc8641_rapidio_read
 *  Description:  This function performs the reading in the UART device.
 * =====================================================================================
 */
static exception_t mpc8641_rapidio_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	mpc8641_rapidio_dev_t *dev = opaque->obj;
	switch(offset)
	{
		case ARCH_DIDCAR:
			*(uint32*)buf = dev->arch_regs.didcar;
			break;
		case ARCH_DICAR:
			*(uint32*)buf = dev->arch_regs.dicar;
			break;
		case ARCH_AIDCAR:
			*(uint32*)buf = dev->arch_regs.aidcar;
			break;
		case ARCH_AICAR:
			*(uint32*)buf = dev->arch_regs.aicar;
			break;
		case ARCH_PEFCAR:
			*(uint32*)buf = dev->arch_regs.pefcar;
			break;
		case ARCH_SOCAR:
			*(uint32*)buf = dev->arch_regs.socar;
			break;
		case ARCH_DOCAR:
			*(uint32*)buf = dev->arch_regs.docar;
			break;
		case ARCH_MCSR:
			*(uint32*)buf = dev->arch_regs.mcsr;
			break;
		case ARCH_PWDCSR:
			*(uint32*)buf = dev->arch_regs.pwdcsr;
			break;
		case ARCH_PELLCCSR:
			*(uint32*)buf = dev->arch_regs.pellccsr;
			break;
		case ARCH_LCSBA1CSR:
			*(uint32*)buf = dev->arch_regs.lcsba1csr;
			break;
		case ARCH_BDIDCSR:
			*(uint32*)buf = dev->arch_regs.bdidcsr;
			break;
		case ARCH_HBDIDLCSR:
			*(uint32*)buf = dev->arch_regs.hbdidlcsr;
			break;
		case ARCH_CTCSR:
			*(uint32*)buf = dev->arch_regs.ctcsr;
			break;
		case MU_OM0SR:
			*(uint32*)buf = dev->arch_regs.om0sr;
			break;
		case MU_IM0SR:
			dev->arch_regs.im0sr = 0x0;
			*(uint32*)buf = dev->arch_regs.im0sr;
			break;
		case MSG_DATA:
			*(uint32*)buf = dev->arch_regs.msg_data;
			break;
		default:
			break;
	}

	return No_exp;
}

static exception_t serial_write(conf_object_t *obj, uint32_t value)
{
	mpc8641_rapidio_dev_t *dev = obj->obj;

	return No_exp;
}

char* mpc8641_rapidio_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	//mpc8641_rapidio_dev_t *dev = conf_obj->obj;
	//return dev->arch_regs_name[id];
	return arch_regs_name[id];
}

uint32_t mpc8641_rapidio_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	mpc8641_rapidio_dev_t *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)&(dev->arch_regs) + id;
	return *regs_value;
}


static char* mpc8641_rapidio_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	mpc8641_rapidio_dev_t *dev = conf_obj->obj;
	arch_reg_t* regs = &(dev->arch_regs);
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t mpc8641_rapidio_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	mpc8641_rapidio_dev_t *dev = conf_obj->obj;
	return  sizeof(dev->arch_regs)/sizeof(uint32);
}

static uint32_t mpc8641_rapidio_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	mpc8641_rapidio_dev_t *dev = conf_obj->obj;
	arch_reg_t* regs = &(dev->arch_regs);
	uint32_t regnum;
	regnum = sizeof(dev->arch_regs) / 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, arch_regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

/* receive message */
static int tsi578_rapidio_receive(conf_object_t *obj, tsi578_msg_t *msg){
	mpc8641_rapidio_dev_t *dev = obj->obj;
	dev->arch_regs.msg_data = msg->data[4];
	dev->arch_regs.im0sr = 0x0;
	return 0;
}

static exception_t tsi578_linker_set(conf_object_t *obj, conf_object_t *obj2, tsi578_msg_t *msg){
	mpc8641_rapidio_dev_t *dev = obj->obj;
	dev->tsi578_linker = obj2;
	dev->tsi578_linker_iface = (tsi578_linker_intf *)SKY_get_iface(dev->tsi578_linker, TSI578_LINKER_INTF);

	if (dev->tsi578_linker_iface == NULL){
		if (dev->tsi578_linker == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", TSI578_LINKER_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", TSI578_LINKER_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t tsi578_linker_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	mpc8641_rapidio_dev_t *dev = obj->obj;
	*obj2 = dev->tsi578_linker;
	*port = NULL;
	return No_exp;
}
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  new_mpc8641_rapidio
 *  Description:  instance mpc8641_rapidio.
 *
 *  The function returns base class of point which point to the mpc8641_rapidio.
 * =====================================================================================
 */
static conf_object_t* new_mpc8641_rapidio(char* obj_name)
{
	mpc8641_rapidio_dev_t* mpc8641_rapidio = skyeye_mm_zero(sizeof(mpc8641_rapidio_dev_t));

	mpc8641_rapidio->arch_regs.didcar	= rPAIDIO_ARCH_DIDCAR;
	mpc8641_rapidio->arch_regs.dicar	= rPAIDIO_ARCH_DICAR;
	mpc8641_rapidio->arch_regs.aidcar	= rPAIDIO_ARCH_AIDCAR;
	mpc8641_rapidio->arch_regs.aicar	= rPAIDIO_ARCH_AICAR;
	mpc8641_rapidio->arch_regs.pefcar	= rPAIDIO_ARCH_PEFCAR;
	mpc8641_rapidio->arch_regs.socar	= rPAIDIO_ARCH_SOCAR;
	mpc8641_rapidio->arch_regs.docar	= rPAIDIO_ARCH_DOCAR;
	mpc8641_rapidio->arch_regs.mcsr		= rPAIDIO_ARCH_MCSR;
	mpc8641_rapidio->arch_regs.pwdcsr	= rPAIDIO_ARCH_PWDCSR;
	mpc8641_rapidio->arch_regs.pellccsr	= rPAIDIO_ARCH_PELLCCSR;
	mpc8641_rapidio->arch_regs.lcsba1csr= rPAIDIO_ARCH_LCSBA1CSR;
	mpc8641_rapidio->arch_regs.bdidcsr	= rPAIDIO_ARCH_BDIDCSR;
	mpc8641_rapidio->arch_regs.hbdidlcsr= rPAIDIO_ARCH_HBDIDLCSR;
	mpc8641_rapidio->arch_regs.ctcsr	= rPAIDIO_ARCH_CTCSR;
	mpc8641_rapidio->arch_regs.om0sr	= rPAIDIO_MU_OM0SR;
	mpc8641_rapidio->arch_regs.im0sr	= rPAIDIO_MU_IM0SR;

	mpc8641_rapidio->arch_regs_name = &arch_regs_name;
	mpc8641_rapidio->conf_obj = new_conf_object(obj_name, mpc8641_rapidio);

	return mpc8641_rapidio->conf_obj;
}

static exception_t reset_mpc8641_rapidio(conf_object_t* opaque, const char* args)
{
	mpc8641_rapidio_dev_t *dev = opaque->obj;

	return No_exp;
}

static void free_mpc8641_rapidio(conf_object_t* conf_obj)
{
	mpc8641_rapidio_dev_t* dev = conf_obj->obj;
	skyeye_free(dev->conf_obj);
	skyeye_free(dev);
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	mpc8641_rapidio_dev_t* dev = (mpc8641_rapidio_dev_t*)(conf_obj->obj);
	attr_value_t int_number = SKY_make_attr_uinteger(dev->irq_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	mpc8641_rapidio_dev_t* dev = (mpc8641_rapidio_dev_t*)(conf_obj->obj);
	dev->irq_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void mpc8641_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "irq_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of mpc8641 rapidio");
	return;
}


void init_mpc8641_rapidio(){
	static skyeye_class_t class_data = {
		.class_name = "mpc8641_rapidio",
		.class_desc = "mpc8641_rapidio",
		.new_instance = new_mpc8641_rapidio,
		.reset_instance = reset_mpc8641_rapidio,
		.free_instance = free_mpc8641_rapidio,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_mpc8641d_mach),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const skyeye_serial_device_t serial_device = {
		.write = serial_write,
		.receive_ready = NULL,
	};
	SKY_register_iface(clss, SERIAL_DEVICE_INTERFACE, &serial_device);

	static const memory_space_intf io_memory = {
		.read = mpc8641_rapidio_read,
		.write = mpc8641_rapidio_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const tsi578_dev_intf tsi578_intf = {
		.receive_msg = tsi578_rapidio_receive,
	};
	SKY_register_iface(clss, TSI578_DEV_INTF, &tsi578_intf);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = mpc8641_rapidio_get_regvalue_by_id,
		.get_regname_by_id = mpc8641_rapidio_get_regname_by_id,
		.set_regvalue_by_id = mpc8641_rapidio_set_regvalue_by_id,
		.get_regnum = mpc8641_rapidio_get_regnum,
		.get_regid_by_name = mpc8641_rapidio_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SERIAL_DEVICE_INTERFACE,
			.iface = &serial_device,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME, 
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = TSI578_DEV_INTF, 
			.iface = &tsi578_intf,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			.name = TSI578_LINKER_INTF,
			.set = tsi578_linker_set,
			.get = tsi578_linker_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	mpc8641_register_attribute(clss);
	return;
}

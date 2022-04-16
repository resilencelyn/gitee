#include <skyeye_lock.h>
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
#include <skyeye_dev_intf.h>
#include <skyeye_net_intf.h>
#include "qportal_fd.h"
#include <string.h>
#include <stdlib.h>
#define DEBUG
#include <skyeye_log.h>
#include "QMan_T2080.h"


#define SET_BIT(reg, mask) ((reg) |= (mask))
#define CLR_BIT(reg, mask) ((reg) &= ~(mask))
#define CHK_BIT(reg, mask) ((reg) & (mask))

static exception_t QMan_T2080_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	QMan_T2080_device *dev = opaque->obj;
	uint32_t val = dev->regs[offset];
	*(uint32_t *)buf = val;

	// printf("%s  offset: 0x%x val: 0x%x\n", __func__, offset, *(uint32_t *)buf);
	return 0;
}

static exception_t QMan_T2080_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	QMan_T2080_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	dev->regs[offset] = val;
	switch (offset)
	{
		case 0xB00: { // QMAN_MCR—QMan Management Command/Result Register
			if ((val > 24) == 0x01) {
				dev->regs[offset] = (val & 0xFFFFFF) | 0xF0000000;
			}
		}
	}

	// printf("%s offset: 0x%x val: 0x%x\n", __func__, offset, val);

	return 0;
}

static int register_read(conf_object_t *opaque, generic_address_t offset, uint32_t *value, size_t count)
{
	QMan_T2080_device *dev = opaque->obj;

	switch (count) {
	case 1:
	{
		uint8_t *this_regs = &(dev->sp_regs[offset]);
		uint8_t data;
		data = *this_regs;
		*value = data;
		break;
	}
	case 2:
	{
		uint16_t *this_regs = &(dev->sp_regs[offset]);
		uint16_t data;
		data = *this_regs;
		*value = data;
		break;
	}
	case 4:
	{
		uint32_t *this_regs = &(dev->sp_regs[offset]);
		*value = *this_regs;
		break;
	}
	default:
		printf("read error\n");
		return 0;
	}

	return 1;
}

static int register_write(conf_object_t *opaque, generic_address_t offset, uint32_t value, size_t count)
{
	QMan_T2080_device *dev = opaque->obj;

	switch (count) {
	case 1:
	{
		uint8_t *this_regs = &(dev->sp_regs[offset]);
		*this_regs = value & 0xFF;
		break;
	}
	case 2:
	{
		uint16_t *this_regs = &(dev->sp_regs[offset]);
		*this_regs = value & 0xFFFF;
		break;
	}
	case 4:
	{
		uint32_t *this_regs = &(dev->sp_regs[offset]);
		*this_regs = value;
		break;
	}
	default:
		printf("write error\n");
		return 0;
	}

	return 1;
}

static exception_t QMan_T2080_0xFA_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	QMan_T2080_device *dev = opaque->obj;

	uint32_t val;
	register_read(opaque, offset, &val, count);
	
	
	if (offset ==0x1000004){//QPORTAL_CI_EQ_CONSIDX
		int i;
		uint32_t CI = dev->sp_regs[0x1000004] & 0x7;
		if(CI == 0)
			register_write(opaque, offset, 7, 1);
		else{
			CI-= 1;
			register_write(opaque, offset, CI, 1);
		}

	}
	if (offset >= 0x1000 && offset <= 0x13C0){//DQRR
		*(uint32_t *)buf = val;

	}
	
	*(uint32_t *)buf = val;

	// printf("%s offset: 0x%06x val: 0x%x\n", __func__, offset, val);

}

static exception_t QMan_T2080_0xFA_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	QMan_T2080_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

	if (offset >= 0x0 && offset <= 0x0200){//EQCR
		// printf("%s offset: 0x%06x val: 0x%x\n", __func__, offset, val);
		uint32_t value = 0;
		dev->sub_index  = offset / sizeof(struct qportal_enqueue);
		
		register_write(opaque, offset, val, count);

		uint32_t verb_addr = dev->sub_index*0x40;
		uint32_t verb;
		register_read(opaque, verb_addr, &verb, 1);
		register_read(opaque, verb_addr + 0x1c, &(dev->FqId), 1);

		/*enqueue*/
		if ((verb & 0x1) == 0x1 && dev->FqId != 0){
			uint32_t isr_dqri;
			register_read(opaque,0x1000E00,&value,4);
			register_write(opaque, 0x1000048, 0x0000000F, 4);
			register_read(opaque, 0x1000E00, &isr_dqri, 4);
			SET_BIT(isr_dqri, 0x20000);
			register_write(opaque, 0x1000E00, isr_dqri, 4);
			dev->QPORTAL_ENQUEUE[dev->sub_index].qportalFd.qportalCmd = 0;
			/*dequeue and trigger interrupt*/
			if (dev->queue16_Pin > 0xf) {
				dev->queue16_Pin = 0;
				dev->qportalDqVbit ^= 0x80;
			}

			/*padding dequeue*/
			dev->QPORTAL_DEQUEUE[dev->queue16_Pin].qportalVerb = dev->qportalDqVbit;
			dev->QPORTAL_DEQUEUE[dev->queue16_Pin].qportalFqId = dev->FqId;
			dev->QPORTAL_DEQUEUE[dev->queue16_Pin].qportalCntxB = 0x1;

			memcpy(&(dev->QPORTAL_DEQUEUE[dev->queue16_Pin].qportalFd), &(dev->QPORTAL_ENQUEUE[dev->sub_index].qportalFd), sizeof(QPORTAL_FDESC));
			dev->QPORTAL_DEQUEUE[dev->queue16_Pin].qportalFd.qportalCmd = 0;
			dev->queue16_Pin ++;
			
			if (dev->signal.intf) {
				dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.tx_num);
			}
			verb = 0;
		}
		
	}

	else if(offset == 0x003800) //QCSP0_CR
	{
		int value;
		register_write(opaque, 0x003940, val, 1);
		register_write(opaque, 0x003941, 0xf0, 1);
		
		register_write(opaque, 0x003900, val, 1);
		register_write(opaque, 0x003901, 0xf0, 1);
	}
	else if (offset == 0x1000E00){//QCSP0_ISR
		uint32_t value;
		register_read(opaque,0x1000E00,&value,4);
		value &= ~val;
		register_write(opaque, 0x1000E00, value, 4);

	}

	else 
		register_write(opaque, offset, val, count);

		
	
}

static char* QMan_T2080_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return 0;
}
static exception_t QMan_T2080_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	QMan_T2080_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t QMan_T2080_get_regnum(conf_object_t *conf_obj)
{
	
	return 0;
}
static uint32_t QMan_T2080_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	QMan_T2080_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t QMan_T2080_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	
	return 0;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	QMan_T2080_device *dev =  obj->obj;
	dev->signal.obj = obj2;
	dev->signal.intf = (general_signal_intf *)SKY_get_iface(dev->signal.obj, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal.intf == NULL){
		if (dev->signal.obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	QMan_T2080_device *dev =  obj->obj;
	*obj2 = dev->signal.obj;
	*port = NULL;
	return No_exp;
}
// tx,rx,err interrupt number attribute
static attr_value_t get_attr_tx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	QMan_T2080_device *dev =  conf_obj->obj;
	attr_value_t tx_int_number = SKY_make_attr_uinteger(dev->signal.tx_num);
	return tx_int_number;
}

static exception_t set_attr_tx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	QMan_T2080_device *dev =  conf_obj->obj;
	dev->signal.tx_num = SKY_attr_uinteger(*value);
	return No_exp;
}
static attr_value_t get_attr_rx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	QMan_T2080_device *dev =  conf_obj->obj;
	attr_value_t rx_int_number = SKY_make_attr_uinteger(dev->signal.rx_num);
	return rx_int_number;
}

static exception_t set_attr_rx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	QMan_T2080_device *dev =  conf_obj->obj;
	dev->signal.rx_num = SKY_attr_uinteger(*value);
	return No_exp;
}

static void class_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "tx_int_number", get_attr_tx_int_number, NULL, set_attr_tx_int_number, NULL, SKY_Attr_Optional, "uinteger", "tx interrupt number of MEMAC");
	SKY_register_attribute(clss, "rx_int_number", get_attr_rx_int_number, NULL, set_attr_rx_int_number, NULL, SKY_Attr_Optional, "uinteger", "rx interrupt number of MEMAC");
	return;
 }
static conf_object_t* new_QMan_T2080(const char *obj_name)
{
	QMan_T2080_device* dev = skyeye_mm_zero(sizeof(QMan_T2080_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->regs[0xbf8] = 0x0A010301;
	dev->regs[0xBFC] = 0x00000002;
	dev->regs[0xD04] = 0x0000003C;
	dev->regs[0xE00] = 0x00800000;
	dev->regs[0x504] = 0x00000001;
	dev->regs[0x908] = 0x00000001;
	dev ->sp_regs[0x1000040] = 0x00000010;
	dev ->sp_regs[0x1000080] = 0x00000008;
	dev ->sp_regs[0x1000000] = 0x00000008;
	dev ->sp_regs[0x1000004] = 0x00008808;
	dev ->sp_regs[0x0003000] = 0x00000008;
	dev ->sp_regs[0x0003100] = 0x00008808;
	dev ->sp_regs[0x0003200] = 0x00000010;
	dev ->sp_regs[0x0003400] = 0x00000008;

	dev->QPORTAL_ENQUEUE = (struct qportal_enqueue *)&dev->sp_regs[0];
	dev->QPORTAL_DEQUEUE = (struct qportal_dequeue *)&dev->sp_regs[0x1000];
	dev->qportalDqVbit = 0x80;

	return dev->obj;
}

static exception_t config_QMan_T2080(conf_object_t* conf_obj)
{
	QMan_T2080_device *dev = conf_obj->obj;

	static memory_space_intf io_memory =
	{
		.read  = QMan_T2080_0xFA_read,
		.write = QMan_T2080_0xFA_write
	};

	if (dev->image != NULL) {
	exception_t ret;
	ret = SKY_add_map(dev->image->obj, conf_obj, 0xFA000000, 0x2000000, 0, &io_memory, 1, 1);
	}else {
		skyeye_log(Error_log, __FUNCTION__, "Cannot get dev->image in QMan_T2080\n");
	}
	
	return No_exp;
}

static exception_t free_QMan_T2080(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	QMan_T2080_device *dev = obj->obj;
	dev->image = obj2;
	dev->image_space = (memory_space_intf *)SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);

	if (dev->image_space == NULL){
		if (dev->image == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", MEMORY_SPACE_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", MEMORY_SPACE_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
 
static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	QMan_T2080_device *dev = obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}

static QPORTAL_FDESC* init_fd_QMan_T2080(conf_object_t *obj, struct qportal_enqueue **en, struct qportal_dequeue **de)
{
	QMan_T2080_device *dev = obj->obj;

	*en = (struct qportal_enqueue *)&dev->sp_regs[0];
	*de = (struct qportal_dequeue *)&dev->sp_regs[0x1000];
}

static QPORTAL_FDESC* get_fd_QMan_T2080(conf_object_t *obj)
{
	QMan_T2080_device *dev = obj->obj;

	return 0;
}

static int set_fd_QMan_T2080(conf_object_t *obj,int size,uint32_t Lo_addr)
{
	QMan_T2080_device *dev = obj->obj;

	/*into dequeue*/
	if (dev->queue16_Pin > 0xf) {
		dev->queue16_Pin = 0;
		dev->qportalDqVbit ^= 0x80;
	}

	//padding DQRR
	dev->QPORTAL_DEQUEUE[dev->queue16_Pin].qportalVerb = dev->qportalDqVbit;
	dev->QPORTAL_DEQUEUE[dev->queue16_Pin].qportalFqId = dev->FqId - 0x20;
	dev->QPORTAL_DEQUEUE[dev->queue16_Pin].qportalCntxB = 0;
	dev->QPORTAL_DEQUEUE[dev->queue16_Pin].qportalFd.qportalAddrLo = Lo_addr;
	dev->QPORTAL_DEQUEUE[dev->queue16_Pin].qportalFd.qportalOffLen = size;

	dev->queue16_Pin ++;

	// ISR_DQRI
	uint32_t isr_dqri;
	register_write(obj, 0x1000048, 0x0000000F, 4);
	register_read(obj, 0x1000E00, &isr_dqri, 4);
	SET_BIT(isr_dqri, 0x20000);
	register_write(obj, 0x1000E00, isr_dqri, 4);
	
	//interrupt
	if (dev->signal.intf) {
			dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.rx_num);
		}

	return 0;
}


void init_QMan_T2080(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "QMan_T2080",
		.class_desc = "QMan_T2080",
		.new_instance = new_QMan_T2080,
		.free_instance = free_QMan_T2080,
		.config_instance = config_QMan_T2080,
		.module_type = SKYML_ONCHIP(SC_t2080),

	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = QMan_T2080_read,
		.write = QMan_T2080_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static qportal_fd_intf qportal_fdesc =
	{
		.get_fd  = get_fd_QMan_T2080,
		.set_fd  = set_fd_QMan_T2080,
		.init_fd = init_fd_QMan_T2080,
	};
	SKY_register_iface(clss, QPORTAL_FDESC_INTF_NAME, &qportal_fdesc);
	
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = QMan_T2080_get_regvalue_by_id,
		.get_regname_by_id = QMan_T2080_get_regname_by_id,
		.set_regvalue_by_id = QMan_T2080_set_regvalue_by_id,
		.get_regnum = QMan_T2080_get_regnum,
		.get_regid_by_name = QMan_T2080_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

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
		(struct InterfaceDescription)
		{
			.name = QPORTAL_FDESC_INTF_NAME,
			.iface = &qportal_fdesc
		},
	};
	static const struct ConnectDescription connects[] = 
	{
		(struct ConnectDescription ){
			.name = MEMORY_SPACE_INTF_NAME,
			.set = memory_set,
			.get = memory_get,
		},
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = signal_set,
			.get = signal_get,
		}
		
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	class_register_attribute(clss);
}

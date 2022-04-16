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
#define DEBUG
#include <skyeye_log.h>
#include "iic_c6713.h"

static void check_transmit_stop_condition(conf_object_t* opaque)
{
	iic_c6713_device *dev = opaque->obj;
	dev->data_count -= 1;
	if(dev->data_count == 0){

		if(dev->regs.i2cmdr & (1 << 11)){
			dev->regs.i2cstr &= ~(0x1010); //set bb and ICXRDY to 0;
			dev->regs.i2cmdr &= ~(1 << 11); //set STP to 0; 

			if(dev->i2c_bus_iface != NULL){
				dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
			}
		}else{
			dev->regs.i2cstr &= (~0x1010); //set bb and ICXRDY to 0;
			dev->regs.i2cisrc = I2C_INTCODE_ARDY;
		}
	}
	//dev->regs.i2cstr &= ~(1 << 12);//set bb to 0   Bus free
}
static void check_receive_stop_condition(conf_object_t *opaque)
{
	iic_c6713_device *dev = opaque->obj;
	dev->data_count -= 1;
	if(dev->data_count == 0){
		if(dev->regs.i2cmdr & (1 << 11)){
			if(dev->i2c_bus_iface != NULL){
				dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
			}
			dev->regs.i2cstr &= ~(0x1008);
			dev->regs.i2cmdr &= ~(1 << 11);
		}
	}
}
static exception_t iic_c6713_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	iic_c6713_device *dev = opaque->obj;
	switch (offset)
	{
		case I2COAR:
			*(uint32_t *)buf = dev->regs.i2coar;
			break;
		case I2CIER:
			*(uint32_t *)buf = dev->regs.i2cier;
			break;
		case I2CSTR:
			*(uint32_t *)buf = dev->regs.i2cstr;
			break;
		case I2CCLKL:
			*(uint32_t *)buf = dev->regs.i2cclkl;
			break;
		case I2CCLKH:
			*(uint32_t *)buf = dev->regs.i2cclkh;
			break;
		case I2CCNT:
			*(uint32_t *)buf = dev->regs.i2ccnt;
			break;
		case I2CDRR:
			if(dev->i2c_bus_iface != NULL){
				*(uint32_t *)buf = dev->i2c_bus_iface->read_data(dev->i2c_bus_obj);
			}
			check_receive_stop_condition(opaque);
			break;
		case I2CSAR:
			*(uint32_t *)buf = dev->regs.i2csar;
			break;
		case I2CDXR:
			*(uint32_t *)buf = dev->regs.i2cdxr;
			break;
		case I2CMDR:
			*(uint32_t *)buf = dev->regs.i2cmdr;
			break;
		case I2CISRC:
			*(uint32_t *)buf = dev->regs.i2cisrc;
			break;
		case I2CPSC:
			*(uint32_t *)buf = dev->regs.i2cpsc;
			break;
		case I2CPID1:
			*(uint32_t *)buf = dev->regs.i2cpid1;
			break;
		case I2CPID2:
			*(uint32_t *)buf = dev->regs.i2cpid2;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in iic_c6713\n", offset);
			break;
	}
	return 0;
}
static exception_t iic_c6713_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	iic_c6713_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case I2COAR:
			dev->regs.i2coar = val;
			if(dev->regs.i2cmdr & (1 << 8)){
				dev->own_slave_addr = val & 0x7f;
			}else{
				dev->own_slave_addr = val & 0x3ff;
			}
			break;
		case I2CIER:
			dev->regs.i2cier = val;
			break;
		case I2CSTR:
			dev->regs.i2cstr = val;
			break;
		case I2CCLKL:
			dev->regs.i2cclkl = val;
			break;
		case I2CCLKH:
			dev->regs.i2cclkh = val;
			break;
		case I2CCNT:
			dev->regs.i2ccnt = val;
			if(dev->regs.i2cmdr & (1 << 7) == 0){
				dev->data_count = val & 0xffff;
			}
			break;
		case I2CDRR:
			dev->regs.i2cdrr = val;
			break;
		case I2CSAR:
			dev->regs.i2csar = val;
			if(dev->regs.i2cmdr & (1 << 8) == 0){
				dev->slave_addr = val & 0x7f;
			}else{
				dev->slave_addr = val & 0x3ff;
			}
			break;
		case I2CDXR:
			dev->regs.i2cdxr = val;
			if(dev->i2c_bus_iface != NULL){
				dev->i2c_bus_iface->write_data(dev->i2c_bus_obj, val & 0xff);
			}
			check_transmit_stop_condition(opaque);
			break;
		case I2CMDR:
			dev->regs.i2cmdr = val;
			if(dev->regs.i2cmdr & (1 << 3)){
				dev->regs.i2cstr |= (1 << 9);//AAS set to 1
			}
			if(dev->regs.i2cmdr & (1 << 5)){
				if(dev->regs.i2cmdr & (1 << 10)){
					//master mode 
					if(dev->regs.i2cmdr & (1 << 13)){
						if(dev->i2c_bus_iface != NULL){
							dev->i2c_bus_iface->start(dev->i2c_bus_obj, dev->slave_addr & (~0x1));
						}
						if(dev->regs.i2cmdr & (1 << 9)){
							//transmit data
							dev->regs.i2cstr |= (1 << 4);//set ICXRDY
							if(dev->regs.i2cier & (1 << 4)){
								dev->regs.i2cisrc = I2C_INTCODE_ICXRDY;
							}
							check_transmit_stop_condition(opaque);
						}else{
							//receive data
							dev->regs.i2cstr |= (1 << 3); //set ICRRDY
							if(dev->regs.i2cstr & (1 << 3)){
								dev->regs.i2cisrc = I2C_INTCODE_ICRRDY;
							}
							check_receive_stop_condition(opaque);
						}
						dev->regs.i2cmdr &= ~(1 << 13);
					}
				}else{
					//slave mode
					if(dev->regs.i2cmdr & (1 << 13)){
						if(dev->i2c_bus_iface != NULL){
							dev->regs.i2cdrr = dev->i2c_bus_iface->read_data(dev->i2c_bus_obj);
						}
						dev->regs.i2cstr |= (1 << 12);//set bb to 1   Bus is busy
					}
				}
			}
			break;
		case I2CISRC:
			dev->regs.i2cisrc = val;
			break;
		case I2CPSC:
			dev->regs.i2cpsc = val;
			break;
		case I2CPID1:
			dev->regs.i2cpid1 = val;
			break;
		case I2CPID2:
			dev->regs.i2cpid2 = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in iic_c6713\n", offset);
			break;
	}
	return 0;
}
static char* iic_c6713_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t iic_c6713_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	iic_c6713_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t iic_c6713_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t iic_c6713_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	iic_c6713_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t iic_c6713_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static exception_t set_attr_i2c_bus(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	iic_c6713_device *dev = obj->obj;
	dev->i2c_bus_obj = obj2;
	dev->i2c_bus_iface = (i2c_bus_intf *)SKY_get_iface(dev->i2c_bus_obj, I2C_BUS_INTF_NAME);
	if (dev->i2c_bus_iface == NULL){
		if (dev->i2c_bus_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", I2C_BUS_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", I2C_BUS_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
static exception_t get_attr_i2c_bus(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	iic_c6713_device *dev = obj->obj;
	*obj2 = dev->i2c_bus_obj;
	*port =NULL;
	return No_exp;
}
static conf_object_t* new_iic_c6713(char *obj_name)
{
	iic_c6713_device* dev = skyeye_mm_zero(sizeof(iic_c6713_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	regs->i2cpid1 = 0x00000103;
	regs->i2cpid2 = 0x00000005;
	dev->i2c_module = -1;
	return dev->obj;
}
static exception_t free_iic_c6713(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_iic_c6713(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6713_iic",
		.class_desc = "c6713_iic",
		.new_instance = new_iic_c6713,
		.free_instance = free_iic_c6713,
		.module_type = SKYML_ONCHIP(SC_c6713),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = iic_c6713_read,
		.write = iic_c6713_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = iic_c6713_get_regvalue_by_id,
		.get_regname_by_id = iic_c6713_get_regname_by_id,
		.set_regvalue_by_id = iic_c6713_set_regvalue_by_id,
		.get_regnum = iic_c6713_get_regnum,
		.get_regid_by_name = iic_c6713_get_regid_by_name
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
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription)
		{
			.name = I2C_BUS_INTF_NAME,
			.set = set_attr_i2c_bus,
			.get = get_attr_i2c_bus,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

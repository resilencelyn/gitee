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
#include <skyeye_system.h>
#include "htey_cmd_file.h"

static exception_t htey_cmd_file_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_cmd_file_device *dev = opaque->obj;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in htey_cmd_file\n", offset);
			break;
	}
	return 0;
}
static exception_t htey_cmd_file_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_cmd_file_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in htey_cmd_file\n", offset);
			break;
	}
	return 0;
}
static char* htey_cmd_file_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t htey_cmd_file_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	htey_cmd_file_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t htey_cmd_file_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t htey_cmd_file_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	htey_cmd_file_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t htey_cmd_file_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static exception_t serial_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	htey_cmd_file_device *dev = obj->obj;
	dev->serial = obj2;
	dev->serial_iface = (skyeye_serial_device_t*)SKY_get_iface(obj2, SERIAL_DEVICE_INTERFACE);
	if (dev->serial_iface == NULL){
		if (dev->serial == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SERIAL_DEVICE_INTERFACE);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SERIAL_DEVICE_INTERFACE, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
 
static exception_t serial_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	htey_cmd_file_device *dev = obj->obj;
	*obj2 = dev->serial;
	*port = NULL;
	return No_exp;
}


char cfk_03[300] = {0x3, 0x7f, 0x72, 0x10, 0x3b, 0xc4, 0xae, 0xe0, 0x3f, 0xf6, 0xa6, 0x93, 0x7d, 0xdf, 0x92, 0xde, 0xbf, 0xe, 0x6d, 0x10, 0x3b, 0xc4, 0xae, 0xe0, 0xbf, 0xee, 0xb0, 0x93, 0x7d, 0xdf, 0x92, 0xde, 0x3f, 0x0, 0x0, 0x24, 0x42, 0x20, 0xb7, 0xfe, 0xc2, 0x0, 0x0, 0xf8, 0x41, 0x20, 0xb7, 0xfe, 0xc2, 0xdd, 0xfc, 0xde, 0xbc, 0x9a, 0x90, 0x44, 0x40, 0xbb, 0xa7, 0x68, 0x24, 0xe0, 0x23, 0x59, 0x40, 0x0, 0x0, 0xb4, 0x42, 0x0, 0x80, 0x6f, 0x44, 0x8, 0xe5, 0x3c, 0x1e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xd2, 0x2, 0x8, 0xc8, 0xab, 0x2c, 0x58, 0x48, 0x59, 0xda, 0x55, 0x48, 0x74, 0x33, 0xa1, 0x43, 0xcf, 0xf3, 0x72, 0xc4, 0x38, 0x96, 0x42, 0xc4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc8, 0x42, 0xc5, 0x6, 0x8c, 0x47, 0x0, 0x0, 0xf0, 0x42, 0x66, 0x66, 0xa2, 0x41, 0x0, 0x0, 0x70, 0x42, 0xed, 0x83, 0xc2, 0xc2, 0x0, 0x0, 0xc8, 0x42, 0x0, 0x0, 0x0, 0x0, 0x67, 0x0, 0x0, 0x50, 0xc3, 0x47, 0x0, 0x40, 0x1c, 0x47, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x25, 0x7f};

char cfk_96[16] = {0x96, 0x02, 0x8f};
void send_func(void *obj)
{
	htey_cmd_file_device *dev = ((conf_object_t *)obj)->obj;
	dev->send_index++;
	//dev->serial_iface->write(dev->serial, 0x3);
	//char str[10] = {0x3, 0xf, 0x10, 0x1f};
	
	if(dev->send_index == 1){
		dev->serial_iface->write_chars(dev->serial, cfk_03, 203);
	}
	if(dev->send_index == 3){
		dev->serial_iface->write_chars(dev->serial, cfk_96, 4);
	}
}

static conf_object_t* new_htey_cmd_file(char *obj_name)
{
	htey_cmd_file_device* dev = skyeye_mm_zero(sizeof(htey_cmd_file_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->send_index = 0;
	struct registers *regs = &(dev->regs);

 	//system_register_timer_handler(dev->obj, 10000, (sched_func_t)send_func, SCHED_NORMAL);
	system_register_timer_handler(dev->obj, 100, (time_sched_t)send_func, SCHED_MS | SCHED_NORMAL);

	return dev->obj;
}

static exception_t free_htey_cmd_file(conf_object_t* opaque)
{
	struct cmd_file_device *dev = opaque->obj;
	fclose(dev->tx_fp);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t serial_write(conf_object_t *obj, uint32_t value)
{
	struct cmd_file_device *dev = obj->obj;
	char ch = value;
	if(dev->tx_fp != NULL)
		fwrite(&ch, 1, 1, dev->tx_fp);
		fflush(dev->tx_fp);
	return No_exp;
}	

static exception_t serial_write_chars(conf_object_t *obj, void *buf, uint32_t cnt)
{
	struct cmd_file_device *dev = obj->obj;
	return No_exp;
}	

static attr_value_t get_tx_file(void* arg, conf_object_t* obj, attr_value_t* idx){
	struct cmd_file_device *dev = obj->obj;
	attr_value_t name = SKY_make_attr_string("");
	return name;
}

static exception_t set_tx_file(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	struct cmd_file_device *dev = obj->obj;
	char *fname = SKY_attr_string(*value);
	dev->tx_fp = fopen(fname, "w+");
}

static void class_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "tx_file", get_tx_file, NULL, set_tx_file, NULL, SKY_Attr_Optional, "string", "");
	return;
}

void init_htey_cmd_file(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "htey_cmd_file",
		.class_desc = "htey_cmd_file",
		.new_instance = new_htey_cmd_file,
		.free_instance = free_htey_cmd_file
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = htey_cmd_file_read,
		.write = htey_cmd_file_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = htey_cmd_file_get_regvalue_by_id,
		.get_regname_by_id = htey_cmd_file_get_regname_by_id,
		.set_regvalue_by_id = htey_cmd_file_set_regvalue_by_id,
		.get_regnum = htey_cmd_file_get_regnum,
		.get_regid_by_name = htey_cmd_file_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static skyeye_serial_device_t serial_device = {
		.write = serial_write,
		.write_chars = serial_write_chars,
		.receive_ready = NULL,
		};

	SKY_register_iface(clss, SERIAL_DEVICE_INTERFACE, &serial_device);
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
		(struct InterfaceDescription) {
			.name = SERIAL_DEVICE_INTERFACE,
			.iface = &serial_device,
		},
	
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription ){
			.name = SERIAL_DEVICE_INTERFACE,
			.set = serial_set,
			.get = serial_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	class_register_attribute(clss);
}

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
#include <skyeye_uart_ops.h>
#define DEBUG
#include <skyeye_log.h>
#include "htey_hdlc_cmd_file.h"
static exception_t htey_hdlc_cmd_file_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_hdlc_cmd_file_device *dev = opaque->obj;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in htey_hdlc_cmd_file\n", offset);
			break;
	}
	return 0;
}
static exception_t htey_hdlc_cmd_file_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_hdlc_cmd_file_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in htey_hdlc_cmd_file\n", offset);
			break;
	}
	return 0;
}
static char* htey_hdlc_cmd_file_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t htey_hdlc_cmd_file_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	htey_hdlc_cmd_file_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t htey_hdlc_cmd_file_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t htey_hdlc_cmd_file_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	htey_hdlc_cmd_file_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t htey_hdlc_cmd_file_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static exception_t serial_write(conf_object_t *obj, uint32_t value)
{
	htey_hdlc_cmd_file_device *dev = obj->obj;

	return No_exp;
}

static exception_t serial_write_chars(conf_object_t *obj, void *buf, uint32_t cnt)
{
	uint32_t frame_cnt, hit = 0;
	htey_hdlc_cmd_file_device *dev = obj->obj;

	unsigned char key[3];

	key[1] = *((char *)buf);
	key[0] = *((char *)buf+1);
	key[2] = *((char *)buf+2);

	file_data_t *data = dev->file_data;
	while (data != NULL) {
		unsigned char *ch = data->buf+data->index*data->frme_len;
		if(data->index == data->frme_cnt){
				skyeye_log(Warning_log, __FUNCTION__, "file data is send completed!!\n");
				break;
			}

		if ((*(ch) == key[0]) && (*(ch+1) == key[1]) && (*(ch+2) == key[2])) {			
			
			if(dev->serial_iface != NULL){
					dev->serial_iface->write_chars(dev->serial, ch, data->frme_len);
			}
			if(data->index < data->frme_cnt)
				data->index++;
			hit = 1;
			break;
		}
		data = data->next;
	}
	if(!hit)
		skyeye_log(Warning_log, __FUNCTION__, "no command file found\n");
	
	return No_exp;
}	

static conf_object_t* new_htey_hdlc_cmd_file(char *obj_name)
{
	htey_hdlc_cmd_file_device* dev = skyeye_mm_zero(sizeof(htey_hdlc_cmd_file_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->file_data = NULL;
	return dev->obj;
}
static exception_t free_htey_hdlc_cmd_file(conf_object_t* opaque)
{
	htey_hdlc_cmd_file_device *dev = opaque->obj;

	file_data_t *data = dev->file_data;
	file_data_t *tmp = data;
	int i;
	while (data != NULL) {
		skyeye_free(data->buf);
		tmp = data;
		data = data->next;
		skyeye_free(tmp);
	}
	dev->file_data = NULL;

	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t serial_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	htey_hdlc_cmd_file_device *dev = obj->obj;
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
	htey_hdlc_cmd_file_device *dev = obj->obj;
	*obj2 = dev->serial;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_cmd_file_name(void* arg, conf_object_t* obj, attr_value_t* idx){
	htey_hdlc_cmd_file_device *dev = obj->obj;
	attr_value_t name = SKY_make_attr_string("");
	return name;
}

static exception_t set_cmd_file_name(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	htey_hdlc_cmd_file_device *dev = obj->obj;
	char *filename;
	filename = SKY_attr_string(*value);
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) {
		char error[128] = {0};
		sprintf(error, "open file %s error", filename);
		perror(error);
		return File_open_exp;
	}

	int flen;

	fseek(fp, 0, SEEK_END);
	flen = ftell(fp);

	file_data_t *data = skyeye_mm_zero(sizeof(struct file_data_s));
	data->file_len = flen;
	data->index = 0;
	data->filename = filename;

	if (dev->file_data == NULL) {
		dev->file_data = data;
	} else {
		data->next = dev->file_data;
		dev->file_data = data;
	}

	unsigned char *fbuf = skyeye_mm_zero(flen+1);
	data->buf = fbuf;
	rewind(fp);
	fread(fbuf, 1, flen, fp);

	unsigned char key[3];

	key[0] = *fbuf;
	key[1] = *(fbuf+1);
	key[2] = *(fbuf+2);
	

	int i, buf_len = flen;
	for (i = 3; i < flen; i++) {
		if ((*(fbuf+i) == key[0])) {
			if ((*(fbuf+i+1) == key[1])) {
				if ((*(fbuf+i+2) == key[2])) {
					buf_len = i;
					break;
				}
			}
		}
	}
	
	data->frme_len = buf_len;
	data->frme_cnt = data->file_len / data->frme_len;
	
	fclose(fp);
	return No_exp;
}

static attr_value_t get_tx_file(void* arg, conf_object_t* obj, attr_value_t* idx){
	htey_hdlc_cmd_file_device *dev = obj->obj;
	attr_value_t name = SKY_make_attr_string("");
	return name;
}

static exception_t set_tx_file(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	htey_hdlc_cmd_file_device *dev = obj->obj;
	char *fname = SKY_attr_string(*value);
	dev->tx_fp = fopen(fname, "w+");
}


static void class_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "cmd_file_name", get_cmd_file_name, NULL, set_cmd_file_name, NULL, SKY_Attr_Optional, "string", "");

	SKY_register_attribute(clss, "tx_file", get_tx_file, NULL, set_tx_file, NULL, SKY_Attr_Optional, "string", "");
	return;
}

void init_htey_hdlc_cmd_file(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "htey_hdlc_cmd_file",
		.class_desc = "htey_hdlc_cmd_file",
		.new_instance = new_htey_hdlc_cmd_file,
		.free_instance = free_htey_hdlc_cmd_file
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = htey_hdlc_cmd_file_read,
		.write = htey_hdlc_cmd_file_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = htey_hdlc_cmd_file_get_regvalue_by_id,
		.get_regname_by_id = htey_hdlc_cmd_file_get_regname_by_id,
		.set_regvalue_by_id = htey_hdlc_cmd_file_set_regvalue_by_id,
		.get_regnum = htey_hdlc_cmd_file_get_regnum,
		.get_regid_by_name = htey_hdlc_cmd_file_get_regid_by_name
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

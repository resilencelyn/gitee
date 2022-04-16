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
#include <skyeye_log.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>
#include <skyeye_uart_ops.h>
#include <skyeye_system.h>
//#define DEBUG
#include "c6678_mt29f1g.h"

#ifdef DEBUG
#define debug_printf printf
#else
#define debug_printf
#endif

#define IMAGE_BASE_ADDR    0x100

// Register read operation method implementation
static exception_t c6678_mt29f1g_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	c6678_mt29f1g_device *dev = (c6678_mt29f1g_device *)opaque->obj;

	if(dev->regs.reg_c6678_mt29f1g_read_operation == MT29F1G_FLASH_INS_RDPAGE)
	{
		fclose(dev->wr_fp);
		dev->rd_fp = fopen(dev->filename, "r");
		if (dev->rd_fp == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "can not open file: %s\n", dev->filename);
			return Not_found_exp;
		}
		fseek(dev->rd_fp, (offset - IMAGE_BASE_ADDR), SEEK_SET);
		fread(buf, 1, 1, dev->rd_fp);
		printf("File read addr 0x%x, data 0x%x\n", offset, *(uint8_t*)buf);

		if (dev->image_space)
		{
			dev->image_space->read(dev->image, offset, buf, count);
			printf("image read addr :%x, buf:%x\n", offset, *(uint8_t*)buf);
		}
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t c6678_mt29f1g_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	c6678_mt29f1g_device *dev = opaque->obj;
	uint8_t data = *(uint8_t *)buf;
	uint32_t i = 0;

	if (dev->write_flag == 1)
	{
		if (dev->image_space)
		{
			dev->image_space->write(dev->image, offset, &data, 1);
			debug_printf("write addr :%x, buf:%x\n", offset, data);
		}
		if(dev->wr_fp == NULL)
		{
			dev->wr_fp = fopen(dev->filename, "w+");
		}
		if (dev->wr_fp != NULL)
		{
			debug_printf("offset 0x%x | data : %c \n",offset, (data));
			fwrite(&data, 1, 1, dev->wr_fp);
		}
	}

	switch (offset)
	{
		case REG_C6678_MT29F1G_STATUS_OPERATION:
		{
			dev->regs.reg_c6678_mt29f1g_status_operation = data;
			break;
		}
		case REG_C6678_MT29F1G_READ_OPERATION:
		{
			dev->regs.reg_c6678_mt29f1g_read_operation = data;
			break;
		}
		case REG_C6678_MT29F1G_PROGRAM_OPERATION:
		{
			dev->regs.reg_c6678_mt29f1g_program_operation = data;
			
			if (dev->regs.reg_c6678_mt29f1g_program_operation == MT29F1G_FLASH_INS_PROGRAM_PAGE)
			{
				if (dev->erase_ready == 1)
				{
					dev->write_flag = 1;
					dev->erase_ready = 0;
				}
			}
			
			break;
		}
		case REG_C6678_MT29F1G_ERASE_OPERATION:
		{
			dev->regs.reg_c6678_mt29f1g_erase_operation = data;
			
			if (dev->regs.reg_c6678_mt29f1g_erase_operation == MT29F1G_FLASH_INS_ERASEBLOCK)
			{
				for(i = 0; i < MT29F1G_FLASHBLOCKSIZE; i++)
				{
					data = 0xff;
					if (dev->image_space)
					{
						dev->image_space->write(dev->image, i, &data, 1);
					}
				}	
				dev->erase_ready = 1;
				dev->write_flag = 0;
			}
			break;
		}
		default:
		{
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in c6678_mt29f1g\n", offset);
			break;
		}
	}
	return No_exp;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_c6678_mt29f1g(const char *obj_name)
{
	c6678_mt29f1g_device* dev = skyeye_mm_zero(sizeof(c6678_mt29f1g_device));
	dev->obj = new_conf_object(obj_name, dev);

	struct registers *regs = &(dev->regs);
	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_c6678_mt29f1g(conf_object_t* conf_obj)
{
	c6678_mt29f1g_device *dev = (c6678_mt29f1g_device *)conf_obj->obj;

	dev->wr_fp = fopen(dev->filename, "w+");
	dev->rd_fp = fopen(dev->filename, "r");

	return No_exp;
}

// Free the device object to do some memory free
static exception_t free_c6678_mt29f1g(conf_object_t* conf_obj)
{
	c6678_mt29f1g_device *dev = (c6678_mt29f1g_device *)conf_obj->obj;
	
	fclose(dev->wr_fp);
	fclose(dev->rd_fp);
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

static exception_t set_attr_filename(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	c6678_mt29f1g_device *dev = conf_obj->obj;
	dev->filename = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static attr_value_t get_attr_filename(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	c6678_mt29f1g_device *dev = conf_obj->obj;
	attr_value_t value = SKY_make_attr_string(dev->filename);
	return value;
}

static void class_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "filename", get_attr_filename, NULL, set_attr_filename, NULL, SKY_Attr_Optional, "string", "The file To Flash Data");
	return;
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	c6678_mt29f1g_device *dev = (c6678_mt29f1g_device *)obj->obj;
	dev->image = obj2;
	dev->image_space = (memory_space_intf*)SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);
	if (dev->image_space == NULL){
		if (dev->image == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", MEMORY_SPACE_INTF_NAME);
		} else
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", MEMORY_SPACE_INTF_NAME, obj2->objname);
		}

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	c6678_mt29f1g_device *dev = (c6678_mt29f1g_device *)obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_c6678_mt29f1g(){
	static skyeye_class_t class_data =
	{
		.class_name = "c6678_mt29f1g",
		.class_desc = "c6678_mt29f1g device",
		.new_instance = new_c6678_mt29f1g,
		.free_instance = free_c6678_mt29f1g,
		.config_instance = config_c6678_mt29f1g,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory =
	{
		.read = c6678_mt29f1g_read,
		.write = c6678_mt29f1g_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
 	};
	static const struct ConnectDescription connects[] =
	{
		(struct ConnectDescription ){
			.name = MEMORY_SPACE_INTF_NAME,
			.set = memory_set,
			.get = memory_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	class_register_attribute(clss);
}

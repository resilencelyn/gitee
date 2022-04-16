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
#include <skyeye_log.h>
#include <skyeye_system.h>
#define DEBUG
#include "loongson_flash.h"

static exception_t loongson_flash_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_flash_device *dev = (loongson_flash_device *)conf_obj->obj;
	if (dev->filename != NULL)
	{
		if (dev->rd_fp == NULL)
			dev->rd_fp = fopen(dev->filename, "rb");

		if (dev->rd_fp == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not open file: %s!\n", dev->filename);
			if (dev->image_space)
				dev->image_space->read(dev->image, offset, buf, count);
		}
		else
		{
			fseek(dev->rd_fp, offset, SEEK_SET);
			fread(buf, 1, count, dev->rd_fp);
		}
	}
	else
	{
		if (dev->image_space)
			dev->image_space->read(dev->image, offset, buf, count);
	}

	return No_exp;
}

static exception_t loongson_flash_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_flash_device *dev = (loongson_flash_device *)conf_obj->obj;

	if (dev->filename != NULL)
	{
		if (dev->wr_fp == NULL)
			dev->wr_fp = fopen(dev->filename, "wb");

		if (dev->wr_fp == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not open file: %s!\n", dev->filename);
			if (dev->image_space)
				dev->image_space->write(dev->image, offset, buf, count);
			return Not_found_exp;
		}
		else
		{
			fseek(dev->wr_fp, offset, SEEK_SET);
			fwrite(buf, 1, count, dev->wr_fp);
			fflush(dev->wr_fp);
		}
	}

	if (dev->image_space)
		dev->image_space->write(dev->image, offset, buf, count);

	return No_exp;
}

// Register read operation method implementation
static exception_t loongson_flash_reg_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	uint32_t data;
	exception_t ret;
	loongson_flash_device* dev = conf_obj->obj;

	if (dev->image_space)
		ret = dev->image_space->read(dev->image, offset, &data, 4);

	if(ret != No_exp)
	{
		printf("In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
		return Not_found_exp;
	}

	*(uint32_t*)buf = data;
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_flash_reg_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_flash_device *dev = (loongson_flash_device *)conf_obj->obj;
	loongson_flash_reg_t* regs = dev->regs;
	uint32_t data = *(uint32_t*)buf;
	exception_t ret = Not_found_exp;

	if (regs->cmd.bits.command == 0xe && dev->image_space)	//write only on 编程命令
	{
		if(!regs->sts.bits.no_permission && !regs->sts.bits.pe_end)
		{
			if (dev->image_space)
				ret = dev->image_space->write(dev->image, offset, &data, 4);
			if(ret != No_exp)
			{
				printf("loongson flash:In %s, %s Not_found offset 0x%x\n", __func__, dev->obj->class_name, offset);
				return Not_found_exp;
			}
			if(regs->pet.bits.int_en & 0x1)
				regs->sts.bits.pe_end = 1;			//write ok,up pe_end interrupt
		}
		else
		{
			printf("loongson flash:write failed,you should clean interrupt before write!");
			return Not_found_exp;
		}
	}
	return No_exp;
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	loongson_flash_device* dev = (loongson_flash_device*)obj->obj;
	dev->image = obj2;
	dev->image_space = (memory_space_intf* )SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);
	return No_exp;
}
 
static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	loongson_flash_device* dev = (loongson_flash_device*)obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}

// Register interface: gets the register name based on the register ID
static char* loongson_flash_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static void move_cmd(conf_object_t *conf_obj)
{
	loongson_flash_device *dev = (loongson_flash_device *)conf_obj->obj;
	loongson_flash_reg_t *regs = dev->regs;
	if((regs->pet.bits.int_en & 0x2) &&regs->cmd.bits.reserved)	//up a no_permission interrupt
		regs->sts.bits.no_permission = 1;
	switch(regs->cmd.bits.command)
	{
		case 0x3:
			regs->sts.bits.no_permission = regs->sts.bits.pe_end = 0; 	//clear interrupt
			break;
		case 0x4:
			break;
		case 0xa:
			break;
		case 0xe:
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "loongson_flash:regs->cmd.bits.command is %d,failed\n", regs->cmd.bits.command);
	}

}
static void move(conf_object_t *conf_obj,  uint32_t id)
{
	loongson_flash_device *dev = (loongson_flash_device *)conf_obj->obj;
	loongson_flash_reg_t *regs = dev->regs;
	switch(id)
	{
		case 0:
			move_cmd(conf_obj);
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "loongson_flash:regs id %d,failed\n", id);
	}
}

// Register interface: set register value according to register ID
static exception_t loongson_flash_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	loongson_flash_device *dev = (loongson_flash_device *)conf_obj->obj;
	loongson_flash_reg_t *regs = dev->regs;
	*((uint32_t*)regs + id) = value;
	move(conf_obj, id);
	return No_exp;
}

// Register interface: gets how many registers the device has
static uint32_t loongson_flash_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(loongson_flash_reg_t) / 4;
}

// Register interface: gets the register value based on the register ID
static uint32_t loongson_flash_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	loongson_flash_device *dev = (loongson_flash_device *)conf_obj->obj;
	loongson_flash_reg_t *regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

// Register interface: gets the register ID based on the register name
static uint32_t loongson_flash_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(loongson_flash_reg_t) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_loongson_flash(const char *obj_name)
{
	loongson_flash_device* dev = skyeye_mm_zero(sizeof(loongson_flash_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->regs = skyeye_mm_zero(sizeof(loongson_flash_reg_t));
	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_loongson_flash(conf_object_t* conf_obj)
{
	loongson_flash_device *dev = (loongson_flash_device *)conf_obj->obj;
	// TODO: place the code here

	return No_exp;
}

// Free the device object to do some memory free
static exception_t free_loongson_flash(conf_object_t* conf_obj)
{
	loongson_flash_device *dev = (loongson_flash_device *)conf_obj->obj;
	skyeye_free(dev->regs);
	if (dev->wr_fp != NULL)
		fclose(dev->wr_fp);
	if (dev->rd_fp != NULL)
		fclose(dev->rd_fp);
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

static exception_t set_attr_filename(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	loongson_flash_device *dev = conf_obj->obj;
	dev->filename = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static attr_value_t get_attr_filename(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	loongson_flash_device *dev = conf_obj->obj;
	attr_value_t value = SKY_make_attr_string(dev->filename);
	return value;
}

static void class_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "filename", get_attr_filename, NULL, set_attr_filename, NULL, SKY_Attr_Optional, "string", "Set The flash filename");
	return;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_loongson_flash(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "loongson_flash",
		.class_desc      = "loongson_flash",
		.new_instance    = new_loongson_flash,
		.free_instance   = free_loongson_flash,
		.config_instance   = config_loongson_flash,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);
	
	// sets the register address read-write method
	static memory_space_intf io_memory_interface[] =
	{
		//0:flash reg
		{
			.read  = loongson_flash_reg_read,
			.write = loongson_flash_reg_write
		},
		//1:flash image
		{
			.read  = loongson_flash_read,
			.write = loongson_flash_write
		},

	};
	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);
	
	// register access interface (for SkyEye graphical control interface)
	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = loongson_flash_get_regvalue_by_id,
		.get_regname_by_id  = loongson_flash_get_regname_by_id,
		.set_regvalue_by_id = loongson_flash_set_regvalue_by_id,
		.get_regnum         = loongson_flash_get_regnum,
		.get_regid_by_name  = loongson_flash_get_regid_by_name
	};
	SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);
	
	// list of interface methods
	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name  = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory_interface
		},
		(struct InterfaceDescription)
		{
			.name  = SKYEYE_REG_INTF,
			.iface = &reg_interface
		},
	};
	
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = MEMORY_SPACE_INTF_NAME,
			.set = memory_set,
			.get = memory_get,
		}
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);

	class_register_attribute(dev_class);
}

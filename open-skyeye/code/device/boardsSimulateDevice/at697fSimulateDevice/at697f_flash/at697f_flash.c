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
#include <skyeye_swapendian.h>
#include "at697f_flash.h"

static exception_t at697f_flash_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	at697f_flash_device *dev = opaque->obj;

	if (dev->filename != NULL)
	{
		if (dev->rd_fp == NULL)
		{
			dev->rd_fp = fopen(dev->filename, "rb");
		}

		if (dev->rd_fp == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "can not open file: %s\n", dev->filename);
			if (dev->image_space)
				dev->image_space->read(dev->image, offset, buf, count);
		} else
		{
			fseek(dev->rd_fp, offset, SEEK_SET);
			fread(buf, 1, count, dev->rd_fp);

			if (count == 1)
			{
				//do nothing
			} else if (count == 2)
			{
				*(uint16_t *)buf = bswap_half(*(uint16_t *)buf);
			} else if (count == 4)
			{
				*(uint32_t *)buf = bswap_word(*(uint32_t *)buf);
			} else
			{
				skyeye_log(Error_log, __FUNCTION__, "Count : %d Error!\n", count);
				return Access_exp;
			}
		}
	}
	else
	{
		if (dev->image_space)
			dev->image_space->read(dev->image, offset, buf, count);
	}

	return No_exp;
}

void flash_addr8_write(conf_object_t *opaque, generic_address_t offset, uint8_t data)
{
	at697f_flash_device *dev = opaque->obj;
	struct flash_cmd_addr8 *cmd_addr = &dev->flash_cmd.cmd_addr8;
	cmd_addr->is_erase = 0;

	if (cmd_addr->write_flag)
	{
		if(dev->wr_fp == NULL)
		{
			dev->wr_fp = fopen(dev->filename, "wb+");

			if (dev->wr_fp == NULL)
			{
				skyeye_log(Error_log, __FUNCTION__, "can not open file: %s\n", dev->filename);
				return Not_found_exp;
			}

			fseek(dev->wr_fp, offset, 0);
			fwrite(&data, 1, 1, dev->wr_fp);
			fflush(dev->wr_fp);
			cmd_addr->write_flag = 0;
			cmd_addr->keyVal1 = 0;
			cmd_addr->keyVal2 = 0;
			cmd_addr->keyVal3 = 0;
		}
	}

	if (offset == Addr1_bit8)
	{
		if (cmd_addr->keyVal1 == Key1Val8)
		{
			if (data == Key3DataVal8)
			{
				cmd_addr->keyVal3 = data;
			} else if (data == Key3Erase1Val8)
			{
				cmd_addr->erase_ready = 1;
			} else if (data == Key3Erase2Val8 && cmd_addr->erase_ready)
			{
				cmd_addr->is_erase = 1;
			}
		} else if (data == Key1Val8)
		{
			cmd_addr->keyVal1 = data;
		} else if (data == 0)
		{
			cmd_addr->keyVal1 = 0;
			cmd_addr->keyVal3 = 0;
		}
	}else if (offset == Addr2_bit8)
	{
		if (data == Key2Val8)
			cmd_addr->keyVal2 = data;
	}

	if (cmd_addr->keyVal1 == Key1Val8 && cmd_addr->keyVal2 == Key2Val8
			&& cmd_addr->keyVal3 == Key3DataVal8)
	{
		cmd_addr->write_flag = 1;
	}

	if (cmd_addr->is_erase)
	{
		int i;
		for (i = 0; i < FLASH_SIZE; i++)
		{
			data = 0xff;
			if (dev->image_space)
				dev->image_space->write(dev->image, i, &data, 1);
		}
	} else
	{
		if (dev->image_space)
			dev->image_space->write(dev->image, offset, &data, 1);
	}
}

void flash_addr16_write(conf_object_t *opaque, generic_address_t offset, uint16_t data)
{
	at697f_flash_device *dev = opaque->obj;
	struct flash_cmd_addr16 *cmd_addr = &dev->flash_cmd.cmd_addr16;
	cmd_addr->is_erase = 0;

	if (cmd_addr->write_flag)
	{
		if(dev->wr_fp == NULL)
		{
			dev->wr_fp = fopen(dev->filename, "wb+");

			if (dev->wr_fp == NULL)
			{
				skyeye_log(Error_log, __FUNCTION__, "can not open file: %s\n", dev->filename);
				return Not_found_exp;
			}

			fseek(dev->wr_fp, offset, 0);
			fwrite(&data, 1, 2, dev->wr_fp);
			fflush(dev->wr_fp);
			cmd_addr->write_flag = 0;
			cmd_addr->keyVal1 = 0;
			cmd_addr->keyVal2 = 0;
			cmd_addr->keyVal3 = 0;
		}
	}

	if (offset == Addr1_bit16)
	{
		if (cmd_addr->keyVal1 == Key1Val16)
		{
			if (data == Key3DataVal16)
			{
				cmd_addr->keyVal3 = data;
			} else if (data == Key3Erase1Val16)
			{
				cmd_addr->erase_ready = 1;
			} else if (data == Key3Erase2Val16 && cmd_addr->erase_ready)
			{
				cmd_addr->is_erase = 1;
			}
		} else if (data == Key1Val16)
		{
			cmd_addr->keyVal1 = data;
		} else if (data == 0)
		{
			cmd_addr->keyVal1 = 0;
			cmd_addr->keyVal3 = 0;
		}
	}else if (offset == Addr2_bit16)
	{
		if (data == Key2Val16)
			cmd_addr->keyVal2 = data;
	}

	if (cmd_addr->keyVal1 == Key1Val16 && cmd_addr->keyVal2 == Key2Val16
			&& cmd_addr->keyVal3 == Key3DataVal16)
	{
		cmd_addr->write_flag = 1;
	}

	if (cmd_addr->is_erase)
	{
		int i;
		for (i = 0; i < FLASH_SIZE; i++)
		{
			data = 0xffff;
			if (dev->image_space)
				dev->image_space->write(dev->image, i, &data, 2);
		}
	} else
	{
		if (dev->image_space)
			dev->image_space->write(dev->image, offset, &data, 2);
	}
}

void flash_addr32_write(conf_object_t *opaque, generic_address_t offset, uint32_t data)
{
	at697f_flash_device *dev = opaque->obj;
	struct flash_cmd_addr32 *cmd_addr = &dev->flash_cmd.cmd_addr32;
	cmd_addr->is_erase = 0;

	//驱动写Flash操作如下 ADDR_UNLOCK1 = Addr1_bit32 ADDR_UNLOCK2 = Addr2_bit32
	//	ADDR_UNLOCK1 = CMD_RESET;
	//	ADDR_UNLOCK1 = CMD_UNLOCK1;
	//	ADDR_UNLOCK2 = CMD_UNLOCK2;
	//	ADDR_UNLOCK1 = CMD_PROGRAM;
	if (cmd_addr->write_flag)
	{
		if(dev->wr_fp == NULL)
		{
			dev->wr_fp = fopen(dev->filename, "wb+");

			if (dev->wr_fp == NULL)
			{
				skyeye_log(Error_log, __FUNCTION__, "can not open file: %s\n", dev->filename);
				return Not_found_exp;
			}

			fseek(dev->wr_fp, offset, 0);
			fwrite(&data, 1, 4, dev->wr_fp);
			fflush(dev->wr_fp);
			cmd_addr->write_flag = 0;
			cmd_addr->keyVal1 = 0;
			cmd_addr->keyVal2 = 0;
			cmd_addr->keyVal3 = 0;
		}
	}

	if (offset == Addr1_bit32)
	{
		if (cmd_addr->keyVal1 == CMD_UNLOCK1)
		{
			if (data == CMD_PROGRAM)
			{
				cmd_addr->keyVal3 = data;
			} else if (data == CMD_ERASE_SETUP)
			{
				cmd_addr->erase_ready = 1;
			} else if (data == CMD_ERASE_CHIP && cmd_addr->erase_ready)
			{
				cmd_addr->is_erase = 1;
			}
		} else if (data == CMD_UNLOCK1)
		{
			cmd_addr->keyVal1 = data;
		} else if (data == 0)
		{
			cmd_addr->keyVal1 = 0;
			cmd_addr->keyVal3 = 0;
		}
	}else if (offset == Addr2_bit32)
	{
		if (data == CMD_UNLOCK2)
			cmd_addr->keyVal2 = data;
	}

	if (cmd_addr->keyVal1 == CMD_UNLOCK1 && cmd_addr->keyVal2 == CMD_UNLOCK2
			&& cmd_addr->keyVal3 == CMD_PROGRAM)
	{
		cmd_addr->write_flag = 1;
	}

	if (cmd_addr->is_erase)
	{
		int i;
		for (i = 0; i < FLASH_SIZE; i++)
		{
			data = 0xffffffff;
			if (dev->image_space)
				dev->image_space->write(dev->image, i, &data, 4);
		}
	} else
	{
		if (dev->image_space)
			dev->image_space->write(dev->image, offset, &data, 4);
	}
}

static exception_t at697f_flash_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	at697f_flash_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf("in %s val: 0x%x, offset: 0x%x\n", __FUNCTION__, val, offset);

	if (val == CMD_ERASE_SECTOR)
	{
		int i;
		int data = 0xffffffff;
		if(dev->wr_fp == NULL)
		{
			dev->wr_fp = fopen(dev->filename, "wb");

			if (dev->wr_fp == NULL)
			{
				skyeye_log(Error_log, __FUNCTION__, "can not open file: %s\n", dev->filename);
				return Not_found_exp;
			}

			for (i = 0; i < FLASH_SECTOR_LEN; i++, offset++)
			{
				fseek(dev->wr_fp, offset, SEEK_SET);
				fwrite(&data, 1, count, dev->wr_fp);
				fflush(dev->wr_fp);
			}
		} else
		{
			for (i = 0; i < FLASH_SECTOR_LEN; i++, offset++)
			{
				fseek(dev->wr_fp, offset, SEEK_SET);
				fwrite(&data, 1, count, dev->wr_fp);
				fflush(dev->wr_fp);
			}
		}
	} else
	{
		if(dev->wr_fp == NULL)
		{
			dev->wr_fp = fopen(dev->filename, "wb");

			if (dev->wr_fp == NULL)
			{
				skyeye_log(Error_log, __FUNCTION__, "can not open file: %s\n", dev->filename);
				return Not_found_exp;
			}

			if (count == 1)
			{
				//do nothing
			} else if (count == 2)
			{
				*(uint16_t *)buf = bswap_half(*(uint16_t *)buf);
			} else if (count == 4)
			{
				*(uint32_t *)buf = bswap_word(*(uint32_t *)buf);
			} else
			{
				skyeye_log(Error_log, __FUNCTION__, "Count : %d Error!\n", count);
				return Access_exp;
			}

			fseek(dev->wr_fp, offset, SEEK_SET);
			fwrite(buf, 1, count, dev->wr_fp);
			fflush(dev->wr_fp);
		} else
		{
			if (count == 1)
			{
				//do nothing
			} else if (count == 2)
			{
				*(uint16_t *)buf = bswap_half(*(uint16_t *)buf);
			} else if (count == 4)
			{
				*(uint32_t *)buf = bswap_word(*(uint32_t *)buf);
			} else
			{
				skyeye_log(Error_log, __FUNCTION__, "Count : %d Error!\n", count);
				return Access_exp;
			}

			fseek(dev->wr_fp, offset, SEEK_SET);
			fwrite(buf, 1, count, dev->wr_fp);
			fflush(dev->wr_fp);
		}
	}

	if (count == 1)
	{
		flash_addr8_write(opaque, offset, (uint8_t)val);
	} else if (count == 2)
	{
		flash_addr16_write(opaque, offset, (uint16_t)val);
	} else
	{
		flash_addr32_write(opaque, offset, val);
	}

	return No_exp;
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	at697f_flash_device *dev = obj->obj;
	dev->image = obj2;
	dev->image_space = (memory_space_intf*)SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);
	if (dev->image_space == NULL)
		skyeye_log(Error_log, __func__, "at697f_flash register memory_space_intf failed!\n");
	return No_exp;
}

static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	at697f_flash_device *dev = obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}

static conf_object_t* new_at697f_flash(const char *obj_name)
{
	at697f_flash_device* dev = skyeye_mm_zero(sizeof(at697f_flash_device));
	dev->obj = new_conf_object(obj_name, dev);

	return dev->obj;
}

static exception_t free_at697f_flash(conf_object_t* opaque)
{
	at697f_flash_device *dev = opaque->obj;

	if (dev->wr_fp != NULL)
		fclose(dev->wr_fp);
	if (dev->rd_fp != NULL)
		fclose(dev->rd_fp);

	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t set_attr_filename(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	at697f_flash_device *dev = conf_obj->obj;
	dev->filename = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static attr_value_t get_attr_filename(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	at697f_flash_device *dev = conf_obj->obj;
	attr_value_t value = SKY_make_attr_string(dev->filename);
	return value;
}

static void class_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "filename", get_attr_filename, NULL, set_attr_filename, NULL, SKY_Attr_Optional, "string", "The file To Flash Data");
	return;
}

void init_at697f_flash(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "at697f_flash",
		.class_desc = "at697f_flash",
		.new_instance = new_at697f_flash,
		.free_instance = free_at697f_flash
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = at697f_flash_read,
		.write = at697f_flash_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
	};

	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
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

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
#include <skyeye_swapendian.h>
#define DEBUG
#include "at697f_iic.h"

void parseI2cDataFile(conf_object_t *conf_obj, uint8_t type)
{
	at697f_iic_device *dev = (at697f_iic_device *)conf_obj->obj;
	uint32_t cmdType, dataLen, checksum;
	uint16_t tmpLBuf[IICdataReadlen] = {0};
	uint16_t tmpHBuf[IICdataReadlen] = {0};
	int i;

	if (type == 0xe3)
	{
		dev->fp = dev->fp_e3;
	} else if (type == 0xe5)
	{
		dev->fp = dev->fp_e5;
	} else
	{
		skyeye_log(Error_log, __FUNCTION__, "Can not parse cmdType: %x\n", type);
		return;
	}

	if (dev->fp == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "fp is NULL!\n");
		return;
	}
	//get IIC syndata
	fread(&dev->recv_buf[0], 1, 4, dev->fp);
	dev->recv_buf[0] = strtol(&dev->recv_buf[0], NULL, 16);
	dev->recv_buf[0] <<= 16;

	//get IIC unitid and cmdtype
	fseek(dev->fp, 1, SEEK_CUR);
	fread(&cmdType, 1, 4, dev->fp);
	cmdType = strtol(&cmdType, NULL, 16);
	dev->recv_buf[0] |= cmdType;
	//大小端转换
	dev->recv_buf[0] = bswap_word(dev->recv_buf[0]);

	//get IIC length
	fseek(dev->fp, 1, SEEK_CUR);
	fread(&dataLen, 1, 4, dev->fp);
	dataLen = strtol(&dataLen, NULL, 16);
	dev->recv_buf[1] = dataLen << 16;

	//get IIC data(unsigned short) and load data to recv_buf(unsigned int)
	for (i = 0; i < dataLen/4; i++)
	{
		fseek(dev->fp, 1, SEEK_CUR);
		fread(&tmpLBuf[i*2], 1, 4, dev->fp);
		tmpLBuf[i*2] = strtol(&tmpLBuf[i*2], NULL, 16);

		fseek(dev->fp, 1, SEEK_CUR);
		fread(&tmpHBuf[i*2+1], 1, 4, dev->fp);
		tmpHBuf[i*2+1] = strtol(&tmpHBuf[i*2+1], NULL, 16);
		dev->recv_buf[i+2] = (tmpLBuf[i*2] << 16) | tmpHBuf[i*2+1];
		//大小端转换
		dev->recv_buf[i+2] = bswap_word(dev->recv_buf[i+2]);
	}

	if ((dataLen % 4) != 0)
	{
		int lastLen = dataLen % 4;
		if (lastLen == 1)
		{
			fseek(dev->fp, 1, SEEK_CUR);
			fread(&tmpLBuf[i*2], 1, 4, dev->fp);
			tmpLBuf[i*2] = strtol(&tmpLBuf[i*2], NULL, 16);
			checksum = tmpLBuf[i*2] & 0xff;
			dev->recv_buf[i+1] = tmpLBuf[i*2];
			dev->recv_buf[i+2] = bswap_half(checksum);
		} else if (lastLen == 2)
		{
			fseek(dev->fp, 1, SEEK_CUR);
			fread(&tmpLBuf[i*2], 1, 4, dev->fp);
			tmpLBuf[i*2] = strtol(&tmpLBuf[i*2], NULL, 16);

			fseek(dev->fp, 1, SEEK_CUR);
			fread(&tmpHBuf[i*2+1], 1, 4, dev->fp);
			tmpHBuf[i*2+1] = strtol(&tmpHBuf[i*2+1], NULL, 16);
			dev->recv_buf[i+1] = (tmpLBuf[i*2] << 16) | tmpHBuf[i*2+1];
			//大小端转换
			dev->recv_buf[i+1] = bswap_word(dev->recv_buf[i+1]);
		} else
		{
			fseek(dev->fp, 1, SEEK_CUR);
			fread(&tmpLBuf[i*2], 1, 4, dev->fp);
			tmpLBuf[i*2] = strtol(&tmpLBuf[i*2], NULL, 16);

			fseek(dev->fp, 1, SEEK_CUR);
			fread(&tmpHBuf[i*2+1], 1, 4, dev->fp);
			tmpHBuf[i*2+1] = strtol(&tmpHBuf[i*2+1], NULL, 16);
			checksum = tmpHBuf[i*2+1] & 0xff;
			dev->recv_buf[i+1] = (tmpLBuf[i*2] << 16) | (tmpHBuf[i*2+1] >> 8);
			//大小端转换
			dev->recv_buf[i+1] = bswap_word(dev->recv_buf[i+1]);
			dev->recv_buf[i+2] = bswap_half(checksum);
		}
	} else
	{
		//get IIC checksum
		fseek(dev->fp, 1, SEEK_CUR);
		fread(&checksum, 1, 4, dev->fp);
		checksum = strtol(&checksum, NULL, 16);
		dev->recv_buf[i+1] = checksum << 16;
	}

	printf("[IIC Read File]:cmdType = %x\n", cmdType);
	for (i = 0; i < (dataLen/4 + 2); i++)
	{
		printf("%x ",dev->recv_buf[i]);
	}
	printf("\n\n");
	//fp set to 0
	fseek(dev->fp, 0, SEEK_SET);
}

// Register read operation method implementation
static exception_t at697f_iic_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	at697f_iic_device *dev = (at697f_iic_device *)conf_obj->obj;
	switch (offset)
	{
		case CMD:
			*(uint32_t *)buf = dev->regs.cmd;
			break;
		case IIC_WRITE:
			*(uint32_t *)buf = dev->regs.iic_write;
			break;
		case IIC_READ:
			if (dev->regs.cmd && 0x1)
			{
				dev->regs.iic_read = dev->recv_buf[dev->rx_index];
				dev->rx_index++;
				if (dev->rx_index == dev->rx_length)
					dev->rx_index = 0;
			}
			*(uint32_t *)buf = dev->regs.iic_read;
			break;
		case STATUS:
			*(uint32_t *)buf = dev->regs.status;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in at697f_iic\n", offset);
			break;
	}
	//printf("%s offset:0x%x, val:0x%x\n", __FUNCTION__, offset, *(uint32_t *)buf);
	return No_exp;
}

// Register write operation method implementation
static exception_t at697f_iic_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	at697f_iic_device *dev = (at697f_iic_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;
	IICipmbcmdparadef sttemp;
	//printf("%s offset:0x%x, val:0x%x\n", __FUNCTION__, offset, val);
	switch (offset)
	{
		case CMD:
			dev->regs.cmd = val;
			uint32_t cmd_rw, cmd_subaddr, cmd_num;
			cmd_rw = (val >> 0)& 0x1;
			cmd_subaddr = (val >> 1) & 0x7f;
			cmd_num = (val >> 8) & 0xFFFF;
			/* IIC read 1 or write 0 flag */
			if (cmd_rw == 1)
			{
				/*read syndata, cmdtype and length*/
				memset(&sttemp, 0, sizeof(IICipmbcmdparadef));
				sttemp.syndata = dev->iic_buf[dev->tx_index - 1] ;
				sttemp.unitid = dev->iic_buf[dev->tx_index - 1] >> 16;
				sttemp.cmdtype = dev->iic_buf[dev->tx_index - 1] >> 24;
				sttemp.length = (cmd_num / 4) + (((cmd_num % 4) == 0) ? 0 : 1);

				dev->rx_length = sttemp.length;

				if (sttemp.cmdtype == 0xe3 || sttemp.cmdtype == 0xe5)
				{
					parseI2cDataFile(conf_obj, sttemp.cmdtype);
				} else
				{
					return No_exp;
				}
			}
			else
			{
				/*write offset, subaddr, data and num*/
				/*send to slave*/
				dev->regs.status = (dev->regs.status >> 31) | 0;
			}
			break;
		case IIC_WRITE:
			dev->regs.status |= 1;
			dev->regs.iic_write = val;
			uint8_t tmp = val >> 24;
			if (tmp == 0xe3 || tmp== 0xe5)
			{
				dev->iic_buf[dev->tx_index] = val;
				dev->tx_index++;
				if(dev->tx_index >= BUF_SIZE)
					dev->tx_index = 0;
			}
			break;
		case IIC_READ:
			dev->regs.iic_read = val;
			break;
		case STATUS:
			dev->regs.status = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in at697f_iic\n", offset);
			break;
	}
	return No_exp;
}

// Register interface: gets the register name based on the register ID
static char* at697f_iic_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

// Register interface: set register value according to register ID
static exception_t at697f_iic_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	at697f_iic_device *dev = (at697f_iic_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

// Register interface: gets how many registers the device has
static uint32_t at697f_iic_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

// Register interface: gets the register value based on the register ID
static uint32_t at697f_iic_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	at697f_iic_device *dev = (at697f_iic_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

// Register interface: gets the register ID based on the register name
static uint32_t at697f_iic_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_at697f_iic(const char *obj_name)
{
	at697f_iic_device* dev = skyeye_mm_zero(sizeof(at697f_iic_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here

	// Register initialization
	struct registers *regs = &(dev->regs);

	char* filename_e3 = "iic_data//iic_e3.data";
	dev->fp_e3 = fopen(filename_e3, "r+");

	char* filename_e5 = "iic_data//iic_e5.data";
	dev->fp_e5 = fopen(filename_e5, "r+");

	if(dev->fp_e3 == NULL || dev->fp_e5 == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "can not open file: %s or %s \n", filename_e3, filename_e5);
		return NULL;
	}

	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_at697f_iic(conf_object_t* conf_obj)
{
	at697f_iic_device *dev = (at697f_iic_device *)conf_obj->obj;
	// TODO: place the code here

	No_exp;
}

// Free the device object to do some memory free
static exception_t free_at697f_iic(conf_object_t* conf_obj)
{
	at697f_iic_device *dev = (at697f_iic_device *)conf_obj->obj;

	if (dev->fp_e3 != NULL)
		fclose(dev->fp_e3);

	if (dev->fp_e5 != NULL)
		fclose(dev->fp_e5);

	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_at697f_iic(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "at697f_iic",
		.class_desc      = "at697f_iic",
		.new_instance    = new_at697f_iic,
		.free_instance   = free_at697f_iic,
		.config_instance = config_at697f_iic,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

	// sets the register address read-write method
	static memory_space_intf io_memory_interface =
	{
		.read  = at697f_iic_read,
		.write = at697f_iic_write
	};
	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

	// register access interface (for SkyEye graphical control interface)
	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = at697f_iic_get_regvalue_by_id,
		.get_regname_by_id  = at697f_iic_get_regname_by_id,
		.set_regvalue_by_id = at697f_iic_set_regvalue_by_id,
		.get_regnum         = at697f_iic_get_regnum,
		.get_regid_by_name  = at697f_iic_get_regid_by_name
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

	// list of interface connection methods
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
}

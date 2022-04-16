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
#define DEBUG
#include "loongson_ipmb.h"

int ad_receive_data(conf_object_t *ad_device, void *buf, int length)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)ad_device->obj;
	uint8_t RecvBuf[256] = {0};
	uint8_t dataType = *(uint8_t *)buf;
	int i, write_len;

	//buf[0] used to dataType, valid data from buf[1]
	memcpy(RecvBuf, &buf[1], length);
	dev->dataType = dataType;

	if(dataType > TYPE_MAX)
	{
		printf("In %s, Unknown dataType:%x\n", __func__, dataType);
		return -1;
	}

	if((length - 1) % 2 == 0)
	{
		write_len = length / 2;
	}
	else
	{
		write_len = (length -1) / 2 + 1;
	}

	/*
	printf("length = %d write_len = %d\n", length, write_len);
	for (i = 0;i < write_len;i++)
	{
		printf("0x%x ",RecvBuf[i]);
	}
	printf("\n");
	*/
	switch(dev->dataType)
	{
		case AD_TYPE:
			for(i = 1;i <= write_len; i++)
			{
				//dev->regs.ad_data[i - 1] = RecvBuf[i];
				dev->regs.ad_data[i - 1] = U8_TO_U16(RecvBuf, i);
			}
			break;
		case DA_TYPE:
			for(i = 1; i <= write_len; i++)
			{
				//dev->regs.da_data[i - 1] = RecvBuf[i];
				dev->regs.da_data[i - 1] = U8_TO_U16(RecvBuf, i);
			}
			break;
		case WARN_TYPE:
			for(i = 1; i <= write_len; i++)
			{
				//dev->regs.warn_data[i - 1] = RecvBuf[i];
				dev->regs.warn_data[i - 1] = U8_TO_U16(RecvBuf, i);
			}
			break;
		case THRESHOLD_TYPE:
			for(i = 0;i < THRE_CHANNEL;i++)
			{
				dev->regs.thre_data[i] = 0x00FF00FF;
			}
			break;
		case ANADATA_GET_TYPE:
			for(i = 1;i <= write_len;i++)
			{
				//dev->regs.ana_data[i - 1] = RecvBuf[i];
				dev->regs.ana_data[i - 1] = U8_TO_U16(RecvBuf, i);
			}

			break;
		default:
			printf("Cannot recognize datatype : %d\n",dev->dataType);
			break;
	}
	dev->regs.busy_status = 0x7f;
	return No_exp;
}

// Register read operation method implementation

static exception_t loongson_ipmb_poweroff_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;

	switch(offset)
	{
		case POWEROFF_REG:
			*(uint32_t *)buf = dev->regs.poweroff_reg;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_ipmb_poweroff_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	switch(offset)
	{
		case POWEROFF_REG:
			dev->regs.poweroff_reg = val;
			if (dev->regs.poweroff_reg == 0xA5)
			{
				//��ֵ�ض���д��ƫ��Ϊ136�ļĴ�����
				dev->regs.thre_data[0x88 / 4] = (dev->regs.threshod_reg[28] >> 12) & 0xff;
				dev->regs.thre_data[0x8c / 4] = (dev->regs.threshod_reg[29] >> 12) & 0xff;
			}
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

static exception_t loongson_ipmb_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;

	switch(offset)
	{
		case 0:
			*(uint32_t *)buf = dev->regs.readFlag_reg;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_ipmb_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	switch(offset)
	{
		case 0:
			dev->regs.readFlag_reg = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

static exception_t loongson_ipmb_set_threshold_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;

	switch(offset)
	{
		case THRESHOD_REG:
			*(uint32_t *)buf = dev->regs.threshod_reg[dev->cnt];
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_ipmb_set_threshold_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	//printf("In %s, offset = %x val = %x count = %x\n", __func__, offset, val, count);
	switch(offset)
	{
		case THRESHOD_REG:
			dev->regs.threshod_reg[dev->cnt] = val;
			dev->cnt++;
			if (dev->cnt == THRESHOD_CHANNEL)
				dev->cnt = 0;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

static exception_t loongson_ipmb_busy_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;

	if((offset < 0x4) && (offset > 0x0))
	{
		*(uint32_t*)buf = dev->regs.busy_status >> (offset * 8);
		return No_exp;
	}

	switch(offset)
	{
		case 0x0:
			*(uint32_t *)buf = dev->regs.busy_status;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_ipmb_busy_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	if(offset > 0x0 && offset < 0x4)
	{
		dev->regs.busy_status &= clear_bits(dev->regs.busy_status,(offset + 1) * 8 - 1 , offset * 8 );
		dev->regs.busy_status |= (val & 0xff) << (8 * offset);
		return No_exp;
	}

	switch(offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

static exception_t loongson_ipmb_warnning_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;

	if((offset % 4) != 0)
	{
		uint8_t chanId = offset / 4;
		*(uint32_t*)buf = dev->regs.warn_data[chanId] >> ((offset % 4) * 8);
		return No_exp;
	}

	if((offset >= 0) && (offset <= 0x9C))
	{
		*(uint32_t *)buf = dev->regs.warn_data[offset / 4];
		return No_exp;
	}

	switch(offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_ipmb_warnning_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	if((offset % 4) != 0)
	{
		uint8_t chanId = (offset) / 4;
		dev->regs.warn_data[chanId] &= clear_bits(dev->regs.warn_data[chanId],((offset % 4) + 1) * 8 - 1 , (offset % 4) * 8 );
		dev->regs.warn_data[chanId] |= (val & 0xff) << (8 * offset);
		return No_exp;
	}

	switch(offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

static exception_t loongson_ipmb_ad_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	
	if((offset % 4) != 0)
	{
		uint8_t chanId = offset / 4;
		*(uint32_t*)buf = dev->regs.ad_data[chanId] >> ((offset % 4) * 8);
		return No_exp;
	}

	if((offset >= 0) && (offset <= 0x5C))
	{
		*(uint32_t *)buf = dev->regs.ad_data[offset / 4];
		return No_exp;
	}

	switch(offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_ipmb_ad_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	if((offset % 4) != 0)
	{
		uint8_t chanId = (offset) / 4;
		dev->regs.ad_data[chanId] &= clear_bits(dev->regs.ad_data[chanId],((offset % 4) + 1) * 8 - 1 , (offset % 4) * 8 );
		dev->regs.ad_data[chanId] |= (val & 0xff) << (8 * offset);
		return No_exp;
	}

	switch(offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

static exception_t loongson_ipmb_da_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;

	if((offset % 4) != 0)
	{
		uint8_t chanId = offset / 4;
		*(uint32_t*)buf = dev->regs.da_data[chanId] >> ((offset % 4) * 8);
		return No_exp;
	}

	if((offset >= 0) && (offset <= 0x38))
	{
		*(uint32_t *)buf = dev->regs.da_data[offset / 4];
		return No_exp;
	}

	switch(offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_ipmb_da_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	if((offset % 4) != 0)
	{
		uint8_t chanId = (offset) / 4;
		dev->regs.da_data[chanId] &= clear_bits(dev->regs.da_data[chanId],((offset % 4) + 1) * 8 - 1 , (offset % 4) * 8 );
		dev->regs.da_data[chanId] |= (val & 0xff) << (8 * offset);
		return No_exp;
	}

	switch(offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

static exception_t loongson_get_curThreshold_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;

	//printf("In %s, offset = %x count = %d\n", __func__, offset, count);
	if((offset % 4) != 0)
	{
		uint8_t chanId = offset / 4;
		assert((offset / 4) < THRE_CHANNEL);
		*(uint32_t*)buf = dev->regs.thre_data[chanId] >> ((offset % 4) * 8);
		return No_exp;
	}

	if((offset >= 0) && (offset <= THRE_CHANNEL * 4))
	{
		assert((offset / 4) < THRE_CHANNEL);
		*(uint32_t *)buf = dev->regs.thre_data[offset / 4];
		return No_exp;
	}

	switch(offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_get_curThreshold_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	if((offset % 4) != 0)
	{
		uint8_t chanId = (offset) / 4;
		dev->regs.thre_data[chanId] &= clear_bits(dev->regs.thre_data[chanId],((offset % 4) + 1) * 8 - 1 , (offset % 4) * 8 );
		dev->regs.thre_data[chanId] |= (val & 0xff) << (8 * offset);
		return No_exp;
	}

	switch(offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

static exception_t loongson_ipmb_analogy_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;

	if((offset % 4) != 0)
	{
		uint8_t chanId = offset / 4;
		*(uint32_t*)buf = dev->regs.ana_data[chanId] >> ((offset % 4) * 8);
		return No_exp;
	}

	if((offset >= 0) && (offset < (ANA_CHANNEL * 4)))
	{
		*(uint32_t *)buf = dev->regs.ana_data[offset / 4];
		return No_exp;
	}

	switch(offset){
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_ipmb_analogy_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	if((offset % 4) != 0)
	{
		uint8_t chanId = (offset) / 4;
		dev->regs.ana_data[chanId] &= clear_bits(dev->regs.ana_data[chanId],((offset % 4) + 1) * 8 - 1 , (offset % 4) * 8 );
		dev->regs.ana_data[chanId] |= (val & 0xff) << (8 * offset);
		return No_exp;
	}

	if((offset >= 0) && (offset < (ANA_CHANNEL * 4)))
	{
		dev->regs.ana_data[offset / 4] = val;
		return No_exp;
	}

	switch(offset){
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_ipmb\n", offset);
			break;
	}
	return No_exp;
}

// Register interface: gets the register name based on the register ID
static char* loongson_ipmb_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

// Register interface: set register value according to register ID
static exception_t loongson_ipmb_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

// Register interface: gets how many registers the device has
static uint32_t loongson_ipmb_get_regnum(conf_object_t *conf_obj)
{
	return  0;
}

// Register interface: gets the register value based on the register ID
static uint32_t loongson_ipmb_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

// Register interface: gets the register ID based on the register name
static uint32_t loongson_ipmb_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_loongson_ipmb(const char *obj_name)
{
	loongson_ipmb_device* dev = skyeye_mm_zero(sizeof(loongson_ipmb_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here


	// Register initialization
	struct registers *regs = &(dev->regs);
	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_loongson_ipmb(conf_object_t* conf_obj)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	// TODO: place the code here

	No_exp;
}

// Free the device object to do some memory free
static exception_t free_loongson_ipmb(conf_object_t* conf_obj)
{
	loongson_ipmb_device *dev = (loongson_ipmb_device *)conf_obj->obj;
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

static exception_t ad_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	loongson_ipmb_device* dev = (loongson_ipmb_device*)(obj->obj);
	dev->ad_linker = obj2;

	dev->ad_linker_iface = (ad_linker_intf*)SKY_get_iface(obj2, AD_LINKER_INTF);
	if (dev->ad_linker_iface == NULL){
		if (obj2 == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Get 'ad_linker_iface' interface from NULL object fail.", obj2);
		else
			skyeye_log(Error_log, __FUNCTION__, "Get 'ad_linker_iface' interface from %s fail.", obj2->objname);
		return Not_found_exp;
	}
	return No_exp;

}

static exception_t ad_linker_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	loongson_ipmb_device* dev = (loongson_ipmb_device*)(obj->obj);

	*obj2 = dev->ad_linker;
	return No_exp;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_loongson_ipmb(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "loongson_ipmb",
		.class_desc      = "loongson_ipmb",
		.new_instance    = new_loongson_ipmb,
		.free_instance   = free_loongson_ipmb,
		.config_instance = config_loongson_ipmb,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);
	
	// sets the register address read-write method
	static memory_space_intf io_memory_interface[] =
	{
		//0:IPMB 断电指令地址
		{
			.read  = loongson_ipmb_poweroff_read,
			.write = loongson_ipmb_poweroff_write
		},
		//1:读取完成标志
		{
			.read  = loongson_ipmb_read,
			.write = loongson_ipmb_write
		},
		//2:IPMB 阖值设置地址
		{
			.read  = loongson_ipmb_set_threshold_read,
			.write = loongson_ipmb_set_threshold_write
		},
		//3:IPMB 是否空闲查询
		{
			.read  = loongson_ipmb_busy_read,
			.write = loongson_ipmb_busy_write
		},
		//4:IPMB 告警数据记录
		{
			.read  = loongson_ipmb_warnning_read,
			.write = loongson_ipmb_warnning_write
		},
		//5:IPMB 模拟量读取
		{
			.read  = loongson_ipmb_ad_read,
			.write = loongson_ipmb_ad_write
		},
		//6:IPMB 数字量读取
		{
			.read  = loongson_ipmb_da_read,
			.write = loongson_ipmb_da_write
		},
		//7:IPMB 电流阈值读取
		{
			.read  = loongson_get_curThreshold_read,
			.write = loongson_get_curThreshold_write
		},
		//8.
		{
			.read  = loongson_ipmb_analogy_read,
			.write = loongson_ipmb_analogy_write
		}

	};

	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

	// register access interface (for SkyEye graphical control interface)

	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = loongson_ipmb_get_regvalue_by_id,
		.get_regname_by_id  = loongson_ipmb_get_regname_by_id,
		.set_regvalue_by_id = loongson_ipmb_set_regvalue_by_id,
		.get_regnum         = loongson_ipmb_get_regnum,
		.get_regid_by_name  = loongson_ipmb_get_regid_by_name
	};
	SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);

	static const ad_dev_intf ad_dev_iface = {
		.receive_data = ad_receive_data
	};
	SKY_register_iface(dev_class, AD_DEV_INTF, &ad_dev_iface);

	// list of interface methods
	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name  = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory_interface
		},
		/*
		(struct InterfaceDescription)
		{
			.name  = SKYEYE_REG_INTF,
			.iface = &reg_interface
		},
		*/
		(struct InterfaceDescription)
		{
			.name  = AD_DEV_INTF,
			.iface = &ad_dev_iface
		},
	};
	
	// list of interface connection methods
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription ){
			.name = AD_LINKER_INTF,
			.set = ad_linker_set,
			.get = ad_linker_get,
		},
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
}

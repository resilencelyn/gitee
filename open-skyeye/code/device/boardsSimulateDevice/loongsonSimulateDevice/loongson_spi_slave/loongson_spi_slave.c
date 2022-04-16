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
#include "loongson_spi_slave.h"

uint8_t AddSum ( const uint8_t* pBuffer, uint8_t len )
{
	uint8_t sum = 0;	/* 累加和 */

	while ( len != 0 )
	{
		sum = ( sum + ( *pBuffer ) );		/*按字节进行累加*/
		pBuffer++;
		len--;
	}

	return sum;	/* 返回累加和计算值 */
}

static exception_t loongson_spi_slave_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_spi_slave_device *dev = (loongson_spi_slave_device *)conf_obj->obj;
	
}

static exception_t loongson_spi_slave_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_spi_slave_device *dev = (loongson_spi_slave_device *)conf_obj->obj;
}

void spi_msg_process(conf_object_t *slave)
{
	loongson_spi_slave_device *dev = slave->obj;
	/*send cmd to device on FPGA*/

	if (dev->sendProtocol.cmd == SPI_READ)
	{
		if(dev->memory_iface)
		{
			if (dev->sendProtocol.data.dwData == 0xAAAAAAAA) //读出数据并发送给SPI Master
			{
				dev->sendProtocol.data.dwData = 0;
				if (dev->sendProtocol.addr == RT2_BASE_ADDR || dev->sendProtocol.addr == RT3_BASE_ADDR)
					dev->memory_iface->read(dev->memory, dev->readAddr, &dev->sendProtocol.data.dwData, 4);
				else
					dev->memory_iface->read(dev->memory, dev->sendProtocol.addr, &dev->sendProtocol.data.dwData, 4);

				debug_printf("[SPI SLAVE FPGA READ]=>Addr = %x, Data = %x\n", dev->sendProtocol.addr, dev->sendProtocol.data.dwData);
				dev->sendProtocol.cs = AddSum ( ( uint8_t* ) &dev->sendProtocol.cmd, 9 );
				if (dev->spi_bus_iface)
					dev->spi_bus_iface->spi_bus_receive(dev->spi_bus_obj, (char*)&dev->sendProtocol.head, sizeof(stProtocolFormat));
			}
		}
	}
	else
	{
		debug_printf("[SPI SLAVE FPGA WRITE]=>Addr = %x, Data = %x\n", dev->sendProtocol.addr, dev->sendProtocol.data.dwData);
		if (dev->sendProtocol.data.st.MemPri && dev->sendProtocol.addr == RT3_BASE_ADDR)
			dev->sendProtocol.addr += 0x1000;

		dev->readAddr = dev->sendProtocol.addr;
		if(dev->memory_iface)
			dev->memory_iface->write(dev->memory, dev->sendProtocol.addr, &dev->sendProtocol.data.dwData, 4);
	}
}

static void spi_device_receive(conf_object_t *slave, void *buf, int len)
{
	loongson_spi_slave_device *dev = slave->obj;
	unsigned char data = *(uint8_t *)buf;
	if (data == SPI_HEAD && dev->spiIndex == 0)
	{
			dev->validFrame = 1; //有效数据包
			dev->spiData[dev->spiIndex++] = data;
			return;
	}

	if (dev->validFrame)
	{
		dev->spiData[dev->spiIndex++] = data;

		if (dev->spiIndex == sizeof(stProtocolFormat))
		{
			memcpy(&dev->sendProtocol.head, dev->spiData, dev->spiIndex);
			dev->spiIndex = 0;
			dev->validFrame = 0;
			spi_msg_process(slave);
		}
	}
}

static exception_t spi_bus_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	loongson_spi_slave_device *dev = obj->obj;

	dev->spi_bus_obj = obj2;
	dev->spi_bus_iface = (spi_bus_intf *)SKY_get_iface(obj2, SPI_BUS_INTF_NAME);

	if (dev->spi_bus_iface == NULL){
		if (dev->spi_bus_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SPI_BUS_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SPI_BUS_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t spi_bus_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	return No_exp;
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	loongson_spi_slave_device *dev = obj->obj;
	dev->memory = obj2;
	dev->memory_iface = (memory_space_intf *)SKY_get_iface(dev->memory, MEMORY_SPACE_INTF_NAME);

	if (dev->memory_iface == NULL){
		if (dev->memory == NULL)
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
	loongson_spi_slave_device *dev = obj->obj;
	*obj2 = dev->memory;
	*port = NULL;
	return No_exp;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_loongson_spi_slave(const char *obj_name)
{
	loongson_spi_slave_device* dev = skyeye_mm_zero(sizeof(loongson_spi_slave_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here

	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_loongson_spi_slave(conf_object_t* conf_obj)
{
	loongson_spi_slave_device *dev = (loongson_spi_slave_device *)conf_obj->obj;
	// TODO: place the code here


	No_exp;
}

// Free the device object to do some memory free
static exception_t free_loongson_spi_slave(conf_object_t* conf_obj)
{
	loongson_spi_slave_device *dev = (loongson_spi_slave_device *)conf_obj->obj;
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_loongson_spi_slave(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "loongson_spi_slave",
		.class_desc      = "",
		.new_instance    = new_loongson_spi_slave,
		.free_instance   = free_loongson_spi_slave,
		.config_instance = config_loongson_spi_slave,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);
	
	static spi_device_intf spi_intf = {
		.spi_device_receive = spi_device_receive

	};
	SKY_register_iface(dev_class, SPI_DEVICE_INTF_NAME, &spi_intf);
	
	// list of interface methods
	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = SPI_DEVICE_INTF_NAME,
			.iface = &spi_intf
		}
		
	};
	// list of interface connection methods
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
			(struct ConnectDescription) {
			.name = SPI_BUS_INTF_NAME,
			.set = spi_bus_set,
			.get = spi_bus_get,
			},
			{
			.name  = MEMORY_SPACE_INTF_NAME,
			.set = memory_set,
			.get = memory_get,
		}

	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
}

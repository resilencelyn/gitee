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
#include "loongson_spi_fifo.h"
#include "loongson_spi.h"

static void spi_bus_receive(void *spi_bus, unsigned char *buf, int len)
{
	loongson_spi_device *dev = ((conf_object_t *)spi_bus)->obj;
	struct registers *regs = &dev->regs;
	int i = 0;
	debug_printf("recv data from slave,len:0x%x\n",len);
	//Write last data to fifo
	WriteFIFO(dev->spi_fifo, &dev->regs.data, 1);
	if (len > 0) {
		for (i = 0; i < len; i++) {
			WriteFIFO(dev->spi_fifo, &buf[i], 1);
			debug_printf("%x ", buf[i]);
		}
		debug_printf("\n");
	}

	dev->regs.spsr |= RD_FIFO_EMPTY; //读FIFO为空
}

// Register read operation method implementation
static exception_t loongson_spi_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_spi_device *dev = (loongson_spi_device *)conf_obj->obj;
	uint8_t data = 0;

	switch (offset)
	{
		case SPCR:
			*(uint8_t*)buf = dev->regs.spcr;
			break;
		case SOFTCS:
			*(uint8_t*)buf = dev->regs.softcs;
			break;
		case PARAM:
			*(uint8_t*)buf = dev->regs.param;
			break;
		case SPER:
			*(uint8_t*)buf = dev->regs.sper;
			break;
		case DATA:
			if (CheckFIFOReadNum(dev->spi_fifo) > 0)
			{
				ReadFIFO(dev->spi_fifo, buf, 1);
				//printf("read fifo data = %x\n", *(uint8_t*)buf );
			}
			else
			{
				*(uint8_t*)buf = dev->regs.data;
			}
			dev->regs.spsr |= RD_FIFO_EMPTY; //读FIFO为空
			break;
		case SPSR:
			*(uint8_t*)buf = dev->regs.spsr;
			break;
		case TIMING:
			*(uint8_t*)buf = dev->regs.timing;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_spi\n", offset);
			break;
	}
	//printf("in %s, offset: 0x%x, val: 0x%x \n", __FUNCTION__, offset, *(uint8_t *)buf);
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_spi_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_spi_device *dev = (loongson_spi_device *)conf_obj->obj;
	spi_device_intf *spi_dev_iface;
	conf_object_t *slave_obj;
	uint8_t val = *(uint8_t *)buf;

	debug_printf("in %s, offset: 0x%x, val: 0x%x \n", __FUNCTION__, offset, val);
	switch (offset)
	{
		case SPCR:
			dev->regs.spcr = val;
			break;
		case SOFTCS:
			dev->regs.softcs = val;
			break;
		case PARAM:
			dev->regs.param = val;
			break;
		case SPER:
			dev->regs.sper = val;
			break;
		case DATA:
			dev->regs.data = val;
			spi_dev_iface = dev->slaves[0].spi_dev_iface;
			slave_obj = dev->slaves[0].slave_obj;

			if (slave_obj && spi_dev_iface)
				spi_dev_iface->spi_device_receive(slave_obj, (uint8_t*)buf, 1);

			dev->regs.spsr &= ~RD_FIFO_EMPTY; //读FIFO不为空
			dev->regs.spsr |= WR_FIFO_EMPTY; //写FIFO为空
			break;
		case SPSR:
			dev->regs.spsr = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_spi\n", offset);
			break;
	}
	return No_exp;
}

// Register interface: gets the register name based on the register ID
static char* loongson_spi_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

// Register interface: set register value according to register ID
static exception_t loongson_spi_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	loongson_spi_device *dev = (loongson_spi_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint8_t*)regs + id) = value;
	return No_exp;
}

// Register interface: gets how many registers the device has
static uint32_t loongson_spi_get_regnum(conf_object_t *conf_obj)
{
	return sizeof(struct registers);
}

// Register interface: gets the register value based on the register ID
static uint32_t loongson_spi_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	loongson_spi_device *dev = (loongson_spi_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint8_t* regs_value = (uint8_t*)regs + id;
	return *regs_value;
}

// Register interface: gets the register ID based on the register name
static uint32_t loongson_spi_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers);
	int i;

	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t loongson_spi_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return spi_regs_offset[id];
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_loongson_spi(const char *obj_name)
{
	loongson_spi_device* dev = skyeye_mm_zero(sizeof(loongson_spi_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here

	// Register initialization
	struct registers *regs = &(dev->regs);
	dev->spi_fifo = CreateFIFO(0x1000);

	//init registers
	regs->spsr = RD_FIFO_EMPTY | WR_FIFO_EMPTY;
	regs->spcr =   0x12;
	regs->param =  0x01;
	regs->softcs = 0xf0;
	regs->timing = 0x03;

	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_loongson_spi(conf_object_t* conf_obj)
{
	loongson_spi_device *dev = (loongson_spi_device *)conf_obj->obj;
	// TODO: place the code here

	No_exp;
}

// Free the device object to do some memory free
static exception_t free_loongson_spi(conf_object_t* conf_obj)
{
	loongson_spi_device *dev = (loongson_spi_device *)conf_obj->obj;
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

static exception_t spi_device_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	loongson_spi_device *dev = obj->obj;

	dev->slaves[dev->slave_index].spi_dev_iface = (spi_device_intf *)SKY_get_iface(obj2, SPI_DEVICE_INTF_NAME);
	dev->slaves[dev->slave_index].slave_obj = obj2;

	if (dev->slaves[dev->slave_index].spi_dev_iface == NULL){
		if (dev->slaves[dev->slave_index].slave_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SPI_DEVICE_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SPI_DEVICE_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	dev->slave_index ++;
	return No_exp;
}

static exception_t spi_device_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	return No_exp;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_loongson_spi(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "loongson_spi",
		.class_desc      = "",
		.new_instance    = new_loongson_spi,
		.free_instance   = free_loongson_spi,
		.config_instance = config_loongson_spi,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);
	
	// sets the register address read-write method
	static memory_space_intf io_memory_interface =
	{
		.read  = loongson_spi_read,
		.write = loongson_spi_write
	};
	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);
	
	static spi_bus_intf spi_bus_intf = {
		.spi_bus_receive = spi_bus_receive
	};
	SKY_register_iface(dev_class, SPI_BUS_INTF_NAME, &spi_bus_intf);
	
	// register access interface (for SkyEye graphical control interface)
	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = loongson_spi_get_regvalue_by_id,
		.get_regname_by_id  = loongson_spi_get_regname_by_id,
		.set_regvalue_by_id = loongson_spi_set_regvalue_by_id,
		.get_regnum         = loongson_spi_get_regnum,
		.get_regid_by_name  = loongson_spi_get_regid_by_name,
		.get_regoffset_by_id = loongson_spi_get_regoffset_by_id
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
		(struct InterfaceDescription)
		{
			.name = SPI_BUS_INTF_NAME,
			.iface = &spi_bus_intf
		}
	};
	
	// list of interface connection methods
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = SPI_DEVICE_INTF_NAME,
			.set = spi_device_set,
			.get = spi_device_get,
		}
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
}

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
#include "at697f_spi_fifo.h"
#include "at697f_spi.h"

static void spi_bus_receive(void *spi_bus, unsigned char *buf, int len)
{
	at697f_spi_device *dev = ((conf_object_t *)spi_bus)->obj;
	struct registers *regs = &dev->regs;
	int i = 0;
	//printf("recv data from slave,len:0x%x\n",len);
	if (len > 0) {
		for (i = 0; i < len; i++) {
			WriteFIFO(dev->spi_fifo, &buf[i], 1);
		//	printf("%x ", buf[i]);
		}
	//	printf("\n");
	}
	
}

// Register read operation method implementation
static exception_t at697f_spi_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	at697f_spi_device *dev = (at697f_spi_device *)conf_obj->obj;
	unsigned char data;
	switch (offset)
	{
		case CTRL:
			*(uint32_t *)buf = dev->regs.ctrl;
			break;
		case INTMSK:
			*(uint32_t *)buf = dev->regs.intmsk;
			break;
		case DATA_REV:
			//if (CheckFIFOReadNum(dev->spi_fifo) > 0)
			{
				ReadFIFO(dev->spi_fifo, &data, 1);
			//	printf("[read fifo : 0x%x, 0x%x] \n", data, dev->regs.data_rev);
				dev->regs.data_rev = data;
				*(uint32_t *)buf = dev->regs.data_rev;
			}
			break;
		case DATA_SEND:
			*(uint32_t *)buf = dev->regs.data_send;
			break;
		case CMD:
			*(uint32_t *)buf = dev->regs.cmd;
			break;
		case STATUS:
			*(uint32_t *)buf = dev->regs.status;
			break;
		case CHIPS:
			*(uint32_t *)buf = dev->regs.chips;
			break;
		case ATX:
			*(uint32_t *)buf = dev->regs.atx;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in at697f_spi\n", offset);
			break;
	}
//	printf("in %s, offset: 0x%x, val: 0x%x \n", __FUNCTION__, offset, *(uint32_t *)buf);
	return No_exp;
}

// Register write operation method implementation
static exception_t at697f_spi_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	at697f_spi_device *dev = (at697f_spi_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;
	conf_object_t *slave_obj;
	spi_device_intf *spi_dev_iface;
	//printf("in %s, offset: 0x%x, val: 0x%x \n", __FUNCTION__, offset, val);
	switch (offset)
	{
		case CTRL:
			dev->regs.ctrl = val;
			break;
		case INTMSK:
			dev->regs.intmsk = val;
			break;
		case DATA_REV:
			dev->regs.data_rev = val;
			break;
		case DATA_SEND:
			dev->regs.data_send = val;

			if (dev->regs.cmd == 0x2)
			{
				slave_obj = dev->slaves[dev->slave_index].slave_obj;
				spi_dev_iface = dev->slaves[dev->slave_index].spi_dev_iface;
			
				if (slave_obj && spi_dev_iface)
					spi_dev_iface->spi_device_receive(slave_obj, (uint8_t*)buf, 1);
			}
			break;
		case CMD:
			dev->regs.cmd = val;
			if (val == 0x2)
			{
				dev->index = 0;
			}
			break;
		case STATUS:
			dev->regs.status = val;
			
			break;
		case CHIPS:
			dev->regs.chips = val;
			if (val == 0x1)
				dev->slave_index = 0;
			else if (val == 0x4)
				dev->slave_index = 2;
			else if (val == 0x8)
				dev->slave_index = 3;
			else if (val == 0x80)
				dev->slave_index = 1;
			else
				dev->slave_index = val;
			break;
		case ATX:
			dev->regs.atx = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in at697f_spi\n", offset);
			break;
	}
	return No_exp;
}

// Register interface: gets the register name based on the register ID
static char* at697f_spi_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

// Register interface: set register value according to register ID
static exception_t at697f_spi_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	at697f_spi_device *dev = (at697f_spi_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

// Register interface: gets how many registers the device has
static uint32_t at697f_spi_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

// Register interface: gets the register value based on the register ID
static uint32_t at697f_spi_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	at697f_spi_device *dev = (at697f_spi_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

// Register interface: gets the register ID based on the register name
static uint32_t at697f_spi_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_at697f_spi(const char *obj_name)
{
	at697f_spi_device* dev = skyeye_mm_zero(sizeof(at697f_spi_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here
	// Register initialization
	struct registers *regs = &(dev->regs);
	
	dev->spi_fifo = CreateFIFO(0x40000);
	dev->slave_index = 0;
	RWLOCK_INIT(dev->lock);
	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_at697f_spi(conf_object_t* conf_obj)
{
	at697f_spi_device *dev = (at697f_spi_device *)conf_obj->obj;
	// TODO: place the code here


	No_exp;
}

// Free the device object to do some memory free
static exception_t free_at697f_spi(conf_object_t* conf_obj)
{
	at697f_spi_device *dev = (at697f_spi_device *)conf_obj->obj;
	FreeFIFO(dev->spi_fifo);
	RWLOCK_DESTROY(dev->lock);
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	at697f_spi_device *dev =  obj->obj;
	dev->signal.obj = obj2;
	dev->signal.intf = (general_signal_intf *)SKY_get_iface(dev->signal.obj, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal.intf == NULL){
		if (dev->signal.obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	at697f_spi_device *dev =  obj->obj;
	*obj2 = dev->signal.obj;
	*port = NULL;
	return No_exp;
}

static exception_t spi_device_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	at697f_spi_device *dev = obj->obj;

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
void init_at697f_spi(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "at697f_spi",
		.class_desc      = "at697f_spi",
		.new_instance    = new_at697f_spi,
		.free_instance   = free_at697f_spi,
		.config_instance = config_at697f_spi,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);
	
	// sets the register address read-write method
	static memory_space_intf io_memory_interface =
	{
		.read  = at697f_spi_read,
		.write = at697f_spi_write
	};
	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);
	
	// register access interface (for SkyEye graphical control interface)
	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = at697f_spi_get_regvalue_by_id,
		.get_regname_by_id  = at697f_spi_get_regname_by_id,
		.set_regvalue_by_id = at697f_spi_set_regvalue_by_id,
		.get_regnum         = at697f_spi_get_regnum,
		.get_regid_by_name  = at697f_spi_get_regid_by_name
	};
	SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);
	
	static spi_bus_intf spi_bus_intf = {
		.spi_bus_receive = spi_bus_receive
	};
	SKY_register_iface(dev_class, SPI_BUS_INTF_NAME, &spi_bus_intf);

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
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = signal_set,
			.get = signal_get,
		},
		(struct ConnectDescription) {
			.name = SPI_DEVICE_INTF_NAME,
			.set = spi_device_set,
			.get = spi_device_get,
		}
		
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
}

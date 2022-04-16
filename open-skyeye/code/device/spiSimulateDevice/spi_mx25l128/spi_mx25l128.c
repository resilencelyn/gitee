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
#include <semaphore.h>
#define DEBUG
#include <skyeye_log.h>
#include "spi_mx25l128.h"

#define MAXSIZE 1024

static conf_object_t* new_spi_mx25l128(char *obj_name)
{
	spi_mx25l128_device* dev = skyeye_mm_zero(sizeof(spi_mx25l128_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->mx25l128 = init_mx25l128();
	if (dev->mx25l128 == NULL)
		exit(-1);

	dev->tx_data.buf = skyeye_mm_zero(MAXSIZE - sizeof(data_buf_t));
	dev->tx_data.pos = 0;

	return dev->obj;
}

static exception_t free_spi_mx25l128(conf_object_t* opaque)
{
	spi_mx25l128_device *dev = opaque->obj;

	free_mx25l128(dev->mx25l128);

	skyeye_free(dev->tx_data.buf);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t spi_bus_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	spi_mx25l128_device *dev = obj->obj;

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

static void spi_device_receive(conf_object_t *slave, char *buf, int len)
{
	spi_mx25l128_device *dev = slave->obj;
	char  *data = (char *)dev->tx_data.buf;
	int index;

	for (index = 0; index < len; index++) {
		if (dev->tx_data.pos >= MAXSIZE) {
			skyeye_log(Error_log, __FUNCTION__, "Spi buff overflow:%s.", SPI_BUS_INTF_NAME, slave->objname);
			break;
		}
		data[dev->tx_data.pos] = buf[index];
		dev->tx_data.pos++;
		dev->mx25l128->data.length = dev->tx_data.pos;
		if (dev->is_read_action) {
			dev->mx25l128->is_poll_mode = 1;
			sem_post(&dev->mx25l128->sem);
		}
	}
}

static void spi_device_receive_done(conf_object_t *slave)
{
	spi_mx25l128_device *dev = slave->obj;
	data_buf_t *buf = dev->tx_data.buf;
	mx25l128_t *mx25l128 = dev->mx25l128;
	uint32_t addr = 0;
	char *paddr = (char *)&addr;

	dev->is_read_action = 0;

	switch (buf->cmd) {
		case WREN:
			mx25l128->wren_action(mx25l128,
					     dev->spi_bus_iface->spi_bus_receive,
					     dev->spi_bus_obj);
			break;
		case WRDI:
			mx25l128->wrdi_action(mx25l128,
					     dev->spi_bus_iface->spi_bus_receive,
					     dev->spi_bus_obj);
			break;
		case RDID:
			mx25l128->rdid_action(mx25l128,
					     dev->spi_bus_iface->spi_bus_receive,
					     dev->spi_bus_obj);
			break;
		case RDSR:
			mx25l128->rdsr_action(mx25l128,
					     dev->spi_bus_iface->spi_bus_receive,
					     dev->spi_bus_obj);
			break;
		case RDSR1:
			mx25l128->rdsr1_action(mx25l128,
					     dev->spi_bus_iface->spi_bus_receive,
					     dev->spi_bus_obj);
			break;
		case WRSR:
			mx25l128->wrsr_action(mx25l128,
					     buf->data[0],
					     dev->spi_bus_iface->spi_bus_receive,
					     dev->spi_bus_obj);
			break;
		case READ:
			dev->is_read_action = 1;
			/* MSB */
			paddr[0] = buf->addr[2];
			paddr[1] = buf->addr[1];
			paddr[2] = buf->addr[0];
			mx25l128->read_action(mx25l128,
					     addr,
					     dev->spi_bus_iface->spi_bus_receive,
					     dev->spi_bus_obj);
			break;
		case FAST_READ:
			dev->is_read_action = 1;
			/* MSB */
			paddr[0] = buf->addr[2];
			paddr[1] = buf->addr[1];
			paddr[2] = buf->addr[0];
			mx25l128->fast_read_action(mx25l128,
						  buf->dummy_byte,
					          addr,
						  dev->spi_bus_iface->spi_bus_receive,
						  dev->spi_bus_obj);
			break;
		case PP:
			/* MSB */
			paddr[0] = buf->addr[2];
			paddr[1] = buf->addr[1];
			paddr[2] = buf->addr[0];
			mx25l128->pp_action(mx25l128,
					   addr,
					   buf->data + 3,
					   dev->tx_data.pos - 4,
					   dev->spi_bus_iface->spi_bus_receive,
					   dev->spi_bus_obj);
			break;
		case SE:
			/* MSB */
			paddr[0] = buf->addr[2];
			paddr[1] = buf->addr[1];
			paddr[2] = buf->addr[0];
			mx25l128->se_action(mx25l128,
					   addr,
					   dev->spi_bus_iface->spi_bus_receive,
					   dev->spi_bus_obj);
			break;
		case BE:
			mx25l128->be_action(mx25l128,
					   dev->spi_bus_iface->spi_bus_receive,
					   dev->spi_bus_obj);
			break;
		default:
			break;
	}
#if 0
	int i;
	printf("===============begin===================\n");
	printf("CMD: %02x\tLEN: %d\n", (unsigned char)buf->cmd, dev->tx_data.pos);
	for (i = 0; i < dev->tx_data.pos - 1; i++)
		printf("%02x ", (unsigned char)buf->data[i]);
	printf("\n");
	printf("===============end===================\n");
#endif
	dev->tx_data.pos = 0;
	memset(dev->tx_data.buf, 0, MAXSIZE);
}
static void spi_deassert_cs_line(conf_object_t *slave)
{
	spi_mx25l128_device *dev = slave->obj;
	dev->mx25l128->set_cs_line(dev->mx25l128, 0);
}
static void spi_assert_cs_line(conf_object_t *slave)
{
	spi_mx25l128_device *dev = slave->obj;
	dev->mx25l128->set_cs_line(dev->mx25l128, 1);
}
static int spi_get_cs_line(conf_object_t *slave)
{
	spi_mx25l128_device *dev = slave->obj;
	return dev->mx25l128->get_cs_line(dev->mx25l128);
}

static attr_value_t get_attr_flash_type(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	spi_mx25l128_device *dev = conf_obj->obj;
	attr_value_t flash_type = SKY_make_attr_uinteger(dev->mx25l128->flash_type);
	return flash_type;
}

static exception_t set_attr_flash_type(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	spi_mx25l128_device *dev =  conf_obj->obj;
	dev->mx25l128->flash_type = SKY_attr_uinteger(*value);
	if (dev->mx25l128->flash_type == MX25L12805D)
	{
		dev->mx25l128->flash_id[0] = 0x20;
		dev->mx25l128->flash_id[1] = 0x18;
	} else
	{
		dev->mx25l128->flash_id[0] = 0x26;
		dev->mx25l128->flash_id[1] = 0x18;
	}
	return No_exp;
}

static attr_value_t get_attr_filename(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	spi_mx25l128_device* dev = conf_obj->obj;
	attr_value_t value = SKY_make_attr_string(dev->mx25l128->filename);
	return value;
}

static exception_t set_attr_filename(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	spi_mx25l128_device* dev = conf_obj->obj;
	char *filename;
	filename = SKY_attr_string(*value);
	strncpy(dev->mx25l128->filename, filename, MAX_FILE_NAME_LEN);
	dev->mx25l128->fp = fopen(dev->mx25l128->filename, "wb+");
	if(dev->mx25l128->fp == NULL){
		skyeye_log(Error_log, __FUNCTION__, "can not open file: %s\n", dev->mx25l128->filename);
	}
	return No_exp;
}

static void spi_mx25l128_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "flash_type", get_attr_flash_type, NULL, set_attr_flash_type, NULL, SKY_Attr_Optional, "uinteger", "set ID of flash");
	SKY_register_attribute(clss, "filename", get_attr_filename, NULL, set_attr_filename, NULL, SKY_Attr_Optional, "string", "flash file name");
	return;
}

void init_spi_mx25l128(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "spi_mx25l128",
		.class_desc = "spi_mx25l128",
		.new_instance = new_spi_mx25l128,
		.free_instance = free_spi_mx25l128,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static spi_device_intf spi_intf = {
		.spi_device_receive = spi_device_receive,
		.spi_device_receive_done = spi_device_receive_done,
		.spi_deassert_cs_line = spi_deassert_cs_line,
		.spi_assert_cs_line = spi_assert_cs_line,
		.spi_get_cs_line = spi_get_cs_line,
	};
	SKY_register_iface(clss, SPI_DEVICE_INTF_NAME, &spi_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription)
		{
			.name = SPI_DEVICE_INTF_NAME,
			.iface = &spi_intf
		}
	};

	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = SPI_BUS_INTF_NAME,
			.set = spi_bus_set,
			.get = spi_bus_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	spi_mx25l128_register_attribute(clss);
}

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
#include "spi_m25p128.h"

#define MAXSIZE 1024

static conf_object_t* new_spi_m25p128(char *obj_name)
{
	spi_m25p128_device* dev = skyeye_mm_zero(sizeof(spi_m25p128_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->m25p128 = init_m25p128();
	if (dev->m25p128 == NULL)
		exit(-1);

	dev->tx_data.buf = skyeye_mm_zero(MAXSIZE - sizeof(data_buf_t));
	dev->tx_data.pos = 0;

	return dev->obj;
}
static exception_t free_spi_m25p128(conf_object_t* opaque)
{
	spi_m25p128_device *dev = opaque->obj;

	free_m25p128(dev->m25p128);

	skyeye_free(dev->tx_data.buf);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
static exception_t spi_bus_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	spi_m25p128_device *dev = obj->obj;

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
	spi_m25p128_device *dev = slave->obj;
	char  *data = (char *)dev->tx_data.buf;
	int index;

	for (index = 0; index < len; index++) {
		if (dev->tx_data.pos >= MAXSIZE) {
			skyeye_log(Error_log, __FUNCTION__, "Spi buff overflow:%s.", SPI_BUS_INTF_NAME, slave->objname);
			break;
		}
		data[dev->tx_data.pos] = buf[index];
		dev->tx_data.pos++;
		dev->m25p128->data.length = dev->tx_data.pos;
		if (dev->is_read_action) {
			dev->m25p128->is_poll_mode = 1;
			sem_post(&dev->m25p128->sem);
		}
	}
}
static void spi_device_receive_done(conf_object_t *slave)
{
	spi_m25p128_device *dev = slave->obj;
	data_buf_t *buf = dev->tx_data.buf;
	m25p128_t *m25p128 = dev->m25p128;
	uint32_t addr = 0;
	char *paddr = (char *)&addr;

	dev->is_read_action = 0;

	switch (buf->cmd) {
		case WREN:
			m25p128->wren_action(m25p128,
					     dev->spi_bus_iface->spi_bus_receive,
					     dev->spi_bus_obj);
			break;
		case WRDI:
			m25p128->wrdi_action(m25p128,
					     dev->spi_bus_iface->spi_bus_receive,
					     dev->spi_bus_obj);
			break;
		case RDID:
			m25p128->rdid_action(m25p128,
					     dev->spi_bus_iface->spi_bus_receive,
					     dev->spi_bus_obj);
			break;
		case RDSR:
			m25p128->rdsr_action(m25p128,
					     dev->spi_bus_iface->spi_bus_receive,
					     dev->spi_bus_obj);
			break;
		case WRSR:
			m25p128->wrsr_action(m25p128,
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
			m25p128->read_action(m25p128,
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
			m25p128->fast_read_action(m25p128,
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
			m25p128->pp_action(m25p128,
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
			m25p128->se_action(m25p128,
					   addr,
					   dev->spi_bus_iface->spi_bus_receive,
					   dev->spi_bus_obj);
			break;
		case BE:
			m25p128->be_action(m25p128,
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
	spi_m25p128_device *dev = slave->obj;
	dev->m25p128->set_cs_line(dev->m25p128, 0);
}
static void spi_assert_cs_line(conf_object_t *slave)
{
	spi_m25p128_device *dev = slave->obj;
	dev->m25p128->set_cs_line(dev->m25p128, 1);
}
static int spi_get_cs_line(conf_object_t *slave)
{
	spi_m25p128_device *dev = slave->obj;
	return dev->m25p128->get_cs_line(dev->m25p128);
}
void init_spi_m25p128(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "spi_m25p128",
		.class_desc = "spi_m25p128",
		.new_instance = new_spi_m25p128,
		.free_instance = free_spi_m25p128,
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
}

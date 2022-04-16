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
#include "spi_omapl138.h"
#include "omapl138_spi_fifo.h"

static uint32_t get_format_reg(conf_object_t *opaque)
{
	spi_omapl138_device *dev = opaque->obj;
	/* slave mode only support spifmt0 */
	if ((dev->regs.spigcr1 & 0x3) == 0x0)
		return dev->regs.spifmt0;

	switch ((dev->regs.spidat1 >> 24) & 0x3)
	{
		case 0: return dev->regs.spifmt0;
		case 1: return dev->regs.spifmt1;
		case 2: return dev->regs.spifmt2;
		case 3: return dev->regs.spifmt3;
	}
	return 0;
}

/* length unit is bit */
static int get_data_length(uint32_t spifmt)
{
	int len;

	len = spifmt & 0x1f;
	len = len < 2 ? 8 : len > 16 ? 16 : len;
	return len;
}

static void dbg_spi_data(conf_object_t *opaque, uint32_t spidat)
{
	spi_omapl138_device *dev = opaque->obj;
	uint32_t spifmt, data;
	int len, i;
	char *p = (char *)&spidat;

	spifmt = get_format_reg(opaque);
	len = get_data_length(spifmt);
	printf("--------------------spi transmit begin dbg-----------\n");
	printf("bits: %d\n", len);

	for (i = 0; i < len / 8; i++)
	{
		printf("%02x ", (unsigned char)p[i]);
	}
	printf("\n");
	printf("--------------------spi transmit begin end-----------\n");
}

static int send_data_to_spi_device(conf_object_t *opaque, uint32_t spidat)
{
	spi_omapl138_device *dev = opaque->obj;
	uint32_t spifmt, data;
	int len;
	char *p = (char *)&spidat;

	conf_object_t *slave_obj = dev->slaves[dev->index].slave_obj;
	spi_device_intf *spi_dev_iface = dev->slaves[dev->index].spi_dev_iface;

	spifmt = get_format_reg(opaque);
	len = get_data_length(spifmt);

	if (slave_obj && spi_dev_iface)
		spi_dev_iface->spi_device_receive(slave_obj, p, len / 8);

	return 0;
}

static int notify_transmit_done(conf_object_t *opaque)
{
	spi_omapl138_device *dev = opaque->obj;
	conf_object_t *slave_obj = dev->slaves[dev->index].slave_obj;
	spi_device_intf *spi_dev_iface = dev->slaves[dev->index].spi_dev_iface;

	if (slave_obj && spi_dev_iface)
		spi_dev_iface->spi_device_receive_done(slave_obj);
	return 0;
}

static void deassert_cs_line(conf_object_t *opaque)
{
	spi_omapl138_device *dev = opaque->obj;
	conf_object_t *slave_obj;
	spi_device_intf *spi_dev_iface;

	slave_obj = dev->slaves[dev->index].slave_obj;
	spi_dev_iface = dev->slaves[dev->index].spi_dev_iface;

	if (spi_dev_iface && slave_obj)
		spi_dev_iface->spi_deassert_cs_line(slave_obj);
}

static void assert_cs_line(conf_object_t *opaque)
{
	spi_omapl138_device *dev = opaque->obj;
	conf_object_t *slave_obj;
	spi_device_intf *spi_dev_iface;

	slave_obj = dev->slaves[dev->index].slave_obj;
	spi_dev_iface = dev->slaves[dev->index].spi_dev_iface;

	if (spi_dev_iface && slave_obj)
		spi_dev_iface->spi_assert_cs_line(slave_obj);
}

static void check_cs_line(conf_object_t *opaque)
{
	spi_omapl138_device *dev = opaque->obj;
	conf_object_t *slave_obj;
	spi_device_intf *spi_dev_iface;

	/* if CSHOLD bit is zero, then deassert cs line */
	if ((dev->regs.spidat1 & (1 << 28)) == 0)
	{
		slave_obj = dev->slaves[dev->index].slave_obj;
		spi_dev_iface = dev->slaves[dev->index].spi_dev_iface;

		if (spi_dev_iface && slave_obj)
			spi_dev_iface->spi_deassert_cs_line(slave_obj);
	}
}

static int start_transmit(conf_object_t *opaque, uint32_t spidat)
{
	spi_omapl138_device *dev = opaque->obj;

	RW_WRLOCK(dev->lock);
	dev->regs.spiflg &= ~(1 << 9);
	dev->regs.intvec1 = 0;
	RW_UNLOCK(dev->lock);

	if ((dev->regs.spigcr1 & 0x3) == 0x3)
	{
		/* master mode, spidata & 0xff is real data, begin
		 * transmit here we should get data length and dir
		 */
		/* enable cs line */
		assert_cs_line(opaque);
		/* send data to spi device */
		send_data_to_spi_device(opaque, spidat);
		/* after transmit data, we diasble cs line by cshold */
		check_cs_line(opaque);
		if (dev->regs.spiint0 & (1 << 9))
		{
			/* raise signal */
			RW_WRLOCK(dev->lock);
			dev->regs.spiflg |= 1 << 9;
			dev->regs.intvec1 = 0x14 << 1;
			RW_UNLOCK(dev->lock);

			if (dev->signal_iface != NULL)
			{
				dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);
			}
		}
	} else {
		/* FIXME:slave mode */
		/* spidata & 0xff is real data, begin transmit */
		if (dev->regs.spiint0 & (1 << 9))
		{
			//dbg_spi_data(opaque, spidat);
			/* raise signal */
			RW_WRLOCK(dev->lock);
			dev->regs.spiflg |= 1 << 9;
			dev->regs.intvec1 = 0x14 << 1;
			RW_UNLOCK(dev->lock);

			if (dev->signal_iface != NULL)
			{
				dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);
			}
		}
	}
}

static int get_slave_index(uint32_t spidef)
{
	int i;

	for (i = 0; i < 8; i++)
	{
		if (spidef & (1 << i))
			return i;
	}
}

static exception_t spi_omapl138_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	spi_omapl138_device *dev = opaque->obj;
	switch (offset)
	{
		case SPIGCR0:
			*(uint32_t *)buf = dev->regs.spigcr0;
			break;
		case SPIGCR1:
			*(uint32_t *)buf = dev->regs.spigcr1;
			break;
		case SPIINT0:
			*(uint32_t *)buf = dev->regs.spiint0;
			break;
		case SPILVL:
			*(uint32_t *)buf = dev->regs.spilvl;
			break;
		case SPIFLG:
			*(uint32_t *)buf = dev->regs.spiflg;
			break;
		case SPIPC0:
			*(uint32_t *)buf = dev->regs.spipc0;
			break;
		case SPIPC1:
			*(uint32_t *)buf = dev->regs.spipc1;
			break;
		case SPIPC2:
			*(uint32_t *)buf = dev->regs.spipc2;
			break;
		case SPIPC3:
			*(uint32_t *)buf = dev->regs.spipc3;
			break;
		case SPIPC4:
			*(uint32_t *)buf = dev->regs.spipc4;
			break;
		case SPIPC5:
			*(uint32_t *)buf = dev->regs.spipc5;
			break;
		case SPIDAT0:
			*(uint32_t *)buf = dev->regs.spidat0;
			break;
		case SPIDAT1:
			*(uint32_t *)buf = dev->regs.spidat1;
			break;
		case SPIBUF:
			if (CheckCanReadNum(dev->spi_fifo) > 0)
			{
				char ch;

				ReadFIFO(dev->spi_fifo, &ch, 1);
				dev->regs.spibuf &= ~0xffff;
				dev->regs.spibuf |= 0xff & ch;

				*(uint32_t *)buf = dev->regs.spibuf;
				if (CheckCanReadNum(dev->spi_fifo) <= 0)
					dev->regs.spiflg &= ~(1 << 8);
			} else {
				*(uint32_t *)buf = dev->regs.spibuf;
				dev->regs.spiflg &= ~(1 << 8);
			}
			break;
		case SPIEMU:
			*(uint32_t *)buf = dev->regs.spiemu;
			break;
		case SPIDELAY:
			*(uint32_t *)buf = dev->regs.spidelay;
			break;
		case SPIDEF:
			*(uint32_t *)buf = dev->regs.spidef;
			break;
		case SPIFMT0:
			*(uint32_t *)buf = dev->regs.spifmt0;
			break;
		case SPIFMT1:
			*(uint32_t *)buf = dev->regs.spifmt1;
			break;
		case SPIFMT2:
			*(uint32_t *)buf = dev->regs.spifmt2;
			break;
		case SPIFMT3:
			*(uint32_t *)buf = dev->regs.spifmt3;
			break;
		case INTVEC1:
			RW_WRLOCK(dev->lock);
			*(uint32_t *)buf = dev->regs.intvec1;
			if (((dev->regs.intvec1 >> 1) & 0x1f) == 0x12)
			{
				dev->regs.spiflg &= ~(1 << 8);
				dev->regs.spiflg &= ~(1 << 6);
			}
			RW_UNLOCK(dev->lock);
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in spi_omapl138\n", offset);
			break;
	}
	return 0;
}

static exception_t spi_omapl138_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	spi_omapl138_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	uint32_t spiint0, spidat1;

	/* support byte access for register */
	if (offset >SPIDAT1 && offset < SPIBUF && count == 1)
	{
		char *pspidata1 = (char *)&dev->regs.spidat1;
		uint32_t off = offset - SPIDAT1;

		spidat1 = dev->regs.spidat1;
		*(pspidata1 + off) = val;

		/* deassert cs line, stop translate and receive */
		if ((spidat1 & (1 << 28)) && ((dev->regs.spidat1 & (1 << 28)) == 0))
		{
			deassert_cs_line(opaque);
		}
		return No_exp;
	}

	switch (offset)
	{
		case SPIGCR0:
			dev->regs.spigcr0 = val;
			if ((dev->regs.spigcr0 & 0x1) == 0)
			{
				memset(&dev->regs, 0, sizeof(dev->regs));
				dev->regs.spidef = 0xff;
			}
			break;
		case SPIGCR1:
			dev->regs.spigcr1 = val;
			if ((val & (1 << 24)) == 0)
			{
				dev->regs.spidat0 = 0;
				dev->regs.spidat1 &= ~0xff;

				RW_WRLOCK(dev->lock);
				dev->regs.spiflg &= ~(1 << 9);
				dev->regs.spiflg &= ~(1 << 8);
				dev->regs.spiflg &= ~(1 << 4);
				dev->regs.spiflg &= ~(1 << 3);
				dev->regs.spiflg &= ~(1 << 2);
				dev->regs.spiflg &= ~(1 << 1);
				dev->regs.intvec1 = 0;
				RW_UNLOCK(dev->lock);
			}
			break;
		case SPIINT0:
			spiint0 = dev->regs.spiint0;
			dev->regs.spiint0 = val;
			/* because begin, we should raise signal to active transmit */
			if ((dev->regs.spiint0 & (1 << 9)) && (spiint0 & (1 << 9)) == 0)
			{
				RW_WRLOCK(dev->lock);
				dev->regs.spiflg |= 1 << 9;
				dev->regs.intvec1 = 0x14 << 1;
				RW_UNLOCK(dev->lock);

				if (dev->signal_iface != NULL)
				{
					dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);
				}
			}
			/* DMA active transmit */
			if ((dev->regs.spiint0 & (1 << 16)) && (spiint0 & (1 << 16)) == 0)
			{
				RW_WRLOCK(dev->lock);
				dev->regs.spiflg |= 1 << 9;
				dev->regs.intvec1 = 0x14 << 1;
				RW_UNLOCK(dev->lock);

				if (dev->signal_iface != NULL)
				{
					dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);
				}
			}
			/* transmit done */
			if (((dev->regs.spiint0 & (1 << 9)) == 0) && (spiint0 & (1 << 9)))
			{
				RW_WRLOCK(dev->lock);
				dev->regs.intvec1 = 0;
				dev->regs.spiflg &= ~(1 << 9);
				RW_UNLOCK(dev->lock);

				notify_transmit_done(opaque);
			}
			/*DMA Mode transmit done */
			if (((dev->regs.spiint0 & (1 << 16)) == 0) && (spiint0 & (1 << 16)))
			{
				RW_WRLOCK(dev->lock);
				dev->regs.intvec1 = 0;
				dev->regs.spiflg &= ~(1 << 9);
				RW_UNLOCK(dev->lock);

				notify_transmit_done(opaque);
			}
			/* receive done */
			if (((dev->regs.spiint0 & (1 << 8)) == 0) && (spiint0 & (1 << 8)))
			{
				RW_WRLOCK(dev->lock);
				dev->regs.spiflg &= ~(1 << 8);
				dev->regs.intvec1 = 0;
				RW_UNLOCK(dev->lock);
			}
			break;
		case SPILVL:
			dev->regs.spilvl = val;
			break;
		case SPIFLG:
			RW_WRLOCK(dev->lock);
			dev->regs.spiflg &= ~val;
			dev->regs.intvec1 = 0;
			RW_UNLOCK(dev->lock);
			break;
		case SPIPC0:
			dev->regs.spipc0 = val;
			break;
		case SPIPC1:
			dev->regs.spipc1 = val;
			break;
		case SPIPC2:
			dev->regs.spipc2 = val;
			break;
		case SPIPC3:
			dev->regs.spipc3 = val;
			break;
		case SPIPC4:
			dev->regs.spipc4 = val;
			dev->regs.spipc3 |= val;
			break;
		case SPIPC5:
			dev->regs.spipc5 = val;
			dev->regs.spipc3 &= ~val;
			break;
		case SPIDAT0:
			dev->regs.spidat0 = val;
			if (dev->regs.spigcr1 & (1 << 24))
			{
				/* enable, now we start transmit */
				start_transmit(opaque, dev->regs.spidat0);
			}
			break;
		case SPIDAT1:
			dev->regs.spidat1 = val;
			if (dev->regs.spigcr1 & (1 << 24))
			{
				/* enable, now we start transmit, only write
				 * spidat1[0:15] can casuse new transmit */
				start_transmit(opaque, dev->regs.spidat1);
			}
			break;
		case SPIBUF:
			dev->regs.spibuf = val;
			break;
		case SPIEMU:
			dev->regs.spiemu = val;
			break;
		case SPIDELAY:
			dev->regs.spidelay = val;
			break;
		case SPIDEF:
			dev->regs.spidef = val;
			dev->index = get_slave_index(val);
			break;
		case SPIFMT0:
			dev->regs.spifmt0 = val;
			break;
		case SPIFMT1:
			dev->regs.spifmt1 = val;
			break;
		case SPIFMT2:
			dev->regs.spifmt2 = val;
			break;
		case SPIFMT3:
			dev->regs.spifmt3 = val;
			break;
		case INTVEC1:
			RW_WRLOCK(dev->lock);
			dev->regs.intvec1 = val;
			RW_UNLOCK(dev->lock);
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in spi_omapl138\n", offset);
			break;
	}
	return 0;
}

static char* spi_omapl138_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t spi_omapl138_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	spi_omapl138_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t spi_omapl138_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t spi_omapl138_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	spi_omapl138_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t spi_omapl138_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_spi_omapl138(char *obj_name)
{
	spi_omapl138_device* dev = skyeye_mm_zero(sizeof(spi_omapl138_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	regs->spidef = 0xff;
	dev->index = -1;
	dev->spi_fifo = CreateFIFO(1024);
	RWLOCK_INIT(dev->lock);
	return dev->obj;
}

static exception_t free_spi_omapl138(conf_object_t* opaque)
{
	spi_omapl138_device *dev = opaque->obj;

	FreeFIFO(dev->spi_fifo);
	RWLOCK_DESTROY(dev->lock);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t spi_device_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	spi_omapl138_device *dev = obj->obj;

	if (index >= MAX_SLAVES || index < 0)
		return No_exp;

	dev->slaves[index].spi_dev_iface = (spi_device_intf *)SKY_get_iface(obj2, SPI_DEVICE_INTF_NAME);
	dev->slaves[index].slave_obj = obj2;

	if (dev->slaves[index].spi_dev_iface == NULL)
	{
		if (dev->slaves[index].slave_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SPI_DEVICE_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SPI_DEVICE_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t spi_device_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	return No_exp;
}

static void spi_bus_receive(void *spi_bus, char *buf, int len)
{
	spi_omapl138_device *dev = ((conf_object_t *)spi_bus)->obj;
	int i = 0;

	if (len > 0) {
		for (i = 0; i < len; i++)
		{
			WriteFIFO(dev->spi_fifo, &buf[i], 1);
			if ((dev->regs.spiint0 & (1 << 8)) || (dev->regs.spiint0 & (1 << 16)))
			{
				RW_WRLOCK(dev->lock);
				dev->regs.spiflg |= 1 << 8;
				dev->regs.intvec1 = 0x12 << 1;
				RW_UNLOCK(dev->lock);

				if (dev->signal_iface != NULL)
				{
					dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);
				}
			}
		}

		return;
	}
	
	if ((dev->regs.spiint0 & (1 << 8)) && (len == 0))
	{
		RW_WRLOCK(dev->lock);
		dev->regs.spiflg |= 1 << 8;
		dev->regs.intvec1 = 0x12 << 1;
		RW_UNLOCK(dev->lock);

		if (dev->signal_iface != NULL)
		{
			dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);
		}
	}
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	spi_omapl138_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL)
	{
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	spi_omapl138_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	spi_omapl138_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	spi_omapl138_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

void spi_omapl138_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of systimer");
}

void init_spi_omapl138(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "omapl138_spi",
		.class_desc = "omapl138_spi",
		.new_instance = new_spi_omapl138,
		.free_instance = free_spi_omapl138,
		.module_type = SKYML_ONCHIP(SC_omapl138),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = spi_omapl138_read,
		.write = spi_omapl138_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = spi_omapl138_get_regvalue_by_id,
		.get_regname_by_id = spi_omapl138_get_regname_by_id,
		.set_regvalue_by_id = spi_omapl138_set_regvalue_by_id,
		.get_regnum = spi_omapl138_get_regnum,
		.get_regid_by_name = spi_omapl138_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static spi_bus_intf spi_bus_intf = {
		.spi_bus_receive = spi_bus_receive
	};
	SKY_register_iface(clss, SPI_BUS_INTF_NAME, &spi_bus_intf);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription)
		{
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf
		},
		(struct InterfaceDescription)
		{
			.name = SPI_BUS_INTF_NAME,
			.iface = &spi_bus_intf
		},
	};
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = SPI_DEVICE_INTF_NAME,
			.set = spi_device_set,
			.get = spi_device_get,
		},
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	spi_omapl138_register_attribute(clss);
}

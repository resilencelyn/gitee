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
#include <skyeye_system.h>
#define DEBUG
#include <skyeye_log.h>
#include "sopc_spi_fifo.h"
#include "sopc_spi.h"

static spi_callback(conf_object_t *opaque)
{
	sopc_spi_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;

	if(dev->signal.obj)
		dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.num);
}

static void assert_cs_line(conf_object_t *opaque)
{
	sopc_spi_device *dev = opaque->obj;
	conf_object_t *slave_obj;
	spi_device_intf *spi_dev_iface;

	if (dev->slave_index > 0)
	{
		slave_obj = dev->slaves[0].slave_obj;
		spi_dev_iface = dev->slaves[0].spi_dev_iface;
	}

	if (spi_dev_iface && slave_obj)
		spi_dev_iface->spi_assert_cs_line(slave_obj);
}

static void spi_bus_receive(void *spi_bus, char *buf, int len)
{
	sopc_spi_device *dev = ((conf_object_t *)spi_bus)->obj;
	struct registers *regs = &dev->regs;
	int i = 0;

	if (len > 0) {
		for (i = 0; i < len; i++) {
			WriteFIFO(dev->spi_fifo, &buf[i], 1);
			regs->rxflr ++;
			regs->sr |= SR_TFE;
			regs->sr &= ~SR_BUSY;
				/*
			if (((regs->ctrlr0 >> 8) & 3) == 3)
			{
				if (dev->recv_num == regs->rxflr)
				{
					//receive over
					regs->sr |= SR_TFE;
					regs->sr &= ~SR_BUSY;
				} else
				{
					regs->sr |= SR_BUSY;
				}
			} else
			{
				regs->sr |= SR_TFE;
				regs->sr &= ~SR_BUSY;
			}
			*/
			if (regs->rxflr >= regs->rxftlr)
			{
				RW_WRLOCK(dev->lock);
				regs->isr |= RXFIS;
				RW_UNLOCK(dev->lock);
				if (regs->imr & RXFIM)
				{
					if(dev->signal.obj)
						dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.num);
				}
			}
		}

		return;
	} else
	{
		regs->sr |= SR_TFE;
		regs->sr &= ~SR_BUSY;
	}
}

static int send_data_to_spi_device(conf_object_t *opaque, uint32_t spidat)
{
	sopc_spi_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	uint32_t spifmt, data;
	char *p = (char *)&spidat;
	conf_object_t *slave_obj;
	spi_device_intf *spi_dev_iface;

	if (dev->slave_index > 0)
	{
		slave_obj = dev->slaves[0].slave_obj;
		spi_dev_iface = dev->slaves[0].spi_dev_iface;
	}

	int len = regs->ctrlr0 & 0xf;
	if (len <= 7)
		len = 8;
	else
		len = 16;

	if (slave_obj && spi_dev_iface)
	{
		spi_dev_iface->spi_device_receive(slave_obj, p, len / 8);
		return 0;
	}

	return -1;
}

static int notify_transmit_done(conf_object_t *opaque)
{
	sopc_spi_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	conf_object_t *slave_obj;
	spi_device_intf *spi_dev_iface;

	if (dev->slave_index > 0)
	{
		slave_obj = dev->slaves[0].slave_obj;
		spi_dev_iface = dev->slaves[0].spi_dev_iface;
	}

	if (slave_obj && spi_dev_iface)
		spi_dev_iface->spi_device_receive_done(slave_obj);
	return 0;
}

static int start_transmit(conf_object_t *opaque, uint32_t spidat)
{
	sopc_spi_device *dev = opaque->obj;

	/* send data to spi device */
	if (0 == send_data_to_spi_device(opaque, spidat))
	{
		//notify_transmit_done(opaque);
	}
}

static exception_t sopc_spi_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_spi_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;

	if (offset >= DR && offset <= DR_END)
	{
		unsigned char data;
		if (CheckFIFOReadNum(dev->spi_fifo) > 0)
		{
			ReadFIFO(dev->spi_fifo, &data, 1);
			*(uint32_t *)buf = data;
		}
		return No_exp;
	}

	switch (offset)
	{
		case CTRLR0:
			*(uint32_t *)buf = regs->ctrlr0;
			break;
		case CTRLR1:
			*(uint32_t *)buf = regs->ctrlr1;
			break;
		case SPIENR:
			*(uint32_t *)buf = regs->spienr;
			break;
		case SER:
			*(uint32_t *)buf = regs->ser;
			break;
		case BAUDR:
			*(uint32_t *)buf = regs->baudr;
			break;
		case TXFTLR:
			*(uint32_t *)buf = regs->txftlr;
			break;
		case RXFTLR:
			*(uint32_t *)buf = regs->rxftlr;
			break;
		case TXFLR:
			*(uint32_t *)buf = regs->txflr;
			break;
		case RXFLR:
			if (regs->rxflr > 0xff)
			{
				*(uint32_t *)buf = 0xff;
				regs->rxflr -= 0xff;
			} else
				*(uint32_t *)buf = regs->rxflr;
			break;
		case SR:
			*(uint32_t *)buf = regs->sr;
			notify_transmit_done(opaque);
			break;
		case IMR:
			*(uint32_t *)buf = regs->imr;
			break;
		case ISR:
			*(uint32_t *)buf = regs->isr;
			break;
		case RISR:
			*(uint32_t *)buf = regs->risr;
			break;
		case TXOICR:
			*(uint32_t *)buf = regs->txoicr;
			break;
		case RXOICR:
			*(uint32_t *)buf = regs->rxoicr;
			break;
		case RXUICR:
			*(uint32_t *)buf = regs->rxuicr;
			break;
		case ICR:
			*(uint32_t *)buf = regs->icr;
			break;
		case DMACR:
			*(uint32_t *)buf = regs->dmacr;
			break;
		case DMATDLR:
			*(uint32_t *)buf = regs->dmatdlr;
			break;
		case DMARDLR:
			*(uint32_t *)buf = regs->dmardlr;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sopc_spi\n", offset);
			return Not_found_exp;
	}
	return No_exp;
}

static exception_t sopc_spi_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sopc_spi_device *dev = opaque->obj;
	struct registers *regs = &dev->regs;
	uint32_t val = *(uint32_t *)buf;

	if (offset >= DR && offset <= DR_END)
	{
		regs->sr &= ~SR_TFE;
		if (regs->spienr == 1)
		{
			regs->sr |= SR_TFNF;
			start_transmit(opaque, val);
		}
		return No_exp;
	}
	switch (offset)
	{
		case CTRLR0:
			if (regs->spienr != 1)
				regs->ctrlr0 = val;
			break;
		case CTRLR1:
			if (regs->spienr != 1)
			{
				regs->ctrlr1 = val;
				if (((regs->ctrlr0 >> 8) & 0x3) >= 2)
				{
					dev->recv_num = val + 1;
					regs->sr &= ~SR_BUSY;
				}
				regs->txflr = 0;
				regs->rxflr = 0;
				ClearFIFO(dev->spi_fifo);
			}
			break;
		case SWITCH:
			regs->spi_switch = val;
			break;
		case SPIENR:
			regs->spienr = val;
			break;
		case SER:
			regs->ser = val;
			if (regs->ser == 1) //enable CS line
				assert_cs_line(opaque);
			break;
		case BAUDR:
			if (regs->spienr != 1)
				regs->baudr = val;
			break;
		case TXFTLR:
			if (regs->spienr != 1)
				regs->txftlr = val;
			break;
		case RXFTLR:
			if (regs->spienr != 1)
				regs->rxftlr = val;
			break;
		case IMR:
			regs->imr = val;
			break;
		case DMACR:
			regs->dmacr = val;
			break;
		case DMATDLR:
			regs->dmatdlr = val;
			break;
		case DMARDLR:
			regs->dmardlr = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sopc_spi\n", offset);
			return Not_found_exp;
	}

	return No_exp;
}

static char* sopc_spi_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t sopc_spi_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	sopc_spi_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t sopc_spi_get_regnum(conf_object_t *conf_obj)
{
	return  (sizeof(struct registers) / 4 - 15);
}

static uint32_t sopc_spi_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	sopc_spi_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t sopc_spi_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t sopc_spi_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static void init_spi_register(sopc_spi_device *dev)
{
	struct registers *regs = &(dev->regs);
	regs->sr = 0x6;
	regs->imr = 0x3f;
}

static conf_object_t* new_sopc_spi(const char *obj_name)
{
	sopc_spi_device* dev = skyeye_mm_zero(sizeof(sopc_spi_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	init_spi_register(dev);
	dev->spi_fifo = CreateFIFO(0x40000);
	dev->slave_index = 0;
	RWLOCK_INIT(dev->lock);
	return dev->obj;
}

static exception_t config_sopc_spi(conf_object_t* opaque)
{
	sopc_spi_device *dev =  opaque->obj;
	system_register_timer_handler(opaque, 1000, (time_sched_t)spi_callback, SCHED_US|SCHED_NORMAL);
	return No_exp;
}

static exception_t free_sopc_spi(conf_object_t* opaque)
{
	sopc_spi_device *dev =  opaque->obj;

	FreeFIFO(dev->spi_fifo);
	RWLOCK_DESTROY(dev->lock);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sopc_spi_device *dev =  obj->obj;
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
	sopc_spi_device *dev =  obj->obj;
	*obj2 = dev->signal.obj;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	sopc_spi_device *dev =  conf_obj->obj;
	attr_value_t int_number_attr = SKY_make_attr_uinteger(dev->signal.num);
	return int_number_attr;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	sopc_spi_device *dev =  conf_obj->obj;
	dev->signal.num = SKY_attr_uinteger(*value);
	return No_exp;
}

static void sopc_spi_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of sopc spi");
	return;
}

static exception_t spi_device_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sopc_spi_device *dev = obj->obj;

	if (index >= MAX_SLAVES || index < 0)
		return No_exp;

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

void init_sopc_spi(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "sopc_spi",
		.class_desc = "sopc_spi",
		.new_instance = new_sopc_spi,
		.free_instance = free_sopc_spi,
		.config_instance = config_sopc_spi
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = sopc_spi_read,
		.write = sopc_spi_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = sopc_spi_get_regvalue_by_id,
		.get_regname_by_id = sopc_spi_get_regname_by_id,
		.set_regvalue_by_id = sopc_spi_set_regvalue_by_id,
		.get_regnum = sopc_spi_get_regnum,
		.get_regid_by_name = sopc_spi_get_regid_by_name,
		.get_regoffset_by_id = sopc_spi_get_regoffset_by_id
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
		}
	};

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
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	sopc_spi_register_attribute(clss);
}

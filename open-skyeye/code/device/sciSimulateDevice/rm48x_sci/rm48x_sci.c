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
#include <skyeye_emif.h>
#define DEBUG
#include <skyeye_log.h>
#include "rm48x_sci.h"

#define SET_BIT(reg, mask) ((reg) |= (mask))
#define CLR_BIT(reg, mask) ((reg) &= ~(mask))
#define CHK_BIT(reg, mask) ((reg) & (mask))

static void sci_interrupt_callback(void *obj, time_data_t *data)
{
	rm48x_sci_device *dev = ((conf_object_t *)obj)->obj;

	//SET_BIT(dev->regs.sciflr,0x100);
	if (dev->TX_CNT <= 8)
	{
		/* raise scitxbuf interrupt */
		if (dev->signal_iface)
		{
			dev->signal_iface->raise_signal(dev->signal, dev->Tx_int_number);
		}
	}
}

static void dmc_receive(conf_object_t *obj, void *buf)
{
	rm48x_sci_device *dev = obj->obj;

	unsigned char rx_buf = *(unsigned char *)buf;
	dev->regs.scird = rx_buf;
	WriteFIFO(dev->recv_fifo, &rx_buf, 1);
	dev->regs.sciflr |= 0x200;
	// raise an receive interrupt
	if (dev->signal_iface)
	{
		dev->signal_iface->raise_signal(dev->signal, dev->Rx_int_number);
	}

	return 0;
}

static void dmc_raise_signal(conf_object_t *obj, int irq_number)
{
	rm48x_sci_device *dev = obj->obj;
	if (dev->signal_iface)
	{
		dev->signal_iface->raise_signal(dev->signal, irq_number);
	}
}

static int recv_data_from_stimulate(conf_object_t *obj, unsigned char *buf, int length)
{
	rm48x_sci_device *dev = obj->obj;
	struct registers regs = dev->regs;
	int i;

	for(i = 0; i < length; i++)
	{
		RW_WRLOCK(dev->lock);
			WriteFIFO(dev->recv_fifo, buf, 1);
			//printf("buf:%s ",buf[i]);
			SET_BIT(dev->regs.sciflr, 0x200);
			if (dev->signal_iface)
			{
				dev->signal_iface->raise_signal(dev->signal, dev->Rx_int_number);
			}
		RW_UNLOCK(dev->lock);
	}
	//printf("\n");

	return 0;
}

void send_data_to_stimulate(rm48x_sci_device* dev, int length)
{
	unsigned char buf[48] = {0};
	memcpy(buf, dev->SCITXSHF, length);
	//printf("dev->SCITXSHF[0]: 0x%x\n",dev->SCITXSHF[0]);
	dev->TX_CNT = 0;
	if (dev->bus_iface != NULL)
	{
		dev->bus_iface->write_data(dev->bus_obj, dev->obj, buf, 0, length);
	}
}

void start_transmit_scidata(rm48x_sci_device* dev)
{
	if (dev->regs.scigcr1 & (1 << 25))
	{
		if (CHAR_LENGTH == 8)
		{
			dev->regs.scird = dev->regs.scitd;
			WriteFIFO(dev->recv_fifo, &dev->regs.scird, 1);
		}
		else
		{
			dev->regs.scird = dev->regs.scitd << (CHAR_LENGTH);
			WriteFIFO(dev->recv_fifo, &dev->regs.scird, 1);
		}

		dev->TX_CNT ++;
		SET_BIT(dev->regs.sciflr, 0x200);
		dev->RX_CNT = dev->TX_CNT;
		dev->TX_CNT = 0;
		if (dev->signal_iface)
		{
			dev->signal_iface->raise_signal(dev->signal, dev->Rx_int_number);
		}
		send_data_to_stimulate(dev, dev->RX_CNT);
	}
}

static exception_t rm48x_sci_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_sci_device *dev = opaque->obj;
	unsigned char recv_ch;
	switch (offset)
	{
		case SCIGCR0:
			*(uint32_t *)buf = dev->regs.scigcr0;
			break;
		case SCIGCR1:
			*(uint32_t *)buf = dev->regs.scigcr1;
			break;
		case SCIGCR2:
			*(uint32_t *)buf = dev->regs.scigcr2;
			break;
		case SCISETINT:
			*(uint32_t *)buf = dev->regs.scisetint;
			break;
		case SCICLEARINT:
			*(uint32_t *)buf = dev->regs.sciclearint;
			break;
		case SCISETINTLVL:
			*(uint32_t *)buf = dev->regs.scisetintlvl;
			break;
		case SCICLEARINTLVL:
			*(uint32_t *)buf = dev->regs.sciclearintlvl;
			break;
		case SCIFLR:
			*(uint32_t *)buf = dev->regs.sciflr;
			break;
		case SCIINTVECT0:
			*(uint32_t *)buf = dev->regs.sciintvect0;
			break;
		case SCIINTVECT1:
			*(uint32_t *)buf = dev->regs.sciintvect1;
			break;
		case SCIFORMAT:
			*(uint32_t *)buf = dev->regs.sciformat;
			break;
		case BRS:
			*(uint32_t *)buf = dev->regs.brs;
			break;
		case SCIED:
			*(uint32_t *)buf = dev->regs.scied;
			break;
		case SCIRD:
			ReadFIFO(dev->recv_fifo, &recv_ch, 1);
			*(uint32_t *)buf = recv_ch;
			dev->regs.scitd = recv_ch;
			SET_BIT(dev->regs.sciflr,0x200);

			break;
		case SCITD:
			*(uint32_t *)buf = dev->regs.scitd;
			break;
		case SCIPIO0:
			*(uint32_t *)buf = dev->regs.scipio0;
			break;
		case SCIPIO1:
			*(uint32_t *)buf = dev->regs.scipio1;
			break;
		case SCIPIO2:
			*(uint32_t *)buf = dev->regs.scipio2;
			break;
		case SCIPIO3:
			*(uint32_t *)buf = dev->regs.scipio3;
			break;
		case SCIPIO4:
			*(uint32_t *)buf = dev->regs.scipio4;
			break;
		case SCIPIO5:
			*(uint32_t *)buf = dev->regs.scipio5;
			break;
		case SCIPIO6:
			*(uint32_t *)buf = dev->regs.scipio6;
			break;
		case SCIPIO7:
			*(uint32_t *)buf = dev->regs.scipio7;
			break;
		case SCIPIO8:
			*(uint32_t *)buf = dev->regs.scipio8;
			break;
		case LINCOMPARE:
			*(uint32_t *)buf = dev->regs.lincompare;
			break;
		case LINRD0:
			*(uint32_t *)buf = dev->regs.linrd0;
			break;
		case LINRD1:
			*(uint32_t *)buf = dev->regs.linrd1;
			break;
		case LINMASK:
			*(uint32_t *)buf = dev->regs.linmask;
			break;
		case LINID:
			*(uint32_t *)buf = dev->regs.linid;
			break;
		case LINTD0:
			*(uint32_t *)buf = dev->regs.lintd0;
			break;
		case LINTD1:
			*(uint32_t *)buf = dev->regs.lintd1;
			break;
		case MBRS:
			*(uint32_t *)buf = dev->regs.mbrs;
			break;
		case IODFTCTRL:
			*(uint32_t *)buf = dev->regs.iodftctrl;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rm48x_sci\n", offset);
			break;
	}
	//printf(" %s\t offset = 0x%x\t value = 0x%x\n",__FUNCTION__,offset,*(uint32_t*)buf);
	return 0;
}

static exception_t rm48x_sci_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_sci_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf(" %s offset = 0x%x value = 0x%x\n",__FUNCTION__,offset,val);
	switch (offset)
	{
		case SCIGCR0:
			dev->regs.scigcr0 = val;
			if ((dev->regs.scigcr0 & 0x1) == 0)
			{
				memset(&dev->regs, 0, sizeof(dev->regs));
				dev->regs.sciflr = 0x900;
			}
			break;
		case SCIGCR1:
			dev->regs.scigcr1 = val;
			if ((dev->regs.scigcr1 & 0x80 )== 0x80 && dev->t_flag == 1)
			{
				int i,j;
				for (i = 0; i < 16; i++)
				{
					j = i;
					if(i > 8)
						j = 0;
					dev->t_data[i] = j+1;
					dev->regs.scird = dev->t_data[i];
					WriteFIFO(dev->recv_fifo, &dev->regs.scird, 1);
					SET_BIT(dev->regs.sciflr, 0x200);
				}
			}
			if ((dev->regs.scigcr1 & 0x80) != 0)
			{
				if (dev->handle == NULL)
					dev->handle = system_register_timer_handler(opaque, (int)2000, (time_sched_t)sci_interrupt_callback, SCHED_US|SCHED_NORMAL);
			}
			break;
		case SCIGCR2:
			dev->regs.scigcr2 = val;
			break;
		case SCISETINT:
			dev->regs.scisetint = val;
			break;
		case SCICLEARINT:
			dev->regs.sciclearint = val;
			break;
		case SCISETINTLVL:
			dev->regs.scisetintlvl = val;
			break;
		case SCICLEARINTLVL:
			dev->regs.sciclearintlvl = val;
			break;
		case SCIFLR:
			if ((val & 0xFF000000) != 0)
			{
				CLR_BIT(dev->regs.sciflr,0xFF000000);
			}
			else
			{
				dev->regs.sciflr = val;
			}
			break;
		case SCIINTVECT0:
			dev->regs.sciintvect0 = val;
			break;
		case SCIINTVECT1:
			dev->regs.sciintvect1 = val;
			break;
		case SCIFORMAT:
			dev->regs.sciformat = val;
			break;
		case BRS:
			dev->regs.brs = val;
			break;
		case SCIED:
			dev->regs.scied = val;
			break;
		case SCIRD:
			dev->regs.scird = val;
			WriteFIFO(dev->recv_fifo, &dev->regs.scird, 1);
			break;
		case SCITD:
			dev->regs.scitd = val;
			if (dev->regs.scigcr1 & (1 << 16))
			{
				start_transmit_scidata(dev);
			}
			else
			{
				dev->SCITXSHF[0] = val;
				SET_BIT(dev->regs.sciflr, 0x100);
				send_data_to_stimulate(dev, 1);
				if (dev->dmc_linker_iface)
				{
					dev->dmc_linker_iface->send_data(dev->dmc_linker, opaque, buf);
				}
			}

			//if (dev->TX_CNT == 0x8)
			//dev->TX_CNT = 0;
			//dev->SCITXSHF[dev->TX_CNT] = val;
			//dev->TX_CNT++;
			///* send data */
			//send_data_to_stimulate(dev, 1);
			//SET_BIT(dev->regs.sciflr, 0x100);
			if (CHK_BIT(dev->regs.scisetint, 0x100))
			{
				if (dev->signal_iface)
				{
					dev->signal_iface->raise_signal(dev->signal, dev->Tx_int_number);
				}
			}
			break;
		case SCIPIO0:
			dev->regs.scipio0 = val;
			break;
		case SCIPIO1:
			dev->regs.scipio1 = val;
			break;
		case SCIPIO2:
			dev->regs.scipio2 = val;
			break;
		case SCIPIO3:
			dev->regs.scipio3 = val;
			break;
		case SCIPIO4:
			dev->regs.scipio4 = val;
			break;
		case SCIPIO5:
			dev->regs.scipio5 = val;
			break;
		case SCIPIO6:
			dev->regs.scipio6 = val;
			break;
		case SCIPIO7:
			dev->regs.scipio7 = val;
			break;
		case SCIPIO8:
			dev->regs.scipio8 = val;
			break;
		case LINCOMPARE:
			dev->regs.lincompare = val;
			break;
		case LINRD0:
			dev->regs.linrd0 = val;
			break;
		case LINRD1:
			dev->regs.linrd1 = val;
			break;
		case LINMASK:
			dev->regs.linmask = val;
			break;
		case LINID:
			dev->regs.linid = val;
			break;
		case LINTD0:
			dev->regs.lintd0 = val;
			break;
		case LINTD1:
			dev->regs.lintd1 = val;
			break;
		case MBRS:
			dev->regs.mbrs = val;
			break;
		case IODFTCTRL:
			dev->regs.iodftctrl = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rm48x_sci\n", offset);
			break;
	}
	return 0;
}

static char* rm48x_sci_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t rm48x_sci_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	rm48x_sci_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t rm48x_sci_get_regnum(conf_object_t *conf_obj)
{
	return sizeof(struct registers) / 4;
}

static uint32_t rm48x_sci_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	rm48x_sci_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t rm48x_sci_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++)
	{
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_rm48x_sci(const char *obj_name)
{
	rm48x_sci_device* dev = skyeye_mm_zero(sizeof(rm48x_sci_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->regs.sciflr = 0x900;
	dev->recv_fifo = CreateFIFO(1024);
	RWLOCK_INIT(dev->lock);
	return dev->obj;
}

static exception_t free_rm48x_sci(conf_object_t* opaque)
{
	rm48x_sci_device *dev = opaque->obj;
	FreeFIFO(dev->recv_fifo);
	skyeye_free(dev->obj);
	RWLOCK_DESTROY(dev->lock);
	skyeye_free(dev);
	
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rm48x_sci_device *dev = obj->obj;
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
	rm48x_sci_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	rm48x_sci_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	rm48x_sci_device *dev = conf_obj->obj;
	dev->int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_Tx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	rm48x_sci_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->Tx_int_number);
	return int_number;
}

static exception_t set_attr_Tx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	rm48x_sci_device *dev = conf_obj->obj;
	dev->Tx_int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_Rx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	rm48x_sci_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->Rx_int_number);
	return int_number;
}

static exception_t set_attr_Rx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	rm48x_sci_device *dev = conf_obj->obj;
	dev->Rx_int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_test_data(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	rm48x_sci_device *dev = conf_obj->obj;
	attr_value_t t_flag = SKY_make_attr_uinteger(dev->t_flag);
	return t_flag;
}

static exception_t set_attr_test_data(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	rm48x_sci_device *dev = conf_obj->obj;
	dev->t_flag = SKY_attr_uinteger(*value);
	return No_exp;
}

static rm48x_sci_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number");
	SKY_register_attribute(clss, "Tx_int_number", get_attr_Tx_int_number, NULL, set_attr_Tx_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of sci Tx");
	SKY_register_attribute(clss, "Rx_int_number", get_attr_Rx_int_number, NULL, set_attr_Rx_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of sci Rx");
	SKY_register_attribute(clss, "test_data", get_attr_test_data, NULL, set_attr_test_data, NULL, SKY_Attr_Optional, "uinteger", "this is a flag to test");
}

static exception_t rs232_iface_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rm48x_sci_device *dev = obj->obj;
	dev->bus_obj = obj2;
	dev->bus_iface = (Rs232_bus_intf*)SKY_get_iface(dev->bus_obj, RS232_BUS_INTF);

	if (dev->bus_iface == NULL)
	{
		if (dev->bus_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", RS232_BUS_INTF);
		}
		else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", RS232_BUS_INTF, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t rs232_iface_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	rm48x_sci_device *dev = obj->obj;
	*obj2 = dev->bus_obj;
	*port = NULL;
	return No_exp;
}

static exception_t dmc_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rm48x_sci_device *dev =  obj->obj;
	dev->dmc_linker = obj2;
	dev->dmc_linker_iface = (dmc_linker_intf *)SKY_get_iface(dev->dmc_linker, DMC_LINKER_INTF);

	if (dev->dmc_linker_iface == NULL)
	{
		if (dev->dmc_linker == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", DMC_LINKER_INTF);
		}
		else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", DMC_LINKER_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t dmc_linker_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	rm48x_sci_device *dev =  obj->obj;
	*obj2 = dev->dmc_linker;
	*port = NULL;
	return No_exp;
}
static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rm48x_sci_device *dev = obj->obj;
	dev->core = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->core, CORE_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL)
	{
		if (dev->core == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	rm48x_sci_device *dev = obj->obj;
	*obj2 = dev->core;
	*port = NULL;
	return No_exp;

}

void init_rm48x_sci(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "rm48x_sci",
		.class_desc = "rm48x_sci",
		.new_instance = new_rm48x_sci,
		.free_instance = free_rm48x_sci
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = rm48x_sci_read,
		.write = rm48x_sci_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const dmc_dev_intf dmc_intf = {
		.receive_data = dmc_receive,
		.raise_signal = dmc_raise_signal,
	};
	SKY_register_iface(clss, DMC_DEV_INTF, &dmc_intf);

	static const Rs232_emif_intf rs232_emif_interface = {
		.set_state = recv_data_from_stimulate,
	};
	SKY_register_iface(clss, RS232_EMIF_INTF, &rs232_emif_interface);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = rm48x_sci_get_regvalue_by_id,
		.get_regname_by_id = rm48x_sci_get_regname_by_id,
		.set_regvalue_by_id = rm48x_sci_set_regvalue_by_id,
		.get_regnum = rm48x_sci_get_regnum,
		.get_regid_by_name = rm48x_sci_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

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
		(struct InterfaceDescription) {
			.name = DMC_DEV_INTF, 
			.iface = &dmc_intf,
		},
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
		(struct ConnectDescription) {
			.name = RS232_BUS_INTF,
			.set = rs232_iface_set,
			.get = rs232_iface_get,
		},
			(struct ConnectDescription) {
			.name = DMC_LINKER_INTF,
			.set = dmc_linker_set,
			.get = dmc_linker_get,
		},
		(struct ConnectDescription){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	
	rm48x_sci_register_attribute(clss);
}

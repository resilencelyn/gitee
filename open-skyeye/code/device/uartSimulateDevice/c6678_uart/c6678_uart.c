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
//#define DEBUG
#include "c6678_uart.h"

static void update_signal(c6678_uart_device *dev)
{
	struct registers *reg = &dev->regs;
	unsigned char buf;

	//DBG("in %s, ier=0x%x\n", __FUNCTION__, reg->ier);
	if(CHK_BIT(reg->ier, IER_ETHREI)) /* Check if transmit data interrupt is enabled */
	{
		if(CHK_BIT(reg->lsr, (LSR_THRE)))
		{
		/* set iir with 0b0010 meaning uthr is empty */
			//CLR_BIT(reg->iir, IIR_NINT);
			SET_BIT(reg->iir, IIR_TE);
			CLR_BIT(reg->iir, IIR_NINT);
		}
		/* thansmitter fifo and thr is empty */
		//SET_BIT(reg->lsr, (LSR_TEMT | LSR_THRE));
	}

	if(CHK_BIT(reg->ier, IER_ERDAI))
	{//Check if receiv data interrupt is enabled
		if(CHK_BIT(reg->lsr, LSR_DR))
		{ // if some data is already in RBR
			CLR_BIT(reg->iir, IIR_NINT); /* set uart interrupt flag */
			SET_BIT(reg->iir, IIR_RDA); /* set recei interrupt flag */
		}
	}
	/* some uart interrupt is happened */
	if(!CHK_BIT(reg->iir, IIR_NINT))
	{
		int core_index = 0;
		if(dev->general_signal.obj)
		{
			dev->general_signal.iface->event_signal(dev->general_signal.obj, dev->event_number, &core_index);
		}
	}
	return;
}

void uart_int_callback(conf_object_t* opaque)
{
	c6678_uart_device *dev = (c6678_uart_device *)(opaque->obj);
	struct registers *reg = &dev->regs;
	if(CHK_BIT(reg->iir, IIR_NINT)) /* if idle */
	{
		update_signal(dev);
    }
}

// �Ĵ�����ȡ��������
static exception_t c6678_uart_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	c6678_uart_device *dev = (c6678_uart_device *)conf_obj->obj;
	switch (offset)
	{
		case THR:
			if(CHK_BIT(dev->regs.lcr, LCR_DLAB))
			{
				*(uint8_t *)buf = dev->regs.dlb;
			} else
			{
				*(uint8_t *)buf = dev->regs.rbr;
				/* clear lsr receive data ready */
				CLR_BIT(dev->regs.lsr, LSR_DR);
				/* set iir no interrupt pending */
				//CLR_BIT(dev->regs.iir, IIR_NON);
				//SET_BIT(dev->regs.iir, IIR_NINT);
			}
			break;
		case IER:
			if(CHK_BIT(dev->regs.lcr, LCR_DLAB))
			{
				*(uint8_t *)buf = dev->regs.dmb;
			} else
				*(uint8_t *)buf = dev->regs.ier;
			break;
		case LCR:
			*(uint8_t *)buf = dev->regs.lcr;
			break;
		case IIR:
			if(CHK_BIT(dev->regs.lcr, LCR_DLAB))
			{
				*(uint8_t *)buf = dev->regs.afr;
			} else
			{
				*(uint8_t *)buf = dev->regs.iir;
				DBG("in %s,!!!!!!!!!!!  read iir=0x%x\n\n\n", __FUNCTION__, dev->regs.iir);
				CLR_BIT(dev->regs.iir, IIR_NON);
				SET_BIT(dev->regs.iir, IIR_NINT);
			}
			break;
		case MCR:
			*(uint8_t *)buf = dev->regs.mcr;
			break;
		case LSR:
			*(uint8_t *)buf = dev->regs.lsr;
			break;
		default:
			//skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in c6678_uart\n", offset);
			break;
	}
	return No_exp;
}

// �Ĵ���д���������
static exception_t c6678_uart_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	c6678_uart_device *dev = (c6678_uart_device *)conf_obj->obj;
	uint8_t val = *(uint8_t *)buf;
	switch (offset)
	{
		case THR:
			if(CHK_BIT(dev->regs.lcr, LCR_DLAB))
			{
				dev->regs.dlb = val;
			} else
			{
				if(dev->skyeye_uart.iface)
				{
					dev->skyeye_uart.iface->write(dev->skyeye_uart.obj, &val, 1);
				}
				if (dev->uart_file.iface)
				{
					dev->uart_file.iface->write(dev->uart_file.obj, &val, 1);
				}

				if (dev->uart_com)
				{
					dev->uart_com_iface->write(dev->uart_com, buf, 1);  //output to rs232
				}

				if(CHK_BIT(dev->regs.iir, IIR_NINT))
				{
					update_signal(dev);
				}
				/* set TEMT and THRE bit */
				SET_BIT(dev->regs.lsr, (LSR_TEMT | LSR_THRE));
			}
			break;
		case IER:
			if(CHK_BIT(dev->regs.lcr, LCR_DLAB))
			{
				dev->regs.dmb = val;
			} else
			{
				//DBG("in %s, ier=0x%x\n", __FUNCTION__, val);
				dev->regs.ier = val;
			}
			break;
		case IIR:
			if(CHK_BIT(dev->regs.lcr, LCR_DLAB))
			{
				dev->regs.afr = val;
			} else
			{
				dev->regs.fcr = val;
#if 0
				if(CHK_BIT(dev->regs.iir, IIR_TE))
				{
					/* set iir no interrupt pending */
					CLR_BIT(dev->regs.iir, IIR_NON);
					SET_BIT(dev->regs.iir, IIR_NINT);
				}
#endif
			}
			break;
		case LCR:
			dev->regs.lcr = val;
			break;
		case MCR:
			dev->regs.mcr = val;
			break;
		default:
			//skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in c6678_uart\n", offset);
			break;
	}
	return No_exp;
}

// �Ĵ����ӿ�:���ݼĴ���ID��ȡ�Ĵ�����
static char* c6678_uart_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

// �Ĵ����ӿ�:���ݼĴ���ID���üĴ���ֵ
static exception_t c6678_uart_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	c6678_uart_device *dev = (c6678_uart_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint8_t*)regs + id) = value;
	return No_exp;
}

// �Ĵ����ӿ�:��ȡ�豸�ж��ټĴ���
static uint32_t c6678_uart_get_regnum(conf_object_t *conf_obj)
{
	return sizeof(struct registers);
}

// �Ĵ����ӿ�:���ݼĴ���ID��ȡ�Ĵ���ֵ
static uint32_t c6678_uart_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	c6678_uart_device *dev = (c6678_uart_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint8_t* regs_value = (uint8_t*)regs + id;
	return *regs_value;
}

// �Ĵ����ӿ�:���ݼĴ�������ȡ�Ĵ���ID
static uint32_t c6678_uart_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 1, i;
	for (i = 0; i < regnum; i++)
	{
		if (strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

// ���ڷ���һ���ֽ�
static exception_t c6678_uart_serial_write(conf_object_t *conf_obj, uint32_t value)
{
	c6678_uart_device *dev = (c6678_uart_device *)conf_obj->obj;
	exception_t ret = 0;
	unsigned char buf = value;

//	CLR_BIT(dev->regs.iir, IIR_NON);
//	SET_BIT(dev->regs.iir, IIR_RDA);
	SET_BIT(dev->regs.lsr, LSR_DR);
	if (dev->regs.fcr & 1)
	{
		if (buf == 127)
			buf = '\b';
		else if(buf == 10 || buf == 13)
		{
			buf = '\r';
		}
		dev->regs.rbr = buf;
	}
	DBG("in %s, get a char=%c, ier=0x%x, iir=0x%x\n", __FUNCTION__, buf, dev->regs.ier, dev->regs.iir);
	dev->regs.rbr = buf;
	if(CHK_BIT(dev->regs.iir, IIR_NINT))
	{
		update_signal(dev);
	}
	return No_exp;
}

// ȷ���Ƿ���Խ�������
static exception_t c6678_uart_receive_ready(conf_object_t *conf_obj)
{
	c6678_uart_device *dev = (c6678_uart_device *)conf_obj->obj;

	return No_exp;
}

// ��������һ������
static uint32_t c6678_uart_serial_write_chars(conf_object_t *conf_obj, char *buf, uint32_t cnt)
{
	c6678_uart_device *dev = (c6678_uart_device *)conf_obj->obj;

	return No_exp;
}

// ʵ����һ�����豸����obj_name����json�ļ������õ�����
static conf_object_t* new_c6678_uart(const char *obj_name)
{
	c6678_uart_device* dev = skyeye_mm_zero(sizeof(c6678_uart_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: �ڴ������Զ���Դ��


	// �Ĵ�����ʼ��
	struct registers *regs = &(dev->regs);
	regs->lsr = 0x60;
	regs->iir = 0x1;
	return dev->obj;
}

// ����ʵ�������豸����
static exception_t config_c6678_uart(conf_object_t* conf_obj)
{
	c6678_uart_device *dev = (c6678_uart_device *)conf_obj->obj;
	// TODO: �ڴ������Զ���Դ��

	//system_register_timer_handler(conf_obj, 500, (time_sched_t)uart_int_callback, SCHED_US|SCHED_NORMAL);
	return No_exp;
}

// �ͷ��豸��������һЩ�ڴ��ͷ�
static exception_t free_c6678_uart(conf_object_t* conf_obj)
{
	c6678_uart_device *dev = (c6678_uart_device *)conf_obj->obj;
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

// ϵͳ�����豸general_signal�ӿڹ���
static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	c6678_uart_device *dev = (c6678_uart_device *)obj->obj;

	dev->general_signal.obj = obj2;
	if (obj2 == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", GENERAL_SIGNAL_INTF_NAME);
		return Not_found_exp;
	}

	dev->general_signal.iface = (general_signal_iface_t *)SKY_get_iface(obj2, GENERAL_SIGNAL_INTF_NAME);
	if (dev->general_signal.iface == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", GENERAL_SIGNAL_INTF_NAME, obj2->objname);
		return Not_found_exp;
	}
	return No_exp;
}

// ϵͳ��ȡ�豸general_signal�ӿں���
static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	c6678_uart_device *dev = (c6678_uart_device *)obj->obj;
	*obj2 = dev->general_signal.obj;
	*port = NULL;
	return No_exp;
}

// ϵͳ�����豸skyeye_uart�ӿڹ���
static exception_t skyeye_uart_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	c6678_uart_device *dev = (c6678_uart_device *)obj->obj;

	dev->skyeye_uart.obj = obj2;
	if (obj2 == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", SKYEYE_UART_INTF);
		return Not_found_exp;
	}

	dev->skyeye_uart.iface = (skyeye_uart_intf *)SKY_get_iface(obj2, SKYEYE_UART_INTF);
	if (dev->skyeye_uart.iface == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", SKYEYE_UART_INTF, obj2->objname);
		return Not_found_exp;
	}
	return No_exp;
}

// ϵͳ��ȡ�豸skyeye_uart�ӿڹ���
static exception_t skyeye_uart_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	c6678_uart_device *dev = (c6678_uart_device *)obj->obj;
	*obj2 = dev->skyeye_uart.obj;
	*port = NULL;
	return No_exp;
}

// ϵͳ�����豸uart_file�ӿڹ���
static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	c6678_uart_device *dev = (c6678_uart_device *)obj->obj;
	dev->uart_file.obj = obj2;
	if (obj2 == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", UART_FILE_INTF);
		return Not_found_exp;
	}

	dev->uart_file.iface = (uart_file_intf *)SKY_get_iface(obj2, UART_FILE_INTF);
	if (dev->uart_file.iface == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", UART_FILE_INTF, obj2->objname);
		return Not_found_exp;
	}
	return No_exp;
}

// ϵͳ��ȡ�豸uart_file�ӿں���
static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	c6678_uart_device *dev = (c6678_uart_device *)obj->obj;
	*obj2 = dev->uart_file.obj;
	*port = NULL;
	return No_exp;
}

static exception_t uart_com_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	c6678_uart_device* dev = (c6678_uart_device*)(obj->obj);
	dev->uart_com = obj2;
	dev->uart_com_iface = (uart_com_intf*)SKY_get_iface(obj2, UART_COM_INTF);
	if (NULL == dev->uart_com_iface)
	{
		if (dev->uart_com == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", UART_COM_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Get 'uart_com_intf' interface from %s fail.", obj2->objname);
		return Not_found_exp;
	}
	return No_exp;
}

static exception_t uart_com_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	c6678_uart_device* dev = (c6678_uart_device*)(obj->obj);
	*obj2 = dev->uart_com;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_event_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	c6678_uart_device *dev =  conf_obj->obj;
	attr_value_t event_number = SKY_make_attr_uinteger(dev->event_number);
	return event_number;
}

static exception_t set_attr_event_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	c6678_uart_device *dev =  conf_obj->obj;
	dev->event_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void c6678_uart_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "event_number", get_attr_event_number, NULL, set_attr_event_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of c6678 uart");
	return;
}

// ��ʼ��ģ���࣬����SkyEye����ģ��ʱִ�����´���
void init_c6678_uart(void)
{
	// ��SkyEyeע��һ���豸
	static skyeye_class_t class_data =
	{
		.class_name      = "c6678_uart",
		.class_desc      = "c6678 uart",
		.new_instance    = new_c6678_uart,
		.free_instance   = free_c6678_uart,
		.config_instance = config_c6678_uart,
		.module_type     = SKYML_ONCHIP(SC_c6678)
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

	// ���üĴ�����ַ��д����
	static memory_space_intf io_memory_interface =
	{
		.read  = c6678_uart_read,
		.write = c6678_uart_write
	};
	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

	// ע����ʽӿ�(����SkyEyeͼ�ο��ƽӿ�)
	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = c6678_uart_get_regvalue_by_id,
		.get_regname_by_id  = c6678_uart_get_regname_by_id,
		.set_regvalue_by_id = c6678_uart_set_regvalue_by_id,
		.get_regnum         = c6678_uart_get_regnum,
		.get_regid_by_name  = c6678_uart_get_regid_by_name
	};
	SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);

	// �����豸�豸������ӿ�
	static const skyeye_serial_device_t skyeye_serial_interface =
	{
		.write         = c6678_uart_serial_write,
		.write_chars   = c6678_uart_serial_write_chars,
		.receive_ready = c6678_uart_receive_ready
	};
	SKY_register_iface(dev_class, SERIAL_DEVICE_INTERFACE, &skyeye_serial_interface);

	// �ӿڷ����б�
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
			.name  = SERIAL_DEVICE_INTERFACE,
			.iface = &skyeye_serial_interface
		},
	};

	// �ӿ����ӷ����б�
	static const struct ConnectDescription connects[] =
	{
		(struct ConnectDescription)
		{
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set  = general_signal_set,
			.get  = general_signal_get
		},
		(struct ConnectDescription)
		{
			.name = SKYEYE_UART_INTF,
			.set  = skyeye_uart_set,
			.get  = skyeye_uart_get
		},
		(struct ConnectDescription)
		{
			.name = UART_FILE_INTF,
			.set  = uart_file_set,
			.get  = uart_file_get
		},
		(struct ConnectDescription)
		{
			.name = UART_COM_INTF,
			.set = uart_com_set,
			.get = uart_com_get,
		}
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
	c6678_uart_register_attribute(dev_class);
}

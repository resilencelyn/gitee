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
#include "McBSP_c6748.h"

#define McBSP0_RINT 87
#define McBSP0_XINT 88
#define McBSP1_RINT 89
#define McBSP1_XINT 90
#define channel0_R   (2)
#define channel0_T   (3)
#define channel1_R   (4)
#define channel1_T   (5)

static void mcbsp_receive_data(conf_object_t *obj)
{
	McBSP_c6748_device *dev = obj->obj;
	if(CheckCanReadNum(dev->receive_fifo) >= 4){
		if (dev->regs.spcr.bit.rrdy) {
			if(dev->signal_iface != NULL){
				dev->signal_iface->raise_signal(dev->signal, dev->rint_event + dev->dev_number * 2);
			}
		}
	}
}

static void mcbsp_send_data(conf_object_t *obj)
{
	McBSP_c6748_device *dev = obj->obj;
	if(CheckCanWriteNum(dev->receive_fifo) > 16){
		if (dev->regs.spcr.bit.xrdy) {
			if(dev->signal_iface != NULL){
				dev->signal_iface->raise_signal(dev->signal, dev->xint_event + dev->dev_number * 2);
			}
		}
	}

	//WFIFO enable, mcbsp send XEVT to BFIFO
	if (dev->regs.wfifoctl.bit.wena && dev->regs.spcr.bit.xrst) {
		if (dev->fifo_event != NULL) {
			dev->fifo_event->write(dev->fifo, dev->channel_T, 1);
		}
	}
}

static exception_t McBSP_c6748_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	McBSP_c6748_device *dev = opaque->obj;
	char data[4];
	switch (offset)
	{
		case DRR:
			//*(uint32_t *)buf = dev->regs.drr;
			if(CheckCanReadNum(dev->receive_fifo) >= 4){
				ReadFIFO(dev->receive_fifo,data,4);
			}
			*(uint32_t *)buf = *(uint32_t*)data;
			break;
		case DXR:
			*(uint32_t *)buf = dev->regs.dxr;
			break;
		case SPCR:
			*(uint32_t *)buf = dev->regs.spcr.all;
			break;
		case RCR:
			*(uint32_t *)buf = dev->regs.rcr.all;
			break;
		case XCR:
			*(uint32_t *)buf = dev->regs.xcr.all;
			break;
		case SRGR:
			*(uint32_t *)buf = dev->regs.srgr.all;
			break;
		case MCR:
			*(uint32_t *)buf = dev->regs.mcr.all;
			break;
		case RCERE0:
			*(uint32_t *)buf = dev->regs.rcere0;
			break;
		case XCERE0:
			*(uint32_t *)buf = dev->regs.xcere0;
			break;
		case PCR:
			*(uint32_t *)buf = dev->regs.pcr;
			break;
		case RCERE1:
			*(uint32_t *)buf = dev->regs.rcere1;
			break;
		case XCERE1:
			*(uint32_t *)buf = dev->regs.xcere1;
			break;
		case RCERE2:
			*(uint32_t *)buf = dev->regs.rcere2;
			break;
		case XCERE2:
			*(uint32_t *)buf = dev->regs.xcere2;
			break;
		case RCERE3:
			*(uint32_t *)buf = dev->regs.rcere3;
			break;
		case XCERE3:
			*(uint32_t *)buf = dev->regs.xcere3;
			break;
		case BFIFOREV:
			*(uint32_t *)buf = dev->regs.bfiforev;
			break;
		case WFIFOCTL:
			*(uint32_t *)buf = dev->regs.wfifoctl.all;
			break;
		case WFIFOSTS:
			*(uint32_t *)buf = dev->regs.wfifosts;
			break;
		case RFIFOCTL:
			*(uint32_t *)buf = dev->regs.rfifoctl.all;
			break;
		case RFIFOSTS:
			*(uint32_t *)buf = dev->regs.rfifosts;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in McBSP_c6748\n", offset);
			break;
	}
	//printf("%s  %d offset: 0x%x val: 0x%x\n", __func__, __LINE__, offset, *(uint32_t *)buf);

	return No_exp;
}
static exception_t McBSP_c6748_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	McBSP_c6748_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf("%s %d offset: 0x%x val: 0x%x\n", __func__, __LINE__, offset, val);
	switch (offset)
	{
		case DRR:
			dev->regs.drr = val;
			break;
		case DXR:
			//dev->regs.dxr = val;
			if(dev->regs.spcr.bit.xrst == 1){
				if(dev->McBSP_linker_intf != NULL){
					dev->McBSP_linker_intf->McBSP_linker_data_send(dev->McBSP_linker,opaque,(char *)buf,2);
				}
			}


			break;
		case SPCR:
			dev->regs.spcr.all = val;
			if(dev->regs.spcr.bit.rrst == 1){
				if(dev->sche_rr == -1){
					create_thread_scheduler(100, Periodic_sched, mcbsp_receive_data, (void *)opaque, &dev->sche_rr);
				}
			}else{
				del_thread_scheduler(dev->sche_rr);
				dev->sche_rr = -1;
			}

			if(dev->regs.spcr.bit.xrst == 1){
				if(dev->sche_xr == -1){
					create_thread_scheduler(700, Periodic_sched, mcbsp_send_data, (void *)opaque, &dev->sche_xr);
				}
			}
			else{
				del_thread_scheduler(dev->sche_xr);
				dev->sche_xr = -1;
			}

			break;
		case RCR:
			dev->regs.rcr.all = val;
			break;
		case XCR:
			dev->regs.xcr.all = val;
			break;
		case SRGR:
			dev->regs.srgr.all = val;
			break;
		case MCR:
			dev->regs.mcr.all = val;
			break;
		case RCERE0:
			dev->regs.rcere0 = val;
			break;
		case XCERE0:
			dev->regs.xcere0 = val;
			break;
		case PCR:
			dev->regs.pcr = val;
			break;
		case RCERE1:
			dev->regs.rcere1 = val;
			break;
		case XCERE1:
			dev->regs.xcere1 = val;
			break;
		case RCERE2:
			dev->regs.rcere2 = val;
			break;
		case XCERE2:
			dev->regs.xcere2 = val;
			break;
		case RCERE3:
			dev->regs.rcere3 = val;
			break;
		case XCERE3:
			dev->regs.xcere3 = val;
			break;
		case BFIFOREV:
			dev->regs.bfiforev = val;
			break;
		case WFIFOCTL:
			dev->regs.wfifoctl.all = val;
			break;
		case WFIFOSTS:
			dev->regs.wfifosts = val;
			break;
		case RFIFOCTL:
			dev->regs.rfifoctl.all = val;
			break;
		case RFIFOSTS:
			dev->regs.rfifosts = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in McBSP_c6748\n", offset);
			break;
	}
	return No_exp;
}
static char* McBSP_c6748_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t McBSP_c6748_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	McBSP_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t McBSP_c6748_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t McBSP_c6748_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	McBSP_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t McBSP_c6748_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_McBSP_c6748(char *obj_name)
{
	McBSP_c6748_device* dev = skyeye_mm_zero(sizeof(McBSP_c6748_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->sche_rr = -1;
	dev->sche_xr = -1;
	dev->receive_fifo = CreateFIFO(512);

	// The device number defaults to 0 device
	dev->dev_number = 0;
	dev->rint_event = McBSP0_RINT;
	dev->xint_event = McBSP0_XINT;
	dev->channel_T = channel0_T;
	dev->channel_R = channel0_R;

	return dev->obj;
}
static exception_t free_McBSP_c6748(conf_object_t* opaque)
{
	McBSP_c6748_device *dev = opaque->obj;
	FreeFIFO(dev->receive_fifo);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	McBSP_c6748_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
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
	McBSP_c6748_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static exception_t McBSP_linker_data_intf_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	McBSP_c6748_device *dev = obj->obj;
	dev->McBSP_linker = obj2;
	dev->McBSP_linker_intf = (McBSP_linker_data_connect_intf *)SKY_get_iface(dev->McBSP_linker, McBSP_LINKER_DATA_CONNECT_INTF);
	if (dev->McBSP_linker_intf == NULL){
		if (dev->McBSP_linker == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", McBSP_LINKER_DATA_CONNECT_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", McBSP_LINKER_DATA_CONNECT_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t McBSP_linker_data_intf_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	McBSP_c6748_device *dev = obj->obj;
	*obj2 = dev->McBSP_linker;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_device_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	McBSP_c6748_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->dev_number);
	return int_number;
}
static exception_t set_attr_device_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	McBSP_c6748_device *dev = conf_obj->obj;
	dev->dev_number = SKY_attr_uinteger(*value);
	if (dev->dev_number == 0) {
		dev->rint_event = McBSP0_RINT;
		dev->xint_event = McBSP0_XINT;
		dev->channel_T = channel0_T;
		dev->channel_R = channel0_R;
	} else if (dev->dev_number == 1) {
		dev->rint_event = McBSP1_RINT;
		dev->xint_event = McBSP1_XINT;
		dev->channel_T = channel1_T;
		dev->channel_R = channel1_R;
	} else {
		skyeye_log(Error_log, __FUNCTION__, "The device McbSP is only 0 and 1 only, from %s fail.", conf_obj->objname);
	}
	return No_exp;
}

static McBSP_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "device_number", get_attr_device_number, NULL, set_attr_device_number, NULL, SKY_Attr_Optional, "uinteger", "McBSP device number");
}

static exception_t rx_receive_data(conf_object_t *obj,char *data,uint32_t datalength)
{
	McBSP_c6748_device *dev = obj->obj;
	if(dev->receive_fifo == NULL){
		return 0;
	}
	WriteFIFO(dev->receive_fifo, data, datalength);

	if (dev->fifo_data_intf != NULL) {
		dev->fifo_data_intf->write_fifo(dev->fifo_data, data, 2);
	}

	if (dev->fifo_event) {
		dev->fifo_event->write(dev->fifo, dev->channel_R, 1);
	}

	return No_exp;
}

static exception_t write_fifo_data(conf_object_t *obj, void *buf, size_t length)
{
	McBSP_c6748_device *dev = obj->obj;
	McBSP_c6748_write(obj, DXR, buf, length);
	return No_exp;
}

static exception_t edma_event_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	McBSP_c6748_device *dev = obj->obj;
	dev->edma = obj2;
	dev->edma_event = (edma_device_event_t *)SKY_get_iface(dev->edma, EDMA_DEVICE_EVENT_INTERFACE);

	if (dev->edma_event == NULL){
		if (dev->edma == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", EDMA_DEVICE_EVENT_INTERFACE);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", EDMA_DEVICE_EVENT_INTERFACE, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t edma_event_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	McBSP_c6748_device *dev = obj->obj;
	*obj2 = dev->edma;
	*port = NULL;
	return No_exp;
}

static exception_t fifo_event_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	McBSP_c6748_device *dev = obj->obj;
	dev->fifo = obj2;
	dev->fifo_event = (fifo_device_event_t *)SKY_get_iface(dev->fifo, FIFO_DEVICE_EVENT_INTERFACE);

	if (dev->fifo_event == NULL){
		if (dev->fifo == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", FIFO_DEVICE_EVENT_INTERFACE);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", FIFO_DEVICE_EVENT_INTERFACE, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t fifo_event_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	McBSP_c6748_device *dev = obj->obj;
	*obj2 = dev->fifo;
	*port = NULL;
	return No_exp;
}

static exception_t fifo_data_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	McBSP_c6748_device *dev = obj->obj;
	dev->fifo_data = obj2;
	dev->fifo_data_intf = (fifo_data_intf *)SKY_get_iface(dev->fifo_data, FIFO_DATA_INTF);

	if (dev->fifo_data_intf == NULL){
		if (dev->fifo_data == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", FIFO_DATA_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", FIFO_DATA_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t fifo_data_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	McBSP_c6748_device *dev = obj->obj;
	*obj2 = dev->fifo_data;
	*port = NULL;
	return No_exp;
}

void init_McBSP_c6748(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6748_McBSP",
		.class_desc = "c6748_McBSP",
		.new_instance = new_McBSP_c6748,
		.free_instance = free_McBSP_c6748,
		.module_type = SKYML_ONCHIP(SC_c6748),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = McBSP_c6748_read,
		.write = McBSP_c6748_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = McBSP_c6748_get_regvalue_by_id,
		.get_regname_by_id = McBSP_c6748_get_regname_by_id,
		.set_regvalue_by_id = McBSP_c6748_set_regvalue_by_id,
		.get_regnum = McBSP_c6748_get_regnum,
		.get_regid_by_name = McBSP_c6748_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
	static McBSP_data_receive_intf data_receive =
	{
		.receive_data = rx_receive_data,
	};
	SKY_register_iface(clss,McBSP_DATA_RECEIVE_INTF, &data_receive);

	static fifo_data_intf fifo_data = {
		.write_fifo = write_fifo_data,
		.read_fifo = NULL,
	};
	SKY_register_iface(clss, FIFO_DATA_INTF, &fifo_data);

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
			.name = McBSP_DATA_RECEIVE_INTF,
			.iface = &data_receive,
		},
		(struct InterfaceDescription)
		{
			.name = FIFO_DATA_INTF,
			.iface = &fifo_data
		},
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
		(struct ConnectDescription){
			.name = McBSP_LINKER_DATA_CONNECT_INTF,
			.set = McBSP_linker_data_intf_set,
			.get = McBSP_linker_data_intf_get,
		},
		(struct ConnectDescription){
			.name = EDMA_DEVICE_EVENT_INTERFACE,
			.set = edma_event_set,
			.get = edma_event_get,
		},
		(struct ConnectDescription){
			.name = FIFO_DEVICE_EVENT_INTERFACE,
			.set = fifo_event_set,
			.get = fifo_event_get,
		},
		(struct ConnectDescription){
			.name = FIFO_DATA_INTF,
			.set = fifo_data_set,
			.get = fifo_data_get,
		},

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	McBSP_register_attribute(clss);
}

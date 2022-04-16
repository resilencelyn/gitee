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
#include "fifo_McBSP_c6748.h"

#define FIFO_DEPTH 0x1024
#define channel0_R   (2)
#define channel0_T   (3)
#define channel1_R   (4)
#define channel1_T   (5)


static exception_t fifo_McBSP_c6748_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	fifo_McBSP_c6748_device *dev = opaque->obj;
    uint32_t data;
	switch (offset)
	{
		case XBUF:
            if(CheckCanReadNum(dev->rfifo) >= 2){
                ReadFIFO(dev->rfifo, &data, 2);
            }
			*(uint32_t *)buf = data;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in fifo_McBSP_c6748\n", offset);
			break;
	}
    //printf("%s %d offset: 0x%x buf: 0x%x\n",__func__,__LINE__,offset,*(uint32_t *)buf);
	return 0;
}
static exception_t fifo_McBSP_c6748_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	fifo_McBSP_c6748_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
    uint32_t datalength = count;
    uint32_t data;
    switch (offset) {
		case XBUF:
            if (dev->fifo_w) {
                WriteFIFO(dev->wfifo, (char *)buf, datalength);
                if (CheckCanReadNum(dev->wfifo) >= 2) {
                    ReadFIFO(dev->wfifo, &data, 2);
                    if (dev->fifo_data_intf != NULL){
				        dev->fifo_data_intf->write_fifo(dev->fifo_data, &data, 2);
			        }
                }
            }
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in fifo_McBSP_c6748\n", offset);
			break;
	}
	return 0;
}

static char* fifo_McBSP_c6748_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t fifo_McBSP_c6748_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	fifo_McBSP_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t fifo_McBSP_c6748_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t fifo_McBSP_c6748_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	fifo_McBSP_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t fifo_McBSP_c6748_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_fifo_McBSP_c6748(char *obj_name)
{
	fifo_McBSP_c6748_device* dev = skyeye_mm_zero(sizeof(fifo_McBSP_c6748_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
    dev->wfifo = CreateFIFO(FIFO_DEPTH);
    dev->rfifo = CreateFIFO(FIFO_DEPTH);

    // The device number defaults to 0 device
    dev->dev_number = 0;
    dev->channel_T = channel0_T;
    dev->channel_R = channel0_R;

	return dev->obj;
}

static exception_t free_fifo_McBSP_c6748(conf_object_t* opaque)
{
	fifo_McBSP_c6748_device *dev = opaque->obj;
    FreeFIFO(dev->wfifo);
    FreeFIFO(dev->rfifo);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t eventProduce(conf_object_t *obj,uint32_t channel,uint32_t value)
{
    fifo_McBSP_c6748_device *dev = obj->obj;

    dev->fifo_w = value;
    if(channel == dev->channel_T) {
        dev->xevt = 1;

        dev->xevt = 0;
        if(dev->edma_event != NULL)
            dev->edma_event->write(dev->edma, dev->channel_T, 0);
        return ;
    } else if (channel == dev->channel_R) {
        dev->revt = 1;

        dev->revt = 0;
        if(dev->edma_event != NULL)
        dev->edma_event->write(dev->edma, dev->channel_R, 0);
    }
    return No_exp;
}

static exception_t write_fifo_data(conf_object_t *obj, void *buf, size_t length)
{
    fifo_McBSP_c6748_device *dev = obj->obj;
    WriteFIFO(dev->rfifo, (char *)buf, length);
    return No_exp;
}

static exception_t edma_event_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    fifo_McBSP_c6748_device *dev = obj->obj;
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
    fifo_McBSP_c6748_device *dev = obj->obj;
    *obj2 = dev->edma;
    *port = NULL;
    return No_exp;
}

static exception_t fifo_data_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    fifo_McBSP_c6748_device *dev = obj->obj;
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
    fifo_McBSP_c6748_device *dev = obj->obj;
    *obj2 = dev->fifo_data;
    *port = NULL;
    return No_exp;
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	fifo_McBSP_c6748_device *dev = obj->obj;
	dev->image = obj2;
	dev->image_space = (memory_space_intf*)SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);
	if (dev->image_space == NULL){
		if (dev->image == NULL)
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
	fifo_McBSP_c6748_device *dev = obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_device_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	fifo_McBSP_c6748_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->dev_number);
	return int_number;
}

static exception_t set_attr_device_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	fifo_McBSP_c6748_device *dev = conf_obj->obj;
	dev->dev_number = SKY_attr_uinteger(*value);
    if (dev->dev_number == 0) {
        dev->channel_T = channel0_T;
        dev->channel_R = channel0_R;
    } else if (dev->dev_number == 1) {
        dev->channel_T = channel1_T;
        dev->channel_R = channel1_R;
    } else {
        skyeye_log(Error_log, __FUNCTION__, "The device McbSP_FIFO is only 0 and 1 only, from %s fail.", conf_obj->objname);
    }
	return No_exp;
}

static McBSP_FIFO_register_attribute(conf_class_t* clss){
    SKY_register_attribute(clss, "device_number", get_attr_device_number, NULL, set_attr_device_number, NULL, SKY_Attr_Optional, "uinteger", "McBSP device number");
}

void init_fifo_McBSP_c6748(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "fifo_McBSP_c6748",
		.class_desc = "fifo_McBSP_c6748",
		.new_instance = new_fifo_McBSP_c6748,
		.free_instance = free_fifo_McBSP_c6748
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = fifo_McBSP_c6748_read,
		.write = fifo_McBSP_c6748_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = fifo_McBSP_c6748_get_regvalue_by_id,
		.get_regname_by_id = fifo_McBSP_c6748_get_regname_by_id,
		.set_regvalue_by_id = fifo_McBSP_c6748_set_regvalue_by_id,
		.get_regnum = fifo_McBSP_c6748_get_regnum,
		.get_regid_by_name = fifo_McBSP_c6748_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static fifo_device_event_t fifo_event = {
        .write = eventProduce,
    };
    SKY_register_iface(clss,FIFO_DEVICE_EVENT_INTERFACE,&fifo_event);

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
            .name = FIFO_DEVICE_EVENT_INTERFACE,
            .iface = &fifo_event
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
            .name = EDMA_DEVICE_EVENT_INTERFACE,
            .set = edma_event_set,
            .get = edma_event_get,
        },
        (struct ConnectDescription){
            .name = FIFO_DATA_INTF,
            .set = fifo_data_set,
            .get = fifo_data_get,
        },
		(struct ConnectDescription ){
			.name = MEMORY_SPACE_INTF_NAME,
			.set = memory_set,
			.get = memory_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
    McBSP_FIFO_register_attribute(clss);
}

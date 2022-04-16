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
#include "McBSP_linker.h"

static conf_object_t* new_McBSP_linker(char *obj_name)
{
	McBSP_linker_t* dev = skyeye_mm_zero(sizeof(McBSP_linker_t));
	dev->obj = new_conf_object(obj_name, dev);
	return dev->obj;
}

static exception_t free_McBSP_linker(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t reset_McBSP_linker(conf_object_t* opaque, const char* args)
{
	McBSP_linker_t* dev = opaque->obj;

	return No_exp;
}

static exception_t McBSP_tranf_data(conf_object_t* opaque,conf_object_t *McBSP,char *data,uint32_t datalength)
{
	McBSP_linker_t* dev = opaque->obj;
	if(dev->McBSP_device_intf[0] == NULL && dev->McBSP_device_intf[1] == NULL){
		return 0;
	}
	int index = (McBSP == dev->McBSP_device[0] ? 1 : 0);
	dev->McBSP_device_intf[index]->receive_data(dev->McBSP_device[index],data,datalength);
	return No_exp;
}

static McBSP_data_receive_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	McBSP_linker_t *dev = obj->obj;
	dev->McBSP_device[index] = obj2;
	dev->McBSP_device_intf[index] = (McBSP_data_receive_intf*)SKY_get_iface(dev->McBSP_device[index], McBSP_DATA_RECEIVE_INTF);

	if (dev->McBSP_device_intf[index] == NULL){
		if (dev->McBSP_device[index] == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", McBSP_DATA_RECEIVE_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", McBSP_DATA_RECEIVE_INTF, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static McBSP_data_receive_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	McBSP_linker_t *dev = obj->obj;
	*obj2 = dev->McBSP_device[index];
	*port = NULL;
	return No_exp;
}

void init_McBSP_linker(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "McBSP_linker",
		.class_desc = "McBSP_linker",
		.new_instance = new_McBSP_linker,
		.reset_instance = reset_McBSP_linker,
		.free_instance = free_McBSP_linker,
		.get_attr = NULL,
		.set_attr = NULL,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static McBSP_linker_data_connect_intf McBSP_data_tranf =
	{
		.McBSP_linker_data_send = McBSP_tranf_data,
	};
	SKY_register_iface(clss, McBSP_LINKER_DATA_CONNECT_INTF, &McBSP_data_tranf);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription){
			.name = McBSP_LINKER_DATA_CONNECT_INTF,
			.iface = &McBSP_data_tranf
		}
	};

	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription){
			.name = McBSP_DATA_RECEIVE_INTF,
			.set = McBSP_data_receive_set,
			.get = McBSP_data_receive_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

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
#include "upp_linker.h"

static conf_object_t* new_upp_linker(char *obj_name)
{
	upp_linker_t* dev = skyeye_mm_zero(sizeof(upp_linker_t));
	dev->obj = new_conf_object(obj_name, dev);
	return dev->obj;
}
static exception_t free_upp_linker(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
static exception_t reset_upp_linker(conf_object_t* opaque, const char* args)
{
	upp_linker_t *rs422_linker = opaque->obj;
	
	return No_exp;        
}
static upp_data_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	upp_linker_t *dev = obj->obj;
	dev->upp[index] = obj2;
	dev->upp_data_signal[index] = (upp_data_signal_intf*)SKY_get_iface(dev->upp[index], UPP_DATA_SIGNAL_INTF);
	return No_exp;
}
static upp_data_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	upp_linker_t *dev = obj->obj;
	*obj2 = dev->upp[index];
	*port = NULL;
	return No_exp;
}
static int upp_tranf_data(conf_object_t *obj, conf_object_t *upp,uint32_t channel, char *data, uint32_t datalength)
{
	upp_linker_t *dev = obj->obj;
    if(dev->upp_data_signal[0] == NULL && dev->upp_data_signal[1] == NULL){
        return 0;
    }
    int index = (upp == dev->upp[0] ? 1 : 0);
    return dev->upp_data_signal[index]->send_data(dev->upp[index],channel,data,datalength);

}
void init_upp_linker(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "upp_linker",
		.class_desc = "upp_linker",
		.new_instance = new_upp_linker,
		.reset_instance = reset_upp_linker,
		.free_instance = free_upp_linker,
		.get_attr = NULL,
		.set_attr = NULL,
	};
	conf_class_t* clss = SKY_register_linker_class(class_data.class_name, &class_data);
    static upp_linker_data_tranf_intf upp_data_tranf = 
    {
        .upp_linker_data_send = upp_tranf_data,
    };
    SKY_register_iface(clss, UPP_LINKER_DATA_TRANF_INTF, &upp_data_tranf);
	static const struct InterfaceDescription ifaces[] = 
	{
        (struct InterfaceDescription){
            .name = UPP_LINKER_DATA_TRANF_INTF,
            .iface = &upp_data_tranf,
        }
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
        (struct ConnectDescription){
            .name = UPP_DATA_SIGNAL_INTF,
            .set = upp_data_signal_set,
            .get = upp_data_signal_get,
        }
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

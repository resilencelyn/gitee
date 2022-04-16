#include <string.h>
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
#include "gpio_linker.h"

#define GPIO_CONNECT_FORMAT "GPIO_NUM0.PIN1.PIN2...PINn;GPIO_NUM1.PIN4.pin5...PINn\n \
                            The connection format of the GPIO is as shown above,\n \
                            and ';' is used to represent two different boards,\n \
                            '.' Is used to split the GPIO number and PIN\n \ 
                            GPIO_NUM0.PIN1.PIN2...PINn;\n\
                                       |    |  ... |   \n\
                            GPIO_NUM1.PIN4.pin5...PINn\n\
                            Note: GPIO_NUM must be 0 or 1"
                            

static conf_object_t* new_gpio_linker(char *obj_name)
{
	gpio_linker_t* dev = skyeye_mm_zero(sizeof(gpio_linker_t));
	dev->obj = new_conf_object(obj_name, dev);
    memset(dev->connect_pin[0],-1,128*4);
    memset(dev->connect_pin[1],-1,128*4);
	return dev->obj;
}
static exception_t free_gpio_linker(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
static exception_t reset_gpio_linker(conf_object_t* opaque, const char* args)
{
	gpio_linker_t *rs422_linker = opaque->obj;
	
	return No_exp;        
}
static exception_t gpio_pin_data_set(conf_object_t *obj,conf_object_t *obj2, const char *port, int index)
{
	gpio_linker_t *dev = obj->obj;
    dev->gpio[index] = obj2;
	dev->gpio_intf[index] = (gpio_pin_set **)SKY_get_iface(dev->gpio[index], GPIO_PIN_SET);
	if (dev->gpio_intf[index] == NULL){
		if (dev->gpio[index] == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GPIO_PIN_SET);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GPIO_PIN_SET, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
static exception_t gpio_pin_data_get(conf_object_t *obj,conf_object_t **obj2, const char **port, int index)
{
	gpio_linker_t *dev = obj->obj;
    *obj2 = dev->gpio[index];
    *port = NULL;
    return No_exp;
}

static attr_value_t get_attr_pin_num(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	gpio_linker_t *dev = conf_obj->obj;
	attr_value_t dev_id = SKY_make_attr_string(dev->connect_list);
	return dev_id;
}

static exception_t set_attr_pin_num(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	gpio_linker_t *dev = conf_obj->obj;
	dev->connect_list = SKY_attr_string(*value);
    char tranf[256] = "";
    strcpy(tranf,dev->connect_list);
    char *delim_gpio = ";";
    char *delim_pin = ".";
    char *swap;
    int gpio_num;
    char str[2][128];
    strcpy(str[0],strtok(tranf,delim_gpio));
    strcpy(str[1],strtok(NULL,delim_gpio));
    int i=0,j=0;
    for(i=0; i<2; i++)
    {
        gpio_num = atoi(strtok(str[i],delim_pin)); 
        j=0;
        while(swap = strtok(NULL,delim_pin)){
            dev->connect_pin[gpio_num][j] = atoi(swap);
            j++;
        }
        dev->group_pin[i] = j;
    }
	return No_exp;
} 



static void gpio_link_register_attribute(conf_class_t* clss){
    SKY_register_attribute(clss,"pin_connect",get_attr_pin_num,NULL,set_attr_pin_num,NULL,SKY_Attr_Optional,"string",GPIO_CONNECT_FORMAT);
}

static exception_t linker_gpio_pin_change(conf_object_t *obj, conf_object_t *gpio,uint32_t pin_num,uint32_t status)
{
	gpio_linker_t *dev = obj->obj;
    int gpio_num = (dev->gpio[0] == gpio ? 1 : 0);
    int i = 0;
    for(i=0; i < dev->group_pin[!gpio_num]; i++)
    {
        if(dev->connect_pin[!gpio_num][i] == pin_num){
            break;
        }
    }
    if(dev->connect_pin[gpio_num][i] != -1){
        if(dev->gpio_intf[gpio_num] != NULL){
            dev->gpio_intf[gpio_num]->set_gpio(dev->gpio[gpio_num],dev->connect_pin[gpio_num][i],status);   
        }else{
            skyeye_log(Error_log, __FUNCTION__, "gpio:%d pin:%d is not connected to other gpio pins.", gpio_num, pin_num);
        }
    }

    return No_exp;
}

void init_gpio_linker(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "gpio_linker",
		.class_desc = "gpio_linker",
		.new_instance = new_gpio_linker,
		.reset_instance = reset_gpio_linker,
		.free_instance = free_gpio_linker,
		.get_attr = NULL,
		.set_attr = NULL,
	};
	conf_class_t* clss = SKY_register_linker_class(class_data.class_name, &class_data);

    static const gpio_linker_pin_intf change_pin_state = {
        .linker_pin_change_state = linker_gpio_pin_change, 
    };
    SKY_register_iface(clss,GPIO_LINKER_PIN_INTF,&change_pin_state);

	static const struct InterfaceDescription ifaces[] = 
	{
        (struct InterfaceDescription){
            .name = GPIO_LINKER_PIN_INTF,
            .iface = &change_pin_state,
        },
	};
	static const struct ConnectDescription connects[] = 
	{
        (struct ConnectDescription){
            .name = GPIO_PIN_SET,
            .set = gpio_pin_data_set,
            .get = gpio_pin_data_get,
        },
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

    gpio_link_register_attribute(clss);
}

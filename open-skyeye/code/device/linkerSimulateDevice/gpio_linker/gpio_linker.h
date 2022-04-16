#ifndef __GPIO_LINKER_H__ 
#define __GPIO_LINKER_H__ 

struct gpio_linker 
{
	conf_object_t* obj;
    conf_object_t* gpio[2];
    gpio_pin_set* gpio_intf[2];
    char *connect_list;
    int connect_pin[2][128];
    int group_pin[2];
};
typedef struct gpio_linker gpio_linker_t;
#endif  

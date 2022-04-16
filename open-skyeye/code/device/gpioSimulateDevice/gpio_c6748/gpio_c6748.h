#ifndef __GPIO_C6748_H__
#define __GPIO_C6748_H__
struct registers
{

	uint32_t ctrl_data;

	union {
		uint32_t all;
		struct {
			uint32_t pin0 :1;
			uint32_t pin1 :1;
			uint32_t pin2 :1;
			uint32_t pin3 :1;
			uint32_t pin4 :1;
			uint32_t pin5 :1;
			uint32_t pin6 :1;
			uint32_t pin7 :1;
		};
	}gpio_pin;
};


struct c6748_device
{

	conf_object_t* obj;
	struct registers regs;

	core_signal_intf* signal_iface;
	conf_object_t* signal;

	skyeye_gpio_connector_intf *gpio_connector_iface;
	conf_object_t* gpio_connector;

};
typedef struct c6748_device gpio_c6748_device;
static char* regs_name[] =
{
	"ctrl_data",
	"gpio_pin",
	NULL
};
#define CTRL_DATA  0x0
#define GPIO_PIN0 0x4
#define GPIO_PIN1 0x8
#define GPIO_PIN2 0xC
#define GPIO_PIN3 0x10
#define GPIO_PIN4 0x14
#define GPIO_PIN5 0x18
#define GPIO_PIN6 0x1c
#define GPIO_PIN7 0x20
#endif

#ifndef __FT2000A_GPIO_H__
#define __FT2000A_GPIO_H__

#define PORT_NUM 4

typedef struct
{
	uint32_t gpio_swport_dr; //端口数据寄存器
	uint32_t gpio_swport_ddr;//端口数据方向寄存器
}gpio_swport_t;

struct gpio_registers
{
	gpio_swport_t gpio_swport[PORT_NUM];	//端口 A-D
	uint32_t gpio_ext_port[PORT_NUM];	//端口 A-D 外部端口寄存器
};

struct ft2000a_gpio_device_t
{
	conf_object_t* obj;
	struct gpio_registers regs;
};

typedef struct ft2000a_gpio_device_t ft2000a_gpio_device;

static char* regs_name[] =
{
	"gpio_swporta_dr",
	"gpio_swporta_ddr",
	"gpio_swportb_dr",
	"gpio_swportb_ddr",
	"gpio_swportc_dr",
	"gpio_swportc_ddr",
	"gpio_swportd_dr",
	"gpio_swportd_ddr",
	"gpio_ext_porta",
	"gpio_ext_portb",
	"gpio_ext_portc",
	"gpio_ext_portd",
	NULL
};

#define GPIO_SWPORTA_DR       0x00
#define GPIO_SWPORTA_DDR      0x04
#define GPIO_SWPORTB_DR       0x0c
#define GPIO_SWPORTB_DDR      0x10
#define GPIO_SWPORTC_DR       0x18
#define GPIO_SWPORTC_DDR      0x1c
#define GPIO_SWPORTD_DR       0x24
#define GPIO_SWPORTD_DDR      0x28
#define GPIO_EXT_PORTA        0x50
#define GPIO_EXT_PORTB        0x54
#define GPIO_EXT_PORTC        0x58
#define GPIO_EXT_PORTD        0x5c

static uint32_t regs_offset[] =
{
	GPIO_SWPORTA_DR,
	GPIO_SWPORTA_DDR,
	GPIO_SWPORTB_DR,
	GPIO_SWPORTB_DDR,
	GPIO_SWPORTC_DR,
	GPIO_SWPORTC_DDR,
	GPIO_SWPORTD_DR,
	GPIO_SWPORTD_DDR,
	GPIO_EXT_PORTA,
	GPIO_EXT_PORTB,
	GPIO_EXT_PORTC,
	GPIO_EXT_PORTD,
};

#endif

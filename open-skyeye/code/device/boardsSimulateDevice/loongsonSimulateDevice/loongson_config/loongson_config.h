#ifndef __loongson_CONFIG_H__
#define __loongson_CONFIG_H__

// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	"int_en",
	"int_edge",
	"int_pol",
	"int_clr",
	"int_set",
	"int_out",
	"sram_af",
	"gpio_mux",
	"gpio_o",
	"gpio_e",
	"gpio_i",
	"vr_cfg",
	"init_i",
	"bandgap",
	"sram_errad",
	"lcnt",
	NULL
};

// Register offset
#define INT_EN    0x0
#define INT_EDGE  0x4
#define INT_POL   0x8
#define INT_CLR   0xc
#define INT_SET   0x10
#define INT_OUT   0x14
#define SRAM_AF   0x34
#define GPIO_MUX  0x18
#define GPIO_O    0x1c
#define GPIO_E    0x20
#define GPIO_I    0x24
#define VR_CFG    0x28
#define INIT_I    0x2c
#define BANDGAP   0x30
#define SRAM_ERRAD 0x38
#define LCNT      0x3c

// device register structure
struct registers
{
	uint32_t int_en;
	uint32_t int_edge;
	uint32_t int_pol;
	uint32_t int_clr;
	uint32_t int_set;
	uint32_t int_out;
	uint32_t sram_af;
	uint32_t gpio_mux;
	uint32_t gpio_o;
	uint32_t gpio_e;
	uint32_t gpio_i;
	uint32_t vr_cfg;
	uint32_t init_i;
	uint32_t bandgap;
	uint32_t sram_errad;
	uint32_t lcnt;
};

// Device object structure
struct loongson_config_device_t
{
	conf_object_t* obj;
	// Interface objects and methods;
	core_signal_intf    *signal_iface;
	conf_object_t       *signal;
	int IP_num;
	struct registers regs;
	
};
typedef struct loongson_config_device_t loongson_config_device;

#define IP7     7
#endif

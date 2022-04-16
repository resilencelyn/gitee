#ifndef __rm48x_gio_H__
#define __rm48x_gio_H__
struct registers 
{
	uint32_t giogcr0;
	uint32_t giointdet;
	uint32_t giopol;
	uint32_t gioenaset;
	uint32_t gioenaclr;
	uint32_t giolvlset;
	uint32_t giolvlclr;
	uint32_t gioflg;
	uint32_t giooff1;
	uint32_t giooff2;
	uint32_t gioemu1;
	uint32_t gioemu2;
	uint32_t giodira;
	uint32_t giodina;
	uint32_t giodouta;
	uint32_t giodseta;
	uint32_t giodclra;
	uint32_t giopdra;
	uint32_t giopuldisa;
	uint32_t giopsla;
	uint32_t giodirb;
	uint32_t giodinb;
	uint32_t giodoutb;
	uint32_t giodsetb;
	uint32_t giodclrb;
	uint32_t giopdrb;
	uint32_t giopuldisb;
	uint32_t giopslb;
};

typedef enum{
	BankA = 0,
	BankB

} Gio_Bank_t;

typedef enum{
	Gio_pin0 = 0,
	Gio_pin1,
	Gio_pin2,
	Gio_pin3,
	Gio_pin4,
	Gio_pin5,
	Gio_pin6,
	Gio_pin7,

} Gio_Pin_t;

struct rm48x_gio_device_t 
{
	conf_object_t* obj;

	core_signal_intf* signal_iface;
	conf_object_t* signal;

	conf_object_t* gpio_link;
	gpio_linker_pin_intf* gpio_linker_intf;
	struct registers regs;
	int connect_quanaity;
	int connect_pin[128];
	char *connect_list;
	uint32_t pin_num;
	char *circuit_display;
};
typedef struct rm48x_gio_device_t rm48x_gio_device;

static char* regs_name[] =
{
	"GIOGCR0",
	"GIOINTDET",
	"GIOPOL",
	"GIOENASET",
	"GIOENACLR",
	"GIOLVLSET",
	"GIOLVLCLR",
	"GIOFLG",
	"GIOOFF1",
	"GIOOFF2",
	"GIOEMU1",
	"GIOEMU2",
	"GIODIRA",
	"GIODINA",
	"GIODOUTA",
	"GIODSETA",
	"GIODCLRA",
	"GIOPDRA",
	"GIOPULDISA",
	"GIOPSLA",
	"GIODIRB",
	"GIODINB",
	"GIODOUTB",
	"GIODSETB",
	"GIODCLRB",
	"GIOPDRB",
	"GIOPULDISB",
	"GIOPSLB",
	NULL
};

#define GIOGCR0    0x00
#define GIOINTDET  0x08
#define GIOPOL     0x0c
#define GIOENASET  0x10
#define GIOENACLR  0x14
#define GIOLVLSET  0x18
#define GIOLVLCLR  0x1c
#define GIOFLG     0x20
#define GIOOFF1    0x24
#define GIOOFF2    0x28
#define GIOEMU1    0x2c
#define GIOEMU2    0x30
#define GIODIRA    0x34
#define GIODINA    0x38
#define GIODOUTA   0x3c
#define GIODSETA   0x40
#define GIODCLRA   0x44
#define GIOPDRA    0x48
#define GIOPULDISA 0x4c
#define GIOPSLA    0x50
#define GIODIRB    0x54
#define GIODINB    0x58
#define GIODOUTB   0x5c
#define GIODSETB   0x60
#define GIODCLRB   0x64
#define GIOPDRB    0x68
#define GIOPULDISB 0x6c
#define GIOPSLB    0x70
#endif

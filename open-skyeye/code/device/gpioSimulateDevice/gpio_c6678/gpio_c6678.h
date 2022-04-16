#ifndef __GPIO_C6678_H__
#define __GPIO_C6678_H__

struct registers
{
	uint32_t binten;			// Interrupt Per-Bank Ebable Register
	uint32_t dir;				// Directrion Register
	uint32_t out_data;			// Output Data register
	uint32_t set_data;			// Set Data Register
	uint32_t clr_data;			// Cleat Data Register
	uint32_t in_data;			// Input Data Register
	uint32_t set_ris_trig;
	uint32_t clr_ris_trig;
	uint32_t set_fal_trig;
	uint32_t clr_fal_trig;
};

struct c6678_device
{
	conf_object_t* obj;

	core_signal_intf* signal_iface;
	conf_object_t* signal;

	struct registers regs;
	uint32_t ris_trig;
	uint32_t fal_trig;
};
typedef struct c6678_device gpio_c6678_device;

static char* regs_name[] =
{
	"binten",
	"dir",
	"out_data",
	"set_data",
	"clr_data",
	"in_data",
	"set_ris_trig",
	"clr_ris_trig",
	"set_fal_trig",
	"clr_fal_trig",
	NULL
};

#define BINTEN			0x08
#define DIR				0x10
#define OUT_DATA		0x14
#define SET_DATA		0x18
#define CLR_DATA		0x1c
#define IN_DATA			0x20
#define SET_RIS_TRIG	0x24
#define CLR_RIS_TRIG	0x28
#define SET_FAL_TRIG	0x2c
#define CLR_FAL_TRIG	0x30
#endif

#ifndef __P2020_ELBC_H__
#define __P2020_ELBC_H__

#define ELBC_BR0  0x0
#define ELBC_ORG0 0x4
#define ELBC_BR1  0x8
#define ELBC_ORG1 0xc
#define ELBC_BR2  0x10
#define ELBC_ORG2 0x14
#define ELBC_BR3  0x18
#define ELBC_ORG3 0x1c
#define ELBC_BR4  0x20
#define ELBC_ORG4 0x24
#define ELBC_LBCR 0xd0
#define ELBC_LCRR 0xd4
#define ELBC_FMR  0xe0
// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	"elbc_br0",
	"elbc_org0",
	"elbc_br1",
	"elbc_org1",
	"elbc_br2",
	"elbc_org2",
	"elbc_br3",
	"elbc_org3",
	"elbc_br4",
	"elbc_org4",
	"elbc_lbcr",
	"elbc_lcrr",
	"elbc_fmr",
	NULL
};

// Register offset

// device register structure
struct registers
{
	uint32_t elbc_br0;
	uint32_t elbc_org0;
	uint32_t elbc_br1;
	uint32_t elbc_org1;
	uint32_t elbc_br2;
	uint32_t elbc_org2;
	uint32_t elbc_br3;
	uint32_t elbc_org3;
	uint32_t elbc_br4;
	uint32_t elbc_org4;
	uint32_t elbc_lbcr;
	uint32_t elbc_lcrr;
	uint32_t elbc_fmr;
};


// Interrupt manager interrupt signal interface
struct general_signal_s
{
	conf_object_t* obj;
	general_signal_iface_t* iface;
};


// Device object structure
struct p2020_elbc_device_t
{
	conf_object_t* obj;
	// Interface objects and methods;
	struct general_signal_s general_signal;
	struct registers regs;
	// attribute;
	uint32_t int_number;
	// timer;
	time_handle_t timer_handle[1];
};
typedef struct p2020_elbc_device_t p2020_elbc_device;

#endif

#ifndef __RM48X_IO_H__
#define __RM48X_IO_H__

struct registers
{
	uint32_t revision;
	uint32_t endian;
	uint32_t kick0;
	uint32_t kick1;
	uint32_t err_raw_status;
	uint32_t err_enabled_status;
	uint32_t err_enable_reg;
	uint32_t err_enable_clr;
	uint32_t fault_address;
	uint32_t fault_status;
	uint32_t fault_clear;
	uint32_t pinmmrnn[32];
};


struct rm48x_io_device_t
{
	conf_object_t* obj;

	core_signal_intf* signal_iface;
	conf_object_t* signal;

	struct registers regs;
};
typedef struct rm48x_io_device_t rm48x_io_device;

static char* regs_name[] =
{
	"revision",
	"endian",
	"kick0",
	"kick1",
	"err_raw_status",
	"err_enabled_status",
	"err_enable_reg",
	"err_enable_clr",
	"fault_address",
	"fault_status",
	"fault_clear",
	"pinmmrnn0",
	"pinmmrnn1",
	"pinmmrnn2",
	"pinmmrnn3",
	"pinmmrnn4",
	"pinmmrnn5",
	"pinmmrnn6",
	"pinmmrnn7",
	"pinmmrnn8",
	"pinmmrnn9",
	"pinmmrnn10",
	"pinmmrnn11",
	"pinmmrnn12",
	"pinmmrnn13",
	"pinmmrnn14",
	"pinmmrnn15",
	"pinmmrnn16",
	"pinmmrnn17",
	"pinmmrnn18",
	"pinmmrnn19",
	"pinmmrnn20",
	"pinmmrnn21",
	"pinmmrnn22",
	"pinmmrnn23",
	"pinmmrnn24",
	"pinmmrnn25",
	"pinmmrnn26",
	"pinmmrnn27",
	"pinmmrnn28",
	"pinmmrnn29",
	"pinmmrnn30",
	"pinmmrnn31",
	NULL
};

#define REVISION           0x0
#define ENDIAN             0x20
#define KICK0              0x38
#define KICK1              0x3c
#define ERR_RAW_STATUS     0xe0
#define ERR_ENABLED_STATUS 0xe4
#define ERR_ENABLE_REG     0xe8
#define ERR_ENABLE_CLR     0xec
#define FAULT_ADDRESS      0xf4
#define FAULT_STATUS       0xf8
#define FAULT_CLEAR        0xfc
#endif

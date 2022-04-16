#ifndef __LOONGSON_HPET_H__
#define __LOONGSON_HPET_H__

// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	"hpet_cfg",
	"hpet_cnt",
	"hpet_cmp",
	"hpet_stp",
	NULL
};

// Register offset
#define HPET_CFG 0x0
#define HPET_CNT 0x4
#define HPET_CMP 0x8
#define HPET_STP 0xc
// device register structure
struct registers
{
	uint32_t hpet_cfg;
	uint32_t hpet_cnt;
	uint32_t hpet_cmp;
	uint32_t hpet_stp;
};


// Device object structure
struct loongson_hpet_device_t
{
	
	conf_object_t* obj;
	
	// Interface objects and methods;
	conf_object_t* signal;
	general_signal_intf* signal_iface;
	
	struct registers regs;
	double cpu_freq;
	int interrupt_number;
};
typedef struct loongson_hpet_device_t loongson_hpet_device;

#endif

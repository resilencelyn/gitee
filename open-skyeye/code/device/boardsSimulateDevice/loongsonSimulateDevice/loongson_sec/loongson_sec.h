#ifndef __LOONGSON_SEC_H__
#define __LOONGSON_SEC_H__

// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	"wd_cfg",
	"wd_feed",
	"sec_cmd",
	"sec_sts",
	"sec_gpr0",
	"sec_gpr1",
	"sec_gate_sram",
	"sec_gate_flash",
	NULL
};

// Register offset
#define WD_CFG          0x0
#define WD_FEED         0x4
#define SEC_CMD			0x8
#define SEC_STS			0xc
#define SEC_GPR0		0x10
#define SEC_GPR1		0x14
#define SEC_GATE_SRAM   0x18
#define SEC_GATE_FLASH  0x1c 
// device register structure
struct registers
{
	uint32_t wd_cfg;
	uint32_t wd_feed;
	uint32_t sec_cmd;
	uint32_t sec_sts;
	uint32_t sec_gpr0;
	uint32_t sec_gpr1;
	uint32_t sec_gate_sram;
	uint32_t sec_gate_flash;
};


// Device object structure
struct loongson_sec_device_t
{
	
	conf_object_t* obj;
	
	// Interface objects and methods;
	struct registers regs;
	
};
typedef struct loongson_sec_device_t loongson_sec_device;

#endif

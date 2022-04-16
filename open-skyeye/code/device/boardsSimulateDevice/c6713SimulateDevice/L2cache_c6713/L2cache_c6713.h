#ifndef __L2CACHE_C6713_H__ 
#define __L2CACHE_C6713_H__ 
struct registers 
{
	
	uint32_t ccfg;
	
	uint32_t l2wbar;
	
	uint32_t l2wwc;
	
	uint32_t l2wibar;
	
	uint32_t l2wiwc;
	
	uint32_t l1pibar;
	
	uint32_t l1piwc;
	
	uint32_t l1dwibar;
	
	uint32_t l1dwiwc;
	
	uint32_t l2wb;
	
	uint32_t l2wbinv;

    uint32_t mar[16];
	
};


struct c6713_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;

    int num;
	
};
typedef struct c6713_device L2cache_c6713_device;
static char* regs_name[] = 
{
	"CCFG",
	"L2WBAR",
	"L2WWC",
	"L2WIBAR",
	"L2WIWC",
	"L1PIBAR",
	"L1PIWC",
	"L1DWIBAR",
	"L1DWIWC",
	"L2WB",
	"L2WBINV",
	NULL
};
#define CCFG 0x0000
#define L2WBAR 0x4000
#define L2WWC 0x4004
#define L2WIBAR 0x4010
#define L2WIWC 0x4014
#define L1PIBAR 0x4020
#define L1PIWC 0x4024
#define L1DWIBAR 0x4030
#define L1DWIWC 0x4034
#define L2WB 0x5000
#define L2WBINV 0x5004

#define CE0 0x1
#define CE1 0x2
#define CE2 0x3
#define CE3 0x4

#endif  

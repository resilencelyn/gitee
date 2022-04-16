#ifndef __MPC8378_DDR_H__ 
#define __MPC8378_DDR_H__ 
struct registers 
{
	
	uint32_t cs0_bnds;
	
	uint32_t cs1_bnds;
	
	uint32_t cs2_bnds;
	
	uint32_t cs3_bnds;
	
	uint32_t cs0_config;
	
	uint32_t cs1_config;
	
	uint32_t cs2_config;
	
	uint32_t cs3_config;
	
	uint32_t timing_cfg_3;
	
	uint32_t timing_cfg_0;
	
	uint32_t timing_cfg_1;
	
	uint32_t timing_cfg_2;
	
	uint32_t ddr_sdram_cfg;
	
	uint32_t ddr_sdram_cfg_2;
	
	uint32_t ddr_sdram_mode;
	
	uint32_t ddr_sdram_mode_2;
	
	uint32_t ddr_sdram_md_cntl;
	
	uint32_t ddr_sdram_interval;
	
	uint32_t ddr_data_init;
	
	uint32_t ddr_sdram_clk_cntl;
	
	uint32_t ddr_init_addr;
	
	uint32_t ddr_ip_rev1;
	
	uint32_t ddr_ip_rev2;
	
	uint32_t data_err_inject_hi;
	
	uint32_t data_err_inject_lo;
	
	uint32_t err_inject;
	
	uint32_t capture_data_hi;
	
	uint32_t capture_data_lo;
	
	uint32_t capture_ecc;
	
	uint32_t err_detect;
	
	uint32_t err_disable;
	
	uint32_t err_int_en;
	
	uint32_t capture_attributes;
	
	uint32_t capture_address;
	
	uint32_t err_sbe;
	
};


struct mpc8378_ddr_device_t 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct mpc8378_ddr_device_t mpc8378_ddr_device;
static char* regs_name[] = 
{
	"cs0_bnds",
	"cs1_bnds",
	"cs2_bnds",
	"cs3_bnds",
	"cs0_config",
	"cs1_config",
	"cs2_config",
	"cs3_config",
	"timing_cfg_3",
	"timing_cfg_0",
	"timing_cfg_1",
	"timing_cfg_2",
	"ddr_sdram_cfg",
	"ddr_sdram_cfg_2",
	"ddr_sdram_mode",
	"ddr_sdram_mode_2",
	"ddr_sdram_md_cntl",
	"ddr_sdram_interval",
	"ddr_data_init",
	"ddr_sdram_clk_cntl",
	"ddr_init_addr",
	"ddr_ip_rev1",
	"ddr_ip_rev2",
	"data_err_inject_hi",
	"data_err_inject_lo",
	"err_inject",
	"capture_data_hi",
	"capture_data_lo",
	"capture_ecc",
	"err_detect",
	"err_disable",
	"err_int_en",
	"capture_attributes",
	"capture_address",
	"err_sbe",
	NULL
};
#define CS0_BNDS 0x000
#define CS1_BNDS 0x008
#define CS2_BNDS 0x010
#define CS3_BNDS 0x018
#define CS0_CONFIG 0x080
#define CS1_CONFIG 0x084
#define CS2_CONFIG 0x088
#define CS3_CONFIG 0x08C
#define TIMING_CFG_3 0x100
#define TIMING_CFG_0 0x104
#define TIMING_CFG_1 0x108
#define TIMING_CFG_2 0x10C
#define DDR_SDRAM_CFG 0x110
#define DDR_SDRAM_CFG_2 0x114
#define DDR_SDRAM_MODE 0x118
#define DDR_SDRAM_MODE_2 0x11C
#define DDR_SDRAM_MD_CNTL 0x120
#define DDR_SDRAM_INTERVAL 0x124
#define DDR_DATA_INIT 0x128
#define DDR_SDRAM_CLK_CNTL 0x130
#define DDR_INIT_ADDR 0x148
#define DDR_IP_REV1 0xBF8
#define DDR_IP_REV2 0xBFC
#define DATA_ERR_INJECT_HI 0xE00
#define DATA_ERR_INJECT_LO 0xE04
#define ERR_INJECT 0xE08
#define CAPTURE_DATA_HI 0xE20
#define CAPTURE_DATA_LO 0xE24
#define CAPTURE_ECC 0xE28
#define ERR_DETECT 0xE40
#define ERR_DISABLE 0xE44
#define ERR_INT_EN 0xE48
#define CAPTURE_ATTRIBUTES 0xE4C
#define CAPTURE_ADDRESS 0xE50
#define ERR_SBE 0xE58
#endif  

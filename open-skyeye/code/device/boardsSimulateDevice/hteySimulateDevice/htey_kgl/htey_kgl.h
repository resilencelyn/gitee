#ifndef __HTEY_KGL_H__ 
#define __HTEY_KGL_H__ 
struct registers 
{
	
	uint32_t kg_1swidth;
	
	uint32_t kg_tongbu;
	
	uint32_t kg_paozhao;
	
	uint32_t kg_fenli;
	
	uint32_t kg_tianxian1;
	
	uint32_t kg_tianxian2;
	
	uint32_t kg_zikong;
	
	uint32_t kg_guikong;
	
	uint32_t kg_anquan;
	
	uint32_t kg_rstwidth;
	
	uint32_t kg_tlwidth;
	
	uint32_t kg_pzwidth;
	
	uint32_t kg_flwidth;
	
	uint32_t kg_pulseinput;
	
	uint32_t kg_input;
	
	uint32_t kg_zigui;
	
	uint32_t kg_tb_tst;
	
	uint32_t kg_pz_tst;
	
	uint32_t kg_fl_tst;
	
	uint32_t kg_rstlock;
	
};


struct kgl_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;

	
};
typedef struct kgl_device htey_kgl_device;
static char* regs_name[] = 
{
	"kg_1swidth",
	"kg_tongbu",
	"kg_paozhao",
	"kg_fenli",
	"kg_tianxian1",
	"kg_tianxian2",
	"kg_zikong",
	"kg_guikong",
	"kg_anquan",
	"kg_rstwidth",
	"kg_tlwidth",
	"kg_pzwidth",
	"kg_flwidth",
	"kg_pulseinput",
	"kg_input",
	"kg_zigui",
	"kg_tb_tst",
	"kg_pz_tst",
	"kg_fl_tst",
	"kg_rstlock",
	NULL
};
#define KG_1SWIDTH 0x00
#define KG_TONGBU 0x10
#define KG_PAOZHAO 0x20
#define KG_FENLI 0x30
#define KG_TIANXIAN1 0x40
#define KG_TIANXIAN2 0x50
#define KG_ZIKONG 0x60
#define KG_GUIKONG 0x70
#define KG_ANQUAN 0x80
#define KG_RSTWIDTH 0x90
#define KG_TLWIDTH 0xB0
#define KG_PZWIDTH 0xA0
#define KG_FLWIDTH 0xC0
#define KG_PULSEINPUT 0xD0
#define KG_INPUT 0xE0
#define KG_ZIGUI 0xF0
#define KG_TB_TST 0xD0
#define KG_PZ_TST 0xE0
#define KG_FL_TST 0xF0
#define KG_RSTLOCK 0x500
#endif  

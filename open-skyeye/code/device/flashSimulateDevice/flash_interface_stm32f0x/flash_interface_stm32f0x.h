#ifndef __FLASH_INTERFACE_STM32F0X_H__ 
#define __FLASH_INTERFACE_STM32F0X_H__ 
struct registers 
{
	
	uint32_t flash_acr;
	
	uint32_t flash_keyr;
	
	uint32_t flash_optkeyr;
	
	uint32_t flash_sr;
	
	uint32_t flash_cr;
	
	uint32_t flash_ar;
	
	uint32_t flash_obr;
	
	uint32_t flash_wrpr;
	
};


struct stm32f0x_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct stm32f0x_device flash_interface_stm32f0x_device;
static char* regs_name[] = 
{
	"FLASH_ACR",
	"FLASH_KEYR",
	"FLASH_OPTKEYR",
	"FLASH_SR",
	"FLASH_CR",
	"FLASH_AR",
	"FLASH_OBR",
	"FLASH_WRPR",
	NULL
};
#define FLASH_ACR 0x00
#define FLASH_KEYR 0x04
#define FLASH_OPTKEYR 0x08
#define FLASH_SR 0x0c
#define FLASH_CR 0x10
#define FLASH_AR 0x14
#define FLASH_OBR 0x1c
#define FLASH_WRPR 0x20
#endif  

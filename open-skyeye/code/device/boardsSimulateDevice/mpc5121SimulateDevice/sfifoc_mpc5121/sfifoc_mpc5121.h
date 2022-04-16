#ifndef __SFIFOC_MPC5121_H__ 
#define __SFIFOC_MPC5121_H__ 
struct registers 
{
	
	uint32_t fifo_cmd;
	
	uint32_t fifo_int;
	
	uint32_t fifo_dma;
	
	uint32_t fifo_axe;
	
	uint32_t fifo_debug;
	
};


struct mpc5121_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct mpc5121_device sfifoc_mpc5121_device;
static char* regs_name[] = 
{
	"fifo_cmd",
	"fifo_int",
	"fifo_dma",
	"fifo_axe",
	"fifo_debug",
	NULL
};
#define FIFO_CMD 0x0
#define FIFO_INT 0x4
#define FIFO_DMA 0x8
#define FIFO_AXE 0xc
#define FIFO_DEBUG 0x10
#endif  

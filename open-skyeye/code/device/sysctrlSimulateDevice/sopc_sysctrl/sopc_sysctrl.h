#ifndef __SOPC_SYSCTRL_H__
#define __SOPC_SYSCTRL_H__

struct registers
{
	uint32_t  gck_con;        //Timer Control Configure Register
	uint32_t  srst_con;       //Soft Reset Config Register
	uint32_t  pllm_cfg ;      //PLL Config Reigster
	uint32_t  fpgaif_con;     //FPGA Interface Config Register 1
	uint32_t  timer_div_con;  //Timer Div Config Register
	uint32_t  brm1553_con;    //1553B Config Register
	uint32_t  pad_ctrl0;      //PAD Multiplex Ctrl Register0
	uint32_t  pad_ctrl1;      //PAD Multiplex Ctrl Register1
	uint32_t  ver_sel;        //UART Valid Select Register
};

struct sopc_sysctrl_device_t
{
	conf_object_t* obj;
	core_signal_intf* signal_iface;
	conf_object_t* signal;
	struct registers regs;
};

typedef struct sopc_sysctrl_device_t sopc_sysctrl_device;

/*SYSCTRL Registers Base--0x69000000 Offset*/
#define GCK_CON           0x00
#define SRST_CON          0x04
#define PLLM_CFG          0x08
#define FPGAIF_CON        0x10
#define TIMER_DIV_CON     0x18
#define BRM1553_CON       0x24
#define PAD_DUPLI_CTRL0   0x28
#define PAD_DUPLI_CTRL1   0x2c
#define VER_SEL           0x30

static char* regs_name[] =
{
	"gck_con",
	"srst_con",
	"pllm_cfg",
	"fpgaif_con",
	"timer_div_con",
	"brm1553_con",
	"pad_ctrl0",
	"pad_ctrl1",
	"ver_sel",
	NULL
};

static int regs_offset[] =
{
	GCK_CON,
	SRST_CON,
	PLLM_CFG,
	FPGAIF_CON,
	TIMER_DIV_CON,
	BRM1553_CON,
	PAD_DUPLI_CTRL0,
	PAD_DUPLI_CTRL1,
	VER_SEL,
};
#endif

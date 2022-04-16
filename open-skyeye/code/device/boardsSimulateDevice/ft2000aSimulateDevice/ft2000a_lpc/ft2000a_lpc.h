#ifndef __FT2000A_LPC_H__
#define __FT2000A_LPC_H__

struct lpc_registers
{
	uint32_t int_mask;						//中断屏蔽寄存器
	uint32_t dma_devid_conf;				//DMA 设备 ID 配置寄存器
	uint32_t firmware_id_conf;				//firmware memory 设备 ID 选择配置寄存器
	uint32_t error_state;				//错误状态寄存器
	uint32_t int_conf;					//中断配置寄存器
	uint32_t firmware_len_conf;				//firmware memory 类型的报文长度配置寄存器
	uint32_t clr_int;						//清除中断寄存器
	uint32_t int_state;						//中断状态寄存器
	uint32_t long_timeout;				//长等待超时控制寄存器
	uint32_t apb_space_conf;				//配置 APB 接口地址的设备类型
};

struct ft2000a_lpc_device_t
{
	conf_object_t* obj;
	struct lpc_registers regs;
	conf_object_t* image;
	memory_space_intf* image_space;
};

typedef struct ft2000a_lpc_device_t ft2000a_lpc_device;

static char* regs_name[] =
{
	"int_mask",
	"dma_devid_conf",
	"firmware_id_conf",
	"error_state",
	"int_conf",
	"firmware_len_conf",
	"clr_int",
	"int_state",
	"long_timeout",
	"apb_space_conf",
	NULL
};

#define INT_MASK               0x7ffffd8
#define DMA_DEVID_CONF         0x7ffffdc
#define FIRMW_ID_CONF          0x7ffffe0
#define ERROR_STATE            0x7ffffe4
#define INT_CONF               0x7ffffe8
#define FIRMW_LEN_CONF         0x7ffffec
#define CLR_INT                0x7fffff0
#define INT_STATE              0x7fffff4
#define LONG_TIMEOUT           0x7fffff8
#define INT_APB_SPACE_CONF     0x7fffffc

static uint32_t regs_offset[] =
{
	INT_MASK,
	DMA_DEVID_CONF,
	FIRMW_ID_CONF,
	ERROR_STATE,
	INT_CONF,
	FIRMW_LEN_CONF,
	CLR_INT,
	INT_STATE,
	LONG_TIMEOUT,
	INT_APB_SPACE_CONF,
};

#endif

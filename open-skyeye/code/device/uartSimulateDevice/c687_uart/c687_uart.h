#ifndef __C687_UART_H__ 
#define __C687_UART_H__ 
struct registers 
{
	uint32_t  uart_rev_send_reg;
	uint32_t  uart_comm_ctr_reg;
	uint32_t  uart_baud_reg;
	uint32_t  uart_comm_sta_reg;
	uint32_t  uart_int_ctr_reg;
	uint32_t  uart_int_flg_reg;
};


struct c687_uart_device_t 
{
	struct registers regs;
	
	conf_object_t* obj;
	
	general_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	skyeye_uart_intf *term_iface;
	
	conf_object_t *term;
	
	
	int num;
	
};
typedef struct c687_uart_device_t c687_uart_device;
static char* regs_name[] = 
{
	"uart_rev_send_reg",
	"uart_comm_ctr_reg",
	"uart_baud_reg",
	"uart_comm_sta_reg",
	"uart_int_ctr_reg",
	"uart_int_flg_reg",
	NULL
};
#define  UART_REV_SEND_REG     0x0//数据收发寄存器
#define  UART_COMM_CTR_REG     0x0C//通信控制寄存器
#define  UART_BAUD_REG         0x0//除数锁存器..由通信控制寄存器的最高位置1控制
#define  UART_COMM_STA_REG     0x14//通信状态寄存器
#define  UART_INT_CTR_REG      0x4//中断控制寄存器
#define  UART_INT_FLG_REG      0x8//中断标志寄存器
#endif  

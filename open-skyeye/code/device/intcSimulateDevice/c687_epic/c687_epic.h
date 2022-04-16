#ifndef __C687_EPIC_H__
#define __C687_EPIC_H__

struct registers
{
	uint32_t int_mask; // 中断屏蔽寄存器
	uint32_t int_eoi;  // 中断结束寄存器
	uint32_t int_ack;  // 中断响应寄存器
};


struct c687_epic_device_t
{
	conf_object_t    * obj;

	core_signal_intf * signal_iface;
	conf_object_t    * signal_obj;

	general_signal_intf *pic_mpc8641d_signal_iface;
	conf_object_t *pic_mpc8641d_signal_obj;

	// 临时方案: 使用MPC8641D PIC中断控制器

//#define M_INT_VECTOR_MAP_SIZE 0x60
#define M_INT_VECTOR_MAP_SIZE 60
	uint32_t irq_map[M_INT_VECTOR_MAP_SIZE];
//#define M_INT_VECTOR_COM_DEBUG 0x50
//#define M_INT_VECTOR_COM_DISPLAY 0x51
//#define M_INT_VECTOR_TIMER1 0x52
//#define M_INT_VECTOR_TIMER2 0x53

// 修改
#define M_INT_VECTOR_COM_DEBUG   50
#define M_INT_VECTOR_COM_DISPLAY 51
#define M_INT_VECTOR_TIMER1      52
#define M_INT_VECTOR_TIMER2      53

#define M_INT_VECTOR_ERROR       0xFFFFFFFF

	struct registers regs;
};
typedef struct c687_epic_device_t c687_epic_device;

static char* regs_name[] =
{
	"INT_MASK",
	"INT_EOI",
	"INT_ACK",
	NULL
};

#define INT_MASK 0x04
#define INT_EOI  0x0
#define INT_ACK  0x10

#endif

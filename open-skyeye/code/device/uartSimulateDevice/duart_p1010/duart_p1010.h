#ifndef __DUART_P1010_H__
#define __DUART_P1010_H__
#include <stdint.h>

#define MASK(a, b) ({uint32_t mask = (b) - (a) == 31 ? 0xffffffff : ((1u << ((b) - (a) + 1)) - 1); mask;})
#define CLRS(val, a, b) (val) = (val) & ~(MASK(a, b) << (a))
#define SETS(val, a, b, v) do {		\
	CLRS(val, a, b);		\
	(val) = (val) | (v) << (a);	\
} while(0)
#define BITS(val, a, b) (((val) >> (a)) & ((1 << (1 + (b) - (a))) - 1))

struct registers {
	uint8_t urbr1;
	uint8_t uthr1;
	uint8_t udlb1;


	uint8_t udmb1;
	uint8_t uier1;

	uint8_t uiir1;
	uint8_t ufcr1;
	uint8_t uafr1;

	uint8_t ulcr1;
	uint8_t umcr1;
	uint8_t ulsr1;
	uint8_t umsr1;
	uint8_t uscr1;
	uint8_t udsr1;
};


struct p1010_device {
	conf_object_t* obj;
	struct registers regs;

	conf_object_t *term;
	skyeye_uart_intf *term_iface;
	uint32_t fifo_empty;

	conf_object_t *signal;
	general_signal_intf *signal_iface;

	uint32_t irq_num;
	int dev_enable;

};
typedef struct p1010_device duart_p1010_device;

#define F_TRUE  1
#define F_FALSE 0

static char* regs_name[] = {
	"urbr1",
	"uthr1",
	"udlb1",

	"udmb1",
	"uier1",

	"uiir1",
	"ufcr1",
	"uafr1",

	"ulcr1",
	"umcr1",
	"ulsr1",
	"umsr1",
	"uscr1",
	"udsr1",
	NULL
};
#define URBR1 0x00
#define UTHR1 0x00
#define UDLB1 0x00

#define UDMB1 0x01
#define UIER1 0x01

#define UIIR1 0x02
#define UFCR1 0x02
#define UAFR1 0x02

#define ULCR1 0x03
#define UMCR1 0x04
#define ULSR1 0x05
#define UMSR1 0x06
#define USCR1 0x07
#define UDSR1 0x10

#define ETHREI 0x2
#endif

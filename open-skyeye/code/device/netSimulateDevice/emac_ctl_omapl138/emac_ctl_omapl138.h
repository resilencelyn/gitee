#ifndef __EMAC_CTL_OMAPL138_H__ 
#define __EMAC_CTL_OMAPL138_H__ 
#include <stdint.h>

#define MASK(a, b) ({uint32_t mask = (b) - (a) == 31 ? 0xffffffff : ((1u << ((b) - (a) + 1)) - 1); mask;})
#define CLRS(val, a, b) (val) = (val) & ~(MASK(a, b) << (a))
#define SETS(val, a, b, v) do {		\
	CLRS(val, a, b);		\
	(val) = (val) | (v) << (a);	\
} while(0)
#define BITS(val, a, b) (((val) >> (a)) & ((1 << (1 + (b) - (a))) - 1))

struct registers 
{
	uint32_t revid;
	uint32_t softreset;
	uint32_t intcontrol;
	uint32_t c0rxthreshen;
	uint32_t c0rxen;
	uint32_t c0txen;
	uint32_t c0miscen;
	uint32_t c1rxthreshen;
	uint32_t c1rxen;
	uint32_t c1txen;
	uint32_t c1miscen;
	uint32_t c2rxthreshen;
	uint32_t c2rxen;
	uint32_t c2txen;
	uint32_t c2miscen;
	uint32_t c0rxthreshstat;
	uint32_t c0rxstat;
	uint32_t c0txstat;
	uint32_t c0miscstat;
	uint32_t c1rxthreshstat;
	uint32_t c1rxstat;
	uint32_t c1txstat;
	uint32_t c1miscstat;
	uint32_t c2rxthreshstat;
	uint32_t c2rxstat;
	uint32_t c2txstat;
	uint32_t c2miscstat;
	uint32_t c0rximax;
	uint32_t c0tximax;
	uint32_t c1rximax;
	uint32_t c1tximax;
	uint32_t c2rximax;
	uint32_t c2tximax;
};

struct omapl138_device 
{
	
	conf_object_t* obj;
	core_signal_intf* signal_iface;
	conf_object_t* signal;
	struct registers regs;
};
typedef struct omapl138_device emac_ctl_omapl138_device;
static char* regs_name[] = 
{
	"revid",
	"softreset",
	"intcontrol",
	"c0rxthreshen",
	"c0rxen",
	"c0txen",
	"c0miscen",
	"c1rxthreshen",
	"c1rxen",
	"c1txen",
	"c1miscen",
	"c2rxthreshen",
	"c2rxen",
	"c2txen",
	"c2miscen",
	"c0rxthreshstat",
	"c0rxstat",
	"c0txstat",
	"c0miscstat",
	"c1rxthreshstat",
	"c1rxstat",
	"c1txstat",
	"c1miscstat",
	"c2rxthreshstat",
	"c2rxstat",
	"c2txstat",
	"c2miscstat",
	"c0rximax",
	"c0tximax",
	"c1rximax",
	"c1tximax",
	"c2rximax",
	"c2tximax",
	NULL
};
#define REVID 0x000
#define SOFTRESET 0x004
#define INTCONTROL 0x00c
#define C0RXTHRESHEN 0x010
#define C0RXEN 0x014
#define C0TXEN 0x018
#define C0MISCEN 0x01c
#define C1RXTHRESHEN 0x020
#define C1RXEN 0x024
#define C1TXEN 0x028
#define C1MISCEN 0x02c
#define C2RXTHRESHEN 0x030
#define C2RXEN 0x034
#define C2TXEN 0x038
#define C2MISCEN 0x03c
#define C0RXTHRESHSTAT 0x040
#define C0RXSTAT 0x044
#define C0TXSTAT 0x048
#define C0MISCSTAT 0x04c
#define C1RXTHRESHSTAT 0x050
#define C1RXSTAT 0x054
#define C1TXSTAT 0x058
#define C1MISCSTAT 0x05c
#define C2RXTHRESHSTAT 0x060
#define C2RXSTAT 0x064
#define C2TXSTAT 0x068
#define C2MISCSTAT 0x06c
#define C0RXIMAX 0x070
#define C0TXIMAX 0x074
#define C1RXIMAX 0x078
#define C1TXIMAX 0x07c
#define C2RXIMAX 0x080
#define C2TXIMAX 0x084
#endif  

#ifndef __EMAC_MDIO_OMAPL138_H__ 
#define __EMAC_MDIO_OMAPL138_H__ 
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
	
	uint32_t control;
	
	uint32_t alive;
	
	uint32_t link;
	
	uint32_t linkintraw;
	
	uint32_t linkintmasked;
	
	uint32_t userintraw;
	
	uint32_t userintmasked;
	
	uint32_t userintmaskset;
	
	uint32_t userintmaskclear;
	
	uint32_t useraccess0;
	
	uint32_t userphysel0;
	
	uint32_t useraccess1;
	
	uint32_t userphysel1;
	
};

struct lan8710a {
	uint16_t basic_control;
	uint16_t basic_status;
	uint16_t phy_identifier1;
	uint16_t phy_identifier2;
	uint16_t auto_neg_adv;
	uint16_t auto_neg_link;
	uint16_t auto_neg_exp;
	uint16_t mode_control_status;
	uint16_t special_mode;
	uint16_t symbol_error;
	uint16_t control_status_id;
	uint16_t int_src;
	uint16_t int_mask;
	uint16_t phy_special_control_status;
};

#define BASIC_CONTROL			0
#define BASIC_STATUS			1
#define PHY_IDENTIFIER1			2
#define PHY_IDENTIFIER2			3
#define AUTO_NEG_ADV			4
#define AUTO_NEG_LINK			5
#define AUTO_NEG_EXP			6
#define MODE_CONTROL_STATUS		7
#define SPECIAL_MODE			8
#define SYMBOL_ERROR			9
#define CONTROL_STATUS_ID		10
#define INT_SRC				11
#define INT_MASK			12
#define PHY_SPECIAL_CONTROL_STATUS	13


struct omapl138_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	struct lan8710a lan_regs;
	uint32_t intmask;
	
};
typedef struct omapl138_device emac_mdio_omapl138_device;
static char* regs_name[] = 
{
	"revid",
	"control",
	"alive",
	"link",
	"linkintraw",
	"linkintmasked",
	"userintraw",
	"userintmasked",
	"userintmaskset",
	"userintmaskclear",
	"useraccess0",
	"userphysel0",
	"useraccess1",
	"userphysel1",
	NULL
};
#define REVID 0x000
#define CONTROL 0x004
#define ALIVE 0x008
#define LINK 0x00c
#define LINKINTRAW 0x010
#define LINKINTMASKED 0x014
#define USERINTRAW 0x020
#define USERINTMASKED 0x024
#define USERINTMASKSET 0x028
#define USERINTMASKCLEAR 0x02c
#define USERACCESS0 0x080
#define USERPHYSEL0 0x084
#define USERACCESS1 0x088
#define USERPHYSEL1 0x08c
#endif  

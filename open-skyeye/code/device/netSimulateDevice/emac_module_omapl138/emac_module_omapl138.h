#ifndef __EMAC_MODULE_OMAPL138_H__ 
#define __EMAC_MODULE_OMAPL138_H__ 
#include <stdint.h>
struct registers 
{
	uint32_t txrevid;
	uint32_t txcontrol;
	uint32_t txteardown;
	uint32_t rxrevid;
	uint32_t rxcontrol;
	uint32_t rxteardown;
	uint32_t txintstatraw;
	uint32_t txintstatmasked;
	uint32_t txintmaskset;
	uint32_t txintmaskclear;
	uint32_t macinvector;
	uint32_t maceoivector;
	uint32_t rxintstatraw;
	uint32_t rxintstatmasked;
	uint32_t rxintmaskset;
	uint32_t rxintmaskclear;
	uint32_t macintstatraw;
	uint32_t macintstatmasked;
	uint32_t macintmaskset;
	uint32_t macintmaskclear;
	uint32_t rxmbpenable;
	uint32_t rxunicastset;
	uint32_t rxunicastclear;
	uint32_t rxmaxlen;
	uint32_t rxbufferoffset;
	uint32_t rxfilterlowthresh;
	uint32_t rx0flowthresh;
	uint32_t rx1flowthresh;
	uint32_t rx2flowthresh;
	uint32_t rx3flowthresh;
	uint32_t rx4flowthresh;
	uint32_t rx5flowthresh;
	uint32_t rx6flowthresh;
	uint32_t rx7flowthresh;
	uint32_t rx0freebuffer;
	uint32_t rx1freebuffer;
	uint32_t rx2freebuffer;
	uint32_t rx3freebuffer;
	uint32_t rx4freebuffer;
	uint32_t rx5freebuffer;
	uint32_t rx6freebuffer;
	uint32_t rx7freebuffer;
	uint32_t maccontrol;
	uint32_t macstatus;
	uint32_t emcontrol;
	uint32_t fifocontrol;
	uint32_t macconfig;
	uint32_t softreset;
	uint32_t macsrcaddrlo;
	uint32_t macsrcaddrhi;
	uint32_t machash1;
	uint32_t machash2;
	uint32_t bofftest;
	uint32_t tpacetest;
	uint32_t rxpause;
	uint32_t txpause;
	uint32_t macaddrlo;
	uint32_t macaddrhi;
	uint32_t macindex;
	uint32_t tx0hdp;
	uint32_t tx1hdp;
	uint32_t tx2hdp;
	uint32_t tx3hdp;
	uint32_t tx4hdp;
	uint32_t tx5hdp;
	uint32_t tx6hdp;
	uint32_t tx7hdp;
	uint32_t rx0hdp;
	uint32_t rx1hdp;
	uint32_t rx2hdp;
	uint32_t rx3hdp;
	uint32_t rx4hdp;
	uint32_t rx5hdp;
	uint32_t rx6hdp;
	uint32_t rx7hdp;
	uint32_t tx0cp;
	uint32_t tx1cp;
	uint32_t tx2cp;
	uint32_t tx3cp;
	uint32_t tx4cp;
	uint32_t tx5cp;
	uint32_t tx6cp;
	uint32_t tx7cp;
	uint32_t rx0cp;
	uint32_t rx1cp;
	uint32_t rx2cp;
	uint32_t rx3cp;
	uint32_t rx4cp;
	uint32_t rx5cp;
	uint32_t rx6cp;
	uint32_t rx7cp;
	uint32_t rxgoodframes;
	uint32_t rxbcastframes;
	uint32_t rxmcastframes;
	uint32_t rxpauseframes;
	uint32_t rxcrcerrors;
	uint32_t rxaligncodeerrors;
	uint32_t rxoversized;
	uint32_t rxjabber;
	uint32_t rxundersized;
	uint32_t rxfragments;
	uint32_t rxfiltered;
	uint32_t rxqosfiltered;
	uint32_t rxoctets;
	uint32_t txgoodframes;
	uint32_t txbcastframes;
	uint32_t txmcastframes;
	uint32_t txpauseframes;
	uint32_t txdeferred;
	uint32_t txcollision;
	uint32_t txsinglecoll;
	uint32_t txmulticoll;
	uint32_t txexcessivecoll;
	uint32_t txlatecoll;
	uint32_t txunderrun;
	uint32_t txcarriersense;
	uint32_t txoctets;
	uint32_t frame64;
	uint32_t frame65t127;
	uint32_t frame128t255;
	uint32_t frame256t511;
	uint32_t frame512t1023;
	uint32_t frame1024tup;
	uint32_t netoctets;
	uint32_t rxsofoverruns;
	uint32_t rxmofoverruns;
	uint32_t rxdmaoverruns;
};


#define EMAC_DSC_FLAG_SOP 0x8000u
#define EMAC_DSC_FLAG_EOP 0x4000u
#define EMAC_DSC_FLAG_OWNER 0x2000u
#define EMAC_DSC_FLAG_EOQ 0x1000u
#define EMAC_DSC_FLAG_TDOWNCMPLT 0x0800u
#define EMAC_DSC_FLAG_PASSCRC 0x04000u
#define EMAC_DSC_FLAG_JABBER 0x0200u
#define EMAC_DSC_FLAG_OVERSIZE 0x0100u
#define EMAC_DSC_FLAG_FRAGMENT 0x0080u
#define EMAC_DSC_FLAG_UNDERSIZED 0x0040u
#define EMAC_DSC_FLAG_CONTROL 0x0020u
#define EMAC_DSC_FLAG_OVERRUN 0x0010u
#define EMAC_DSC_FLAG_CODEERROR 0x0008u
#define EMAC_DSC_FLAG_ALIGNERROR 0x0004u
#define EMAC_DSC_FLAG_CRCERROR 0x0002u
#define EMAC_DSC_FLAG_NOMATCH 0x0001u

typedef struct buff_desc {
	uint32_t pnext;
	uint32_t pbuffer;

	uint16_t buff_len;
	uint16_t buff_off;
	uint16_t pkt_len;
	uint16_t pkt_flg;
#define HAS_SOP(flg) ((flg) & EMAC_DSC_FLAG_SOP)
#define HAS_EOP(flg) ((flg) & EMAC_DSC_FLAG_EOP)
#define HAS_OWNER(flg) ((flg) & EMAC_DSC_FLAG_OWNER)
#define HAS_EOQ(flg) ((flg) & EMAC_DSC_FLAG_EOQ)
#define HAS_TDOWNCMPLT(flg) ((flg) & EMAC_DSC_FLAG_TDOWNCMPLT)
#define HAS_PASSCRC(flg) ((flg) & EMAC_DSC_FLAG_PASSCRC)

#define HAS_JABBER(flg) ((flg) & EMAC_DSC_FLAG_JABBER)
#define HAS_OVERSIZE(flg) ((flg) & EMAC_DSC_FLAG_OVERSIZE)
#define HAS_FRAGMENT(flg) ((flg) & EMAC_DSC_FLAG_FRAGMENT)
#define HAS_UNDERSIZED(flg) ((flg) & EMAC_DSC_FLAG_UNDERSIZED)
#define HAS_CONTROL(flg) ((flg) & EMAC_DSC_FLAG_CONTROL)
#define HAS_OVERRUN(flg) ((flg) & EMAC_DSC_FLAG_OVERRUN)
#define HAS_CODEERROR(flg) ((flg) & EMAC_DSC_FLAG_CODEERROR)
#define HAS_ALIGNERROR(flg) ((flg) & EMAC_DSC_FLAG_ALIGNERROR)
#define HAS_CRCERROR(flg) ((flg) & EMAC_DSC_FLAG_CRCERROR)
#define HAS_NOMATCH(flg) ((flg) & EMAC_DSC_FLAG_NOMATCH)

#define CLR_OWNER(flg) ((flg) = ((flg) & ~EMAC_DSC_FLAG_OWNER))

#define SET_SOP(flg) ((flg) = ((flg) | EMAC_DSC_FLAG_SOP))
#define SET_EOP(flg) ((flg) = ((flg) | EMAC_DSC_FLAG_EOP))
#define SET_EOQ(flg) ((flg) = ((flg) | EMAC_DSC_FLAG_EOQ))
#define SET_BUFF_LEN(buff_len, val) ((buff_len) = (val))
#define SET_PKG_LEN(pkg_len, val) ((pkg_len) = (val))
} buff_desc_t;

typedef struct _mac_table {
	uint32_t channel;
	uint32_t matchfilt;
	uint32_t valid;
	unsigned char macaddr[6];
} mac_table_t;

#define MASK(a, b) ({uint32_t mask = (b) - (a) == 31 ? 0xffffffff : ((1u << ((b) - (a) + 1)) - 1); mask;})
#define CLRS(val, a, b) (val) = (val) & ~(MASK(a, b) << (a))
#define SETS(val, a, b, v) do {		\
	CLRS(val, a, b);		\
	(val) = (val) | (v) << (a);	\
} while(0)
#define BITS(val, a, b) (((val) >> (a)) & ((1 << (1 + (b) - (a))) - 1))

typedef struct _rx_ctx {
	uint32_t *rxnhdp;
	uint32_t *rxncp;
	mac_table_t *mac_info;
} rx_ctx_t;

#define ETH_ALEN 6
#define ETH_HLEN 14

struct ethhdr {
    unsigned char  h_dest[ETH_ALEN];   /* destination eth addr */
    unsigned char  h_source[ETH_ALEN]; /* source ether addr    */
    unsigned short h_proto;            /* packet type ID field */
};

struct omapl138_device 
{
	conf_object_t* obj;

	general_signal_intf* signal_iface;
	conf_object_t* signal;

	memory_space_intf *memory_iface;
	conf_object_t *memory;
	
	skyeye_net_ops_intf *net_iface;
	conf_object_t *net;

	skyeye_reg_intf *reg_iface;
	conf_object_t *emac_ctl;
#define NET_DEVICE(dev) ((struct net_device *)(dev)->net->obj)

	uint8_t *tx_data;
	uint8_t *rx_data;

	/* rx thread */
	int scheduler_id;
	int net_sim_isok;

	struct registers regs;
#define MAXCHANNEL 8
#define MAXMACS 8
	mac_table_t mac_table[MAXMACS];
	rx_ctx_t rx_ctx[MAXCHANNEL];

	uint32_t txintmask;
	uint32_t rxintmask;
	uint32_t macintmask;
	uint32_t rxunicast;
};
typedef struct omapl138_device emac_module_omapl138_device;
static char* regs_name[] = 
{
	"txrevid",
	"txcontrol",
	"txteardown",
	"rxrevid",
	"rxcontrol",
	"rxteardown",
	"txintstatraw",
	"txintstatmasked",
	"txintmaskset",
	"txintmaskclear",
	"macinvector",
	"maceoivector",
	"rxintstatraw",
	"rxintstatmasked",
	"rxintmaskset",
	"rxintmaskclear",
	"macintstatraw",
	"macintstatmasked",
	"macintmaskset",
	"macintmaskclear",
	"rxmbpenable",
	"rxunicastset",
	"rxunicastclear",
	"rxmaxlen",
	"rxbufferoffset",
	"rxfilterlowthresh",
	"rx0flowthresh",
	"rx1flowthresh",
	"rx2flowthresh",
	"rx3flowthresh",
	"rx4flowthresh",
	"rx5flowthresh",
	"rx6flowthresh",
	"rx7flowthresh",
	"rx0freebuffer",
	"rx1freebuffer",
	"rx2freebuffer",
	"rx3freebuffer",
	"rx4freebuffer",
	"rx5freebuffer",
	"rx6freebuffer",
	"rx7freebuffer",
	"maccontrol",
	"macstatus",
	"emcontrol",
	"fifocontrol",
	"macconfig",
	"softreset",
	"macsrcaddrlo",
	"macsrcaddrhi",
	"machash1",
	"machash2",
	"bofftest",
	"tpacetest",
	"rxpause",
	"txpause",
	"macaddrlo",
	"macaddrhi",
	"macindex",
	"tx0hdp",
	"tx1hdp",
	"tx2hdp",
	"tx3hdp",
	"tx4hdp",
	"tx5hdp",
	"tx6hdp",
	"tx7hdp",
	"rx0hdp",
	"rx1hdp",
	"rx2hdp",
	"rx3hdp",
	"rx4hdp",
	"rx5hdp",
	"rx6hdp",
	"rx7hdp",
	"tx0cp",
	"tx1cp",
	"tx2cp",
	"tx3cp",
	"tx4cp",
	"tx5cp",
	"tx6cp",
	"tx7cp",
	"rx0cp",
	"rx1cp",
	"rx2cp",
	"rx3cp",
	"rx4cp",
	"rx5cp",
	"rx6cp",
	"rx7cp",
	"rxgoodframes",
	"rxbcastframes",
	"rxmcastframes",
	"rxpauseframes",
	"rxcrcerrors",
	"rxaligncodeerrors",
	"rxoversized",
	"rxjabber",
	"rxundersized",
	"rxfragments",
	"rxfiltered",
	"rxqosfiltered",
	"rxoctets",
	"txgoodframes",
	"txbcastframes",
	"txmcastframes",
	"txpauseframes",
	"txdeferred",
	"txcollision",
	"txsinglecoll",
	"txmulticoll",
	"txexcessivecoll",
	"txlatecoll",
	"txunderrun",
	"txcarriersense",
	"txoctets",
	"frame64",
	"frame65t127",
	"frame128t255",
	"frame256t511",
	"frame512t1023",
	"frame1024tup",
	"netoctets",
	"rxsofoverruns",
	"rxmofoverruns",
	"rxdmaoverruns",
	NULL
};
#define TXREVID 0x000
#define TXCONTROL 0x004
#define TXTEARDOWN 0x008
#define RXREVID 0x010
#define RXCONTROL 0x014
#define RXTEARDOWN 0x018
#define TXINTSTATRAW 0x080
#define TXINTSTATMASKED 0x084
#define TXINTMASKSET 0x088
#define TXINTMASKCLEAR 0x08c
#define MACINVECTOR 0x090
#define MACEOIVECTOR 0x094
#define RXINTSTATRAW 0x0a0
#define RXINTSTATMASKED 0x0a4
#define RXINTMASKSET 0x0a8
#define RXINTMASKCLEAR 0x0ac
#define MACINTSTATRAW 0x0b0
#define MACINTSTATMASKED 0x0b4
#define MACINTMASKSET 0x0b8
#define MACINTMASKCLEAR 0x0bc
#define RXMBPENABLE 0x100
#define RXUNICASTSET 0x104
#define RXUNICASTCLEAR 0x108
#define RXMAXLEN 0x10c
#define RXBUFFEROFFSET 0x110
#define RXFILTERLOWTHRESH 0x114
#define RX0FLOWTHRESH 0x120
#define RX1FLOWTHRESH 0x124
#define RX2FLOWTHRESH 0x128
#define RX3FLOWTHRESH 0x12c
#define RX4FLOWTHRESH 0x130
#define RX5FLOWTHRESH 0x134
#define RX6FLOWTHRESH 0x138
#define RX7FLOWTHRESH 0x13c
#define RX0FREEBUFFER 0x140
#define RX1FREEBUFFER 0x144
#define RX2FREEBUFFER 0x148
#define RX3FREEBUFFER 0x14c
#define RX4FREEBUFFER 0x150
#define RX5FREEBUFFER 0x154
#define RX6FREEBUFFER 0x158
#define RX7FREEBUFFER 0x15c
#define MACCONTROL 0x160
#define MACSTATUS 0x164
#define EMCONTROL 0x168
#define FIFOCONTROL 0x16c
#define MACCONFIG 0x170
#define SOFTRESET 0x174
#define MACSRCADDRLO 0x1d0
#define MACSRCADDRHI 0x1d4
#define MACHASH1 0x1d8
#define MACHASH2 0x1dc
#define BOFFTEST 0x1e0
#define TPACETEST 0x1e4
#define RXPAUSE 0x1e8
#define TXPAUSE 0x1ec
#define MACADDRLO 0x500
#define MACADDRHI 0x504
#define MACINDEX 0x508
#define TX0HDP 0x600
#define TX1HDP 0x604
#define TX2HDP 0x608
#define TX3HDP 0x60c
#define TX4HDP 0x610
#define TX5HDP 0x614
#define TX6HDP 0x618
#define TX7HDP 0x61c
#define RX0HDP 0x620
#define RX1HDP 0x624
#define RX2HDP 0x628
#define RX3HDP 0x62c
#define RX4HDP 0x630
#define RX5HDP 0x634
#define RX6HDP 0x638
#define RX7HDP 0x63c
#define TX0CP 0x640
#define TX1CP 0x644
#define TX2CP 0x648
#define TX3CP 0x64c
#define TX4CP 0x650
#define TX5CP 0x654
#define TX6CP 0x658
#define TX7CP 0x65c
#define RX0CP 0x660
#define RX1CP 0x664
#define RX2CP 0x668
#define RX3CP 0x66c
#define RX4CP 0x670
#define RX5CP 0x674
#define RX6CP 0x678
#define RX7CP 0x67c
#define RXGOODFRAMES 0x200
#define RXBCASTFRAMES 0x204
#define RXMCASTFRAMES 0x208
#define RXPAUSEFRAMES 0x20c
#define RXCRCERRORS 0x210
#define RXALIGNCODEERRORS 0x214
#define RXOVERSIZED 0x218
#define RXJABBER 0x21c
#define RXUNDERSIZED 0x220
#define RXFRAGMENTS 0x224
#define RXFILTERED 0x228
#define RXQOSFILTERED 0x22c
#define RXOCTETS 0x230
#define TXGOODFRAMES 0x234
#define TXBCASTFRAMES 0x238
#define TXMCASTFRAMES 0x23c
#define TXPAUSEFRAMES 0x240
#define TXDEFERRED 0x244
#define TXCOLLISION 0x248
#define TXSINGLECOLL 0x24c
#define TXMULTICOLL 0x250
#define TXEXCESSIVECOLL 0x254
#define TXLATECOLL 0x258
#define TXUNDERRUN 0x25c
#define TXCARRIERSENSE 0x260
#define TXOCTETS 0x264
#define FRAME64 0x268
#define FRAME65T127 0x26c
#define FRAME128T255 0x270
#define FRAME256T511 0x274
#define FRAME512T1023 0x278
#define FRAME1024TUP 0x27c
#define NETOCTETS 0x280
#define RXSOFOVERRUNS 0x284
#define RXMOFOVERRUNS 0x288
#define RXDMAOVERRUNS 0x28c
#endif  

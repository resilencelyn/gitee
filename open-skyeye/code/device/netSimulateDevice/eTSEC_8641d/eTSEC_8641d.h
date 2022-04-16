#ifndef __ETSEC_8641D_H__ 
#define __ETSEC_8641D_H__ 
#include <stdint.h>

typedef struct etsecDesc
{
    volatile uint16_t	bdSts;
    volatile uint16_t	bdLen;
    volatile uint32_t	bufptr;
} ETSEC_DESC;

#define BD_WRAP       (1 << 13)
#define BD_INTERRUPT  (1 << 12)
#define BD_LAST       (1 << 11)
#define BD_TX_READY     (1 << 15)
#define BD_TX_PADCRC    (1 << 14)
#define BD_TX_TC        (1 << 10)
#define BD_TX_PREDEF    (1 << 9)
#define BD_TX_HFELC     (1 << 7)
#define BD_TX_CFRL      (1 << 6)
#define BD_TX_RC_MASK   0xF
#define BD_TX_RC_OFFSET 0x2
#define BD_TX_TOEUN     (1 << 1)
#define BD_TX_TR        (1 << 0)

#define BD_RX_EMPTY     (1 << 15)
#define BD_RX_RO1       (1 << 14)
#define BD_RX_FIRST     (1 << 10)
#define BD_RX_MISS      (1 << 8)
#define BD_RX_BROADCAST (1 << 7)
#define BD_RX_MULTICAST (1 << 6)
#define BD_RX_LG        (1 << 5)
#define BD_RX_NO        (1 << 4)
#define BD_RX_SH        (1 << 3)
#define BD_RX_CR        (1 << 2)
#define BD_RX_OV        (1 << 1)
#define BD_RX_TR        (1 << 0)

#define MACCFG1_TX_EN  (1 << 0)
#define MACCFG1_SYNC_TX_EN  (1 << 1)
#define MACCFG1_RX_EN  (1 << 2)
#define MACCFG1_SYNC_RX_EN  (1 << 3)

#define MACCFG2_CRC_EN  (1 << 1)
#define MACCFG2_PADCRC  (1 << 2)

#define RCTRL_PRSDEP_MASK   (0x3)
#define RCTRL_PRSDEP_OFFSET (6)
#define RCTRL_RSF           (1 << 2)
/* Tx FCB flags */
#define FCB_TX_VLN     (1 << 7)
#define FCB_TX_IP      (1 << 6)
#define FCB_TX_IP6     (1 << 5)
#define FCB_TX_TUP     (1 << 4)
#define FCB_TX_UDP     (1 << 3)
#define FCB_TX_CIP     (1 << 2)
#define FCB_TX_CTU     (1 << 1)
#define FCB_TX_NPH     (1 << 0)

/* Transmit descriptor status bits */

#define ETSEC_TBD_R		0x8000 /* owner, 0 == host, 1 == chip */
#define ETSEC_TBD_PADCRC	0x4000 /* autopad and append CRC */
#define ETSEC_TBD_W		0x2000 /* wrap, last descriptor in ring */
#define ETSEC_TBD_I		0x1000 /* interrupt after processing is done */
#define ETSEC_TBD_L		0x0800 /* last descriptor in frame */
#define ETSEC_TBD_TC		0x0400 /* append CRC */
#define ETSEC_TBD_DEF		0x0200 /* excess deferral condition */
#define ETSEC_TBD_TO1		0x0100 /* software scratch bit */
#define ETSEC_TBD_HFE_LC	0x0080 /* huge frame, or late collision */
#define ETSEC_TBD_RL		0x0040 /* retry limit exceeded */
#define ETSEC_TBD_RC		0x003C /* retry count */
#define ETSEC_TBD_UN		0x0002 /* TX underrun */
#define ETSEC_TBD_TOE		0x0002 /* offload enable */
#define ETSEC_TBD_TR		0x0001 /* packet truncated */

/* Receive descriptor status bits */

#define ETSEC_RBD_E		0x8000 /* owner, 0 == host, 1 == chip */
#define ETSEC_RBD_RO1		0x4000 /* software scratch bit */
#define ETSEC_RBD_W		0x2000 /* wrap, last descriptor in ring */
#define ETSEC_RBD_I		0x1000 /* interrupt after processing is done */
#define ETSEC_RBD_L		0x0800 /* last descriptor in frame */
#define ETSEC_RBD_F		0x0400 /* first descriptor in frame */
#define ETSEC_RBD_M		0x0200 /* filter miss */
#define ETSEC_RBD_BC		0x0080 /* broadcast frame */
#define ETSEC_RBD_MC		0x0040 /* multicast frame */
#define ETSEC_RBD_LG		0x0020 /* giant frame error */
#define ETSEC_RBD_NO		0x0010 /* alignment error */
#define ETSEC_RBD_SH		0x0008 /* runt frame error */
#define ETSEC_RBD_CR		0x0004 /* CRC error */
#define ETSEC_RBD_OV		0x0002 /* FIFO overrun */
#define ETSEC_RBD_TR		0x0001 /* packet truncated */

#define ETSEC_RX_DESC_CNT	128
#define ETSEC_TX_DESC_CNT	128

/* Transmit FCB format */

typedef struct etsecTxFcb
{
     	uint8_t	fcbFlags;
     	uint8_t	fcbRsvd;
     	uint8_t	fcbl4os;
     	uint8_t	fcbl3os;
     	uint16_t	fcbPhcs;
     	uint16_t	fcbVlctl;
} ETSEC_TX_FCB;

#define ETSEC_TX_FCB_VLAN	0x80	/* VLAN CTL is valid */
#define ETSEC_TX_FCB_IP		0x40	/* Packet is IP */
#define ETSEC_TX_FCB_IP6	0x20	/* Packet is IPv6 */
#define ETSEC_TX_FCB_TUP	0x10	/* Packet is TCP or UDP */
#define ETSEC_TX_FCB_UDP	0x08	/* Packet is UDP */
#define ETSEC_TX_FCB_CIP	0x04	/* Calculate IP header checksum */
#define ETSEC_TX_FCB_CTU	0x02	/* Calculate TCP/UDP checksum */
#define ETSEC_TX_FCB_NPH	0x01	/* Disable pseudo-header csum */

/* Receive FCB format */

typedef struct etsecRxFcb
{
	uint8_t	fcbFlags;
     	uint8_t	fcbPerr;
     	uint8_t	fcbRxQueue;
     	uint8_t	fcbProto;
     	uint16_t	fcbRsvd;
     	uint16_t	fcbVlctl;
} ETSEC_RX_FCB;

#define ETSEC_RX_FCB_VLAN	0x80	/* VLAN tagged frame found */
#define ETSEC_RX_FCB_IP		0x40	/* Packet is IP */
#define ETSEC_RX_FCB_IP6	0x20	/* Packet is IPv6 */
#define ETSEC_RX_FCB_TUP	0x10	/* Packet is TCP/UDP */
#define ETSEC_RX_FCB_CIP	0x08	/* IP header checksum checked */
#define ETSEC_RX_FCB_CTU	0x04	/* TCP/UDP checksum checked */
#define ETSEC_RX_FCB_EIP	0x02	/* IP header checksum error */
#define ETSEC_RX_FCB_ETU	0x01	/* TCP/UCP checksum error */

#define ETSEC_RX_FCB_PERR_NONE	0x00	/* No parser error */
#define ETSEC_RX_FCB_PERR_BADL3	0x0C	/* Unrecognized L3 sequence */

#define ETSEC_INC_DESC(x, y)	(x) = ((x + 1) & (y - 1))
#define ETSEC_MAXFRAG		16
#define ETSEC_MAX_RX		32
#define ETSEC_MAX_RX_QUEUES	8
#define ETSEC_CAM_CNT		16
#define ETSEC_RXCHUNK		4
#define ETSEC_ALIGN		2
#define ETSEC_RX_ADJ	\
    (ETHER_CRC_LEN + sizeof(ETSEC_RX_FCB) + ETSEC_ALIGN)

#define ETH_ALEN 	6
#define ARP_TYPE 	0x0806
#define IP_TYPE  	0x800
#define ARP_ASK 	0x1
#define ARP_REPLY 	0x2

struct ethhdr {
    	unsigned char  h_dest[ETH_ALEN];   /* destination eth addr */
    	unsigned char  h_source[ETH_ALEN]; /* source ether addr    */
    	unsigned short h_type;            /* packet type ID field */
    	unsigned short h_HardwareType; //0x0001表示以太网卡
	unsigned short h_ProtoType; //0x800表示IP协议
	unsigned char  h_MacLen; //MAC地址长度
	unsigned char  h_IPLen; //IP地址长度
	unsigned short h_OptType;//操作类型 1为ARP请求 2为ARP应答 3为RARP请求 4为RARP应答
    	unsigned char  h_destMac[ETH_ALEN];   /* destination eth addr */
    	unsigned char  h_srcMac[ETH_ALEN];   /* source eth addr */
    	unsigned char  h_destIP[ETH_ALEN];   /* destination IP addr */
    	unsigned char  h_srcIP[ETH_ALEN];   /* destination IP addr */
};
#define MASK(a, b) ({uint32_t mask = (b) - (a) == 31 ? 0xffffffff : ((1u << ((b) - (a) + 1)) - 1); mask;})
#define CLRS(val, a, b) (val) = (val) & ~(MASK(a, b) << (a))
#define SETS(val, a, b, v) do {		\
	CLRS(val, a, b);		\
	(val) = (val) | (v) << (a);	\
} while(0)
#define BITS(val, a, b) (((val) >> (a)) & ((1 << (1 + (b) - (a))) - 1))

struct TBIPA_BIT_T{
	int reverser :27;
	int TBIPADDR :5;
};

union TBIPA_REG_T{
	uint32_t all;
	struct TBIPA_BIT_T bit;	
};

struct MIIMADD_BIT_T{
	int reverser0 :19;
	int PHY_ADDR  :5;	
	int reverser1 :3;
	int REG_ADDR  :5;
};

union MIIM_REG_T{
	uint32_t all;
	struct MIIMADD_BIT_T bit;	
};

struct registers 
{
	/*eTSEC General Control and Status Register*/	
	uint32_t id;
	uint32_t id2;
	uint32_t ievent;
	uint32_t imask;
	uint32_t edis;
	uint32_t ecntrl;
	uint32_t ptv;
	uint32_t dmactrl;
	union TBIPA_REG_T tbipa;
	/*eTSEC Transmit Control and Status Registers*/	
	uint32_t tctrl;
	uint32_t tstat;
	uint32_t dfvlan;
	uint32_t txic;
	uint32_t tqueue;
	uint32_t tr03wt;
	uint32_t tr47wt;
	uint32_t tbdbph;
	uint32_t tbptr[8];
	uint32_t tbaseh;
	uint32_t tbase[8];
	
	/*eTSEC Receive Control and Status Registers*/	
	uint32_t rctrl;
	uint32_t rstat;
	uint32_t rxic;
	uint32_t rqueue;
	uint32_t rbifx;
	uint32_t rqfar;
	uint32_t rqfcr;
	uint32_t rqfpr;
	uint32_t mrblr;
	uint32_t rbdbph;
	uint32_t rbptr[8];
	uint32_t rbaseh;
	uint32_t rbase[8];
	/*eTSEC MAC Registers*/
	uint32_t maccfg1;
	uint32_t maccfg2;
	uint32_t ipgifg;
	uint32_t hafdup;
	uint32_t maxfrm;
	uint32_t miimcfg;
	uint32_t miimcom;
	union  MIIM_REG_T miimadd;
	uint32_t miimcon;
	uint32_t miimstat;
	uint32_t miimind;
	uint32_t ifstat;
	uint32_t macstnaddr1;
	uint32_t macstnaddr2;
	uint32_t mac01addr1;
	uint32_t mac01addr2;
	uint32_t mac02addr1;
	uint32_t mac02addr2;
	uint32_t mac03addr1;
	uint32_t mac03addr2;
	uint32_t mac04addr1;
	uint32_t mac04addr2;
	uint32_t mac05addr1;
	uint32_t mac05addr2;
	uint32_t mac06addr1;
	uint32_t mac06addr2;
	uint32_t mac07addr1;
	uint32_t mac07addr2;
	uint32_t mac08addr1;
	uint32_t mac08addr2;
	uint32_t mac09addr1;
	uint32_t mac09addr2;
	uint32_t mac10addr1;
	uint32_t mac10addr2;
	uint32_t mac11addr1;
	uint32_t mac11addr2;
	uint32_t mac12addr1;
	uint32_t mac12addr2;
	uint32_t mac13addr1;
	uint32_t mac13addr2;
	uint32_t mac14addr1;
	uint32_t mac14addr2;
	uint32_t mac15addr1;
	uint32_t mac15addr2;
	/*eTSEC Transmit and Receive Counters*/
	uint32_t tr64;
	uint32_t tr127;
	uint32_t tr255;
	uint32_t tr511;
	uint32_t tr1k;
	uint32_t trmax;
	uint32_t trmgv;
	/*eTSEC Receive Counters*/
	uint32_t rbyt;
	uint32_t rpkt;
	uint32_t rfcs;
	uint32_t rmca;
	uint32_t rbca;
	uint32_t rxcf;
	uint32_t rxpf;
	uint32_t rxuo;
	uint32_t raln;
	uint32_t rflr;
	uint32_t rcde;
	uint32_t rcse;
	uint32_t rund;
	uint32_t rovr;
	uint32_t rfrg;
	uint32_t rjbr;
	uint32_t rdrp;
	/*eTSEC Transmit Counters*/
	uint32_t tbyt;
	uint32_t tpkt;
	uint32_t tmca;
	uint32_t tbca;
	uint32_t txpf;
	uint32_t tdfr;
	uint32_t tedf;
	uint32_t tscl;
	uint32_t tmcl;
	uint32_t tlcl;
	uint32_t txcl;
	uint32_t tncl;
	uint32_t tdrp;
	uint32_t tjbr;
	uint32_t tfcs;
	uint32_t txcf;
	uint32_t tovr;
	uint32_t tund;
	uint32_t tfrg;
	/*eTSEC Counter Control and TOE Statistics Registers*/
	uint32_t car1;
	uint32_t car2;
	uint32_t cam1;
	uint32_t cam2;
	uint32_t rrej;
	/*Hash Function Registers*/
	uint32_t igaddr0;
	uint32_t igaddr1;
	uint32_t igaddr2;
	uint32_t igaddr3;
	uint32_t igaddr4;
	uint32_t igaddr5;
	uint32_t igaddr6;
	uint32_t igaddr7;
	uint32_t gaddr0;
	uint32_t gaddr1;
	uint32_t gaddr2;
	uint32_t gaddr3;
	uint32_t gaddr4;
	uint32_t gaddr5;
	uint32_t gaddr6;
	uint32_t gaddr7;
	/*eTSEC FIFO Control Registers*/
	uint32_t fifocfg;
	/*eTSEC DMA Attribute Registers*/
	uint32_t attr;
	/*eTSEC Lossless Flow Control Registers*/
	uint32_t rqprm0;
	uint32_t rqprm1;
	uint32_t rqprm2;
	uint32_t rqprm3;
	uint32_t rqprm4;
	uint32_t rqprm5;
	uint32_t rqprm6;
	uint32_t rqprm7;
	uint32_t rfbptr0;
	uint32_t rfbptr1;
	uint32_t rfbptr2;
	uint32_t rfbptr3;
	uint32_t rfbptr4;
	uint32_t rfbptr5;
	uint32_t rfbptr6;
	uint32_t rfbptr7;
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

#define NET_DEVICE(dev) ((struct net_device *)(dev)->net->obj)
struct eTSEC_device 
{
	
	conf_object_t* obj;
	struct registers regs;
	skyeye_net_ops_intf *net_iface;
	conf_object_t *net;
	memory_space_intf *memory_iface;
	conf_object_t *memory;

	int net_sim_isok;
    /* Tx */
    ETSEC_DESC first_bd;
	uint8_t *tx_data;
	uint32_t tx_buffer_len;
    /* Rx */
    uint8_t       *rx_data;
    uint32_t       rx_buffer_len;
    uint32_t       rx_remaining_data;
    uint8_t        rx_first_in_frame;
    uint8_t        rx_fcb_size;
    ETSEC_DESC  rx_first_bd;
    uint8_t        rx_fcb[10];
    uint32_t       rx_padding;

	uint32_t intmask;
	struct {
		int tx_num;
		int rx_num;
		int err_num;
		conf_object_t          *obj;
		general_signal_intf    *intf;
	}signal;
	uint8_t macInfo[6];
    uint8_t mac_valid;
    uint32_t phy_control;
    uint32_t phy_status;
};

typedef struct eTSEC_device eTSEC_8641d_device;
static char* regs_name[] = 
{
	"id",
	"id2",
	"ievent",
	"imask",
	"edis",
	"ecntrl",
	"ptv",
	"dmactrl",
	"tbipa",
	"tctrl",
	"tstat",
	"dfvlan",
	"txic",
	"tqueue",
	"tr03wt",
	"tr47wt",
	"tbdbph",
	"tbptr0",
	"tbptr1",
	"tbptr2",
	"tbptr3",
	"tbptr4",
	"tbptr5",
	"tbptr6",
	"tbptr7",
	"tbaseh",
	"tbase0",
	"tbase1",
	"tbase2",
	"tbase3",
	"tbase4",
	"tbase5",
	"tbase6",
	"tbase7",
	"rctrl",
	"rstat",
	"rxic",
	"rqueue",
	"rbifx",
	"rqfar",
	"rqfcr",
	"rqfpr",
	"mrblr",
	"rbdbph",
	"rbptr0",
	"rbptr1",
	"rbptr2",
	"rbptr3",
	"rbptr4",
	"rbptr5",
	"rbptr6",
	"rbptr7",
	"rbaseh",
	"rbase0",
	"rbase1",
	"rbase2",
	"rbase3",
	"rbase4",
	"rbase5",
	"rbase6",
	"rbase7",
	"maccfg1",
	"maccfg2",
	"ipgifg",
	"hafdup",
	"maxfrm",
	"miimcfg",
	"miimcom",
	"miimadd",
	"miimcon",
	"miimstat",
	"miimind",
	"ifstat",
	"macstnaddr1",
	"macstnaddr2",
	"mac01addr1",
	"mac01addr2",
	"mac02addr1",
	"mac02addr2",
	"mac03addr1",
	"mac03addr2",
	"mac04addr1",
	"mac04addr2",
	"mac05addr1",
	"mac05addr2",
	"mac06addr1",
	"mac06addr2",
	"mac07addr1",
	"mac07addr2",
	"mac08addr1",
	"mac08addr2",
	"mac09addr1",
	"mac09addr2",
	"mac10addr1",
	"mac10addr2",
	"mac11addr1",
	"mac11addr2",
	"mac12addr1",
	"mac12addr2",
	"mac13addr1",
	"mac13addr2",
	"mac14addr1",
	"mac14addr2",
	"mac15addr1",
	"mac15addr2",
	"tr64",
	"tr127",
	"tr255",
	"tr511",
	"tr1k",
	"trmax",
	"trmgv",
	"rbyt",
	"rpkt",
	"rfcs",
	"rmca",
	"rbca",
	"rxcf",
	"rxpf",
	"rxuo",
	"raln",
	"rflr",
	"rcde",
	"rcse",
	"rund",
	"rovr",
	"rfrg",
	"rjbr",
	"rdrp",
	"tbyt",
	"tpkt",
	"tmca",
	"tbca",
	"txpf",
	"tdfr",
	"tedf",
	"tscl",
	"tmcl",
	"tlcl",
	"txcl",
	"tncl",
	"tdrp",
	"tjbr",
	"tfcs",
	"txcf",
	"tovr",
	"tund",
	"tfrg",
	"car1",
	"car2",
	"cam1",
	"cam2",
	"rrej",
	"igaddr0",
	"igaddr1",
	"igaddr2",
	"igaddr3",
	"igaddr4",
	"igaddr5",
	"igaddr6",
	"igaddr7",
	"gaddr0",
	"gaddr1",
	"gaddr2",
	"gaddr3",
	"gaddr4",
	"gaddr5",
	"gaddr6",
	"gaddr7",
	"fifocfg",
	"attr",
	"rqprm0",
	"rqprm1",
	"rqprm2",
	"rqprm3",
	"rqprm4",
	"rqprm5",
	"rqprm6",
	"rqprm7",
	"rfbptr0",
	"rfbptr1",
	"rfbptr2",
	"rfbptr3",
	"rfbptr4",
	"rfbptr5",
	"rfbptr6",
	"rfbptr7",
	NULL
};

#define ID 		0x000
#define ID2 		0x004
#define IEVENT 		0x010
#define IMASK 		0x014
#define EDIS 		0x018
#define ECNTRL 		0x020
#define PTV 		0x028
#define DMACTRL 	0x02c
#define TBIPA 		0x030
#define TCTRL 		0x0100
#define TSTAT 		0x0104
#define DFVLAN 		0x0108
#define TXIC 		0x0110
#define TQUEUE 		0x0114
#define TR03WT 		0x0140
#define TR47WT 		0x0144
#define TBDBPH 		0x0180
#define TBPTR0 		0x0184
#define TBPTR1 		0x018c
#define TBPTR2 		0x0194
#define TBPTR3 		0x019c
#define TBPTR4 		0x01a4
#define TBPTR5 		0x01ac
#define TBPTR6 		0x01b4
#define TBPTR7 		0x01bc
#define TBASEH 		0x0200
#define TBASE0 		0x0204
#define TBASE1 		0x020c
#define TBASE2 		0x0214
#define TBASE3 		0x021c
#define TBASE4 		0x0224
#define TBASE5 		0x022c
#define TBASE6 		0x0234
#define TBASE7 		0x023c
#define RCTRL 		0x0300
#define RSTAT 		0x0304
#define RXIC 		0x0310
#define RQUEUE 		0x0314
#define RBIFX 		0x0330
#define RQFAR 		0x0334
#define RQFCR 		0x0338
#define RQFPR 		0x033c
#define MRBLR 		0x0340
#define RBDBPH 		0x0380
#define RBPTR0 		0x0384
#define RBPTR1 		0x038c
#define RBPTR2 		0x0394
#define RBPTR3 		0x039c
#define RBPTR4 		0x03a4
#define RBPTR5 		0x03ac
#define RBPTR6 		0x03b4
#define RBPTR7 		0x03bc
#define RBASEH 		0x0400
#define RBASE0 		0x0404
#define RBASE1 		0x040c
#define RBASE2 		0x0414
#define RBASE3 		0x041c
#define RBASE4 		0x0424
#define RBASE5 		0x042c
#define RBASE6 		0x0434
#define RBASE7 		0x043c
#define MACCFG1 	0x0500
#define MACCFG2 	0x0504
#define IPGIFG 		0x0508
#define HAFDUP 		0x050c
#define MAXFRM 		0x0510
#define MIIMCFG 	0x0520
#define MIIMCOM 	0x0524
#define MIIMADD 	0x0528
#define MIIMCON 	0x052c
#define MIIMSTAT 	0x0530
#define MIIMIND 	0x0534
#define IFSTAT 		0x053c
#define MACSTNADDR1 	0x0540
#define MACSTNADDR2 	0x0544
#define MAC01ADDR1 	0x0548
#define MAC01ADDR2 	0x054c
#define MAC02ADDR1 	0x0550
#define MAC02ADDR2 	0x0554
#define MAC03ADDR1 	0x0558
#define MAC03ADDR2 	0x055c
#define MAC04ADDR1 	0x0560
#define MAC04ADDR2 	0x0564
#define MAC05ADDR1 	0x0568
#define MAC05ADDR2 	0x056c
#define MAC06ADDR1 	0x0570
#define MAC06ADDR2 	0x0574
#define MAC07ADDR1 	0x0578
#define MAC07ADDR2 	0x057c
#define MAC08ADDR1 	0x0580
#define MAC08ADDR2 	0x0584
#define MAC09ADDR1 	0x0588
#define MAC09ADDR2 	0x058c
#define MAC10ADDR1 	0x0590
#define MAC10ADDR2 	0x0594
#define MAC11ADDR1 	0x0598
#define MAC11ADDR2 	0x059c
#define MAC12ADDR1 	0x05a0
#define MAC12ADDR2 	0x05a4
#define MAC13ADDR1 	0x05a8
#define MAC13ADDR2 	0x05ac
#define MAC14ADDR1 	0x05b0
#define MAC14ADDR2 	0x05b4
#define MAC15ADDR1 	0x05b8
#define MAC15ADDR2 	0x05bc
#define TR64 		0x0680
#define TR127 		0x0684
#define TR255 		0x0688
#define TR511 		0x068c
#define TR1K 		0x0690
#define TRMAX 		0x0694
#define TRMGV 		0x0698
#define RBYT 		0x069c
#define RPKT 		0x06a0
#define RFCS 		0x06a4
#define RMCA 		0x06a8
#define RBCA 		0x06ac
#define RXCF 		0x06b0
#define RXPF 		0x06b4
#define RXUO 		0x06b8
#define RALN 		0x06bc
#define RFLR 		0x06c0
#define RCDE 		0x06c4
#define RCSE 		0x06c8
#define RUND 		0x06cc
#define ROVR 		0x06d0
#define RFRG 		0x06d4
#define RJBR 		0x06d8
#define RDRP 		0x06dc
#define TBYT 		0x06e0
#define TPKT 		0x06e4
#define TMCA 		0x06e8
#define TBCA 		0x06ec
#define TXPF 		0x06f0
#define TDFR 		0x06f4
#define TEDF 		0x06f8
#define TSCL 		0x06fc
#define TMCL 		0x0700
#define TLCL 		0x0704
#define TXCL 		0x0708
#define TNCL 		0x070c
#define TDRP 		0x0714
#define TJBR 		0x0718
#define TFCS 		0x071c
#define TXCF 		0x0720
#define TOVR 		0x0724
#define TUND 		0x0728
#define TFRG 		0x072c
#define CAR1 		0x0730
#define CAR2 		0x0734
#define CAM1 		0x0738
#define CAM2 		0x073c
#define RREJ 		0x0740
#define IGADDR0		0x0800
#define IGADDR1		0x0804
#define IGADDR2		0x0808
#define IGADDR3		0x080c
#define IGADDR4		0x0810
#define IGADDR5		0x0814
#define IGADDR6		0x0818
#define IGADDR7		0x081c
#define GADDR0 		0x0880
#define GADDR1 		0x0884
#define GADDR2 		0x0888
#define GADDR3 		0x088c
#define GADDR4 		0x0890
#define GADDR5 		0x0894
#define GADDR6 		0x0898
#define GADDR7 		0x089c
#define FIFOCFG		0x0a00
#define ATTR		0x0bf8
#define RQPRM0		0x0c00
#define RQPRM1		0x0c04
#define RQPRM2		0x0c08
#define RQPRM3		0x0c0c
#define RQPRM4		0x0c10
#define RQPRM5		0x0c14
#define RQPRM6		0x0c18
#define RQPRM7		0x0c1c
#define RFBPTR0		0x0c44
#define RFBPTR1		0x0c4c
#define RFBPTR2		0x0c54
#define RFBPTR3		0x0c5c
#define RFBPTR4		0x0c64
#define RFBPTR5		0x0c6c
#define RFBPTR6		0x0c74
#define RFBPTR7		0x0c7c

/* IEVENT and IMASK Register definitions */

#define ETSEC_IEVENT_BABR		0x80000000 /* giant frame RXed */
#define ETSEC_IEVENT_RXC		0x40000000 /* pause frame RXed */
#define ETSEC_IEVENT_BSY		0x20000000 /* RX overrun */
#define ETSEC_IEVENT_EBERR		0x10000000 /* eth bus error */
#define ETSEC_IEVENT_MSRO		0x04000000 /* stats oflow */
#define ETSEC_IEVENT_GTSC		0x02000000 /* graceful TX stop done */
#define ETSEC_IEVENT_BABT		0x01000000 /* giant frame TXed */
#define ETSEC_IEVENT_TXC		0x00800000 /* pause frame TXed */
#define ETSEC_IEVENT_TXE		0x00400000 /* TX error */
#define ETSEC_IEVENT_TXB		0x00200000 /* TX descriptor updated */
#define ETSEC_IEVENT_TXF		0x00100000 /* TX frame completed */
#define ETSEC_IEVENT_LC			0x00040000 /* late collision */
#define ETSEC_IEVENT_CRL		0x00020000 /* excess collision */
#define ETSEC_IEVENT_XFUN		0x00010000 /* TX underrun */
#define ETSEC_IEVENT_RXB		0x00008000 /* RX descriptor updated */
#define ETSEC_IEVENT_MMRD		0x00000400 /* MDIO read, 8349 only */
#define ETSEC_IEVENT_MMWR		0x00000200 /* MDIO write, 8349 only */
#define ETSEC_IEVENT_GRSC		0x00000100 /* graceful RX stop done */
#define ETSEC_IEVENT_RXF		0x00000080 /* RX frame completed */

/*
 * Error event disable register. Note that with the IMASK
 * register, setting a bit enables the interrupt, whereas with
 * this register, setting a bit disables the interrupt.
 */

#define ETSEC_EDIS_BSYDIS		0x20000000
#define ETSEC_EDIS_EBERRDIS		0x10000000
#define ETSEC_EDIS_TXEDIS		0x00400000
#define ETSEC_EDIS_LCDIS		0x00040000
#define ETSEC_EDIS_CRLDIS		0x00020000
#define ETSEC_EDIS_XFUNDIS		0x00010000

/* Ethernet Control Register */

#define ETSEC_ECNTRL_FIFO		0x00008000 /* FIFO mode enable */
#define ETSEC_ECNTRL_CLRCNT		0x00004000 /* zero status counters */
#define ETSEC_ECNTRL_AUTOZ		0x00002000 /* zero counter on read */
#define ETSEC_ECNTRL_STEN		0x00001000 /* status enable */
#define ETSEC_ECNTRL_GMIIM		0x00000040 /* GMII mode (strap) */
#define ETSEC_ECNTRL_TBIM		0x00000020 /* TBI mode (strap) */
#define ETSEC_ECNTRL_RPM		0x00000010 /* reduced pin mode 1000 */
#define ETSEC_ECNTRL_R100M		0x00000008 /* 0 = R10, 1 == R100 */
#define ETSEC_ECNTRL_RMM		0x00000004 /* reduced pin mode 10/100*/
#define ETSEC_ECNTRL_SGMIIM		0x00000002 /* Serial GMII mode */

/* Pause time value register */

#define ETSEC_PTV_PT			0x0000FFFF /* pause time value */
#define ETSEC_PTV_PTE			0xFFFF0000 /* extended pause control */

/* DMA control register */

#define ETSEC_DMACTRL_TDSEN		0x00000080 /* TX data snoop */
#define ETSEC_DMACTRL_TBDSEN		0x00000040 /* TX bd snoop */
#define ETSEC_DMACTRL_GRS		0x00000010 /* graceful RX stop */
#define ETSEC_DMACTRL_GTS		0x00000008 /* graceful TX stop */
#define ETSEC_DMACTRL_TOD		0x00000004 /* TX on demand */
#define ETSEC_DMACTRL_WWR		0x00000002 /* write with response */
#define ETSEC_DMACTRL_WOP		0x00000001 /* wait or poll */

/* Transmit control register */

#define ETSEC_TCTRL_IPCSEN		0x00004000 /* IP header csum enable */
#define ETSEC_TCTRL_TUCSEN		0x00002000 /* TCP/UDP csum enable */
#define ETSEC_TCTRL_VLINS		0x00001000 /* VLAN tag insert enable */
#define ETSEC_TCTRL_THDF		0x00000800 /* half duplex flow ctl */
#define ETSEC_TCTRL_RFC_PAUSE		0x00000010 /* received pause frame */
#define ETSEC_TCTRL_TFC_PAUSE		0x00000008 /* send pause frame */
#define ETSEC_TCTRL_TXSCHED		0x00000006 /* TX sched algorithm */

#define ETSEC_TXSCHED_SINGLEPOLL	0x00000000
#define ETSEC_TXSCHED_PRIOSCHED		0x00000002
#define ETSEC_TXSCHED_MWROUNDROBIN	0x00000004

/*
 * Transmit status register
 * The THLT bit will be set whenever the TX DMA engine encounters
 * an error condition, including when it runs out of ready descriptors
 * to process. The transmitter is resumed by clearing the bit (i.e.
 * writing 0 to this register).
 */

#define ETSEC_TSTAT_QHLT(i)             (1 << (31 - i)) /* TX halted / resume */
#define ETSEC_TSTAT_TXF0		0x00008000 /* TX event */
#define ETSEC_TSTAT_TXF1		0x00004000 /* TX event */
#define ETSEC_TSTAT_TXF2		0x00002000 /* TX event */
#define ETSEC_TSTAT_TXF3		0x00001000 /* TX event */
#define ETSEC_TSTAT_TXF4		0x00000800 /* TX event */
#define ETSEC_TSTAT_TXF5		0x00000400 /* TX event */
#define ETSEC_TSTAT_TXF6		0x00000200 /* TX event */
#define ETSEC_TSTAT_TXF7		0x00000100 /* TX event */

/* Transmit interrupt coalescing register */

#define ETSEC_TXIC_ICEN			0x80000000 /* TX coal enable */
#define ETSEC_TXIC_ICFCT		0x1FE00000 /* frame count thresh */
#define ETSEC_TXIC_ICTT			0x0000FFFF /* timer thresh */

/* Transmit queue control register */

#define ETSEC_TQUEUE_EN0		0x00008000 /* TX queue enabled */
#define ETSEC_TQUEUE_EN1		0x00004000 /* TX queue enabled */
#define ETSEC_TQUEUE_EN2		0x00002000 /* TX queue enabled */
#define ETSEC_TQUEUE_EN3		0x00001000 /* TX queue enabled */
#define ETSEC_TQUEUE_EN4		0x00000800 /* TX queue enabled */
#define ETSEC_TQUEUE_EN5		0x00000400 /* TX queue enabled */
#define ETSEC_TQUEUE_EN6		0x00000200 /* TX queue enabled */
#define ETSEC_TQUEUE_EN7		0x00000100 /* TX queue enabled */

/* Receive control register */

#define ETSEC_RCTRL_PAL			0x001F0000 /* RX frame alignment pad */
#define ETSEC_RCTRL_VLEX		0x00002000 /* VLAN extract enable */
#define ETSEC_RCTRL_FILREN		0x00001000 /* filer enable */
#define ETSEC_RCTRL_FSQEN		0x00000800 /* single queue filer mode */
#define ETSEC_RCTRL_GHTX		0x00000400 /* mcast hash table extend */
#define ETSEC_RCTRL_IPCSEN		0x00000200 /* IP header csum enable */
#define ETSEC_RCTRL_TUCSEN		0x00000100 /* TCP/UDP csum enable */
#define ETSEC_RCTRL_PRSDEP		0x000000C0 /* Parser control */
#define ETSEC_RCTRL_BC_REJ		0x00000010 /* reject broadcasts */
#define ETSEC_RCTRL_PROM		0x00000008 /* promisc mode */
#define ETSEC_RCTRL_RSF			0x00000004 /* receive short frames */
#define ETSEC_RCTRL_EMEN		0x00000002 /* enable CAM1-CAM15 */

#define ETSEC_PARSER_DISABLE		0x00000000
#define ETSEC_PARSER_L2			0x00000040
#define ETSEC_PARSER_L2_L3		0x00000080
#define ETSEC_PARSER_L2_L3_L4		0x000000C0

/*
 * Receive status register
 * Like the TSTAT register, the QHLT bit indicates the RX DMA engine
 * has stalled due to running out of descriptors, or encountering an
 * error. The bit must be cleared to resume the RX engine.
 */

#define ETSEC_RSTAT_QHLT(i)             (1 << (23 - i)) /* RX halted / resume */
#define ETSEC_RSTAT_RXF0		0x00000080 /* RX event */
#define ETSEC_RSTAT_RXF1		0x00000040 /* RX event */
#define ETSEC_RSTAT_RXF2		0x00000020 /* RX event */
#define ETSEC_RSTAT_RXF3		0x00000010 /* RX event */
#define ETSEC_RSTAT_RXF4		0x00000008 /* RX event */
#define ETSEC_RSTAT_RXF5		0x00000004 /* RX event */
#define ETSEC_RSTAT_RXF6		0x00000002 /* RX event */
#define ETSEC_RSTAT_RXF7		0x00000001 /* RX event */

/* Receive interrupt coalescing register */

#define ETSEC_RXIC_ICEN			0x80000000 /* RX coal enable */
#define ETSEC_RXIC_ICFCT		0x1FE00000 /* frame count thresh */
#define ETSEC_RXIC_ICTT			0x0000FFFF /* timer thresh */

#define ETSEC_COAL_FRAMES(x)		((x) & 0xFF) << 21
#define ETSEC_COAL_TIMER(x)		((x) & 0xFFFF)

/* RX Queue control register */

#define ETSEC_RQUEUE_EXT0		0x00800000 /* L2 extraction */
#define ETSEC_RQUEUE_EXT1		0x00400000 /* L2 extraction */
#define ETSEC_RQUEUE_EXT2		0x00200000 /* L2 extraction */
#define ETSEC_RQUEUE_EXT3		0x00100000 /* L2 extraction */
#define ETSEC_RQUEUE_EXT4		0x00080000 /* L2 extraction */
#define ETSEC_RQUEUE_EXT5		0x00040000 /* L2 extraction */
#define ETSEC_RQUEUE_EXT6		0x00020000 /* L2 extraction */
#define ETSEC_RQUEUE_EXT7		0x00010000 /* L2 extraction */
#define ETSEC_RQUEUE_EN0		0x00000080 /* queue enable */
#define ETSEC_RQUEUE_EN1		0x00000040 /* queue enable */
#define ETSEC_RQUEUE_EN2		0x00000020 /* queue enable */
#define ETSEC_RQUEUE_EN3		0x00000010 /* queue enable */
#define ETSEC_RQUEUE_EN4		0x00000008 /* queue enable */
#define ETSEC_RQUEUE_EN5		0x00000004 /* queue enable */
#define ETSEC_RQUEUE_EN6		0x00000002 /* queue enable */
#define ETSEC_RQUEUE_EN7		0x00000001 /* queue enable */

/* RX bit field extraction control register */

#define ETSEC_RBIFX_B0CTL		0xC0000000
#define ETSEC_RBIFX_B0OFF		0x3F000000
#define ETSEC_RBIFX_B1CTL		0x00C00000
#define ETSEC_RBIFX_B1OFF		0x003F0000
#define ETSEC_RBIFX_B2CTL		0x0000C000
#define ETSEC_RBIFX_B2OFF		0x00003F00
#define ETSEC_RBIFX_B3CTL		0x000000C0
#define ETSEC_RBIFX_B3OFF		0x0000003F

/*
 * byte 0 control bits decide how the offset value will be applied.
 * Options include no extraction, extract from <start of ethernet frame
 * header> - 8 (i.e. start of preamble), extract starting from L3
 * header, and extract starting from L4 header.
 */

#define ETSEC_BCTL_NONE			0x00 /* no extraction */
#define ETSEC_BCTL_PREAMBLE		0x01 /* -8 bytes from start of frame */
#define ETSEC_BCTL_L3			0x02 /* from start of L3 header */
#define ETSEC_BCTL_L4			0x03 /* from start of L4 header */

/* MAC configuration 1 register */

#define ETSEC_MACCFG1_SOFT_RESET	0x80000000 /* Reset the entire MAC */
#define ETSEC_MACCFG1_RESET_RX_MC	0x00080000 /* Reset RX mac control */
#define ETSEC_MACCFG1_RESET_TX_MC	0x00040000 /* Reset TX mac control */
#define ETSEC_MACCFG1_RESET_RX_FUN	0x00020000 /* Reset RX function */
#define ETSEC_MACCFG1_RESET_TX_FUN	0x00010000 /* Reset TX function */
#define ETSEC_MACCFG1_LOOPBACK		0x00000100 /* MAC loopback mode */
#define ETSEC_MACCFG1_RX_FLOW		0x00000020 /* enable RX flow control */
#define ETSEC_MACCFG1_TX_FLOW		0x00000010 /* enable TX flow control */
#define ETSEC_MACCFG1_SYNCD_RX_EN	0x00000008 /* RX sync'ed */
#define ETSEC_MACCFG1_RX_EN		0x00000004 /* RX enable */
#define ETSEC_MACCFG1_SYNCD_TX_EN	0x00000002 /* TX sync'ed */
#define ETSEC_MACCFG1_TX_EN		0x00000001 /* TX enable */


/* MAC configuration 2 register */

#define ETSEC_MACCFG2_IF_MODE_MASK	0x00000003
#define ETSEC_MACCFG2_IF_MODE_MII	0x00000001
#define ETSEC_MACCFG2_IF_MODE_GMII_TBI	0x00000002

#define ETSEC_MACCFG2_PREAMBLE_LEN	0x0000F000
#define ETSEC_MACCFG2_IF_MODE		0x00000300
#define ETSEC_MACCFG2_HUGE_FRAME	0x00000020 
#define ETSEC_MACCFG2_LENGTH_CHECK	0x00000010
#define ETSEC_MACCFG2_PADCRC		0x00000004
#define ETSEC_MACCFG2_CRC_EN		0x00000002
#define ETSEC_MACCFG2_FULL_DUPLEX	0x00000001

#define ETSEC_IFMODE_RSVD0		0x00000000
#define ETSEC_IFMODE_MII		0x00000100 /* nibble mode */
#define ETSEC_IFMODE_GMII_TBI		0x00000200 /* byte mode */
#define ETSEC_IFMODE_RSVD1		0x00000300

/* Interpacket gap register */

#define ETSEC_IPGIFG_BACKTOBACK		0x0000007F
#define ETSEC_IPGIFG_MINIMUM		0x00007F00
#define ETSEC_IPGIFG_NONBACKTOBACK1	0x007F0000
#define ETSEC_IPGIFG_NONBACKTOBACK2	0x7F000000

/* Halfduplex register */

#define ETSEC_HALFDUP_ALTBEB_TRUNC	0x00F00000
#define ETSEC_HALFDUP_BEB		0x00080000
#define ETSEC_HALFDUP_BPNBO		0x00040000
#define ETSEC_HALFDUP_NBO		0x00020000
#define ETSEC_HALFDUP_EXCESS_DEF	0x00010000
#define ETSEC_HALFDUP_RETRY		0x0000F000
#define ETSEC_HALFDUP_COL_WINDOW	0x0000003F

/* MII management configuration register */

#define ETSEC_MIIMCFG_RESET		0x80000000
#define ETSEC_MIIMCFG_NO_PRE		0x00000010
#define ETSEC_MIIMCFG_MGMT_CLK		0x0000000F

#define ETSEC_MIIMCFG_MCS_2		0x00000000
#define ETSEC_MIIMCFG_MCS_4		0x00000001
#define ETSEC_MIIMCFG_MCS_6		0x00000002
#define ETSEC_MIIMCFG_MCS_8		0x00000003
#define ETSEC_MIIMCFG_MCS_10		0x00000004
#define ETSEC_MIIMCFG_MCS_14		0x00000005
#define ETSEC_MIIMCFG_MCS_20		0x00000006
#define ETSEC_MIIMCFG_MCS_28		0x00000007

/* MII management command register */

#define ETSEC_MIIMCOM_SCAN		0x00000002
#define ETSEC_MIIMCOM_READ		0x00000001

/* MII management address register */

#define ETSEC_MIIMADD_PHYADDR		0x0000001F
#define ETSEC_MIIMADD_REGADDR		0x00001F00

/* MII management control register */

#define ETSEC_MIIMCON_WRDATA		0x0000FFFF

/* MII management status register */

#define ETSEC_MIIMSTAT_RDDATA		0x0000FFFF

/* MII management indicator register */

#define ETSEC_MIIMIND_NOT_VALID		0x00000004
#define ETSEC_MIIMIND_SCAN		0x00000002
#define ETSEC_MIIMIND_BUSY		0x00000001

#define ISBROADCAST(pmac) ({							\
	unsigned char broadcast[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};	\
	int ret = 0;								\
	ret = memcmp(pmac, broadcast, sizeof(broadcast));			\
	!ret;									\
})
#define ISMULTICAST(pmac) ((pmac)[0] & 0x1)
#define ISUNICAST(pmac) (!((pmac)[0] & 0x1))

/* Transmit descriptor status bits */

#define ETSEC_TBD_R		0x8000 /* owner, 0 == host, 1 == chip */
#define ETSEC_TBD_PADCRC	0x4000 /* autopad and append CRC */
#define ETSEC_TBD_W		0x2000 /* wrap, last descriptor in ring */
#define ETSEC_TBD_I		0x1000 /* interrupt after processing is done */
#define ETSEC_TBD_L		0x0800 /* last descriptor in frame */
#define ETSEC_TBD_TC		0x0400 /* append CRC */
#define ETSEC_TBD_DEF		0x0200 /* excess deferral condition */
#define ETSEC_TBD_TO1		0x0100 /* software scratch bit */
#define ETSEC_TBD_HFE_LC	0x0080 /* huge frame, or late collision */
#define ETSEC_TBD_RL		0x0040 /* retry limit exceeded */
#define ETSEC_TBD_RC		0x003C /* retry count */
#define ETSEC_TBD_UN		0x0002 /* TX underrun */
#define ETSEC_TBD_TOE		0x0002 /* offload enable */
#define ETSEC_TBD_TR		0x0001 /* packet truncated */

/* Receive descriptor status bits */

#define ETSEC_RBD_E		0x8000 /* owner, 0 == host, 1 == chip */
#define ETSEC_RBD_RO1		0x4000 /* software scratch bit */
#define ETSEC_RBD_W		0x2000 /* wrap, last descriptor in ring */
#define ETSEC_RBD_I		0x1000 /* interrupt after processing is done */
#define ETSEC_RBD_L		0x0800 /* last descriptor in frame */
#define ETSEC_RBD_F		0x0400 /* first descriptor in frame */
#define ETSEC_RBD_M		0x0200 /* filter miss */
#define ETSEC_RBD_BC		0x0080 /* broadcast frame */
#define ETSEC_RBD_MC		0x0040 /* multicast frame */
#define ETSEC_RBD_LG		0x0020 /* giant frame error */
#define ETSEC_RBD_NO		0x0010 /* alignment error */
#define ETSEC_RBD_SH		0x0008 /* runt frame error */
#define ETSEC_RBD_CR		0x0004 /* CRC error */
#define ETSEC_RBD_OV		0x0002 /* FIFO overrun */
#define ETSEC_RBD_TR		0x0001 /* packet truncated */

#define QUEUE_CHANNEL 8

#define DMACTRL_LE  (1 << 15)
#define DMACTRL_GRS (1 <<  4)
#define DMACTRL_GTS (1 <<  3)
#define DMACTRL_WOP (1 <<  0)

#define IEVENT_PERR  (1 <<  0)
#define IEVENT_DPE   (1 <<  1)
#define IEVENT_FIQ   (1 <<  2)
#define IEVENT_FIR   (1 <<  3)
#define IEVENT_FGPI  (1 <<  4)
#define IEVENT_RXF   (1 <<  7)
#define IEVENT_GRSC  (1 <<  8)
#define IEVENT_MMRW  (1 <<  9)
#define IEVENT_MMRD  (1 << 10)
#define IEVENT_MAG   (1 << 11)
#define IEVENT_RXB   (1 << 15)
#define IEVENT_XFUN  (1 << 16)
#define IEVENT_CRL   (1 << 17)
#define IEVENT_LC    (1 << 18)
#define IEVENT_TXF   (1 << 20)
#define IEVENT_TXB   (1 << 21)
#define IEVENT_TXE   (1 << 22)
#define IEVENT_TXC   (1 << 23)
#define IEVENT_BABT  (1 << 24)
#define IEVENT_GTSC  (1 << 25)
#define IEVENT_MSRO  (1 << 26)
#define IEVENT_EBERR (1 << 28)
#define IEVENT_BSY   (1 << 29)
#define IEVENT_RXC   (1 << 30)
#define IEVENT_BABR  (1 << 31)

#define IEVENT_RX_MASK (IEVENT_RXF | IEVENT_RXB)
#define IEVENT_TX_MASK (IEVENT_TXF | IEVENT_TXB)

#define IEVENT_ERR_MASK (IEVENT_MAG | IEVENT_GTSC | IEVENT_GRSC | IEVENT_TXC | \
    IEVENT_RXC | IEVENT_BABR | IEVENT_BABT | IEVENT_LC | \
    IEVENT_CRL | IEVENT_FGPI | IEVENT_FIR | IEVENT_FIQ | \
    IEVENT_DPE | IEVENT_PERR | IEVENT_EBERR | IEVENT_TXE | \
    IEVENT_XFUN | IEVENT_BSY | IEVENT_MSRO | IEVENT_MMRD | \
    IEVENT_MMRW)

#define MIN(a, b) (a<b?a:b)

#define MIIM_CONTROL    0
#define MIIM_STATUS     1
#define MIIM_T2_STATUS  10
#define MIIMCOM_READ    1

/* PHY Status Register */

#define MII_SR_EXTENDED_CAPS     0x0001    /* Extended register capabilities */
#define MII_SR_JABBER_DETECT     0x0002    /* Jabber Detected */
#define MII_SR_LINK_STATUS       0x0004    /* Link Status 1 = link */
#define MII_SR_AUTONEG_CAPS      0x0008    /* Auto Neg Capable */
#define MII_SR_REMOTE_FAULT      0x0010    /* Remote Fault Detect */
#define MII_SR_AUTONEG_COMPLETE  0x0020    /* Auto Neg Complete */
#define MII_SR_PREAMBLE_SUPPRESS 0x0040    /* Preamble may be suppressed */
#define MII_SR_EXTENDED_STATUS   0x0100    /* Ext. status info in Reg 0x0F */
#define MII_SR_100T2_HD_CAPS     0x0200    /* 100T2 Half Duplex Capable */
#define MII_SR_100T2_FD_CAPS     0x0400    /* 100T2 Full Duplex Capable */
#define MII_SR_10T_HD_CAPS       0x0800    /* 10T   Half Duplex Capable */
#define MII_SR_10T_FD_CAPS       0x1000    /* 10T   Full Duplex Capable */
#define MII_SR_100X_HD_CAPS      0x2000    /* 100X  Half Duplex Capable */
#define MII_SR_100X_FD_CAPS      0x4000    /* 100X  Full Duplex Capable */
#define MII_SR_100T4_CAPS        0x8000    /* 100T4 Capable */

static void do_etesc_rx(conf_object_t *opaque, uint8_t *rx_data, uint16_t len, int channel);
static void etsec_update_irq(eTSEC_8641d_device *dev);
static void ievent_set(eTSEC_8641d_device *etsec, uint32_t flags);
#endif  

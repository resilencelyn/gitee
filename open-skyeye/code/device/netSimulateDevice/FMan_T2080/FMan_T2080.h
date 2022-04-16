#ifndef __FMAN_T2080_H__ 
#define __FMAN_T2080_H__ 

struct ID_FMHP{
	uint32_t BMI[0xFF];
	uint32_t QMI[0xFF];
	uint32_t Parser[0xFF];
};

struct hardware_ports_memory{
	uint32_t BMI_common[0xFF];
	uint32_t QMI_common[0xFF];
	struct ID_FMHP id_fmhp[63];
};

struct EMAC_regs{
	uint32_t mEMAC[0x200];
	uint32_t MACsec[0x200];
	uint32_t MDIO[0x400];
	
};

struct controller_configuration_data {
	uint32_t FMCDADDR;
	uint32_t FMCDREADY;
	uint32_t FMCDDATA[0x3c0];
};

struct rtl8201_PHY{
	uint32_t PHY_CR;
	uint32_t PHY_SR;
	uint32_t PHY1_Register;
	uint32_t PHY2_Register;
	uint32_t AAR;
	uint32_t ALPAR;
	uint32_t AER;
};

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
	unsigned char  h_srcIP[ETH_ALEN];   /* source IP addr */
};


#define NET_DEVICE(dev) ((struct net_device *)(dev)->net->obj)
struct FMan_T2080_device_t 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	conf_object_t* signal;

	skyeye_net_ops_intf *net_iface;
	conf_object_t *net;
	
	qportal_fd_intf* fd_iface;
	conf_object_t* fd;
	
	memory_space_intf *memory_iface;
	conf_object_t *memory;
	
	int net_sim_isok;
	uint32_t FMan_memory[0x20000];
	struct hardware_ports_memory FMHP;
	uint32_t Policer[0x400];
	uint32_t KeyGen[0x400];
	uint32_t DMA[0x400];
	uint32_t FPM[0x400];
	struct controller_configuration_data FMIR;
	uint32_t Soft_Parser[0x400];
	uint32_t Congestion_groups[0x400];
	uint32_t Virtual_Storage_Profile[0x400];
	struct EMAC_regs EMAC[10];
	uint32_t MDIO1[0x400];
	uint32_t MDIO2[0x400];
	uint32_t IEEE_1588[0x400];
	uint32_t PRAM[256][ 64 / 4];
	struct rtl8201_PHY PHY;
	/* Tx */
	uint8_t *tx_data;
	uint32_t tx_buffer_len;

	/* Rx */
	uint8_t       *rx_data;
	uint32_t       rx_buffer_len;
	uint32_t       rx_remaining_data;

	uint32_t       rx_padding;
	uint32_t       rx_addr;
	uint32_t       rx_size;
	uint8_t macInfo[6];

	//QPORTAL_FDESC *qportalFd;
	struct qportal_enqueue *QPORTAL_ENQUEUE;
	struct qportal_dequeue *QPORTAL_DEQUEUE;
};
typedef struct FMan_T2080_device_t FMan_T2080_device;
static char* regs_name[] = 
{
	NULL
};
#define ISBROADCAST(pmac) ({							\
	unsigned char broadcast[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};	\
	int ret = 0;								\
	ret = memcmp(pmac, broadcast, sizeof(broadcast));			\
	!ret;									\
})
#define ISMULTICAST(pmac) ((pmac)[0] & 0x1)
#define ISUNICAST(pmac) (!((pmac)[0] & 0x1))


#endif  

#ifndef __FT2000A_GMAC_H__
#define __FT2000A_GMAC_H__

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t res0     :2;  //����
		uint8_t RE       :1;  //����ʹ��
		uint8_t TE       :1;  //����ʹ��
		uint8_t DC       :1;  //���ڼ��
		uint8_t BL       :2;  //��������
		uint8_t ACS      :1;  //�Զ����� CRC/PAD
		uint8_t LUD      :1;  //Link UP/DOWN
		uint8_t DR       :1;  //�����ط�
		uint8_t IPC      :1;  //У��ж��
		uint8_t DM       :1;  //˫��ģʽ
		uint8_t LM       :1;  //�ػ�ģʽ
		uint8_t DO       :1;  //�����Խ���
		uint8_t FES      :1;  //�ٶ�
		uint8_t PS       :1;  //�˿�ѡ��
		uint8_t DCRS     :1;  //�ڴ�������в����� Sense
		uint8_t IFG      :3;  //֡�ڼ��
		uint8_t JE       :1;  //Jumbo ֡ʹ��
		uint8_t BE       :1;  //֡ͻ��ʹ��
		uint8_t JD       :1;  //Jabber �ر�
		uint8_t WD       :1;  //���Ź��ر�
		uint8_t TC       :1;  //�������� RGMII/SGMII/SMII
		uint8_t CST      :1;  //CRC ����
		uint8_t SFTERR   :1;  //SMII ǿ�ƴ������
	};
}cfg_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t PR      :1;  //����ģʽ
		uint32_t HUC     :1;  //Hash ���������
		uint32_t HMC     :1;  //Hash �෢�����
		uint32_t DAIF    :1;  //Ŀ�ĵ�ַ��ת����
		uint32_t PM      :1;  //ͨ�����й㲥
		uint32_t DBF     :1;  //��ʹ�ܹ㲥֡
		uint32_t PCF     :2;  //�������֡
		uint32_t SAIF    :1;  //Դ��ַ�������
		uint32_t SAF     :1;  //Դ��ַ����ʹ��
		uint32_t HPF     :1;  //Hash �� perfect ����
		uint32_t res     :20; //����
		uint32_t RA      :1;  //��������
	};
}frame_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint16_t GB       :1;  //MDIO ����æ������ PHY �Ķ�д��Ҫ������λΪ 1,��д��ɺ�ᱻӲ���Զ��� 0
		uint16_t GW       :1;  //GMII д
		uint16_t CR       :4;  //CSR ʱ�ӷ�Χ
		uint16_t GR       :5;  //GMII �Ĵ���
		uint16_t PA       :5;  //������ַ
	};
}gimm_addr_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint16_t FCB     :1;  //ȫ˫��ģʽ������ͣ֡����˫��ģʽ back-pressure����
		uint16_t TFE     :1;  //�������̿���ʹ��
		uint16_t RFE     :1;  //�������̿���ʹ��
		uint16_t UP      :1;  //��������ͣ֡̽��
		uint16_t PLT     :2;  //��ͣ����ֵ
		uint16_t res0    :1;  //����
		uint16_t DZPQ    :1;  //����Ϊ 1 ʱ���������� Zero-Quanta ��ͣ����֡
		uint16_t res1    :8;  //����
		uint16_t PT      :16; //���Ϳ���֡����ͣʱ��
	};
}flow_ctrl_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t SWR      :1;  //DMA ��λ
		uint8_t DA       :1;  //DMA �ٲõ���
		uint8_t DSL      :5;  //��������Ծ����
		uint8_t ATDS     :1;  //�����������С
		uint8_t PBL      :6;  //�ɱ��ͻ������
		uint8_t PR       :2;  //���ȼ�����
		uint8_t FB       :1;  //�̶�ͻ������
		uint8_t RPBL     :6;  //���� DMA �Ŀɱ��ͻ������
		uint8_t USP      :1;  //�÷�����ͻ������
		uint8_t PBL_Mode :1;  //����Ϊ 1 ʱ��PBL �� RPBL ���� 8 ��ͻ�����ȣ����ͻ������Ϊ 8��16��32��64��128��256
		uint8_t AAL      :1;  //��ַ���� Beats
		uint8_t MB       :1;  //���ͻ��
		uint8_t TXPR     :1;  //�������ȼ�
		uint8_t PRWG     :2;  //ͨ�����ȼ�
	};
}bus_mode_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t TI       :1;  //�����ж�
		uint8_t TPS      :1;  //���ʹ���ֹͣ
		uint8_t TU       :1;  //���� buffer ���ܻ�ȡ
		uint8_t TJT      :1;  //���� Jabber ��ʱ
		uint8_t OVF      :1;  //��������
		uint8_t UNF      :1;  //��������
		uint8_t RI       :1;  //�����ж�
		uint8_t RU       :1;  //���� Buffer ���ܻ�ȡ
		uint8_t RPS      :1;  //���մ���ֹͣ
		uint8_t RWT      :1;  //���տ��Ź���ʱ
		uint8_t ETI      :1;  //�緢���ж�
		uint8_t res0     :2;  //����
		uint8_t FBI      :1;  //�������ߴ����ж�
		uint8_t ERI      :1;  //������ж�
		uint8_t AIS      :1;  //�쳣�ж���״̬
		uint8_t NIS      :1;  //�����ж���״̬
		uint8_t RS       :3;  //���մ���״̬
		uint8_t TS       :3;  //���ʹ���״̬
		uint8_t EB       :3;  //����λ
		uint8_t GLI      :1;  //GMAC �߽ӿ��ж�
		uint8_t GMI      :1;  //GMAC MMC �ж�
		uint8_t GPI      :1;  //GMAC PMT �ж�
		uint8_t TTI      :1;  //ʱ��������ж�
		uint8_t GLPII    :1;  //GMAC �͹��Ľӿ��ж�
		uint8_t res1     :1;  //����
	};
}stu_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t res0     :1;  //����
		uint8_t SR       :1;  //����/ֹͣ����
		uint8_t OSF      :1;  //�����ڶ�֡
		uint8_t RTC      :2;  //������ֵ����
		uint8_t res1     :1;  //����
		uint8_t FUF      :1;  //���� FIFO ����С�� 64 �ֽڵ��޴�֡
		uint8_t FEF      :1;  //���մ���֡
		uint8_t FEC      :1;  //ʹ��Ӳ����������
		uint8_t RFA      :2;  //���ջ���̿�����ֵ
		uint8_t RFD      :2;  //��Ч����̿���
		uint8_t ST       :1;  //����/ֹͣ��������
		uint8_t TTC      :3;  //������ֵ����
		uint8_t res2     :3;  //����
		uint8_t FIF      :1;  //��շ��� FIFO
		uint8_t TSF      :1;  //���ʹ洢������
		uint8_t RFD2     :1;  //��Ч����̿�����ֵ��λ
		uint8_t RFA2     :1;  //����̿�����ֵ��λ
		uint8_t DFF      :1;  //��ֹ��ս���֡
		uint8_t RSF      :1;  //���մ洢�Ҵ���
		uint8_t DT       :1;  //������ TCP/IP У������֡
	};
}ope_mode_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t TIE      :1;  //�����ж�ʹ��
		uint8_t TST      :1;  //����ֹͣ�ж�ʹ��
		uint8_t TUE      :1;  //���� buffer ���ܻ�ȡ�ж�ʹ��
		uint8_t THE      :1;  //���� Jabber ��ʱ�ж�ʹ��
		uint8_t OVE      :1;  //���������ж�ʹ��
		uint8_t UNE      :1;  //���������ж�ʹ��
		uint8_t RIE      :1;  //�����ж�ʹ��
		uint8_t RUE      :1;  //���� Buffer ���ܻ�ȡ�ж�ʹ��
		uint8_t RSE      :1;  //����ֹͣ�ж�ʹ��
		uint8_t RWE      :1;  //���տ��Ź���ʱ�ж�ʹ��
		uint8_t ETE      :1;  //��ķ����ж�ʹ��
		uint8_t res0     :2;  //����
		uint8_t FBE      :1;  //�������ߴ���ʹ��
		uint8_t ERE      :1;  //��Ľ����ж�ʹ��
		uint8_t AIE      :1;  //�����ж���ʹ��
		uint8_t NIE      :1;  //�����ж���ʹ��
	};
}int_enable_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t UNDEF               :1;  //AXI ����û�ж��� BURST Length
		uint8_t BLEN4               :1;  //���� AXI ���� burst Length Ϊ 4
		uint8_t BLEN8               :1;  //���� AXI ���� burst Length Ϊ 8
		uint8_t BLEN16              :1;  //���� AXI ���� burst Length Ϊ 16
		uint8_t BLEN32              :1;  //���� AXI ���� burst Length Ϊ 32
		uint8_t BLEN64              :1;  //���� AXI ���� burst Length Ϊ 64
		uint8_t BLEN128             :1;  //���� AXI ���� burst Length Ϊ 128
		uint8_t BLEN256             :1;  //���� AXI ���� burst Length Ϊ 256
		uint8_t res0                :4;  //����
		uint8_t AXI_AAL             :1;  //AXI ���ߵ�ַ���� busrt ����д����
		uint8_t res1                :3;  //����
		uint8_t RD_OSR_LMT          :4;  //AXI ���Ķ� outstanding ��������
		uint8_t WR_OSR_LMT          :4;  //AXI ����д outstanding ��������
		uint8_t res2                :6;  //����
		uint8_t UNLCK_ON_MGK_RWK    :1;  //���� Magic Packet ��Զ�̻���
		uint8_t EN_LPI              :1;  //�͹��Ľӿ�ʹ��
	};
}axi_bus_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t W_BUSY              :1;  //AXI ����дͨ�����ڻ״̬�����ڴ�������
		uint8_t R_BUSY              :1;  //AXI ���߶�ͨ�����ڻ״̬�����ڴ�������
	};
}axi_stu_t;

typedef struct
{
	cfg_reg_t       mac_cfg_reg;				//MAC ����ģʽ���ƼĴ���
	frame_reg_t     mac_frame_filter;			//MAC ֡����
	uint32_t        hash_table_high;			//Hash ����˸� 32 λ
	uint32_t        hash_table_low;				//Hash ����˵� 32 λ
	gimm_addr_t     gmii_addr_reg;				//GMII ��ַ�Ĵ���
	uint32_t        gmii_data_reg;				//GMII ���ݼĴ���
	flow_ctrl_t     flow_control_reg;			//���̿��ƼĴ���
	uint32_t        version_reg;				//MAC �汾�Ĵ���
	uint32_t        mac_addr0_high;				//MAC ��ַ 0 ��λ�Ĵ���
	uint32_t        mac_addr0_low;				//MAC ��ַ 0 ��λ�Ĵ���
	bus_mode_t      bus_mode_reg;				//���������ӿ�ģʽ
	uint32_t        recv_desc_addr;				//DMA ���俪ʼ�������������ַ�Ĵ���,��һ�������������б�ĵ�ַ
	uint32_t        send_desc_addr;				//DMA ���俪ʼ�������������ַ�Ĵ���,��һ�������������б�ĵ�ַ
	stu_reg_t       status_reg;					//DMA ״̬�Ĵ���
	ope_mode_t      ope_mode_reg;				//DMA ����ģʽ�Ĵ���
	int_enable_t    interrupt_enable_reg;		//DMA �ж�ʹ�ܼĴ���
	axi_bus_t       axi_bus_mode_reg;			//AXI ����ģʽ�Ĵ���
	axi_stu_t       axi_status_reg;				//AXI ״̬�Ĵ���
}gmac_reg_t;

typedef struct
{
	uint32_t gmac0_awcache;
	uint32_t gmac0_arcache;
	uint32_t gmac1_awcache;
	uint32_t gmac1_arcache;
	uint32_t gmac_awdomain;
	uint32_t gmac_ardomain;
	uint32_t gmac_awbar;
	uint32_t gmac_arbar;
	uint32_t gmac_awsnoop;
	uint32_t gmac_arsnoop;
}gmac_config_reg_t;

struct gmac_registers
{
	gmac_reg_t  gmac_regs;
	gmac_config_reg_t  gmac_conf_regs;
};

struct ft2000a_gmac_device_t
{
	conf_object_t* obj;
	struct gmac_registers regs;
    uint8_t phy_addr;
    uint32_t phy_control;
    uint32_t phy_status;
    uint32_t phy_regs[32];
};

typedef struct ft2000a_gmac_device_t ft2000a_gmac_device;

static char* gmac_regs_name[] =
{
	"mac_cfg_reg",
	"mac_frame_filter",
	"hash_table_high",
	"hash_table_low",
	"gmii_addr_reg",
	"gmii_data_reg",
	"flow_control_reg",
	"version_reg",
	"mac_addr0_high",
	"mac_addr0_low",
	"bus_mode_reg",
	"recv_desc_addr",
	"send_desc_addr",
	"status_reg",
	"ope_mode_reg",
	"interrupt_enable_reg",
	"axi_bus_mode_reg",
	"axi_status_reg",
	NULL
};

static char* config_regs_name[] =
{
	"gmac0_awcache",
	"gmac0_arcache",
	"gmac1_awcache",
	"gmac1_arcache",
	"gmac_awdomain",
	"gmac_ardomain",
	"gmac_awbar",
	"gmac_arbar",
	"gmac_awsnoop",
	"gmac_arsnoop",
	NULL
};

#define MAC_CFG_REG        0x0000
#define MAC_FRAME_FILTER   0x0004
#define HTH                0x0008
#define HTL                0x000c
#define GMII_ADDR          0x0010
#define GMII_DATA          0x0014
#define FLOW_CTRL_REG      0x0018
#define VERSION_REG        0x0020
#define MAH                0x0040
#define MAL                0x0044
#define BUS_MODE           0x1000
#define RECV_DESC_ADDR     0x100c
#define SEND_DESC_ADDR     0x1010
#define STATUS_REG         0x1014
#define OPE_MODE_REG       0x1018
#define INT_ENABLE_REG     0x101c
#define AXI_BUS_MODE       0x1028
#define AXI_STATUS         0x102c

#define CONFIG_BASE_ADDR   0x0000
#define GMAC0_AWCACHE      (CONFIG_BASE_ADDR + 0x0000)
#define GMAC0_ARCACHE      (CONFIG_BASE_ADDR + 0x0004)
#define GMAC1_AWCACHE      (CONFIG_BASE_ADDR + 0x0008)
#define GMAC1_ARCACHE      (CONFIG_BASE_ADDR + 0x000c)
#define GMAC_AWDOMAIN      (CONFIG_BASE_ADDR + 0x0010)
#define GMAC_ARDOMAIN      (CONFIG_BASE_ADDR + 0x0014)
#define GMAC_AWBAR         (CONFIG_BASE_ADDR + 0x0018)
#define GMAC_ARBAR         (CONFIG_BASE_ADDR + 0x001c)
#define GMAC_AWSNOOP       (CONFIG_BASE_ADDR + 0x0020)
#define GMAC_ARSNOOP       (CONFIG_BASE_ADDR + 0x0024)

/*Generic MII registers*/
typedef enum
{
    MII_BMCR = 0,               /*Basic mode control resiger*/
    MII_BMSR,                   /*Basic mode status resiger*/
    MII_PHYSID1,                /*PHYS ID1 resiger*/
    MII_PHYSID2,                /*PHYS ID2 resiger*/
    MII_ADVERTISE,              /*advertise control resiger*/
    MII_LPA,                    /*Link partner ability reg*/
    MII_EXPANSION,              /*Expansion register*/
    MII_CTRL1000 = 9,           /*1000BASE-T control*/
    MII_STAT1000,               /*1000BASE-T status*/
    MII_MMD_CTRL = 0xD,         /*MMD Access Control register*/
    MII_MMD_DATA,               /*MMD Access Data register*/
    MII_ESTATUS,                /*Extended Status*/
    MII_DCOUNTER = 0x12,        /*Disconnect counter*/
    MII_FCSOUNTER,              /*False carrier counter*/
    MII_NWAYTEST,               /*N-way auto-neg test reg*/
    MII_RERRCOUNTER,            /*Receive error counter*/
    MII_SREVISION,              /*0x16 Silicon revision*/
    MII_RESV1,                  /*Reserved*/
    MII_LBRERROR,               /*Lpback, rx bypass error*/
    MII_PHYADDR,                /*PHY address*/
    MII_RESV2,                  /*reserved*/
    MII_TPISTATUS,              /*TPI status for 10mbps*/
    MII_NCONFIG,                /*0x1C Network interface config*/
    MII_MAX_OFFSET
}MII_REG_OFFSET;

/*Basic mode control register*/
#define BMCR_RESV           0x003f
#define BMCR_SPEED1000      0x0040
#define BMCR_CTST           0x0080
#define BMCR_FULLDPLX       0x0100
#define BMCR_ANRESTART      0x0200
#define BMCR_ISOLATE        0x0400
#define BMCR_PDOWN          0x0800
#define BMCR_ANENABLE       0x1000
#define BMCR_SPEED100       0x2000
#define BMCR_LOOPBACK       0x4000
#define BMCE_RESET          0x8000

/*Basic mode status register*/
#define BMSR_ERCAP          0x0001
#define BMSR_JCD            0x0002
#define BMSR_LSTATUS        0x0004
#define BMSR_ANEGCAPABLE    0x0008
#define BMSR_RFAULT         0x0010
#define BMSR_ANEGCOMPLETE   0x0020
#define BMSR_RESV           0x00c0
#define BMSR_ESTATEN        0x0100
#define BMSR_100HALF2       0x0200
#define BMSR_100FULL2       0x0400
#define BMSR_10HALF         0x0800
#define BMSR_10FULL         0x1000
#define BMSR_100HALF        0x2000
#define BMSR_100FULL        0x4000
#define BMSR_100BASE4       0x8000

static uint32_t gmac_regs_offset[] =
{
	MAC_CFG_REG,
	MAC_FRAME_FILTER,
	HTH,
	HTL,
	GMII_ADDR,
	GMII_DATA,
	FLOW_CTRL_REG,
	VERSION_REG,
	MAH,
	MAL,
	BUS_MODE,
	RECV_DESC_ADDR,
	SEND_DESC_ADDR,
	STATUS_REG,
	OPE_MODE_REG,
	INT_ENABLE_REG,
	AXI_BUS_MODE,
	AXI_STATUS,
};

static uint32_t config_regs_offset[] =
{
	GMAC0_AWCACHE,
	GMAC0_ARCACHE,
	GMAC1_AWCACHE,
	GMAC1_ARCACHE,
	GMAC_AWDOMAIN,
	GMAC_ARDOMAIN,
	GMAC_AWBAR,
	GMAC_ARBAR,
	GMAC_AWSNOOP,
	GMAC_ARSNOOP,
};


#define BMSR_ANEGCOMPLETE 0x20
#endif

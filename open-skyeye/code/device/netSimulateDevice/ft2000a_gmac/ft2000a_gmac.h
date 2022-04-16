#ifndef __FT2000A_GMAC_H__
#define __FT2000A_GMAC_H__

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t res0     :2;  //保留
		uint8_t RE       :1;  //接收使能
		uint8_t TE       :1;  //发送使能
		uint8_t DC       :1;  //延期检测
		uint8_t BL       :2;  //后退限制
		uint8_t ACS      :1;  //自动剥离 CRC/PAD
		uint8_t LUD      :1;  //Link UP/DOWN
		uint8_t DR       :1;  //不能重发
		uint8_t IPC      :1;  //校验卸下
		uint8_t DM       :1;  //双工模式
		uint8_t LM       :1;  //回环模式
		uint8_t DO       :1;  //不能自接收
		uint8_t FES      :1;  //速度
		uint8_t PS       :1;  //端口选择
		uint8_t DCRS     :1;  //在传输过程中不搬运 Sense
		uint8_t IFG      :3;  //帧内间隔
		uint8_t JE       :1;  //Jumbo 帧使能
		uint8_t BE       :1;  //帧突发使能
		uint8_t JD       :1;  //Jabber 关闭
		uint8_t WD       :1;  //看门狗关闭
		uint8_t TC       :1;  //传输配置 RGMII/SGMII/SMII
		uint8_t CST      :1;  //CRC 剥离
		uint8_t SFTERR   :1;  //SMII 强制传输错误
	};
}cfg_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t PR      :1;  //混杂模式
		uint32_t HUC     :1;  //Hash 单发射过滤
		uint32_t HMC     :1;  //Hash 多发射过滤
		uint32_t DAIF    :1;  //目的地址反转过滤
		uint32_t PM      :1;  //通过所有广播
		uint32_t DBF     :1;  //不使能广播帧
		uint32_t PCF     :2;  //处理控制帧
		uint32_t SAIF    :1;  //源地址反相过滤
		uint32_t SAF     :1;  //源地址过滤使能
		uint32_t HPF     :1;  //Hash 或 perfect 过滤
		uint32_t res     :20; //保留
		uint32_t RA      :1;  //接收所有
	};
}frame_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint16_t GB       :1;  //MDIO 总线忙，对于 PHY 的读写需要设置这位为 1,读写完成后会被硬件自动清 0
		uint16_t GW       :1;  //GMII 写
		uint16_t CR       :4;  //CSR 时钟范围
		uint16_t GR       :5;  //GMII 寄存器
		uint16_t PA       :5;  //物理层地址
	};
}gimm_addr_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint16_t FCB     :1;  //全双工模式发送暂停帧，半双工模式 back-pressure功能
		uint16_t TFE     :1;  //发送流程控制使能
		uint16_t RFE     :1;  //接收流程控制使能
		uint16_t UP      :1;  //单发射暂停帧探测
		uint16_t PLT     :2;  //暂停低域值
		uint16_t res0    :1;  //保留
		uint16_t DZPQ    :1;  //设置为 1 时：不能生成 Zero-Quanta 暂停控制帧
		uint16_t res1    :8;  //保留
		uint16_t PT      :16; //发送控制帧，暂停时间
	};
}flow_ctrl_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t SWR      :1;  //DMA 软复位
		uint8_t DA       :1;  //DMA 仲裁调度
		uint8_t DSL      :5;  //描述符跳跃长度
		uint8_t ATDS     :1;  //交替描述表大小
		uint8_t PBL      :6;  //可编程突发长度
		uint8_t PR       :2;  //优先级比率
		uint8_t FB       :1;  //固定突发长度
		uint8_t RPBL     :6;  //接收 DMA 的可编程突发长度
		uint8_t USP      :1;  //用分离编程突发长度
		uint8_t PBL_Mode :1;  //设置为 1 时，PBL 和 RPBL 乘以 8 的突发长度，因此突发长度为 8、16、32、64、128、256
		uint8_t AAL      :1;  //地址对齐 Beats
		uint8_t MB       :1;  //混合突发
		uint8_t TXPR     :1;  //发送优先级
		uint8_t PRWG     :2;  //通道优先级
	};
}bus_mode_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t TI       :1;  //发送中断
		uint8_t TPS      :1;  //发送处理停止
		uint8_t TU       :1;  //发送 buffer 不能获取
		uint8_t TJT      :1;  //发送 Jabber 超时
		uint8_t OVF      :1;  //接收上溢
		uint8_t UNF      :1;  //发送下溢
		uint8_t RI       :1;  //接收中断
		uint8_t RU       :1;  //接收 Buffer 不能获取
		uint8_t RPS      :1;  //接收处理停止
		uint8_t RWT      :1;  //接收看门狗超时
		uint8_t ETI      :1;  //早发送中断
		uint8_t res0     :2;  //保留
		uint8_t FBI      :1;  //致命总线错误中断
		uint8_t ERI      :1;  //早接收中断
		uint8_t AIS      :1;  //异常中断总状态
		uint8_t NIS      :1;  //正常中断总状态
		uint8_t RS       :3;  //接收处理状态
		uint8_t TS       :3;  //发送处理状态
		uint8_t EB       :3;  //错误位
		uint8_t GLI      :1;  //GMAC 线接口中断
		uint8_t GMI      :1;  //GMAC MMC 中断
		uint8_t GPI      :1;  //GMAC PMT 中断
		uint8_t TTI      :1;  //时间戳触发中断
		uint8_t GLPII    :1;  //GMAC 低功耗接口中断
		uint8_t res1     :1;  //保留
	};
}stu_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t res0     :1;  //保留
		uint8_t SR       :1;  //启动/停止接收
		uint8_t OSF      :1;  //操作第二帧
		uint8_t RTC      :2;  //接收域值控制
		uint8_t res1     :1;  //保留
		uint8_t FUF      :1;  //接收 FIFO 接收小于 64 字节的无错帧
		uint8_t FEF      :1;  //接收错误帧
		uint8_t FEC      :1;  //使能硬件控制流程
		uint8_t RFA      :2;  //接收活动流程控制域值
		uint8_t RFD      :2;  //无效活动流程控制
		uint8_t ST       :1;  //启动/停止发送命令
		uint8_t TTC      :3;  //发送域值控制
		uint8_t res2     :3;  //保留
		uint8_t FIF      :1;  //清空发送 FIFO
		uint8_t TSF      :1;  //发送存储且运送
		uint8_t RFD2     :1;  //无效活动流程控制域值高位
		uint8_t RFA2     :1;  //活动流程控制域值高位
		uint8_t DFF      :1;  //禁止清空接收帧
		uint8_t RSF      :1;  //接收存储且处理
		uint8_t DT       :1;  //不丢弃 TCP/IP 校验错误的帧
	};
}ope_mode_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t TIE      :1;  //发送中断使能
		uint8_t TST      :1;  //发送停止中断使能
		uint8_t TUE      :1;  //发送 buffer 不能获取中断使能
		uint8_t THE      :1;  //发送 Jabber 超时中断使能
		uint8_t OVE      :1;  //接收上溢中断使能
		uint8_t UNE      :1;  //发送下溢中断使能
		uint8_t RIE      :1;  //接收中断使能
		uint8_t RUE      :1;  //接收 Buffer 不能获取中断使能
		uint8_t RSE      :1;  //接收停止中断使能
		uint8_t RWE      :1;  //接收看门狗超时中断使能
		uint8_t ETE      :1;  //早的发送中断使能
		uint8_t res0     :2;  //保留
		uint8_t FBE      :1;  //致命总线错误使能
		uint8_t ERE      :1;  //早的接收中断使能
		uint8_t AIE      :1;  //反常中断总使能
		uint8_t NIE      :1;  //正常中断总使能
	};
}int_enable_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t UNDEF               :1;  //AXI 总线没有定义 BURST Length
		uint8_t BLEN4               :1;  //设置 AXI 总线 burst Length 为 4
		uint8_t BLEN8               :1;  //设置 AXI 总线 burst Length 为 8
		uint8_t BLEN16              :1;  //设置 AXI 总线 burst Length 为 16
		uint8_t BLEN32              :1;  //设置 AXI 总线 burst Length 为 32
		uint8_t BLEN64              :1;  //设置 AXI 总线 burst Length 为 64
		uint8_t BLEN128             :1;  //设置 AXI 总线 burst Length 为 128
		uint8_t BLEN256             :1;  //设置 AXI 总线 burst Length 为 256
		uint8_t res0                :4;  //保留
		uint8_t AXI_AAL             :1;  //AXI 总线地址对齐 busrt 读、写传输
		uint8_t res1                :3;  //保留
		uint8_t RD_OSR_LMT          :4;  //AXI 最大的读 outstanding 请求限制
		uint8_t WR_OSR_LMT          :4;  //AXI 最大的写 outstanding 请求限制
		uint8_t res2                :6;  //保留
		uint8_t UNLCK_ON_MGK_RWK    :1;  //开启 Magic Packet 和远程唤醒
		uint8_t EN_LPI              :1;  //低功耗接口使能
	};
}axi_bus_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t W_BUSY              :1;  //AXI 总线写通道处于活动状态并且在传输数据
		uint8_t R_BUSY              :1;  //AXI 总线读通道处于活动状态并且在传输数据
	};
}axi_stu_t;

typedef struct
{
	cfg_reg_t       mac_cfg_reg;				//MAC 操作模式配制寄存器
	frame_reg_t     mac_frame_filter;			//MAC 帧过滤
	uint32_t        hash_table_high;			//Hash 表过滤高 32 位
	uint32_t        hash_table_low;				//Hash 表过滤低 32 位
	gimm_addr_t     gmii_addr_reg;				//GMII 地址寄存器
	uint32_t        gmii_data_reg;				//GMII 数据寄存器
	flow_ctrl_t     flow_control_reg;			//流程控制寄存器
	uint32_t        version_reg;				//MAC 版本寄存器
	uint32_t        mac_addr0_high;				//MAC 地址 0 高位寄存器
	uint32_t        mac_addr0_low;				//MAC 地址 0 低位寄存器
	bus_mode_t      bus_mode_reg;				//控制主机接口模式
	uint32_t        recv_desc_addr;				//DMA 传输开始接收描述符表地址寄存器,第一个接收描述符列表的地址
	uint32_t        send_desc_addr;				//DMA 传输开始发送描述符表地址寄存器,第一个发送描述符列表的地址
	stu_reg_t       status_reg;					//DMA 状态寄存器
	ope_mode_t      ope_mode_reg;				//DMA 操作模式寄存器
	int_enable_t    interrupt_enable_reg;		//DMA 中断使能寄存器
	axi_bus_t       axi_bus_mode_reg;			//AXI 总线模式寄存器
	axi_stu_t       axi_status_reg;				//AXI 状态寄存器
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

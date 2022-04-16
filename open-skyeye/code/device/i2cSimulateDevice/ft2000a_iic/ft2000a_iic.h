#ifndef __FT2000A_IIC_H__
#define __FT2000A_IIC_H__

typedef union
{
	uint16_t all;
	struct
	{
		uint8_t MASTER_MODE           :1;  //此位是 I2C Master 的使能位
		uint8_t SPEED                 :2;  //这个参数用来设定 I2C 控制器工作在 Master 模式时的速率
		uint8_t IC_10BITADDR_SLAVE    :1;  //当工作在 slave 模式时，此位用来选择 I2C 控制器响应 7位地址访问模式还是响应 10 位地址访问请求模式
		uint8_t IC_10BITADDR_MASTER   :1;  //配置参数
		uint8_t IC_RESTART_EN         :1;  //此位设置作为 I2C Master 使用时是否支持 restart 功能
		uint8_t IC_SLAVE_DISABLE      :1;  //此位是 I2C Slave 功能是否关闭的控制位
	};
}iic_con_t;

typedef union
{
	uint16_t all;
	struct
	{
		uint8_t DAT                   :8;  //DAT中存放用来发送的数据或从I2C总线上接收到的数据
		uint8_t CMD                   :1;  //此位是 I2C 控制器工作在 Master 模式时进行读写操作的控制位
		uint8_t STOP                  :1;  //此位设置是否在发送或接收到一个字节数据后发起STOP
		uint8_t RESTART               :1;  //此 位设置是否在发送或接收一个字节数据前发起RESTART
	};
}data_cmd_t;

typedef union
{
	uint16_t all;
	struct
	{
		uint8_t M_RX_UNDER            :1;
		uint8_t M_RX_OVER             :1;
		uint8_t M_RX_FULL             :1;
		uint8_t M_TX_OVER             :1;
		uint8_t M_TX_EMPTY            :1;
		uint8_t M_RD_REQ              :1;
		uint8_t M_TX_ABRT             :1;
		uint8_t M_RX_DONE             :1;
		uint8_t M_ACTIVITY            :1;
		uint8_t M_STOP_DET            :1;
		uint8_t M_START_DET           :1;
		uint8_t M_GEN_CALL            :1;
	};
}intr_mask_t;

typedef union
{
	uint16_t all;
	struct
	{
		uint8_t RX_UNDER              :1;
		uint8_t RX_OVER               :1;
		uint8_t RX_FULL               :1;
		uint8_t TX_OVER               :1;
		uint8_t TX_EMPTY              :1;
		uint8_t RD_REQ                :1;
		uint8_t TX_ABRT               :1;
		uint8_t RX_DONE               :1;
		uint8_t ACTIVITY              :1;
		uint8_t STOP_DET              :1;
		uint8_t START_DET             :1;
		uint8_t GEN_CALL              :1;
	};
}intr_state_t;

typedef union
{
	uint16_t all;
	struct
	{
		uint8_t ACTIVITY              :1;  //I2C 控制器活动状态标志
		uint8_t TFNF                  :1;  //发送 FIFO 未满标志
		uint8_t TFE                   :1;  //发送 FIFO 全空标志
		uint8_t RFNE                  :1;  //接收 FIFO 不为空标志
		uint8_t RFF                   :1;  //接收 FIFO 全满标志
		uint8_t MST_ACTIVITY          :1;  //Master FSM活动状态标志
		uint8_t SLV_ACTIVITY          :1;  //Slave FSM 活动状态标志
	};
}iic_state_t;

typedef union
{
	uint16_t all;
	struct
	{
		uint16_t IC_TAR                :10; //此位用来选择工作在 I2C Master 时使用 7 位地址模式还是 10 位地址模式进行通信
		uint16_t GC_OR_START           :1;  //此位用来选择 I2C 通信使用广播呼叫地址格式还是使用START BYTE 格式
		uint16_t SPECIAL               :1;  //如果位 11 (SPECIAL)为 1，此位设定 DW_apb_i2c 使用广播呼叫地址格式还是 START BYTE 格式
		uint16_t IC_10BITADDR_MASTER   :1;  //此处用来存放 Master 通信的目的地址
	};
}iic_tar_t;

struct iic_registers
{
	iic_con_t iic_con;						//I2C 控制寄存器
	iic_tar_t iic_tar;						//I2C 目的地址寄存器
	uint16_t iic_sar;						//I2C 从设备地址寄存器
	uint16_t iic_hs_maddr;					//I2C 接收/发送数据 buff 和命令寄存器
	data_cmd_t iic_data_cmd;				//I2C 接收/发送数据 buff 和命令寄存器
	uint16_t iic_ss_scl_hcnt;				//标准速率 I2C 时钟高速计数器
	uint16_t iic_ss_scl_lcnt;				//标准速率 I2C 时钟低速计数器
	uint16_t iic_fs_scl_hcnt;				//快速 I2C 时钟高速计数器
	uint16_t iic_fs_scl_lcnt;				//快速 I2C 时钟低速计数器
	uint16_t iic_hs_scl_hcnt;				//高速 I2C 时钟快速计数器
	uint16_t iic_hs_scl_lcnt;				//高速 I2C 时钟低速计数器
	intr_mask_t iic_intr_mask;				//I2C 中断 mask 寄存器
	intr_state_t iic_intr_state;			//I2C 原始中断状态寄存器
	uint16_t iic_rx_tl;						//I2C 接收 FIFO 门限寄存器
	uint16_t iic_tx_tl;						//I2C 发送 FIFO 门限寄存器
	uint16_t iic_enable;					//I2C 使能寄存器
	iic_state_t iic_state;						//I2C 状态寄存器
	uint16_t iic_enable_state;				//I2C 使能状态寄存器
};

struct ft2000a_iic_device_t
{
	conf_object_t* obj;
	struct iic_registers regs;
#define FIFO_SIZE 256
	char fifo_buffer[FIFO_SIZE];
	conf_object_t* i2c_bus;
	i2c_bus_intf *i2c_bus_iface;
	conf_object_t* signal;
	general_signal_intf* signal_iface;
	int int_number;
	FIFO* send_fifo;
};

typedef struct ft2000a_iic_device_t ft2000a_iic_device;

static char* regs_name[] =
{
	"iic_con",
	"iic_tar",
	"iic_sar",
	"iic_hs_maddr",
	"iic_data_cmd",
	"iic_ss_scl_hcnt",
	"iic_ss_scl_lcnt",
	"iic_fs_scl_hcnt",
	"iic_fs_scl_lcnt",
	"iic_hs_scl_hcnt",
	"iic_hs_scl_lcnt",
	"iic_intr_mask",
	"iic_intr_state",
	"iic_rx_tl",
	"iic_tx_tl",
	"iic_enable",
	"iic_state",
	"iic_enable_state",
	NULL
};

#define IIC_CON                  0x00
#define IIC_TAR                  0x04
#define IIC_SAR                  0x08
#define IIC_HS_MADDR             0x0c
#define IIC_DATA_CMD             0x10
#define IIC_SS_SCL_HCNT          0x14
#define IIC_SS_SCL_LCNT          0x18
#define IIC_FS_SCL_HCNT          0x1c
#define IIC_FS_SCL_LCNT          0x20
#define IIC_HS_SCL_HCNT          0x24
#define IIC_HS_SCL_LCNT          0x28
#define IIC_INTR_MASK            0x30
#define IIC_INTR_STATE           0x34
#define IIC_RX_TL                0x38
#define IIC_TX_TL                0x3c
#define IIC_ENABLE               0x6c
#define IIC_STATE                0x70
#define IIC_ENABLE_STATE         0x9c

static uint32_t regs_offset[] =
{
	IIC_CON,
	IIC_TAR,
	IIC_SAR,
	IIC_HS_MADDR,
	IIC_DATA_CMD,
	IIC_SS_SCL_HCNT,
	IIC_SS_SCL_LCNT,
	IIC_FS_SCL_HCNT,
	IIC_FS_SCL_LCNT,
	IIC_HS_SCL_HCNT,
	IIC_HS_SCL_LCNT,
	IIC_INTR_MASK,
	IIC_INTR_STATE,
	IIC_RX_TL,
	IIC_TX_TL,
	IIC_ENABLE,
	IIC_STATE,
	IIC_ENABLE_STATE,
};

#define IC_HS_MASTER_CODE  0
#endif

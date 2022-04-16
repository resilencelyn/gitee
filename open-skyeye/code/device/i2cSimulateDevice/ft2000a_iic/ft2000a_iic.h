#ifndef __FT2000A_IIC_H__
#define __FT2000A_IIC_H__

typedef union
{
	uint16_t all;
	struct
	{
		uint8_t MASTER_MODE           :1;  //��λ�� I2C Master ��ʹ��λ
		uint8_t SPEED                 :2;  //������������趨 I2C ������������ Master ģʽʱ������
		uint8_t IC_10BITADDR_SLAVE    :1;  //�������� slave ģʽʱ����λ����ѡ�� I2C ��������Ӧ 7λ��ַ����ģʽ������Ӧ 10 λ��ַ��������ģʽ
		uint8_t IC_10BITADDR_MASTER   :1;  //���ò���
		uint8_t IC_RESTART_EN         :1;  //��λ������Ϊ I2C Master ʹ��ʱ�Ƿ�֧�� restart ����
		uint8_t IC_SLAVE_DISABLE      :1;  //��λ�� I2C Slave �����Ƿ�رյĿ���λ
	};
}iic_con_t;

typedef union
{
	uint16_t all;
	struct
	{
		uint8_t DAT                   :8;  //DAT�д���������͵����ݻ��I2C�����Ͻ��յ�������
		uint8_t CMD                   :1;  //��λ�� I2C ������������ Master ģʽʱ���ж�д�����Ŀ���λ
		uint8_t STOP                  :1;  //��λ�����Ƿ��ڷ��ͻ���յ�һ���ֽ����ݺ���STOP
		uint8_t RESTART               :1;  //�� λ�����Ƿ��ڷ��ͻ����һ���ֽ�����ǰ����RESTART
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
		uint8_t ACTIVITY              :1;  //I2C �������״̬��־
		uint8_t TFNF                  :1;  //���� FIFO δ����־
		uint8_t TFE                   :1;  //���� FIFO ȫ�ձ�־
		uint8_t RFNE                  :1;  //���� FIFO ��Ϊ�ձ�־
		uint8_t RFF                   :1;  //���� FIFO ȫ����־
		uint8_t MST_ACTIVITY          :1;  //Master FSM�״̬��־
		uint8_t SLV_ACTIVITY          :1;  //Slave FSM �״̬��־
	};
}iic_state_t;

typedef union
{
	uint16_t all;
	struct
	{
		uint16_t IC_TAR                :10; //��λ����ѡ������ I2C Master ʱʹ�� 7 λ��ַģʽ���� 10 λ��ַģʽ����ͨ��
		uint16_t GC_OR_START           :1;  //��λ����ѡ�� I2C ͨ��ʹ�ù㲥���е�ַ��ʽ����ʹ��START BYTE ��ʽ
		uint16_t SPECIAL               :1;  //���λ 11 (SPECIAL)Ϊ 1����λ�趨 DW_apb_i2c ʹ�ù㲥���е�ַ��ʽ���� START BYTE ��ʽ
		uint16_t IC_10BITADDR_MASTER   :1;  //�˴�������� Master ͨ�ŵ�Ŀ�ĵ�ַ
	};
}iic_tar_t;

struct iic_registers
{
	iic_con_t iic_con;						//I2C ���ƼĴ���
	iic_tar_t iic_tar;						//I2C Ŀ�ĵ�ַ�Ĵ���
	uint16_t iic_sar;						//I2C ���豸��ַ�Ĵ���
	uint16_t iic_hs_maddr;					//I2C ����/�������� buff ������Ĵ���
	data_cmd_t iic_data_cmd;				//I2C ����/�������� buff ������Ĵ���
	uint16_t iic_ss_scl_hcnt;				//��׼���� I2C ʱ�Ӹ��ټ�����
	uint16_t iic_ss_scl_lcnt;				//��׼���� I2C ʱ�ӵ��ټ�����
	uint16_t iic_fs_scl_hcnt;				//���� I2C ʱ�Ӹ��ټ�����
	uint16_t iic_fs_scl_lcnt;				//���� I2C ʱ�ӵ��ټ�����
	uint16_t iic_hs_scl_hcnt;				//���� I2C ʱ�ӿ��ټ�����
	uint16_t iic_hs_scl_lcnt;				//���� I2C ʱ�ӵ��ټ�����
	intr_mask_t iic_intr_mask;				//I2C �ж� mask �Ĵ���
	intr_state_t iic_intr_state;			//I2C ԭʼ�ж�״̬�Ĵ���
	uint16_t iic_rx_tl;						//I2C ���� FIFO ���޼Ĵ���
	uint16_t iic_tx_tl;						//I2C ���� FIFO ���޼Ĵ���
	uint16_t iic_enable;					//I2C ʹ�ܼĴ���
	iic_state_t iic_state;						//I2C ״̬�Ĵ���
	uint16_t iic_enable_state;				//I2C ʹ��״̬�Ĵ���
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

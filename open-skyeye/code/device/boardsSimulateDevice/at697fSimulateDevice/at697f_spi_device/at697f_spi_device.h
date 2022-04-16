#ifndef __AT697F_SPI_DEVICE_H__
#define __AT697F_SPI_DEVICE_H__
#define SPI_MSGQ_DATA_LEN 4096
#define SPI_MSGQ_HEADER_SIZE 5
#define MAXSIZE (SPI_MSGQ_DATA_LEN + 12)
#define FILE_MAX_NUM		24
#define MAX_FILE_NAME_LEN	256
typedef struct data_buf {
	unsigned char byResved;
	unsigned char	bySynData;
	unsigned char	byDevData;
	unsigned char	byDataType;
	uint16_t	wDataLen;
	unsigned char	byData[SPI_MSGQ_DATA_LEN + 6];
} data_buf_t;

typedef struct transmit_data {
	int pos;
	data_buf_t *buf;
} transmit_data_t;

typedef struct receive_data {
	int len;
	char *buf;
} receive_data_t;

// Device object structure
struct at697f_spi_device_device_t
{
	
	conf_object_t* obj;
	
	// Interface objects and methods;
	conf_object_t *spi_bus_obj;
	spi_bus_intf *spi_bus_iface;
	transmit_data_t tx_data;
	int index;
	unsigned char recvbuf[MAXSIZE];
	int is_read_action;

#define FILE_MAX_NUM 12
	FILE* fp[FILE_MAX_NUM];
	uint8_t fileCount;
	char *file[FILE_MAX_NUM];

	uint8_t start_doc_cmd[2];
	uint16_t count;
	data_buf_t recvBuf;
	int recvDataLen;
	int validFrame;
	int msgCnt;
	int readDataCnt[FILE_MAX_NUM];
	int readMsgCnt[FILE_MAX_NUM];
};
typedef struct at697f_spi_device_device_t at697f_spi_device_device;

#define SPI_MICU_EVENT			0x11	// 事件转发
#define SPI_MICU_EM_MSG			0x22	// 紧急断电
#define SPI_MICU_SW_MSG			0x23	// 切备时给数管发送的断电消息

#define SPI_TIME_CODE			0x33	// 时间码
#define SPI_BCST_DINGWEI		0x41	// 定位数据
#define SPI_BCST_ZITAI			0x42	// 姿态数据
#define SPI_BCST_WEIZHONGLI		0x43	// 微重力数据
#define SPI_BCST_GUIDAO			0x44	// 轨道参数
#define SPI_BCST_HUANREQI		0x45	// 换热器数据
#define SPI_SYS_STATE			0x55	// 系统状态
#define SPI_TRANS_INJECT		0x66	// 转发数据注入
#define SPI_INJECT_RETURN		0x77	// 1553B数据注入返回
#define SPI_PRJ_DATA			0x88	// 工程数据
#define SPI_PRJ_DATA_REC_SG     0x89    // 采集数管工参
#define SPI_DIGITAL_YAOCE2		0x92	// 数字量遥测//数管
#define SPI_DIGITAL_YAOCE3		0x93	// 数字量遥测//存储
#define SPI_DIGITAL_YAOCE4		0x94	// 数字量遥测//计算
#define SPI_DIGITAL_OTHER		0x95	// 数字量遥测//NT

#define SPI_ALARM_PARAM			0xAA	// 报警参数
#define SPI_APP_DATA		    0xBB	// 应用数据
#define SPI_10GE_INJECT			0xCC	// 万兆网数据注入
#define SPI_BCST_BUSCMD			0xDD	// 改为广播总线指令

#define SPI_DIGITAL_JH1			0x99	// 采集交换1数字量 JH1->MICU,与交换调试后确认为0x99-20190913
#define SPI_INJ_FILE_JH1		0xFF	// 转发数据注入交换1文件 MICU->JH1 20191009
//#define SPI_PRJ_JH1           0xEE

#define SPI_DIGITAL_CG			0xE1	// 采集重构数字量 CG->MICU
#define SPI_PRJ_DATA_CG			0xE2	// 采集重构工参 CG->MICU
#define SPI_INJ_FILE_CG			0xE3	// 转发数据注入重构文件 MICU->CG

#define SPI_CHAXUN_CMD          0xC0    //查询指令
#define SPI_DOC_CMD             0xCD    //文件数据注入

#define CHONGGOU_STATE_NO_BIT	0x80	// 包序号错误
#define CHONGGOU_STATE_CRC_BIT	0x40	// 包校验错误
#define CHONGGOU_STATE_BUSY		0x20	// 正在写入flash

#define ZHUKONG 0x69
#define SPI_COMM_SYNC 0x98

#define outnum 18
typedef struct{
		uint8_t ID;
		unsigned char type;
		unsigned char* type_name;
		double cur_time;
		uint8_t data[SPI_MSGQ_DATA_LEN +6];
}OUTPUTINFO;


OUTPUTINFO outinfo[outnum] =
{
	{1, SPI_TIME_CODE, "SPI_TIME_CODE", 0.0, {0}},
	{2, SPI_MICU_EM_MSG, "SPI_MICU_EM_MSG", 0.0, {0}},
	{3, SPI_MICU_SW_MSG, "SPI_MICU_SW_MSG", 0.0, {0}},
	{4, SPI_BCST_DINGWEI, "SPI_BCST_DINGWEI", 0.0, {0}},
	{5, SPI_BCST_ZITAI, "SPI_BCST_ZITAI", 0.0, {0}},
	{6, SPI_BCST_WEIZHONGLI, "SPI_BCST_WEIZHONGLI", 0.0, {0}},
	{7, SPI_BCST_GUIDAO, "SPI_BCST_GUIDAO", 0.0, {0}},
	{8, SPI_BCST_HUANREQI, "SPI_BCST_HUANREQI", 0.0, {0}},
	{9, SPI_SYS_STATE, "SPI_SYS_STATE", 0.0, {0}},
	{10, SPI_BCST_BUSCMD, "SPI_BCST_BUSCMD", 0.0, {0}},
	{11, SPI_INJECT_RETURN, "SPI_INJECT_RETURN", 0.0, {0}},
	{12, SPI_PRJ_DATA, "SPI_PRJ_DATA", 0.0, {0}},
	{13, SPI_APP_DATA, "SPI_APP_DATA", 0.0, {0}},
	{14, SPI_MICU_EVENT, "SPI_MICU_EVENT", 0.0, {0}},
	{15, SPI_TRANS_INJECT, "SPI_TRANS_INJECT", 0.0, {0}},
	{16, SPI_INJ_FILE_CG, "SPI_INJ_FILE_CG", 0.0, {0}},
	{17, SPI_INJ_FILE_JH1, "SPI_INJ_FILE_JH1", 0.0, {0}},
};

#define SPI_BUF_SIZE   1024

typedef enum
{
	YAOCE2 = 0,
	YAOCE3,
	YAOCE4,
	OTHER,
	ALARM_PARAM,
	INJECT,
	CHAXUN_CMD,
	DOC_CMD,
	CG,
	DATA_CG,
	JH1,
	STORE,
	MAX_ID
}FILE_ID;

#endif

#ifndef __FT2000A_WDT_H__
#define __FT2000A_WDT_H__

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t WDT_EN      :1;  //WDT ʹ��
		uint8_t RMOD        :1;  //��Ӧģʽ
		uint8_t RPL         :3;  //�������峤��
		uint8_t APB         :1;  //�߳��Ķ�дλ
	};
}wdt_cr_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t TOP         :4;  //��ʱʱ��
		uint8_t TOP_INIT    :4;  //��ʼ���ĳ�ʱʱ��
	};
}wdt_torr_t;

struct wdt_registers
{
	wdt_cr_t wdt_cr;					//���ƼĴ���
	wdt_torr_t wdt_torr;					//��ʱ��Χ�Ĵ���
	uint32_t wdt_ccvr;					//��ǰ����ֵ�Ĵ���
	uint32_t wdt_crr;					//�����������Ĵ���
	uint32_t wdt_stat;					//�ж�״̬�Ĵ���
	uint32_t wdt_eoi;					//�ж�����Ĵ���
	uint32_t wdt_comp_params[5];		//��ʼֵ����ʼֵȡ�����û�����
	uint32_t wdt_comp_version;			//DesignWare ����汾�Ĵ���
	uint32_t wdt_comp_type;				//DesignWare ������ͼĴ���
};

struct ft2000a_wdt_device_t
{
	conf_object_t* obj;
	struct wdt_registers regs;
	conf_object_t* signal;
	general_signal_intf* signal_iface;
	int int_number;
	time_handle_t *handle;
	int wdt_int_cnt;
};

typedef struct ft2000a_wdt_device_t ft2000a_wdt_device;

static char* regs_name[] =
{
	"wdt_cr",
	"wdt_torr",
	"wdt_ccvr",
	"wdt_crr",
	"wdt_stat",
	"wdt_eoi",
	"wdt_comp_params_5",
	"wdt_comp_params_4",
	"wdt_comp_params_3",
	"wdt_comp_params_2",
	"wdt_comp_params_1",
	"wdt_comp_version",
	"wdt_comp_type",
	NULL
};

#define WDT_CR             0x00
#define WDT_TORR           0x04
#define WDT_CCVR           0x08
#define WDT_CRR            0x0c
#define WDT_STAT           0x10
#define WDT_EOI            0x14
#define WDT_COMP_PARAMS_5  0xe4
#define WDT_COMP_PARAMS_4  0xe8
#define WDT_COMP_PARAMS_3  0xec
#define WDT_COMP_PARAMS_2  0xf0
#define WDT_COMP_PARAMS_1  0xf4
#define WDT_COMP_VERSION   0xf8
#define WDT_COMP_TYPE      0xfc

static uint32_t regs_offset[] =
{
	WDT_CR,
	WDT_TORR,
	WDT_CCVR,
	WDT_CRR,
	WDT_STAT,
	WDT_EOI,
	WDT_COMP_PARAMS_5,
	WDT_COMP_PARAMS_4,
	WDT_COMP_PARAMS_3,
	WDT_COMP_PARAMS_2,
	WDT_COMP_PARAMS_1,
	WDT_COMP_VERSION,
	WDT_COMP_TYPE,
};

#define  DEFAULT_CLK  125000000
#endif

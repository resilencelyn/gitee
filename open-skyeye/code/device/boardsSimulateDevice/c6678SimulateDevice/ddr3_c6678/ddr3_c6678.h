#ifndef __DDR3_C6678_H__
#define __DDR3_C6678_H__
struct registers
{
	uint32_t midr;
	uint32_t status;
	uint32_t sdcfg;
	uint32_t sdrfc;
	uint32_t sdtim1;
	uint32_t sdtim2;
	uint32_t sdtim3;
	uint32_t pmctl;
	uint32_t lat_config;
	uint32_t perf_cnt_1;
	uint32_t perf_cnt_2;
	uint32_t perf_cnt_cfg;
	uint32_t perf_cnt_sel;
	uint32_t perf_cnt_tim;
	uint32_t irqstatus_raw_sys;
	uint32_t irq_status_sys;
	uint32_t irqenable_set_sys;
	uint32_t irqenable_clr_sys;
	uint32_t zqconfig;
	uint32_t rdwr_lvl_rmp_win;
	uint32_t rdwr_lvl_rmp_ctrl;
	uint32_t rdwr_lvl_ctrl;
	uint32_t ddr_phy_ctrl_1;
	uint32_t pri_cos_map;
	uint32_t mstid_cos_1_map;
	uint32_t mstid_cos_2_map;
	uint32_t eccctl;
	uint32_t eccaddr1;
	uint32_t eccaddr2;
	uint32_t rwthresh;
	uint32_t ddr3_config_0;
	uint32_t ddr3_config_1;
	uint32_t ddr3_config_2;
	uint32_t ddr3_config_3;
	uint32_t ddr3_config_4;
	uint32_t ddr3_config_5;
	uint32_t ddr3_config_6;
	uint32_t ddr3_config_7;
	uint32_t ddr3_config_8;
	uint32_t ddr3_config_9;
	uint32_t ddr3_config_10;
	uint32_t ddr3_config_12;
	uint32_t ddr3_config_14;
	uint32_t ddr3_config_15;
	uint32_t ddr3_config_16;
	uint32_t ddr3_config_17;
	uint32_t ddr3_config_18;
	uint32_t ddr3_config_19;
	uint32_t ddr3_config_20;
	uint32_t ddr3_config_21;
	uint32_t ddr3_config_22;
	uint32_t ddr3_config_23;
	uint32_t ddr3_config_24;
	uint32_t ddr3_config_reg_25;
	uint32_t ddr3_config_reg_26;
	uint32_t ddr3_config_reg_27;
	uint32_t ddr3_config_reg_28;
	uint32_t ddr3_config_reg_29;
	uint32_t ddr3_config_reg_30;
	uint32_t ddr3_config_reg_31;
	uint32_t ddr3_config_reg_32;
	uint32_t ddr3_config_reg_33;
	uint32_t ddr3_config_reg_34;
	uint32_t ddr3_config_reg_35;
	uint32_t ddr3_config_reg_36;
	uint32_t ddr3_config_reg_37;
	uint32_t ddr3_config_reg_38;
	uint32_t ddr3_config_reg_39;
	uint32_t ddr3_config_reg_40;
	uint32_t ddr3_config_reg_41;
	uint32_t ddr3_config_reg_42;
	uint32_t ddr3_config_reg_43;
	uint32_t ddr3_config_reg_44;
	uint32_t ddr3_config_reg_45;
	uint32_t ddr3_config_reg_46;
	uint32_t ddr3_config_reg_47;
	uint32_t ddr3_config_reg_48;
	uint32_t ddr3_config_reg_49;
	uint32_t ddr3_config_reg_50;
	uint32_t ddr3_config_reg_51;
	uint32_t ddr3_config_reg_52;
	uint32_t ddr3_config_reg_53;
	uint32_t ddr3_config_reg_54;
	uint32_t ddr3_config_reg_55;
	uint32_t ddr3_config_reg_56;
	uint32_t ddr3_config_reg_57;
	uint32_t ddr3_config_reg_58;
	uint32_t ddr3_config_reg_59;
	uint32_t ddr3_config_reg_60;
};


struct c6678_device
{
	conf_object_t* obj;

	core_signal_intf* signal_iface;
	conf_object_t* signal;

	struct registers regs;
};

typedef struct c6678_device ddr3_c6678_device;

static char* regs_name[] =
{
	"midr",
	"status",
	"sdcfg",
	"sdrfc",
	"sdtim1",
	"sdtim2",
	"sdtim3",
	"pmctl",
	"lat_config",
	"perf_cnt_1",
	"perf_cnt_2",
	"perf_cnt_cfg",
	"perf_cnt_sel",
	"perf_cnt_tim",
	"irqstatus_raw_sys",
	"irq_status_sys",
	"irqenable_set_sys",
	"irqenable_clr_sys",
	"zqconfig",
	"rdwr_lvl_rmp_win",
	"rdwr_lvl_rmp_ctrl",
	"rdwr_lvl_ctrl",
	"ddr_phy_ctrl_1",
	"pri_cos_map",
	"mstid_cos_1_map",
	"mstid_cos_2_map",
	"eccctl",
	"eccaddr1",
	"eccaddr2",
	"rwthresh",
	"ddr3_config_0",
	"ddr3_config_1",
	"ddr3_config_2",
	"ddr3_config_3",
	"ddr3_config_4",
	"ddr3_config_5",
	"ddr3_config_6",
	"ddr3_config_7",
	"ddr3_config_8",
	"ddr3_config_9",
	"ddr3_config_10",
	"ddr3_config_12",
	"ddr3_config_14",
	"ddr3_config_15",
	"ddr3_config_16",
	"ddr3_config_17",
	"ddr3_config_18",
	"ddr3_config_19",
	"ddr3_config_20",
	"ddr3_config_21",
	"ddr3_config_22",
	"ddr3_config_23",
	"ddr3_config_24",
	"ddr3_config_reg_25",
	"ddr3_config_reg_26",
	"ddr3_config_reg_27",
	"ddr3_config_reg_28",
	"ddr3_config_reg_29",
	"ddr3_config_reg_30",
	"ddr3_config_reg_31",
	"ddr3_config_reg_32",
	"ddr3_config_reg_33",
	"ddr3_config_reg_34",
	"ddr3_config_reg_35",
	"ddr3_config_reg_36",
	"ddr3_config_reg_37",
	"ddr3_config_reg_38",
	"ddr3_config_reg_39",
	"ddr3_config_reg_40",
	"ddr3_config_reg_41",
	"ddr3_config_reg_42",
	"ddr3_config_reg_43",
	"ddr3_config_reg_44",
	"ddr3_config_reg_45",
	"ddr3_config_reg_46",
	"ddr3_config_reg_47",
	"ddr3_config_reg_48",
	"ddr3_config_reg_49",
	"ddr3_config_reg_50",
	"ddr3_config_reg_51",
	"ddr3_config_reg_52",
	"ddr3_config_reg_53",
	"ddr3_config_reg_54",
	"ddr3_config_reg_55",
	"ddr3_config_reg_56",
	"ddr3_config_reg_57",
	"ddr3_config_reg_58",
	"ddr3_config_reg_59",
	"ddr3_config_reg_60",
	NULL
};

#define MIDR					0x000
#define STATUS					0x004
#define SDCFG					0x008
#define SDRFC					0x010
#define SDTIM1					0x018
#define SDTIM2					0x020
#define SDTIM3					0x028
#define PMCTL					0x038
#define LAT_CONFIG				0x054
#define PERF_CNT_1				0x080
#define PERF_CNT_2				0x084
#define PERF_CNT_CFG			0x088
#define PERF_CNT_SEL			0x08C
#define PERF_CNT_TIM			0x090
#define IRQSTATUS_RAW_SYS		0x0A4
#define IRQ_STATUS_SYS			0x0AC
#define IRQENABLE_SET_SYS		0x0B4
#define IRQENABLE_CLR_SYS		0x0BC
#define ZQCONFIG				0x0C8
#define RDWR_LVL_RMP_WIN		0x0D4
#define RDWR_LVL_RMP_CTRL		0x0D8
#define RDWR_LVL_CTRL			0x0DC
#define DDR_PHY_CTRL_1			0x0E4
#define PRI_COS_MAP				0x100
#define MSTID_COS_1_MAP			0x104
#define MSTID_COS_2_MAP			0x108
#define ECCCTL					0x110
#define ECCADDR1				0x114
#define ECCADDR2				0x118
#define RWTHRESH				0x120
#define DDR3_CONFIG_0			0x404
#define DDR3_CONFIG_1			0x408
#define DDR3_CONFIG_2			0x40C
#define DDR3_CONFIG_3			0x410
#define DDR3_CONFIG_4			0x414
#define DDR3_CONFIG_5			0x418
#define DDR3_CONFIG_6			0x41C
#define DDR3_CONFIG_7			0x420
#define DDR3_CONFIG_8			0x424
#define DDR3_CONFIG_9			0x428
#define DDR3_CONFIG_10			0x42C
#define DDR3_CONFIG_12			0x434
#define DDR3_CONFIG_14			0x43C
#define DDR3_CONFIG_15			0x440
#define DDR3_CONFIG_16			0x444
#define DDR3_CONFIG_17			0x448
#define DDR3_CONFIG_18			0x44C
#define DDR3_CONFIG_19			0x450
#define DDR3_CONFIG_20			0x454
#define DDR3_CONFIG_21			0x458
#define DDR3_CONFIG_22			0x45C
#define DDR3_CONFIG_23			0x460
#define DDR3_CONFIG_24			0x464
#define DDR3_CONFIG_REG_25		0x468
#define DDR3_CONFIG_REG_26		0x46C
#define DDR3_CONFIG_REG_27		0x470
#define DDR3_CONFIG_REG_28		0x474
#define DDR3_CONFIG_REG_29		0x478
#define DDR3_CONFIG_REG_30		0x47C
#define DDR3_CONFIG_REG_31		0x480
#define DDR3_CONFIG_REG_32		0x484
#define DDR3_CONFIG_REG_33		0x488
#define DDR3_CONFIG_REG_34		0x48C
#define DDR3_CONFIG_REG_35		0x490
#define DDR3_CONFIG_REG_36		0x494
#define DDR3_CONFIG_REG_37		0x498
#define DDR3_CONFIG_REG_38		0x49C
#define DDR3_CONFIG_REG_39		0x4A0
#define DDR3_CONFIG_REG_40		0x4A4
#define DDR3_CONFIG_REG_41		0x4A8
#define DDR3_CONFIG_REG_42		0x4AC
#define DDR3_CONFIG_REG_43		0x4B0
#define DDR3_CONFIG_REG_44		0x4B4
#define DDR3_CONFIG_REG_45		0x4B8
#define DDR3_CONFIG_REG_46		0x4BC
#define DDR3_CONFIG_REG_47		0x4C0
#define DDR3_CONFIG_REG_48		0x4C4
#define DDR3_CONFIG_REG_49		0x4C8
#define DDR3_CONFIG_REG_50		0x4CC
#define DDR3_CONFIG_REG_51		0x4D0
#define DDR3_CONFIG_REG_52		0x4D4
#define DDR3_CONFIG_REG_53		0x4D8
#define DDR3_CONFIG_REG_54		0x4DC
#define DDR3_CONFIG_REG_55		0x4E0
#define DDR3_CONFIG_REG_56		0x4E4
#define DDR3_CONFIG_REG_57		0x4E8
#define DDR3_CONFIG_REG_58		0x4EC
#define DDR3_CONFIG_REG_59		0x4F0
#define DDR3_CONFIG_REG_60		0x4F4
#endif

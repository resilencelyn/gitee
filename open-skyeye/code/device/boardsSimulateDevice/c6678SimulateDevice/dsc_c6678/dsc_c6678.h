#ifndef __DSC_C6678_H__
#define __DSC_C6678_H__

struct registers
{
	uint32_t jtagid;			// JTAG ID Register Value

	uint32_t devstat;

	uint32_t kick0;				// Kicker Mechanism Registers
	uint32_t kick1;

	uint32_t dsp_boot_addr0;
	uint32_t dsp_boot_addr1;
	uint32_t dsp_boot_addr2;
	uint32_t dsp_boot_addr3;
	uint32_t dsp_boot_addr4;
	uint32_t dsp_boot_addr5;
	uint32_t dsp_boot_addr6;
	uint32_t dsp_boot_addr7;

	uint32_t macid1;
	uint32_t macid2;

	uint32_t lrstnmipinstat_clr;

	uint32_t reset_stat_clr;

	uint32_t bootcomplete;

	uint32_t reset_stat;

	uint32_t lrstnmipinstat;

	uint32_t devcfg;

	uint32_t pwrstatectl;

	uint32_t srio_serdes_sts;

	uint32_t smgii_serdes_sts;

	uint32_t pcie_serdes_sts;

	uint32_t hyperlink_serdes_sts;

	uint32_t nmigr[8];

	uint32_t ipcgr[8];
	uint32_t ipcgrh;
	uint32_t ipcar[8];
	uint32_t ipcarh;

	uint32_t tinpsel;

	uint32_t toutpsel;

	uint32_t rstmux[8];

	union {
		uint32_t all;
		struct {
			uint32_t PLLD		:  5;
			uint32_t Reserved_2 :  6;
			uint32_t PLLM_06	:  1;
			uint32_t PLLM_07	:  1;
			uint32_t PLLM_08	:  1;
			uint32_t PLLM_09	:  1;
			uint32_t PLLM_10	:  1;
			uint32_t PLLM_11	:  1;
			uint32_t PLLM_12	:  1;
			uint32_t Reserved_1 :  5;
			uint32_t BWADJ_01	:  1;
			uint32_t BWADJ_02	:  1;
			uint32_t BWADJ_03	:  1;
			uint32_t BWADJ_04	:  1;
			uint32_t BWADJ_05	:  1;
			uint32_t BWADJ_06	:  1;
			uint32_t BWADJ_07	:  1;
		};
	} mainpllctl0;
	union {
		uint32_t all;
		struct {
			uint32_t BWADJ_08	:  1;
			uint32_t BWADJ_09	:  1;
			uint32_t BWADJ_10	:  1;
			uint32_t BWADJ_11	:  1;
			uint32_t Reserved_2 :  2;
			uint32_t ENSAT		:  1;
			uint32_t Reserved_1 : 24;
		};
	} mainpllctl1;

	uint32_t ddr3pllctl0;
	uint32_t ddr3pllctl1;

	uint32_t passpllctl0;
	uint32_t passpllctl1;

	uint32_t sgmii_serdes_cfgpll;
	uint32_t sgmii_serdes_cfgrx0;
	uint32_t sgmii_serdes_cfgtx0;
	uint32_t sgmii_serdes_cfgrx1;
	uint32_t sgmii_serdes_cfgtx1;

	uint32_t pcie_serdes_cfgpll;
	uint32_t srio_serdes_cfgpll;

	uint32_t srio_serdes_cfgrx0;
	uint32_t srio_serdes_cfgtx0;
	uint32_t srio_serdes_cfgrx1;
	uint32_t srio_serdes_cfgtx1;
	uint32_t srio_serdes_cfgrx2;
	uint32_t srio_serdes_cfgtx2;
	uint32_t srio_serdes_cfgrx3;
	uint32_t srio_serdes_cfgtx3;

	uint32_t dsp_susp_ctl;

	uint32_t hyperlink_serdes_cfgpll;
	uint32_t hyperlink_serdes_cfgrx0;
	uint32_t hyperlink_serdes_cfgtx0;
	uint32_t hyperlink_serdes_cfgrx1;
	uint32_t hyperlink_serdes_cfgtx1;
	uint32_t hyperlink_serdes_cfgrx2;
	uint32_t hyperlink_serdes_cfgtx2;
	uint32_t hyperlink_serdes_cfgrx3;
	uint32_t hyperlink_serdes_cfgtx3;

	uint32_t devspeed;

	uint32_t chip_misc_ctl;
};


struct c6678_device
{
	conf_object_t* obj;

	core_signal_intf* signal_iface;
	conf_object_t* signal;

	general_signal_intf* general_signal;
	conf_object_t* core;

	int interrupt_number;

	/*
	 * Kicker Mechanism Registers
	 * 1. Unlock
	 * KICK0 = 0x83E70B13 and KICK1 = 0x95A4F1E0;
	 * 2. lock
	 * KICK0 与 KICK1 写入任意值
	 * */
	uint32_t BootCfgKicker_Status;
	#define BOOTCFGKICKER_UNLOCK 1
	#define BOOTCFGKICKER_LOCK   0
	#define KICK0_KEY            0x83E70B13
	#define KICK1_KEY            0x95A4F1E0

	struct registers regs;
};

typedef struct c6678_device dsc_c6678_device;

static char* regs_name[] =
{
	"jtagid",
	"devstat",
	"kick0",
	"kick1",
	"dsp_boot_addr0",
	"dsp_boot_addr1",
	"dsp_boot_addr2",
	"dsp_boot_addr3",
	"dsp_boot_addr4",
	"dsp_boot_addr5",
	"dsp_boot_addr6",
	"dsp_boot_addr7",
	"macid1",
	"macid2",
	"lrstnmipinstat_clr",
	"reset_stat_clr",
	"bootcomplete",
	"reset_stat",
	"lrstnmipinstat",
	"devcfg",
	"pwrstatectl",
	"srio_serdes_sts",
	"smgii_serdes_sts",
	"pcie_serdes_sts",
	"hyperlink_serdes_sts",
	"nmigr0",
	"nmigr1",
	"nmigr2",
	"nmigr3",
	"nmigr4",
	"nmigr5",
	"nmigr6",
	"nmigr7",
	"ipcgr0",
	"ipcgr1",
	"ipcgr2",
	"ipcgr3",
	"ipcgr4",
	"ipcgr5",
	"ipcgr6",
	"ipcgr7",
	"ipcgrh",
	"ipcar0",
	"ipcar1",
	"ipcar2",
	"ipcar3",
	"ipcar4",
	"ipcar5",
	"ipcar6",
	"ipcar7",
	"ipcarh",
	"tinpsel",
	"toutpsel",
	"rstmux0",
	"rstmux1",
	"rstmux2",
	"rstmux3",
	"rstmux4",
	"rstmux5",
	"rstmux6",
	"rstmux7",
	"mainpllctl0",
	"mainpllctl1",
	"ddr3pllctl0",
	"ddr3pllctl1",
	"passpllctl0",
	"passpllctl1",
	"sgmii_serdes_cfgpll",
	"sgmii_serdes_cfgrx0",
	"sgmii_serdes_cfgtx0",
	"sgmii_serdes_cfgrx1",
	"sgmii_serdes_cfgtx1",
	"pcie_serdes_cfgpll",
	"srio_serdes_cfgpll",
	"srio_serdes_cfgrx0",
	"srio_serdes_cfgtx0",
	"srio_serdes_cfgrx1",
	"srio_serdes_cfgtx1",
	"srio_serdes_cfgrx2",
	"srio_serdes_cfgtx2",
	"srio_serdes_cfgrx3",
	"srio_serdes_cfgtx3",
	"dsp_susp_ctl",
	"hyperlink_serdes_cfgpll",
	"hyperlink_serdes_cfgrx0",
	"hyperlink_serdes_cfgtx0",
	"hyperlink_serdes_cfgrx1",
	"hyperlink_serdes_cfgtx1",
	"hyperlink_serdes_cfgrx2",
	"hyperlink_serdes_cfgtx2",
	"hyperlink_serdes_cfgrx3",
	"hyperlink_serdes_cfgtx3",
	"devspeed",
	"chip_misc_ctl",
	NULL
};

#define JTAGID					0x018
#define DEVSTAT					0x020
#define KICK0					0x038
#define KICK1					0x03c
#define DSP_BOOT_ADDR0			0x040
#define DSP_BOOT_ADDR1			0x044
#define DSP_BOOT_ADDR2			0x048
#define DSP_BOOT_ADDR3			0x04c
#define DSP_BOOT_ADDR4			0x050
#define DSP_BOOT_ADDR5			0x054
#define DSP_BOOT_ADDR6			0x058
#define DSP_BOOT_ADDR7			0x05c
#define MACID1					0x110
#define MACID2					0x114
#define LRSTNMIPINSTAT_CLR		0x130
#define RESET_STAT_CLR			0x134
#define BOOTCOMPLETE			0x13c
#define RESET_STAT				0x144
#define LRSTNMIPINSTAT			0x148
#define DEVCFG					0x14c
#define PWRSTATECTL				0x150
#define SRIO_SERDES_STS			0x154
#define SMGII_SERDES_STS		0x158
#define PCIE_SERDES_STS			0x15c
#define HYPERLINK_SERDES_STS	0x160

#define NMIGR0					0x200
#define NMIGR1					0x204
#define NMIGR2					0x208
#define NMIGR3					0x20c
#define NMIGR4					0x210
#define NMIGR5					0x214
#define NMIGR6					0x218
#define NMIGR7					0x21c

#define IPCGR0					0x240
#define IPCGR1					0x244
#define IPCGR2					0x248
#define IPCGR3					0x24c
#define IPCGR4					0x250
#define IPCGR5					0x254
#define IPCGR6					0x258
#define IPCGR7					0x25c

#define IPCGRH					0x27c

#define IPCAR0					0x280
#define IPCAR1					0x284
#define IPCAR2					0x288
#define IPCAR3					0x28c
#define IPCAR4					0x290
#define IPCAR5					0x294
#define IPCAR6					0x298
#define IPCAR7					0x29c

#define IPCARH					0x2bc

#define TINPSEL					0x300
#define TOUTPSEL				0x304
#define RSTMUX0					0x308
#define RSTMUX1					0x30c
#define RSTMUX2					0x310
#define RSTMUX3					0x314
#define RSTMUX4					0x318
#define RSTMUX5					0x31c
#define RSTMUX6					0x320
#define RSTMUX7					0x324

#define MAINPLLCTL0				0x328
#define MAINPLLCTL1				0x32c

#define DDR3PLLCTL0				0x330
#define DDR3PLLCTL1				0x334

#define PASSPLLCTL0				0x338
#define PASSPLLCTL1				0x33c

#define SGMII_SERDES_CFGPLL		0x340
#define SGMII_SERDES_CFGRX0		0x344
#define SGMII_SERDES_CFGTX0		0x348
#define SGMII_SERDES_CFGRX1		0x34c
#define SGMII_SERDES_CFGTX1		0x350

#define PCIE_SERDES_CFGPLL		0x358

#define SRIO_SERDES_CFGPLL		0x360
#define SRIO_SERDES_CFGRX0		0x364
#define SRIO_SERDES_CFGTX0		0x368
#define SRIO_SERDES_CFGRX1		0x36c
#define SRIO_SERDES_CFGTX1		0x370
#define SRIO_SERDES_CFGRX2		0x374
#define SRIO_SERDES_CFGTX2		0x378
#define SRIO_SERDES_CFGRX3		0x37c
#define SRIO_SERDES_CFGTX3		0x380

#define DSP_SUSP_CTL			0x38c

#define HYPERLINK_SERDES_CFGPLL	0x3b4
#define HYPERLINK_SERDES_CFGRX0	0x3b8
#define HYPERLINK_SERDES_CFGTX0	0x3bc
#define HYPERLINK_SERDES_CFGRX1	0x3c0
#define HYPERLINK_SERDES_CFGTX1	0x3c4
#define HYPERLINK_SERDES_CFGRX2	0x3c8
#define HYPERLINK_SERDES_CFGTX2	0x3cc
#define HYPERLINK_SERDES_CFGRX3	0x3d0
#define HYPERLINK_SERDES_CFGTX3	0x3d4
#define DEVSPEED				0x3f8
#define CHIP_MISC_CTL			0x400

#endif

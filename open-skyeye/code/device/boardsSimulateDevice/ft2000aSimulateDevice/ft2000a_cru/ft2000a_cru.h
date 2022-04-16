#ifndef __FT2000A_CRU_H__
#define __FT2000A_CRU_H__

struct cru_registers
{
	uint32_t reg_set_soft_warmrst;		//[WO]设置软件热复位寄存器
	uint32_t reg_fer_mask;				//[WR]fatal error屏蔽寄存器
	uint32_t reg_softrst_local;			//[WR]软件局部复位寄存器
	uint32_t reg_set_softrst_local;		//[WO]设置软件局部复位寄存器
	uint32_t reg_rst_source;			//[RO]标识上一次复位原因寄存器
	uint32_t reg_softrst_core_source;	//[RO]标识触发上一次软件复位的内核
	uint32_t reg_lpr_core_source;		//[RO]标识触发上一次低功耗复位的内核
	uint32_t reg_lpr_module_source;		//[RO]标识触发上一次低功耗复位的设备模块
	uint32_t reg_softrst_local_source;	//[RO]标识触发上一次软件复位的设备模块
	uint32_t reg_wdt_rst_source;		//[RO]标识触发上一次WDT复位的设备模块
	uint32_t reg_core_rst_source;		//[RO]标识core0和core1上一次复位的原因
	uint32_t reg_hard_warmrst_cnt;		//[RO]系统上电后硬件热复位计数器
	uint32_t reg_soft_warmrst_cnt;		//[RO]系统上电后软件热复位计数器
	uint32_t reg_softrst_core_cnt;		//[RO]系统上电后软件内核复位计数器
	uint32_t reg_lpr_cnt;				//[RO]系统上电后低功耗复位计数器
	uint32_t reg_fer_cnt;				//[RO]系统上电后fatal error复位计数器
	uint32_t reg_softrst_local_cnt;		//[RO]系统上电后软件局部复位计数器
	uint32_t reg_wdt_cnt;				//[RO]系统上电后WDT复位计数器
};

struct ft2000a_cru_device_t
{
	conf_object_t* obj;
	struct cru_registers regs;
};

typedef struct ft2000a_cru_device_t ft2000a_cru_device;

static char* regs_name[] =
{
	"reg_set_soft_warmrst",
	"reg_fer_mask",
	"reg_softrst_local",
	"reg_set_softrst_local",
	"reg_rst_source",
	"reg_softrst_core_source",
	"reg_lpr_core_source",
	"reg_lpr_module_source",
	"reg_softrst_local_source",
	"reg_wdt_rst_source",
	"reg_core_rst_source",
	"reg_hard_warmrst_cnt",
	"reg_soft_warmrst_cnt",
	"reg_softrst_core_cnt",
	"reg_lpr_cnt",
	"reg_fer_cnt",
	"reg_softrst_local_cnt",
	"reg_wdt_cnt",
	NULL
};

#define REG_SET_SOFT_WARMRST		0x200
#define REG_FER_MASK				0x204
#define REG_SOFTRST_LOCAL			0x208
#define REG_SET_SOFTRST_LOCAL		0x20c
#define REG_RST_SOURCE				0x300
#define REG_SOFTRST_CORE_SOURCE		0x304
#define REG_LPR_CORE_SOURCE			0x308
#define REG_LPR_MODULE_SOURCE		0x30c
#define REG_SOFTRST_LOCAL_SOURCE	0x310
#define REG_WDT_RST_SOURCE			0x314
#define REG_CORE_RST_SOURCE			0x318
#define REG_HARD_WARMRST_CNT		0x400
#define REG_SOFT_WARMRST_CNT		0x404
#define REG_SOFTRST_CORE_CNT		0x408
#define REG_LPR_CNT					0x40c
#define REG_FER_CNT					0x410
#define REG_SOFTRST_LOCAL_CNT		0x414
#define REG_WDT_RST_CNT				0x418

static uint32_t regs_offset[] = {
	REG_SET_SOFT_WARMRST,
	REG_FER_MASK,
	REG_SOFTRST_LOCAL,
	REG_SET_SOFTRST_LOCAL,
	REG_RST_SOURCE,
	REG_SOFTRST_CORE_SOURCE,
	REG_LPR_CORE_SOURCE,
	REG_LPR_MODULE_SOURCE,
	REG_SOFTRST_LOCAL_SOURCE,
	REG_WDT_RST_SOURCE,
	REG_CORE_RST_SOURCE,
	REG_HARD_WARMRST_CNT,
	REG_SOFT_WARMRST_CNT,
	REG_SOFTRST_CORE_CNT,
	REG_LPR_CNT,
	REG_FER_CNT,
	REG_SOFTRST_LOCAL_CNT,
	REG_WDT_RST_CNT
};

#endif

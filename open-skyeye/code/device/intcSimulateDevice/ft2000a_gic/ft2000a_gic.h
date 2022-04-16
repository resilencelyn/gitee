#ifndef __FT2000A_GIC_H__
#define __FT2000A_GIC_H__

#define MAX_CPU_NUM		8

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t EnableGrp0		:1;
		uint32_t EnableGrp1		:1;
	}bit;
}gicd_ctlr_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t SGIINTID			:4;		//给目标内核的中断ID
		uint32_t rev				:11;
		uint32_t NSATT				:1;		//指定所需的安全级别 0— 仅当SGI被配置为group0时才将 SGIINTID发送到目标core1— 仅当SGI被配置为group1时才将SGIINTID发送到目标core
		uint32_t CPUTargetList		:8;		//目标core列表，每位代表一个core
		uint32_t TargetListFilter	:2;		//决定Distributor如何处理此SGI的分派。
											//00—按CPUTargetList分派此SGI中断
											//01—除了发出此SGI中断的Core外，向其他所有Core分派此SGI中断
											//02—仅向发出此SGI中断的Core分派此中断
	}bit;
}gicd_sgir_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t ITLinesNumber	:5;	//Indicates the maximum number of interrupts that the GIC supports.
		uint32_t CPUNumber		:3;	// Indicates the number of implemented CPU interfaces
		uint32_t rev				:2;
		uint32_t SecurityExtn	:1;	//Indicates whether the GIC implements the Security Extensions
		uint32_t LSPI			:5;	//the GIC implements the Security Extensions, the value of this field is the maximum number of implemented lockable SPIs, from 0 (0b00000) to 31 (0b11111),
	}bit;
}gicd_type_reg_t;

struct gicd_registers
{
	gicd_ctlr_reg_t ctlr[MAX_CPU_NUM];	//Distributor Control Register
	gicd_type_reg_t typer;	//[RO]Interrupt Controller Type Register
	uint32_t iidr;			//[RO]Distributor Implementer Identification Register
	uint32_t igroup[32];	//Interrupt Group Registers
	uint32_t isenable[32];	//Interrupt Set-Enable Registers
	uint32_t icenable[32];	//Interrupt Clear-Enable Registers
	uint32_t ispend[32];	//Interrupt Set-Pending Registers
	uint32_t icpend[32];	//Interrupt Clear-Pending Registers
	uint32_t isactive[32];	//GICv2 Interrupt Set-Active Registers
	uint32_t icactive[32];	//Interrupt Clear-Active Registers
	uint32_t ipriority[255];//Interrupt Priority Registers
	uint32_t itargets[255];	//[RO]Interrupt Processor Targets Registers
	uint32_t icfg[64];		//Interrupt Configuration Registers
	uint32_t nsac[64];		//Non-secure Access Control Registers, optional
	gicd_sgir_reg_t sgir;	//[WO]Software Generated Interrupt Register
	uint32_t cpendsgi[4];	//SGI Clear-Pending Registers
	uint32_t spendsgi[4];	//SGI Set-Pending Registers
};

typedef union
{
	uint32_t all;
	struct
	{
		uint8_t EnableGrp0		:1;			//Enable for the signaling of Group 0 interrupts by the CPU interface to the connected processor
		uint8_t EnableGrp1		:1;			//Enable for the signaling of Group 1 interrupts by the CPU interface to the connected processor
		uint8_t AckCtl			:1;			//When the highest priority pending interrupt is a Group 1 interrupt
		uint8_t FIQEn			:1;			//Controls whether the CPU interface signals Group 0 interrupts to a target processor using the FIQ or the IRQ signal
		uint8_t CBPR			:1;			//Controls whether the GICC_BPR provides common control to Group 0 and Group 1 interrupts.
		uint8_t FIQBypDisGrp0	:1;			//When the signaling of FIQs by the CPU interface is disabled, this bit partly controls whether the bypass FIQ signal is signaled to the processor
		uint8_t IRQBypDisGrp0	:1;			//When the signaling of IRQs by the CPU interface is disabled, this bit partly controls whether the bypass IRQ signal is signaled to the processor
		uint8_t FIQBypDisGrp1	:1;			//Alias of FIQBypDisGrp1 from the Non-secure copy of this register
		uint8_t IRQBypDisGrp1	:1;			//Alias of IRQBypDisGrp1 from the Non-secure copy of this register
		uint8_t EOImodeS		:1;			//Controls the behavior of accesses to GICC_EOIR and GICC_DIR registers
		uint8_t EOImodeNS		:1;			//Alias of EOImodeNS from the Non-secure copy of this register
	}bit;
}gicc_ctlr_reg_t;

struct gicc_registers
{
	gicc_ctlr_reg_t ctlr;	//CPU Interface Control Register
	uint32_t pmr;			//Interrupt Priority Mask Register
	uint32_t bpr;			//Binary Point Register
	uint32_t iar;			//Interrupt Acknowledge Register
	uint32_t eoir;			// End of Interrupt Register
	uint32_t rpr;			//Running Priority Register
	uint32_t hppir;			//Highest Priority Pending Interrupt Register
	uint32_t abpr;			//Aliased Binary Point Register
	uint32_t aiar;			//Aliased Interrupt Acknowledge Register
	uint32_t aeoir;			//Aliased End of Interrupt Register
	uint32_t ahppir;		//Aliased Highest Priority Pending Interrupt Register
	uint32_t apr[4];		//Active Priorities Registers
	uint32_t nsapr[4];		//Non-secure Active Priorities Registers
	uint32_t iidr;			//CPU Interface Identification Register
	uint32_t dir;			//Deactivate Interrupt Register
};

struct gic_registers
{
	struct gicd_registers gicd_regs;
	struct gicc_registers gicc_regs[MAX_CPU_NUM];
};

typedef struct {
	conf_object_t *signal;
	core_signal_intf *signal_iface;
}cpu_iface_t;

struct ft2000a_gic_device_t
{
	conf_object_t* obj;
	struct gic_registers regs;
	cpu_iface_t cpu_list[MAX_CPU_NUM];
	int cpuNum;
	int sgiIntCpu_list[16];
};

typedef struct ft2000a_gic_device_t ft2000a_gic_device;

static char* regs_name[] =
{
	"gicd_ctlr",
	"gicd_typer",
	"gicd_iidr",
	"igroup",
	"isenable",
	"icenable",
	"ispend",
	"icpend",
	"isactive",
	"icactive",
	"ipriority",
	"itargets",
	"icfg",
	"nsac",
	"sgir",
	"cpendsgi",
	"spendsgi",
	"gicc_ctlr",
	"pmr",
	"bpr",
	"iar",
	"eoir",
	"rpr",
	"hppir",
	"abpr",
	"aiar",
	"aeoir",
	"ahppir",
	"apr",
	"nsapr",
	"gicc_iidr",
	"dir",
	NULL
};

#define GICD_BASE_ADDR		0x1000
#define GICC_BASE_ADDR		0x2000
/* Register offsets for the ARM generic interrupt controller (GIC) */
/* Distributor Registers */
#define GICD_CTLR			(GICD_BASE_ADDR + 0x0000)
#define GICD_TYPER			(GICD_BASE_ADDR + 0x0004)
#define GICD_IIDR			(GICD_BASE_ADDR + 0x0008)
#define GICD_IGROUPR0		(GICD_BASE_ADDR + 0x0080)
#define GICD_IGROUPR31		(GICD_BASE_ADDR + 0x00fc)
#define GICD_ISENABLER0		(GICD_BASE_ADDR + 0x0100)
#define GICD_ISENABLER31	(GICD_BASE_ADDR + 0x017c)
#define GICD_ICENABLER0		(GICD_BASE_ADDR + 0x0180)
#define GICD_ICENABLER31	(GICD_BASE_ADDR + 0x01fc)
#define GICD_ISPENDR0		(GICD_BASE_ADDR + 0x0200)
#define GICD_ISPENDR31		(GICD_BASE_ADDR + 0x027c)
#define GICD_ICPENDR0		(GICD_BASE_ADDR + 0x0280)
#define GICD_ICPENDR31		(GICD_BASE_ADDR + 0x02fc)
#define GICD_ISACTIVER0		(GICD_BASE_ADDR + 0x0300)
#define GICD_ISACTIVER31	(GICD_BASE_ADDR + 0x037c)
#define GICD_ICACTIVER0		(GICD_BASE_ADDR + 0x0380)
#define GICD_ICACTIVER31	(GICD_BASE_ADDR + 0x03fc)
#define GICD_IPRIORITYR0	(GICD_BASE_ADDR + 0x0400)
#define GICD_IPRIORITYR254	(GICD_BASE_ADDR + 0x07f8)
#define GICD_ITARGETSR0		(GICD_BASE_ADDR + 0x0800)
#define GICD_ITARGETSR254	(GICD_BASE_ADDR + 0x0bf8)
#define GICD_ICFGR0			(GICD_BASE_ADDR + 0x0c00)
#define GICD_ICFGR63		(GICD_BASE_ADDR + 0x0cfc)
#define GICD_NSACR0			(GICD_BASE_ADDR + 0x0e00)
#define GICD_NSACR63		(GICD_BASE_ADDR + 0x0efc)
#define GICD_SGIR			(GICD_BASE_ADDR + 0x0f00)
#define GICD_CPENDSGIR0		(GICD_BASE_ADDR + 0x0f10)
#define GICD_CPENDSGIR3		(GICD_BASE_ADDR + 0x0f1c)
#define GICD_SPENDSGIR0		(GICD_BASE_ADDR + 0x0f20)
#define GICD_SPENDSGIR3		(GICD_BASE_ADDR + 0x0f2c)

/* Cpu Interface Memory Mapped Registers */
#define GICC_CTLR			(GICC_BASE_ADDR + 0x0000)
#define GICC_PMR			(GICC_BASE_ADDR + 0x0004)
#define GICC_BPR			(GICC_BASE_ADDR + 0x0008)
#define GICC_IAR			(GICC_BASE_ADDR + 0x000C)
#define GICC_EOIR			(GICC_BASE_ADDR + 0x0010)
#define GICC_RPR			(GICC_BASE_ADDR + 0x0014)
#define GICC_HPPIR			(GICC_BASE_ADDR + 0x0018)
#define GICC_ABPR			(GICC_BASE_ADDR + 0x001c)
#define GICC_AIAR			(GICC_BASE_ADDR + 0x0020)
#define GICC_AEOIR			(GICC_BASE_ADDR + 0x0024)
#define GICC_AHPPIR			(GICC_BASE_ADDR + 0x0028)
#define GICC_APR0			(GICC_BASE_ADDR + 0x00d0)
#define GICC_APR3			(GICC_BASE_ADDR + 0x00dc)
#define GICC_NSAPR0			(GICC_BASE_ADDR + 0x00e0)
#define GICC_NSAPR3			(GICC_BASE_ADDR + 0x00ec)
#define GICC_IIDR			(GICC_BASE_ADDR + 0x00fc)
#define GICC_DIR			(GICC_BASE_ADDR + 0x1000)

static uint32_t regs_offset[] = {
	GICD_CTLR,
	GICD_TYPER,
	GICD_IIDR,
	GICD_IGROUPR0,
	GICD_ISENABLER0,
	GICD_ICENABLER0,
	GICD_ISPENDR0,
	GICD_ICPENDR0,
	GICD_ISACTIVER0,
	GICD_ICACTIVER0,
	GICD_IPRIORITYR0,
	GICD_ITARGETSR0,
	GICD_ICFGR0,
	GICD_NSACR0,
	GICD_SGIR,
	GICD_CPENDSGIR0,
	GICD_SPENDSGIR0,
	GICC_CTLR,
	GICC_PMR,
	GICC_BPR,
	GICC_IAR,
	GICC_EOIR,
	GICC_RPR,
	GICC_HPPIR,
	GICC_ABPR,
	GICC_AIAR,
	GICC_AEOIR,
	GICC_AHPPIR,
	GICC_APR0,
	GICC_NSAPR0,
	GICC_IIDR,
	GICC_DIR,
};

#define MINIMUM_BINARY_POINT 0

static inline bool_t isEnabled(ft2000a_gic_device *dev, int intID)
{
	int grpNum = intID / 32;
	int grpIndex = intID % 32;
	return (dev->regs.gicd_regs.isenable[grpNum] >> grpIndex) & 0x1;
}

static inline bool_t isActive(ft2000a_gic_device *dev, int intID)
{
	int grpNum = intID / 32;
	int grpIndex = intID % 32;
	return (dev->regs.gicd_regs.isactive[grpNum] >> grpIndex) & 0x1;
}

static inline void setActive(ft2000a_gic_device *dev, int intID)
{
	int grpNum = intID / 32;
	int grpIndex = intID % 32;
	dev->regs.gicd_regs.isactive[grpNum] |= (1 << grpIndex);
}

static inline void clearActive(ft2000a_gic_device *dev, int intID)
{
	int grpNum = intID / 32;
	int grpIndex = intID % 32;
	dev->regs.gicd_regs.isactive[grpNum] &= ~(1 << grpIndex);
}

static inline bool_t isTargetCpu(ft2000a_gic_device *dev, int intID, int cpu_id)
{
	int grpNum = intID / 4;
	int grpIndex = intID % 4;
	return (dev->regs.gicd_regs.itargets[grpNum] >> cpu_id) & 0x1;
}

static inline uint32_t ReadGICD_IPRIORITYR(ft2000a_gic_device *dev, int intID)
{
	int grpNum = intID / 4;
	int grpIndex = intID % 4;
	return ((dev->regs.gicd_regs.ipriority[grpNum] >> (grpIndex * 8)) & 0xff);
}

static inline uint8_t GIC_PriorityMask(int n, int NS_mask)
{
	assert(n >= 0);
	assert(n <= 7);

	if (NS_mask == 1)
		n = n - 1;

	if (n < MINIMUM_BINARY_POINT)
		n = MINIMUM_BINARY_POINT;

	uint8_t mask = (0xff00 >> (7 - n)) & 0xff;

	return mask;
}

static inline bool_t isGrp0Int(ft2000a_gic_device *dev, int intID)
{
	int grpNum = intID / 32;
	int grpIndex = intID % 32;
	uint32_t grpValue = dev->regs.gicd_regs.igroup[grpNum];

	return ((grpValue & (1 << grpIndex)) == 0);
}

static inline bool_t anyActiveInterrupts(ft2000a_gic_device *dev)
{
	int max_support_grp = dev->regs.gicd_regs.typer.bit.ITLinesNumber + 1;
	int i;

	for (i = 0; i < max_support_grp; i++)
	{
		if (dev->regs.gicd_regs.isactive[i] != 0)
			return True;
	}

	return False;
}

static inline bool_t isPend(ft2000a_gic_device *dev, int intID)
{
	int grpNum = intID / 32;
	int grpIndex = intID % 32;
	return (dev->regs.gicd_regs.ispend[grpNum] >> grpIndex) & 0x1;
}

static inline bool_t isPending(ft2000a_gic_device* dev, int intID, int cpu_id)
{
	struct gic_registers *regs = &dev->regs;
	bool_t pending = False;

	//if (isTargetCpu(dev, intID, cpu_id) && isPend(dev, intID) && !isActive(dev, intID))
	if (isPend(dev, intID) && !isActive(dev, intID))
		pending = True;

	return pending;
}

static inline void setPend(ft2000a_gic_device *dev, int intID)
{
	int grpNum = intID / 32;
	int grpIndex = intID % 32;
	dev->regs.gicd_regs.ispend[grpNum] |= (1 << grpIndex);
}

static inline void clearPend(ft2000a_gic_device *dev, int intID)
{
	int grpNum = intID / 32;
	int grpIndex = intID % 32;
	dev->regs.gicd_regs.ispend[grpNum] &= ~(1 << grpIndex);
}

static inline uint32_t ReadGICD_NSACR(ft2000a_gic_device *dev, int intID)
{
	int grpNum = intID / 16;
	int grpIndex = intID % 16;
	uint32_t grpValue = (dev->regs.gicd_regs.nsac[grpNum] >> (2 * grpIndex)) & 0x3;

	return grpValue;
}

static inline uint32_t getSGISourceCpuID(uint8_t data)
{
	int i;

	for (i = 0 ; i < 8; i++)
	{
		if ((data >> i) & 0x1)
			break;
	}

	if (i < 8)
		return i;
	else
		return -1;
}

#define PriorityIsHigher(pr1, pr2)	(pr1 < pr2 ? True : False)

#define IGNORE_GROUP_ENABLE 1

exception_t gic_interface_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count, int cpu_id);
exception_t gic_interface_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count, int cpu_id);


int HighestPriorityPendingInterrupt(conf_object_t *conf_obj, int cpu_id);
#endif

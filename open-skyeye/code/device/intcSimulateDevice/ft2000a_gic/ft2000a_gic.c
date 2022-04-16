/*************************************************************************
  > File Name    : ft2000a_gic.c
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/11/13 14:50:41
  > This is ft-2000A gic(Generic Interrupt Controller) device
 ************************************************************************/
#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_attr.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#include <skyeye_system.h>
#include <skyeye_dev_intf.h>
#include <skyeye_log.h>
#include "ft2000a_gic.h"

static inline void SignalIRQ(conf_object_t *conf_obj, int cpu_id, int intID)
{
	ft2000a_gic_device* dev = (ft2000a_gic_device*)conf_obj->obj;
	interrupt_signal_t interrupt_signal;

	interrupt_signal.arm_signal.irq = Low_level;
	interrupt_signal.arm_signal.firq = Prev_level;
	interrupt_signal.arm_signal.reset = Prev_level;

	/*Interrupt Acknowledge*/
	dev->regs.gicc_regs[cpu_id].iar = (cpu_id << 10) | intID;

	/*Clear Pending state of the corresponding interrupt*/
	clearPend(dev, intID);

	/*Set the interrupt is active*/
	setActive(dev, intID);
	assert(cpu_id < dev->cpuNum);

	if (dev->cpu_list[cpu_id].signal_iface)
		dev->cpu_list[cpu_id].signal_iface->signal(dev->cpu_list[cpu_id].signal, &interrupt_signal);
}

static inline void SignalFIQ(conf_object_t *conf_obj, int cpu_id, int intID)
{
	ft2000a_gic_device* dev = (ft2000a_gic_device*)conf_obj->obj;
	interrupt_signal_t interrupt_signal;

	interrupt_signal.arm_signal.firq = Low_level;
	interrupt_signal.arm_signal.irq = Prev_level;
	interrupt_signal.arm_signal.reset = Prev_level;

	dev->regs.gicc_regs[cpu_id].iar = (cpu_id << 10) | intID;
	assert(cpu_id < dev->cpuNum);
	if (dev->cpu_list[cpu_id].signal_iface)
		dev->cpu_list[cpu_id].signal_iface->signal(dev->cpu_list[cpu_id].signal, &interrupt_signal);
}

int HighestPriorityPendingInterrupt(conf_object_t *conf_obj, int cpu_id)
{
	ft2000a_gic_device* dev = (ft2000a_gic_device*)conf_obj->obj;
	struct gic_registers *regs = &dev->regs;
	int group_enabled, intID;

	int hppi = 1023;	// Set initial ID to be no intterupt pending
	int max_support_interrupt_num = 32 * (regs->gicd_regs.typer.bit.ITLinesNumber + 1);	//how many interrupts are supported
	for (intID = 0; intID < max_support_interrupt_num; intID++)
	{
		group_enabled = (isGrp0Int(dev, intID) && regs->gicd_regs.ctlr[cpu_id].bit.EnableGrp0)
			|| (!isGrp0Int(dev, intID) && regs->gicd_regs.ctlr[cpu_id].bit.EnableGrp1);

		if (isPending(dev, intID, cpu_id) && isEnabled(dev, intID))
		{
			if (group_enabled || IGNORE_GROUP_ENABLE)
			{
				//if (PriorityIsHigher(ReadGICD_IPRIORITYR(dev, intID), ReadGICD_IPRIORITYR(dev, hppi)))
				hppi = intID;
			}
		}
	}

	return hppi;
}

static bool_t updateExceptionState(conf_object_t *conf_obj, int cpu_id, int *next_grp0, int *ackIntID)
{
	ft2000a_gic_device* dev = (ft2000a_gic_device*)conf_obj->obj;
	struct gic_registers *regs = &dev->regs;
	uint32_t sbp = regs->gicc_regs[cpu_id].bpr & 0x7;	//Secure binary point
	uint32_t nsbp = regs->gicc_regs[cpu_id].abpr & 0x7;	//non-Secure binary point
	bool_t next_int = False;
	*next_grp0 = False;

	int intID = HighestPriorityPendingInterrupt(conf_obj, cpu_id);

	if (PriorityIsHigher(ReadGICD_IPRIORITYR(dev, intID), regs->gicc_regs[cpu_id].pmr & 0xff)
			&& isPending(dev, intID, cpu_id))
	{
		uint8_t smsk, nsmsk;
		smsk = GIC_PriorityMask(sbp, 0);
		if (regs->gicc_regs[cpu_id].ctlr.bit.CBPR)
			nsmsk = smsk;
		else
		{
			nsmsk = GIC_PriorityMask(nsbp, 1);
		}

		if (isGrp0Int(dev, intID) && regs->gicd_regs.ctlr[cpu_id].bit.EnableGrp0)
		{
			if (!anyActiveInterrupts(dev) ||
					PriorityIsHigher(ReadGICD_IPRIORITYR(dev, intID), regs->gicc_regs[cpu_id].rpr & smsk))
			{
				next_int = True;
				*next_grp0 = True;
			}
		} else if (!isGrp0Int(dev, intID) && regs->gicd_regs.ctlr[cpu_id].bit.EnableGrp1)
		{
			if (!anyActiveInterrupts(dev) ||
					PriorityIsHigher(ReadGICD_IPRIORITYR(dev, intID), regs->gicc_regs[cpu_id].rpr & nsmsk))
			{
				next_int = True;
				*next_grp0 = False;
			}
		}
	}
	*ackIntID = intID;

	return next_int;
}

void gic_generateExceptions(conf_object_t *conf_obj)
{
	ft2000a_gic_device* dev = (ft2000a_gic_device*)conf_obj->obj;
	int cpu_count, cpu_id, next_grp0 = 0, intID = 0;
	bool_t next_int = False;
	bool_t cpu_fiq = False;
	bool_t cpu_irq = False;

	cpu_count = dev->regs.gicd_regs.typer.bit.CPUNumber;
	for (cpu_id = 0; cpu_id < cpu_count; cpu_id++)
	{
		next_int = updateExceptionState(conf_obj, cpu_id, &next_grp0, &intID);
		struct gicc_registers *gicc_regs = &dev->regs.gicc_regs[cpu_id];

		if (next_int)
		{
			if (next_grp0 && gicc_regs->ctlr.bit.FIQEn)
			{
				if (gicc_regs->ctlr.bit.EnableGrp0)
					cpu_fiq = True;
			} else
			{
				if ((next_grp0 && gicc_regs->ctlr.bit.EnableGrp0)
						|| (!next_grp0 && gicc_regs->ctlr.bit.EnableGrp1))
					cpu_irq = True;
			}
		}
		// Optional bypass logic
		if (gicc_regs->ctlr.bit.EnableGrp0 == 0
				|| gicc_regs->ctlr.bit.FIQEn == 0)
		{
			if (gicc_regs->ctlr.bit.FIQBypDisGrp0 == 0
					|| (gicc_regs->ctlr.bit.FIQBypDisGrp1 == 0 && gicc_regs->ctlr.bit.FIQEn == 0))
				cpu_fiq = False;	// Set FIQ to bypass
		}

		if (gicc_regs->ctlr.bit.EnableGrp1 == 0
				&& (gicc_regs->ctlr.bit.EnableGrp0 == 0 || gicc_regs->ctlr.bit.FIQEn == 1))
		{
			if (gicc_regs->ctlr.bit.FIQBypDisGrp1 == 0
					|| (gicc_regs->ctlr.bit.FIQBypDisGrp0 == 0 && gicc_regs->ctlr.bit.FIQEn == 1))
				cpu_irq = False;	// Set IRQ to bypass
		}

		if (cpu_irq)
		{
			SignalIRQ(conf_obj, cpu_id, intID);
		}

		if (cpu_fiq)
		{
			SignalFIQ(conf_obj, cpu_id, intID);
		}
	}
}

static void gicd_registers_write(conf_object_t *opaque, generic_address_t offset, uint32_t data)
{
	ft2000a_gic_device *dev = (ft2000a_gic_device *)opaque->obj;
	struct gic_registers *regs = &dev->regs;
	int n;

	//printf("gicd write offset = %x data = %x\n", offset, data);
	switch (offset)
	{
		case GICD_CTLR:
			regs->gicd_regs.ctlr[0].all = data;
			break;
		case GICD_IGROUPR0 ... GICD_IGROUPR31:
			n = (offset - GICD_IGROUPR0) / 4;
			regs->gicd_regs.igroup[n] = data;
			break;
		case GICD_ISENABLER0 ... GICD_ISENABLER31:
			n = (offset - GICD_ISENABLER0) / 4;
			regs->gicd_regs.isenable[n] |= data;
			break;
		case GICD_ICENABLER0 ... GICD_ICENABLER31:
			n = (offset - GICD_ICENABLER0) / 4;
			regs->gicd_regs.icenable[n] = data;
			regs->gicd_regs.isenable[n] &= ~data;
			break;
		case GICD_ISPENDR0 ... GICD_ISPENDR31:
			n = (offset - GICD_ISPENDR0) / 4;
			regs->gicd_regs.ispend[n] = data;
			break;
		case GICD_ICPENDR0 ... GICD_ICPENDR31:
			n = (offset - GICD_ICPENDR0) / 4;
			regs->gicd_regs.icpend[n] = data;
			regs->gicd_regs.ispend[n] &= ~data;
			break;
		case GICD_ISACTIVER0 ... GICD_ISACTIVER31:
			n = (offset - GICD_ISACTIVER0) / 4;
			regs->gicd_regs.isactive[n] = data;
			regs->gicd_regs.ispend[n] &= ~data;
			break;
		case GICD_ICACTIVER0 ... GICD_ICACTIVER31:
			n = (offset - GICD_ICACTIVER0) / 4;
			regs->gicd_regs.icactive[n] = data;
			break;
		case GICD_IPRIORITYR0 ... GICD_IPRIORITYR254:
			n = (offset - GICD_IPRIORITYR0) / 4;
			regs->gicd_regs.ipriority[n] = data;
			break;
		case GICD_ITARGETSR0 ... GICD_ITARGETSR254:
			n = (offset - GICD_ITARGETSR0) / 4;
			regs->gicd_regs.itargets[n] = data;
			break;
		case GICD_ICFGR0 ... GICD_ICFGR63:
			n = (offset - GICD_ICFGR0) / 4;
			regs->gicd_regs.icfg[n] = data;
			break;
		case GICD_NSACR0 ... GICD_NSACR63:
			n = (offset - GICD_NSACR0) / 4;
			regs->gicd_regs.nsac[n] = data;
			break;
		case GICD_SGIR:
			{
				regs->gicd_regs.sgir.all = data;
				/* SGI:interrupt ID 0 ~ 15 */
				int sgiIntID = regs->gicd_regs.sgir.bit.SGIINTID;
				regs->gicd_regs.ispend[0] |= (1 << sgiIntID);
			}
			break;
		case GICD_CPENDSGIR0 ... GICD_CPENDSGIR3:
			{
				n = (offset - GICD_CPENDSGIR0) / 4;
				regs->gicd_regs.cpendsgi[n] = data;
				int sgiIntID = regs->gicd_regs.sgir.bit.SGIINTID;
				regs->gicd_regs.ispend[0] &= ~(1 << sgiIntID);
			}
			break;
		case GICD_SPENDSGIR0 ... GICD_SPENDSGIR3:
			{
				n = (offset - GICD_SPENDSGIR0) / 4;
				regs->gicd_regs.spendsgi[n] = data;
				int sgiIntID = regs->gicd_regs.sgir.bit.SGIINTID;
				int grdIndex = sgiIntID % 4;
				dev->sgiIntCpu_list[sgiIntID] = getSGISourceCpuID((uint8_t)(data >> (8 * grdIndex)));
			}
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "In %s, Cannot write the register at 0x%x in ft2000a_gic\n", __func__, offset);
			break;
	}
	return;
}

static uint32_t gicd_registers_read(conf_object_t *opaque, generic_address_t offset)
{
	ft2000a_gic_device *dev = (ft2000a_gic_device *)opaque->obj;
	struct gic_registers *regs = &dev->regs;
	uint32_t data = 0, n = 0;

	switch (offset)
	{
		case GICD_CTLR:
			data = regs->gicd_regs.ctlr[0].all;
			break;
		case GICD_TYPER:
			data = regs->gicd_regs.typer.all;
			break;
		case GICD_IIDR:
			data = regs->gicd_regs.iidr;
			break;
		case GICD_IGROUPR0 ... GICD_IGROUPR31:
			n = (offset - GICD_IGROUPR0) / 4;
			data = regs->gicd_regs.igroup[n];
			break;
		case GICD_ISENABLER0 ... GICD_ISENABLER31:
			n = (offset - GICD_ISENABLER0) / 4;
			data = regs->gicd_regs.isenable[n];
			break;
		case GICD_ICENABLER0 ... GICD_ICENABLER31:
			n = (offset - GICD_ICENABLER0) / 4;
			data = regs->gicd_regs.icenable[n];
			break;
		case GICD_ISPENDR0 ... GICD_ISPENDR31:
			n = (offset - GICD_ISPENDR0) / 4;
			data = regs->gicd_regs.ispend[n];
			break;
		case GICD_ICPENDR0 ... GICD_ICPENDR31:
			n = (offset - GICD_ICPENDR0) / 4;
			data = regs->gicd_regs.icpend[n];
			break;
		case GICD_ISACTIVER0 ... GICD_ISACTIVER31:
			n = (offset - GICD_ISACTIVER0) / 4;
			data = regs->gicd_regs.isactive[n];
			break;
		case GICD_ICACTIVER0 ... GICD_ICACTIVER31:
			n = (offset - GICD_ICACTIVER0) / 4;
			data = regs->gicd_regs.icactive[n];
			break;
		case GICD_IPRIORITYR0 ... GICD_IPRIORITYR254:
			n = (offset - GICD_IPRIORITYR0) / 4;
			data = regs->gicd_regs.ipriority[n];
			break;
		case GICD_ITARGETSR0 ... GICD_ITARGETSR254:
			n = (offset - GICD_ITARGETSR0) / 4;
			data = regs->gicd_regs.itargets[n];
			break;
		case GICD_ICFGR0 ... GICD_ICFGR63:
			n = (offset - GICD_ICFGR0) / 4;
			data = regs->gicd_regs.icfg[n];
			break;
		case GICD_NSACR0 ... GICD_NSACR63:
			n = (offset - GICD_NSACR0) / 4;
			data = regs->gicd_regs.nsac[n];
			break;
		case GICD_CPENDSGIR0 ... GICD_CPENDSGIR3:
			n = (offset - GICD_CPENDSGIR0) / 4;
			data = regs->gicd_regs.cpendsgi[n];
			break;
		case GICD_SPENDSGIR0 ... GICD_SPENDSGIR3:
			n = (offset - GICD_SPENDSGIR0) / 4;
			data = regs->gicd_regs.spendsgi[n];
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "In %s, Cannot write the register at 0x%x in ft2000a_gic\n", __func__, offset);
			break;
	}
	//printf("gicd read offset = %x data = %x\n", offset, data);
	return data;
}

static void gicc_registers_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, int cpu_id)
{
	ft2000a_gic_device *dev = (ft2000a_gic_device *)opaque->obj;
	struct gic_registers *regs = &dev->regs;
	int n = 0;

	assert(cpu_id < MAX_CPU_NUM);
	//printf("gicc write offset = %x data = %x\n", offset, data);
	switch (offset)
	{
		case GICC_CTLR:
			regs->gicc_regs[cpu_id].ctlr.all = data;
			break;
		case GICC_PMR:
			regs->gicc_regs[cpu_id].pmr = data;
			break;
		case GICC_BPR:
			regs->gicc_regs[cpu_id].bpr = data;
			break;
		case GICC_IAR:
			regs->gicc_regs[cpu_id].iar = data;
			break;
		case GICC_EOIR:
			regs->gicc_regs[cpu_id].eoir = data;
			regs->gicc_regs[cpu_id].iar &= ~data;
			/*clear the interrupt active state*/
			clearActive(dev, data & 0x3ff);
			break;
		case GICC_RPR:
			regs->gicc_regs[cpu_id].rpr = data;
			break;
		case GICC_HPPIR:
			regs->gicc_regs[cpu_id].hppir = data;
			break;
		case GICC_ABPR:
			regs->gicc_regs[cpu_id].abpr = data;
			break;
		case GICC_AIAR:
			regs->gicc_regs[cpu_id].aiar = data;
			break;
		case GICC_AEOIR:
			regs->gicc_regs[cpu_id].aeoir = data;
			break;
		case GICC_AHPPIR:
			regs->gicc_regs[cpu_id].ahppir = data;
			break;
		case GICC_APR0 ... GICC_APR3:
			n = (offset - GICC_APR0) / 4;
			regs->gicc_regs[cpu_id].apr[n] = data;
			break;
		case GICC_NSAPR0 ... GICC_NSAPR3:
			n = (offset - GICC_NSAPR0) / 4;
			regs->gicc_regs[cpu_id].nsapr[n] = data;
			break;
		case GICC_IIDR:
			regs->gicc_regs[cpu_id].iidr = data;
			break;
		case GICC_DIR:
			regs->gicc_regs[cpu_id].dir = data;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "In %s, Cannot write the register at 0x%x in gicc_registers_write\n", __func__, offset);
			break;
	}
	return;
}

static uint32_t gicc_registers_read(conf_object_t *opaque, generic_address_t offset, int cpu_id)
{
	ft2000a_gic_device *dev = (ft2000a_gic_device *)opaque->obj;
	struct gic_registers *regs = &dev->regs;
	uint32_t data = 0, n = 0;

	assert(cpu_id < MAX_CPU_NUM);
	switch (offset)
	{
		case GICC_CTLR:
			data = regs->gicc_regs[cpu_id].ctlr.all;
			break;
		case GICC_PMR:
			data = regs->gicc_regs[cpu_id].pmr;
			break;
		case GICC_BPR:
			data = regs->gicc_regs[cpu_id].bpr;
			break;
		case GICC_IAR:
			data = regs->gicc_regs[cpu_id].iar;
			break;
		case GICC_EOIR:
			data = regs->gicc_regs[cpu_id].eoir;
			break;
		case GICC_RPR:
			data = regs->gicc_regs[cpu_id].rpr;
			break;
		case GICC_HPPIR:
			data = regs->gicc_regs[cpu_id].hppir;
			break;
		case GICC_ABPR:
			data = regs->gicc_regs[cpu_id].abpr;
			break;
		case GICC_AIAR:
			data = regs->gicc_regs[cpu_id].aiar;
			break;
		case GICC_AEOIR:
			data = regs->gicc_regs[cpu_id].aeoir;
			break;
		case GICC_AHPPIR:
			data = regs->gicc_regs[cpu_id].ahppir;
			break;
		case GICC_APR0 ... GICC_APR3:
			n = (offset - GICC_APR0) / 4;
			data = regs->gicc_regs[cpu_id].apr[n];
			break;
		case GICC_NSAPR0 ... GICC_NSAPR3:
			n = (offset - GICC_NSAPR0) / 4;
			data = regs->gicc_regs[cpu_id].nsapr[n];
			break;
		case GICC_IIDR:
			data = regs->gicc_regs[cpu_id].iidr;
			break;
		case GICC_DIR:
			data = regs->gicc_regs[cpu_id].dir;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "In %s, Cannot read the register at 0x%x in gicc_registers_read\n", __func__, offset);
			break;
	}
	//printf("gicc read offset = %x data = %x\n", offset, data);

	return data;
}

static exception_t ft2000a_gic_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_gic_device *dev = (ft2000a_gic_device *)opaque->obj;
	struct gic_registers *regs = &dev->regs;
	uint32_t *data = (uint32_t *)buf;

	if (offset >= GICD_BASE_ADDR && offset < GICC_BASE_ADDR)
	{
		/*Distributor register*/
		*data = gicd_registers_read(opaque, offset);
		return No_exp;
	}

	if (offset >= GICC_BASE_ADDR)
	{
		/*CPU interface register*/
		*data = gicc_registers_read(opaque, offset, 0);
		return No_exp;
	}
	skyeye_log(Error_log, __FUNCTION__, "In %s, Cannot read the register at 0x%x in ft2000a_gic\n", __func__, offset);

	return Access_exp;
}

static exception_t ft2000a_gic_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_gic_device *dev = (ft2000a_gic_device *)opaque->obj;
	struct gic_registers *regs = &dev->regs;
	uint32_t data = *(uint32_t *)buf;

	if (offset >= GICD_BASE_ADDR && offset < GICC_BASE_ADDR)
	{
		/*Distributor register*/
		gicd_registers_write(opaque, offset, data);
		return No_exp;
	}
	if (offset >= GICC_BASE_ADDR)
	{
		/*CPU interface register*/
		gicc_registers_write(opaque, offset, data, 0);
		return No_exp;
	}
	skyeye_log(Error_log, __FUNCTION__, "In %s, Cannot write the register at 0x%x in ft2000a_gic\n", __func__, offset);
	return Access_exp;
}

static char* ft2000a_gic_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	static char group_name[100] = {0};
	switch(id)
	{
		case 0 ... 2:
			return regs_name[id];
		case 3 ... 34:
			/*igroup[32]*/
			sprintf(group_name, "%s%d", regs_name[3], id - 3);
			break;
		case 35 ... 66:
			/*isenable[32]*/
			sprintf(group_name, "%s%d", regs_name[4], id - 35);
			break;
		case 67 ... 98:
			/*icenable[32]*/
			sprintf(group_name, "%s%d", regs_name[5], id - 67);
			break;
		case 99 ... 130:
			/*ispend[32]*/
			sprintf(group_name, "%s%d", regs_name[6], id - 99);
			break;
		case 131 ... 162:
			/*icpend[32]*/
			sprintf(group_name, "%s%d", regs_name[7], id - 131);
			break;
		case 163 ... 194:
			/*isactive[32]*/
			sprintf(group_name, "%s%d", regs_name[8], id - 163);
			break;
		case 195 ... 226:
			/*icactive[32]*/
			sprintf(group_name, "%s%d", regs_name[9], id - 195);
			break;
		case 227 ... 481:
			/*ipriority[255]*/
			sprintf(group_name, "%s%d", regs_name[10], id - 227);
			break;
		case 482 ... 736:
			/*itargets[255]*/
			sprintf(group_name, "%s%d", regs_name[11], id - 482);
			break;
		case 737 ... 800:
			/*icfg[64]*/
			sprintf(group_name, "%s%d", regs_name[12], id - 737);
			break;
		case 801 ... 864:
			/*nsac[64]*/
			sprintf(group_name, "%s%d", regs_name[13], id - 801);
			break;
		case 865:
			/*sgir*/
			return regs_name[14];
		case 866 ... 869:
			/*cpendsgi[4]*/
			sprintf(group_name, "%s%d", regs_name[15], id - 866);
			break;
		case 870 ... 873:
			/*spendsgi[4]*/
			sprintf(group_name, "%s%d", regs_name[16], id - 870);
			break;
		case 874 ... 884:
			/*ctlr ... ahppir*/
			return regs_name[id - 874 + 17];
		case 885 ... 888:
			/*apr[4]*/
			sprintf(group_name, "%s%d", regs_name[28], id - 885);
			break;
		case 889 ... 892:
			/*nsapr[4]*/
			sprintf(group_name, "%s%d", regs_name[29], id - 889);
			break;
		case 893 ... 894:
			/*iidr dir*/
			return regs_name[id - 893 + 30];
		default:
			break;
	}
	return group_name;
}

static exception_t ft2000a_gic_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	ft2000a_gic_device *dev = (ft2000a_gic_device*)conf_obj->obj;
	struct gic_registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t ft2000a_gic_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct gicd_registers) / 4;
}

static uint32_t ft2000a_gic_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	ft2000a_gic_device *dev = (ft2000a_gic_device*)conf_obj->obj;
	struct gic_registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t ft2000a_gic_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct gic_registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t ft2000a_gic_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	uint32_t offset = 0;
	switch(id)
	{
		case 0 ... 2:
			offset = regs_offset[id];
			break;
		case 3 ... 34:
			/*GICD_IGROUPR0 ... GICD_IGROUPR31*/
			offset = regs_offset[3] + (id - 3) * 4;
			break;
		case 35 ... 66:
			/*GICD_ISENABLER0 ... GICD_ISENABLER31*/
			offset = regs_offset[4] + (id - 35) * 4;
			break;
		case 67 ... 98:
			/*GICD_ICENABLER0 ... GICD_ICENABLER31*/
			offset = regs_offset[5] + (id - 67) * 4;
			break;
		case 99 ... 130:
			/*GICD_ISPENDR0 ... GICD_ISPENDR31*/
			offset = regs_offset[6] + (id - 99) * 4;
			break;
		case 131 ... 162:
			/*GICD_ICPENDR0 ... GICD_ICPENDR31*/
			offset = regs_offset[7] + (id - 131) * 4;
			break;
		case 163 ... 194:
			/*GICD_ISACTIVER0 ... GICD_ISACTIVER31*/
			offset = regs_offset[8] + (id - 163) * 4;
			break;
		case 195 ... 226:
			/*GICD_ICACTIVER0 ... GICD_ICACTIVER31*/
			offset = regs_offset[9] + (id - 195) * 4;
			break;
		case 227 ... 481:
			/*GICD_IPRIORITYR0 ... GICD_IPRIORITYR254*/
			offset = regs_offset[10] + (id - 227) * 4;
			break;
		case 482 ... 736:
			/*GICD_ITARGETSR0 ... GICD_ITARGETSR255*/
			offset = regs_offset[11] + (id - 482) * 4;
			break;
		case 737 ... 800:
			/*GICD_ICFGR0 ... GICD_ICFGR63*/
			offset = regs_offset[12] + (id - 737) * 4;
			break;
		case 801 ... 864:
			/*GICD_NSACR0 ... GICD_NSACR63*/
			offset = regs_offset[13] + (id - 801) * 4;
			break;
		case 865:
			/*GICD_SGIR*/
			offset = regs_offset[14];
			break;
		case 866 ... 869:
			/*GICD_CPENDSGIR0 ... GICD_CPENDSGIR3*/
			offset = regs_offset[15] + (id - 866) * 4;
			break;
		case 870 ... 873:
			/*GICD_SPENDSGIR0 ... GICD_SPENDSGIR3*/
			offset = regs_offset[16] + (id - 870) * 4;
			break;
		case 874 ... 884:
			/*GICC_CTLR ... GICC_AHPPIR*/
			offset = regs_offset[id - 874 + 17];
			break;
		case 885 ... 888:
			/*GICC_APR0 ... GICC_APR3*/
			offset = regs_offset[28] + (id - 885) * 4;
			break;
		case 889 ... 892:
			/* GICC_NSAPR0 ... GICC_NSAPR3*/
			offset = regs_offset[29] + (id - 889) * 4;
			break;
		case 893 ... 894:
			/*GICC_IIDR GICC_DIR*/
			offset = regs_offset[id - 893 + 30];
			break;
		default:
			break;
	}
	return offset;
}

static int peripheral_raise_signal(conf_object_t *conf_obj, int interruptID)
{
	ft2000a_gic_device* dev = (ft2000a_gic_device*)conf_obj->obj;

	setPend(dev, interruptID);
	/*
	 *PPI:interrupt ID 16 ~ 31
	 *SPIs:interrupt ID 32 ~ 1019
	 */
	return 0;
}

static int peripheral_lower_signal(conf_object_t *conf_obj, int interruptID)
{
	ft2000a_gic_device* dev = (ft2000a_gic_device*)conf_obj->obj;

	return 0;
}

void init_gic_register(struct gic_registers *regs)
{
	regs->gicd_regs.ctlr[0].all = 0;
	regs->gicd_regs.igroup[0] = 0;
}

static conf_object_t* new_ft2000a_gic(const char *obj_name)
{
	ft2000a_gic_device* dev = skyeye_mm_zero(sizeof(ft2000a_gic_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct gic_registers *regs = &(dev->regs);
	init_gic_register(regs);
	dev->cpuNum = 0;
	return dev->obj;
}

static exception_t free_ft2000a_gic(conf_object_t* opaque)
{
	ft2000a_gic_device *dev = (ft2000a_gic_device*)opaque->obj;
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t config_ft2000a_gic(conf_object_t* opaque)
{
	ft2000a_gic_device *dev = (ft2000a_gic_device *)opaque->obj;
	dev->regs.gicd_regs.typer.bit.CPUNumber = dev->cpuNum;
	dev->regs.gicd_regs.typer.bit.ITLinesNumber = 2; //ft200a-hk support number of interrupts is 96. 32*(ITLinesNumber + 1)
	//int pthread_id;
	//create_thread_scheduler(1000, Periodic_sched, gic_generateExceptions, opaque, &pthread_id);
	system_register_timer_handler(opaque, 1000, (time_sched_t)gic_generateExceptions, SCHED_US|SCHED_NORMAL);
	return No_exp;
}

static void ft2000a_gic_register_attribute(conf_class_t* clss)
{
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	ft2000a_gic_device *dev = obj->obj;
	if (index < MAX_CPU_NUM)
	{
		dev->cpu_list[index].signal = obj2;
		dev->cpu_list[index].signal_iface = (core_signal_intf *)SKY_get_iface(obj2, CORE_SIGNAL_INTF_NAME);
		if(dev->cpu_list[index].signal_iface == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get cpuID = %d %s interface from %s fail.", index, CORE_SIGNAL_INTF_NAME, obj2->objname);
		}
		dev->cpuNum ++;
	} else
	{
		printf("Can not support cpuID = %d iface register! MAX_CPU_NUM = %d\n", index, MAX_CPU_NUM);
	}
	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	ft2000a_gic_device *dev = obj->obj;
	if (index < MAX_CPU_NUM)
		*obj2 = dev->cpu_list[index].signal;
	*port = NULL;
	return No_exp;
}

void init_ft2000a_gic(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "ft2000a_gic",
		.class_desc = "ft2000a_gic",
		.new_instance = new_ft2000a_gic,
		.free_instance = free_ft2000a_gic,
		.config_instance = config_ft2000a_gic
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = ft2000a_gic_read,
		.write = ft2000a_gic_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = ft2000a_gic_get_regvalue_by_id,
		.get_regname_by_id = ft2000a_gic_get_regname_by_id,
		.set_regvalue_by_id = ft2000a_gic_set_regvalue_by_id,
		.get_regnum = ft2000a_gic_get_regnum,
		.get_regid_by_name = ft2000a_gic_get_regid_by_name,
		.get_regoffset_by_id = ft2000a_gic_get_regoffset_by_id
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static general_signal_intf signal_intf = {
		.raise_signal = peripheral_raise_signal,
		.lower_signal = peripheral_lower_signal,
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_intf);

	static gic_interface_intf gic_intf = {
		.write_gic_interface = gic_interface_write,
		.read_gic_interface = gic_interface_read,
	};
	SKY_register_iface(clss, GIC_INTERFACE_INTF, &gic_intf);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription)
		{
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf
		},
		(struct InterfaceDescription)
		{
			.name = GENERAL_SIGNAL_INTF_NAME,
			.iface = &signal_intf
		},
		(struct InterfaceDescription)
		{
			.name = GIC_INTERFACE_INTF,
			.iface = &gic_intf
		},
	};

	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	ft2000a_gic_register_attribute(clss);
}

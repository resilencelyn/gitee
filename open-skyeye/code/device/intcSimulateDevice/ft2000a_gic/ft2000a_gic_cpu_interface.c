/*************************************************************************
	> File Name    : ft2000a_gic_cpu_interface.c
	> Author       : jiachao
	> Mail         : jiachao@163.com
	> Created Time : 2019/11/15 9:45:30
************************************************************************/
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

uint32_t ReadGICC_IAR(conf_object_t *opaque, int cpu_id)
{
	uint32_t iar_val, pendID, cpuID;
	ft2000a_gic_device *dev = opaque->obj;
	struct gicc_registers *gicc_regs = &dev->regs.gicc_regs[cpu_id];
	struct gicd_registers *gicd_regs = &dev->regs.gicd_regs;

	pendID = HighestPriorityPendingInterrupt(opaque, cpu_id);

	if (isGrp0Int(dev, pendID) && (gicd_regs->ctlr[cpu_id].bit.EnableGrp0 == 0 || gicc_regs->ctlr.bit.EnableGrp0 == 0)
		|| (!isGrp0Int(dev, pendID) && (gicd_regs->ctlr[cpu_id].bit.EnableGrp1 == '0' || gicc_regs->ctlr.bit.EnableGrp1 == '0')))
		pendID = 1023;

	if (pendID != 1023)
	{
		if (isGrp0Int(dev, pendID))
		{
			if (ReadGICD_NSACR(dev, pendID))
				pendID = 1023;
		} else
		{
			if (ReadGICD_NSACR(dev, pendID) == 0 && gicc_regs->ctlr.bit.AckCtl == 0)
				pendID = 1022;
		}
	}

	if (pendID < 16)
	{
		int grpNum = pendID / 4;
		int grpIndex = pendID % 4;
		for (cpuID = 0; cpuID < dev->cpuNum; cpuID++)
		{
			int cpuListValue = gicd_regs->spendsgi[grpNum] >> (8 * grpIndex);
			if ((cpuListValue >> cpuID) & 0x1)
				break;
		}
	}

	if (cpuID == dev->cpuNum)
		cpuID = 0;

	if (pendID < 1020)
	{
		/* Set active and attempt to clear pending */
		setActive(dev, pendID);
		clearPend(dev, pendID);
	}

	iar_val = pendID | (cpuID << 10);
	return iar_val;
}

exception_t gicc_interface_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, int cpu_id)
{
	ft2000a_gic_device *dev = opaque->obj;
	struct gic_registers *regs = &dev->regs;
	int n = 0;

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
			skyeye_log(Error_log, __FUNCTION__, "In %s, Cannot write the register at 0x%x in gic_interface_write\n", __func__, offset);
			return Not_found_exp;
	}
}

static void gicd_interface_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, int cpu_id)
{
	ft2000a_gic_device *dev = opaque->obj;
	struct gic_registers *regs = &dev->regs;
	int n;

	switch (offset)
	{
		case GICD_CTLR:
			regs->gicd_regs.ctlr[cpu_id].all = data;
			break;
		case GICD_IGROUPR0 ... GICD_IGROUPR31:
			n = (offset - GICD_IGROUPR0) / 4;
			regs->gicd_regs.igroup[n] = data;
			break;
		case GICD_ISENABLER0 ... GICD_ISENABLER31:
			n = (offset - GICD_ISENABLER0) / 4;
			regs->gicd_regs.isenable[n] = data;
			break;
		case GICD_ICENABLER0 ... GICD_ICENABLER31:
			n = (offset - GICD_ICENABLER0) / 4;
			regs->gicd_regs.icenable[n] = data;
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

exception_t gic_interface_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count, int cpu_id)
{
	ft2000a_gic_device *dev = opaque->obj;
	struct gic_registers *regs = &dev->regs;
	uint32_t data = *(uint32_t *)buf;

	assert(cpu_id < MAX_CPU_NUM);
	if (offset >= GICD_BASE_ADDR && offset < GICC_BASE_ADDR)
	{
		/*Distributor register*/
		gicd_interface_write(opaque, offset, data, cpu_id);
		return No_exp;
	}

	if (offset >= GICC_BASE_ADDR)
	{
		/*CPU interface register*/
		gicc_interface_write(opaque, offset, data, cpu_id);
		return No_exp;
	}

	return No_exp;
}

static uint32_t gicc_interface_read(conf_object_t *opaque, generic_address_t offset, int cpu_id)
{
	ft2000a_gic_device *dev = opaque->obj;
	struct gic_registers *regs = &dev->regs;
	uint32_t data;
	int n = 0;

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
			regs->gicc_regs[cpu_id].iar = ReadGICC_IAR(opaque, cpu_id);
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
			skyeye_log(Error_log, __FUNCTION__, "In %s, Cannot read the register at 0x%x in gic_interface_read\n", __func__, offset);
			data = 0;
	}

	return data;
}

static uint32_t gicd_interface_read(conf_object_t *opaque, generic_address_t offset, int cpu_id)
{
	ft2000a_gic_device *dev = opaque->obj;
	struct gic_registers *regs = &dev->regs;
	uint32_t data = 0, n = 0;

	switch (offset)
	{
		case GICD_CTLR:
			data = regs->gicd_regs.ctlr[cpu_id].all;
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
	return data;
}

exception_t gic_interface_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count, int cpu_id)
{
	ft2000a_gic_device *dev = opaque->obj;
	struct gic_registers *regs = &dev->regs;
	uint32_t data;
	int n = 0;

	if (offset >= GICD_BASE_ADDR && offset < GICC_BASE_ADDR)
	{
		/*Distributor register*/
		data = gicd_interface_read(opaque, offset, cpu_id);
		return No_exp;
	}

	if (offset >= GICC_BASE_ADDR)
	{
		/*CPU interface register*/
		data = gicc_interface_read(opaque, offset, cpu_id);
		return No_exp;
	}

	*(uint32_t *)buf = data;
	return No_exp;
}

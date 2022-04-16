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
#include "zynq7000_gic.h"

static bool_t isEnabled(zynq7000_gic_device *dev, uint32_t intID)
{
    uint32_t grpNum = intID / 32;
    uint32_t grpIndex = intID % 32;
    return (dev->regs.gicd_regs.isenable[grpNum] >> grpIndex) & 0x1;
}

static bool_t isActive(zynq7000_gic_device *dev, uint32_t intID)
{
    uint32_t grpNum = intID / 32;
    uint32_t grpIndex = intID % 32;
    return (dev->regs.gicd_regs.isactive[grpNum] >> grpIndex) & 0x1;
}

static void setActive(zynq7000_gic_device *dev, uint32_t intID)
{
    uint32_t grpNum = intID / 32;
    uint32_t grpIndex = intID % 32;
    dev->regs.gicd_regs.isactive[grpNum] |= (1 << grpIndex);
}

static void clearActive(zynq7000_gic_device *dev, uint32_t intID)
{
    uint32_t grpNum = intID / 32;
    uint32_t grpIndex = intID % 32;
    dev->regs.gicd_regs.isactive[grpNum] &= ~(1 << grpIndex);
}

static bool_t isTargetCpu(zynq7000_gic_device *dev, uint32_t intID, uint32_t cpu_id)
{
    uint32_t grpNum = intID / 4;
    uint32_t grpIndex = intID % 4;
    return (dev->regs.gicd_regs.itargets[grpNum] >> cpu_id) & 0x1;
}

static uint32_t ReadGICD_IPRIORITYR(zynq7000_gic_device *dev, uint32_t intID)
{
    uint32_t grpNum = intID / 4;
    uint32_t grpIndex = intID % 4;
    return ((dev->regs.gicd_regs.ipriority[grpNum] >> (grpIndex * 8)) & 0xff);
}

static uint32_t GIC_PriorityMask(uint32_t n, uint32_t NS_mask)
{
    assert(n >= 0);
    assert(n <= 7);

    if (NS_mask == 1)
        n = n - 1;

    if (n < MINIMUM_BINARY_POINT)
        n = MINIMUM_BINARY_POINT;

    uint32_t mask = (0xff00 >> (7 - n)) & 0xff;

    return mask;
}

static bool_t isGrp0Int(zynq7000_gic_device *dev, uint32_t intID)
{
    uint32_t grpNum = intID / 32;
    uint32_t grpIndex = intID % 32;
    uint32_t grpValue = dev->regs.gicd_regs.igroup[grpNum];

    return ((grpValue & (1 << grpIndex)) == 0);
}

static bool_t anyActiveInterrupts(zynq7000_gic_device *dev)
{
    uint32_t max_support_grp = dev->regs.gicd_regs.ictr.bit.ITLinesNumber + 1;
    uint32_t i;

    for (i = 0; i < max_support_grp; i++)
    {
        if (dev->regs.gicd_regs.isactive[i] != 0)
            return True;
    }

    return False;
}

static bool_t isPend(zynq7000_gic_device *dev, uint32_t intID)
{
    uint32_t grpNum = intID / 32;
    uint32_t grpIndex = intID % 32;
    if(bit(dev->regs.gicd_regs.ispend[grpNum], grpIndex))
        return True;
    else
        return False;
}

static bool_t isPending(zynq7000_gic_device* dev, uint32_t intID, uint32_t cpu_id)
{
    struct gic_registers *regs = &dev->regs;
    bool_t pending = False;

    if (isPend(dev, intID) && !isActive(dev, intID))
    {
        pending = True;
    }
    return pending;
}

static void setPend(zynq7000_gic_device *dev, uint32_t intID)
{
    uint32_t grpNum = intID / 32;
    uint32_t grpIndex = intID % 32;
    dev->regs.gicd_regs.ispend[grpNum] |= (1 << grpIndex);
}

static void clearPend(zynq7000_gic_device *dev, uint32_t intID)
{
    uint32_t grpNum = intID / 32;
    uint32_t grpIndex = intID % 32;
    dev->regs.gicd_regs.ispend[grpNum] &= ~(1 << grpIndex);
}

static uint32_t ReadGICD_NSACR(zynq7000_gic_device *dev, uint32_t intID)
{
    uint32_t grpNum = intID / 16;
    uint32_t grpIndex = intID % 16;
    uint32_t grpValue = (dev->regs.gicd_regs.nsac[grpNum] >> (2 * grpIndex)) & 0x3;

    return grpValue;
}

static uint32_t getSGISourceCpuID(uint8_t data)
{
    uint32_t i;

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

static void SignalIRQ(conf_object_t *conf_obj, uint32_t cpu_id, uint32_t intID)
{
    zynq7000_gic_device* dev = (zynq7000_gic_device*)conf_obj->obj;
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

static void SignalFIQ(conf_object_t *conf_obj, uint32_t cpu_id, uint32_t intID)
{
    zynq7000_gic_device* dev = (zynq7000_gic_device*)conf_obj->obj;
    interrupt_signal_t interrupt_signal;

    interrupt_signal.arm_signal.firq = Low_level;
    interrupt_signal.arm_signal.irq = Prev_level;
    interrupt_signal.arm_signal.reset = Prev_level;
    dev->regs.gicc_regs[cpu_id].iar = (cpu_id << 10) | intID;
    assert(cpu_id < dev->cpuNum);
    if (dev->cpu_list[cpu_id].signal_iface)
        dev->cpu_list[cpu_id].signal_iface->signal(dev->cpu_list[cpu_id].signal, &interrupt_signal);
}

uint32_t HighestPriorityPendingInterrupt(conf_object_t *conf_obj, uint32_t cpu_id)
{
    zynq7000_gic_device* dev = (zynq7000_gic_device*)conf_obj->obj;
    struct gic_registers *regs = &dev->regs;
    uint32_t group_enabled, intID;

    uint32_t hppi = 1023;    // Set initial ID to be no intterupt pending
    uint32_t max_support_interrupt_num = 32 * (regs->gicd_regs.ictr.bit.ITLinesNumber + 1);    //how many interrupts are supported

    for (intID = 0; intID < max_support_interrupt_num; intID++)
    {
        group_enabled = (isGrp0Int(dev, intID) && regs->gicd_regs.dcr[cpu_id].bit.EnableGrp0)
            || (!isGrp0Int(dev, intID) && regs->gicd_regs.dcr[cpu_id].bit.EnableGrp1);
        
        if (isPending(dev, intID, cpu_id) && isEnabled(dev, intID))
        {
            if (group_enabled || IGNORE_GROUP_ENABLE)
            {
                hppi = intID;
            }
        }
    }

    return hppi;
}

static bool_t updateExceptionState(conf_object_t *conf_obj, uint32_t cpu_id, uint32_t *next_grp0, uint32_t *ackIntID)
{
    zynq7000_gic_device* dev = (zynq7000_gic_device*)conf_obj->obj;
    struct gic_registers *regs = &dev->regs;
    uint32_t sbp = regs->gicc_regs[cpu_id].bpr & 0x7;    //Secure binary point
    uint32_t nsbp = regs->gicc_regs[cpu_id].abpr & 0x7;    //non-Secure binary point
    bool_t next_int = False;
    *next_grp0 = False;

    uint32_t intID = HighestPriorityPendingInterrupt(conf_obj, cpu_id);
    if (PriorityIsHigher(ReadGICD_IPRIORITYR(dev, intID), regs->gicc_regs[cpu_id].pmr & 0xff)
            && isPending(dev, intID, cpu_id))
    {
        uint8_t smsk, nsmsk;
        fflush(stdout);
        smsk = GIC_PriorityMask(sbp, 0);
        if (regs->gicc_regs[cpu_id].icr.bit.CBPR)
            nsmsk = smsk;
        else
        {
            fflush(stdout);
            nsmsk = GIC_PriorityMask(nsbp, 1);
        }

        if (isGrp0Int(dev, intID) && regs->gicd_regs.dcr[cpu_id].bit.EnableGrp0)
        {
            fflush(stdout);
            if (!anyActiveInterrupts(dev) ||
                    PriorityIsHigher(ReadGICD_IPRIORITYR(dev, intID), regs->gicc_regs[cpu_id].rpr & smsk))
            {
                fflush(stdout);
                next_int = True;
                *next_grp0 = True;
            }
        } else if (!isGrp0Int(dev, intID) && regs->gicd_regs.dcr[cpu_id].bit.EnableGrp1)
        {
            fflush(stdout);
            if (!anyActiveInterrupts(dev) ||
                    PriorityIsHigher(ReadGICD_IPRIORITYR(dev, intID), regs->gicc_regs[cpu_id].rpr & nsmsk))
            {
                fflush(stdout);
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
    zynq7000_gic_device* dev = (zynq7000_gic_device*)conf_obj->obj;
    uint32_t cpu_count, cpu_id, next_grp0 = 0, intID = 0;
    bool_t next_int = False;
    bool_t cpu_fiq = False;
    bool_t cpu_irq = False;

    cpu_count = dev->regs.gicd_regs.ictr.bit.CPUNumber;
    for (cpu_id = 0; cpu_id < cpu_count; cpu_id++)
    {
        next_int = updateExceptionState(conf_obj, cpu_id, &next_grp0, &intID);
        struct gicc_registers *gicc_regs = &dev->regs.gicc_regs[cpu_id];
        if (next_int)
        {
            if (next_grp0 && gicc_regs->icr.bit.FIQEn)
            {
                if (gicc_regs->icr.bit.EnableGrp0)
                    cpu_fiq = True;
            } else
            {
                if ((next_grp0 && gicc_regs->icr.bit.EnableGrp0)
                        || (!next_grp0 && gicc_regs->icr.bit.EnableGrp1))
                    cpu_irq = True;
            }
        }
        // Optional bypass logic
        if (gicc_regs->icr.bit.EnableGrp0 == 0
                || gicc_regs->icr.bit.FIQEn == 0)
        {
            if (gicc_regs->icr.bit.FIQEn == 0)

                cpu_fiq = False;    // Set FIQ to bypass
        }

        if (gicc_regs->icr.bit.EnableGrp1 == 0
                && (gicc_regs->icr.bit.EnableGrp0 == 0 || gicc_regs->icr.bit.FIQEn == 1))
        {
            if (gicc_regs->icr.bit.FIQEn)
                cpu_irq = False;    // Set IRQ to bypass
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

static exception_t gicd_registers_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    zynq7000_gic_device *dev = (zynq7000_gic_device *)opaque->obj;
    struct gic_registers *regs = &dev->regs;
    uint32_t n;
    uint32_t data = *(uint32_t *)buf;
    switch (offset)
    {
        case GICD_DCR:
            regs->gicd_regs.dcr[0].all = data;
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
            regs->gicd_regs.ipriority[n] = clear_bits(data, 2, 0);
            break;
        case GICD_ITARGETSR0 ... GICD_ITARGETSR254:
            n = (offset - GICD_ITARGETSR0) / 4;
            regs->gicd_regs.itargets[n] = data;
            break;
        case GICD_ICFGR0 ... GICD_ICFGR63:
            n = (offset - GICD_ICFGR0) / 4;
            regs->gicd_regs.icfg[n] = data;
            break;
        case GICD_SGIR:
            {
                regs->gicd_regs.sgir.all = data;
                /* SGI:interrupt ID 0 ~ 15 */
                uint32_t sgiIntID = regs->gicd_regs.sgir.bit.SGIINTID;
                regs->gicd_regs.ispend[0] |= (1 << sgiIntID);
            }
            break;
        case GICD_CPENDSGIR0 ... GICD_CPENDSGIR3:
            {
                n = (offset - GICD_CPENDSGIR0) / 4;
                regs->gicd_regs.cpendsgi[n] = data;
                uint32_t sgiIntID = regs->gicd_regs.sgir.bit.SGIINTID;
                regs->gicd_regs.ispend[0] &= ~(1 << sgiIntID);
            }
            break;
        case GICD_SPENDSGIR0 ... GICD_SPENDSGIR3:
            {
                n = (offset - GICD_SPENDSGIR0) / 4;
                regs->gicd_regs.spendsgi[n] = data;
                uint32_t sgiIntID = regs->gicd_regs.sgir.bit.SGIINTID;
                uint32_t grdIndex = sgiIntID % 4;
                dev->sgiIntCpu_list[sgiIntID] = getSGISourceCpuID((uint8_t)(data >> (8 * grdIndex)));
            }
            break;
        case GICD_ICPIDR0 ... GICD_ICPIDR7:
            n = (offset - GICD_ICPIDR0) / 4;
            regs->gicd_regs.icpidr[n] = data;
            break;
        case GICD_ICCIDR0 ... GICD_ICCIDR3:
            n = (offset - GICD_ICCIDR0) / 4;
            regs->gicd_regs.iccidr[n] = data;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "In %s, Cannot write the register at 0x%x in zynq7000_gic\n", __func__, offset);
            break;
    }
    return;
}

static exception_t gicd_registers_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    zynq7000_gic_device *dev = (zynq7000_gic_device *)opaque->obj;
    struct gic_registers *regs = &dev->regs;
    uint32_t n = 0;
    switch (offset)
    {
        case GICD_DCR:
            *(uint32_t *)buf = regs->gicd_regs.dcr[0].all;
            break;
        case GICD_ICTR:
            *(uint32_t *)buf = regs->gicd_regs.ictr.all;
            break;
        case GICD_IIDR:
            *(uint32_t *)buf = regs->gicd_regs.iidr;
            break;
        case GICD_IGROUPR0 ... GICD_IGROUPR31:
            n = (offset - GICD_IGROUPR0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.igroup[n];
            break;
        case GICD_ISENABLER0 ... GICD_ISENABLER31:
            n = (offset - GICD_ISENABLER0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.isenable[n];
            break;
        case GICD_ICENABLER0 ... GICD_ICENABLER31:
            n = (offset - GICD_ICENABLER0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.icenable[n];
            break;
        case GICD_ISPENDR0 ... GICD_ISPENDR31:
            n = (offset - GICD_ISPENDR0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.ispend[n];
            break;
        case GICD_ICPENDR0 ... GICD_ICPENDR31:
            n = (offset - GICD_ICPENDR0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.icpend[n];
            break;
        case GICD_ISACTIVER0 ... GICD_ISACTIVER31:
            n = (offset - GICD_ISACTIVER0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.isactive[n];
            break;
        case GICD_ICACTIVER0 ... GICD_ICACTIVER31:
            n = (offset - GICD_ICACTIVER0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.icactive[n];
            break;
        case GICD_IPRIORITYR0 ... GICD_IPRIORITYR254:
            n = (offset - GICD_IPRIORITYR0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.ipriority[n];
            break;
        case GICD_ITARGETSR0 ... GICD_ITARGETSR254:
            n = (offset - GICD_ITARGETSR0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.itargets[n];
            break;
        case GICD_ICFGR0 ... GICD_ICFGR63:
            n = (offset - GICD_ICFGR0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.icfg[n];
            break;
        case GICD_CPENDSGIR0 ... GICD_CPENDSGIR3:
            n = (offset - GICD_CPENDSGIR0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.cpendsgi[n];
            break;
        case GICD_SPENDSGIR0 ... GICD_SPENDSGIR3:
            n = (offset - GICD_SPENDSGIR0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.spendsgi[n];
            break;
        case GICD_ICPIDR0 ... GICD_ICPIDR7:
            n = (offset - GICD_ICPIDR0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.icpidr[n];
            break;
        case GICD_ICCIDR0 ... GICD_ICCIDR3:
            n = (offset - GICD_ICCIDR0) / 4;
            *(uint32_t *)buf = regs->gicd_regs.iccidr[n];
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "In %s, Cannot write the register at 0x%x in zynq7000_gic\n", __func__, offset);
            break;
    }
    return;
}

uint32_t ReadGICC_IAR(conf_object_t *opaque, uint32_t cpu_id)
{
    uint32_t iar_val, pendID, cpuID;
    zynq7000_gic_device *dev = opaque->obj;
    struct gicc_registers *gicc_regs = &dev->regs.gicc_regs[cpu_id];
    struct gicd_registers *gicd_regs = &dev->regs.gicd_regs;

    pendID = HighestPriorityPendingInterrupt(opaque, cpu_id);

    if (isGrp0Int(dev, pendID) && (gicd_regs->dcr[cpu_id].bit.EnableGrp0 == 0 || gicc_regs->icr.bit.EnableGrp0 == 0)
        || (!isGrp0Int(dev, pendID) && (gicd_regs->dcr[cpu_id].bit.EnableGrp1 == '0' || gicc_regs->icr.bit.EnableGrp1 == '0')))
        pendID = 1023;

    if (pendID != 1023)
    {
        if (isGrp0Int(dev, pendID))
        {
            if (ReadGICD_NSACR(dev, pendID))
                pendID = 1023;
        } else
        {
            if (ReadGICD_NSACR(dev, pendID) == 0 && gicc_regs->icr.bit.AckCtl == 0)
                pendID = 1022;
        }
    }

    if (pendID < 16)
    {
        uint32_t grpNum = pendID / 4;
        uint32_t grpIndex = pendID % 4;
        for (cpuID = 0; cpuID < dev->cpuNum; cpuID++)
        {
            uint32_t cpuListValue = gicd_regs->spendsgi[grpNum] >> (8 * grpIndex);
            if ((cpuListValue >> cpuID) & 0x1)
                break;
        }
    }

    if (cpuID == dev->cpuNum)
        cpuID = 0;

    if (pendID < 1020)
    {
        // Set active and attempt to clear pending
        setActive(dev, pendID);
        clearPend(dev, pendID);
    }

    iar_val = pendID | (cpuID << 10);
    return iar_val;
}

static exception_t gicc_registers_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    zynq7000_gic_device *dev = (zynq7000_gic_device *)opaque->obj;
    struct gic_registers *regs = &dev->regs;
    uint32_t n = 0;
    uint32_t data = *(uint32_t *)buf;
    uint32_t cpu_id = 0;
    switch (offset)
    {
        case GICC_ICR:
            regs->gicc_regs[cpu_id].icr.all = data;
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
        case GICC_IDR:
            regs->gicc_regs[cpu_id].idr = data;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "In %s, Cannot write the register at 0x%x in gicc_registers_write\n", __func__, offset);
            break;
    }
    return;
}

static exception_t gicc_registers_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    zynq7000_gic_device *dev = (zynq7000_gic_device *)opaque->obj;
    struct gic_registers *regs = &dev->regs;
    uint32_t n = 0;
    uint32_t cpu_id = 0;
    switch (offset)
    {
        case GICC_ICR:
            *(uint32_t *)buf = regs->gicc_regs[cpu_id].icr.all;
            break;
        case GICC_PMR:
            *(uint32_t *)buf = regs->gicc_regs[cpu_id].pmr;
            break;
        case GICC_BPR:
            *(uint32_t *)buf = regs->gicc_regs[cpu_id].bpr;
            break;
        case GICC_IAR:
            *(uint32_t *)buf = regs->gicc_regs[cpu_id].iar;
            break;
        case GICC_EOIR:
            *(uint32_t *)buf = regs->gicc_regs[cpu_id].eoir;
            break;
        case GICC_RPR:
            *(uint32_t *)buf = regs->gicc_regs[cpu_id].rpr;
            break;
        case GICC_HPPIR:
            *(uint32_t *)buf = regs->gicc_regs[cpu_id].hppir;
            break;
        case GICC_ABPR:
            *(uint32_t *)buf = regs->gicc_regs[cpu_id].abpr;
            break;
        case GICC_IDR:
            *(uint32_t *)buf = regs->gicc_regs[cpu_id].idr;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "In %s, Cannot read the register at 0x%x in gicc_registers_read\n", __func__, offset);
            break;
    }
    //printf("gicc read offset = %x *(uint32_t *)buf = %x\n", offset, *(uint32_t *)buf);

    return;
}

static char* zynq7000_gic_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
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
            /*icr ... ahppir*/
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
            /*iidr idr*/
            return regs_name[id - 893 + 30];
        default:
            break;
    }
    return group_name;
}

static exception_t zynq7000_gic_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    zynq7000_gic_device *dev = (zynq7000_gic_device*)conf_obj->obj;
    struct gic_registers *regs = &(dev->regs);
    *((uint32_t*)regs + id) = value;
    return No_exp;
}

static uint32_t zynq7000_gic_get_regnum(conf_object_t *conf_obj)
{
    return  sizeof(struct gicd_registers) / 4;
}

static uint32_t zynq7000_gic_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    zynq7000_gic_device *dev = (zynq7000_gic_device*)conf_obj->obj;
    struct gic_registers *regs = &(dev->regs);
    uint32_t* regs_value = (uint32_t*)regs + id;
    return *regs_value;
}

static uint32_t zynq7000_gic_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
    uint32_t regnum = sizeof(struct gic_registers) / 4, i;
    for(i = 0; i < regnum; i++) 
    {
        if(strcmp(name, regs_name[i]) == 0)
            return i;
    }
    return regnum;
}

static uint32_t zynq7000_gic_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
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
            /*GICC_ICR ... GICC_AHPPIR*/
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
            /*GICC_IIDR GICC_IDR*/
            offset = regs_offset[id - 893 + 30];
            break;
        default:
            break;
    }
    return offset;
}

static int peripheral_raise_signal(conf_object_t *conf_obj, int interruptID)
{
    zynq7000_gic_device* dev = (zynq7000_gic_device*)conf_obj->obj;
    setPend(dev, interruptID);
    uint32_t grpNum = interruptID / 32;
    uint32_t grpIndex = interruptID % 32;
    return 0;
}

static int peripheral_lower_signal(conf_object_t *conf_obj, int interruptID)
{
    zynq7000_gic_device* dev = (zynq7000_gic_device*)conf_obj->obj;
    return 0;
}

void init_gic_register(struct gic_registers *regs)
{
    regs->gicd_regs.dcr[0].all = 0;
    regs->gicd_regs.igroup[0] = 0;
}

static conf_object_t* new_zynq7000_gic(const char *obj_name)
{
    zynq7000_gic_device* dev = skyeye_mm_zero(sizeof(zynq7000_gic_device));
    dev->obj = new_conf_object(obj_name, dev);
    struct gic_registers *regs = &(dev->regs);
    init_gic_register(regs);
    dev->cpuNum = 0;
    //cc
    regs->gicc_regs[0].bpr = 0x00000002;
    regs->gicc_regs[0].iar = 0x000003FF;
    regs->gicc_regs[0].rpr = 0x000000FF;
    regs->gicc_regs[0].hppir = 0x000003FF;
    regs->gicc_regs[0].abpr = 0x00000003;
    regs->gicc_regs[0].idr = 0x3901243B;

    //cd
    regs->gicd_regs.ictr.all = 0x0000FC22;
    regs->gicd_regs.iidr = 0x0102043B;
    regs->gicd_regs.isenable[0] = 0x0000FFFF;
    regs->gicd_regs.icenable[0] = 0x0000FFFF;
    regs->gicd_regs.itargets[0] = 0x01010101;
    regs->gicd_regs.itargets[1] = 0x01010101;
    regs->gicd_regs.itargets[2] = 0x01010101;
    regs->gicd_regs.itargets[3] = 0x01010101;
    regs->gicd_regs.itargets[6] = 0x01000000;
    regs->gicd_regs.itargets[7] = 0x01010101;
    regs->gicd_regs.icfg[0] = 0xAAAAAAAA;
    regs->gicd_regs.icfg[1] = 0x7DC00000;
    regs->gicd_regs.icfg[2] = 0x55555555;
    regs->gicd_regs.icfg[3] = 0x55555555;
    regs->gicd_regs.icfg[4] = 0x55555555;
    regs->gicd_regs.icfg[5] = 0x55555555;
    regs->gicd_regs.icpidr[0] = 0x00000004;
    regs->gicd_regs.icpidr[4] = 0x00000090;
    regs->gicd_regs.icpidr[5] = 0x000000B3;
    regs->gicd_regs.icpidr[6] = 0x0000001B;
    regs->gicd_regs.iccidr[0] = 0x0000000D;
    regs->gicd_regs.iccidr[1] = 0x000000F0;
    regs->gicd_regs.iccidr[2] = 0x00000005;
    regs->gicd_regs.iccidr[3] = 0x000000B1;
    return dev->obj;
}

static exception_t free_zynq7000_gic(conf_object_t* opaque)
{
    zynq7000_gic_device *dev = (zynq7000_gic_device*)opaque->obj;
    skyeye_free(opaque->objname);
    skyeye_free(opaque->obj);
    skyeye_free(opaque);
    return No_exp;
}

static exception_t config_zynq7000_gic(conf_object_t* opaque)
{
    zynq7000_gic_device *dev = (zynq7000_gic_device *)opaque->obj;
    dev->regs.gicd_regs.ictr.bit.CPUNumber = dev->cpuNum;
    dev->regs.gicd_regs.ictr.bit.ITLinesNumber = 2;
    system_register_timer_handler(opaque, 10, (time_sched_t)gic_generateExceptions, SCHED_US|SCHED_NORMAL);
    return No_exp;
}

static void zynq7000_gic_register_attribute(conf_class_t* clss)
{
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    zynq7000_gic_device *dev = obj->obj;
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
    zynq7000_gic_device *dev = obj->obj;
    if (index < MAX_CPU_NUM)
        *obj2 = dev->cpu_list[index].signal;
    *port = NULL;
    return No_exp;
}

void init_zynq7000_gic(void)
{
    static skyeye_class_t class_data =
    {
        .class_name = "zynq7000_gic",
        .class_desc = "zynq7000_gic",
        .new_instance = new_zynq7000_gic,
        .free_instance = free_zynq7000_gic,
        .config_instance = config_zynq7000_gic
    };
    conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

    static memory_space_intf io_memory[] =
    {
        {
            .read  = gicc_registers_read,
            .write = gicc_registers_write
        },
        {
            .read  = gicd_registers_read,
            .write = gicd_registers_write
        }
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static skyeye_reg_intf reg_intf =
    {
        .get_regvalue_by_id = zynq7000_gic_get_regvalue_by_id,
        .get_regname_by_id = zynq7000_gic_get_regname_by_id,
        .set_regvalue_by_id = zynq7000_gic_set_regvalue_by_id,
        .get_regnum = zynq7000_gic_get_regnum,
        .get_regid_by_name = zynq7000_gic_get_regid_by_name,
        .get_regoffset_by_id = zynq7000_gic_get_regoffset_by_id
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static general_signal_intf signal_intf = {
        .raise_signal = peripheral_raise_signal,
        .lower_signal = peripheral_lower_signal,
    };
    SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_intf);

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
        }
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
    zynq7000_gic_register_attribute(clss);
}

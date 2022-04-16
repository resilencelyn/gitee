#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#include <skyeye_log.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>
#include <skyeye_uart_ops.h>
#include <skyeye_system.h>
#include <skyeye_api.h>
#define DEBUG
#include "p2020_pic.h"

static timer_func(void *obj, time_data_t * data) {
	p2020_pic_device *dev = ((conf_object_t *)obj)->obj;

    dev->regs.GTVPRA[0] =  dev->regs.GTVPRA[0] | VPR_ACTIVE_BIT_MASK;
    if (dev->core_signal[0].iface)
    {
       dev->core_signal[0].iface->signal(dev->core_signal[0].obj, NULL);
    }

    if (dev->core_signal[1].iface)
    {
       dev->core_signal[1].iface->signal(dev->core_signal[1].obj, NULL);
    }

	return 0;
}

static void resetRegister(conf_object_t *conf_obj)
{
    p2020_pic_device *dev = (p2020_pic_device *)conf_obj->obj;
    uint32_t index = 0;
    struct registers *regs = &(dev->regs);

    regs->BBR1 = 0x400301;
    regs->BBR2 = 1;

    for (index = 0; index < sizeof(regs->IPIDR) / 4; index++)
    {
        regs->IPIDR[index] = 0;
    }

    regs->CTPR = 0xf;
    regs->WHOAMI = 0;
    regs->IACK = 0;
    regs->EOI = 0;
    regs->FRR = 0x6b0002;
    regs->GCR = 0;
    regs->VIR = 0;
    regs->PIR = 0;

    for (index = 0; index < sizeof(regs->IPIVPR) / 4; index++)
    {
        regs->IPIVPR[index] = 0x80000000;
    }

    regs->SVR = 0xffff;
    regs->TFRR0 = 0;
    regs->TFRR1 = 0;

    for (index = 0; index < sizeof(regs->GTCCRA) / 4; index++)
    {
        regs->GTCCRA[index] = 0;
    }

    for (index = 0; index < sizeof(regs->GTBCRA) / 4; index++)
    {
        regs->GTBCRA[index] = 0x80000000;
    }

    for (index = 0; index < sizeof(regs->GTVPRA) / 4; index++)
    {
        regs->GTVPRA[index] = 0x80000000;
    }

    for (index = 0; index < sizeof(regs->GTDRA) / 4; index++)
    {
        regs->GTDRA[index] = 1;
    }

    regs->TCR0 = 0;
    regs->TCR1 = 0;
    regs->ERQSR = 0;
    regs->IRQSR0 = 0;
    regs->IRQSR1 = 0;
    regs->IRQSR2 = 0;
    regs->CISR0 = 0;
    regs->CISR1 = 0;
    regs->CISR2 = 0;

    for (index = 0; index < sizeof(regs->PMMR0) / 4; index++)
    {
        regs->PMMR0[index] = 0xffffffff;
    }

    for (index = 0; index < sizeof(regs->PMMR1) / 4; index++)
    {
        regs->PMMR1[index] = 0xffffffff;
    }

    for (index = 0; index < sizeof(regs->PMMR2) / 4; index++)
    {
        regs->PMMR2[index] = 0xffffffff;
    }

    for (index = 0; index < sizeof(regs->MSGR) / 4; index++)
    {
        regs->MSGR[index] = 0;
    }

    regs->MER = 0;
    regs->MSR = 0;

    for (index = 0; index < sizeof(regs->MSIR) / 4; index++)
    {
        regs->MSIR[index] = 0;
    }

    regs->MSISR = 0;
    regs->MSIIR = 0;

    for (index = 0; index < sizeof(regs->GTCCRB) / 4; index++)
    {
        regs->GTCCRB[index] = 0;
    }

    for (index = 0; index < sizeof(regs->GTBCRB) / 4; index++)
    {
        regs->GTBCRB[index] = 0x80000000;
    }

    for (index = 0; index < sizeof(regs->GTVPRB) / 4; index++)
    {
        regs->GTVPRB[index] = 0x80000000;
    }

    for (index = 0; index < sizeof(regs->GTDRB) / 4; index++)
    {
        regs->GTDRB[index] = 1;
    }

    for (index = 0; index < sizeof(regs->MSGRa) / 4; index++)
    {
        regs->MSGRa[index] = 0;
    }

    regs->MERa = 0;
    regs->MSRa = 0;

    for (index = 0; index < sizeof(regs->EIVPR) / 4; index++)
    {
        regs->EIVPR[index] = 0x80000000;
    }

    for (index = 0; index < sizeof(regs->EIDR) / 4; index++)
    {
        regs->EIDR[index] = 1;
    }

    for (index = 0; index < sizeof(regs->IIVPR) / 4; index++)
    {
        regs->IIVPR[index] = 0x80800000;
    }

    for (index = 0; index < sizeof(regs->IIDR) / 4; index++)

    {
        regs->IIDR[index] = 1;
    }

    for (index = 0; index < sizeof(regs->MIVPR) / 4; index++)
    {
        regs->MIVPR[index] = 0x80000000;
    }

    for (index = 0; index < sizeof(regs->MIDR) / 4; index++)
    {
        regs->MIDR[index] = 1;
    }

    for (index = 0; index < sizeof(regs->MSIVPR) / 4; index++)
    {
        regs->MSIVPR[index] = 0x80000000;
    }

    for (index = 0; index < sizeof(regs->MSIDR) / 4; index++)
    {
        regs->MSIDR[index] = 1;
    }

    regs->CPU[0].IPIDR[0] = 0;
    regs->CPU[0].IPIDR[1] = 0;
    regs->CPU[0].IPIDR[2] = 0;
    regs->CPU[0].IPIDR[3] = 0;
    regs->CPU[0].CTPR = 0xf;
    regs->CPU[0].WHOAMI = 0;
    regs->CPU[0].IACK = 0;
    regs->CPU[0].EOI = 0;
    regs->CPU[1].IPIDR[0] = 0;
    regs->CPU[1].IPIDR[1] = 0;
    regs->CPU[1].IPIDR[2] = 0;
    regs->CPU[1].IPIDR[3] = 0;
    regs->CPU[1].CTPR = 0xf;
    regs->CPU[1].WHOAMI = 0;
    regs->CPU[1].IACK = 0;
    regs->CPU[1].EOI = 0;
}

// 寄存器读取操作方法
static exception_t p2020_pic_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    p2020_pic_device *dev = (p2020_pic_device *)conf_obj->obj;

    /* IPIVPR */
    if (offset >= _IPIVPR0 && offset <= _IPIVPR3)
    {
        offset = (offset - _IPIVPR0) / Register_Address_Interval;

        *(uint32_t *)buf = dev->regs.IPIVPR[offset];

        return No_exp;
    }

    /* GTCCRA || GTBCRA || GTVPRA || GTDRA */
    if (offset >= _GTCCRA0 && offset <= _GTDRA3)
    {
        offset = (offset - _GTCCRA0) / Register_Address_Interval;
        uint32_t TargetChoice = offset % 4;
        offset /= 4;

        switch (TargetChoice)
        {
            case 0: // GTCCRA
                *(uint32_t *)buf = dev->regs.GTCCRA[offset];
                break;
            case 1: // GTBCRA
                *(uint32_t *)buf = dev->regs.GTBCRA[offset];
                break;
            case 2: // GTVPRA
                *(uint32_t *)buf = dev->regs.GTVPRA[offset];
                break;
            case 3: // GTDRA
                *(uint32_t *)buf = dev->regs.GTDRA[offset];
                break;
        }

        return No_exp;
    }

    /* MSGR */
    if (offset >= _MSGR0 && offset <= _MSGR3)
    {
        offset = (offset - _MSGR0) / Register_Address_Interval;

        *(uint32_t *)buf = dev->regs.MSGR[offset];

        return No_exp;
    }

    /* MSIR */
    if (offset >= _MSIR0 && offset <= _MSIR7)
    {
        offset = (offset - _MSIR0) / Register_Address_Interval;

        *(uint32_t *)buf = dev->regs.MSIR[offset];

        return No_exp;
    }

    /* GTCCRB || GTBCRB || GTVPRB || GTDRB */
    if (offset >= _GTCCRB0 && offset <= _GTDRB3)
    {
        offset = (offset - _GTCCRB0) / Register_Address_Interval;
        uint32_t TargetChoice = offset % 4;
        offset /= 4;

        switch (TargetChoice)
        {
            case 0: // GTCCRB
                *(uint32_t *)buf = dev->regs.GTCCRB[offset];
                break;
            case 1: // GTBCRB
                *(uint32_t *)buf = dev->regs.GTBCRB[offset];
                break;
            case 2: // GTVPRB
                *(uint32_t *)buf = dev->regs.GTVPRB[offset];
                break;
            case 3: // GTDRB
                *(uint32_t *)buf = dev->regs.GTDRB[offset];
                break;
        }

        return No_exp;
    }

    /* MSGRA */
    if (offset >= _MSGRA0 && offset <= _MSGRA3)
    {
        offset = (offset - _MSGRA0) / Register_Address_Interval;

        *(uint32_t *)buf = dev->regs.MSGRa[offset];

        return No_exp;
    }

    /* EIVPR || EIDR */
    if (offset >= _EIVPR0 && offset <= _EIDR11)
    {
        offset = (offset - _EIVPR0) / Register_Address_Interval;
        uint32_t TargetChoice = offset % 2;
        offset /= 2;

        switch (TargetChoice)
        {
            case 0: // EIVPR
                *(uint32_t *)buf = dev->regs.EIVPR[offset];
                break;
            case 1: // EIDR
                *(uint32_t *)buf = dev->regs.EIDR[offset];
                break;
        }

        return No_exp;
    }

    /* IIVPR || IIDR */
    if (offset >= _IIVPR0 && offset <= _IIDR63)
    {
        offset = (offset - _IIVPR0) / Register_Address_Interval;
        uint32_t TargetChoice = offset % 2;
        offset /= 2;

        switch (TargetChoice)
        {
            case 0: // IIVPR
                *(uint32_t *)buf = dev->regs.IIVPR[offset];
                break;
            case 1: // IIDR
                *(uint32_t *)buf = dev->regs.IIDR[offset];
                break;
        }

        return No_exp;
    }

    /* MIVPR || MIDR */
    if (offset >= _MIVPR0 && offset <= _MIDR7)
    {
        offset = (offset - _MIVPR0) / Register_Address_Interval;
        uint32_t TargetChoice = offset % 2;
        offset /= 2;

        switch (TargetChoice)
        {
            case 0: // MIVPR
                *(uint32_t *)buf = dev->regs.IIVPR[offset];
                break;
            case 1: // MIDR
                *(uint32_t *)buf = dev->regs.IIDR[offset];
                break;
        }

        return No_exp;
    }

    /* MSIVPR || MSIDR */
    if (offset >= _MSIVPR0 && offset <= _MSIDR7)
    {
        offset = (offset - _MSIVPR0) / Register_Address_Interval;
        uint32_t TargetChoice = offset % 2;
        offset /= 2;

        switch (TargetChoice)
        {
            case 0: // MSIVPR
                *(uint32_t *)buf = dev->regs.MSIVPR[offset];
                break;
            case 1: // MSIDR
                *(uint32_t *)buf = dev->regs.MSIDR[offset];
                break;
        }

        return No_exp;
    }

    /* CPU0 || CPU1 */
    if (offset >= _IPIDR_CPU00 && offset <= _EOI_CPU1)
    {
        offset -= _IPIDR_CPU00;
		uint32_t CoreID = offset / 0x1000;
		offset = (offset % 0x1000) / Register_Address_Interval;

        switch (offset)
		{
            case 0:
            case 1:
            case 2:
            case 3: // IPIDR
                *(uint32_t *)buf = dev->regs.CPU[CoreID].IPIDR[offset];
                break;
            case 4: // CTPR
                *(uint32_t *)buf = dev->regs.CPU[CoreID].CTPR;
                break;
            case 5: // WHOAMI
                *(uint32_t *)buf = dev->regs.CPU[CoreID].WHOAMI;
                break;
            case 6: // IACK
                if ((dev->regs.GTVPRA[0] & VPR_ACTIVE_BIT_MASK) == 0)
                {
                    if ((dev->regs.IACK & 0xffff) == 0)
                    {
                        *(uint32_t *)buf = dev->regs.IACK;
                        *(uint32_t *)buf |= dev->regs.SVR & 0xffff;
                    } else
                    {
                        *(uint32_t *)buf = dev->regs.IACK;
                    }
                } else
                {
                    dev->regs.CPU[CoreID].IACK = VECTOR_PIT_GROUPA_GPTIMER_0;
                    *(uint32_t *)buf = dev->regs.CPU[CoreID].IACK;
                    dev->regs.GTVPRA[0] =  dev->regs.GTVPRA[0] & (~VPR_ACTIVE_BIT_MASK);
                }
                break;
            case _EOI: // EOI
                *(uint32_t *)buf = dev->regs.CPU[CoreID].EOI;
                break;
            default:
                skyeye_log(Error_log, __FUNCTION__, "Cannot read the cpu private register at 0x%x in pic_p2020\n", offset);
                break;
		}

		return No_exp;
    }

    switch (offset)
    {
        case _BBR1:
            *(uint32_t *)buf = dev->regs.BBR1;
            break;
        case _BBR2:
            *(uint32_t *)buf = dev->regs.BBR2;
            break;
        case _IPIDR0:
            *(uint32_t *)buf = dev->regs.IPIDR[0];
            break;
        case _IPIDR1:
            *(uint32_t *)buf = dev->regs.IPIDR[1];
            break;
        case _IPIDR2:
            *(uint32_t *)buf = dev->regs.IPIDR[2];
            break;
        case _IPIDR3:
            *(uint32_t *)buf = dev->regs.IPIDR[3];
            break;
        case _CTPR:
            *(uint32_t *)buf = dev->regs.CTPR;
            break;
        case _WHOAMI:
            *(uint32_t *)buf = dev->regs.WHOAMI;
            break;
        case _IACK:
            if ((dev->regs.GTVPRA[0] & VPR_ACTIVE_BIT_MASK) == 0)
            {
                if ((dev->regs.IACK & 0xffff) == 0)
                {
                    *(uint32_t *)buf = dev->regs.IACK;
                    *(uint32_t *)buf |= dev->regs.SVR & 0xffff;
                } else
                {
                    *(uint32_t *)buf = dev->regs.IACK;
                }
			} else
            {
                dev->regs.IACK = VECTOR_PIT_GROUPA_GPTIMER_0;
				*(uint32_t *)buf = dev->regs.IACK;
                dev->regs.GTVPRA[0] =  dev->regs.GTVPRA[0] & (~VPR_ACTIVE_BIT_MASK);
			}
            break;
        case _EOI:
            *(uint32_t *)buf = dev->regs.EOI;
            break;
        case _FRR:
            *(uint32_t *)buf = dev->regs.FRR;
            break;
        case _GCR:
            *(uint32_t *)buf = dev->regs.GCR;
            break;
        case _VIR:
            *(uint32_t *)buf = dev->regs.VIR;
            break;
        case _PIR:
            *(uint32_t *)buf = dev->regs.PIR;
            break;
        case _SVR:
            *(uint32_t *)buf = dev->regs.SVR;
            break;
        case _TFRR0:
            *(uint32_t *)buf = dev->regs.TFRR0;
            break;
        case _TFRR1:
            *(uint32_t *)buf = dev->regs.TFRR1;
            break;
        case _TCR0:
            *(uint32_t *)buf = dev->regs.TCR0;
            break;
        case _TCR1:
            *(uint32_t *)buf = dev->regs.TCR1;
            break;
        case _ERQSR:
            *(uint32_t *)buf = dev->regs.ERQSR;
            break;
        case _IRQSR0:
            *(uint32_t *)buf = dev->regs.IRQSR0;
            break;
        case _IRQSR1:
            *(uint32_t *)buf = dev->regs.IRQSR1;
            break;
        case _IRQSR2:
            *(uint32_t *)buf = dev->regs.IRQSR2;
            break;
        case _CISR0:
            *(uint32_t *)buf = dev->regs.CISR0;
            break;
        case _CISR1:
            *(uint32_t *)buf = dev->regs.CISR1;
            break;
        case _CISR2:
            *(uint32_t *)buf = dev->regs.CISR2;
            break;
        case _PMMR00:
            *(uint32_t *)buf = dev->regs.PMMR0[0];
            break;
        case _PMMR01:
            *(uint32_t *)buf = dev->regs.PMMR0[1];
            break;
        case _PMMR02:
            *(uint32_t *)buf = dev->regs.PMMR0[2];
            break;
        case _PMMR03:
            *(uint32_t *)buf = dev->regs.PMMR0[3];
            break;
        case _PMMR10:
            *(uint32_t *)buf = dev->regs.PMMR1[0];
            break;
        case _PMMR11:
            *(uint32_t *)buf = dev->regs.PMMR1[1];
            break;
        case _PMMR12:
            *(uint32_t *)buf = dev->regs.PMMR1[2];
            break;
        case _PMMR13:
            *(uint32_t *)buf = dev->regs.PMMR1[3];
            break;
        case _PMMR20:
            *(uint32_t *)buf = dev->regs.PMMR2[0];
            break;
        case _PMMR21:
            *(uint32_t *)buf = dev->regs.PMMR2[1];
            break;
        case _PMMR22:
            *(uint32_t *)buf = dev->regs.PMMR2[2];
            break;
        case _PMMR23:
            *(uint32_t *)buf = dev->regs.PMMR2[3];
            break;
        case _MER:
            *(uint32_t *)buf = dev->regs.MER;
            break;
        case _MSR:
            *(uint32_t *)buf = dev->regs.MSR;
            break;
        case _MSISR:
            *(uint32_t *)buf = dev->regs.MSISR;
            break;
        case _MSIIR:
            *(uint32_t *)buf = dev->regs.MSIIR;
            break;
        case _MERA:
            *(uint32_t *)buf = dev->regs.MERa;
            break;
        case _MSRA:
            *(uint32_t *)buf = dev->regs.MSRa;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in p2020_pic\n", offset);
            break;
    }
    return No_exp;
}

// 寄存器写入操作方法
static exception_t p2020_pic_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    p2020_pic_device *dev = (p2020_pic_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;

    /* IPIVPR */
    if (offset >= _IPIVPR0 && offset <= _IPIVPR3)
    {
        offset = (offset - _IPIVPR0) / Register_Address_Interval;

        dev->regs.IPIVPR[offset] = val;

        return No_exp;
    }

    /* GTCCRA || GTBCRA || GTVPRA || GTDRA */
    if (offset >= _GTCCRA0 && offset <= _GTDRA3)
    {
        offset = (offset - _GTCCRA0) / Register_Address_Interval;
        uint32_t TargetChoice = offset % 4;
        offset /= 4;

        switch (TargetChoice)
        {
            case 0: // GTCCRA
                dev->regs.GTCCRA[offset] = val;
                break;
            case 1: // GTBCRA
                dev->regs.GTBCRA[offset] = val;
                break;
            case 2: // GTBCRA
                dev->regs.GTVPRA[offset] = val;
                if(offset == 0)
                {
                    val &= 0xffff;
                    if(val == VECTOR_PIT_GROUPA_GPTIMER_0)
                    {
                       system_register_timer_handler(dev->obj, TICKCLOCK_PERIOD, (time_sched_t)timer_func, SCHED_US|SCHED_NORMAL);
                    }
                }
                break;
            case 3: // GTDRA
                dev->regs.GTDRA[offset] = val;
                break;
        }

        return No_exp;
    }

    /* MSGR */
    if (offset >= _MSGR0 && offset <= _MSGR3)
    {
        offset = (offset - _MSGR0) / Register_Address_Interval;

        dev->regs.MSGR[offset] = val;

        return No_exp;
    }

    /* MSIR */
    if (offset >= _MSIR0 && offset <= _MSIR7)
    {
        offset = (offset - _MSIR0) / Register_Address_Interval;

        dev->regs.MSIR[offset] = val;

        return No_exp;
    }

    /* GTCCRB || GTBCRB || GTVPRB || GTDRB */
    if (offset >= _GTCCRB0 && offset <= _GTDRB3)
    {
        offset = (offset - _GTCCRB0) / Register_Address_Interval;
        uint32_t TargetChoice = offset % 4;
        offset /= 4;

        switch (TargetChoice)
        {
            case 0: // GTCCRB
                dev->regs.GTCCRB[offset] = val;
                break;
            case 1: // GTBCRB
                dev->regs.GTBCRB[offset] = val;
                break;
            case 2: // GTVPRB
                dev->regs.GTVPRB[offset] = val;
                break;
            case 3: // GTDRB
                dev->regs.GTDRB[offset] = val;
                break;
        }

        return No_exp;
    }

    /* MSGRA */
    if (offset >= _MSGRA0 && offset <= _MSGRA3)
    {
        offset = (offset - _MSGRA0) / Register_Address_Interval;

        dev->regs.MSGRa[offset] = val;

        return No_exp;
    }

    /* EIVPR || EIDR */
    if (offset >= _EIVPR0 && offset <= _EIDR11)
    {
        offset = (offset - _EIVPR0) / Register_Address_Interval;
        uint32_t TargetChoice = offset % 2;
        offset /= 2;

        switch (TargetChoice)
        {
            case 0: // EIVPR
                dev->regs.EIVPR[offset] = val;
                break;
            case 1: // EIDR
                dev->regs.EIDR[offset] = val;
                break;
        }

        return No_exp;
    }

    /* IIVPR || IIDR */
    if (offset >= _IIVPR0 && offset <= _IIDR63)
    {
        offset = (offset - _IIVPR0) / Register_Address_Interval;
        uint32_t TargetChoice = offset % 2;
        offset /= 2;

        switch (TargetChoice)
        {
            case 0: // IIVPR
                dev->regs.IIVPR[offset] = val;
                break;
            case 1: // IIDR
                dev->regs.IIDR[offset] = val;
                break;
        }

        return No_exp;
    }

    /* MIVPR || MIDR */
    if (offset >= _MIVPR0 && offset <= _MIDR7)
    {
        offset = (offset - _MIVPR0) / Register_Address_Interval;
        uint32_t TargetChoice = offset % 2;
        offset /= 2;

        switch (TargetChoice)
        {
            case 0: // IIVPR
                dev->regs.IIVPR[offset] = val;
                break;
            case 1: // IIDR
                dev->regs.IIDR[offset] = val;
                break;
        }

        return No_exp;
    }

    /* MSIVPR || MSIDR */
    if (offset >= _MSIVPR0 && offset <= _MSIDR7)
    {
        offset = (offset - _MSIVPR0) / Register_Address_Interval;
        uint32_t TargetChoice = offset % 2;
        offset /= 2;

        switch (TargetChoice)
        {
            case 0: // MSIVPR
                dev->regs.MSIVPR[offset] = val;
                break;
            case 1: // MSIDR
                dev->regs.MSIDR[offset] = val;
                break;
        }

        return No_exp;
    }

    /* CPU0 || CPU1 */
	if (offset >= _IPIDR_CPU00 && offset <= _EOI_CPU1)
	{
		offset -= _IPIDR_CPU00;
		uint32_t CoreID = offset / 0x1000;
		offset = (offset % 0x1000) / Register_Address_Interval;

		switch (offset)
		{
            case 0:
            case 1:
            case 2:
            case 3: // IPIDR
                dev->regs.CPU[CoreID].IPIDR[offset] = val;
                if (val & P0)
                {
                    if (dev->core_signal[0].iface)
                    {
                       dev->core_signal[0].iface->signal(dev->core_signal[0].obj, NULL);
                    }
                }
                if (val & P1)
                {
                    if (dev->core_signal[1].iface)
                    {

                       dev->core_signal[1].iface->signal(dev->core_signal[1].obj, NULL);
                    }
                }
                break;
            case 4: // CTPR
                dev->regs.CPU[CoreID].CTPR = val;
                break;
            case 5: // WHOAMI
                dev->regs.CPU[CoreID].WHOAMI = val;
                break;
            case 6: // IACK
                dev->regs.CPU[CoreID].IACK = val;
                break;
            case 7: // EOI
                dev->regs.CPU[CoreID].EOI = val;
                if (val == 0)
                {
                    dev->regs.CPU[CoreID].IACK = dev->regs.SVR;
                }
                break;
            default:
                skyeye_log(Error_log, __FUNCTION__, "Cannot write the cpu private register at 0x%x in pic_p2020\n", offset);
                break;
        }

        return No_exp;
	}

    switch (offset)
    {
        case _BBR1:
            dev->regs.BBR1 = val;
            break;
        case _BBR2:
            dev->regs.BBR2 = val;
            break;
        case _IPIDR0:
            dev->regs.IPIDR[0] = val;
            break;
        case _IPIDR1:
            dev->regs.IPIDR[1] = val;
            break;
        case _IPIDR2:
            dev->regs.IPIDR[2] = val;
            break;
        case _IPIDR3:
            dev->regs.IPIDR[3] = val;
            break;
        case _CTPR:
            dev->regs.CTPR = val;
            break;
        case _WHOAMI:
            dev->regs.WHOAMI = val;
            break;
        case _IACK:
            dev->regs.IACK = val;
            break;
        case _EOI:
            dev->regs.EOI = val;
            if (val == 0)
            {
                dev->regs.IACK = dev->regs.SVR;
            }
            break;
        case _FRR:
            dev->regs.FRR = val;
            break;
        case _GCR:
            if (val & PIC_RESET_BIT)
            {
                resetRegister(conf_obj);
                dev->regs.GCR = val;
                dev->regs.GCR &= ~PIC_RESET_BIT;
            }
            break;
        case _VIR:
            dev->regs.VIR = val;
            break;
        case _PIR:
            dev->regs.PIR = val;
            break;
        case _SVR:
            dev->regs.SVR = val;
            break;
        case _TFRR0:
            dev->regs.TFRR0 = val;
            break;
        case _TFRR1:
            dev->regs.TFRR1 = val;
            break;
        case _TCR0:
            dev->regs.TCR0 = val;
            break;
        case _TCR1:
            dev->regs.TCR1 = val;
            break;
        case _ERQSR:
            dev->regs.ERQSR = val;
            break;
        case _IRQSR0:
            dev->regs.IRQSR0 = val;
            break;
        case _IRQSR1:
            dev->regs.IRQSR1 = val;
            break;
        case _IRQSR2:
            dev->regs.IRQSR2 = val;
            break;
        case _CISR0:
            dev->regs.CISR0 = val;
            break;
        case _CISR1:
            dev->regs.CISR1 = val;
            break;
        case _CISR2:
            dev->regs.CISR2 = val;
            break;
        case _PMMR00:
            dev->regs.PMMR0[0] = val;
            break;
        case _PMMR01:
            dev->regs.PMMR0[1] = val;
            break;
        case _PMMR02:
            dev->regs.PMMR0[2] = val;
            break;
        case _PMMR03:
            dev->regs.PMMR0[3] = val;
            break;
        case _PMMR10:
            dev->regs.PMMR1[0] = val;
            break;
        case _PMMR11:
            dev->regs.PMMR1[1] = val;
            break;
        case _PMMR12:
            dev->regs.PMMR1[2] = val;
            break;
        case _PMMR13:
            dev->regs.PMMR1[3] = val;
            break;
        case _PMMR20:
            dev->regs.PMMR2[0] = val;
            break;
        case _PMMR21:
            dev->regs.PMMR2[1] = val;
            break;
        case _PMMR22:
            dev->regs.PMMR2[2] = val;
            break;
        case _PMMR23:
            dev->regs.PMMR2[3] = val;
            break;
        case _MER:
            dev->regs.MER = val;
            break;
        case _MSR:
            dev->regs.MSR = val;
            break;
        case _MSISR:
            dev->regs.MSISR = val;
            break;
        case _MSIIR:
            dev->regs.MSIIR = val;
            break;
        case _MERA:
            dev->regs.MERa = val;
            break;
        case _MSRA:
            dev->regs.MSRa = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in p2020_pic\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* p2020_pic_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t p2020_pic_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    p2020_pic_device *dev = (p2020_pic_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t p2020_pic_get_regnum(conf_object_t *conf_obj)
{
    return sizeof(struct registers) / 4;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t p2020_pic_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    p2020_pic_device *dev = (p2020_pic_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t p2020_pic_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
    uint32_t regnum = sizeof(struct registers) / 4;
    int i;

    for (i = 0; i < regnum; i++)
    {
        if (strcmp(name, regs_name[i]) == 0)
            return i;
    }

    return regnum;
}

// 寄存器接口:根据寄存器ID获取寄存器偏移
static uint32_t p2020_pic_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 中断信号:上升沿
static int p2020_pic_raise_signal(conf_object_t *conf_obj, int line)
{
    p2020_pic_device *dev = (p2020_pic_device *)conf_obj->obj;

    dev->regs.IACK = line;
    dev->core_signal[0].iface->signal(dev->core_signal[0].obj, NULL);

    return No_exp;
}

// 中断信号:下降沿
static int p2020_pic_lower_signal(conf_object_t *conf_obj, int line)
{
    p2020_pic_device *dev = (p2020_pic_device *)conf_obj->obj;

    return No_exp;
}

// 中断信号:事件
static int p2020_pic_event_signal(conf_object_t *conf_obj, int line, void *args)
{
    p2020_pic_device *dev = (p2020_pic_device *)conf_obj->obj;

    return No_exp;
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_p2020_pic(const char *obj_name)
{
    p2020_pic_device* dev = skyeye_mm_zero(sizeof(p2020_pic_device));
    dev->obj = new_conf_object(obj_name, dev);
    struct registers *regs = &(dev->regs);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    resetRegister(dev->obj);

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_p2020_pic(conf_object_t* conf_obj)
{
    p2020_pic_device *dev = (p2020_pic_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_p2020_pic(conf_object_t* conf_obj)
{
    p2020_pic_device *dev = (p2020_pic_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 系统设置设备 core_signal 接口功能
static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    p2020_pic_device *dev = (p2020_pic_device *)obj->obj;

    dev->core_signal[index].obj = obj2;
    if (obj2 == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", CORE_SIGNAL_INTF_NAME);
        return Not_found_exp;
    }
    dev->core_signal[index].iface = (core_signal_intf *)SKY_get_iface(obj2, CORE_SIGNAL_INTF_NAME);
    if (dev->core_signal[index].iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", CORE_SIGNAL_INTF_NAME, obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

// 系统获取设备 core_signal 接口功能
static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    p2020_pic_device *dev = (p2020_pic_device *)obj->obj;

    *obj2 = dev->core_signal[index].obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 memory_space 接口功能
static exception_t memory_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    p2020_pic_device *dev = (p2020_pic_device *)obj->obj;

    dev->memory_space.obj = obj2;
    if (obj2 == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", MEMORY_SPACE_INTF_NAME);
        return Not_found_exp;
    }
    dev->memory_space.iface = (memory_space_iface_t *)SKY_get_iface(obj2, MEMORY_SPACE_INTF_NAME);
    if (dev->memory_space.iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", MEMORY_SPACE_INTF_NAME, obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

// 系统获取设备 memory_space 接口功能
static exception_t memory_space_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    p2020_pic_device *dev = (p2020_pic_device *)obj->obj;

    *obj2 = dev->memory_space.obj;
    *port = NULL;

    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_p2020_pic(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "p2020_pic",
        .class_desc      = "p2020_pic",
        .new_instance    = new_p2020_pic,
        .free_instance   = free_p2020_pic,
        .config_instance = config_p2020_pic,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = p2020_pic_read,
        .write = p2020_pic_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = p2020_pic_get_regvalue_by_id,
        .get_regname_by_id   = p2020_pic_get_regname_by_id,
        .set_regvalue_by_id  = p2020_pic_set_regvalue_by_id,
        .get_regnum          = p2020_pic_get_regnum,
        .get_regid_by_name   = p2020_pic_get_regid_by_name,
        .get_regoffset_by_id = p2020_pic_get_regoffset_by_id
    };
    SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);

    // 中断管理器中断信号接口
    static const general_signal_iface_t general_signal_interface =
    {
        .raise_signal = p2020_pic_raise_signal,
        .lower_signal = p2020_pic_lower_signal,
        .event_signal = p2020_pic_event_signal
    };
    SKY_register_iface(dev_class, GENERAL_SIGNAL_INTF_NAME, &general_signal_interface);

    // 接口方法列表
    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name  = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory_interface
        },
        (struct InterfaceDescription)
        {
            .name  = SKYEYE_REG_INTF,
            .iface = &reg_interface
        },
        (struct InterfaceDescription)
        {
            .name  = GENERAL_SIGNAL_INTF_NAME,
            .iface = &general_signal_interface
        },
    };

    // 接口连接方法列表
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription)
        {
            .name = CORE_SIGNAL_INTF_NAME,
            .set  = core_signal_set,
            .get  = core_signal_get
        },
        (struct ConnectDescription)
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .set  = memory_space_set,
            .get  = memory_space_get
        },
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
}

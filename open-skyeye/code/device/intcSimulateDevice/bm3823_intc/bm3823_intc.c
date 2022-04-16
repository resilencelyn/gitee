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
#include <skyeye_system.h>
#include <skyeye_api.h>
#define DEBUG
#include "bm3823_intc.h"

void signal_interrupt_to_cpu(conf_object_t *opaque)
{
    bm3823_intc_device *dev = opaque->obj;
    int irq_num = 0;

    if (CheckIntcReadNum(dev->fifo) == 0)
    {
       return;
    }

    RW_RDLOCK(dev->lock);
    ReadFIFO(dev->fifo, &irq_num, 1);
    RW_UNLOCK(dev->lock);

    interrupt_signal_t interrupt_signal;
    interrupt_signal.sparc_signal.irq_no = irq_num;
    interrupt_signal.sparc_signal.level = High_level;

    if (IFORCE(irq_num) || IPEND(irq_num))
    {
        CLR_PEND(irq_num);
        CLR_FORCE(irq_num);
    }
    if (dev->signal.intf)
    {
       dev->signal.intf->signal(dev->signal.obj, &interrupt_signal);
    }
}

void select_send_interrupt(conf_object_t *opaque)
{
    bm3823_intc_device *dev = opaque->obj;
    int i = 15;

    for (; i >= 0; i--)
    {
        if (IPEND(i) || IFORCE(i))
        {
            if (dev->regs.irq1mask_pri.ilevel != 0)
            {
                if (ILEVEL(i))
                {
                    /*insert high level interrupt number to fifo*/
                    WriteFIFO(dev->fifo, &i, 1);
                    continue;
                }
            } else
            {
                /*insert lower level interrupt number to fifo*/
                WriteFIFO(dev->fifo, &i, 1);
                continue;
            }
        }
    }
}

void interrupt_request_handle(conf_object_t *opaque, int irq_num)
{
    bm3823_intc_device *dev = opaque->obj;

    if (IMASK(irq_num))
    {
        SET_PEND(irq_num);
        SET_FORCE(irq_num);
        select_send_interrupt(opaque);
    }
}

static exception_t bm3823_intc_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    uint32_t val = 0;
    bm3823_intc_device *dev = opaque->obj;

    switch (offset)
    {   case CACHECTRL:
            *(uint32_t *)buf = dev->regs.cachectrl.all;
            break;
        case IRQ1MASK_PRI:
            *(uint32_t *)buf = dev->regs.irq1mask_pri.all;
            break;
        case IRQ1FORCE_PEND:
            *(uint32_t *)buf = dev->regs.irq1force_pend.all;
            break;
        case IRQ1SUBPRI:
            *(uint32_t *)buf = dev->regs.irq1subpri.all;
            break;
        case IRQ1CLEAR:
            *(uint32_t *)buf = dev->regs.irq1clear.all;
            break;
        case IRQ2MASK:
            *(uint32_t *)buf = dev->regs.irq2mask.all;
            break;
        case IRQ2PEND:
            *(uint32_t *)buf = dev->regs.irq2pend.all;
            break;
        case IRQ2STATE_CLEAR:
            *(uint32_t *)buf = dev->regs.irq2state_clear.all;
            break;
        case EXIRQCONFIG:
            *(uint32_t *)buf = dev->regs.exirqconfig;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in intr_memory_write\n", offset);
            return Not_found_exp;
    }

    /*printf("%s offset:0x%x, val:0x%x\n", __FUNCTION__, offset, *(uint32_t *)buf);*/

    return No_exp;
}

static exception_t bm3823_intc_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    bm3823_intc_device *dev = opaque->obj;
    uint32_t val = *(uint32_t *)buf;

    /*printf("%s offset:0x%x, val:0x%x\n", __FUNCTION__, offset, val);*/

    switch (offset)
    {
        case CACHECTRL:
            dev->regs.cachectrl.all = val;
            break;
        case IRQ1MASK_PRI:
            dev->regs.irq1mask_pri.all = val;
            break;
        case IRQ1FORCE_PEND:
            dev->regs.irq1force_pend.all = val;
            break;
        case IRQ1SUBPRI:
            dev->regs.irq1subpri.all = val;
            break;
        case IRQ1CLEAR:
            dev->regs. irq1clear.all = val;
            break;
        case IRQ2MASK:
            dev->regs.irq2mask.all = val;
            break;
        case IRQ2PEND:
            dev->regs.irq2pend.all = val;
            break;
        case IRQ2STATE_CLEAR:
            dev->regs.irq2state_clear.all = val;
            break;
        case EXIRQCONFIG:
            dev->regs.exirqconfig = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in bm3823_intc\n", offset);
            break;
    }

    return No_exp;
}

static char* bm3823_intc_get_regname_by_id(conf_object_t *opaque, uint32_t id)
{
    return regs_name[id];
}

static exception_t bm3823_intc_set_regvalue_by_id(conf_object_t *opaque, uint32_t value, uint32_t id)
{
    bm3823_intc_device *dev = opaque->obj;
    struct registers *regs = &(dev->regs);
    *((uint32_t*)regs + id) = value;

    return No_exp;
}

static uint32_t bm3823_intc_get_regnum(conf_object_t *opaque)
{
    return sizeof(struct registers) / 4;
}

static uint32_t bm3823_intc_get_regvalue_by_id(conf_object_t *opaque, uint32_t id)
{
    bm3823_intc_device *dev = opaque->obj;
    struct registers *regs = &(dev->regs);
    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

static uint32_t bm3823_intc_get_regid_by_name(conf_object_t *opaque, char *name)
{
    uint32_t regnum = sizeof(struct registers) / 4, i;
    for (i = 0; i < regnum; i++)
    {
        if (strcmp(name, regs_name[i]) == 0)
        {
          return i;
        }
    }
    return regnum;
}

static system_control_register_init(bm3823_intc_device* dev)
{
    memset(&dev->regs, 0, sizeof(struct registers));

    dev->regs.cachectrl.dsts = 0x1;
    dev->regs.cachectrl.ists = 0x3;
    dev->regs.cachectrl.irpl = 0x3;
    dev->regs.cachectrl.drpl = 0x3;

}

static conf_object_t* new_bm3823_intc(const char *obj_name)
{
    bm3823_intc_device* dev = skyeye_mm_zero(sizeof(bm3823_intc_device));
    dev->obj = new_conf_object(obj_name, dev);
    dev->fifo = CreateFIFO(MAX_FIFO_NUM);
    struct registers *regs = &(dev->regs);
    system_control_register_init(dev);
    RWLOCK_INIT(dev->lock);

    return dev->obj;
}

static exception_t config_bm3823_intc(conf_object_t* opaque)
{
    bm3823_intc_device *dev = opaque->obj;
    system_register_timer_handler(opaque, 1000, (time_sched_t)signal_interrupt_to_cpu, SCHED_US|SCHED_NORMAL);

    return No_exp;
}

static exception_t free_bm3823_intc(conf_object_t* opaque)
{
    bm3823_intc_device *dev = opaque->obj;
    skyeye_free(opaque->objname);
    skyeye_free(opaque->obj);
    skyeye_free(opaque);

    return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    bm3823_intc_device *dev = obj->obj;
    dev->signal.obj = obj2;
    dev->signal.intf = (core_signal_intf *)SKY_get_iface(dev->signal.obj, CORE_SIGNAL_INTF_NAME);

    if(dev->signal.intf == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from %s fail.", obj2->objname);
    }

    return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    bm3823_intc_device *dev = obj->obj;
    *obj2 = dev->signal.obj;
    *port = NULL;

    return No_exp;
}

void init_bm3823_intc(void)
{
    static skyeye_class_t class_data =
    {
        .class_name      = "bm3823_intc",
        .class_desc      = "bm3823_intc",
        .new_instance    = new_bm3823_intc,
        .free_instance   = free_bm3823_intc,
        .config_instance = config_bm3823_intc,
    };
    conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);

    static memory_space_intf io_memory_interface =
    {
        .read  = bm3823_intc_read,
        .write = bm3823_intc_write
    };
    SKY_register_iface(class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id = bm3823_intc_get_regvalue_by_id,
        .get_regname_by_id  = bm3823_intc_get_regname_by_id,
        .set_regvalue_by_id = bm3823_intc_set_regvalue_by_id,
        .get_regnum         = bm3823_intc_get_regnum,
        .get_regid_by_name  = bm3823_intc_get_regid_by_name
    };
    SKY_register_iface(class, SKYEYE_REG_INTF, &reg_interface);

    static const general_signal_iface_t signal_iface = {
        .raise_signal = interrupt_request_handle,
    };
    SKY_register_iface(class, GENERAL_SIGNAL_INTF_NAME, &signal_iface);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription) {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .iface = &signal_iface,
        },
        (struct InterfaceDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory_interface,
        },
        (struct InterfaceDescription) {
            .name = SKYEYE_REG_INTF,
            .iface = &reg_interface,
        },
    };

    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription ){
            .name = CORE_SIGNAL_INTF_NAME,
            .set = core_signal_set,
            .get = core_signal_get,
        },
    };

    class_register_ifaces(class, ifaces);
    class_register_connects(class, connects);
}

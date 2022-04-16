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
#include <skyeye_system.h>
#define DEBUG
#include <skyeye_log.h>
#include "bm3823_system_control.h"

static exception_t bm3823_system_control_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    bm3823_system_control_device *dev = opaque->obj;
    uint32_t val = 0;
    /*Timer Registers Memory Write*/
    if (offset >= TCNT1  && offset <= SRLD)
    {
        timer_memory_read(opaque, offset, &val, count);
        *(uint32_t *)buf = val;
        /*printf("%s offset:0x%x, val:0x%x\n", __FUNCTION__, offset, *(uint32_t *)buf);*/
        return No_exp;
    }

    /*Uart Registers Memory Write*/
    if (offset >= UDATA1 && offset <= USCAL2)
    {
        uart_memory_read(opaque, offset, &val, count);
        *(uint32_t *)buf = val;
        /*printf("%s offset:0x%x, val:0x%x\n", __FUNCTION__, offset, *(uint32_t *)buf);*/
        return No_exp;
    }

    switch (offset)
    {
        case MCFG1:
            *(uint32_t *)buf = dev->regs.memory_reg.mcfg1.all;
            break;
        case MCFG2:
            *(uint32_t *)buf = dev->regs.memory_reg.mcfg2.all;
            break;
        case MCFG3:
            *(uint32_t *)buf = dev->regs.memory_reg.mcfg3.all;
            break;
        case FADDR:
             *(uint32_t *)buf = dev->regs.memory_reg.failar;
             break;
        case MSTAT:
             *(uint32_t *)buf = dev->regs.memory_reg.memstatus.all;
             break;
        case WPROT1:
            *(uint32_t *)buf = dev->regs.memory_reg.wpr1.all;
             break;
        case WPROT2:
             *(uint32_t *)buf = dev->regs.memory_reg.wpr2.all;
             break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in bm3823_system_control\n", offset);
            return Not_found_exp;
    }
    /*printf("%s offset:0x%x, val:0x%x\n", __FUNCTION__, offset, *(uint32_t *)buf);*/
    return No_exp;
}

static exception_t bm3823_system_control_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    bm3823_system_control_device *dev = opaque->obj;
    uint32_t val = *(uint32_t *)buf;
    /*printf("%s offset:0x%x, val:0x%x\n", __FUNCTION__, offset, val);*/
    /*Timer Registers Memory Write*/
    if (offset >= TCNT1  && offset <= SRLD)
    {
        timer_memory_write(opaque, offset, val, count);
        return No_exp;
    }

    /*Uart Registers Memory Write*/
    if (offset >= UDATA1 && offset <= USCAL2)
    {
        uart_memory_write(opaque, offset, val, count);
        return No_exp;
    }

    switch (offset)
    {
        case MCFG1:
            dev->regs.memory_reg.mcfg1.all = val;
            break;
        case MCFG2:
            dev->regs.memory_reg.mcfg2.all = val;
            break;
        case MCFG3:
            dev->regs.memory_reg.mcfg3.all = val;
            break;
        case FADDR:
            dev->regs.memory_reg.failar = val;
            break;
        case MSTAT:
            dev->regs.memory_reg.memstatus.all = val;
            break;
        case WPROT1:
            dev->regs.memory_reg.wpr1.all = val;
            break;
        case WPROT2:
            dev->regs.memory_reg.wpr2.all = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in bm3823_system_control\n", offset);
            return Not_found_exp;
    }

    return No_exp;
}

static char* bm3823_system_control_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

static exception_t bm3823_system_control_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    bm3823_system_control_device *dev = conf_obj->obj;
    struct registers *regs = &(dev->regs);
    *((uint32_t*)regs + id) = value;
    return No_exp;
}

static uint32_t bm3823_system_control_get_regnum(conf_object_t *conf_obj)
{
    return  sizeof(struct registers) / 4;
}

static uint32_t bm3823_system_control_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    bm3823_system_control_device *dev = conf_obj->obj;
    struct registers *regs = &(dev->regs);
    uint32_t* regs_value = (uint32_t*)regs + id;
    return *regs_value;
}

static uint32_t bm3823_system_control_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
    uint32_t regnum = sizeof(struct registers) / 4, i;
    for(i = 0; i < regnum; i++)
    {
        if(strcmp(name, regs_name[i]) == 0)
        {
            return i;
        }
    }
    return regnum;
}

static uint32_t bm3823_system_control_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
    return regs_offset[id];
}

static system_control_register_init(bm3823_system_control_device *dev)
{

     /*Memory Interface Register Init*/
      memset(&dev->regs.memory_reg, 0, sizeof(memory_reg_t));
      dev->regs.memory_reg.mcfg1.prrws  = 0x1f;
      dev->regs.memory_reg.mcfg1.prwws  = 0x1f;
      dev->regs.memory_reg.mcfg1.Iows   = 0x1f;
      dev->regs.memory_reg.mcfg2.srrws  = 0xf;
      dev->regs.memory_reg.mcfg2.srwws  = 0xf;
      dev->regs.memory_reg.mcfg2.sdrcls = 0x2;
      dev->regs.memory_reg.mcfg2.sdrcas = 0x1;
      dev->regs.memory_reg.mcfg2.trfc   = 0x7;
      dev->regs.memory_reg.mcfg2.trp    = 0x1;

      /*Timer Register Init*/
      memset(&dev->regs.timer_reg, 0, sizeof(timer_reg_t));
      dev->regs.timer_reg.timercnt1 = 0xffffffff;
      dev->regs.timer_reg.timercnt2 = 0xffffffff;
      dev->regs.timer_reg.wdog = 0xffffffff;

       /*UART Register Init*/
      memset(&dev->regs.uart_reg, 0, sizeof(uart_reg_t));
      dev->regs.uart_reg.uartstatus1.ts = 1;
      dev->regs.uart_reg.uartstatus1.th = 1;
      dev->regs.uart_reg.uartstatus2.ts = 1;
      dev->regs.uart_reg.uartstatus2.th = 1;
}

static conf_object_t* new_bm3823_system_control(const char *obj_name)
{
    bm3823_system_control_device* dev = skyeye_mm_zero(sizeof(bm3823_system_control_device));
    dev->obj = new_conf_object(obj_name, dev);
    struct registers *regs = &(dev->regs);

    system_control_register_init(dev);

    RWLOCK_INIT(dev->lock);
    dev->cpu_freq = 25; /*25Mhz*/
    dev->baudrate = 119200;
    dev->timer = 1500;
    dev->timer1_handle = -1;
    dev->timer2_handle = -1;

    return dev->obj;
}

static exception_t free_bm3823_system_control(conf_object_t* opaque)
{
    bm3823_system_control_device *dev = opaque->obj;

    skyeye_free(dev->obj);
    skyeye_free(dev);
    return No_exp;
}

static exception_t config_bm3823_system_control(conf_object_t* opaque)
{
    bm3823_system_control_device *dev = opaque->obj;
    double cpu_cycle = 1 / (double)dev->cpu_freq;
    int period;

    if (cpu_cycle * 1000 > 0)
    {
        period = (int)(cpu_cycle * 1000);
    } else
    {
        period = 1;
    }

    dev->uart1_scaler = dev->uart2_scaler = ((((dev->cpu_freq * 1000000 * 10) / (8 * dev->baudrate)) - 5))/10;
    system_register_timer_handler(opaque, 1000, (time_sched_t)uart_read_handler, SCHED_US|SCHED_NORMAL);
    system_register_timer_handler(opaque, period, (time_sched_t)uart_tick_handler, SCHED_NS|SCHED_NORMAL);
    system_register_timer_handler(opaque, 2000, (time_sched_t)uart_interrupt_handler, SCHED_US|SCHED_NORMAL);
    return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    bm3823_system_control_device *dev =  obj->obj;
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
    bm3823_system_control_device *dev =  obj->obj;
    *obj2 = dev->signal.obj;
    *port = NULL;
    return No_exp;
}
static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    bm3823_system_control_device *dev =  obj->obj;
    dev->signal.obj = obj2;
    dev->signal.iface = (general_signal_intf *)SKY_get_iface(dev->signal.obj, GENERAL_SIGNAL_INTF_NAME);

    if (dev->signal.iface == NULL)
    {
        if (dev->signal.obj == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
        } else
        {
          skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);
        }
        return Not_found_exp;
    }
    return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    bm3823_system_control_device *dev =  obj->obj;
    *obj2 = dev->signal.obj;
    *port = NULL;
    return No_exp;
}

static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    bm3823_system_control_device *dev =  obj->obj;

    if (index == 1)
    {
        dev->term1.obj = obj2;
        dev->term1.intf = (skyeye_uart_intf *)SKY_get_iface(dev->term1.obj, SKYEYE_UART_INTF);
        if(dev->term1.intf == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from %s fail.", obj2->objname);
        }
    } else if (index == 2)
    {
        dev->term2.obj = obj2;
        dev->term2.intf = (skyeye_uart_intf *)SKY_get_iface(dev->term2.obj, SKYEYE_UART_INTF);
        if(dev->term2.intf == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from %s fail.", obj2->objname);
        }
    }

    return No_exp;
}

static exception_t uart_term_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    bm3823_system_control_device *dev =  obj->obj;
    *obj2 = dev->term1.obj;
    *port = NULL;
    return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    bm3823_system_control_device *dev =  obj->obj;
    dev->uart_file.obj = obj2;
    dev->uart_file.intf = (uart_file_intf*)SKY_get_iface(dev->uart_file.obj, UART_FILE_INTF);

    if (dev->uart_file.intf == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'skyeye_uart_intf' interface from %s fail.", obj2->objname);
        return Not_found_exp;
    }
    return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    bm3823_system_control_device *dev =  obj->obj;
    *obj2 = dev->uart_file.obj;
    *port = NULL;
    return No_exp;
}

static attr_value_t get_attr_cpu_freq(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    bm3823_system_control_device *dev =  conf_obj->obj;

    attr_value_t cpu_freq = SKY_make_attr_uinteger(dev->cpu_freq);
    return cpu_freq;
}

static exception_t set_attr_cpu_freq(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    bm3823_system_control_device *dev =  conf_obj->obj;

    /*MHZ*/
    dev->cpu_freq = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_timer(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    bm3823_system_control_device *dev =  conf_obj->obj;

    attr_value_t timer = SKY_make_attr_uinteger(dev->timer);
    return timer;
}

static exception_t set_attr_timer(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    bm3823_system_control_device *dev =  conf_obj->obj;

    /*MHZ*/
    dev->timer = SKY_attr_uinteger(*value);
    return No_exp;
}


static attr_value_t get_attr_uart_baudrate(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    bm3823_system_control_device *dev =  conf_obj->obj;

    attr_value_t baudrate = SKY_make_attr_uinteger(dev->baudrate);
    return baudrate;
}

static exception_t set_attr_uart_baudrate(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    bm3823_system_control_device *dev =  conf_obj->obj;

    dev->baudrate = SKY_attr_uinteger(*value);
    return No_exp;
}

static void class_register_attribute(conf_class_t* clss){
    SKY_register_attribute(clss, "cpu_freq", get_attr_cpu_freq, NULL, set_attr_cpu_freq, NULL, SKY_Attr_Optional, "uinteger", "set bm3823 cpu freq.");
    SKY_register_attribute(clss, "timer", get_attr_timer, NULL, set_attr_timer, NULL, SKY_Attr_Optional, "uinteger", "set bm3823 timer to trigger interrupt.");
    SKY_register_attribute(clss, "uart_baudrate", get_attr_uart_baudrate, NULL, set_attr_uart_baudrate, NULL, SKY_Attr_Optional, "uinteger", "set bm3823 uart baudrate.");
    return;
}

void init_bm3823_system_control(void)
{
    static skyeye_class_t class_data =
    {
        .class_name = "bm3823_system_control",
        .class_desc = "bm3823_system_control",
        .new_instance = new_bm3823_system_control,
        .free_instance = free_bm3823_system_control,
        .config_instance = config_bm3823_system_control
    };
    conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

    static memory_space_intf io_memory =
    {
        .read = bm3823_system_control_read,
        .write = bm3823_system_control_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static skyeye_reg_intf reg_intf =
    {
        .get_regvalue_by_id = bm3823_system_control_get_regvalue_by_id,
        .get_regname_by_id = bm3823_system_control_get_regname_by_id,
        .set_regvalue_by_id = bm3823_system_control_set_regvalue_by_id,
        .get_regnum = bm3823_system_control_get_regnum,
        .get_regid_by_name = bm3823_system_control_get_regid_by_name,
        .get_regoffset_by_id = bm3823_system_control_get_regoffset_by_id
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static const general_signal_iface_t signal_iface = {
    };
    SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_iface);

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
        (struct InterfaceDescription) {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .iface = &signal_iface,
        },
    };

    static const struct ConnectDescription connects[] =
    {
        /* ConnectDescription */
        (struct ConnectDescription ){
            .name = SKYEYE_UART_INTF,
            .set = uart_term_set,
            .get = uart_term_get,
        },
        (struct ConnectDescription ){
            .name = UART_FILE_INTF,
            .set = uart_file_set,
            .get = uart_file_get,
        },
        (struct ConnectDescription ){
            .name = CORE_SIGNAL_INTF_NAME,
            .set = core_signal_set,
            .get = core_signal_get,
        },
        (struct ConnectDescription) {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set = general_signal_set,
            .get = general_signal_get,
        }
    };

    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);
    class_register_attribute(clss);
}

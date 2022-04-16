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
#include "mpc8272_processor_comm.h"

/*支持FCC、SCC、SMC、SPI、I2C*/
void init_rx_tx_params(mpc8272_processor_comm_device *dev, uint8_t page, uint8_t sbc_code)
{
    uint8_t sub_block = sbc_code;

    /*sub block表示当前操作的是哪个设备*/
    switch(sub_block)
    {
        case FCC1_SBC_CODE:
            break;
        case SCC1_SBC_CODE:
            break;
        case SCC3_SBC_CODE:
            break;
        case SCC4_SBC_CODE:
            break;
        case SMC1_SBC_CODE:
            break;
        case SMC2_SBC_CODE:
            break;
        case SPI_SBC_CODE:
            break;
        case I2C_SBC_CODE:
            break;
        default:
            printf("In %s ,sub block code %#x is not exist\n", __func__, sub_block);
            break;
    }

    return;
}

void init_rx_params(mpc8272_processor_comm_device *dev, uint8_t page, uint8_t sbc_code)
{
    return;
}

void init_tx_params(mpc8272_processor_comm_device *dev, uint8_t page, uint8_t sbc_code)
{
    return;
}

void enter_hunt_mode(mpc8272_processor_comm_device *dev, uint8_t page, uint8_t sbc_code)
{
    return;
}

void stop_tx(mpc8272_processor_comm_device *dev, uint8_t page, uint8_t sbc_code)
{
    return;
}

void graceful_stop_tx(mpc8272_processor_comm_device *dev, uint8_t page, uint8_t sbc_code)
{
    return;
}

void restart_tx(mpc8272_processor_comm_device *dev, uint8_t page, uint8_t sbc_code)
{
    return;
}

void close_rx_bd(mpc8272_processor_comm_device *dev, uint8_t page, uint8_t sbc_code)
{
    return;
}

void set_group_address(mpc8272_processor_comm_device *dev, uint8_t page, uint8_t sbc_code)
{
    return;
}

void atm_transfer_command(mpc8272_processor_comm_device *dev, uint8_t page, uint8_t sbc_code)
{
    return;
}

static pc_fun_ptr_t func_ptr_table[14] =
{
    &init_rx_tx_params,
    &init_rx_params,
    &init_tx_params,
    &enter_hunt_mode,
    &stop_tx,
    &graceful_stop_tx,
    &restart_tx,
    &close_rx_bd,
    &set_group_address,
    NULL,
    &atm_transfer_command,
    NULL,
    NULL,
    NULL,
};


static void handle_condition_by_flg(mpc8272_processor_comm_device *dev, uint8_t page, uint8_t sbc_code, uint8_t opcode)
{
    if(opcode == 9 || opcode == 11 || opcode == 12 || opcode == 13)
    {
        printf("current opcode %d function not realize", opcode);
        return;
    }

    /*根据手册page513, 首先判断opcode的值*/
    if(func_ptr_table[opcode] != NULL)
    {
       func_ptr_table[opcode](dev, page, sbc_code);
    }
}

static void reset_regs(conf_object_t *conf_obj)
{
    mpc8272_processor_comm_device *dev = (mpc8272_processor_comm_device *)conf_obj->obj;
    
    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->CPCR = 0;
    regs->RCCR = 0;
    regs->RTER = 0;
    regs->RTMR = 0;
    regs->RTSCR = 0;
    regs->RTSR = 0;
}

// 寄存器读取操作方法
static exception_t mpc8272_processor_comm_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8272_processor_comm_device *dev = (mpc8272_processor_comm_device *)conf_obj->obj;

    switch (offset)
    {
        case _CPCR:
            *(uint32_t *)buf = dev->regs.CPCR;
            break;
        case _RCCR:
            *(uint32_t *)buf = dev->regs.RCCR;
            break;
        case _RTER:
            *(uint16_t *)buf = dev->regs.RTER;
            break;
        case _RTMR:
            *(uint16_t *)buf = dev->regs.RTMR;
            break;
        case _RTSCR:
            *(uint16_t *)buf = dev->regs.RTSCR;
            break;
        case _RTSR:
            *(uint32_t *)buf = dev->regs.RTSR;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc8272_processor_comm\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc8272_processor_comm_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8272_processor_comm_device *dev = (mpc8272_processor_comm_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;

    switch (offset)
    {
        case _CPCR:
            {
                dev->regs.CPCR = val;

                if(dev->regs.CPCR & CPCR_RST)
                {
                    /*手册page 512, 置位CPCR_RST后理论上应该先两个时钟周期内清除RST位*/
                    dev->regs.CPCR &= ~CPCR_RST;
                }
                
                /*提取写入CPCR的PAGE SBC OPCODE字段*/
                uint8_t cpcr_page = (val & CPCR_PAGE_MASK) >> 26;
                uint8_t cpcr_sbc_code = (val & CPCR_SBC_CODE_MASK) >> 21;
                uint8_t cpcr_opcode = val & CPCR_OPCODE_MASK;

                handle_condition_by_flg(dev, cpcr_page, cpcr_sbc_code, cpcr_opcode);

                if(dev->regs.CPCR & CPCR_FLG)
                {
                    dev->regs.CPCR &= ~CPCR_FLG;
                }

                break;
            }
        case _RCCR:
            dev->regs.RCCR = val;
            break;
        case _RTER:
            dev->regs.RTER = val;
            break;
        case _RTMR:
            dev->regs.RTMR = val;
            break;
        case _RTSCR:
            dev->regs.RTSCR = val;
            break;
        case _RTSR:
            dev->regs.RTSR = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc8272_processor_comm\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc8272_processor_comm_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc8272_processor_comm_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc8272_processor_comm_device *dev = (mpc8272_processor_comm_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc8272_processor_comm_get_regnum(conf_object_t *conf_obj)
{
    return 6;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc8272_processor_comm_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc8272_processor_comm_device *dev = (mpc8272_processor_comm_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc8272_processor_comm_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
    uint32_t regnum = 6;
    int i;

    for (i = 0; i < regnum; i++)
    {
        if (strcmp(name, regs_name[i]) == 0)
            return i;
    }

    return regnum;
}

// 寄存器接口:根据寄存器ID获取寄存器偏移
static uint32_t mpc8272_processor_comm_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc8272_processor_comm(const char *obj_name)
{
    mpc8272_processor_comm_device* dev = skyeye_mm_zero(sizeof(mpc8272_processor_comm_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    reset_regs(dev->obj);

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc8272_processor_comm(conf_object_t* conf_obj)
{
    mpc8272_processor_comm_device *dev = (mpc8272_processor_comm_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc8272_processor_comm(conf_object_t* conf_obj)
{
    mpc8272_processor_comm_device *dev = (mpc8272_processor_comm_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc8272_processor_comm(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc8272_processor_comm",
        .class_desc      = "mpc8272 communication processor device",
        .new_instance    = new_mpc8272_processor_comm,
        .free_instance   = free_mpc8272_processor_comm,
        .config_instance = config_mpc8272_processor_comm,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc8272_processor_comm_read,
        .write = mpc8272_processor_comm_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc8272_processor_comm_get_regvalue_by_id,
        .get_regname_by_id   = mpc8272_processor_comm_get_regname_by_id,
        .set_regvalue_by_id  = mpc8272_processor_comm_set_regvalue_by_id,
        .get_regnum          = mpc8272_processor_comm_get_regnum,
        .get_regid_by_name   = mpc8272_processor_comm_get_regid_by_name,
        .get_regoffset_by_id = mpc8272_processor_comm_get_regoffset_by_id
    };
    SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);

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
    };

    // 接口连接方法列表
    static const struct ConnectDescription connects[] =
    {
        /* ConnectDescription */
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
}

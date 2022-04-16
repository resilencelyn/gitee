/*************************************************************************
  > File Name    : ft2000a_gmac.c
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/11/27 10:10:41
  > This is ft-2000A GMAC(以太网控制器) register device
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
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#define DEBUG
#include <skyeye_log.h>
#include "ft2000a_gmac.h"

static void miim_write_cycle(ft2000a_gmac_device *dev)
{
    struct gmac_registers *regs = &dev->regs;
    uint8_t addr = regs->gmac_regs.gmii_addr_reg.GR;
    uint8_t phy = regs->gmac_regs.gmii_addr_reg.PA;
    uint32_t data = regs->gmac_regs.gmii_data_reg & 0xffff;

    switch (addr)
    {
        case MII_BMCR:
            dev->phy_control = data & ~(0x8100);
            break;
        default:
            dev->phy_regs[addr] = data;
            //printf("In %s, phy_reg = %x data = %x\n", __func__, addr, data);
            break;
    }
}

static uint16_t miim_read_cycle(ft2000a_gmac_device *dev)
{
    struct gmac_registers *regs = &dev->regs;
    uint16_t data = 0xffff;
    uint8_t addr = regs->gmac_regs.gmii_addr_reg.GR;
    uint8_t phy = regs->gmac_regs.gmii_addr_reg.PA;

    //printf("In %s, addr = %x\n", __func__,addr);
    switch (addr)
    {
        case MII_BMCR:
            data = dev->phy_control;
            break;
        case MII_BMSR:
            data = BMSR_ANEGCOMPLETE;
            break;
        case MII_PHYSID1:
            if (phy == dev->phy_addr)
            {
                data = 1;
            }
            break;
        default:
            data = dev->phy_regs[addr];
            //printf("In %s, phy_reg = %x\n", __func__, addr);
            break;
    }

    return data;
}

static exception_t ft2000a_gmac_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    ft2000a_gmac_device *dev = opaque->obj;
    struct gmac_registers *regs = &dev->regs;
    uint32_t *data = (uint32_t *)buf;

    switch (offset)
    {
        case MAC_CFG_REG:
            *data = regs->gmac_regs.mac_cfg_reg.all;
            break;
        case MAC_FRAME_FILTER:
            *data = regs->gmac_regs.mac_frame_filter.all;
            break;
        case HTH:
            *data = regs->gmac_regs.hash_table_high;
            break;
        case HTL:
            *data = regs->gmac_regs.hash_table_low;
            break;
        case GMII_ADDR:
            *data = regs->gmac_regs.gmii_addr_reg.all;
            break;
        case GMII_DATA:
            *data = regs->gmac_regs.gmii_data_reg;
            break;
        case FLOW_CTRL_REG:
            *data = regs->gmac_regs.flow_control_reg.all;
            break;
        case VERSION_REG:
            *data = regs->gmac_regs.version_reg;
            break;
        case MAH:
            *data = regs->gmac_regs.mac_addr0_high;
            break;
        case MAL:
            *data = regs->gmac_regs.mac_addr0_low;
            break;
        case BUS_MODE:
            *data = regs->gmac_regs.bus_mode_reg.all;
            break;
        case RECV_DESC_ADDR:
            *data = regs->gmac_regs.recv_desc_addr;
            break;
        case SEND_DESC_ADDR:
            *data = regs->gmac_regs.send_desc_addr;
            break;
        case STATUS_REG:
            *data = regs->gmac_regs.status_reg.all;
            break;
        case OPE_MODE_REG:
            *data = regs->gmac_regs.ope_mode_reg.all;
            break;
        case INT_ENABLE_REG:
            *data = regs->gmac_regs.interrupt_enable_reg.all;
            break;
        case AXI_BUS_MODE:
            *data = regs->gmac_regs.axi_bus_mode_reg.all;
            break;
        case AXI_STATUS:
            *data = regs->gmac_regs.axi_status_reg.all;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in ft2000a_gmac\n", offset);
            break;
    }

    //printf("In %s, offset = %x value  = %x\n", __func__, offset, *data);
    return 0;
}

static exception_t ft2000a_gmac_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    ft2000a_gmac_device *dev = opaque->obj;
    struct gmac_registers *regs = &dev->regs;
    uint32_t data = *(uint32_t *)buf;

    //printf("In %s, offset = %x value  = %x\n", __func__, offset, data);
    switch (offset)
    {
        case MAC_CFG_REG:
            regs->gmac_regs.mac_cfg_reg.all = data;
            break;
        case MAC_FRAME_FILTER:
            regs->gmac_regs.mac_frame_filter.all = data;
            break;
        case HTH:
            regs->gmac_regs.hash_table_high = data;
            break;
        case HTL:
            regs->gmac_regs.hash_table_low = data;
            break;
        case GMII_ADDR:
            regs->gmac_regs.gmii_addr_reg.all = data;
            //读写PHY，GB位自动清0
            regs->gmac_regs.gmii_addr_reg.GB = 0;
            if (regs->gmac_regs.gmii_addr_reg.GW)//写请求
            {
                miim_write_cycle(dev);
            } else
            {
                uint16_t data = miim_read_cycle(dev);
                regs->gmac_regs.gmii_data_reg = data;
            }
            break;
        case GMII_DATA:
            regs->gmac_regs.gmii_data_reg = data;
            if (data & BMCE_RESET)
            {
                //RESET PHY
            }
            break;
        case FLOW_CTRL_REG:
            regs->gmac_regs.flow_control_reg.all = data;
            break;
        case VERSION_REG:
            regs->gmac_regs.version_reg = data;
            break;
        case MAH:
            regs->gmac_regs.mac_addr0_high = data;
            //printf("mac_addr0_high = %x\n", data);
            break;
        case MAL:
            regs->gmac_regs.mac_addr0_low = data;
            //printf("mac_addr0_low = %x\n", data);
            break;
        case BUS_MODE:
            regs->gmac_regs.bus_mode_reg.all = data;
            if (regs->gmac_regs.bus_mode_reg.SWR)
            {
                //DMA软复位，该位清0
                regs->gmac_regs.bus_mode_reg.SWR = 0;
            }
            break;
        case RECV_DESC_ADDR:
            regs->gmac_regs.recv_desc_addr = data;
            break;
        case SEND_DESC_ADDR:
            regs->gmac_regs.send_desc_addr = data;
            break;
        case STATUS_REG:
            regs->gmac_regs.status_reg.all = data;
            break;
        case OPE_MODE_REG:
            regs->gmac_regs.ope_mode_reg.all = data;
            break;
        case INT_ENABLE_REG:
            regs->gmac_regs.interrupt_enable_reg.all = data;
            break;
        case AXI_BUS_MODE:
            regs->gmac_regs.axi_bus_mode_reg.all = data;
            break;
        case AXI_STATUS:
            regs->gmac_regs.axi_status_reg.all = data;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in ft2000a_gmac\n", offset);
            break;
    }
    return 0;
}

static char* ft2000a_gmac_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return gmac_regs_name[id];
}

static exception_t ft2000a_gmac_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    ft2000a_gmac_device *dev = conf_obj->obj;
    gmac_reg_t *regs = &(dev->regs.gmac_regs);
    *((uint32_t*)regs + id) = value;
    return No_exp;
}

static uint32_t ft2000a_gmac_get_regnum(conf_object_t *conf_obj)
{
    return  sizeof(gmac_reg_t) / 4;
}

static uint32_t ft2000a_gmac_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    ft2000a_gmac_device *dev = conf_obj->obj;
    gmac_reg_t *regs = &(dev->regs.gmac_regs);
    uint32_t* regs_value = (uint32_t*)regs + id;
    return *regs_value;
}

static uint32_t ft2000a_gmac_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
    uint32_t regnum = sizeof(gmac_reg_t) / 4, i;
    for(i = 0; i < regnum; i++) {
        if(strcmp(name, gmac_regs_name[i]) == 0)
            return i;
    }
    return regnum;
}

static uint32_t ft2000a_gmac_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
    return gmac_regs_offset[id];
}

void init_gmac_register(struct gmac_registers *regs)
{
    regs->gmac_regs.mac_cfg_reg.all = 0;
    regs->gmac_regs.mac_frame_filter.all = 0;
    regs->gmac_regs.hash_table_high = 0;
    regs->gmac_regs.hash_table_low = 0;
    regs->gmac_regs.gmii_addr_reg.all = 0;
    regs->gmac_regs.gmii_data_reg = 0;
    regs->gmac_regs.flow_control_reg.all = 0;
    regs->gmac_regs.version_reg = 0x1036;
    regs->gmac_regs.mac_addr0_high = 0x8000ffff;
    regs->gmac_regs.mac_addr0_low = 0xffffffff;
    regs->gmac_regs.bus_mode_reg.PBL = 1;
    regs->gmac_regs.recv_desc_addr = 0;
    regs->gmac_regs.send_desc_addr = 0;
    regs->gmac_regs.status_reg.all = 0;
    regs->gmac_regs.ope_mode_reg.all = 0;
    regs->gmac_regs.interrupt_enable_reg.all = 0;
    regs->gmac_regs.axi_bus_mode_reg.UNDEF = 1;
    regs->gmac_regs.axi_bus_mode_reg.RD_OSR_LMT = 1;
    regs->gmac_regs.axi_bus_mode_reg.WR_OSR_LMT = 1;
    regs->gmac_regs.axi_status_reg.all = 0;
}

static conf_object_t* new_ft2000a_gmac(const char *obj_name)
{
    ft2000a_gmac_device* dev = skyeye_mm_zero(sizeof(ft2000a_gmac_device));
    dev->obj = new_conf_object(obj_name, dev);
    gmac_reg_t *regs = &(dev->regs);
    init_gmac_register(regs);
    dev->phy_addr = 4; //init phy = 4;
    return dev->obj;
}

static exception_t free_ft2000a_gmac(conf_object_t* opaque)
{
    ft2000a_gmac_device *dev = opaque->obj;
    skyeye_free(opaque->objname);
    skyeye_free(opaque->obj);
    skyeye_free(opaque);
    return No_exp;
}

static exception_t config_ft2000a_gmac(conf_object_t* opaque)
{
    ft2000a_gmac_device *dev = opaque->obj;
    return No_exp;
}

static void ft2000a_gmac_register_attribute(conf_class_t* clss){
}

void init_ft2000a_gmac(void)
{
    static skyeye_class_t class_data =
    {
        .class_name = "ft2000a_gmac",
        .class_desc = "ft2000a_gmac",
        .new_instance = new_ft2000a_gmac,
        .free_instance = free_ft2000a_gmac,
        .config_instance = config_ft2000a_gmac
    };
    conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

    static memory_space_intf io_memory =
    {
        .read = ft2000a_gmac_read,
        .write = ft2000a_gmac_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static skyeye_reg_intf reg_intf =
    {
        .get_regvalue_by_id = ft2000a_gmac_get_regvalue_by_id,
        .get_regname_by_id = ft2000a_gmac_get_regname_by_id,
        .set_regvalue_by_id = ft2000a_gmac_set_regvalue_by_id,
        .get_regnum = ft2000a_gmac_get_regnum,
        .get_regid_by_name = ft2000a_gmac_get_regid_by_name,
        .get_regoffset_by_id = ft2000a_gmac_get_regoffset_by_id
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

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
    };

    static const struct ConnectDescription connects[] =
    {
        /* ConnectDescription */
    };

    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);
    ft2000a_gmac_register_attribute(clss);
}

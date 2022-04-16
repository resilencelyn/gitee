/*************************************************************************
	> File Name    : ft2000a_iic.c
	> Author       : jiachao
	> Mail         : jiachao@163.com
	> Created Time : 2019/11/25 10:10:41
	> This is ft-2000A I2C register device
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
#include "iic_fifo.h"
#include "ft2000a_iic.h"

static void check_transmit_stop_condition(ft2000a_iic_device *dev){
	struct iic_registers *regs = &dev->regs;
	int fifo_cnt = 0;
	char send_data;

	if (regs->iic_data_cmd.STOP)
	{
		if (dev->i2c_bus_iface != NULL)
		{
			fifo_cnt = CheckReadNum(dev->send_fifo);
			if (fifo_cnt >= FIFO_SIZE)
			{
				if (regs->iic_intr_mask.M_TX_OVER == 0)
				{
					regs->iic_intr_state.TX_OVER = 1;
					if (dev->signal != NULL)
						dev->signal_iface->raise_signal(dev->signal, dev->int_number);
				}
				dev->send_fifo->Enteres = 0;
				return ;
			}

			int i;
			for (i = 0; i < fifo_cnt; i++)
			{
				ReadFIFO(dev->send_fifo, &send_data, 1);
				dev->i2c_bus_iface->write_data(dev->i2c_bus, send_data);
			}
		}

		if (regs->iic_intr_mask.M_STOP_DET == 0)
		{
			regs->iic_intr_state.STOP_DET = 1;
			if (dev->signal_iface != NULL)
				dev->signal_iface->raise_signal(dev->signal, dev->int_number);
		}
	}
}

static void check_receive_stop_condition(ft2000a_iic_device *dev)
{
	struct iic_registers *regs = &dev->regs;
	if (regs->iic_data_cmd.STOP)
	{
		if (regs->iic_intr_mask.M_STOP_DET == 0)
		{
			regs->iic_intr_state.STOP_DET = 1;
			if (dev->signal != NULL)
				dev->signal_iface->raise_signal(dev->signal, dev->int_number);
		}
	}
}

static exception_t ft2000a_iic_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_iic_device *dev = opaque->obj;
	struct iic_registers *regs = &dev->regs;
	uint16_t *data = (uint16_t *)buf;

	switch (offset)
	{
		case IIC_CON:
			*data = regs->iic_con.all;
			break;
		case IIC_TAR:
			*data = regs->iic_tar.all;
			break;
		case IIC_SAR:
			*data = regs->iic_sar;
			break;
		case IIC_HS_MADDR:
			*data = regs->iic_hs_maddr;
			break;
		case IIC_DATA_CMD:
			if (dev->i2c_bus_iface != NULL)
			{
				regs->iic_data_cmd.DAT = (uint8_t)dev->i2c_bus_iface->read_data(dev->i2c_bus);
			}
			*data = regs->iic_data_cmd.all;
			break;
		case IIC_SS_SCL_HCNT:
			*data = regs->iic_ss_scl_hcnt;
			break;
		case IIC_SS_SCL_LCNT:
			*data = regs->iic_ss_scl_lcnt;
			break;
		case IIC_FS_SCL_HCNT:
			*data = regs->iic_fs_scl_hcnt;
			break;
		case IIC_FS_SCL_LCNT:
			*data = regs->iic_fs_scl_lcnt;
			break;
		case IIC_HS_SCL_HCNT:
			*data = regs->iic_hs_scl_hcnt;
			break;
		case IIC_HS_SCL_LCNT:
			*data = regs->iic_hs_scl_lcnt;
			break;
		case IIC_INTR_MASK:
			*data = regs->iic_intr_mask.all;
			break;
		case IIC_INTR_STATE:
			*data = regs->iic_intr_state.all;
			break;
		case IIC_RX_TL:
			*data = regs->iic_rx_tl;
			break;
		case IIC_TX_TL:
			*data = regs->iic_tx_tl;
			break;
		case IIC_ENABLE:
			*data = regs->iic_enable;
			break;
		case IIC_STATE:
			*data = regs->iic_state.all;
			break;
		case IIC_ENABLE_STATE:
			*data = regs->iic_enable_state;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in ft2000a_iic\n", offset);
			break;
	}

	return No_exp;
}

static exception_t ft2000a_iic_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ft2000a_iic_device *dev = opaque->obj;
	struct iic_registers *regs = &dev->regs;
	uint16_t data = *(uint16_t *)buf;

	switch (offset)
	{
		case IIC_CON:
			regs->iic_con.all = data;
			if (regs->iic_con.MASTER_MODE)
			{
				if (dev->i2c_bus_iface != NULL)
					dev->i2c_bus_iface->start(dev->i2c_bus, regs->iic_tar.IC_TAR & (~0x1));
			}
			break;
		case IIC_TAR:
			regs->iic_tar.all = data;
			break;
		case IIC_SAR:
			regs->iic_sar = data;
			break;
		case IIC_HS_MADDR:
			regs->iic_hs_maddr = data;
			break;
		case IIC_DATA_CMD:
			{
				regs->iic_data_cmd.all = data;
				uint8_t w_data = regs->iic_data_cmd.DAT;
				WriteFIFO(dev->send_fifo, &w_data, 1);
			}
			if (regs->iic_con.MASTER_MODE)
			{
				if (regs->iic_intr_mask.M_START_DET == 0)
				{
					regs->iic_intr_state.START_DET = 1;
					if (dev->signal_iface != NULL)
						dev->signal_iface->raise_signal(dev->signal, dev->int_number);
				}

				if (regs->iic_data_cmd.CMD == 0)
				{
					//write data
					check_transmit_stop_condition(dev);
				} else
				{
					//read data
					if (regs->iic_tar.SPECIAL == 1 && regs->iic_tar.GC_OR_START == 0)
					{
						regs->iic_intr_state.TX_ABRT = 1;
					}
					check_receive_stop_condition(dev);
				}
			}
			break;
		case IIC_SS_SCL_HCNT:
			regs->iic_ss_scl_hcnt = data;
			break;
		case IIC_SS_SCL_LCNT:
			regs->iic_ss_scl_lcnt = data;
			break;
		case IIC_FS_SCL_HCNT:
			regs->iic_fs_scl_hcnt = data;
			break;
		case IIC_FS_SCL_LCNT:
			regs->iic_fs_scl_lcnt = data;
			break;
		case IIC_HS_SCL_HCNT:
			regs->iic_hs_scl_hcnt = data;
			break;
		case IIC_HS_SCL_LCNT:
			regs->iic_hs_scl_lcnt = data;
			break;
		case IIC_INTR_MASK:
			regs->iic_intr_mask.all = data;
			break;
		case IIC_RX_TL:
			regs->iic_rx_tl = data;
			break;
		case IIC_TX_TL:
			regs->iic_tx_tl = data;
			break;
		case IIC_ENABLE:
			regs->iic_enable = data;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in ft2000a_iic\n", offset);
			break;
	}
	return No_exp;
}

static char* ft2000a_iic_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t ft2000a_iic_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	ft2000a_iic_device *dev = conf_obj->obj;
	struct iic_registers *regs = &(dev->regs);
	*((uint16_t*)regs + id) = value;
	return No_exp;
}

static uint32_t ft2000a_iic_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct iic_registers) / 2;
}

static uint32_t ft2000a_iic_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	ft2000a_iic_device *dev = conf_obj->obj;
	struct iic_registers *regs = &(dev->regs);
	uint16_t* regs_value = (uint16_t*)regs + id;
	return *regs_value;
}

static uint32_t ft2000a_iic_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct iic_registers) / 2, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t ft2000a_iic_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

void init_iic_register(struct iic_registers *regs)
{
	regs->iic_con.all = 0x7f;
	regs->iic_tar.all = 0x1055;
	regs->iic_sar = 0x55;
	regs->iic_data_cmd.all = 0;
	regs->iic_hs_maddr = IC_HS_MASTER_CODE;
	regs->iic_ss_scl_hcnt = 0x190;
	regs->iic_ss_scl_lcnt = 0x1d6;
	regs->iic_fs_scl_hcnt = 0x3c;
	regs->iic_fs_scl_lcnt = 0x82;
	regs->iic_hs_scl_hcnt = 0x6;
	regs->iic_hs_scl_lcnt = 0x10;
	regs->iic_intr_mask.all = 0x8ff;
	regs->iic_intr_state.all = 0x0;
	regs->iic_rx_tl = 0x0;
	regs->iic_tx_tl = 0x0;
	regs->iic_enable = 0x0;
	regs->iic_state.all = 0x6;
	regs->iic_enable_state = 0x0;
}

static conf_object_t* new_ft2000a_iic(const char *obj_name)
{
	ft2000a_iic_device* dev = skyeye_mm_zero(sizeof(ft2000a_iic_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct iic_registers *regs = &(dev->regs);
	init_iic_register(regs);
	dev->send_fifo = CreateFIFO(256);
	return dev->obj;
}

static exception_t free_ft2000a_iic(conf_object_t* opaque)
{
	ft2000a_iic_device *dev = opaque->obj;
	skyeye_free(dev->send_fifo);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t config_ft2000a_iic(conf_object_t* opaque)
{
	ft2000a_iic_device *dev = opaque->obj;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	ft2000a_iic_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	ft2000a_iic_device *dev = conf_obj->obj;
	dev->int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void ft2000a_iic_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of iic.");
}

static exception_t get_attr_i2c_bus(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	ft2000a_iic_device *dev = obj->obj;
	*obj2 = dev->i2c_bus;
	*port = NULL;
	return No_exp;
}

static exception_t set_attr_i2c_bus(conf_object_t *obj, conf_object_t *obj2, const char *port, int index){
	ft2000a_iic_device *dev = obj->obj;
	dev->i2c_bus = obj2;
	dev->i2c_bus_iface = (i2c_bus_intf *)SKY_get_iface(dev->i2c_bus, I2C_BUS_INTF_NAME);
	if (dev->i2c_bus_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", I2C_BUS_INTF_NAME, obj2->objname);
	}
	return No_exp;
}

static exception_t set_general_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	ft2000a_iic_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	return No_exp;
}

static exception_t get_general_signal(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	ft2000a_iic_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void init_ft2000a_iic(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "ft2000a_iic",
		.class_desc = "ft2000a_iic",
		.new_instance = new_ft2000a_iic,
		.free_instance = free_ft2000a_iic,
		.config_instance = config_ft2000a_iic
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = ft2000a_iic_read,
		.write = ft2000a_iic_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = ft2000a_iic_get_regvalue_by_id,
		.get_regname_by_id = ft2000a_iic_get_regname_by_id,
		.set_regvalue_by_id = ft2000a_iic_set_regvalue_by_id,
		.get_regnum = ft2000a_iic_get_regnum,
		.get_regid_by_name = ft2000a_iic_get_regid_by_name,
		.get_regoffset_by_id = ft2000a_iic_get_regoffset_by_id
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
		(struct ConnectDescription) {
			.name = I2C_BUS_INTF_NAME,
			.set = set_attr_i2c_bus,
			.get = get_attr_i2c_bus,
		},
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = set_general_signal,
			.get = get_general_signal,
		},
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	ft2000a_iic_register_attribute(clss);
}

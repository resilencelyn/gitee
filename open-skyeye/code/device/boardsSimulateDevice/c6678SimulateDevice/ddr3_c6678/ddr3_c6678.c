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
#define DEBUG
#include <skyeye_log.h>
#include "ddr3_c6678.h"

static exception_t ddr3_c6678_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ddr3_c6678_device *dev = opaque->obj;

	switch (offset) {
	case MIDR:
		*(uint32_t *)buf = dev->regs.midr;
		break;
	case STATUS:
		*(uint32_t *)buf = dev->regs.status;
		break;
	case SDCFG:
		*(uint32_t *)buf = dev->regs.sdcfg;
		break;
	case SDRFC:
		*(uint32_t *)buf = dev->regs.sdrfc;
		break;
	case SDTIM1:
		*(uint32_t *)buf = dev->regs.sdtim1;
		break;
	case SDTIM2:
		*(uint32_t *)buf = dev->regs.sdtim2;
		break;
	case SDTIM3:
		*(uint32_t *)buf = dev->regs.sdtim3;
		break;
	case PMCTL:
		*(uint32_t *)buf = dev->regs.pmctl;
		break;
	case LAT_CONFIG:
		*(uint32_t *)buf = dev->regs.lat_config;
		break;
	case PERF_CNT_1:
		*(uint32_t *)buf = dev->regs.perf_cnt_1;
		break;
	case PERF_CNT_2:
		*(uint32_t *)buf = dev->regs.perf_cnt_2;
		break;
	case PERF_CNT_CFG:
		*(uint32_t *)buf = dev->regs.perf_cnt_cfg;
		break;
	case PERF_CNT_SEL:
		*(uint32_t *)buf = dev->regs.perf_cnt_sel;
		break;
	case PERF_CNT_TIM:
		*(uint32_t *)buf = dev->regs.perf_cnt_tim;
		break;
	case IRQSTATUS_RAW_SYS:
		*(uint32_t *)buf = dev->regs.irqstatus_raw_sys;
		break;
	case IRQ_STATUS_SYS:
		*(uint32_t *)buf = dev->regs.irq_status_sys;
		break;
	case IRQENABLE_SET_SYS:
		*(uint32_t *)buf = dev->regs.irqenable_set_sys;
		break;
	case IRQENABLE_CLR_SYS:
		*(uint32_t *)buf = dev->regs.irqenable_clr_sys;
		break;
	case ZQCONFIG:
		*(uint32_t *)buf = dev->regs.zqconfig;
		break;
	case RDWR_LVL_RMP_WIN:
		*(uint32_t *)buf = dev->regs.rdwr_lvl_rmp_win;
		break;
	case RDWR_LVL_RMP_CTRL:
		*(uint32_t *)buf = dev->regs.rdwr_lvl_rmp_ctrl;
		break;
	case RDWR_LVL_CTRL:
		*(uint32_t *)buf = dev->regs.rdwr_lvl_ctrl;
		break;
	case DDR_PHY_CTRL_1:
		*(uint32_t *)buf = dev->regs.ddr_phy_ctrl_1;
		break;
	case PRI_COS_MAP:
		*(uint32_t *)buf = dev->regs.pri_cos_map;
		break;
	case MSTID_COS_1_MAP:
		*(uint32_t *)buf = dev->regs.mstid_cos_1_map;
		break;
	case MSTID_COS_2_MAP:
		*(uint32_t *)buf = dev->regs.mstid_cos_2_map;
		break;
	case ECCCTL:
		*(uint32_t *)buf = dev->regs.eccctl;
		break;
	case ECCADDR1:
		*(uint32_t *)buf = dev->regs.eccaddr1;
		break;
	case ECCADDR2:
		*(uint32_t *)buf = dev->regs.eccaddr2;
		break;
	case RWTHRESH:
		*(uint32_t *)buf = dev->regs.rwthresh;
		break;
	case DDR3_CONFIG_0:
		*(uint32_t *)buf = dev->regs.ddr3_config_0;
		break;
	case DDR3_CONFIG_1:
		*(uint32_t *)buf = dev->regs.ddr3_config_1;
		break;
	case DDR3_CONFIG_2:
		*(uint32_t *)buf = dev->regs.ddr3_config_2;
		break;
	case DDR3_CONFIG_3:
		*(uint32_t *)buf = dev->regs.ddr3_config_3;
		break;
	case DDR3_CONFIG_4:
		*(uint32_t *)buf = dev->regs.ddr3_config_4;
		break;
	case DDR3_CONFIG_5:
		*(uint32_t *)buf = dev->regs.ddr3_config_5;
		break;
	case DDR3_CONFIG_6:
		*(uint32_t *)buf = dev->regs.ddr3_config_6;
		break;
	case DDR3_CONFIG_7:
		*(uint32_t *)buf = dev->regs.ddr3_config_7;
		break;
	case DDR3_CONFIG_8:
		*(uint32_t *)buf = dev->regs.ddr3_config_8;
		break;
	case DDR3_CONFIG_9:
		*(uint32_t *)buf = dev->regs.ddr3_config_9;
		break;
	case DDR3_CONFIG_10:
		*(uint32_t *)buf = dev->regs.ddr3_config_10;
		break;
	case DDR3_CONFIG_12:
		*(uint32_t *)buf = dev->regs.ddr3_config_12;
		break;
	case DDR3_CONFIG_14:
		*(uint32_t *)buf = dev->regs.ddr3_config_14;
		break;
	case DDR3_CONFIG_15:
		*(uint32_t *)buf = dev->regs.ddr3_config_15;
		break;
	case DDR3_CONFIG_16:
		*(uint32_t *)buf = dev->regs.ddr3_config_16;
		break;
	case DDR3_CONFIG_17:
		*(uint32_t *)buf = dev->regs.ddr3_config_17;
		break;
	case DDR3_CONFIG_18:
		*(uint32_t *)buf = dev->regs.ddr3_config_18;
		break;
	case DDR3_CONFIG_19:
		*(uint32_t *)buf = dev->regs.ddr3_config_19;
		break;
	case DDR3_CONFIG_20:
		*(uint32_t *)buf = dev->regs.ddr3_config_20;
		break;
	case DDR3_CONFIG_21:
		*(uint32_t *)buf = dev->regs.ddr3_config_21;
		break;
	case DDR3_CONFIG_22:
		*(uint32_t *)buf = dev->regs.ddr3_config_22;
		break;
	case DDR3_CONFIG_23:
		*(uint32_t *)buf = dev->regs.ddr3_config_23;
		break;
	case DDR3_CONFIG_24:
		*(uint32_t *)buf = dev->regs.ddr3_config_24;
		break;
	case DDR3_CONFIG_REG_25:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_25;
		break;
	case DDR3_CONFIG_REG_26:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_26;
		break;
	case DDR3_CONFIG_REG_27:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_27;
		break;
	case DDR3_CONFIG_REG_28:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_28;
		break;
	case DDR3_CONFIG_REG_29:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_29;
		break;
	case DDR3_CONFIG_REG_30:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_30;
		break;
	case DDR3_CONFIG_REG_31:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_31;
		break;
	case DDR3_CONFIG_REG_32:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_32;
		break;
	case DDR3_CONFIG_REG_33:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_33;
		break;
	case DDR3_CONFIG_REG_34:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_34;
		break;
	case DDR3_CONFIG_REG_35:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_35;
		break;
	case DDR3_CONFIG_REG_36:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_36;
		break;
	case DDR3_CONFIG_REG_37:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_37;
		break;
	case DDR3_CONFIG_REG_38:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_38;
		break;
	case DDR3_CONFIG_REG_39:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_39;
		break;
	case DDR3_CONFIG_REG_40:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_40;
		break;
	case DDR3_CONFIG_REG_41:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_41;
		break;
	case DDR3_CONFIG_REG_42:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_42;
		break;
	case DDR3_CONFIG_REG_43:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_43;
		break;
	case DDR3_CONFIG_REG_44:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_44;
		break;
	case DDR3_CONFIG_REG_45:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_45;
		break;
	case DDR3_CONFIG_REG_46:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_46;
		break;
	case DDR3_CONFIG_REG_47:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_47;
		break;
	case DDR3_CONFIG_REG_48:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_48;
		break;
	case DDR3_CONFIG_REG_49:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_49;
		break;
	case DDR3_CONFIG_REG_50:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_50;
		break;
	case DDR3_CONFIG_REG_51:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_51;
		break;
	case DDR3_CONFIG_REG_52:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_52;
		break;
	case DDR3_CONFIG_REG_53:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_53;
		break;
	case DDR3_CONFIG_REG_54:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_54;
		break;
	case DDR3_CONFIG_REG_55:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_55;
		break;
	case DDR3_CONFIG_REG_56:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_56;
		break;
	case DDR3_CONFIG_REG_57:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_57;
		break;
	case DDR3_CONFIG_REG_58:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_58;
		break;
	case DDR3_CONFIG_REG_59:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_59;
		break;
	case DDR3_CONFIG_REG_60:
		*(uint32_t *)buf = dev->regs.ddr3_config_reg_60;
		break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in ddr3_c6678\n", offset);
		break;
	}
	if (offset == 4) return No_exp;
	//printf("%s  offset: 0x%x buf: 0x%x\n", __func__, offset, *(uint32_t *)buf);
	return No_exp;
}

static exception_t ddr3_c6678_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	ddr3_c6678_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf("%s offset: 0x%x buf: 0x%x\n", __func__, offset, *(uint32_t *)buf);

	switch (offset) {
	case MIDR:
		dev->regs.midr = val;
		break;
	case STATUS:
		dev->regs.status = val;
		break;
	case SDCFG:
		dev->regs.sdcfg = val;
		break;
	case SDRFC:
		dev->regs.sdrfc = val;
		break;
	case SDTIM1:
		dev->regs.sdtim1 = val;
		break;
	case SDTIM2:
		dev->regs.sdtim2 = val;
		break;
	case SDTIM3:
		dev->regs.sdtim3 = val;
		break;
	case PMCTL:
		dev->regs.pmctl = val;
		break;
	case LAT_CONFIG:
		dev->regs.lat_config = val;
		break;
	case PERF_CNT_1:
		dev->regs.perf_cnt_1 = val;
		break;
	case PERF_CNT_2:
		dev->regs.perf_cnt_2 = val;
		break;
	case PERF_CNT_CFG:
		dev->regs.perf_cnt_cfg = val;
		break;
	case PERF_CNT_SEL:
		dev->regs.perf_cnt_sel = val;
		break;
	case PERF_CNT_TIM:
		dev->regs.perf_cnt_tim = val;
		break;
	case IRQSTATUS_RAW_SYS:
		dev->regs.irqstatus_raw_sys = val;
		break;
	case IRQ_STATUS_SYS:
		dev->regs.irq_status_sys = val;
		break;
	case IRQENABLE_SET_SYS:
		dev->regs.irqenable_set_sys = val;
		break;
	case IRQENABLE_CLR_SYS:
		dev->regs.irqenable_clr_sys = val;
		break;
	case ZQCONFIG:
		dev->regs.zqconfig = val;
		break;
	case RDWR_LVL_RMP_WIN:
		dev->regs.rdwr_lvl_rmp_win = val;
		break;
	case RDWR_LVL_RMP_CTRL:
		dev->regs.rdwr_lvl_rmp_ctrl = val;
		break;
	case RDWR_LVL_CTRL:
		dev->regs.rdwr_lvl_ctrl = val;
		break;
	case DDR_PHY_CTRL_1:
		dev->regs.ddr_phy_ctrl_1 = val;
		break;
	case PRI_COS_MAP:
		dev->regs.pri_cos_map = val;
		break;
	case MSTID_COS_1_MAP:
		dev->regs.mstid_cos_1_map = val;
		break;
	case MSTID_COS_2_MAP:
		dev->regs.mstid_cos_2_map = val;
		break;
	case ECCCTL:
		dev->regs.eccctl = val;
		break;
	case ECCADDR1:
		dev->regs.eccaddr1 = val;
		break;
	case ECCADDR2:
		dev->regs.eccaddr2 = val;
		break;
	case RWTHRESH:
		dev->regs.rwthresh = val;
		break;
	case DDR3_CONFIG_0:
		dev->regs.ddr3_config_0 = val;
		break;
	case DDR3_CONFIG_1:
		dev->regs.ddr3_config_1 = val;
		break;
	case DDR3_CONFIG_2:
		dev->regs.ddr3_config_2 = val;
		break;
	case DDR3_CONFIG_3:
		dev->regs.ddr3_config_3 = val;
		break;
	case DDR3_CONFIG_4:
		dev->regs.ddr3_config_4 = val;
		break;
	case DDR3_CONFIG_5:
		dev->regs.ddr3_config_5 = val;
		break;
	case DDR3_CONFIG_6:
		dev->regs.ddr3_config_6 = val;
		break;
	case DDR3_CONFIG_7:
		dev->regs.ddr3_config_7 = val;
		break;
	case DDR3_CONFIG_8:
		dev->regs.ddr3_config_8 = val;
		break;
	case DDR3_CONFIG_9:
		dev->regs.ddr3_config_9 = val;
		break;
	case DDR3_CONFIG_10:
		dev->regs.ddr3_config_10 = val;
		break;
	case DDR3_CONFIG_12:
		dev->regs.ddr3_config_12 = val;
		break;
	case DDR3_CONFIG_14:
		dev->regs.ddr3_config_14 = val;
		break;
	case DDR3_CONFIG_15:
		dev->regs.ddr3_config_15 = val;
		break;
	case DDR3_CONFIG_16:
		dev->regs.ddr3_config_16 = val;
		break;
	case DDR3_CONFIG_17:
		dev->regs.ddr3_config_17 = val;
		break;
	case DDR3_CONFIG_18:
		dev->regs.ddr3_config_18 = val;
		break;
	case DDR3_CONFIG_19:
		dev->regs.ddr3_config_19 = val;
		break;
	case DDR3_CONFIG_20:
		dev->regs.ddr3_config_20 = val;
		break;
	case DDR3_CONFIG_21:
		dev->regs.ddr3_config_21 = val;
		break;
	case DDR3_CONFIG_22:
		dev->regs.ddr3_config_22 = val;
		break;
	case DDR3_CONFIG_23:
		dev->regs.ddr3_config_23 = val;
		break;
	case DDR3_CONFIG_24:
		dev->regs.ddr3_config_24 = val;
		break;
	case DDR3_CONFIG_REG_25:
		dev->regs.ddr3_config_reg_25 = val;
		break;
	case DDR3_CONFIG_REG_26:
		dev->regs.ddr3_config_reg_26 = val;
		break;
	case DDR3_CONFIG_REG_27:
		dev->regs.ddr3_config_reg_27 = val;
		break;
	case DDR3_CONFIG_REG_28:
		dev->regs.ddr3_config_reg_28 = val;
		break;
	case DDR3_CONFIG_REG_29:
		dev->regs.ddr3_config_reg_29 = val;
		break;
	case DDR3_CONFIG_REG_30:
		dev->regs.ddr3_config_reg_30 = val;
		break;
	case DDR3_CONFIG_REG_31:
		dev->regs.ddr3_config_reg_31 = val;
		break;
	case DDR3_CONFIG_REG_32:
		dev->regs.ddr3_config_reg_32 = val;
		break;
	case DDR3_CONFIG_REG_33:
		dev->regs.ddr3_config_reg_33 = val;
		break;
	case DDR3_CONFIG_REG_34:
		dev->regs.ddr3_config_reg_34 = val;
		break;
	case DDR3_CONFIG_REG_35:
		dev->regs.ddr3_config_reg_35 = val;
		break;
	case DDR3_CONFIG_REG_36:
		dev->regs.ddr3_config_reg_36 = val;
		break;
	case DDR3_CONFIG_REG_37:
		dev->regs.ddr3_config_reg_37 = val;
		break;
	case DDR3_CONFIG_REG_38:
		dev->regs.ddr3_config_reg_38 = val;
		break;
	case DDR3_CONFIG_REG_39:
		dev->regs.ddr3_config_reg_39 = val;
		break;
	case DDR3_CONFIG_REG_40:
		dev->regs.ddr3_config_reg_40 = val;
		break;
	case DDR3_CONFIG_REG_41:
		dev->regs.ddr3_config_reg_41 = val;
		break;
	case DDR3_CONFIG_REG_42:
		dev->regs.ddr3_config_reg_42 = val;
		break;
	case DDR3_CONFIG_REG_43:
		dev->regs.ddr3_config_reg_43 = val;
		break;
	case DDR3_CONFIG_REG_44:
		dev->regs.ddr3_config_reg_44 = val;
		break;
	case DDR3_CONFIG_REG_45:
		dev->regs.ddr3_config_reg_45 = val;
		break;
	case DDR3_CONFIG_REG_46:
		dev->regs.ddr3_config_reg_46 = val;
		break;
	case DDR3_CONFIG_REG_47:
		dev->regs.ddr3_config_reg_47 = val;
		break;
	case DDR3_CONFIG_REG_48:
		dev->regs.ddr3_config_reg_48 = val;
		break;
	case DDR3_CONFIG_REG_49:
		dev->regs.ddr3_config_reg_49 = val;
		break;
	case DDR3_CONFIG_REG_50:
		dev->regs.ddr3_config_reg_50 = val;
		break;
	case DDR3_CONFIG_REG_51:
		dev->regs.ddr3_config_reg_51 = val;
		break;
	case DDR3_CONFIG_REG_52:
		dev->regs.ddr3_config_reg_52 = val;
		break;
	case DDR3_CONFIG_REG_53:
		dev->regs.ddr3_config_reg_53 = val;
		break;
	case DDR3_CONFIG_REG_54:
		dev->regs.ddr3_config_reg_54 = val;
		break;
	case DDR3_CONFIG_REG_55:
		dev->regs.ddr3_config_reg_55 = val;
		break;
	case DDR3_CONFIG_REG_56:
		dev->regs.ddr3_config_reg_56 = val;
		break;
	case DDR3_CONFIG_REG_57:
		dev->regs.ddr3_config_reg_57 = val;
		break;
	case DDR3_CONFIG_REG_58:
		dev->regs.ddr3_config_reg_58 = val;
		break;
	case DDR3_CONFIG_REG_59:
		dev->regs.ddr3_config_reg_59 = val;
		break;
	case DDR3_CONFIG_REG_60:
		dev->regs.ddr3_config_reg_60 = val;
		break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in ddr3_c6678\n", offset);
		break;
	}
	return No_exp;
}

static char* ddr3_c6678_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t ddr3_c6678_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	ddr3_c6678_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t ddr3_c6678_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t ddr3_c6678_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	ddr3_c6678_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t ddr3_c6678_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if (strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_ddr3_c6678(char *obj_name)
{
	ddr3_c6678_device* dev = skyeye_mm_zero(sizeof(ddr3_c6678_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	regs->status |= 0x4;
	return dev->obj;
}

static exception_t free_ddr3_c6678(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_ddr3_c6678(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6678_ddr3",
		.class_desc = "c6678_ddr3",
		.new_instance = new_ddr3_c6678,
		.free_instance = free_ddr3_c6678
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = ddr3_c6678_read,
		.write = ddr3_c6678_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = ddr3_c6678_get_regvalue_by_id,
		.get_regname_by_id = ddr3_c6678_get_regname_by_id,
		.set_regvalue_by_id = ddr3_c6678_set_regvalue_by_id,
		.get_regnum = ddr3_c6678_get_regnum,
		.get_regid_by_name = ddr3_c6678_get_regid_by_name
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
}

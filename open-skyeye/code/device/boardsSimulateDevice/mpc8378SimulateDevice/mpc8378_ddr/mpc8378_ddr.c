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
#include "mpc8378_ddr.h"

static exception_t mpc8378_i2c_regs_init(mpc8378_ddr_device *dev)
{
	dev->regs.cs0_bnds = 0x00000000;
	dev->regs.cs0_config = 0x00000000;
	dev->regs.timing_cfg_3 = 0x00000000;
	dev->regs.timing_cfg_0 = 0x00110105;
	dev->regs.timing_cfg_1 = 0x00000000;
	dev->regs.timing_cfg_2 = 0x00000000;
	dev->regs.ddr_sdram_cfg = 0x02000000;
	dev->regs.ddr_sdram_cfg_2 = 0x00000000;
	dev->regs.ddr_sdram_mode = 0x00000000;
	dev->regs.ddr_sdram_mode_2 = 0x00000000;
	dev->regs.ddr_sdram_md_cntl = 0x00000000;
	dev->regs.ddr_sdram_interval = 0x00000000;
	dev->regs.ddr_data_init = 0x00000000;
	dev->regs.ddr_sdram_clk_cntl = 0x02000000;
	dev->regs.ddr_init_addr = 0x00000000;
	dev->regs.ddr_ip_rev1 = 0x00020200;
	dev->regs.data_err_inject_hi = 0x00000000;
	dev->regs.data_err_inject_lo = 0x00000000;
	dev->regs.err_inject = 0x00000000;
	dev->regs.capture_data_hi = 0x00000000;
	dev->regs.capture_data_lo = 0x00000000;
	dev->regs.capture_ecc = 0x00000000;
	dev->regs.err_detect = 0x00000000;
	dev->regs.err_disable = 0x00000000;
	dev->regs.err_int_en = 0x00000000;
	dev->regs.capture_attributes = 0x00000000;
	dev->regs.capture_address = 0x00000000;
	dev->regs.err_sbe = 0x00000000;
	
}
static exception_t mpc8378_ddr_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	mpc8378_ddr_device *dev = opaque->obj;
	switch (offset)
	{
		case CS0_BNDS:
			*(uint32_t *)buf = dev->regs.cs0_bnds;
			break;
		case CS1_BNDS:
			*(uint32_t *)buf = dev->regs.cs1_bnds;
			break;
		case CS2_BNDS:
			*(uint32_t *)buf = dev->regs.cs2_bnds;
			break;
		case CS3_BNDS:
			*(uint32_t *)buf = dev->regs.cs3_bnds;
			break;
		case CS0_CONFIG:
			*(uint32_t *)buf = dev->regs.cs0_config;
			break;
		case CS1_CONFIG:
			*(uint32_t *)buf = dev->regs.cs1_config;
			break;
		case CS2_CONFIG:
			*(uint32_t *)buf = dev->regs.cs2_config;
			break;
		case CS3_CONFIG:
			*(uint32_t *)buf = dev->regs.cs3_config;
			break;
		case TIMING_CFG_3:
			*(uint32_t *)buf = dev->regs.timing_cfg_3;
			break;
		case TIMING_CFG_0:
			*(uint32_t *)buf = dev->regs.timing_cfg_0;
			break;
		case TIMING_CFG_1:
			*(uint32_t *)buf = dev->regs.timing_cfg_1;
			break;
		case TIMING_CFG_2:
			*(uint32_t *)buf = dev->regs.timing_cfg_2;
			break;
		case DDR_SDRAM_CFG:
			*(uint32_t *)buf = dev->regs.ddr_sdram_cfg;
			break;
		case DDR_SDRAM_CFG_2:
			*(uint32_t *)buf = dev->regs.ddr_sdram_cfg_2;
			break;
		case DDR_SDRAM_MODE:
			*(uint32_t *)buf = dev->regs.ddr_sdram_mode;
			break;
		case DDR_SDRAM_MODE_2:
			*(uint32_t *)buf = dev->regs.ddr_sdram_mode_2;
			break;
		case DDR_SDRAM_MD_CNTL:
			*(uint32_t *)buf = dev->regs.ddr_sdram_md_cntl;
			break;
		case DDR_SDRAM_INTERVAL:
			*(uint32_t *)buf = dev->regs.ddr_sdram_interval;
			break;
		case DDR_DATA_INIT:
			*(uint32_t *)buf = dev->regs.ddr_data_init;
			break;
		case DDR_SDRAM_CLK_CNTL:
			*(uint32_t *)buf = dev->regs.ddr_sdram_clk_cntl;
			break;
		case DDR_INIT_ADDR:
			*(uint32_t *)buf = dev->regs.ddr_init_addr;
			break;
		case DDR_IP_REV1:
			*(uint32_t *)buf = dev->regs.ddr_ip_rev1;
			break;
		case DDR_IP_REV2:
			*(uint32_t *)buf = dev->regs.ddr_ip_rev2;
			break;
		case DATA_ERR_INJECT_HI:
			*(uint32_t *)buf = dev->regs.data_err_inject_hi;
			break;
		case DATA_ERR_INJECT_LO:
			*(uint32_t *)buf = dev->regs.data_err_inject_lo;
			break;
		case ERR_INJECT:
			*(uint32_t *)buf = dev->regs.err_inject;
			break;
		case CAPTURE_DATA_HI:
			*(uint32_t *)buf = dev->regs.capture_data_hi;
			break;
		case CAPTURE_DATA_LO:
			*(uint32_t *)buf = dev->regs.capture_data_lo;
			break;
		case CAPTURE_ECC:
			*(uint32_t *)buf = dev->regs.capture_ecc;
			break;
		case ERR_DETECT:
			*(uint32_t *)buf = dev->regs.err_detect;
			break;
		case ERR_DISABLE:
			*(uint32_t *)buf = dev->regs.err_disable;
			break;
		case ERR_INT_EN:
			*(uint32_t *)buf = dev->regs.err_int_en;
			break;
		case CAPTURE_ATTRIBUTES:
			*(uint32_t *)buf = dev->regs.capture_attributes;
			break;
		case CAPTURE_ADDRESS:
			*(uint32_t *)buf = dev->regs.capture_address;
			break;
		case ERR_SBE:
			*(uint32_t *)buf = dev->regs.err_sbe;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc8378_ddr\n", offset);
			break;
	}
	return 0;
}
static exception_t mpc8378_ddr_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	mpc8378_ddr_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case CS0_BNDS:
			dev->regs.cs0_bnds = val;
			break;
		case CS1_BNDS:
			dev->regs.cs1_bnds = val;
			break;
		case CS2_BNDS:
			dev->regs.cs2_bnds = val;
			break;
		case CS3_BNDS:
			dev->regs.cs3_bnds = val;
			break;
		case CS0_CONFIG:
			dev->regs.cs0_config = val;
			break;
		case CS1_CONFIG:
			dev->regs.cs1_config = val;
			break;
		case CS2_CONFIG:
			dev->regs.cs2_config = val;
			break;
		case CS3_CONFIG:
			dev->regs.cs3_config = val;
			break;
		case TIMING_CFG_3:
			dev->regs.timing_cfg_3 = val;
			break;
		case TIMING_CFG_0:
			dev->regs.timing_cfg_0 = val;
			break;
		case TIMING_CFG_1:
			dev->regs.timing_cfg_1 = val;
			break;
		case TIMING_CFG_2:
			dev->regs.timing_cfg_2 = val;
			break;
		case DDR_SDRAM_CFG:
			dev->regs.ddr_sdram_cfg = val;
			break;
		case DDR_SDRAM_CFG_2:
			dev->regs.ddr_sdram_cfg_2 = val;
			break;
		case DDR_SDRAM_MODE:
			dev->regs.ddr_sdram_mode = val;
			break;
		case DDR_SDRAM_MODE_2:
			dev->regs.ddr_sdram_mode_2 = val;
			break;
		case DDR_SDRAM_MD_CNTL:
			dev->regs.ddr_sdram_md_cntl = val;
			break;
		case DDR_SDRAM_INTERVAL:
			dev->regs.ddr_sdram_interval = val;
			break;
		case DDR_DATA_INIT:
			dev->regs.ddr_data_init = val;
			break;
		case DDR_SDRAM_CLK_CNTL:
			dev->regs.ddr_sdram_clk_cntl = val;
			break;
		case DDR_INIT_ADDR:
			dev->regs.ddr_init_addr = val;
			break;
		case DDR_IP_REV1:
			dev->regs.ddr_ip_rev1 = val;
			break;
		case DDR_IP_REV2:
			dev->regs.ddr_ip_rev2 = val;
			break;
		case DATA_ERR_INJECT_HI:
			dev->regs.data_err_inject_hi = val;
			break;
		case DATA_ERR_INJECT_LO:
			dev->regs.data_err_inject_lo = val;
			break;
		case ERR_INJECT:
			dev->regs.err_inject = val;
			break;
		case CAPTURE_DATA_HI:
			dev->regs.capture_data_hi = val;
			break;
		case CAPTURE_DATA_LO:
			dev->regs.capture_data_lo = val;
			break;
		case CAPTURE_ECC:
			dev->regs.capture_ecc = val;
			break;
		case ERR_DETECT:
			dev->regs.err_detect = val;
			break;
		case ERR_DISABLE:
			dev->regs.err_disable = val;
			break;
		case ERR_INT_EN:
			dev->regs.err_int_en = val;
			break;
		case CAPTURE_ATTRIBUTES:
			dev->regs.capture_attributes = val;
			break;
		case CAPTURE_ADDRESS:
			dev->regs.capture_address = val;
			break;
		case ERR_SBE:
			dev->regs.err_sbe = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in mpc8378_ddr\n", offset);
			break;
	}
	return 0;
}
static char* mpc8378_ddr_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t mpc8378_ddr_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	mpc8378_ddr_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t mpc8378_ddr_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t mpc8378_ddr_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	mpc8378_ddr_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t mpc8378_ddr_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_mpc8378_ddr(const char *obj_name)
{
	mpc8378_ddr_device* dev = skyeye_mm_zero(sizeof(mpc8378_ddr_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	mpc8378_i2c_regs_init(dev);
	return dev->obj;
}
static exception_t free_mpc8378_ddr(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_mpc8378_ddr(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "mpc8378_ddr",
		.class_desc = "mpc8378_ddr",
		.new_instance = new_mpc8378_ddr,
		.free_instance = free_mpc8378_ddr,
		.module_type = SKYML_ONCHIP(SC_mpc8378),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = mpc8378_ddr_read,
		.write = mpc8378_ddr_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = mpc8378_ddr_get_regvalue_by_id,
		.get_regname_by_id = mpc8378_ddr_get_regname_by_id,
		.set_regvalue_by_id = mpc8378_ddr_set_regvalue_by_id,
		.get_regnum = mpc8378_ddr_get_regnum,
		.get_regid_by_name = mpc8378_ddr_get_regid_by_name
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

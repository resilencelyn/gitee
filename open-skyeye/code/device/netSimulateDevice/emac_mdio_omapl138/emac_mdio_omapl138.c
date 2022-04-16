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
#include "emac_mdio_omapl138.h"

static void reset_lan8710a(struct lan8710a *lregs);
static void lan8710a_read(conf_object_t *opaque, uint32_t *des)
{
	emac_mdio_omapl138_device *dev = opaque->obj;

	switch (BITS(*des, 21, 25)) {
		case BASIC_CONTROL:
			SETS(*des, 0, 15, dev->lan_regs.basic_control);
			break;
		case BASIC_STATUS:
			SETS(*des, 0, 15, dev->lan_regs.basic_status);
			break;
		case PHY_IDENTIFIER1:
			SETS(*des, 0, 15, dev->lan_regs.phy_identifier1);
			break;
		case PHY_IDENTIFIER2:
			SETS(*des, 0, 15, dev->lan_regs.phy_identifier2);
			break;
		case AUTO_NEG_ADV:
			SETS(*des, 0, 15, dev->lan_regs.auto_neg_adv);
			break;
		case AUTO_NEG_LINK:
			SETS(*des, 0, 15, dev->lan_regs.auto_neg_link);
			break;
		case AUTO_NEG_EXP:
			SETS(*des, 0, 15, dev->lan_regs.auto_neg_exp);
			break;
		case MODE_CONTROL_STATUS:
			SETS(*des, 0, 15, dev->lan_regs.mode_control_status);
			break;
		case SPECIAL_MODE:
			SETS(*des, 0, 15, dev->lan_regs.special_mode);
			break;
		case SYMBOL_ERROR:
			SETS(*des, 0, 15, dev->lan_regs.symbol_error);
			break;
		case CONTROL_STATUS_ID:
			SETS(*des, 0, 15, dev->lan_regs.control_status_id);
			break;
		case INT_SRC:
			SETS(*des, 0, 15, dev->lan_regs.int_src);
			break;
		case INT_MASK:
			SETS(*des, 0, 15, dev->lan_regs.int_mask);
			break;
		case PHY_SPECIAL_CONTROL_STATUS:
			SETS(*des, 0, 15, dev->lan_regs.phy_special_control_status);
			break;
		default:
			break;
	}
}
static void lan8710a_write(conf_object_t *opaque, uint32_t src)
{
	emac_mdio_omapl138_device *dev = opaque->obj;
	uint16_t data = BITS(src, 0, 15);
	
	switch (BITS(src, 21, 25)) {
		case BASIC_CONTROL:
			dev->lan_regs.basic_control = data;
			if (BITS(data, 15, 15) == 1) {
				reset_lan8710a(&dev->lan_regs);
				CLRS(dev->lan_regs.basic_control, 15, 15);
			}
			if (BITS(data, 9, 9) == 1) {
				CLRS(dev->lan_regs.basic_control, 9, 9);
			}
			break;
		case BASIC_STATUS:
			dev->lan_regs.basic_status = data;
			break;
		case PHY_IDENTIFIER1:
			dev->lan_regs.phy_identifier1 = data;
			break;
		case PHY_IDENTIFIER2:
			dev->lan_regs.phy_identifier2 = data;
			break;
		case AUTO_NEG_ADV:
			dev->lan_regs.auto_neg_adv = data;
			break;
		case AUTO_NEG_LINK:
			dev->lan_regs.auto_neg_link = data;
			break;
		case AUTO_NEG_EXP:
			dev->lan_regs.auto_neg_exp = data;
			break;
		case MODE_CONTROL_STATUS:
			dev->lan_regs.mode_control_status = data;
			break;
		case SPECIAL_MODE:
			dev->lan_regs.special_mode = data;
			break;
		case SYMBOL_ERROR:
			dev->lan_regs.symbol_error = data;
			break;
		case CONTROL_STATUS_ID:
			dev->lan_regs.control_status_id = data;
			break;
		case INT_SRC:
			dev->lan_regs.int_src = data;
			break;
		case INT_MASK:
			dev->lan_regs.int_mask = data;
			break;
		case PHY_SPECIAL_CONTROL_STATUS:
			dev->lan_regs.phy_special_control_status = data;
			break;
		default:
			break;
	}
}
static exception_t emac_mdio_omapl138_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	emac_mdio_omapl138_device *dev = opaque->obj;
	//printf("EMAC_MDIO------------------R---------offset: 0x%x\n", offset);
	switch (offset)
	{
		case REVID:
			*(uint32_t *)buf = dev->regs.revid;
			break;
		case CONTROL:
			*(uint32_t *)buf = dev->regs.control;
			break;
		case ALIVE:
			*(uint32_t *)buf = dev->regs.alive;
			//printf("alive: 0x%x\n", dev->regs.alive);
			break;
		case LINK:
			*(uint32_t *)buf = dev->regs.link;
			break;
		case LINKINTRAW:
			*(uint32_t *)buf = dev->regs.linkintraw;
			break;
		case LINKINTMASKED:
			*(uint32_t *)buf = dev->regs.linkintmasked;
			break;
		case USERINTRAW:
			*(uint32_t *)buf = dev->regs.userintraw;
			break;
		case USERINTMASKED:
			*(uint32_t *)buf = dev->regs.userintmasked;
			break;
		case USERINTMASKSET:
			*(uint32_t *)buf = dev->regs.userintmaskset;
			break;
		case USERINTMASKCLEAR:
			*(uint32_t *)buf = dev->regs.userintmaskclear;
			break;
		case USERACCESS0:
			*(uint32_t *)buf = dev->regs.useraccess0;
			//printf("useraccess0: 0x%x\n", dev->regs.useraccess0);
			break;
		case USERPHYSEL0:
			*(uint32_t *)buf = dev->regs.userphysel0;
			break;
		case USERACCESS1:
			*(uint32_t *)buf = dev->regs.useraccess1;
			break;
		case USERPHYSEL1:
			*(uint32_t *)buf = dev->regs.userphysel1;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in emac_mdio_omapl138\n", offset);
			break;
	}
	return 0;
}
static exception_t emac_mdio_omapl138_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	emac_mdio_omapl138_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	uint32_t control;
	//printf("EMAC_MDIO------------------W---------offset: 0x%x\n", offset);
	switch (offset)
	{
		case REVID:
			dev->regs.revid = val;
			break;
		case CONTROL:
			control = dev->regs.control;
			dev->regs.control = val;

			/* disable state machine */
			if (BITS(control, 30, 30) == 1
					&& BITS(val, 30, 30) == 0) {
				SETS(dev->regs.control, 31, 31, 1);
			}
			/* enable state machine */
			if (BITS(control, 30, 30) == 0
					&& BITS(val, 30, 30) == 1) {
				CLRS(dev->regs.control, 31, 31);
			}
			if (BITS(val, 19, 19) == 1) {
				CLRS(dev->regs.control, 19, 19);
			}
			break;
		case ALIVE:
			dev->regs.alive &= ~val;
			break;
		case LINK:
			dev->regs.link = val;
			break;
		case LINKINTRAW:
			dev->regs.linkintraw &= ~val;
			break;
		case LINKINTMASKED:
			dev->regs.linkintmasked &= ~val;
			break;
		case USERINTRAW:
			dev->regs.userintraw &= ~val;
			break;
		case USERINTMASKED:
			dev->regs.userintmasked &= ~val;
			break;
		case USERINTMASKSET:
			dev->regs.userintmaskset = val;
			dev->intmask |= val;
			break;
		case USERINTMASKCLEAR:
			dev->regs.userintmaskclear = val;
			dev->intmask &= ~val;
			break;
		case USERACCESS0:
			dev->regs.useraccess0 = val;
			/* GO bit */
			if (BITS(val, 31, 31) == 1) {
				/* do something */
				if (BITS(val, 30, 30) == 0) {
				//printf("--------------READ...PHY_ADDR: 0x%x\tREGADDR: 0x%x\tDATA: 0x%x\n",
				//		BITS(val, 16, 20), BITS(val, 21, 25), BITS(val, 0, 15));
					/* the user command is a read operation */
					lan8710a_read(opaque, &dev->regs.useraccess0);
					SETS(dev->regs.useraccess0, 29, 29, 1);
				} else if (BITS(val, 30, 30) == 1) {
				//printf("++++++++++++++WRITE...PHY_ADDR: 0x%x\tREGADDR: 0x%x\tDATA: 0x%x\n",
				//		BITS(val, 16, 20), BITS(val, 21, 25), BITS(val, 0, 15));
					/* the user command is a write operation */
					lan8710a_write(opaque, dev->regs.useraccess0);
				}
				CLRS(dev->regs.useraccess0, 31, 31);
			}
			break;
		case USERPHYSEL0:
			dev->regs.userphysel0 = val;
			break;
		case USERACCESS1:
			dev->regs.useraccess1 = val;
			/* GO bit */
			if (BITS(val, 31, 31) == 1) {
				/* do something */
				if (BITS(val, 30, 30) == 0) {
					/* the user command is a read operation */
					lan8710a_read(opaque, &dev->regs.useraccess1);
					SETS(dev->regs.useraccess1, 29, 29, 1);
				} else if (BITS(val, 30, 30) == 1) {
					/* the user command is a write operation */
					lan8710a_write(opaque, dev->regs.useraccess1);
				}
				CLRS(dev->regs.useraccess1, 31, 31);
			}
			break;
		case USERPHYSEL1:
			dev->regs.userphysel1 = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in emac_mdio_omapl138\n", offset);
			break;
	}
	return 0;
}
static char* emac_mdio_omapl138_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t emac_mdio_omapl138_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	emac_mdio_omapl138_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t emac_mdio_omapl138_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t emac_mdio_omapl138_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	emac_mdio_omapl138_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t emac_mdio_omapl138_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static void reset_lan8710a(struct lan8710a *lregs)
{
	memset(lregs, 0, sizeof(struct lan8710a));
	/* init lan8710a regs, 8: full duplex, 12: auto negotiation, 13: 100Mps */
	lregs->basic_control		= 1 << 8 | 1 << 12 | 1 << 13;

	lregs->basic_status		= 1 << 0 | 1 << 2 | 1 << 3 | 1 << 5 | 0xf << 11;
	lregs->phy_identifier1		= 0x0007;
	lregs->phy_identifier2		= 0xf << 4 | 0x30 << 10;
	lregs->auto_neg_adv		= 1 | 1 << 7;
	lregs->auto_neg_link		= 1;
	lregs->mode_control_status	= 1 << 1;
	lregs->phy_special_control_status = 1 << 6;
}
static conf_object_t* new_emac_mdio_omapl138(char *obj_name)
{
	emac_mdio_omapl138_device* dev = skyeye_mm_zero(sizeof(emac_mdio_omapl138_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &dev->regs;
	struct lan8710a *lregs = &dev->lan_regs;

	regs->revid	= 0x00070104;
	regs->control	= 0xff << 0 | 0x1 << 24 | 0x1 << 31;

	/* omapl138 lan8710a phy_addr 0x7, C6748 lan8710a phy_addr 0x0, here init lan8710a */
	regs->alive	= 1 | (1 << 0x7);
	regs->link	= 1 | (1 << 0x7);

	reset_lan8710a(lregs);

	return dev->obj;
}
static exception_t free_emac_mdio_omapl138(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_emac_mdio_omapl138(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "omapl138_emac_mdio",
		.class_desc = "omapl138_emac_mdio",
		.new_instance = new_emac_mdio_omapl138,
		.free_instance = free_emac_mdio_omapl138,
		.module_type = SKYML_ONCHIP(SC_omapl138),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = emac_mdio_omapl138_read,
		.write = emac_mdio_omapl138_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = emac_mdio_omapl138_get_regvalue_by_id,
		.get_regname_by_id = emac_mdio_omapl138_get_regname_by_id,
		.set_regvalue_by_id = emac_mdio_omapl138_set_regvalue_by_id,
		.get_regnum = emac_mdio_omapl138_get_regnum,
		.get_regid_by_name = emac_mdio_omapl138_get_regid_by_name
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

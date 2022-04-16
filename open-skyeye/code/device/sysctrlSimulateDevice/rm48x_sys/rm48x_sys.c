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
#include "rm48x_sys.h"

static exception_t rm48x_sys_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_sys_device *dev = opaque->obj;
	switch (offset)
	{
		case SYSPC1:
			*(uint32_t *)buf = dev->regs.syspc1;
			break;
		case SYSPC2:
			*(uint32_t *)buf = dev->regs.syspc2;
			break;
		case SYSPC3:
			*(uint32_t *)buf = dev->regs.syspc3;
			break;
		case SYSPC4:
			*(uint32_t *)buf = dev->regs.syspc4;
			break;
		case SYSPC5:
			*(uint32_t *)buf = dev->regs.syspc5;
			break;
		case SYSPC6:
			*(uint32_t *)buf = dev->regs.syspc6;
			break;
		case SYSPC7:
			*(uint32_t *)buf = dev->regs.syspc7;
			break;
		case SYSPC8:
			*(uint32_t *)buf = dev->regs.syspc8;
			break;
		case SYSPC9:
			*(uint32_t *)buf = dev->regs.syspc9;
			break;
		case CSDIS:
			*(uint32_t *)buf = dev->regs.csdis;
			break;
		case CSDISSET:
			*(uint32_t *)buf = dev->regs.csdisset;
			break;
		case CSDISCLR:
			*(uint32_t *)buf = dev->regs.csdisclr;
			break;
		case CDDIS:
			*(uint32_t *)buf = dev->regs.cddis;
			break;
		case CDDISSET:
			*(uint32_t *)buf = dev->regs.cddisset;
			break;
		case CDDISCLR:
			*(uint32_t *)buf = dev->regs.cddisclr;
			break;
		case GHVSRC:
			*(uint32_t *)buf = dev->regs.ghvsrc;
			break;
		case VCLKASRC:
			*(uint32_t *)buf = dev->regs.vclkasrc;
			break;
		case RCLKSRC:
			*(uint32_t *)buf = dev->regs.rclksrc;
			break;
		case CSVSTAT:
			*(uint32_t *)buf = dev->regs.csvstat;
			dev->regs.csvstat |= ~dev->regs.csdis;
			break;
		case MSTGCR:
			*(uint32_t *)buf = dev->regs.mstgcr;
			break;
		case MINITGCR:
			*(uint32_t *)buf = dev->regs.minitgcr;
			break;
		case MSINENA:
			*(uint32_t *)buf = dev->regs.msinena;
			break;
		case MSTCGSTAT:
			*(uint32_t *)buf = dev->regs.mstcgstat;
			break;
		case MINISTAT:
			*(uint32_t *)buf = dev->regs.ministat;
			break;
		case PLLCTL1:
			*(uint32_t *)buf = dev->regs.pllctl1;
			break;
		case PLLCTL2:
			*(uint32_t *)buf = dev->regs.pllctl2;
			break;
		case SYSPC10:
			*(uint32_t *)buf = dev->regs.syspc10;
			break;
		case DIEIDL:
			*(uint32_t *)buf = dev->regs.dieidl;
			break;
		case DIEIDH:
			*(uint32_t *)buf = dev->regs.dieidh;
			break;
		case LPOMONCTL:
			*(uint32_t *)buf = dev->regs.lpomonctl;
			break;
		case CLKTEST:
			*(uint32_t *)buf = dev->regs.clktest;
			break;
		case DFTCTRLREG:
			*(uint32_t *)buf = dev->regs.dftctrlreg;
			break;
		case DFTCTRLREG2:
			*(uint32_t *)buf = dev->regs.dftctrlreg2;
			break;
		case GPREG1:
			*(uint32_t *)buf = dev->regs.gpreg1;
			break;
		case IMPFASTS:
			*(uint32_t *)buf = dev->regs.impfasts;
			break;
		case IMPFTADD:
			*(uint32_t *)buf = dev->regs.impftadd;
			break;
		case SSIR1:
			*(uint32_t *)buf = dev->regs.ssir1;
			break;
		case SSIR2:
			*(uint32_t *)buf = dev->regs.ssir2;
			break;
		case SSIR3:
			*(uint32_t *)buf = dev->regs.ssir3;
			break;
		case SSIR4:
			*(uint32_t *)buf = dev->regs.ssir4;
			break;
		case RAMGCR:
			*(uint32_t *)buf = dev->regs.ramgcr;
			break;
		case BMMCR1:
			*(uint32_t *)buf = dev->regs.bmmcr1;
			break;
		case RESERVE:
			*(uint32_t *)buf = dev->regs.reserve;
			break;
		case CPURSTCR:
			*(uint32_t *)buf = dev->regs.cpurstcr;
			break;
		case CLKCNTL:
			*(uint32_t *)buf = dev->regs.clkcntl;
			break;
		case ECPCNTL:
			*(uint32_t *)buf = dev->regs.ecpcntl;
			break;
		case DEVCR1:
			*(uint32_t *)buf = dev->regs.devcr1;
			break;
		case SYSECR:
			*(uint32_t *)buf = dev->regs.sysecr;
			break;
		case SYSESR:
			*(uint32_t *)buf = dev->regs.sysesr;
			break;
		case SYSTASR:
			*(uint32_t *)buf = dev->regs.systasr;
			break;
		case GLBSTAT:
			*(uint32_t *)buf = dev->regs.glbstat;
			break;
		case DEVID:
			*(uint32_t *)buf = dev->regs.devid;
			break;
		case SSIVEC:
			*(uint32_t *)buf = dev->regs.ssivec;
			break;
		case SSIF:
			*(uint32_t *)buf = dev->regs.ssif;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rm48x_sys\n", offset);
			break;
	}

	//printf("%s  offset: 0x%x val: 0x%x\n", __func__, offset, *(uint32_t *)buf);
	return 0;
}

static exception_t rm48x_sys_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_sys_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

	//printf("%s offset: 0x%x val: 0x%x\n", __func__, offset, *(uint32_t *)buf);

	switch (offset)
	{
		case SYSPC1:
			dev->regs.syspc1 = val;
			break;
		case SYSPC2:
			dev->regs.syspc2 = val;
			break;
		case SYSPC3:
			dev->regs.syspc3 = val;
			break;
		case SYSPC4:
			dev->regs.syspc4 = val;
			break;
		case SYSPC5:
			dev->regs.syspc5 = val;
			break;
		case SYSPC6:
			dev->regs.syspc6 = val;
			break;
		case SYSPC7:
			dev->regs.syspc7 = val;
			break;
		case SYSPC8:
			dev->regs.syspc8 = val;
			break;
		case SYSPC9:
			dev->regs.syspc9 = val;
			break;
		case CSDIS:
			dev->regs.csdis = val;
			break;
		case CSDISSET:
			dev->regs.csdisset = val;
			if ((dev->regs.csdisset & 0x42) == 0x42){
				dev->regs.csdis |= 0x42;
			}
			break;
		case CSDISCLR:
			dev->regs.csdisclr = val;
			break;
		case CDDIS:
			dev->regs.cddis = val;
			break;
		case CDDISSET:
			dev->regs.cddisset = val;
			break;
		case CDDISCLR:
			dev->regs.cddisclr = val;
			break;
		case GHVSRC:
			dev->regs.ghvsrc = val;
			break;
		case VCLKASRC:
			dev->regs.vclkasrc = val;
			break;
		case RCLKSRC:
			dev->regs.rclksrc = val;
			break;
		case CSVSTAT:
			dev->regs.csvstat = val;
			break;
		case MSTGCR:
			dev->regs.mstgcr = val;
			break;
		case MINITGCR:
			dev->regs.minitgcr = val;
			if (dev->regs.minitgcr == 0xa) {
				dev->regs.mstcgstat |= 0x100;
			}
			break;
		case MSINENA:
			dev->regs.msinena = val;
			break;
		case MSTCGSTAT:
			dev->regs.mstcgstat = val;
			//if ((dev->regs.mstcgstat & 0x100) ==0x100 || (dev->regs.mstcgstat & 0x1) ==0x1) {
			//	dev->regs.mstcgstat &= ~(1 >> 0);
			//	dev->regs.mstcgstat &= ~(1 >> 8);
			//	printf("dev->regs.mstcgstat:0x%x\n",dev->regs.mstcgstat);
			//}
			break;
		case MINISTAT:
			dev->regs.ministat &= ~val;
			break;
		case PLLCTL1:
			dev->regs.pllctl1 = val;
			break;
		case PLLCTL2:
			dev->regs.pllctl2 = val;
			break;
		case SYSPC10:
			dev->regs.syspc10 = val;
			break;
		case DIEIDL:
			dev->regs.dieidl = val;
			break;
		case DIEIDH:
			dev->regs.dieidh = val;
			break;
		case LPOMONCTL:
			dev->regs.lpomonctl = val;
			break;
		case CLKTEST:
			dev->regs.clktest = val;
			break;
		case DFTCTRLREG:
			dev->regs.dftctrlreg = val;
			break;
		case DFTCTRLREG2:
			dev->regs.dftctrlreg2 = val;
			break;
		case GPREG1:
			dev->regs.gpreg1 = val;
			break;
		case IMPFASTS:
			//if ((val & 0x1) == 0x1) {
			//	dev->regs.impfasts = val;
			//	dev->regs.impfasts &= ~(1 << 0);
			//} else {
			//}
			dev->regs.impfasts = val;
			break;
		case IMPFTADD:
			dev->regs.impftadd = val;
			break;
		case SSIR1:
			dev->regs.ssir1 = val;
			break;
		case SSIR2:
			dev->regs.ssir2 = val;
			break;
		case SSIR3:
			dev->regs.ssir3 = val;
			break;
		case SSIR4:
			dev->regs.ssir4 = val;
			break;
		case RAMGCR:
			dev->regs.ramgcr = val;
			break;
		case BMMCR1:
			dev->regs.bmmcr1 = val;
			break;
		case RESERVE:
			dev->regs.reserve = val;
		case CPURSTCR:
			dev->regs.cpurstcr = val;
			break;
		case CLKCNTL:
			dev->regs.clkcntl = val;
			break;
		case ECPCNTL:
			dev->regs.ecpcntl = val;
			break;
		case DEVCR1:
			dev->regs.devcr1 = val;
			break;
		case SYSECR:
			dev->regs.sysecr = val;
			break;
		case SYSESR:
			dev->regs.sysesr = val;
			break;
		case SYSTASR:
			dev->regs.systasr = val;
			break;
		case GLBSTAT:
			//dev->regs.glbstat &= ~val;
			dev->regs.glbstat = val;
			break;
		case DEVID:
			dev->regs.devid = val;
			break;
		case SSIVEC:
			dev->regs.ssivec = val;
			break;
		case SSIF:
			dev->regs.ssif = val;
			//dev->regs.ssif &= ~val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rm48x_sys\n", offset);
			break;
	}
	return 0;
}

static char* rm48x_sys_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t rm48x_sys_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	rm48x_sys_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t rm48x_sys_get_regnum(conf_object_t *conf_obj)
{
	return sizeof(struct registers) / 4;
}

static uint32_t rm48x_sys_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	rm48x_sys_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t rm48x_sys_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if (strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_rm48x_sys(const char *obj_name)
{
	rm48x_sys_device* dev = skyeye_mm_zero(sizeof(rm48x_sys_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->regs.csdis       = 0xce;
	dev->regs.csdisset    = 0xce;
	dev->regs.csdisclr    = 0xce;
	dev->regs.pllctl1     = 0x2f025f00;
	dev->regs.pllctl2     = 0x7fc07200;
	dev->regs.gpreg1      = 0x5ffff;
	dev->regs.vclkasrc    = 0x909;
	dev->regs.rclksrc     = 0x1090109;
	dev->regs.csvstat     = 0x98;
	dev->regs.mstgcr      = 0x5;
	dev->regs.minitgcr    = 0x5;
	dev->regs.lpomonctl   = 0x1001010;
	dev->regs.clktest     = 0xa0000;
	dev->regs.dftctrlreg  = 0x2205;
	dev->regs.dftctrlreg2 = 0x5;
	dev->regs.gpreg1      = 0x5ffff;
	dev->regs.ramgcr      = 0x50000;
	dev->regs.bmmcr1      = 0xa;
	dev->regs.clkcntl     = 0x1010000;
	dev->regs.devcr1      = 0xa0;
	dev->regs.sysecr      = 0x4000;
	return dev->obj;
}

static exception_t free_rm48x_sys(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_rm48x_sys(void)
{
	static skyeye_class_t class_data =
	{
		.class_name    = "rm48x_sys",
		.class_desc    = "rm48x_sys",
		.new_instance  = new_rm48x_sys,
		.free_instance = free_rm48x_sys
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read  = rm48x_sys_read,
		.write = rm48x_sys_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = rm48x_sys_get_regvalue_by_id,
		.get_regname_by_id  = rm48x_sys_get_regname_by_id,
		.set_regvalue_by_id = rm48x_sys_set_regvalue_by_id,
		.get_regnum         = rm48x_sys_get_regnum,
		.get_regid_by_name  = rm48x_sys_get_regid_by_name
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

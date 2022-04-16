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
#include "sata_c6748.h"
static exception_t sata_c6748_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sata_c6748_device *dev = opaque->obj;
	switch (offset)
	{
		case CAP:
			*(uint32_t *)buf = dev->regs.cap.all;
			break;
		case GHC:
			*(uint32_t *)buf = dev->regs.ghc.all;
			break;
		case IS:
			*(uint32_t *)buf = dev->regs.is.all;
			break;
		case PI:
			*(uint32_t *)buf = dev->regs.pi.all;
			break;
		case VS:
			*(uint32_t *)buf = dev->regs.vs.all;
			break;
		case CCC_CTL:
			*(uint32_t *)buf = dev->regs.ccc_ctl.all;
			break;
		case CCC_PORTS:
			*(uint32_t *)buf = dev->regs.ccc_ports;
			break;
		case BISTAFR:
			*(uint32_t *)buf = dev->regs.bistafr.all;
			break;
		case BISTCR:
			*(uint32_t *)buf = dev->regs.bistcr.all;
			break;
		case BISTFCTR:
			*(uint32_t *)buf = dev->regs.bistfctr;
			break;
		case BISTSR:
			*(uint32_t *)buf = dev->regs.bistsr.all;
			break;
		case BISTDECR:
			*(uint32_t *)buf = dev->regs.bistdecr;
			break;
		case TIMER1MS:
			*(uint32_t *)buf = dev->regs.timer1ms.all;
			break;
		case GPARAM1R:
			*(uint32_t *)buf = dev->regs.gparam1r.all;
			break;
		case GPARAM2R:
			*(uint32_t *)buf = dev->regs.gparam2r.all;
			break;
		case PPARAMR:
			*(uint32_t *)buf = dev->regs.pparamr.all;
			break;
		case TESTR:
			*(uint32_t *)buf = dev->regs.testr.all;
			break;
		case VERSIONR:
			*(uint32_t *)buf = dev->regs.versionr;
			break;
		case IDR:
			*(uint32_t *)buf = dev->regs.idr;
			break;
		case P0CLB:
			*(uint32_t *)buf = dev->regs.p0clb.all;
			break;
		case P0FB:
			*(uint32_t *)buf = dev->regs.p0fb.all;
			break;
		case P0IS:
			*(uint32_t *)buf = dev->regs.p0is.all;
			break;
		case P0IE:
			*(uint32_t *)buf = dev->regs.p0ie.all;
			break;
		case P0CMD:
			*(uint32_t *)buf = dev->regs.p0cmd.all;
			break;
		case P0TFD:
			*(uint32_t *)buf = dev->regs.p0tfd.all;
			break;
		case P0SIG:
			*(uint32_t *)buf = dev->regs.p0sig;
			break;
		case P0SSTS:
			*(uint32_t *)buf = dev->regs.p0ssts.all;
			break;
		case P0SCTL:
			*(uint32_t *)buf = dev->regs.p0sctl.all;
			break;
		case P0SERR:
			*(uint32_t *)buf = dev->regs.p0serr.all;
			break;
		case P0SACT:
			*(uint32_t *)buf = dev->regs.p0sact;
			break;
		case P0CI:
			*(uint32_t *)buf = dev->regs.p0ci;
			break;
		case P0SNTF:
			*(uint32_t *)buf = dev->regs.p0sntf.all;
			break;
		case P0DMACR:
			*(uint32_t *)buf = dev->regs.p0dmacr.all;
			break;
		case P0PHYCR:
			*(uint32_t *)buf = dev->regs.p0phycr.all;
			break;
		case P0PHYSR:
			*(uint32_t *)buf = dev->regs.p0physr.all;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sata_c6748\n", offset);
			break;
	}
	return No_exp;
}
static exception_t sata_c6748_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	sata_c6748_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case CAP:
			dev->regs.cap.all = val;
			break;
		case GHC:
			dev->regs.ghc.all = val;
            if(dev->regs.ghc.bit.hr == 0x1){
                /*FIX ME:When set by the software,this bit causes an internal Global reset of the SATASS.
                * ALL state macjines that relate to data transfers and queuing return to and all the Ports 
                * are reinitialized by sending COMRESET if staggered spin-up id not sudpported*/
                dev->regs.ghc.bit.hr = 0x0;
            }
			break;
		case IS:
			dev->regs.is.all = val;
			break;
		case PI:
			dev->regs.pi.all = val;
			break;
		case VS:
			//dev->regs.vs.all = val;
			break;
		case CCC_CTL:
			dev->regs.ccc_ctl.all = val;
			break;
		case CCC_PORTS:
			dev->regs.ccc_ports = val;
			break;
		case BISTAFR:
			//dev->regs.bistafr.all = val;
			break;
		case BISTCR:
			dev->regs.bistcr.all = val;
			break;
		case BISTFCTR:
			dev->regs.bistfctr = val;
			break;
		case BISTSR:
			dev->regs.bistsr.all = val;
			break;
		case BISTDECR:
			//dev->regs.bistdecr = val;
			break;
		case TIMER1MS:
			dev->regs.timer1ms.all = val;
			break;
		case GPARAM1R:
			//dev->regs.gparam1r.all = val;
			break;
		case GPARAM2R:
			//dev->regs.gparam2r.all = val;
			break;
		case PPARAMR:
			//dev->regs.pparamr.all = val;
			break;
		case TESTR:
			dev->regs.testr.all = val;
			break;
		case VERSIONR:
			//dev->regs.versionr = val;
			break;
		case IDR:
			//dev->regs.idr = val;
			break;
		case P0CLB:
			dev->regs.p0clb.all = val & 0xfffffc00;
			break;
		case P0FB:
			dev->regs.p0fb.all = val & 0xffffff00;
			break;
		case P0IS:
			dev->regs.p0is.all = val;
			break;
		case P0IE:
			dev->regs.p0ie.all = val;
			break;
		case P0CMD:
			dev->regs.p0cmd.all = val;
            if(dev->regs.p0cmd.bit.fre == 0x1){
                dev->regs.p0cmd.bit.fr = 0x1;
            }
			break;
		case P0TFD:
			//dev->regs.p0tfd.all = val;
			break;
		case P0SIG:
			//dev->regs.p0sig = val;
			break;
		case P0SSTS:
			//dev->regs.p0ssts.all = val;
			break;
		case P0SCTL:
			dev->regs.p0sctl.all = val;
			break;
		case P0SERR:
			dev->regs.p0serr.all = val;
			break;
		case P0SACT:
			dev->regs.p0sact = val;
			break;
		case P0CI:
			dev->regs.p0ci = val;
			break;
		case P0SNTF:
			dev->regs.p0sntf.all = val;
			break;
		case P0DMACR:
			dev->regs.p0dmacr.all = val;
			break;
		case P0PHYCR:
			dev->regs.p0phycr.all = val;
			break;
		case P0PHYSR:
			//dev->regs.p0physr.all = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in sata_c6748\n", offset);
			break;
	}
	return No_exp;
}
static char* sata_c6748_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t sata_c6748_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	sata_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t sata_c6748_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t sata_c6748_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	sata_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t sata_c6748_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_sata_c6748(char *obj_name)
{
	sata_c6748_device* dev = skyeye_mm_zero(sizeof(sata_c6748_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
    regs->cap.all = 0x6726ff80;
    regs->ghc.all = 0x80000000;
    regs->vs.all = 0x00010100;
    regs->ccc_ctl.all = 0x00010108;
    regs->bistcr.bit.llc = 0x7;
    regs->timer1ms.bit.timv = 0x186a0;
    regs->gparam1r.all = 0xc2410600;
    regs->gparam2r.all = 0x00007296;
    regs->pparamr.all = 0x1;
    regs->versionr = 0x3133302a;
    regs->idr = 0x4e404903;
    regs->p0tfd.bit.sts = 0x7f;
    regs->p0sig = 0xffffffff;
    regs->p0dmacr.all = 0x00000044;
    /*fix me:The value of the following two registers, when the SATA device is inserted 
    * according to the device itself and automatically set the information, here is just 
    * let the program ran*/
    regs->p0ssts.all = 0x3;
    regs->p0sig = 0x101;
	return dev->obj;
}
static exception_t free_sata_c6748(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_sata_c6748(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6748_sata",
		.class_desc = "c6748_sata",
		.new_instance = new_sata_c6748,
		.free_instance = free_sata_c6748,
		.module_type = SKYML_ONCHIP(SC_c6748),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = sata_c6748_read,
		.write = sata_c6748_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = sata_c6748_get_regvalue_by_id,
		.get_regname_by_id = sata_c6748_get_regname_by_id,
		.set_regvalue_by_id = sata_c6748_set_regvalue_by_id,
		.get_regnum = sata_c6748_get_regnum,
		.get_regid_by_name = sata_c6748_get_regid_by_name
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

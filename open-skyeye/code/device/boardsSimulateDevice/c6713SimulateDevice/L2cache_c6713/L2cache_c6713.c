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
#include "L2cache_c6713.h"

#define MAR0 0x8200
#define CEx(val) (val-MAR0)/0x40
#define CEindex(val) ((val-MAR0)-CEx(val)*0x40)


static exception_t L2cache_c6713_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	L2cache_c6713_device *dev = opaque->obj;
    int a=0;
    if((int)(offset-MAR0) >= 0){
        switch(CEindex(offset)+1){
            case CE0:
                *(uint32_t *)buf = dev->regs.mar[CEindex(offset)];
                break;
            case CE1:
                *(uint32_t *)buf = dev->regs.mar[CEindex(offset)];
                break;
            case CE2:
                *(uint32_t *)buf = dev->regs.mar[CEindex(offset)];
                break;
            case CE3:
                *(uint32_t *)buf = dev->regs.mar[CEindex(offset)];
                break;
            default:
                skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in L2cache_c6713\n", offset);
                break;
        }
    }else{
        switch (offset)
        {
            case CCFG:
                *(uint32_t *)buf = dev->regs.ccfg;
                break;
            case L2WBAR:
                *(uint32_t *)buf = dev->regs.l2wbar;
                break;
            case L2WWC:
                *(uint32_t *)buf = dev->regs.l2wwc;
                break;
            case L2WIBAR:
                *(uint32_t *)buf = dev->regs.l2wibar;
                break;
            case L2WIWC:
                if(dev->regs.l2wbinv == 0){
                        dev->regs.l2wiwc = 0;
                }
                *(uint32_t *)buf = dev->regs.l2wiwc;
                break;
            case L1PIBAR:
                *(uint32_t *)buf = dev->regs.l1pibar;
                break;
            case L1PIWC:
                *(uint32_t *)buf = dev->regs.l1piwc;
                break;
            case L1DWIBAR:
                *(uint32_t *)buf = dev->regs.l1dwibar;
                break;
            case L1DWIWC:
                *(uint32_t *)buf = dev->regs.l1dwiwc;
                break;
            case L2WB:
                *(uint32_t *)buf = dev->regs.l2wb;
                break;
            case L2WBINV:
                *(uint32_t *)buf = dev->regs.l2wbinv;
                break;
            default:
                skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in L2cache_c6713\n", offset);
                break;
        }
    }
    //printf("%d,%s,%x,%x\n",__LINE__,__func__,offset,*(uint32_t*)buf);
	return 0;
}
static exception_t L2cache_c6713_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	L2cache_c6713_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
    //printf("%d,%s,%x,%x\n",__LINE__,__func__,offset,val);
    if((int)(offset-MAR0) >= 0){
        switch(CEindex(offset)+1){
            case CE0:
                dev->regs.mar[CEindex(offset)] = val;
                break;
            case CE1:
                dev->regs.mar[CEindex(offset)] = val;
                break;
            case CE2:
                dev->regs.mar[CEindex(offset)] = val;
                break;
            case CE3:
                dev->regs.mar[CEindex(offset)] = val;
                break;
            default:
                skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in L2cache_c6713\n", offset);
                break;
        }
    }else{
        switch (offset)
        {
            case CCFG:
                dev->regs.ccfg = val;
                break;
            case L2WBAR:
                dev->regs.l2wbar = val;
                break;
            case L2WWC:
                dev->regs.l2wwc = val;
                break;
            case L2WIBAR:
                dev->regs.l2wibar = val;
                break;
            case L2WIWC:
                dev->regs.l2wiwc = val;
                break;
            case L1PIBAR:
                dev->regs.l1pibar = val;
                break;
            case L1PIWC:
                dev->regs.l1piwc = val;
                break;
            case L1DWIBAR:
                dev->regs.l1dwibar = val;
                break;
            case L1DWIWC:
                dev->regs.l1dwiwc = val;
                break;
            case L2WB:
                dev->regs.l2wb = val;
                break;
            case L2WBINV:
                dev->regs.l2wbinv = val;
                break;
            default:
                skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in L2cache_c6713\n", offset);
                break;
        }
    }
	return 0;
}
static char* L2cache_c6713_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t L2cache_c6713_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	L2cache_c6713_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t L2cache_c6713_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	//return  sizeof(struct registers) / 4;
	return 11;
}
static uint32_t L2cache_c6713_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	L2cache_c6713_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t L2cache_c6713_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_L2cache_c6713(char *obj_name)
{
	L2cache_c6713_device* dev = skyeye_mm_zero(sizeof(L2cache_c6713_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
    dev->num = 0;
	return dev->obj;
}
static exception_t free_L2cache_c6713(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_L2cache_c6713(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6713_L2cache",
		.class_desc = "c6713_L2cache",
		.new_instance = new_L2cache_c6713,
		.free_instance = free_L2cache_c6713,
		.module_type = SKYML_ONCHIP(SC_c6713),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = L2cache_c6713_read,
		.write = L2cache_c6713_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = L2cache_c6713_get_regvalue_by_id,
		.get_regname_by_id = L2cache_c6713_get_regname_by_id,
		.set_regvalue_by_id = L2cache_c6713_set_regvalue_by_id,
		.get_regnum = L2cache_c6713_get_regnum,
		.get_regid_by_name = L2cache_c6713_get_regid_by_name
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

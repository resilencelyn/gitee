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
#include <skyeye_system.h>
#include "rm48x_timer.h"


void delay()
{
	int i = 0;
	for(i=0;i<100*1000;i++);
}

static void timer_fun_uc0(conf_object_t *obj)
{
	rm48x_timer_device *dev = (rm48x_timer_device *)obj->obj;
	core_signal_intf* signal_iface = dev->signal_iface;
	dev->regs.rtiuc0++;
	if(dev->regs.rtiuc0 == dev->regs.rticpuc0)
	{
		delay();
		//signal_iface->signal(dev->core, 6);
		dev->regs.rtiuc0 = 0;
	}	
	/*compare mode*/
	/*if(dev->frc0)
	  {
	  if((++dev->regs.rtifrc0) == dev->regs.rticomp0)
	//signal_iface->signal(dev->core, 2);		
	if((++dev->regs.rtifrc0) == dev->regs.rticomp1)
	//signal_iface->signal(dev->core, 3);	
	if((++dev->regs.rtifrc0) == dev->regs.rticomp2)
	//signal_iface->signal(dev->core, 4);	
	if((++dev->regs.rtifrc0) == dev->regs.rticomp3)
	//signal_iface->signal(dev->core, 5);	

	if((++dev->regs.rtifrc0) == dev->regs.rticomp0 || (++dev->regs.rtifrc1) == dev->regs.rticomp1)
	signal_iface->signal(dev->core, 6);

	}*/

}

static void timer_fun_uc1(conf_object_t *obj)
{
	rm48x_timer_device *dev = (rm48x_timer_device *)obj->obj;
	core_signal_intf* signal_iface = dev->signal_iface;


	dev->regs.rtiuc1++;
	if(dev->regs.rticpuc1 == dev->regs.rtiuc1)
	{
		//signal_iface->signal(dev->core, 7);
		dev->regs.rtiuc1 = 0;

		/*compare mode*/

		/*if(dev->frc1)
		  {
		  if((++dev->regs.rtifrc1) == dev->regs.rticomp0)
		  signal_iface->signal(dev->core, 2);		
		  if((++dev->regs.rtifrc1) == dev->regs.rticomp1)
		  signal_iface->signal(dev->core, 3);	
		  if((++dev->regs.rtifrc1) == dev->regs.rticomp2)
		  signal_iface->signal(dev->core, 4);	
		  if((++dev->regs.rtifrc1) == dev->regs.rticomp3)
		  signal_iface->signal(dev->core, 5);	

#if 0
if((++dev->regs.rtifrc1) == dev->regs.rticomp2 || (++dev->regs.rtifrc1) == dev->regs.rticomp3)
signal_iface->signal(dev->core, 7);
#endif
}
*/
		}
}


static void system_reset(rm48x_timer_device *dev)
{
	dev->regs.rtidwdctrl = 0x5312aced;
	dev->regs.rtidwdprld = 0xfff;
	dev->regs.rtiwdstatus = 0;
	dev->regs.rtiwdkey = 0xa35c;
	dev->regs.rtidwdcntr = 0x1ffffff;
	dev->regs.rtiwwdrxnctrl = 0x5;
	dev->regs.rtiwwdsizectrl = 0x5;
	dev->regs.rtiintclrenable = 0x05050505;
}

static void intflag_w1c(rm48x_timer_device *dev)
{
	uint32_t value = 0;
	switch(value & dev->regs.rtiintflag)
	{
		case RTIINTFLAG_OVL1INT:
			dev->regs.rtiintflag &= ~(RTIINTFLAG_OVL1INT);
			break;
		case RTIINTFLAG_OVEL0INT:
			dev->regs.rtiintflag &= ~(RTIINTFLAG_OVEL0INT);
			break;
		case RTIINTFLAG_TBINT:
			dev->regs.rtiintflag &= ~(RTIINTFLAG_TBINT);
			break;	
		case RTIINTFLAG_INT3:
			dev->regs.rtiintflag &= ~(RTIINTFLAG_INT3);
			break;	
		case RTIINTFLAG_INT2:
			dev->regs.rtiintflag &= ~(RTIINTFLAG_INT2);
			break;
		case RTIINTFLAG_INT1:
			dev->regs.rtiintflag &= ~(RTIINTFLAG_INT1);
			break;	
		case RTIINTFLAG_INT0:
			dev->regs.rtiintflag &= ~(RTIINTFLAG_INT0);
			break;
		default:
			break;
	}
}


static void watchdog_reset(rm48x_timer_device *dev)
{
	dev->regs.rtidwdctrl |= 0x5312aced;
}


static exception_t rm48x_timer_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	//skyeye_log(Error_log, __FUNCTION__, "[READ] offset :%x, %s\n", offset, opaque->objname);
	rm48x_timer_device *dev = opaque->obj;
	switch (offset)
	{
		case RTIGCTRL:
			*(uint32_t *)buf = dev->regs.rtigctrl;
			break;
		case RTITBCTRL:
			*(uint32_t *)buf = dev->regs.rtitbctrl;
			break;
		case RTICAPCTRL:
			*(uint32_t *)buf = dev->regs.rticapctrl;
			break;
		case RTICOMPCTRL:
			*(uint32_t *)buf = dev->regs.rticompctrl;
			break;
		case RTIFRC0:
			*(uint32_t *)buf = dev->regs.rtifrc0;
			break;
		case RTIUC0:
			*(uint32_t *)buf = dev->regs.rtiuc0;
			break;
		case RTICPUC0:
			*(uint32_t *)buf = dev->regs.rticpuc0;
			break;
		case RTICAFRC0:
			*(uint32_t *)buf = dev->regs.rticafrc0;
			dev->frc0 = 1;
			break;
		case RTICAUC0:
			*(uint32_t *)buf = dev->regs.rticauc0;
			break;
		case RTIFRC1:
			*(uint32_t *)buf = dev->regs.rtifrc1;
			break;
		case RTIUC1:
			*(uint32_t *)buf = dev->regs.rtiuc1;
			break;
		case RTICPUC1:
			*(uint32_t *)buf = dev->regs.rticpuc1;
			break;
		case RTICAFRC1:
			*(uint32_t *)buf = dev->regs.rticafrc1;
			dev->frc1 = 1;
			break;
		case RTICAUC1:
			*(uint32_t *)buf = dev->regs.rticauc1;
			break;
		case RTICOMP0:
			*(uint32_t *)buf = dev->regs.rticomp0;
			break;
		case RTIUDCP0:
			*(uint32_t *)buf = dev->regs.rtiudcp0;
			break;
		case RTICOMP1:
			*(uint32_t *)buf = dev->regs.rticomp1;
			break;
		case RTIUDCP1:
			*(uint32_t *)buf = dev->regs.rtiudcp1;
			break;
		case RTICOMP2:
			*(uint32_t *)buf = dev->regs.rticomp2;
			break;
		case RTIUDCP2:
			*(uint32_t *)buf = dev->regs.rtiudcp2;
			break;
		case RTICOMP3:
			*(uint32_t *)buf = dev->regs.rticomp3;
			break;
		case RTIUDCP3:
			*(uint32_t *)buf = dev->regs.rtiudcp3;
			break;
		case RTITBLCOMP:
			*(uint32_t *)buf = dev->regs.rtitblcomp;
			break;
		case RTITBHCOMP:
			*(uint32_t *)buf = dev->regs.rtitbhcomp;
			break;
		case RTISETINTENA:
			*(uint32_t *)buf = dev->regs.rtisetintena;
			break;
		case RTICLEARINTENA:
			*(uint32_t *)buf = dev->regs.rticlearintena;
			break;
		case RTIINTFLAG:
			*(uint32_t *)buf = dev->regs.rtiintflag;
			break;
		case RTIDWDCTRL:
			*(uint32_t *)buf = dev->regs.rtidwdctrl;
			break;
		case RTIDWDPRLD:
			*(uint32_t *)buf = dev->regs.rtidwdprld;
			break;
		case RTIWDSTATUS:
			*(uint32_t *)buf = dev->regs.rtiwdstatus;
			if(dev->regs.rtiwdstatus &= (1<<5))
				system_reset(dev);
			break;
		case RTIWDKEY:
			*(uint32_t *)buf = dev->regs.rtiwdkey;
			break;
		case RTIDWDCNTR:
			*(uint32_t *)buf = dev->regs.rtidwdcntr;
			break;
		case RTIWWDRXNCTRL:
			*(uint32_t *)buf = dev->regs.rtiwwdrxnctrl;
			break;
		case RTIWWDSIZECTRL:
			*(uint32_t *)buf = dev->regs.rtiwwdsizectrl;
			break;
		case RTIINTCLRENABLE:
			*(uint32_t *)buf = dev->regs.rtiintclrenable;
			break;
		case RTICOMP0CLR:
			*(uint32_t *)buf = dev->regs.rticomp0clr;
			break;
		case RTICOMP1CLR:
			*(uint32_t *)buf = dev->regs.rticomp1clr;
			break;
		case RTICOMP2CLR:
			*(uint32_t *)buf = dev->regs.rticomp2clr;
			break;
		case RTICOMP3CLR:
			*(uint32_t *)buf = dev->regs.rticomp3clr;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rm48x_timer\n", offset);
			break;
	}
	return 0;
}
static exception_t rm48x_timer_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_timer_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	uint32_t current_value0;
	switch (offset)
	{
		case RTIGCTRL:
			dev->regs.rtigctrl = val;
			break;
		case RTITBCTRL:
			dev->regs.rtitbctrl = val;
			break;
		case RTICAPCTRL:
			dev->regs.rticapctrl = val;
			break;
		case RTICOMPCTRL:
			dev->regs.rticompctrl = val;
			break;
		case RTIFRC0:
			if(dev->regs.rtigctrl | RTIGCTRL_CNT0EN)
				dev->regs.rtigctrl &= ~(RTIGCTRL_CNT0EN);
			dev->regs.rtifrc0 = val;	
			dev->regs.rtigctrl |= (RTIGCTRL_CNT0EN);
			break;
		case RTIUC0:
			if(dev->regs.rtigctrl | RTIGCTRL_CNT0EN)
				dev->regs.rtigctrl &= ~(RTIGCTRL_CNT0EN);
			dev->regs.rtigctrl |= (RTIGCTRL_CNT0EN);
			dev->regs.rtiuc0 = val;
			//printf("12312\n");
			if(dev->regs.rtigctrl &= RTIGCTRL_CNT0EN)
			{
				create_thread_scheduler((unsigned int)50000, Periodic_sched, timer_fun_uc0, opaque, &dev->sched_id0);
			}
			if(dev->regs.rtigctrl &= ~RTIGCTRL_CNT0EN)
			{

			}
			//system_register_timer_handler(dev->obj, 100,(time_sched_t)timer_fun_uc0, SCHED_US | SCHED_NORMAL);
			break;
		case RTICPUC0:
			dev->regs.rticpuc0 = val;
			break;
		case RTICAFRC0:
			dev->regs.rticafrc0 = val;
			break;
		case RTICAUC0:
			dev->regs.rticauc0 = val;
			break;
		case RTIFRC1:
			if(dev->regs.rtigctrl | RTIGCTRL_CNT1EN)
				dev->regs.rtigctrl &= ~(RTIGCTRL_CNT1EN);
			dev->regs.rtigctrl |= (RTIGCTRL_CNT1EN);

			dev->regs.rtifrc1 = val;
			break;
		case RTIUC1:
			if(dev->regs.rtigctrl | RTIGCTRL_CNT1EN)
				dev->regs.rtigctrl &= ~(RTIGCTRL_CNT1EN);
			dev->regs.rtigctrl |= (RTIGCTRL_CNT1EN);

			dev->regs.rtiuc1 = val;

			if(dev->regs.rtigctrl &= RTIGCTRL_CNT1EN)
			{	
				val |= val;
				create_thread_scheduler((unsigned int)50000, Periodic_sched, timer_fun_uc1, opaque, &dev->sched_id1);
			}
			break;
		case RTICPUC1:
			dev->regs.rticpuc1 = val;
			break;
		case RTICAFRC1:
			dev->regs.rticafrc1 = val;
			break;
		case RTICAUC1:
			dev->regs.rticauc1 = val;
			break;
		case RTICOMP0:
			dev->regs.rticomp0 = val;
			break;
		case RTIUDCP0:
			dev->regs.rtiudcp0 = val;
			break;
		case RTICOMP1:
			dev->regs.rticomp1 = val;
			break;
		case RTIUDCP1:
			dev->regs.rtiudcp1 = val;
			break;
		case RTICOMP2:
			dev->regs.rticomp2 = val;
			break;
		case RTIUDCP2:
			dev->regs.rtiudcp2 = val;
			break;
		case RTICOMP3:
			dev->regs.rticomp3 = val;
			break;
		case RTIUDCP3:
			dev->regs.rtiudcp3 = val;
			break;
		case RTITBLCOMP:
			dev->regs.rtitblcomp = val;				
			break;
		case RTITBHCOMP:
			dev->regs.rtitbhcomp = val;
			break;
		case RTISETINTENA:
			dev->regs.rtisetintena = val;
			break;
		case RTICLEARINTENA:
			dev->regs.rticlearintena = val;
			break;
		case RTIINTFLAG:
			dev->regs.rtiintflag = val;
			intflag_w1c(dev);
			break;
		case RTIDWDCTRL:
			dev->regs.rtidwdctrl = val;
			break;
		case RTIDWDPRLD:
			if((dev->regs.rtidwdctrl & 0xa98559da) != 1)
				dev->regs.rtidwdprld = val;
			break;
		case RTIWDSTATUS:
			dev->regs.rtiwdstatus = val;
			if(val & RTIWDSTATUS_DWWD_ST)
				dev->regs.rtiwdstatus &= ~(RTIWDSTATUS_DWWD_ST);
			if(val & RTIWDSTATUS_END_TIME_VIOL)
				dev->regs.rtiwdstatus &= ~(RTIWDSTATUS_END_TIME_VIOL);
			if(val & RTIWDSTATUS_START_TIME_VIOL)
				dev->regs.rtiwdstatus &= ~(RTIWDSTATUS_START_TIME_VIOL);
			if(val & RTIWDSTATUS_KEY_ST)
				dev->regs.rtiwdstatus &= ~(RTIWDSTATUS_KEY_ST);
			if(val & RTIWDSTATUS_DWD_ST)
				dev->regs.rtiwdstatus &= ~(RTIWDSTATUS_DWD_ST);
			break;
		case RTIWDKEY:
			dev->old_wdkey = dev->regs.rtiwdkey;
			dev->regs.rtiwdkey = val;
			if((dev->old_wdkey == 0xe51a)&& (val == 0xa35c))
				watchdog_reset(dev);
			else if(dev->old_wdkey == 0xe51a && (val == 0x2345))
				dev->regs.rtiwdstatus |= RTIWDSTATUS_DWD_ST;
			else
				dev->regs.rtiwdstatus |= RTIWDSTATUS_KEY_ST;	
			break;
		case RTIDWDCNTR:
			dev->regs.rtidwdcntr = val;
			break;
		case RTIWWDRXNCTRL:
			dev->regs.rtiwwdrxnctrl = val;
			break;
		case RTIWWDSIZECTRL:
			dev->regs.rtiwwdsizectrl = val;
			break;
		case RTIINTCLRENABLE:
			dev->regs.rtiintclrenable = val;
			break;
		case RTICOMP0CLR:
			dev->regs.rticomp0clr = val;
			break;
		case RTICOMP1CLR:
			dev->regs.rticomp1clr = val;
			break;
		case RTICOMP2CLR:
			dev->regs.rticomp2clr = val;
			break;
		case RTICOMP3CLR:
			dev->regs.rticomp3clr = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rm48x_timer\n", offset);
			break;
	}
	//skyeye_log(Error_log, __FUNCTION__, "[WRITE] offset :%x, %s\n", offset, opaque->objname);
	return 0;
}
static char* rm48x_timer_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t rm48x_timer_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	rm48x_timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t rm48x_timer_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t rm48x_timer_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	rm48x_timer_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t rm48x_timer_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rm48x_timer_device *dev = obj->obj;
	dev->core = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->core, CORE_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->core == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	rm48x_timer_device *dev = obj->obj;
	*obj2 = dev->core;
	*port = NULL;
	return No_exp;
}


static exception_t get_attr_test_value(void* arg, conf_object_t* obj, attr_value_t* idx){
	rm48x_timer_device *dev = obj->obj;
	attr_value_t test_value = SKY_make_attr_uinteger(dev->test_value);
	return No_exp;
}

static exception_t set_attr_test_value(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	rm48x_timer_device *dev = obj->obj;
	dev->test_value = SKY_attr_uinteger(*value);
	return No_exp;
}

/*attr*/
static systimer_register_attribute(conf_object_t* clss){
	SKY_register_attribute(clss, "test_value", get_attr_test_value, NULL, set_attr_test_value, NULL, SKY_Attr_Optional, "uinteger", "timer test value");
}



static conf_object_t* new_rm48x_timer(const char *obj_name)
{	
	rm48x_timer_device* dev = skyeye_mm_zero(sizeof(rm48x_timer_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->sched_id0 = -1;
	dev->sched_id1 = -1;
	dev->frc0 = -1;
	dev->frc1 = -1;
	system_reset(dev);
	return dev->obj;
}
static exception_t free_rm48x_timer(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_rm48x_timer(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "rm48x_timer",
		.class_desc = "rm48x_timer",
		.new_instance = new_rm48x_timer,
		.free_instance = free_rm48x_timer
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = rm48x_timer_read,
		.write = rm48x_timer_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = rm48x_timer_get_regvalue_by_id,
		.get_regname_by_id = rm48x_timer_get_regname_by_id,
		.set_regvalue_by_id = rm48x_timer_set_regvalue_by_id,
		.get_regnum = rm48x_timer_get_regnum,
		.get_regid_by_name = rm48x_timer_get_regid_by_name
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
		(struct ConnectDescription){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
		/* ConnectDescription */
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	systimer_register_attribute(clss);
}

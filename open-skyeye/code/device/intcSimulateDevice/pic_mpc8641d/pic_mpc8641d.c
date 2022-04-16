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
#include "pic_mpc8641d.h"

#define IPI0 (1 >> 0)
uint32_t get_cpu_register(conf_object_t *cpu, char *name){
	if(!SKY_has_attribute(cpu, name)){
		attr_value_t attr = SKY_get_attribute(cpu, name);
		return SKY_attr_uinteger(attr);
	}else{
		skyeye_log(Error_log, __FUNCTION__, "%s not has attribute %s\n", cpu->objname, name);
		return  0;
	}
}

void set_cpu_register(conf_object_t *cpu, char *name, uint32_t value){
	attr_value_t attr;
	attr = SKY_make_attr_uinteger(value);
	if(!SKY_has_attribute(cpu, name)){
		SKY_set_attribute(cpu, name, &attr);
	}else{
		skyeye_log(Error_log, __FUNCTION__, "%s not has attribute %s\n", cpu->objname, name);
	}
}

static exception_t pic_mpc8641d_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	pic_mpc8641d_device *dev = opaque->obj;
	switch (offset)
	{
			case 0x90:
				*(uint32_t *)buf = get_cpu_register(dev->cpu, "pir");
				return No_exp;
			case 0xa0:{
				uint32_t ipr = get_cpu_register(dev->cpu, "ipr");
				uint32_t pir = get_cpu_register(dev->cpu, "pir");
				if(ipr == 0){
					*(uint32_t *)buf = dev->pic_global.svr;
				}else{
					*(uint32_t *)buf = dev->pic_percpu.iack[pir];
				}
				return No_exp;

			}
			case 0x1000:
				*(uint32_t *)buf = dev->pic_global.frr;	/* according to MPC8572 manual */
				return No_exp;
			case 0x1020:
			/* source attribute register for DMA0 */
				*(uint32_t *)buf = dev->pic_global.gcr & ~0x80000000;	/* we clear RST bit after finish initialization of PIC */
				return No_exp;
			case 0x1098:
				*(uint32_t *)buf = dev->pic_global.prr;
				return No_exp;
			case 0x10a0:
			case 0x10b0:
			case 0x10c0:
			case 0x10d0:
				*(uint32_t *)buf = dev->mpic.ipivpr[(offset - 0x10a0) >> 4];
				return No_exp;
			case 0x10e0:
				*(uint32_t *)buf = dev->pic_global.svr;
				break;
			case 0x10f0:
				*(uint32_t *)buf = dev->pic_global.tfrr;
				break;
			case 0x1100:
				*(uint32_t *)buf = dev->pic_global.gtccra0;
				break;
			case 0x1110:
				*(uint32_t *)buf = dev->pic_global.gtbcra0;
				break;
			case 0x1120:
				*(uint32_t *)buf = dev->pic_global.gtvpr0;
				break;
			case 0x1130:
				*(uint32_t *)buf = dev->pic_global.gtdr0;
				break;
			case 0x1150:
				*(uint32_t *)buf = dev->pic_global.gtbcra1;
				break;
			case 0x1160:
				*(uint32_t *)buf = dev->pic_global.gtvpr1;
				break;
			case 0x1170:
				*(uint32_t *)buf = dev->pic_global.gtdr1;
				break;
			case 0x1190:
				*(uint32_t *)buf = dev->pic_global.gtbcra2;
				break;
			case 0x11a0:
				*(uint32_t *)buf = dev->pic_global.gtvpr2;
				break;
			case 0x11B0:
				*(uint32_t *)buf = dev->pic_global.gtdr2;
				break;
			case 0x11E0:
				*(uint32_t *)buf = dev->pic_global.gtvpr3;
				break;
			case 0x11D0:
				*(uint32_t *)buf = dev->pic_global.gtbcra3;
				break;
			case 0x1300:
				*(uint32_t *)buf = dev->pic_global.tcra;
				break;

		default:
			break;
	}

	if (offset >= 0x10000 && offset <= 0x10170) {
			int index = (offset - 0x10000) >> 4;
			if (index & 0x1){
				*(uint32_t *)buf = dev->pic_ram.eidr[index >> 1];
				return No_exp;
			}else{
				*(uint32_t *)buf = dev->pic_ram.eivpr[index >> 1];
				return No_exp;
			}
	}
	if (offset >= 0x10200 && offset <= 0x109F0) {
			int index = (offset - 0x10200) >> 4;
			if (index & 0x1){
				*(uint32_t *)buf =  dev->mpic.iidr[index >> 1];
				return No_exp;
			}else{
				*(uint32_t *)buf =  dev->mpic.iivpr[index >> 1];
				return No_exp;
			}
		}
		if (offset >= 0x11600 && offset <= 0x11670) {
			int index = (offset - 0x11600) >> 4;
			if (index & 0x1){
				*(uint32_t *)buf =dev->mpic.midr[index >> 1];
				return No_exp;
			}else{
				*(uint32_t *)buf =dev->mpic.mivpr[index >> 1];
				return No_exp;
			}
		}

		if (offset >= 0x11680 && offset <= 0x11bf0){
			*(uint32_t *)buf = 0;
			return No_exp;
		} /* Reserved region for MPC8641d*/

		if (offset >= 0x11c00 && offset <= 0x11cf0) {
			int index = (offset - 0x11c00) >> 4;
			if (index & 0x1){
				*(uint32_t *)buf = dev->mpic.msidr[index >> 1];
			}else{
				*(uint32_t *)buf =dev->mpic.msivpr[index >> 1];
			}
			return No_exp;
		}

		if (offset >= 0x11d00 && offset <= 0x1fff0){	 /* Reserved region for MPC8641d*/
			*(uint32_t *)buf  = 0;
			return No_exp;
		}
	switch (offset) {
			case 0x20080:
				*(uint32_t *)buf = dev->pic_ram.ctpr0;
				return No_exp;
			case 0x20090:
				*(uint32_t *)buf =  0;	/* Who as I register for core 0 */
				return No_exp;
			case 0x21090:
				*(uint32_t *)buf =  0x1;	/* Who am I register for core 1 */
				return No_exp;
			case 0x200a0:
				if(get_cpu_register(dev->cpu, "ipr") == 0){
					*(uint32_t *)buf =  dev->pic_global.svr;
					return No_exp;
				}else{
					*(uint32_t *)buf = dev->pic_percpu.iack[0];
					return No_exp;
				}
			case 0x210a0:
				if(get_cpu_register(dev->cpu, "ipr") == 0){
					*(uint32_t *)buf = dev->pic_global.svr;
					return No_exp;
				}else{
					*(uint32_t *)buf = dev->pic_percpu.iack[1];
					return No_exp;
				}
			default:
				break;
	}
	return Not_found_exp;
}
static exception_t pic_mpc8641d_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	pic_mpc8641d_device *dev = opaque->obj;
	uint32_t data = *(uint32_t *)buf;
	offset = offset + 0x40000;
	/* PIC Register map: global register */
	if (offset >= 0x40000 && offset <= 0x4FFF0) {
		switch (offset) {
		case 0x41020:
			/* source a
			   ttribute register for DMA0 */
			dev->pic_global.gcr = data;
			break;
		case 0x410e0:
			dev->pic_global.svr = data;
			break;
		case 0x41120:
			dev->pic_global.gtvpr0 = data;
			break;
		case 0x41130:
			dev->pic_global.gtdr0 = data;
			break;
		case 0x41160:
			dev->pic_global.gtvpr1 = data;
			break;
		case 0x41170:
			dev->pic_global.gtdr1 = data;
			break;
		case 0x411a0:
			dev->pic_global.gtvpr2 = data;
			break;
		case 0x411B0:
			dev->pic_global.gtdr2 = data;
			break;
		case 0x411E0:
			dev->pic_global.gtvpr3 = data;
			break;
		case 0x411F0:
			dev->pic_global.gtdr3 = data;
			break;
		case 0x400b0:{
				uint32_t pir = get_cpu_register(dev->cpu, "pir");
				uint32_t ipr = get_cpu_register(dev->cpu, "ipr");
				dev->pic_ram.eoi[pir] = data;
				if (ipr & (1 << dev->irq_num)) {
					ipr &= ~(1 << dev->irq_num);
					set_cpu_register(dev->cpu, "ipr", ipr);
				}
				/* clear the interrupt with highest priority in ISR */
				if (ipr & IPI0) {
					ipr &= ~IPI0;
					set_cpu_register(dev->cpu, "ipr", ipr);
				}
			}
				break;
			case 0x410a0:
			case 0x410b0:
			case 0x410c0:
			case 0x410d0:
				dev->mpic.ipivpr[(offset - 0x410a0) >> 4] = data;
				break;
			case 0x41098:
				dev->pic_global.prr = data;
				break;
				break;
			case 0x41500:
				dev->pic_global.mer = data;
				break;
			case 0x41300:
				dev->pic_global.tcra = data;
				break;
			default:
				return Not_found_exp;
		}
		return No_exp;
	}
	if (offset >= 0x50000 && offset <= 0x5FFF0) {
		if (offset >= 0x50000 && offset <= 0x50170) {
			int index = (offset - 0x50000) >> 4;
			if (index & 0x1)
				dev->pic_ram.eidr[index >> 1] = data;
			else
				dev->pic_ram.eivpr[index >> 1] = data;
			return No_exp;
		}
		if (offset >= 0x50200 && offset <= 0x509F0) {
			int index = (offset - 0x50200) >> 4;
			if (index & 0x1)
				dev->mpic.iidr[index >> 1] = data;
			else
				dev->mpic.iivpr[index >> 1] = data;
			return No_exp;
		}
		if (offset >= 0x50180 && offset <= 0x501f0)	/* Reserved region for MPC8572 */
			return No_exp;
		if (offset >= 0x50100 && offset <= 0x515f0)	/* Reserved region for MPC8572 */
			return No_exp;

		if (offset >= 0x51600 && offset <= 0x51670) {
			int index = (offset - 0x51600) >> 4;
			if (index & 0x1)
				dev->mpic.midr[index >> 1] = data;
			else
				dev->mpic.mivpr[index >> 1] = data;
			return No_exp;
		}

		if (offset >= 0x51680 && offset <= 0x51bf0)	 /* Reserved region for MPC8641d*/
			return No_exp;

		if (offset >= 0x51c00 && offset <= 0x51cf0) {
			int index = (offset - 0x51c00) >> 4;
			if (index & 0x1)
				dev->mpic.msidr[index >> 1] = data;
			else
				dev->mpic.msivpr[index >> 1] = data;
			return No_exp;
		}

		if (offset >= 0x51d00 && offset <= 0x5fff0)	 /* Reserved region for MPC8641d*/
			return No_exp;

		return No_exp;
	}

	/* per-CPU */
	if (offset >= 0x60000 && offset <= 0x7FFF0) {
		switch (offset) {
			case 0x60050:
				{
				int reg_no = (offset >> 4) & 0x3;
				dev->mpic.ipidr[reg_no] = data;
				int core_id = -1;
				if (data & 0x1)	/* dispatch the interrupt to core 0 */
					core_id = 0;		
				if (data & 0x2)	/* dispatch the interrupt to core 1 */
					core_id = 1;
				if(data & 0x3){	
					/* trigger an interrupt to dedicated core */
					//e500_core_t* core = &cpu->core[core_id];
					uint32_t ipr = get_cpu_register(dev->cpu, "ipr");
					set_cpu_register(dev->cpu, "ipr", ipr);
					dev->mpic.ipivpr[1] |= 0x40000000; /* set activity bit in vpr */
					dev->pic_percpu.iack[core_id] = (dev->pic_percpu.iack[core_id] & 0xFFFF0000) | (dev->mpic.ipivpr[1] & 0xFFFF);
					//ppc_exception(core, PPC_EXC_EXT_INT, 0, 0); fix me 
					set_cpu_register(dev->cpu, "ipi_flag", 1);
				}
				return No_exp;
				}
			case 0x60060:
				{
				int reg_no = (offset >> 4) & 0x3;
				dev->mpic.ipidr[reg_no] = data;
				int core_id = -1;
				if (data & 0x1)	/* dispatch the interrupt to core 0 */
					core_id = 0;		
				if (data & 0x2)	/* dispatch the interrupt to core 1 */
					core_id = 1;
				if(data & 0x3){	
					/* trigger an interrupt to dedicated core */
					//e500_core_t* core = &cpu->core[core_id];
					uint32_t ipr = get_cpu_register(dev->cpu, "ipr");
				        ipr |= IPI0;
					set_cpu_register(dev->cpu, "ipr", ipr);
				        dev->mpic.ipivpr[2] |= 0x40000000; /* set activity bit in vpr */
			        	dev->pic_percpu.iack[core_id] = (dev->pic_percpu.iack[core_id] & 0xFFFF0000) | (dev->mpic.ipivpr[2] & 0xFFFF);
			        	//ppc_exception(dev->cpu, PPC_EXC_EXT_INT, 0, 0); fix me
			        	/* we need to inform the core that npc is changed to exception vector */
					set_cpu_register(dev->cpu, "ipi_flag", 1);
				}
				return No_exp;
				}
			case 0x60080:
				dev->pic_ram.ctpr0 = data;
				return No_exp;
			case 0x600b0:{
				uint32_t ipr = get_cpu_register(dev->cpu, "ipr");
				dev->pic_ram.eoi[0] = data;
				if (ipr & (1 << dev->irq_num)) {
					ipr &= ~(1 << dev->irq_num);
				}
				/* clear the interrupt with highest priority in ISR */
				if (ipr & IPI0) {
					ipr &= ~IPI0;
				}
				set_cpu_register(dev->cpu, "ipr", ipr);

				return No_exp;
			}
			case 0x61050:
				{
				uint32_t ipr = get_cpu_register(dev->cpu, "ipr");
				int reg_no = (offset >> 4) & 0x3;
				dev->mpic.ipidr[reg_no] = data;
				int core_id = -1;
				if (data & 0x1)	/* dispatch the interrupt to core 0 */
					core_id = 0;		
				if (data & 0x2)	/* dispatch the interrupt to core 1 */
					core_id = 1;
				if(data & 0x3){	
					/* trigger an interrupt to dedicated core */
					//e500_core_t* core = &cpu->core[core_id];
					ipr |= IPI0;
					set_cpu_register(dev->cpu, "ipr", ipr);
					dev->mpic.ipivpr[1] |= 0x40000000; /* set activity bit in vpr */

					dev->pic_percpu.iack[core_id] = (dev->pic_percpu.iack[core_id] & 0xFFFF0000) | (dev->mpic.ipivpr[1] & 0xFFFF);
					//ppc_exception(core, PPC_EXC_EXT_INT, 0, 0); fix me
					/* we need to inform the core that npc is changed to exception vector */
					set_cpu_register(dev->cpu, "ipi_flag", 1);
				}
				return No_exp;
				}

			case 0x61060:
				{
				int reg_no = (offset >> 4) & 0x3;
				uint32_t ipr = get_cpu_register(dev->cpu, "ipr");
				dev->mpic.ipidr[reg_no] = data;
				int core_id = -1;
				if (data & 0x1)	/* dispatch the interrupt to core 0 */
					core_id = 0;		
				if (data & 0x2)	/* dispatch the interrupt to core 1 */
					core_id = 1;
				if(data & 0x3){	
					/* trigger an interrupt to dedicated core */
					//e500_core_t* core = &cpu->core[core_id];
					ipr |= IPI0;
					set_cpu_register(dev->cpu, "ipr", ipr);

					dev->mpic.ipivpr[2] |= 0x40000000; /* set activity bit in vpr */
					dev->pic_percpu.iack[core_id] = (dev->pic_percpu.iack[core_id] & 0xFFFF0000) | (dev->mpic.ipivpr[2] & 0xFFFF);
					//ppc_exception(core, PPC_EXC_EXT_INT, 0, 0); fix me
					/* we need to inform the core that npc is changed to exception vector */
					set_cpu_register(dev->cpu, "ipi_flag", 1);
				}
				return No_exp;
				}

			case 0x61080:
				dev->pic_ram.ctpr1 = data;
				return No_exp;
			case 0x610b0:{/* processor 1 end of interrupt register */
				dev->pic_ram.eoi[1] = data;
				uint32_t ipr = get_cpu_register(dev->cpu, "ipr");
				
				/* clear the interrupt with highest priority in ISR */
				if (ipr & (1 << dev->irq_num)) {
					ipr &= ~(1 << dev->irq_num);
				}
				return No_exp;
			}
			default:
				return Not_found_exp;

		}
	}

	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in pic_mpc8641d\n", offset);
			break;
	}
	return 0;
}
static char* pic_mpc8641d_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t pic_mpc8641d_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	pic_mpc8641d_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t pic_mpc8641d_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t pic_mpc8641d_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	pic_mpc8641d_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t pic_mpc8641d_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static void io_do_event (conf_object_t *obj, int line)
{
	const int core_id = 0; /* currently, we only send uart interrupt to cpu0 */
	pic_mpc8641d_device *dev = obj->obj;

	uint32_t msr;
	uint32_t ipr;
	uint32_t ipi_flag;
	msr = get_cpu_register(dev->cpu, "msr");
	ipr = get_cpu_register(dev->cpu, "ipr");
	ipi_flag = get_cpu_register(dev->cpu, "ipi_flag");

	if ((!(dev->mpic.iivpr[line] & 0x80000000)
				&& (msr & 0x8000) 
				&& !(ipr & 1 << line))) {
			//RW_WRLOCK(core->ext_int_lock);
            if (ipi_flag == 1)
                return;
			set_cpu_register(dev->cpu, "ipi_flag", 1);
			ipr |= (1 << line);
			set_cpu_register(dev->cpu, "ipr", ipr);
			dev->irq_num = line;

			/* set activity bit in vpr */
			dev->mpic.iivpr[line] |= 0x40000000;
			dev->pic_percpu.iack[core_id] = dev->mpic.iivpr[line] & 0x0000ffff;

			//RW_UNLOCK(core->ext_int_lock);
	}
}

static conf_object_t* new_pic_mpc8641d(char *obj_name)
{
	pic_mpc8641d_device* dev = skyeye_mm_zero(sizeof(pic_mpc8641d_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->pic_global.frr = 0x6b0102;
        dev->pic_ram.ctpr0 = 0x0000000F;
        dev->pic_global.svr = 0xFFFF;
        dev->mpic.ipivpr[0] = dev->mpic.ipivpr[1] = dev->mpic.ipivpr[2] = dev->mpic.ipivpr[3] = 0x80000000;

        
        dev->pic_global.gtbcra0 = 0x80000000;
        
        int i = 0;
        for (; i < 32; i++) { 
                dev->pic_ram.iidr[i] = 0x1;
                dev->pic_ram.iivpr[i] = 0x80800000;
        }

	return dev->obj;
}


static exception_t free_pic_mpc8641d(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static void class_register_attribute(conf_class_t* clss)
{
	return;
}

int raise_signal(conf_object_t *obj, int line)
{
	pic_mpc8641d_device *dev = obj->obj;
	io_do_event(obj, line);
	return 0;
}

int lower_signal(conf_object_t *obj, int line)
{
	pic_mpc8641d_device *dev = obj->obj;
	return 0;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	pic_mpc8641d_device *dev = obj->obj;
	dev->cpu = obj2;
	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	pic_mpc8641d_device *dev = obj->obj;
	*obj2 = dev->cpu;
	*port = NULL;
	return No_exp;
}
void init_pic_mpc8641d(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "mpc8641d_pic",
		.class_desc = "mpc8641d_pic",
		.new_instance = new_pic_mpc8641d,
		.free_instance = free_pic_mpc8641d,
		.module_type = SKYML_ONCHIP(SC_mpc8641d_mach),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = pic_mpc8641d_read,
		.write = pic_mpc8641d_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = pic_mpc8641d_get_regvalue_by_id,
		.get_regname_by_id = pic_mpc8641d_get_regname_by_id,
		.set_regvalue_by_id = pic_mpc8641d_set_regvalue_by_id,
		.get_regnum = pic_mpc8641d_get_regnum,
		.get_regid_by_name = pic_mpc8641d_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static general_signal_intf signal_intf = {
		.raise_signal = raise_signal,
		.lower_signal = lower_signal,
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_intf);

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
		(struct InterfaceDescription)
		{
			.name = GENERAL_SIGNAL_INTF_NAME,
			.iface = &signal_intf
		},
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		}
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	class_register_attribute(clss);
}

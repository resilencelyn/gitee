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
#include "rcc_stm32f0x.h"

#define BIT_VAL(source,first,length) (((source) << (32 - (first + length))) >> (first + (32 - (first + length))))
//source:需要截取的数值; first:需要截取数的起始地址; length:需要截取的长度;


static exception_t rcc_stm32f0x_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rcc_stm32f0x_device *dev = opaque->obj;
	switch (offset)
	{
		case RCC_CR:
			*(uint32_t *)buf = dev->regs.rcc_cr.all;
			break;
		case RCC_CFGR:
			*(uint32_t *)buf = dev->regs.rcc_cfgr.all;
			break;
		case RCC_CIR:
			*(uint32_t *)buf = dev->regs.rcc_cir.all;
			break;
		case RCC_APB2RSTR:
			*(uint32_t *)buf = dev->regs.rcc_apb2rstr.all;
			break;
		case RCC_APB1RSTR:
			*(uint32_t *)buf = dev->regs.rcc_apb1rstr.all;
			break;
		case RCC_AHBENR:
			*(uint32_t *)buf = dev->regs.rcc_ahbenr.all;
			break;
		case RCC_APB2ENR:
			*(uint32_t *)buf = dev->regs.rcc_apb2enr.all;
			break;
		case RCC_APB1ENR:
			*(uint32_t *)buf = dev->regs.rcc_apb1enr.all;
			break;
		case RCC_BDCR:
			*(uint32_t *)buf = dev->regs.rcc_bdcr.all;
			break;
		case RCC_CSR:
			*(uint32_t *)buf = dev->regs.rcc_csr.all;
			break;
		case RCC_AHBRSTR:
			*(uint32_t *)buf = dev->regs.rcc_ahbrstr.all;
			break;
		case RCC_CFGR2:
			*(uint32_t *)buf = dev->regs.rcc_cfgr2.all;
			break;
		case RCC_CFGR3:
			*(uint32_t *)buf = dev->regs.rcc_cfgr3.all;
			break;
		case RCC_CR2:
			*(uint32_t *)buf = dev->regs.rcc_cr2.all;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rcc_stm32f0x\n", offset);
			break;
	}
    //printf("%d %s 0x%x %x\n",__LINE__,__func__,offset,*(uint32_t*)buf);
	return 0;
}
static exception_t rcc_stm32f0x_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rcc_stm32f0x_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
    //printf("%d %s 0x%x %x\n",__LINE__,__func__,offset,val);
	switch (offset)
	{
		case RCC_CR:
			dev->regs.rcc_cr.all = val;
            if(val & dev->regs.rcc_cr.bit.hsi_on){
                dev->regs.rcc_cr.bit.hsi_on = 0;
                dev->regs.rcc_cr.bit.hsi_rdy = 1;
            }
            if(val & (dev->regs.rcc_cr.bit.hse_on << 16)){
                dev->regs.rcc_cr.bit.hse_on = 0;
                dev->regs.rcc_cr.bit.hse_rdy = 1;
            }
            if(val & (dev->regs.rcc_cr.bit.pll_on << 24)){
                dev->regs.rcc_cr.bit.pll_on = 0;
                dev->regs.rcc_cr.bit.pll_rdy = 1;
            }
			break;
		case RCC_CFGR:
			dev->regs.rcc_cfgr.all = val;
            if(val & dev->regs.rcc_cfgr.bit.sw){
                dev->regs.rcc_cfgr.bit.sws = BIT_VAL(val,0,2);
            }
			break;
		case RCC_CIR:
			dev->regs.rcc_cir.all = val;
			break;
		case RCC_APB2RSTR:
			dev->regs.rcc_apb2rstr.all = val;
			break;
		case RCC_APB1RSTR:
			dev->regs.rcc_apb1rstr.all = val;
			break;
		case RCC_AHBENR:
			dev->regs.rcc_ahbenr.all = val;
			break;
		case RCC_APB2ENR:
			dev->regs.rcc_apb2enr.all = val;
			break;
		case RCC_APB1ENR:
			dev->regs.rcc_apb1enr.all = val;
			break;
		case RCC_BDCR:
			dev->regs.rcc_bdcr.all = val;
			break;
		case RCC_CSR:
			dev->regs.rcc_csr.all = val;
			break;
		case RCC_AHBRSTR:
			dev->regs.rcc_ahbrstr.all = val;
			break;
		case RCC_CFGR2:
			dev->regs.rcc_cfgr2.all = val;
			break;
		case RCC_CFGR3:
			dev->regs.rcc_cfgr3.all = val;
			break;
		case RCC_CR2:
			dev->regs.rcc_cr2.all = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rcc_stm32f0x\n", offset);
			break;
	}
	return 0;
}
static char* rcc_stm32f0x_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t rcc_stm32f0x_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	rcc_stm32f0x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t rcc_stm32f0x_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t rcc_stm32f0x_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	rcc_stm32f0x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t rcc_stm32f0x_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_rcc_stm32f0x(char *obj_name)
{
	rcc_stm32f0x_device* dev = skyeye_mm_zero(sizeof(rcc_stm32f0x_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
    dev->hse_clock = 0;
    dev->hsi_clock = 8000000;
    dev->hsi48_clock = 48000000;
    dev->lse_clock = 32768;
    dev->lsi_clock = 40000;
    dev->adc_clock = 14000000;
    dev->rtc_clock = 0;
	return dev->obj;
}
static exception_t free_rcc_stm32f0x(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static attr_value_t get_attr_hse_frequency(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	rcc_stm32f0x_device *dev = conf_obj->obj;
	attr_value_t hse_clock = SKY_make_attr_uinteger(dev->hse_clock);
	return hse_clock;
}
static exception_t set_attr_hse_frequency(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	rcc_stm32f0x_device *dev = conf_obj->obj;
	dev->hse_clock = SKY_attr_uinteger(*value);
	return No_exp;
}

static timer_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "HSE_frequency", get_attr_hse_frequency, NULL, set_attr_hse_frequency, NULL, SKY_Attr_Optional, "uinteger", "frequency of high speed external clock");
}
static uint32_t clock_frequency(conf_object_t *obj,uint32_t clock)//获取时钟频率 clock:时钟源
{
	rcc_stm32f0x_device *dev = obj->obj;
    if(clock == HSE_CLOCK){
        return dev->hse_clock;
    }
    else if(clock == HSI_CLOCK){
        return dev->hsi_clock;
    }
    else if(clock == HSI48_CLOCK){
        return dev->hsi48_clock;
    }
    else if(clock == LSE_CLOCK){
        return dev->lse_clock;
    }
    else if(clock == LSI_CLOCK){
        return dev->lsi_clock;
    }
    else if(clock == SYSCLOCK){
        if(dev->regs.rcc_cfgr.bit.sws == 0){
            return dev->hsi_clock;
        }
        else if(dev->regs.rcc_cfgr.bit.sws== 1){
            return dev->hse_clock;
        }
        else if(dev->regs.rcc_cfgr.bit.sws == 2){
            int pllmul=dev->regs.rcc_cfgr.bit.pllmul;
            int sys_clock = 0;
            int prediv = dev->regs.rcc_cfgr2.bit.prediv;
            pllmul = pllmul + 2;
            prediv = prediv + 1;
            if(pllmul >= 16){
                pllmul = 16;
            }
            if(dev->regs.rcc_cfgr.bit.pll_src == 0){
                sys_clock = dev->hsi_clock / 2 * pllmul;
            } 
            else if(dev->regs.rcc_cfgr.bit.pll_src == 1){
                sys_clock = dev->hsi_clock / prediv * pllmul;
            }
            else if(dev->regs.rcc_cfgr.bit.pll_src == 2){
                sys_clock = dev->hse_clock / prediv * pllmul;
            }
            else if(dev->regs.rcc_cfgr.bit.pll_src == 3){
                sys_clock = dev->hsi48_clock / prediv * pllmul;
            }else{
                sys_clock = 0;
            }
            if(sys_clock >= 48000000){
                sys_clock = 48000000;
            }
            dev->sysclock = sys_clock;
            return sys_clock;
        }
        else if(dev->regs.rcc_cfgr.bit.sws == 3){
            return dev->hsi48_clock;
        }else{
            return 0;
        }
    }
    else if(clock == ADC_CLOCK){
        return dev->adc_clock;
    }
    else if(clock == RTC_CLOCK){
        return dev->rtc_clock;
    }
    else{
        return 0;
    }
}
void init_rcc_stm32f0x(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "rcc_stm32f0x",
		.class_desc = "rcc_stm32f0x",
		.new_instance = new_rcc_stm32f0x,
		.free_instance = free_rcc_stm32f0x,
		.module_type = SKYML_ONCHIP(SC_stm32),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = rcc_stm32f0x_read,
		.write = rcc_stm32f0x_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = rcc_stm32f0x_get_regvalue_by_id,
		.get_regname_by_id = rcc_stm32f0x_get_regname_by_id,
		.set_regvalue_by_id = rcc_stm32f0x_set_regvalue_by_id,
		.get_regnum = rcc_stm32f0x_get_regnum,
		.get_regid_by_name = rcc_stm32f0x_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
    static get_clock_frequency_t get_clock_frequency = {
        .read = clock_frequency,
    };
    SKY_register_iface(clss,CLOCK_FREQUENCY_INTERFACE,&get_clock_frequency);
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
            .name = CLOCK_FREQUENCY_INTERFACE,
            .iface = &get_clock_frequency
        }
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	timer_register_attribute(clss);
}

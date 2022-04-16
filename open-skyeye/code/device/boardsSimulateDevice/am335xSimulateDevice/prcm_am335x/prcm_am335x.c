/* Copyright (C) 
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
* 
*/
/**
* @file prcm_am335x.c
* @brief The implementation of system controller
* @author 
* @version 78.77
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>

#include "prcm_am335x.h"

static exception_t prcm_am335x_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct prcm_am335x_device *dev = opaque->obj;
	prcm_am335x_reg_t* regs = dev->regs;
	switch(offset) {
		case CM_PER_L4LS_CLKSTCTRL:
			*(uint32_t*)buf = regs->per.l4ls_clkstctrl;
			break;
		case CM_PER_L3S_CLKSTCTRL:
			*(uint32_t*)buf = regs->per.l3s_clkstctrl;
			break;
		case CM_PER_L3_CLKSTCTRL:
			*(uint32_t*)buf = regs->per.l3_clkstctrl;
			break;
		case CM_PER_L4LS_CLKCTRL:
			*(uint32_t*)buf = regs->per.l4ls_clkctrl;
			break;
		case CM_PER_L4HS_CLKCTRL :
			*(uint32_t*)buf = regs->per.l4ls_clkctrl;
			break;
		case CM_PER_CLKDIV32K_CLKCTRL:
			*(uint32_t*)buf = regs->per.clkdiv32k_clkctrl;
			break;
		case CM_PER_L3_INSTR_CLKCTRL:
			*(uint32_t*)buf = regs->per.l3_instr_clkctrl;
			break;
		case CM_PER_L3_CLKCTRL:
			*(uint32_t*)buf = regs->per.l3_clkctrl;
			break;
		case CM_PER_OCPWP_L3_CLKSTCTRL:
			*(uint32_t*)buf = regs->per.ocpwp_l3_clkstctrl;
			break;
		case CM_PER_L4FW_CLKCTRL:
			*(uint32_t*)buf = regs->per.l4fw_clkctrl;
			break;
		case CM_PER_TIMER2_CLKCTRL:
			*(uint32_t*)buf = regs->per.timer2_clkctrl;
			break;
		case CM_PER_UART1_CLKCTRL:
			*(uint32_t*)buf = regs->per.uart1_clkctrl;
			break;
		case CM_DPLL_CLKSEL_TIMER2_CLK:
			*(uint32_t*)buf = regs->dpll.clksel_timer2_clk;
			break;
		case CM_WKUP_CLKSTCTRL:
			*(uint32_t*)buf = regs->wkup.cm_wkup_clkstctrl;
			break;
		case CM_WKUP_CONTROL_CLKCTRL:
			*(uint32_t*)buf = regs->wkup.cm_wkup_control_clkctrl;
			break;
		case CM_WKUP_GPIO0_CLKCTRL:
			*(uint32_t*)buf = regs->wkup.cm_wkup_gpio0_clkctrl;
			break;
		case CM_IDLEST_DPLL_MPU:
			*(uint32_t*)buf = regs->wkup.cm_idlest_dpll_mpu;
			break;
		case CM_IDLEST_DPLL_DDR:
			*(uint32_t*)buf = regs->wkup.cm_idlest_dpll_addr;
			break;
		case CM_IDLEST_DPLL_CORE:
			*(uint32_t*)buf = regs->wkup.cm_idlest_dpll_core;
			break;
		case CM_IDLEST_DPLL_PER:
			*(uint32_t*)buf = regs->wkup.cm_idlest_dpll_per;
			break;
		case CM_CLKMODE_DPLL_MPU:
			*(uint32_t*)buf = regs->wkup.cm_clkmode_dpll_mpu;
			break;
		case CM_L3_AON_CLKSTCTRL:
			*(uint32_t*)buf = regs->wkup.cm_l3_aon_clkstctrl;
			break;
		case CM_WKUP_L4WKUP_CLKCTRL:
			*(uint32_t*)buf = regs->wkup.cm_wkup_l4wkup_clkctrl ;
			break;
		case CM_WKUP_I2C0_CLKCTRL:
			*(uint32_t*)buf = regs->wkup.i2c0_clkctrl;
			break;
		case  CM_L4_WKUP_AON_CLKSTCTRL:
			*(uint32_t*)buf = regs->wkup.cm_l4_wkup_aon_clkstctrl ;
			break;
		case CM_RTC_RTC_CLKCTRL:
			*(uint32_t*)buf = regs->rtc.cm_rtc_rtc_clkctrl;
			break;
		case CM_RTC_CLKSTCTRL:
			*(uint32_t*)buf = regs->rtc.cm_rtc_clkstctrl;
			break;
		case CM_PER_SPI0_CLKCTRL:
			*(uint32_t*)buf = regs->per.spi0_clkctrl;
			break;
		case CM_PER_SPI1_CLKCTRL:
			*(uint32_t*)buf = regs->per.spi1_clkctrl;
			break;
		case CM_PER_I2C2_CLKCTRL:
			*(uint32_t*)buf = regs->per.i2c2_clkctrl;
			break;
		case CM_PER_I2C1_CLKCTRL:
			*(uint32_t*)buf = regs->per.i2c1_clkctrl;
			break;
		default:
			printf("Can not read the register at 0x%x in prcm_am335x\n", offset);
			return Invarg_exp;
	}
	//printf("In %s offset 0x%x buf 0x%x\n", __func__, offset, *(uint32_t*)buf);
	return No_exp;

}

static exception_t prcm_am335x_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct prcm_am335x_device *dev = opaque->obj;
	prcm_am335x_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	//printf("In %s offset 0x%x buf 0x%x\n", __func__, offset, *buf);
	switch(offset) {
		case CM_PER_L4LS_CLKSTCTRL:
			regs->per.l4ls_clkstctrl &= ~0x3;
		       	regs->per.l4ls_clkstctrl |= ((*buf) & 0x3);
			break;
		case CM_PER_L3S_CLKSTCTRL:
			regs->per.l3s_clkstctrl &= ~0x3;
			regs->per.l3s_clkstctrl |= ((*buf) & 0x3);
			break;
		case CM_PER_L3_CLKSTCTRL:
			regs->per.l3_clkstctrl &= ~0x3;
			regs->per.l3_clkstctrl |= ((*buf) & 0x3);
			break;
		case CM_PER_L4LS_CLKCTRL:
			regs->per.l4ls_clkctrl = *buf;
			break;
		case CM_PER_L3_INSTR_CLKCTRL:
			regs->per.l3_instr_clkctrl = *buf;
			break;
		case CM_PER_L3_CLKCTRL:
			regs->per.l3_clkctrl = *buf;
			break;
		case CM_PER_OCPWP_L3_CLKSTCTRL:
			regs->per.ocpwp_l3_clkstctrl &= ~0x3;
			regs->per.ocpwp_l3_clkstctrl |= ((*buf) & 0x3);
			break;
		case CM_PER_L4FW_CLKCTRL:
			regs->per.l4fw_clkctrl = *buf;
			break;
		case CM_PER_L4HS_CLKCTRL :
			regs->per.l4ls_clkctrl = *buf;
			break;
		case CM_PER_CLKDIV32K_CLKCTRL:
			regs->per.clkdiv32k_clkctrl = *buf;
			break;
		case CM_PER_TIMER2_CLKCTRL:
			regs->per.timer2_clkctrl = *buf;
			break;
		case CM_PER_UART1_CLKCTRL:
			regs->per.uart1_clkctrl = *buf;
			break;
		case CM_DPLL_CLKSEL_TIMER2_CLK:
			regs->dpll.clksel_timer2_clk = *buf;
			break;
		case CM_WKUP_CLKSTCTRL:
			regs->wkup.cm_wkup_clkstctrl = *buf;
			break;
		case CM_WKUP_L4WKUP_CLKCTRL:
			regs->wkup.cm_wkup_l4wkup_clkctrl = *buf;
			break;
		case CM_WKUP_CONTROL_CLKCTRL:
			regs->wkup.cm_wkup_control_clkctrl = *buf;
			break;
		case CM_WKUP_I2C0_CLKCTRL:
			regs->wkup.i2c0_clkctrl = *buf;
			break;
		case CM_CLKMODE_DPLL_MPU:
			regs->wkup.cm_clkmode_dpll_mpu = *buf;
			break;
		case CM_L3_AON_CLKSTCTRL:
			regs->wkup.cm_l3_aon_clkstctrl = *buf;
			break;
		case CM_RTC_RTC_CLKCTRL:
			regs->rtc.cm_rtc_rtc_clkctrl= *buf;
			break;
		case CM_RTC_CLKSTCTRL:
			regs->rtc.cm_rtc_clkstctrl = *buf;
			break;
		case CM_PER_SPI0_CLKCTRL:
			regs->per.spi0_clkctrl = *buf;
			break;
		case CM_PER_SPI1_CLKCTRL:
			regs->per.spi1_clkctrl = *buf;
			break;
		case CM_PER_I2C2_CLKCTRL:
			regs->per.i2c2_clkctrl = *buf;
			break;
		case CM_PER_I2C1_CLKCTRL:
			regs->per.i2c1_clkctrl = *buf;
			break;

		default:
			printf("Can not write the register at 0x%x in prcm_am335x\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}
static conf_object_t* new_prcm_am335x(char* obj_name){
	prcm_am335x_device* dev = skyeye_mm_zero(sizeof(prcm_am335x_device));
	prcm_am335x_reg_t* regs =  skyeye_mm_zero(sizeof(prcm_am335x_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init prcm_am335x regs */
	dev->regs = regs;
#if 0
	/* Register io function to the object */
	memory_space_intf* io_memory = skyeye_mm_zero(sizeof(memory_space_intf));
	io_memory->conf_obj = dev->obj;
	io_memory->read = prcm_am335x_read;
	io_memory->write = prcm_am335x_write;
	SKY_register_interface(io_memory, obj_name, MEMORY_SPACE_INTF_NAME);	
#endif
	return dev->obj;
}

exception_t reset_prcm_am335x(conf_object_t* opaque, const char* parameter){
	struct prcm_am335x_device *dev = opaque->obj;
	prcm_am335x_reg_t* regs = dev->regs;
	regs->per.l4ls_clkstctrl = 0xc0102;
	regs->per.l3s_clkstctrl = 0xa;
	regs->per.l3_clkstctrl = 0x12;
	regs->per.ocpwp_l3_clkstctrl = 0x12;
	regs->wkup.cm_wkup_clkstctrl |= 0x804;
	regs->wkup.cm_l3_aon_clkstctrl |= 0x00000008;
	regs->wkup.cm_l4_wkup_aon_clkstctrl |= 0x4;
	regs->rtc.cm_rtc_clkstctrl |= 0x300; 
	regs->per.l3_clkctrl = 0;
	regs->per.l3_instr_clkctrl = 0;
	regs->per.l3_clkstctrl |= 0x10;
	regs->per.ocpwp_l3_clkstctrl |= 0x10;
	regs->wkup.cm_idlest_dpll_mpu |= 0x101; 
	regs->wkup.cm_idlest_dpll_core |=0x101;
	regs->wkup.cm_idlest_dpll_addr |=0x101;
	regs->wkup.cm_idlest_dpll_per |=0x101;
	return No_exp;
}

void free_prcm_am335x(conf_object_t* opaque){
}

static void prcm_am335x_register_interface(conf_class_t* clss){
	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = prcm_am335x_read,
		.write = prcm_am335x_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		}
 	};
	class_register_ifaces(clss, ifaces);
	return;
}
void init_prcm_am335x(){
	static skyeye_class_t class_data = {
		.class_name = "am335x_prcm",
		.class_desc = "am335x_prcm",
		.new_instance = new_prcm_am335x,
		.free_instance = free_prcm_am335x,
		.reset_instance = reset_prcm_am335x,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_am335x SC_am3359),
	};

	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	prcm_am335x_register_interface(class);
}

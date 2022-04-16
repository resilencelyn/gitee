/*
 * skyeye_mach_am335x.c - define machine am335 for skyeye
 *
 * Copyright (C) 2013 Kewei Yu <keweihk@gmail.com>
 * Skyeye Develop Group, for help please send mail to
 * <skyeye-developer@lists.gro.clinux.org>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <stdlib.h>

#include <skyeye.h>
#include <skyeye_config.h>
#include <skyeye_mach.h>
#include <skyeye_arch.h>
#include <skyeye_types.h>
#include <skyeye_obj.h>
#include <skyeye_internal.h>
#include <skyeye_addr_space.h>
#include <skyeye_iface.h>
#include "am335x.h"
exception_t set_conf_attr(conf_object_t* obj, char* attr_name, attr_value_t* value);

static void am335x_update_intr(void* mach) {
}


int am335x_set_space(conf_object_t *obj, addr_space_t *space)
{
	am335x_mach_t *mach = obj->obj;
	mach->attr.space = space;
	return 0;
}

addr_space_t *am335x_get_space(conf_object_t *obj)
{
	am335x_mach_t *mach = obj->obj;
	return  mach->attr.space;
}

int am335x_set_core(conf_object_t *obj, conf_object_t *core)
{
	am335x_mach_t *mach = obj->obj;
	mach->attr.core[mach->attr.core_num++] = core;
	if(mach->attr.core_num >= MAX_CORE_NUM){
		skyeye_log(Error_log, __FUNCTION__, "This MACHINE supports up to 8-cores\n");
	}
	return 0;
}

conf_object_t *am335x_get_core_by_id(conf_object_t *obj, int id){
	am335x_mach_t *mach = obj->obj;
	return mach->attr.core[id];
}

/**
* @brief Initialization of mach
*
* @param obj_name
*
* @return 
*/
static conf_object_t* new_am335x_mach(char* obj_name){
	am335x_mach_t* mach = skyeye_mm_zero(sizeof(am335x_mach_t));
	mach->obj = new_conf_object(obj_name, mach);
	mach->attr.core_num = 0;
	return mach->obj;
}

static void free_am335x_mach(conf_object_t *mach)
{
	am335x_mach_t *am335_mach = mach->obj;
	addr_space_t *phys_mem = am335_mach->attr.space;
	space_obj_free(phys_mem);
	return;
}

static exception_t reset_am335x_mach(conf_object_t* mach)
{
	am335x_mach_t *am335_mach = mach->obj;
	addr_space_t *phys_mem = am335_mach->attr.space;
	space_obj_reset(phys_mem, NULL);
	exception_t ret = reset_conf_obj(am335_mach->attr.core);
	return No_exp;
}
static void am335x_register_attribute(conf_class_t* clss){
}

static void am335x_register_interface(conf_class_t* clss){
	static const skyeye_machine_intf inface = {
		.set_space = am335x_set_space,
		.set_core = am335x_set_core,
		.get_space = am335x_get_space,
		.get_core_by_id = am335x_get_core_by_id
	};
	SKY_register_iface(clss, MACHINE_INTF_NAME, &inface);
	return;
}
void init_am335x_mach(){
	static skyeye_class_t class_data = {
		.class_name = "am335x",
		.class_desc = "am335x machine",
		.new_instance = new_am335x_mach,
		.reset_instance = reset_am335x_mach,
		.free_instance = free_am335x_mach,
		.get_attr = NULL,
		.set_attr = NULL
	};

	conf_class_t* class = SKY_register_soc_class(class_data.class_name, &class_data);
	am335x_register_attribute(class);
	am335x_register_interface(class);
}

static conf_object_t* new_dyncom_am335x_mach(char* obj_name){
	am335x_mach_t* mach = skyeye_mm_zero(sizeof(am335x_mach_t));
	mach->obj = new_conf_object(obj_name, mach);

	//conf_object_t* arm_cpu = pre_conf_obj("arm_cpu0", "arm_cpu");

	conf_object_t* arm_cpu = pre_conf_obj("arm_cpu0", "dyncom_arm_cpu");
	exception_t ret = reset_conf_obj(arm_cpu);
	mach->space = (addr_space_t*)SKY_get_interface(arm_cpu, ADDR_SPACE_INTF_NAME);
	printf("In %s, mach->space=0x%x\n", __FUNCTION__, mach->space);
	attr_value_t* value = NULL;


	conf_object_t* intc0 = pre_conf_obj("intc_am335x_0", "intc_am335x");
	memory_space_intf* intc0_io_memory = (memory_space_intf*)SKY_get_interface(intc0, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x48200000, 0xfff, 0x0, intc0_io_memory, 1, 1);

	conf_object_t* timer2 = pre_conf_obj("timer2_am335x", "timer_am335x");
	memory_space_intf* timer2_io_memory = (memory_space_intf*)SKY_get_interface(timer2, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x48040000, 0x58, 0x0, timer2_io_memory, 1, 1);
	ret = set_conf_attr(timer2, "signal", make_new_attr(Val_Object, intc0));


	conf_object_t* timer0_1ms = pre_conf_obj("timer0_1ms_am335x", "timer_1ms_am335x");
	memory_space_intf* timer0_1ms_io_memory = (memory_space_intf*)SKY_get_interface(timer0_1ms, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x44E31000, 0xfff, 0x0, timer0_1ms_io_memory, 1, 1);
	ret = set_conf_attr(timer0_1ms, "signal", make_new_attr(Val_Object, intc0));
	ret = set_conf_attr(timer0_1ms, "int_number", make_new_attr(Val_UInteger, 67));   //set interrupt number 67


	conf_object_t* prcm0 = pre_conf_obj("prcm_am335x_0", "prcm_am335x");
	memory_space_intf* prcm0_io_memory = (memory_space_intf*)SKY_get_interface(prcm0, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x44e00000, 0xaff, 0x0, prcm0_io_memory, 1, 1);
	ret = reset_conf_obj(prcm0);

	/*instance gpio1*/
	conf_object_t* gpio1 = pre_conf_obj("gpio1", "am335x_gpio");
	memory_space_intf* gpio1_io_memory = (memory_space_intf*)SKY_get_interface(gpio1, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x4804C000, 0xFFF, 0x0, gpio1_io_memory, 1, 1);
        /****************/

	/*instance gpio2*/
	conf_object_t* gpio2 = pre_conf_obj("gpio2", "am335x_gpio");
	memory_space_intf* gpio2_io_memory = (memory_space_intf*)SKY_get_interface(gpio2, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x481AC000, 0xFFF, 0x0, gpio2_io_memory, 1, 1);
	reset_conf_obj(gpio2);
	ret = set_conf_attr(gpio2, "signal", make_new_attr(Val_Object, intc0));
	ret = set_conf_attr(gpio2, "int_number", make_new_attr(Val_UInteger, 32));   //set interrupt number 7
        /****************/

	/*instance xtht_intc*/
	conf_object_t* xdht_intc0 = pre_conf_obj("xdht_intc0", "xdht_intc");
	memory_space_intf* xdhtintc0_io_memory = (memory_space_intf*)SKY_get_interface(xdht_intc0, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x1006200, 0x200, 0x0, xdhtintc0_io_memory, 1, 1);
	reset_conf_obj(xdht_intc0);
	ret = set_conf_attr(xdht_intc0, "signal", make_new_attr(Val_Object, gpio2));
	ret = set_conf_attr(xdht_intc0, "int_number", make_new_attr(Val_UInteger, 7));   //set interrupt number 7
	/*******************/

	/*instance uart_MAIN*/
	conf_object_t* uart0 = pre_conf_obj("UART_MAIN", "xdht_uart");
	memory_space_intf* uart0_io_memory = (memory_space_intf*)SKY_get_interface(uart0, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x1006a00, 0xF, 0x0, uart0_io_memory, 1, 1);
	reset_conf_obj(uart0);
	ret = set_conf_attr(uart0, "signal", make_new_attr(Val_Object, xdht_intc0));
	ret = set_conf_attr(uart0, "int_number", make_new_attr(Val_UInteger, 384));   //set interrupt number 384
	/**********************/

	conf_object_t* uartpc0 = pre_conf_obj("uartpc0", "uart_pc");
	value = make_new_attr(Val_Object, uartpc0);
	ret = set_conf_attr(uart0, "pc", value);
	value = make_new_attr(Val_Object, uart0);
	ret = set_conf_attr(uartpc0, "host", value);

	/*instance uart_VIEW*/
	conf_object_t* uart1 = pre_conf_obj("UART_VIEW", "xdht_uart");
	memory_space_intf* uart1_io_memory = (memory_space_intf*)SKY_get_interface(uart1, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x1006a20, 0xF, 0x0, uart1_io_memory, 1, 1);
	reset_conf_obj(uart1);
	ret = set_conf_attr(uart1, "signal", make_new_attr(Val_Object, xdht_intc0));
	ret = set_conf_attr(uart1, "int_number", make_new_attr(Val_UInteger, 385));   //set interrupt number 385
	/**********************/

	/*instance term0*/
	conf_object_t* term0 = pre_conf_obj("term0", "uart_term");
	value = make_new_attr(Val_Object, term0);
	ret = set_conf_attr(uart1, "term", value);
	/**********************/

	/*instance uart_GEN3*/
	conf_object_t* uart2 = pre_conf_obj("UART_GEN3", "xdht_uart");
	memory_space_intf* uart2_io_memory = (memory_space_intf*)SKY_get_interface(uart2, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x1006a40, 0xF, 0x0, uart2_io_memory, 1, 1);
	reset_conf_obj(uart2);
	ret = set_conf_attr(uart2, "signal", make_new_attr(Val_Object, xdht_intc0));
	/**********************/

	/*instance uart_GEN4*/
	conf_object_t* uart3 = pre_conf_obj("UART_GEN4", "xdht_uart");
	memory_space_intf* uart3_io_memory = (memory_space_intf*)SKY_get_interface(uart3, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x1006a60, 0xF, 0x0, uart3_io_memory, 1, 1);
	reset_conf_obj(uart3);
	ret = set_conf_attr(uart3, "signal", make_new_attr(Val_Object, xdht_intc0));
	/**********************/


	/*instance onchip uart0*/                                                                                                     
	conf_object_t* onchip_uart0 = pre_conf_obj("onchip_uart0", "am335x_uart");
	memory_space_intf* oncuart0_io_memory = (memory_space_intf*)SKY_get_interface(onchip_uart0, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x44E09000, 0xFFF, 0x0, oncuart0_io_memory, 1, 1);
	reset_conf_obj(onchip_uart0);
	/**********************/

	/*instance term1*/
	conf_object_t* term1 = pre_conf_obj("AM335X_POLL_UART", "uart_term");
	value = make_new_attr(Val_Object, term1);
	ret = set_conf_attr(onchip_uart0, "term", value);
	/**********************/

	/*instance uart file0*/
	conf_object_t* uart_file0 = pre_conf_obj("uart_file0", "uart_file");
	value = make_new_attr(Val_Object, uart_file0);
	ret = set_conf_attr(onchip_uart0, "file", value);
	/**********************/


	/*instance can0*/
	conf_object_t* xdht_can0 = pre_conf_obj("can0", "xdht_can");
	memory_space_intf* xdht_can0_io_memory = (memory_space_intf*)SKY_get_interface(xdht_can0, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x1006800, 0x40, 0x0, xdht_can0_io_memory, 1, 1);
	value = make_new_attr(Val_Object, xdht_can0);
	ret = set_conf_attr(xdht_can0, "signal", make_new_attr(Val_Object, xdht_intc0));
	ret = set_conf_attr(xdht_can0, "int_number", make_new_attr(Val_UInteger, 400));   //set interrupt number 400
	/**********************/

	/*instance am335x rtc*/
	conf_object_t* am335x_rtc0 = pre_conf_obj("rtc0", "am335x_rtc");
	memory_space_intf* am335x_rtc0_io_memory = (memory_space_intf*)SKY_get_interface(am335x_rtc0, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x44E3E000, 0xFFF, 0x0, am335x_rtc0_io_memory, 1, 1); 
	value = make_new_attr(Val_Object, am335x_rtc0);
	ret = set_conf_attr(am335x_rtc0, "signal", make_new_attr(Val_Object, intc0));
	ret = set_conf_attr(am335x_rtc0, "int_number", make_new_attr(Val_UInteger, 75));
	/**********************/


	/*instance onchip uart1*/                                                                                                     
	conf_object_t* onchip_uart1 = pre_conf_obj("onchip_uart1", "am335x_uart");
	memory_space_intf* oncuart1_io_memory = (memory_space_intf*)SKY_get_interface(onchip_uart1, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x48022000, 0xFFF, 0x0, oncuart1_io_memory, 1, 1);
	reset_conf_obj(onchip_uart1);
	ret = set_conf_attr(onchip_uart1, "signal", make_new_attr(Val_Object, intc0));
	ret = set_conf_attr(onchip_uart1, "int_number", make_new_attr(Val_UInteger, 73));
	/**********************/
	/**********************/

	///*instance term2*/
	conf_object_t* term2 = pre_conf_obj("AM335x_Interrupt_Uart", "uart_term");
	value = make_new_attr(Val_Object, term2);
	ret = set_conf_attr(onchip_uart1, "term", value);
	/**********************/

	
	/*instance iic bus*/                                                                         
	conf_object_t* iic_bus0 = pre_conf_obj("iic_bus0", "i2c_bus");
	/**********************/

	/*instance onchip iic0*/                                                                 
	conf_object_t* onchip_iic0 = pre_conf_obj("onchip_iic0", "am335x_iic");
	memory_space_intf* onchip_iic0_io_memory = (memory_space_intf*)SKY_get_interface(onchip_iic0, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x44E0B000, 0xFFF, 0x0, onchip_iic0_io_memory, 1, 1);
	ret = set_conf_attr(onchip_iic0, "signal", make_new_attr(Val_Object, intc0));
	ret = set_conf_attr(onchip_iic0, "int_number", make_new_attr(Val_UInteger, 70));
	ret = set_conf_attr(onchip_iic0, "i2c_master", make_new_attr(Val_Object, iic_bus0));
	/**********************/

	/*instance eeprom cat24c256*/                                                                 
	conf_object_t* cat24c256_0 = pre_conf_obj("cat24c256_0", "eeprom_cat24c256");
	ret = set_conf_attr(iic_bus0, "i2c_slave", make_new_attr(Val_Object, cat24c256_0));
	/**********************/

	/*instance FPGA iic bus*/                                                                  
	conf_object_t* iic_bus1 = pre_conf_obj("iic_bus1", "i2c_bus");
	/**********************/

	/*instance FPGA IIC*/                                                                        
	conf_object_t* fpga_iic = pre_conf_obj("xdht_iic0", "xdht_iic");
	memory_space_intf* fpga_io_memory = (memory_space_intf*)SKY_get_interface(fpga_iic, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x1006900, 0xF, 0x0, fpga_io_memory, 1, 1);
	ret = set_conf_attr(fpga_iic, "i2c_master", make_new_attr(Val_Object, iic_bus1));
	/**********************/

	/*instance cat24c64*/                                                                                  
	conf_object_t* cat24c64_0 = pre_conf_obj("cat24c64_0", "eeprom_cat24c64");
	ret = set_conf_attr(iic_bus1, "i2c_slave", make_new_attr(Val_Object, cat24c64_0));
	/**********************/

	/*instance ds3231*/                                                                                    
	conf_object_t* ds3231_0 = pre_conf_obj("ds3231_0", "ds3231");
	ret = set_conf_attr(iic_bus1, "i2c_slave", make_new_attr(Val_Object, ds3231_0));
	/**********************/

	/*instance onchip spi bus*/
	conf_object_t* spi_bus_0 = pre_conf_obj("spi_bus_0", "spi_bus");
	/**********************/

	/*instance max1253_0*/
	conf_object_t* max1253_0 = pre_conf_obj("max1253_0", "max1253");
	ret = set_conf_attr(spi_bus_0, "spi_slave", make_new_attr(Val_Object, max1253_0));
	/**********************/

	/*instance onchip spi0*/                                                                   
	#if 0
	conf_object_t* onchip_spi0 = pre_conf_obj("onchip_spi0", "am335x_spi");
	memory_space_intf* onchip_spi0_io_memory = (memory_space_intf*)SKY_get_interface(onchip_spi0, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x48030000, 0xFFF, 0x0, onchip_spi0_io_memory, 1, 1);
	reset_conf_obj(onchip_spi0);
	ret = set_conf_attr(onchip_spi0, "signal", make_new_attr(Val_Object, intc0));
	ret = set_conf_attr(onchip_spi0, "int_number", make_new_attr(Val_UInteger, 65));
	ret = set_conf_attr(onchip_spi0, "i2c_master", make_new_attr(Val_Object, spi_bus_0));
	#endif
	/**********************/

	/*instance FPGA spi bus*/
	conf_object_t* spi_bus1 = pre_conf_obj("spi_bus1", "spi_bus");
	/**********************/

	/*instance max1253_1*/
	conf_object_t* max1253_1 = pre_conf_obj("max1253_1", "max1253");
	ret = set_conf_attr(spi_bus1, "spi_slave", make_new_attr(Val_Object, max1253_1));
	/**********************/

	/*instance FPGA spi */                                                                   
	conf_object_t* fpga_spi = pre_conf_obj("fpga_spi", "xdht_spi");
	memory_space_intf* fpga_spi_io_memory = (memory_space_intf*)SKY_get_interface(fpga_spi, MEMORY_SPACE_INTF_NAME);
	ret = add_map(mach->space, 0x1006920, 0xFFF, 0x0, fpga_spi_io_memory, 1, 1);
	ret = set_conf_attr(fpga_spi, "signal", make_new_attr(Val_Object, xdht_intc0));
	ret = set_conf_attr(fpga_spi, "int_number", make_new_attr(Val_UInteger, 416));
	ret = set_conf_attr(fpga_spi, "i2c_master", make_new_attr(Val_Object, spi_bus1));
	///**********************/
	return mach->obj;
}
void init_dyncom_am335x_mach(){
	static skyeye_class_t class_data = {
		.class_name = "dyncom_am335x",
		.class_desc = "dyncom am335x machine",
		.new_instance = new_dyncom_am335x_mach,
		.free_instance = free_am335x_mach,
		.get_attr = NULL,
		.set_attr = NULL
	};

	SKY_register_soc_class(class_data.class_name, &class_data);
}


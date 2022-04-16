/* Copyright (C) 
* 2011 - Michael.Kang blackfin.kang@gmail.com
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
* @file uart_imx6.c
* @brief imx6 uart module
* @author Michael.Kang blackfin.kang@gmail.com
* @version 0.1
* @date 2011-07-13
*/
/* Description: update uart improvement from mpc8641d
 * author eJim Lee
 * date 2014-02-27
 */

#include <skyeye_config.h>
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
#include <skyeye_attr.h>
#include <simics/base_types.h>
#include <skyeye_dev_intf.h>
#include <skyeye_system.h>
#include "skyeye_uart.h"

#include "uart_imx6.h"

#if 0
#define UART_DBG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define UART_DBG(fmt, ...)
#endif

#define offset_err(info, offset, ...) \
	fprintf(stderr, "%s: Error offset 0x%x (" info ")\n", __func__, offset, ##__VA_ARGS__)

#define SET_BIT(reg, mask) ((reg) |= (mask))
#define CLR_BIT(reg, mask) ((reg) &= ~(mask))
#define CHK_BIT(reg, mask) ((reg) & (mask))

static void update_signal(uart_imx6_t *uart)
{
	reg_imx6_t *reg = &uart->reg;
	return;
}

static exception_t uart_imx6_read(conf_object_t *obj,generic_address_t offset, void* buf, size_t count)
{
    uart_imx6_t *dev = (uart_imx6_t *)obj->obj;
	switch (offset)
	{
		case URXD:
        {
            int ret = CheckCanReadNum(dev->read_fifo);
            if (ret <= 1) {
			    dev->reg.uts.all |= 1 << 5; //get the rx data, rx empty again
            }
			if (ret == 0) {
                dev->reg.usr2.bit.RDR = 0x0;
                dev->reg.urxd.bit.CHARRDY = 0x0;
            } else if (ret >= 1){
			    char pbuf;
			    ReadFIFO(dev->read_fifo, &pbuf, 1);
                dev->reg.urxd.bit.RX_DATA = pbuf;
                dev->reg.urxd.bit.CHARRDY = 0x1;
            }
			*(uint32_t *)buf = dev->reg.urxd.all;
			break;
        }
		case UTXD:
			*(uint32_t *)buf = dev->reg.utxd;
			break;
		case UCR1:
			*(uint32_t *)buf = dev->reg.ucr1.all;
			break;
		case UCR2:
			*(uint32_t *)buf = dev->reg.ucr2.all;
			break;
		case UCR3:
			*(uint32_t *)buf = dev->reg.ucr3.all;
			break;
		case UCR4:
			*(uint32_t *)buf = dev->reg.ucr4.all;
			break;
		case UFCR:
			*(uint32_t *)buf = dev->reg.ufcr.all;
			break;
		case USR1:
			*(uint32_t *)buf = dev->reg.usr1.all;
			break;
		case USR2:
			*(uint32_t *)buf = dev->reg.usr2.all;
			break;
		case UESC:
			*(uint32_t *)buf = dev->reg.uesc;
			break;
		case UTIM:
			*(uint32_t *)buf = dev->reg.utim;
			break;
		case UBIR:
			*(uint32_t *)buf = dev->reg.ubir;
			break;
		case UBMR:
			*(uint32_t *)buf = dev->reg.ubmr;
			break;
		case UBRC:
			*(uint32_t *)buf = dev->reg.ubrc;
			break;
		case ONEMS:
			*(uint32_t *)buf = dev->reg.onems;
			break;
		case UTS:
			*(uint32_t *)buf = dev->reg.uts.all;
			break;
		case UMCR:
			*(uint32_t *)buf = dev->reg.umcr.all;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in uart_imx6\n", offset);
			break;
	}
    //printf("%s %d %s %x %x\n", __func__, __LINE__, obj->objname, offset,*(uint32_t*)buf);
	return No_exp;
}

static exception_t uart_imx6_write(conf_object_t *obj, generic_address_t offset, const void *buf, size_t count)
{
    uart_imx6_t *dev = (uart_imx6_t *)obj->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf("%s, %d, %s offset:%x, value = %x\n", __func__, __LINE__, obj->objname, offset, *(uint32_t *)buf);
	switch (offset)
	{
		case URXD:
			dev->reg.urxd.all = val;
			break;
		case UTXD:
			dev->reg.utxd = val;
            if(dev->term.intf != NULL){
                dev->term.intf->write(dev->term.obj, buf, 1);
            }
			if (dev->uart_file != NULL){
				dev->uart_file_iface->write(dev->uart_file, buf, 1);
			}
			break;
		case UCR1:
			dev->reg.ucr1.all = val;
			break;
		case UCR2:
			dev->reg.ucr2.all = val;
			//write 0 to reset uart, remains active for 4 module_clock cycles
			if(!(val&0x1)) 				
				dev->reg.ucr2.bit.SRST = 1;
			break;
		case UCR3:
			dev->reg.ucr3.all = val;
			break;
		case UCR4:
			dev->reg.ucr4.all = val;
            if (dev->reg.ucr4.bit.TCEN == 0) {
                dev->reg.usr2.bit.TXDC = 0;   // Disable TXDC interrupt
            } else if (dev->reg.ucr4.bit.TCEN == 0x1) {
                dev->reg.usr2.bit.TXDC = 0x1; // Enable TXDC interrupt
            }
			break;
		case UFCR:
			dev->reg.ufcr.all = val;
			break;
		case USR1:
			dev->reg.usr1.all = val;
			break;
		case USR2:
			dev->reg.usr2.all = val;
			break;
		case UESC:
			dev->reg.uesc = val;
			break;
		case UTIM:
			dev->reg.utim = val;
			break;
		case UBIR:
			dev->reg.ubir = val;
			break;
		case UBMR:
			dev->reg.ubmr = val;
			break;
		case UBRC:
			dev->reg.ubrc = val;
			break;
		case ONEMS:
			dev->reg.onems = val;
			break;
		case UTS:
			//dev->reg.uts.all = val;
			break;
		case UMCR:
			dev->reg.umcr.all = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in uart_imx6\n", offset);
			break;
	}

	return No_exp;
}

exception_t reset_uart_imx6(conf_object_t* obj, const char *parameter)
{
	return No_exp;
}

static exception_t uart_imx6_receive(conf_object_t *obj, uint32_t value)
{
	return No_exp;
}

static exception_t uart_imx6_ready(conf_object_t *obj)
{
	return No_exp;
}

static conf_object_t *new_uart_imx6(const char *obj_name)
{
	uart_imx6_t* uart = (uart_imx6_t *)skyeye_mm_zero(sizeof(uart_imx6_t));
	uart->obj = new_conf_object(obj_name, (void*)uart);
	uart->read_fifo = CreateFIFO(256);
	reset_uart_imx6(uart->obj, NULL);
	return uart->obj;
}

static exception_t free_uart_imx6(conf_object_t *obj) {
	uart_imx6_t *uart = (uart_imx6_t*) obj->obj;
	if(uart->read_fifo) {
		FreeFIFO(uart->read_fifo);
	}
	skyeye_free(uart);
	return No_exp;
}

void uart_thread(void *obj, time_data_t *data)
{
	uart_imx6_t *dev =  ((conf_object_t*)obj)->obj;
	int ret;
	uint8_t ch;
	if(dev->reg.ucr2.all & 0x2); //if rx enable
	{
        if(dev->term.intf != NULL){
            if(dev->term.intf->read(dev->term.obj, &ch, 1)){
	            WriteFIFO(dev->read_fifo, &ch, 1);
	            if (CheckCanReadNum(dev->read_fifo) >= 1) {
			        dev->reg.uts.all &= ~(1<<5);
                    dev->reg.usr2.bit.RDR = 0x1;
                    if (dev->reg.usr2.bit.RDR == 0x1) { // if rx enable
                        dev->reg.usr2.bit.RDR = 0x1;
                        if (dev->signal.intf) {
                            dev->signal.intf->event_signal(dev->signal.obj, dev->signal.num, 0);
                        }
                    }
	            }
            }
		}
	}

    if (dev->reg.ucr4.bit.DREN == 0x1) {
        if (dev->reg.ucr4.bit.TCEN == 0x1) { // if tx enable
            dev->reg.usr2.bit.TXDC = 0x1;
			dev->reg.uts.bit.TXEMPTY = 0x1;
            if (dev->signal.intf)
                dev->signal.intf->event_signal(dev->signal.obj, dev->signal.num, 0);
        }
    }
}

static exception_t config_uart_imx6(conf_object_t *obj) {
	uart_imx6_t *uart = (uart_imx6_t*) obj->obj;
	uart->reg.uts.all |= 1 << 6; //tx empty
	uart->reg.urxd.all |=0x1; 
	//uart->reg.uts.all |= 1 << 5; //rx empty

	system_register_timer_handler(obj, 100, uart_thread, SCHED_NORMAL|SCHED_NS);
	return No_exp;
}

static exception_t set_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	uart_imx6_t *dev =  conf_obj->obj;
	dev->signal.obj = SKY_attr_object(*value);
	dev->signal.intf = (general_signal_intf *)SKY_get_iface(dev->signal.obj, GENERAL_SIGNAL_INTF_NAME);
	return No_exp;
}
static attr_value_t get_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	uart_imx6_t *dev =  conf_obj->obj;
	attr_value_t signal_attr = SKY_make_attr_object(dev->signal.obj);
	return signal_attr;
}

static exception_t set_attr_term(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	uart_imx6_t *dev =  conf_obj->obj;
	dev->term.obj = SKY_attr_object(*value);
	dev->term.intf = (skyeye_serial_device_t *)SKY_get_iface(dev->term.obj, SERIAL_DEVICE_INTERFACE);
	return No_exp;
}
static attr_value_t get_attr_term(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	uart_imx6_t *dev =  conf_obj->obj;
	attr_value_t term_obj = SKY_make_attr_object(dev->term.obj);
	return term_obj;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	uart_imx6_t *dev =  conf_obj->obj;
	attr_value_t int_number_attr = SKY_make_attr_uinteger(dev->signal.num);
	return int_number_attr;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	uart_imx6_t *dev =  conf_obj->obj;
	dev->signal.num = SKY_attr_uinteger(*value);
	return No_exp;
}

static void uart_imx6_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "term", get_attr_term, NULL, set_attr_term, NULL, SKY_Attr_Optional, "object", "uart term for imx6 uart");
	SKY_register_attribute(clss, "signal", get_attr_signal, NULL, set_attr_signal, NULL, SKY_Attr_Optional, "object", "signal for imx6 signal");
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of imx6 uart");
	return;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	uart_imx6_t *dev =  obj->obj;
	dev->signal.obj = obj2;
	dev->signal.intf = (general_signal_intf *)SKY_get_iface(dev->signal.obj, GENERAL_SIGNAL_INTF_NAME);
	if(dev->signal.intf == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from %s fail.", obj2->objname);
	}

	return No_exp;
}
 
static exception_t signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	uart_imx6_t *dev =  obj->obj;
	*obj2 = dev->signal.obj;
	*port = NULL;
	return No_exp;
}

static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	uart_imx6_t *dev =  obj->obj;
	dev->term.obj = obj2;
	dev->term.intf = (skyeye_uart_intf*)SKY_get_iface(obj2, SKYEYE_UART_INTF);
	if(dev->term.intf == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from %s fail.", obj2->objname);
	}
	int  pthread_id;
	//create_thread_scheduler(1000, Periodic_sched, uart_do_cycle, dev->obj, &pthread_id);

	return No_exp;
}
 
static exception_t uart_term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	uart_imx6_t *dev =  obj->obj;
	*obj2 = dev->term.obj;
	*port = NULL;
	return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	uart_imx6_t *dev = obj->obj;
	dev->uart_file = obj2;
	dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
	if (dev->uart_file_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Get 'skyeye_uart_intf' interface from %s fail.", obj2->objname);
		return Not_found_exp;
	}
	return No_exp;
}
 
static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	uart_imx6_t *dev = obj->obj;
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}

void init_imx6_uart(void) {
	static skyeye_class_t class_data = {
		.class_name = "uart_imx6",
		.class_desc = "uart imx6",
		.new_instance = new_uart_imx6,
		.free_instance = free_uart_imx6,
		.reset_instance = reset_uart_imx6,
		.config_instance = config_uart_imx6,
		.module_type = SKYML_ONCHIP(SC_imx6),
	};
	conf_class_t* class = SKY_register_device_class(class_data.class_name,&class_data);

	static const memory_space_intf io_memory = {
		.read = uart_imx6_read,
		.write = uart_imx6_write,
	};
	SKY_register_iface(class, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_serial_device_t recv_intf = {
		.write = uart_imx6_receive,
		.receive_ready = uart_imx6_ready,
		.write_chars = NULL,
	};
	SKY_register_iface(class, SERIAL_DEVICE_INTERFACE, &recv_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SERIAL_DEVICE_INTERFACE,
			.iface = &recv_intf,
		},
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME, 
			.iface = &io_memory,
		},
 	};
	static const struct ConnectDescription connects[] = {

		(struct ConnectDescription ){
			.name = SKYEYE_UART_INTF,
			.set = uart_term_set,
			.get = uart_term_get,
		},
		(struct ConnectDescription ){
			.name = UART_FILE_INTF,
			.set = uart_file_set,
			.get = uart_file_get,
		},
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = signal_set,
			.get = signal_get,
		}
	};
	class_register_ifaces(class, ifaces);
	class_register_connects(class, connects);


	uart_imx6_register_attribute(class);
}


/*
 * Copyright (C)
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

/*
 * @file uart_pc.c
 * @brief The implementation of skyeye uart connect to PC port
 * @author zyuming zyumingfit@gmail.com
 * @version 78.77
 * @date 2014-1-8
 */


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
#include <simics/base_types.h>
#include "skyeye_thread.h"
#include <skyeye_serial.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>

#include "uart_pc.h"
#include "serial.h"


#ifndef __MINGW32__
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#else
#include <windows.h>
#endif

#define TRUE  0
#define  FALSE -1

static int uart_pc_init(conf_object_t* opaque);

#ifndef __MINGW32__
static int linux_serial_init(void *pc_dev)
{
	uart_pc_device *dev = pc_dev;
	dev->port_fd = FALSE;
	int ret;
	dev->port_fd = UART_Open(dev->port_fd, dev->serial->port);
	if(FALSE == dev->port_fd){
		printf("Open Serial Port File Error\n");
		return -1;
	}
	ret =  UART_Init(dev->port_fd, dev->serial->baudrate, dev->serial->flow_ctrl, dev->serial->data_bits, dev->serial->stop_bits, dev->serial->parity);
	if(FALSE == ret){
		printf("Set Port Error\n");
		printf("Connect to %s Failed\n", dev->serial->port);
		return -1;
	}else{
		printf("Connect to %s Successfull\n", dev->serial->port);
	}

	return 0;
}

#else
static int win_serial_init(void *pc_dev)
{
	uart_pc_device *dev = pc_dev;
	int ret;
        dev->port_fd = WIN_UART_Open(dev->serial->port);
	if(dev->port_fd == (HANDLE) -1){
		printf("Open Serial Port File Error\n");
		return -1;
	}

	ret = WIN_UART_Set(dev->port_fd, dev->serial->baudrate, dev->serial->flow_ctrl, dev->serial->data_bits, dev->serial->stop_bits, dev->serial->parity);
	if(ret == -1){
		printf("Set Port Error\n");
		printf("Connect to %s Failed\n", dev->serial->port);
	}else{
		printf("Connect to %s Successfull\n", dev->serial->port);
	}
	return 0;
}
#endif


static exception_t uart_pc_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	uart_pc_device *dev = opaque->obj;
	skyeye_config_t *config = get_current_config();
	int serial_count = config->serial_count;
	char *objname = config->serial_parameter[serial_count].objname;
	char *port =  config->serial_parameter[serial_count].port;
	if(!strncmp(attr_name, "steup", strlen("steup"))){
		if(!strncmp(value->u.string.str, "run", strlen("run"))){
			strncpy(objname, opaque->objname, strlen(opaque->objname));
			config->serial_count += 1;
			uart_pc_init(opaque);
			dev->SteupFlag = 1;
		}
	}else if(!strncmp(attr_name, "baudrate", strlen("baudrate"))){
		dev->serial->baudrate = value->u.uinteger;
		config->serial_parameter[serial_count].baudrate = value->u.uinteger;

	}else if(!strncmp(attr_name, "port", strlen("port"))){
		strncpy(dev->serial->port, value->u.string.str, strlen(value->u.string.str) + 1);
		strncpy(port, value->u.string.str, strlen(value->u.string.str));
	}else if(!strncmp(attr_name, "DataBits", strlen("DataBits"))){
		dev->serial->data_bits = value->u.uinteger;
		config->serial_parameter[serial_count].data_bits = value->u.uinteger;
	}else if(!strncmp(attr_name, "StopBits", strlen("StopBits"))){
		dev->serial->stop_bits = value->u.uinteger;
		config->serial_parameter[serial_count].data_bits = value->u.uinteger;
	}else if(!strncmp(attr_name, "parity", strlen("parity"))){
		dev->serial->parity = value->u.uinteger;
		config->serial_parameter[serial_count].parity = value->u.uinteger;
	}else{
		printf("parameter error: %s\n", attr_name);
		return Invarg_exp;
	}
	return No_exp;
}

static exception_t uart_pc_read(conf_object_t *opaque, void* buf, size_t count)
{
	uart_pc_device *dev = opaque->obj;
	rec_t* receive = dev->receive;
	int i = 0;
	char* rec_buf = buf;
	if (receive->mutex == 1) {
		/* move the charaters in buffer */
		while(i < count && (receive->rec_tail < receive->rec_count)){
			if(receive->rec_head >= receive->rec_tail){
				/* No data */
				receive->rec_head = 0;
				receive->rec_tail = 0;
				return Not_found_exp;
			}else{
				rec_buf[i++] = receive->rec_buf[receive->rec_head++];
				if(receive->rec_head == receive->rec_tail){
					/* data have been read*/
					receive->rec_head = 0;
					receive->rec_tail = 0;
				}
			}
		}
		receive->mutex = 0;
	}else{
		return Not_found_exp;
	}

	return No_exp;
}

static exception_t uart_pc_write(conf_object_t *opaque, void* buf, size_t count)
{
	int ret = -1;
	uart_pc_device *dev = opaque->obj;
#ifndef __MINGW32__
	UART_Send(dev->port_fd, buf, count);
#else
	WIN_UART_Send(dev->port_fd, buf, count);
#endif
	return No_exp;
}

static void *serial_receive(void *serial_dev)
{
	uart_pc_device *dev = serial_dev;
	rec_t* receive = dev->receive;
	int ret;
	char rcv_buf[512];
	while(1){
#ifndef __MINGW32__
		ret = UART_Recv(dev->port_fd, rcv_buf,512);	
#else
		ret = WIN_UART_Recv(dev->port_fd, rcv_buf,512);	
#endif
		if(ret > 0){
			if(receive->rec_tail >= receive->rec_count) {
				fprintf(stderr, "Overflow for uart link.\n");
			} else {
				while(receive->mutex);
				if(receive->rec_count - receive->rec_tail >= ret){
					memmove(receive->rec_buf + receive->rec_tail, rcv_buf, ret);
				}else{
					memmove(receive->rec_buf + receive->rec_tail, rcv_buf, receive->rec_count - receive->rec_tail);
				}
				receive->mutex = 1;
				receive->rec_tail += ret;
			}
		}else{
			printf("Recevie Nothing\n");

		}
	}
}

static int uart_pc_init(conf_object_t* opaque)
{
	uart_pc_device *dev_uart = opaque->obj;
	skyeye_config_t *config = get_current_config();
	int ret;
		if(dev_uart->SteupFlag == 1){
			if(dev_uart->serial != NULL){
#ifndef __MINGW32__
				ret = linux_serial_init(dev_uart);
#else
				ret = win_serial_init(dev_uart);
#endif
				pthread_t pthread_id;
				if(ret != -1){
					dev_uart->serial->connect_flag = 1;
					set_serial_connect_flag(opaque->objname, 1);	
					create_thread(serial_receive, (void*)dev_uart, &pthread_id);
				}
			}
		}else
			printf("%s:Can Not Found Host\n", opaque->objname);
}


static conf_object_t* new_uart_pc(const char* obj_name){
	uart_pc_device* dev_uart = skyeye_mm_zero(sizeof(uart_pc_device));
	int ret;
	rec_t* receive = skyeye_mm_zero(sizeof(rec_t));
	dev_uart->obj = new_conf_object(obj_name, dev_uart);
	dev_uart->obj_name = obj_name;
	dev_uart->SteupFlag = 0;
	dev_uart->serial = skyeye_mm(sizeof(serial_t));

	/* initial recevie */
	dev_uart->receive = receive;
	receive->rec_buf = skyeye_mm_zero(MAX_REC_NUM);
	receive->rec_head = 0;
	receive->rec_tail = 0;
	receive->mutex = 0;
	receive->rec_count = MAX_REC_NUM;
	
	return dev_uart->obj;
}

exception_t free_uart_pc(conf_object_t* dev){
	uart_pc_device* dev_uart = dev->obj; 
	skyeye_free(dev_uart->receive);
	skyeye_free(dev_uart->serial);

#ifndef __MINGW32__
	UART_Close(dev_uart->port_fd);
#else
	WIN_UART_Close(dev_uart->port_fd);
#endif

	return No_exp;
}

void init_uart_pc(){
	static skyeye_class_t class_data = {
		.class_name = "uart_pc",
		.class_desc = "uart_pc",
		.new_instance = new_uart_pc,
		.free_instance = free_uart_pc,
		.get_attr = NULL,
		.set_attr = uart_pc_set_attr,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const skyeye_uart_intf uart_method = {
		.read = uart_pc_read,
		.write = uart_pc_write,
	};
	SKY_register_iface(clss, SKYEYE_UART_INTF, &uart_method);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SKYEYE_UART_INTF,
			.iface = &uart_method,
		}
 	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, CONNECTS_NULL);
}

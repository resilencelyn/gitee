/*
 * dev_iface.h: 
 *
 * Copyright (C) 2013 alloc <alloc.young@gmail.com>
 * Skyeye Develop Group, for help please send mail to
 * <skyeye-developer@lists.gro.clinux.org>
 *
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#ifndef __DEV_IFACE_H__ 
#define __DEV_IFACE_H__  
#include "generic_transaction.h"
//typedef void interface_t;
typedef struct signal {
	void (*signal_raise)(conf_object_t *obj);
	void (*signal_lower)(conf_object_t *obj);
} signal_interface_t;

typedef struct memory_space_s {
	exception_type_t (*access)(conf_object_t *obj,
								generic_transaction_t *mop);
	exception_type_t (*access_simple)(conf_object_t *obj,
									  conf_object_t *initiator,
									  physical_address_t addr,
									  uint8 *buf,
									  physical_address_t len,
									  read_or_write_t rw,
									  endianness_t endian);
} memory_space_interface_t;
typedef struct skyeye_uart_intf{
        conf_object_t* conf_obj;
        exception_t (*write) (conf_object_t *opaque, void* buf, size_t count);
        exception_t (*read) (conf_object_t *opaque, void* buf, size_t count);
}skyeye_uart_intf_interface_t;

typedef struct skyeye_file_data_timer_intf{
        conf_object_t* conf_obj;
        exception_t (*set_timer)(conf_object_t *obj, conf_object_t *dev_obj, int ms);
}skyeye_file_data_timer_intf;

#endif /* __DEV_IFACE_H__ */

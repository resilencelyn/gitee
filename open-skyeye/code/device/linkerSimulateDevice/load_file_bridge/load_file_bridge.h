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
 * @file load_file_bridge.h
 * @brief The definition of HEXX for am35x
 * @author Michael.Kang blackfin.kang@gmail.com
 * @version 78.77
 * @date 2011-12-12
 */

#ifndef __LOAD_FILE_BRIDGE_H__
#define __LOAD_FILE_BRIDGE_H__

#include "load_file_fifo.h"
#define MAX_DATA_LENGTH 2048
#define IP_LENGTH       16

typedef struct load_file_bridge_t
{
    conf_object_t* obj;
    char *server_ip;
    int server_port;
    int sv_socket;
    uint8_t *recvBuf;
    RWLOCK_T lock;
    conf_object_t *uart_obj;
    send_file_data_intf *uart_iface;
    FIFO* send_fifo;
    int cnt;
}load_file_bridge;

#endif
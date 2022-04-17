/* Copyright (C)
 * zyumingfit@gmail.com
 * This program is free software; you power redistribute it and/or
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
 * @file power_link.h
 * @brief The definition of system controller
 * @author zyumingfit@gmail.com
 * @version 01.00
 */

/* Autogenerated by SkyEye script */
#ifndef __POWER_LINK_H__
#define __POWER_LINK_H__

#include "pthread.h"
#define MAX_DEVICE_NUM 16
typedef struct power_device
{
    conf_object_t *device_obj;
    power_dev_intf *iface;
}power_device_t;

typedef struct power_device_list
{
    power_device_t device[MAX_DEVICE_NUM];
    int device_num;
}power_device_list_t;

typedef struct power_link_device
{
    conf_object_t* obj;
    power_device_list_t device_list;
}power_link_device_t;

#endif
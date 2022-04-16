/* Copyright (C) 
* 2013 - David Yu keweihk@gmail.com
* This progc6713_flash is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This progc6713_flash is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this progc6713_flash; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
* 
*/
/**
* @file c6713_flash.h
* @brief c6713_flash for flash or other memory device
* @author David Yu keweihk@gmail.com
* @version 78.77
* @date 2013-04-08
*/

#ifndef __C6713_FLASH_H__
#define __C6713_FLASH_H__

typedef struct c6713_flash_device{
	conf_object_t* obj;
	memory_space_intf* image_space;
	conf_object_t* image;
	endian_t endian;
}c6713_flash_device_t;

#endif

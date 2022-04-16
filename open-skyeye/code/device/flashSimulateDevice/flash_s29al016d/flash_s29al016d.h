/* Copyright (C) 
* 2013 - David Yu keweihk@gmail.com
* This progflash_s29al016d is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This progflash_s29al016d is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this progflash_s29al016d; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
* 
*/
/**
* @file flash_s29al016d.h
* @brief flash_s29al016d for flash or other memory device
* @author David Yu keweihk@gmail.com
* @version 78.77
* @date 2013-04-08
*/

#ifndef __FLASH_S29AL016D_H__
#define __FLASH_S29AL016D_H__
#include<stdio.h>

struct flash_bus {
	uint32_t addr;
	uint32_t data;
};

typedef struct flash_s29al016d_device{
	conf_object_t* obj;
	memory_space_intf* image_space;
	conf_object_t* image;
	endian_t endian;
	int erase_val;
	int cnt;
	int n_bus;
	struct flash_bus bus[6];
}flash_s29al016d_device_t;

#define CHIP_SIZE   0x200000
#define SECTOR_SIZE 0x10000

#define CMD_CHECK(io, n, o, v)				\
	(io->bus[n].addr == o && io->bus[n].data == v)

#define CMD_WORD_PROGRAM(io)				\
	(io->cnt == 3 &&				\
	 CMD_CHECK(io, 0, 0xaaa, 0xaa) &&		\
	 CMD_CHECK(io, 1, 0x554, 0x55) &&		\
	 CMD_CHECK(io, 2, 0xaaa, 0xa0))

#define CMD_ERASE(io)					\
	(io->cnt == 5 &&				\
	 CMD_CHECK(io, 0, 0xaaa, 0xaa) &&		\
	 CMD_CHECK(io, 1, 0x554, 0x55) &&		\
	 CMD_CHECK(io, 2, 0xaaa, 0x80) &&		\
	 CMD_CHECK(io, 3, 0xaaa, 0xaa) &&		\
	 CMD_CHECK(io, 4, 0x554, 0x55))
#endif


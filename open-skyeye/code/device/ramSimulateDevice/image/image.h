/* Copyright (C)
 * 2013 - David Yu keweihk@gmail.com
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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
 * @file image.h
 * @brief image for ram, flash or other memory device
 * @author David Yu keweihk@gmail.com
 * @version 78.77
 * @date 2013-04-08
 */

#ifndef __IMAGE_H
#define __IMAGE_H__

#define BANK_SIZE	0x1000000	//16M
#define BANK_NUM	256

typedef struct image_module{
	conf_object_t* obj;
	uint8_t** image_ptr;
	uint32_t size;
	uint32_t index_array[BANK_NUM];
	uint32_t index_count;
	get_page_t page;
	char save_path[128];
	char load_file[256];
}image_module_t;

#endif

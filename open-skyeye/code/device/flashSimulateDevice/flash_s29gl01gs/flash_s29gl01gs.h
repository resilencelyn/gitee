/* Copyright (C)
* 2013 - David Yu keweihk@gmail.com
* This progflash_s29gl01gs is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This progflash_s29gl01gs is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this progflash_s29gl01gs; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/
/**
* @file flash_s29gl01gs.h
* @brief flash_s29gl01gs for flash or other memory device
*/

#ifndef __FLASH_S29GL01GS_H__
#define __FLASH_S29GL01GS_H__
#include<stdint.h>

typedef struct flash_s29gl01gs_device{
	conf_object_t* obj;
	memory_space_intf* image_space;
	//skyeye_image_info *image_info;
	conf_object_t* image;

	int cycles;
	int status_cycles;

	int is_status_read;

	int is_write2buf;
	int write_buf_wc;

	int is_erase;
	int is_cfi;
	int is_id_entry;

	int is_word_program;

#define RESET_FLAGS(dev) do {		\
	dev->is_cfi		= 0;	\
	dev->is_id_entry	= 0;	\
	dev->cycles		= 0;	\
	dev->is_erase		= 0;	\
	dev->is_write2buf	= 0;	\
	dev->is_word_program	= 0;	\
} while (0);
}flash_s29gl01gs_device_t;

static unsigned short cfi[] = {
	0x0001,/* 0000 */
	0x227e,/* 0001 */
	0x0000,/* 0002 */
	0x0000,/* 0003 */
	0x0000,/* 0004 */
	0x0000,/* 0005 */
	0x0000,/* 0006 */
	0x0000,/* 0007 */
	0x0000,/* 0008 */
	0x0000,/* 0009 */
	0x0000,/* 000a */
	0x0000,/* 000b */
	0x0005,/* 000c */
	0x0000,/* 000d */
	0x2228,/* 000e */
	0x2201,/* 000f */

	0x0051,/* 0010 */
	0x0052,/* 0011 */
	0x0059,/* 0012 */
	0x0002,/* 0013 */
	0x0000,/* 0014 */
	0x0040,/* 0015 */

	0x0000,/* 0016 */
	0x0000,/* 0017 */
	0x0000,/* 0018 */
	0x0000,/* 0019 */
	0x0000,/* 001a */

	0x0027,/* 001b */
	0x0036,/* 001c */
	0x0000,/* 001d */
	0x0000,/* 001e */
	0x0008,/* 001f */
	0x0009,/* 0020 */
	0x0008,/* 0021 */
	0x0012,/* 0022 */

	0x0001,/* 0023 */
	0x0002,/* 0024 */
	0x0003,/* 0025 */

	0x0003,/* 0026 */
	0x001b,/* 0027 */
	0x0001,/* 0028 */
	0x0000,/* 0029 */
	0x0009,/* 002a */

	0x0000,/* 002b */
	0x0001,/* 002c */

	0x00ff,/* 002d */
	0x0003,/* 002e */
	0x0000,/* 002f */
	0x0002,/* 0030 */

	0x0000,/* 0031 */
	0x0000,/* 0032 */
	0x0000,/* 0033 */
	0x0000,/* 0034 */

	0x0000,/* 0035 */
	0x0000,/* 0036 */
	0x0000,/* 0037 */
	0x0000,/* 0038 */

	0x0000,/* 0039 */
	0x0000,/* 003a */
	0x0000,/* 003b */
	0x0000,/* 003c */

	0xffff,/* 003d */
	0xffff,/* 003e */
	0xffff,/* 003f */
};

#define WIDTH 2
#define SECTOR_SIZE 0x20000

#define ASO_EXIT(addr, val)				\
	((val) == 0xf0)
#define STATUS_REG_READ(addr, val)			\
	((addr) == 0x555 * WIDTH && (val) == 0x70)
#define STATUS_REG_CLEAR(addr, val)			\
	((addr) == 0x555 * WIDTH && (val) == 0x71)
#define PROGRAM_BUF2FLASH(addr, val)			\
	((val) == 0x29)
/* ----------------------------------------------------- */
#define IS_UNLOCK_1(addr, val)				\
	((addr) == 0x555 * WIDTH && (val) == 0xaa)
#define IS_UNLOCK_2(addr, val)				\
	((addr) == 0x2aa * WIDTH && (val) == 0x55)

#define WORD_PROGRAM(addr, val)				\
	((addr) == 0x555 * WIDTH && (val) == 0xa0)
#define WRITE2BUF(addr, val)				\
	((val) == 0x25)
#define WRITE2BUF_ABORT(addr, val)			\
	((addr) == 0x555 * WIDTH && (val) == 0xf0)
#define ERASE(addr, val)				\
	((addr) == 0x555 * WIDTH && (val) == 0x80)

#define ID_ENTRY(addr, val)				\
	((addr) == 0x555 * WIDTH && (val) == 0x90)
#define SSR_ENTRY(addr, val)				\
	((addr) == 0x555 * WIDTH && (val) == 0x88)
#define LOCK_REGISTER_ENTRY(addr, val)			\
	((addr) == 0x555 * WIDTH && (val) == 0x40)
#define LOCK_PASSWORD_ENTRY(addr, val)			\
	((addr) == 0x555 * WIDTH && (val) == 0x60)
#define LOCK_PPB_ENTRY(addr, val)			\
	((addr) == 0x555 * WIDTH && (val) == 0xc0)
#define LOCK_PPBLOCK_ENTRY(addr, val)			\
	((addr) == 0x555 * WIDTH && (val) == 0x50)
#define LOCK_DYB_ENTRY(addr, val)			\
	((addr) == 0x555 * WIDTH && (val) == 0xe0)
#define CFI_ENTRY(addr, val)			\
	((val) == 0x98)

#endif


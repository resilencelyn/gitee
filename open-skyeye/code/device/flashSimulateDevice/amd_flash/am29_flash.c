/*
	dev_flash_am29.c - skyeye AMD Am29xxxxxx flash simulation
	Copyright (C) 2007 Skyeye Develop Group
	for help please send mail to <skyeye-developer@lists.gro.clinux.org>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
*/

/*
 * 2007.04.05	Written by Anthony Lee
 */

#include <stdio.h>
#include <stdlib.h>

//#include "armdefs.h"
#include "skyeye_device.h"
#include "amd_flash.h"
#include "bank_defs.h"
#include <skyeye_core.h>
#include <skyeye_swapendian.h>

#define FLASH_AM29_DEBUG			0

#define PRINT(x...)				printf("[FLASH_AM29]: " x)

#if FLASH_AM29_DEBUG
#define DEBUG(x...)				printf("[FLASH_AM29]: " x)
#else
#define DEBUG(x...)				(void)0
#endif

static void flash_am29_fini(void* obj)
{
	struct flash_am29_io *io = (struct flash_am29_io*)obj;

	free(io);
}


static void flash_am29_reset(void* obj)
{
	struct flash_am29_io *io = (struct flash_am29_io*)obj;
	io->bcnt = io->n_bbus = 0;
	memset(&io->bbus[0], 0, sizeof(io->bbus[0]) * 6);

	io->wcnt = io->n_wbus = 0;
	memset(&io->wbus[0], 0, sizeof(io->wbus[0]) * 6);

	io->dump_cnt = 0xffff;
	io->dump_flags = 0;
}


static void flash_am29_update(void* obj)
{
	struct flash_am29_io *io = (struct flash_am29_io*)obj;
#if 0
	if (flash_dev->dump[0] == 0) return;
	if (io->dump_flags == 0 || (io->dump_flags & 0x2) != 0) return;

	io->dump_cnt -= 1;

	if (io->dump_cnt == 0) {
		if (skyeye_flash_dump(flash_dev->dump, dev->base, io->chip_size) != 0) {
			io->dump_flags |= 0x2;
			printf("\n");
			PRINT("*** FAILED: Can't dump to %s\n", flash_dev->dump);
			return;
		}

		io->dump_cnt = 0xffff;
		io->dump_flags = 0;

		printf("\n");
		PRINT("Dumped to %s\n", flash_dev->dump);
	}
#endif
}


static int flash_am29_read_byte(void *obj, uint32_t addr, uint8_t *data)
{
	struct flash_am29_io *io = (struct flash_am29_io*)obj;

	uint32_t offset = addr;

	if (io->bautoselect != NULL && io->bcnt > 0) {
		if (io->query != NULL && CMD_BYTE_QUERY(io)) {
			*data = ((offset >> 1) < io->max_query ? *(io->query + (offset >> 1)) : 0x0);
		}

		if (CMD_BYTE_AUTOSELCT(io)) {
			io->bautoselect(io, offset, data);
		}

		DEBUG("read_byte(offset:0x%08x, data:0x%x)\n", offset, *data);
	}

	if (io->bcnt == 0) {
		/* read data from addr */
		/*
		global_mbp = bank_ptr(addr);
		*data = real_read_byte(state, addr);
		*/
		exception_t ret = io->image->read(io->image->conf_obj, offset, data, 1);
		if(ret != No_exp){
			printf("In %s, Not_found offset 0x%x\n", __func__,  offset);
			return Not_found_exp;
		}
	}

	io->n_bbus = 0;

	return No_exp;
}


static int flash_am29_write_byte(void* obj, uint32_t addr, uint8_t data)
{
	struct flash_am29_io *io = (struct flash_am29_io*)obj;
	uint32_t offset = addr;
	uint32_t start, end;

	if (io->bautoselect == NULL) {
		PRINT("write_byte: Unsupported !!!\n");
		return Invmod_exp;
	}

	DEBUG("write_byte(%dst Bus, offset:0x%08x, data:0x%x)\n", io->n_bbus + 1, offset, data);

	if (io->n_bbus == 0 && (data == 0xb0 || data == 0x30)) {
		DEBUG("Erase Suspend/Resume.\n");
		return ADDR_NOHIT;
	}

	if (CMD_BYTE_PROGRAM(io)) {
		/* write data to addr */
		exception_t ret = io->image->write(io->image->conf_obj, offset, &data, 1);
		if(ret != No_exp){
			printf("In %s, Not_found offset 0x%x\n", __func__, offset);
			return Not_found_exp;
		}

		io->dump_flags |= 0x1;
		goto reset;
	}

	if (CMD_BYTE_ERASE(io)) {
		switch (data) {
			case 0x10: /* Chip-Erase */
				start = 0;
				end = start + io->chip_size;
				break;

			case 0x30: /* Sector-Erase */
				start = addr;
				end = start + io->sector_size;
				break;

			default:
				start = end = 0x0;
				break;
		}

		if (end > start && end <= io->chip_size) {
			for (addr = start; addr < end; addr += 4) {
				uint32_t erase_value = 0xffffffff;
				exception_t ret = io->image->write(io->image->conf_obj, offset, &erase_value, 4);
				if(ret != No_exp){
					printf("In %s, Not_found offset 0x%x\n", __func__, offset);
					return Not_found_exp;
				}
			}
			DEBUG("*** Erase(start:0x%08x, end:0x%08x)\n", start, end);
		} else {
			PRINT("*** ERROR: Erase(start:0x%08x, end:0x%08x)\n", start, end);
		}
		goto reset;
	}

	if (io->n_bbus < 6) {
		io->bbus[io->n_bbus].addr = offset;
		io->bbus[io->n_bbus].data = data;
		io->n_bbus += 1;

		io->bcnt = io->n_bbus;

		if (data == 0xf0) goto reset; /* CMD_RESET */
	}

	if (CMD_BYTE_UNLOCK_BYPASS(io) || CMD_BYTE_UNLOCK_BYPASS_PROGRAM_RESET(io)) {
		DEBUG("*** WARNING: Unlock Bypass.\n");
		goto reset;
	}

	goto exit;

reset:
	io->bcnt = io->n_bbus = 0;
	memset(&io->bbus[0], 0, sizeof(io->bbus[0]) * 6);

exit:
	return No_exp;
}


static int flash_am29_read_halfword(void *obj, uint32_t addr, uint16_t *data)
{
	struct flash_am29_io *io = (struct flash_am29_io*)obj;

	uint32_t offset = addr >> 1;

	if (io->wautoselect != NULL && io->wcnt > 0) {
		if (io->query != NULL && CMD_WORD_QUERY(io)) {
			*data = (offset < io->max_query ? *(io->query + offset) : 0x0);
		}

		if (CMD_WORD_AUTOSELCT(io)) {
			io->wautoselect(io, offset, data);
		}

		DEBUG("read_halfword(offset:0x%08x, data:0x%x)\n", offset, *data);
	}

	if (io->wcnt == 0) {
		/* read data from addr */
		exception_t ret = io->image->read(io->image->conf_obj, offset, data, 2);
		if(ret != No_exp){
			printf("In %s, Not_found offset 0x%x\n", __func__, offset);
			return Not_found_exp;
		}
		//generic_arch_t* arch_instance = get_arch_instance(NULL);
		/* judge the endianess */
		if(SIM_get_current_endianess() == Big_endian)
			*data = half_from_BE(*data);
	}

	io->n_wbus = 0;

	return No_exp;
}


static int flash_am29_write_halfword(void *obj, uint32_t addr, uint16_t data)
{
	struct flash_am29_io *io = (struct flash_am29_io*)obj;

	uint32_t offset = addr >> 1;
	uint32_t start, end;

	if (io->wautoselect == NULL) {
		PRINT("write_halfword: Unsupported !!!\n");
		return ADDR_NOHIT;
	}

	DEBUG("write_halfword(%dst Bus, offset:0x%08x, data:0x%x)\n", io->n_wbus + 1, offset, data);

	if (io->n_wbus == 0 && (data == 0xb0 || data == 0x30)) {
		DEBUG("Erase Suspend/Resume.\n");
		return ADDR_NOHIT;
	}

	if (CMD_WORD_PROGRAM(io)) {
		/* write data to addr */
		/*
		global_mbp = bank_ptr(addr);
		real_write_halfword(state, addr, data);
		*/
		//generic_arch_t* arch_instance = get_arch_instance(NULL);
		/* judge the endianess */
		if(SIM_get_current_endianess() == Big_endian)
			data = half_from_BE(data);

		exception_t ret = io->image->write(io->image->conf_obj, offset, &data, 2);
		if(ret != No_exp){
			printf("In %s, Not_found offset 0x%x\n", __func__, offset);
			return Not_found_exp;
		}

		io->dump_flags |= 0x1;
		goto reset;
	}

	if (CMD_WORD_ERASE(io)) {
		switch (data) {
			case 0x10: /* Chip-Erase */
				start = 0; 
				end = start + io->chip_size;
				break;

			case 0x30: /* Sector-Erase */
				start = addr;
				end = start + io->sector_size;
				break;

			default:
				start = end = 0x0;
				break;
		}
		if (end > start && end <= io->chip_size) {
			for (addr = start; addr < end; addr += 4) {
				uint32_t erase_value = 0xffffffff;
				exception_t ret = io->image->write(io->image->conf_obj,offset, &erase_value, 4);
				if(ret != No_exp){
					printf("In %s, Not_found offset 0x%x\n", __func__, offset);
					return Not_found_exp;
				}
			}
			DEBUG("*** Erase(start:0x%08x, end:0x%08x)\n", start, end);
		} else {
			PRINT("*** ERROR: Erase(start:0x%08x, end:0x%08x)\n", start, end);
		}
		goto reset;
	}

	if (io->n_wbus < 6) {
		io->wbus[io->n_wbus].addr = offset;
		io->wbus[io->n_wbus].data = data;
		io->n_wbus += 1;

		io->wcnt = io->n_wbus;

		if (data == 0xf0) goto reset; /* CMD_RESET */
	}

	if (CMD_WORD_UNLOCK_BYPASS(io) || CMD_WORD_UNLOCK_BYPASS_PROGRAM_RESET(io)) {
		DEBUG("*** WARNING: Unlock Bypass.\n");
		goto reset;
	}

	goto exit;

reset:
	io->wcnt = io->n_wbus = 0;
	memset(&io->wbus[0], 0, sizeof(io->wbus[0]) * 6);

exit:
	return No_exp;
}


static int flash_am29_read_word(void *obj, uint32_t addr, uint32_t *data)
{
	struct flash_am29_io *io = (struct flash_am29_io*)obj;
	exception_t ret = io->image->read(io->image->conf_obj, addr, data, 4);
	if(ret != No_exp){
		printf("In %s, Not_found offset 0x%x\n", __func__, addr);
		return Not_found_exp;
	}
	//generic_arch_t* arch_instance = get_arch_instance(NULL);
	/* judge the endianess */
	if(SIM_get_current_endianess() == Big_endian)
		*data = word_from_BE(*data);
	DEBUG("read_word(addr:0x%08x, data:0x%x)\n", addr, *data);

	return No_exp;
}


static int flash_am29_write_word(void *obj, uint32_t addr, uint32_t data)
{
#if 0
	struct flash_am29_io *io = (struct flash_am29_io*)dev->data;
	struct machine_config *mc = (struct machine_config*)dev->mach;
	ARMul_State *state = (ARMul_State*)mc->state;

	global_mbp = bank_ptr(addr);
	real_write_word(state, addr, data);

	io->dump_flags |= 0x1;

	DEBUG("write_word(addr:0x%08x, data:0x%x)\n", addr, data);

	return ADDR_HIT;
#else
	PRINT("write_word: Unsupported !!! addr 0x%x, 0x%x\n", addr, data);
	return No_exp;
#endif
}


int flash_am29_setup(struct flash_am29_io *io,
			    uint16_t *query, int max_query,
			    flash_am29_bautoselect_func bautoselect,
			    flash_am29_wautoselect_func wautoselect,
			    uint32_t chip_size, uint32_t sector_size)
{
	if (io == NULL) return -1;

	io->query = query;
	io->max_query = max_query;

	io->bautoselect = bautoselect;
	io->wautoselect = wautoselect;

	if (chip_size == 0 && query != NULL)
		io->chip_size = (0x1 << query[0x27]);
	else
		io->chip_size = chip_size;

	if (sector_size == 0 && query != NULL)
		io->sector_size = io->chip_size / max(((uint32_t)query[0x34] << 8) | (uint32_t)query[0x33], 1);
	else
		io->sector_size = sector_size;

	DEBUG("chip_size:%dM, sector_size:%dK\n", io->chip_size >> 20, io->sector_size >> 10);

	io->flash_intf.fini = flash_am29_fini;
	io->flash_intf.reset = flash_am29_reset;
	io->flash_intf.update = flash_am29_update;
	io->flash_intf.read_byte = flash_am29_read_byte;
	io->flash_intf.write_byte = flash_am29_write_byte;
	io->flash_intf.read_halfword = flash_am29_read_halfword;
	io->flash_intf.write_halfword = flash_am29_write_halfword;
	io->flash_intf.read_word = flash_am29_read_word;
	io->flash_intf.write_word = flash_am29_write_word;

	flash_am29_reset(io);

	return 0;
}


static uint16_t am29lv160_query[0x4d] = {
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0 - 4 */
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 5 - 9 */
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* a - e */
	0x0000,					/* f */

	0x0051, 0x0052, 0x0059,			/* 10 - 12 : QRY*/
	0x0002, 0x0000,				/* 13 - 14 */
	0x0040, 0x0000,				/* 15 - 16 */
	0x0000, 0x0000,				/* 17 - 18 */
	0x0000, 0x0000,				/* 19 - 1a */

	0x0027,					/* 1b */
	0x0036,					/* 1c */
	0x0000,					/* 1d */
	0x0000,					/* 1e */
	0x0004,					/* 1f */
	0x0000,					/* 20 */
	0x000a,					/* 21 */
	0x0000,					/* 22 */
	0x0005,					/* 23 */
	0x0000,					/* 24 */
	0x0004,					/* 25 */
	0x0000,					/* 26 */

	0x0015,					/* 27 : Device Size = 2^N Bytes */
	0x0002, 0x0000,				/* 28 - 29 */
	0x0000, 0x0000,				/* 2a - 2b */
	0x0004,					/* 2c */
	0x0000, 0x0000, 0x0040, 0x0000,		/* 2d - 30 */
	0x0001, 0x0000, 0x0020, 0x0000,		/* 31 - 34 */
	0x0000, 0x0000, 0x0080, 0x0000,		/* 35 - 38 */
	0x001e, 0x0000, 0x0000, 0x0001,		/* 39 - 3c */
	0x0000, 0x0000, 0x0000,			/* 3d - 3f */

	0x0050, 0x0052, 0x0049,			/* 40 - 42 : PRI */
	0x0031,					/* 43 */
	0x0030,					/* 44 */
	0x0000,					/* 45 */
	0x0002,					/* 46 */
	0x0001,					/* 47 */
	0x0001,					/* 48 */
	0x0004,					/* 49 */
	0x0000,					/* 4a */
	0x0000,					/* 4b */
	0x0000,					/* 4c */
};


static void flash_am29lv160_bautoselect(struct flash_am29_io *io, uint32_t offset, uint8_t *data)
{
	switch (offset & 0xff) {
		case 0: *data = 0x1; break;
		case 2: *data = (strncmp(io->type, "Am29LV160T", 10) == 0 ? 0xc4 : 0x49); break;
//		case 4: *data = 0x1; break; /* write-protected */

		default: *data = 0x0; break;
	}
}


static void flash_am29lv160_wautoselect(struct flash_am29_io *io, uint32_t offset, uint16_t *data)
{
	switch (offset & 0xff) {
		case 0: *data = 0x1; break;
		case 1: *data = (strncmp(io->type, "Am29LV160T", 10) == 0 ? 0x22c4 : 0x2249); break;
//		case 2: *data = 0x1; break; /* write-protected */

		default: *data = 0x0; break;
	}
}


static void flash_am29lv800_bautoselect(struct flash_am29_io *io, uint32_t offset, uint8_t *data)
{
	switch (offset & 0xff) {
		case 0: *data = 0x1; break;
		case 2: *data = (strncmp(io->type, "Am29LV800T", 10) == 0 ? 0xda : 0x5b); break;
//		case 4: *data = 0x1; break; /* write-protected */

		default: *data = 0x0; break;
	}
}


static void flash_am29lv800_wautoselect(struct flash_am29_io *io, uint32_t offset, uint16_t *data)
{
	switch (offset & 0xff) {
		case 0: *data = 0x1; break;
		case 1: *data = (strncmp(io->type, "Am29LV800T", 10) == 0 ? 0x22da : 0x225b); break;
//		case 2: *data = 0x1; break; /* write-protected */

		default: *data = 0x0; break;
	}
}


#if 0
static int flash_am29lv160_setup(struct device_desc *dev)
{
	int ret = -1;

	if (strlen(dev->type) >= 12) {
		if (strcmp(&dev->type[strlen(dev->type) - 2], "-8") == 0)
			ret = flash_am29_setup(dev, am29lv160_query, 0x4d,
					       flash_am29lv160_bautoselect, NULL,
					       0, 0);
		else if (strcmp(&dev->type[strlen(dev->type) - 3], "-16") == 0)
			ret = flash_am29_setup(dev, am29lv160_query, 0x4d,
					       NULL, flash_am29lv160_wautoselect,
					       0, 0);
	}

	if (ret == 0)
		memcpy(&((struct flash_am29_io*)(dev->data))->type[0], &dev->type[0], MAX_STR_NAME);

	DEBUG("Setup %s, ret:%d\n", dev->type, ret);

	return ret;
}


static int flash_am29lv800_setup(struct device_desc *dev)
{
	int ret = -1;

	if (strlen(dev->type) >= 12) {
		if (strcmp(&dev->type[strlen(dev->type) - 2], "-8") == 0)
			ret = flash_am29_setup(dev, NULL, 0,
					       flash_am29lv800_bautoselect, NULL,
					       0x100000, 0x10000);
		else if (strcmp(&dev->type[strlen(dev->type) - 3], "-16") == 0)
			ret = flash_am29_setup(dev, NULL, 0,
					       NULL, flash_am29lv800_wautoselect,
					       0x100000, 0x10000);
	}

	if (ret == 0)
		memcpy(&((struct flash_am29_io*)(dev->data))->type[0], &dev->type[0], MAX_STR_NAME);

	DEBUG("Setup %s, ret:%d\n", dev->type, ret);

	return ret;
}
#endif

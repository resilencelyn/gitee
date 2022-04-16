/* Copyright (C)
 * This progflash_s29gl01gs is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This progflash_s29gl01gs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this progflash_s29gl01gs; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 *
 */

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_attribute.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_dev_intf.h>
#include <skyeye_uart_ops.h>
#include <skyeye_arch.h>
#include <skyeye_swapendian.h>
#define DEBUG
#include <skyeye_log.h>

#include "flash_s29gl01gs.h"

//#define FLASH_DBG

#ifndef FLASH_DBG
# define AMD_DBG(x...)
#else
# define AMD_DBG(x...) printf(x)
#endif

static exception_t flash_s29gl01gs_read(conf_object_t *opaque, generic_address_t addr, void* buf, size_t count)
{
	flash_s29gl01gs_device_t* dev = opaque->obj;
	uint32_t *data = (uint32_t*)buf;

	if (dev->is_status_read) {
		if (++dev->status_cycles == 2) {
			AMD_DBG("\t[READ STATUS REG]: ADDR: 0x%08x\t DATA: 0x%08x\n\n", addr, *data);
			dev->status_cycles = 0;
			dev->is_status_read = 0;
		}
		return No_exp;
	}
	if (dev->is_cfi) {
		*data = cfi[addr];
		AMD_DBG("\t[CFI READ]: ADDR: 0x%08x\t DATA: 0x%08x\n\n", addr, *data);
		return No_exp;
	}
	if (dev->is_id_entry) {
		*data = cfi[addr];
		AMD_DBG("\t[ID READ]: ADDR: 0x%08x\t DATA: 0x%08x\n\n", addr, *data);
		return No_exp;
	}
	if (dev->image_space) {
		dev->image_space->read(dev->image, addr, buf, count);
	}
	AMD_DBG("\t[NORMAL READ]: ADDR: 0x%08x\t DATA: 0x%08x\tCOUNT: %ld\n\n", addr, *data, count);
	return No_exp;
}

static exception_t flash_s29gl01gs_write(conf_object_t *opaque, generic_address_t addr, void* buf, size_t count)
{

	flash_s29gl01gs_device_t *dev = opaque->obj;
	uint32_t data = *(uint32_t*)buf;

	if (dev->is_write2buf) {
		dev->cycles++;
		if (dev->cycles == 4) {
			dev->write_buf_wc = data + 1;
			AMD_DBG(" %03x %02x | WRITE2BUF\n\n", addr, data);
			return No_exp;
		}

		if (dev->write_buf_wc > 0) {
			if (dev->image_space) {
				dev->image_space->write(dev->image, addr, &data, count);
			}

			AMD_DBG("\t[BUFF WRITE]: ADDR: 0x%08x\t DATA: 0x%08x\tREMAIN-WC: 0x%04x\tCOUNT: %ld\n\n", addr, data, dev->write_buf_wc, count);

			dev->write_buf_wc--;
		}
		if (dev->write_buf_wc == 0) {
			RESET_FLAGS(dev);
		}
		return No_exp;
	}
	if (dev->is_word_program) {
		dev->cycles++;
		AMD_DBG(" %08x %08x", addr, data);
		if (dev->cycles == 4) {
			if (dev->image_space) {
				dev->image_space->write(dev->image, addr, &data, count);
			}
			RESET_FLAGS(dev);

			AMD_DBG(" | WORD_PROGRAM\n\n");
		}
		return No_exp;
	}

	if (dev->is_erase) {
		dev->cycles++;
		AMD_DBG(" %08x %02x", addr, data);
		if (dev->cycles == 6) {
			if (data == 0x10) {
				AMD_DBG(" | ERASE_CHIP\n\n");
			} else if (data == 0x30) {
				uint32_t sa = addr;
				uint64_t val = 0xffffffffffffffff;

				while (sa < addr + SECTOR_SIZE) {
					if (dev->image_space) {
						dev->image_space->write(dev->image, sa, &val, sizeof(val));
					}
					sa += sizeof(val);
				}
				AMD_DBG(" | ERASE_SA\n\n");
			}

			RESET_FLAGS(dev);
		}
		return No_exp;
	}
	if (IS_UNLOCK_1(addr, data)) {
		RESET_FLAGS(dev);
		dev->cycles = 1;

		AMD_DBG("CMD: %03x %02x", addr, data);
		return No_exp;
	}
	if (IS_UNLOCK_2(addr, data)) {
		dev->cycles = 2;

		AMD_DBG(" %03x %02x", addr, data);
		return No_exp;
	}
	if (ASO_EXIT(addr, data)) {
		RESET_FLAGS(dev);

		AMD_DBG("CMD: %03x %02x | ASO_EXIT\n\n", addr, data);
		return No_exp;
	}
	if (STATUS_REG_READ(addr, data)) {
		RESET_FLAGS(dev);

		dev->status_cycles = 1;
		dev->is_status_read = 1;

		AMD_DBG("CMD: %03x %02x | STATUS_REG_READ\n");
		return No_exp;
	}
	if (STATUS_REG_CLEAR(addr, data)) {
		AMD_DBG("CMD: %03x %02x | STATUS_REG_CLEAR\n");

		RESET_FLAGS(dev);

		return No_exp;
	}
	if (PROGRAM_BUF2FLASH(addr, data)) {
		RESET_FLAGS(dev);

		AMD_DBG("CMD: %03x %02x | BUFF2FLASH\n\n", addr, data);
		return No_exp;
	}
	if (CFI_ENTRY(addr, data)) {
		RESET_FLAGS(dev);
		dev->is_cfi = 1;

		AMD_DBG("CMD: %03x %02x | CFI_ENTRY\n\n", addr, data);
		return No_exp;
	}
	if (dev->cycles == 2) {
		dev->cycles = 3;

		if (WORD_PROGRAM(addr, data)) {
			dev->is_word_program = 1;

			AMD_DBG(" %03x %02x", addr, data);
			return No_exp;
		}
		if (WRITE2BUF(addr, data)) {
			dev->is_write2buf = 1;

			AMD_DBG(" %03x %02x", addr, data);
			return No_exp;
		}
		if (WRITE2BUF_ABORT(addr, data)) {
			AMD_DBG(" %03x %02x", addr, data);
			return No_exp;
		}
		if (ERASE(addr, data)) {
			dev->is_erase = 1;

			AMD_DBG(" %03x %02x", addr, data);
			return No_exp;
		}
		if (ID_ENTRY(addr, data)) {
			dev->is_id_entry = 1;

			AMD_DBG(" %03x %02x | ID_ENTRY\n\n", addr, data);
			return No_exp;
		}
		if (SSR_ENTRY(addr, data)) {
			AMD_DBG(" %03x %02x | SSR ENTRY [NO IMPLEMENT]\n\n", addr, data);
			return No_exp;
		}
		if (LOCK_REGISTER_ENTRY(addr, data)) {
			AMD_DBG(" %03x %02x | LOCK REGISTER ENTRY [NO IMPLEMENT]\n\n", addr, data);
			return No_exp;
		}
		if (LOCK_PASSWORD_ENTRY(addr, data)) {
			AMD_DBG(" %03x %02x | LOCK PASSWORD ENTRY [NO IMPLEMENT]\n\n", addr, data);
			return No_exp;
		}
		if (LOCK_PPB_ENTRY(addr, data)) {
			AMD_DBG(" %03x %02x | LOCK PPB_ENTRY [NO IMPLEMENT]\n\n", addr, data);
			return No_exp;
		}
		if (LOCK_PPBLOCK_ENTRY(addr, data)) {
			AMD_DBG(" %03x %02x | LOCK PPBLOCK_ENTRY [NO IMPLEMENT]\n\n", addr, data);
			return No_exp;
		}
		if (LOCK_DYB_ENTRY(addr, data)) {
			AMD_DBG(" %03x %02x | LOCK DYB_ENTRY [NO IMPLEMENT]\n\n", addr, data);
			return No_exp;
		}
	}
	AMD_DBG("[--WRITE WARING--]: ADDR: 0x%08x\t DATA: 0x%08x\n\n", addr, data);
	return No_exp;
}

static conf_object_t* new_flash_s29gl01gs(char* obj_name)
{
	flash_s29gl01gs_device_t* dev= skyeye_mm_zero(sizeof(flash_s29gl01gs_device_t));
	dev->obj = new_conf_object(obj_name, dev);

	return dev->obj;
}

static void free_flash_s29gl01gs(conf_object_t* conf_obj){
	flash_s29gl01gs_device_t* dev = conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev);
}

static exception_t reset_flash_s29gl01gs(conf_object_t* opaque, const char* args)
{
	flash_s29gl01gs_device_t *dev = opaque->obj;

	return No_exp;
}

void flash_s29gl01gs_register_attribute(conf_class_t* clss){
}

static void dbg_read(char *buf, uint32_t offset, uint32_t size)
{
	uint32_t off, i;
	uint32_t *val;

	off = offset;
	val = (uint32_t *)buf;

	for (i = 0; i < size / 4; i++) {
		printf("ADDR: 0x%08x\t%08x\n", off, val[i]);
		off += 4;
	}
}
static int load_image(flash_s29gl01gs_device_t *dev)
{
	char *filename = "/home/digiproto/work/product_testcase/arm/omapl138_wuhan_vxworks/amd_s29_flash_ram0.20160716141605";
	FILE *pfile;
	char buf[8 * 1024];
	uint32_t offset, ret;


	if (dev->image_space == NULL) {
		printf("[%s:%s:%d]: Can't get image_space.\n",
				__FILE__, __FUNCTION__, __LINE__);
		return -1;
	}

	if ((pfile = fopen(filename, "r")) == NULL) {
		printf("[%s:%s:%d]: fopen faild.\n", __FILE__, __FUNCTION__, __LINE__);
		return -2;
	}

	offset = 0;

	while ((ret = fread(buf, 1, sizeof(buf), pfile))) {
		dev->image_space->write(dev->image, offset, buf, ret);
		//dbg_read(buf, offset, ret);
		offset += ret;
	}

	if (!feof(pfile)) {
		printf("[%s:%s:%d]: fread faild.\n", __FILE__, __FUNCTION__, __LINE__);
		fclose(pfile);
		return -3;
	}

	fclose(pfile);
	return 0;
}
static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	flash_s29gl01gs_device_t* dev = (flash_s29gl01gs_device_t*)obj->obj;
	dev->image = obj2;
	dev->image_space = (memory_space_intf*)SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);
	if (dev->image_space == NULL){
		if (dev->image == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", MEMORY_SPACE_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", MEMORY_SPACE_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

#if 0
	dev->image_info = (skyeye_image_info *)SKY_get_iface(dev->image, SKYEYE_IMAGE_INFO);
	if (dev->image_info == NULL) {
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.",
				SKYEYE_IMAGE_INFO, obj2->objname);
	}


	/* here we muset set preload image size, because attribute<size> of image inited after iface */
	if (dev->image_info) {
		/* image size is got from json configure file */
		dev->image_info->set_image_size(dev->image, 0x2000000u);
	}
	load_image(dev);
#endif
	return No_exp;
}

static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	flash_s29gl01gs_device_t* dev = (flash_s29gl01gs_device_t*)obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}
void init_flash_s29gl01gs(){
	static skyeye_class_t class_data = {
		.class_name = "flash_s29gl01gs",
		.class_desc = "flash_s29gl01gs device",
		.new_instance = new_flash_s29gl01gs,
		.free_instance = free_flash_s29gl01gs,
		.reset_instance = reset_flash_s29gl01gs,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = flash_s29gl01gs_read,
		.write = flash_s29gl01gs_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);	

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = MEMORY_SPACE_INTF_NAME,
			.set = memory_set,
			.get = memory_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	flash_s29gl01gs_register_attribute(clss);
}

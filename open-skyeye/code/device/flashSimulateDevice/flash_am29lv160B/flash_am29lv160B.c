#include <skyeye_lock.h>
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
#include <skyeye_core.h>
#include <skyeye_signal.h>
#include <skyeye_arch.h>
#include <skyeye_swapendian.h>
#define DEBUG
#include <skyeye_log.h>
#include "flash_am29lv160B.h"
static exception_t flash_am29lv160B_read(conf_object_t *opaque, generic_address_t addr, void *buf, size_t count)
{
	flash_am29lv160B_device *dev = opaque->obj;
	exception_t ret;

	if (dev->image_space)
		ret = dev->image_space->read(dev->image, addr, buf, count);
	/* printf("In  %s, addr :%x, buf:%x\n", __func__, addr, *(uint16_t*)buf); */
	fflush(stdout);

	if (ret != No_exp) {
		printf("In %s, %s Not_found addr 0x%x\n", __func__, dev->obj->class_name, addr);
		return Not_found_exp;
	}
	return No_exp;
}
static exception_t flash_am29lv160B_write(conf_object_t *opaque, generic_address_t addr, void *buf, size_t count)
{
	flash_am29lv160B_device *dev = opaque->obj;
	uint16_t data = *(uint16_t *)buf;
	uint32_t start, end = 0;
	exception_t ret;

	if(dev->endian == Big_endian)
		data = half_from_BE(*(uint16_t*)buf);
	else
		data = *(uint16_t*)buf;
	/* printf("In %s, addr :%x, buf:%x\n", __func__, addr, *(uint16_t*)buf); */
	if (CMD_WORD_PROGRAM(dev)) {
		if (dev->image_space)
			ret = dev->image_space->write(dev->image, addr, buf, count);
		if (ret != No_exp)
			printf("write error data :%x at addr :%x\n", addr, data);
		goto reset;
	}
	if (CMD_ERASE(dev)) {
		switch (data) {
			case 0x10:
				start = 0;
				end = start + CHIP_SIZE;
				break;
			case 0x30:
				start = addr;
				end = start + SECTOR_SIZE;
				break;
			default:
				start = end = 0x0;
				break;
		}
		if (end > start && end <= CHIP_SIZE) {
			for (addr = start; addr < end; addr += count) {
				if (dev->image_space)
					ret = dev->image_space->write(dev->image, addr, &dev->erase_val, count);
				if (ret != No_exp)
					printf("write error data :%x at addr :%x\n", addr, data);
			}
			DEBUG("*** Erase(start:0x%08x, end:0x%08x)\n", start, end);
		} else {
			printf("*** ERROR: Erase(start:0x%08x, end:0x%08x)\n", start, end);
		}

		goto reset;
	}
	if (dev->n_regs < 6) {
		dev->regs[dev->n_regs].addr = addr;
		dev->regs[dev->n_regs].data = data;
		dev->n_regs += 1;
		dev->cnt = dev->n_regs;
	}
	goto exit;
reset:
	dev->cnt = dev->n_regs = 0;
	memset(&dev->regs[0], 0, sizeof(dev->regs[0]) * 6);
exit:
	return No_exp;
}

static char* flash_am29lv160B_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return 0;
}

static exception_t flash_am29lv160B_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	flash_am29lv160B_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t flash_am29lv160B_get_regnum(conf_object_t *conf_obj)
{
	return 0;
}

static uint32_t flash_am29lv160B_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	flash_am29lv160B_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t flash_am29lv160B_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	return 0;
}

static conf_object_t* new_flash_am29lv160B(const char *obj_name)
{
	flash_am29lv160B_device* dev = skyeye_mm_zero(sizeof(flash_am29lv160B_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->erase_val = 0xffff;
	struct registers *regs = &(dev->regs);
	return dev->obj;
}

static exception_t free_flash_am29lv160B(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	flash_am29lv160B_device* dev = obj->obj;
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
	return No_exp;
}
 
static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	flash_am29lv160B_device* dev = obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}

void init_flash_am29lv160B(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "flash_am29lv160B",
		.class_desc = "flash_am29lv160B",
		.new_instance = new_flash_am29lv160B,
		.free_instance = free_flash_am29lv160B,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = flash_am29lv160B_read,
		.write = flash_am29lv160B_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = flash_am29lv160B_get_regvalue_by_id,
		.get_regname_by_id = flash_am29lv160B_get_regname_by_id,
		.set_regvalue_by_id = flash_am29lv160B_set_regvalue_by_id,
		.get_regnum = flash_am29lv160B_get_regnum,
		.get_regid_by_name = flash_am29lv160B_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
	static const struct InterfaceDescription ifaces[] = 
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription)
		{
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf
		},
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription ){
			.name = MEMORY_SPACE_INTF_NAME,
			.set = memory_set,
			.get = memory_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

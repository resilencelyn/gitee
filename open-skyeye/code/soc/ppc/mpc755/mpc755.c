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
#include <skyeye_system.h>
#define DEBUG
#include <skyeye_log.h>
#include "mpc755.h"

int update_boot_info(conf_object_t *obj)
{
	int i;
	mpc755_device *dev = obj->obj;
	sys_soc_t *soc = obj->sys_struct;
	for (i = 0; i < soc->cpu_cnt; i++) {
		sys_cpu_t *cpu = soc->cpus + i;
	}
	return 0;
}

static uint32_t
io_read_byte (void *state, uint32_t offset)
{
	return Not_found_exp;
}

static exception_t
io_write_byte (void *state, uint32_t offset, uint32_t data)
{
	return Not_found_exp;
}

static uint32_t
io_read_halfword (void *state, uint32_t offset)
{
	return Not_found_exp;
}

static exception_t
io_write_halfword (void *state, uint32_t offset, uint32_t data)
{
	return Not_found_exp;
}

static uint32_t
io_read_word (void *state, uint32_t offset)
{
	return Not_found_exp;
}

static exception_t
io_write_word (void *state, uint32_t offset, uint32_t data)
{
	return Not_found_exp;
}

static conf_object_t* new_mpc755(char *obj_name)
{
	mpc755_device* dev = skyeye_mm_zero(sizeof(mpc755_device));
	dev->obj = new_conf_object(obj_name, dev);

	skyeye_config_t *config = get_current_config();
	machine_config_t* mach_config = skyeye_mm(sizeof(machine_config_t));

	config->mach                         = mach_config;
	config->mach->mach_io_read_byte      = io_read_byte;
	config->mach->mach_io_write_byte     = io_write_byte;
	config->mach->mach_io_read_halfword  = io_read_halfword;
	config->mach->mach_io_write_halfword = io_write_halfword;
	config->mach->mach_io_read_word      = io_read_word;
	config->mach->mach_io_write_word     = io_write_word;

	return dev->obj;
}

static exception_t free_mpc755(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

int set_space(conf_object_t *obj, addr_space_t *space)
{
	return 0;
}

addr_space_t *get_space(conf_object_t *obj)
{
	return  (addr_space_t *)NULL;
}

int set_core(conf_object_t *obj, conf_object_t *core)
{
	return 0;
}

conf_object_t *get_core_by_id(conf_object_t *obj, int id){
	return NULL;
}


void init_mpc755(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "mpc755",
		.class_desc = "mpc755",
		.new_instance = new_mpc755,
		.free_instance = free_mpc755
	};
	conf_class_t* clss = SKY_register_soc_class(class_data.class_name, &class_data);

	static const skyeye_machine_intf inface = {
		.update_boot_info = update_boot_info,
		.set_space = set_space,
		.set_core = set_core,
		.get_core_by_id = get_core_by_id,
		.get_space = get_space,

	};
	SKY_register_iface(clss, MACHINE_INTF_NAME, &inface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MACHINE_INTF_NAME,
			.iface = &inface,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, CONNECTS_NULL);
}

/* Copyright (C) 
* 2015 - Michael.Kang blackfin.kang@gmail.com
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
* @file x86_core.c
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2015-01-15
*/

#include <skyeye_types.h>
#include <skyeye_class.h>
#include <skyeye_core_intf.h>
#include <skyeye_int_register.h>
#include <skyeye_misc.h>
#include <sim_control.h>
#include <memory_space.h>
#include <skyeye_attribute.h>
#include <skyeye_checkpoint.h>
#include <skyeye_mm.h>
#include <skyeye_iface.h>
#include "x86_core.h"
#include "x86_arch_interface.h"
extern "C"{
#include <skyeye_x86devs_fin.h>
#include <skyeye_x86devs_intf.h>
#include "check_devices.h"
}

/* 
 *
 * ===  FUNCTION  ======================================================================
 *         Name:  x86_set_pc
 *  Description:  This function sets the Program Counter
 * =====================================================================================
 */
void x86_core_set_pc(conf_object_t* conf_obj, generic_address_t addr)
{
	//x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	//x86_set_pc(addr);
	return;
}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  x86_get_pc
 *  Description:  This function returns the Program Counter
 * =====================================================================================
 */
generic_address_t x86_core_get_pc(conf_object_t* conf_obj)
{
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return x86_get_pc();
}

exception_t x86_memory_write(conf_object_t* obj, generic_address_t addr, void* buf, size_t count){  
	BX_MEM(0)->writePhysicalPage(NULL, addr, count, buf);
	return No_exp;
}
exception_t x86_memory_read(conf_object_t* obj, generic_address_t addr, void* buf, size_t count){
	BX_MEM(0)->readPhysicalPage(NULL, addr, count, buf);
	return No_exp;
}

static char* x86_core_disassemble(conf_object_t* obj, generic_address_t addr){
	return x86_disassemble(addr);
}
static conf_object_t* new_x86_core(const char* obj_name) {
	x86_core_t *core =  (x86_core_t *)skyeye_mm_zero(sizeof(x86_core_t));
	conf_object_t* obj = new_conf_object(obj_name, core);
	core->obj = obj;

	x86_init_state();
	core->steps = core->stop_step = 0;
	return core->obj;
}

exception_t free_x86_core(conf_object_t* obj) {
	return No_exp;
}

exception_t reset_x86_core(conf_object_t* conf_obj, const char* parameter) {
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return No_exp;
}

static char* x86_get_architecture(conf_object_t* obj) {
    return "x86";
}

static void x86_stepi_check(conf_object_t *conf_obj)
{
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	if(x86_state->stop_step == x86_state->steps && x86_state->stop_step != 0)
	{
		SIM_stop(NULL);
		x86_state->stop_step = 0;

	}
	return ;
}
static void x86_breakpoint_check(conf_object_t *conf_obj){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	uint32_t pc = x86_get_pc();
	check_breakpoint(&(x86_state->bp_manager), pc, x86_state->steps);
	return;
}

static void x86_core_run (conf_object_t* conf_obj){
	x86_step_once();
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	x86_state->steps ++;
	x86_stepi_check(conf_obj);
	x86_breakpoint_check(conf_obj);

	return;
}

static void x86_stepi(conf_object_t *conf_obj, uint64_t steps){

	x86_core_t* core = (x86_core_t *)conf_obj->obj;
	core->stop_step = core->steps + steps;
	return ;
}

uint64_t x86_core_get_steps(conf_object_t* conf_obj){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
        return x86_state->steps;
}
uint32 x86_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return x86_get_regval_by_id(id);
}

uint32 x86_get_regnum(conf_object_t* conf_obj){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return 15;
}
char *x86_get_regname_by_id(conf_object_t* conf_obj, uint32_t id){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return x86_get_reg_name_by_id(id);
}

exception_t x86_set_regvalue_by_id(conf_object_t* conf_obj,uint32 value, uint32 id){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return x86_set_register_by_id(id, value);
}

uint32 x86_get_regid_by_name(conf_object_t* conf_obj, char* name)
{
	return x86_get_reg_id_by_name(name);
}

static int x86_insert_bp(conf_object_t* conf_obj, generic_address_t addr){
	x86_core_t *core = (x86_core_t*)conf_obj->obj;
	int ret;
	ret = create_a_breakpoint(&core->bp_manager, addr);
	if(ret != 0)
		return ret;
	return 0;
}

static int x86_delete_bp(conf_object_t* conf_obj, generic_address_t addr){
	x86_core_t *core = (x86_core_t*)conf_obj->obj;
	int ret;
	ret = remove_bp_by_addr(&core->bp_manager, addr);
	if(ret != 0)
		return ret;
	return 0;
}

static  uint32_t x86_get_bp_numbers(conf_object_t *conf_obj){
	x86_core_t *core = (x86_core_t*)conf_obj->obj;
	return get_bp_numbers(&core->bp_manager);
}

static uint32_t x86_get_bp_addr_by_id(conf_object_t *conf_obj, int id){
	x86_core_t *core = (x86_core_t*)conf_obj->obj;
	breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);
	if(bp)
		return bp->address;
	return 0;
}

static int x86_get_trigger(conf_object_t *conf_obj)
{
	x86_core_t *core = (x86_core_t*)conf_obj->obj;
	return get_breakpoint_hit_id(&core->bp_manager);
}

static bool_t x86_check_bp_trigger(conf_object_t *conf_obj)
{
	x86_core_t *core = (x86_core_t*)conf_obj->obj;
	int ret;
	ret = check_breakpoint_hit_flag(&core->bp_manager);
	if(ret == 0)
		return True;
	return False;
}

static int x86_clear_bp_trigger(conf_object_t *conf_obj)
{
	x86_core_t *core = (x86_core_t*)conf_obj->obj;
	clear_breakpoint_hit_flag(&core->bp_manager);
	return 0;
}

char * get_devices_reg_name(conf_object_t *conf_obj, int did, int rid){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return x86_devs_get_reg_name_by_id(did, rid);
}

unsigned int get_devices_reg_num(conf_object_t *conf_obj, int did){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return x86_devs_get_reg_num(did);
}

unsigned int get_devices_reg_width(conf_object_t *conf_obj, int did){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return x86_devs_get_reg_width(did);
}

unsigned int get_devices_reg_val(conf_object_t *conf_obj, int did, int rid){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return x86_devs_get_reg_value_by_id(did, rid);
}

unsigned int get_devices_reg_large_val(conf_object_t *conf_obj, int did, int rid, int offset){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return x86_devs_get_reg_large_value_by_id(did, rid, offset);
}

unsigned int get_devices_reg_addr(conf_object_t *conf_obj, int did, int rid){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return x86_devs_get_reg_addr(did,rid);
}


char *get_devices_name(conf_object_t *conf_obj, int did){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return x86_devs_get_devs_name(did);
}

unsigned int get_devices_num(conf_object_t *conf_obj){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return x86_devs_get_devs_num();
}

void devices_save_fin(conf_object_t *conf_obj, int did, unsigned int addr, int bitnum, int mode){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	x86_devs_save_fin(did, addr, bitnum, mode);
	return;
}

fi_q_t *devices_get_fin(conf_object_t *conf_obj, int did){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	return x86_devs_get_fin(did);
}

void devices_clear_fin(conf_object_t *conf_obj, int did, unsigned addr, int bitnum, int mode){
	x86_core_t *x86_state = (x86_core_t*)conf_obj->obj;
	x86_devs_clear_fin(did, addr, bitnum, mode);
	return;
}

static exception_t cp_file_to_path(const char* filename, const char* path){
	char buf[BUFSIZ];
	memcpy(&buf[0], path, strlen(path));
	buf[strlen(path)] = '/';
	memcpy(&buf[strlen(path) + 1] , filename, strlen(filename));
	buf[strlen(path) + strlen(filename) + 1] = '\0';
	return file_copy(filename, &buf[0]);
}

static exception_t x86_save_configure(conf_object_t *obj, char *saved_path)
{
	x86_core_t *core = (x86_core_t*)obj->obj;
	exception_t ret = No_exp;
	//printf("%s:%d, %s\n", __func__, __LINE__, saved_path);
	if (SIM->save_state(saved_path)) { /* successfully */
		extern char* bochsrc_filename;
		//printf("copy %s\n", bochsrc_filename);
		if((ret = cp_file_to_path(bochsrc_filename, saved_path)) != No_exp)
			goto failed;
		//printf("copy %s\n", SIM->get_param_string(BXPN_ROM_PATH)->getptr());
		if((ret = cp_file_to_path(SIM->get_param_string(BXPN_ROM_PATH)->getptr(), saved_path)) != No_exp)
			goto failed;
		//printf("copy %s\n", SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr());
		if((ret = cp_file_to_path(SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr(), saved_path)) != No_exp)
			goto failed;
		//printf("copy %s", SIM->get_param_string(BXPN_FLOPPYA_PATH)->getptr());
		if((ret = cp_file_to_path(SIM->get_param_string(BXPN_FLOPPYA_PATH)->getptr(), saved_path)) != No_exp)
			goto failed;
		int channel;
		for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
			for (Bit8u device=0; device<2; device ++) {
  				char  ata_name[20];
  				bx_list_c *base;
				sprintf(ata_name, "ata.%d.%s", channel, (device==0)?"master":"slave");
				base = (bx_list_c*) SIM->get_param(ata_name);
      				if (SIM->get_param_bool("present", base)->get()){
					//printf("ata image:%s\n", SIM->get_param_string("path", base)->getptr());
					if((ret = cp_file_to_path(SIM->get_param_string("path", base)->getptr(), saved_path)) != No_exp)
						goto failed;
				}
			}
		}
		//printf("checkpoint to %s succ!\n", saved_path);
		skyeye_log(Info_log, __FUNCTION__, "checkpoint to %s succ!\n", saved_path);
	}
	else{
		ret = Unknown_exp;
	}
failed:
	return ret;
}

static exception_t x86_load_configure(conf_object_t *obj, char *load_path)
{
	x86_core_t *core = (x86_core_t*)obj->obj;
	//printf("%s:%d, %s\n", __func__, __LINE__, load_path);
	exception_t ret = No_exp;
	SIM->get_param_string(BXPN_RESTORE_PATH)->set(load_path);
	if (SIM->restore_hardware()) {
		void bx_sr_after_restore_state(void);
		bx_sr_after_restore_state();
	} else {
		ret = Unknown_exp;
		//printf(("cannot restore hardware state from %s\n", load_path));
		skyeye_log(Warning_log, __FUNCTION__, "cannot restore hardware state from %s\n", load_path);
	}
	return ret;
}

static void x86_core_reset(conf_object_t* obj){
}

void x86_core_register_interface(conf_class_t* clss) {
	static const core_info_intf core_info = {
		NULL,
		set_pc : x86_core_set_pc,
		get_pc : x86_core_get_pc,
		get_steps : x86_core_get_steps,
		get_endian : NULL,
		get_alignment : NULL,
		get_architecture : x86_get_architecture,
		reset : x86_core_reset,
		disassemble : x86_core_disassemble,
	};
	SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

	static const skyeye_reg_intf reg = {
		NULL,
		get_regvalue_by_id : x86_get_regvalue_by_id,
		get_regname_by_id : x86_get_regname_by_id,
		set_regvalue_by_id : x86_set_regvalue_by_id,
		get_regid_by_name : x86_get_regid_by_name,
		get_regnum : x86_get_regnum,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg);

	static const core_exec_intf core_exec = {
		NULL,
		run : x86_core_run,
		stop : NULL,
		stepi : x86_stepi,
	};
	SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

	static const memory_space_intf memory_space = {
		NULL,
		read : x86_memory_read,
		write : x86_memory_write,
		get_page : NULL,
	};

	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

	static const core_signal_intf core_signal = {
		NULL,
		signal : NULL,
	};
	SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

	static const x86_checkpoint_intf checkpoint= {
		save_configure : x86_save_configure,
		load_configure : x86_load_configure,
	};
	SKY_register_iface(clss, X86_CHECKPOINT_INTF_NAME, &checkpoint);

	static const core_breakpoint_op_intf bp_op = {
		insert_bp : x86_insert_bp,
		delete_bp : x86_delete_bp,
		get_bp_numbers : x86_get_bp_numbers,
		get_bp_addr_by_id : x86_get_bp_addr_by_id,
		get_bp_trigger : x86_get_trigger,
		check_bp_trigger : x86_check_bp_trigger,
		clear_bp_trigger : x86_clear_bp_trigger,
	};
	SKY_register_iface(clss, CORE_BREAKPOINT_OP_INTF_NAME, &bp_op);

	static const core_devices_list devices_list ={
		NULL,
		get_devs_reg_name : get_devices_reg_name,
		get_devs_reg_num : get_devices_reg_num,
		get_devs_reg_width : get_devices_reg_width,
		get_devs_reg_val : get_devices_reg_val,
		get_devs_reg_large_val : get_devices_reg_large_val,
		get_devs_name : get_devices_name,
		get_devs_num : get_devices_num,
		get_devs_reg_addr : get_devices_reg_addr
		};
	SKY_register_iface(clss, DEVICE_LIST_INTF_NAME, &devices_list);

	static const core_devices_fault_in devices_fault_in ={
		NULL,
		devs_save_fin : devices_save_fin,
		devs_get_fin : devices_get_fin,
		devs_clear_fin : devices_clear_fin
		};
	SKY_register_iface(clss, DEVICE_FIN_INTF_NAME, &devices_fault_in);
}

void x86_core_register_attribute(conf_class_t* clss) {

	return;
}

void init_x86_core() {
	static skyeye_class_t class_data = {
		class_name : "x86",
		class_desc : "x86 core",
		new_instance : new_x86_core,
		free_instance : free_x86_core,
		reset_instance : reset_x86_core,
		get_attr : NULL,
		set_attr : NULL
	};
	conf_class_t* clss = SKY_register_cpu_class(class_data.class_name, &class_data);
	x86_core_register_interface(clss);
	x86_core_register_attribute(clss);
}

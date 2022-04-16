/* Copyright (C) 
* 2013 - Michael.Kang blackfin.kang@gmail.com
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
* @file c54x_core.c
* @brief The core of c54x or c54x
* @author jiachao 
* @version 7849
* @date 2013-09-03
*/

#include "skyeye_internal.h"
#include "skyeye_types.h"
#include "skyeye_config.h"
#include "skyeye_options.h"
#include <skyeye_exec.h>
#include <skyeye_cell.h>
#include <skyeye_arch.h>
#include <skyeye_mm.h>
#include <skyeye_obj.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_disassemble.h>
#include <skyeye_callback.h>
#include <skyeye_core_intf.h>
#include <skyeye_int_register.h>
#include <sim_control.h>
#include "c54x_cpu.h"
#include "c54x_decode.h"

#define SKYEYE_LOG 0

uint32_t exec_insn_cache(c54x_core_t* core, insn_cache_t* cache);

void c54x_set_pc (conf_object_t* obj, generic_address_t addr)
{
	c54x_core_t* core = (c54x_core_t *)obj->obj;
	core->pc = addr;
	return;
}
generic_address_t c54x_get_pc(conf_object_t* obj){
	c54x_core_t* core = (c54x_core_t *)obj->obj;
	return core->pc;
}

uint32_t c54x_get_address_width(conf_object_t *obj){
	return 1;
}

char *c54x_disassemble(conf_object_t *obj, generic_address_t addr)
{
	c54x_core_t* core = (c54x_core_t *)obj->obj;
	return skyeye_disassemble(obj, addr);
}

char *c54x_get_architecture(conf_object_t *obj){
	c54x_core_t* core = (c54x_core_t *)obj->obj;
	return "tic54x";
}

int c54x_get_cpu_exception_num(conf_object_t *obj)
{
	c54x_core_t* core = (c54x_core_t *)obj->obj;

	return C54X_IRQ_NUM;
}

void c54x_set_cpu_exception_by_id(conf_object_t *obj, int exception_id)
{
	c54x_core_t* core = (c54x_core_t *)obj->obj;

	if (exception_id < C54X_IRQ_NUM)
	{
		RW_WRLOCK(core->ifr_lock);
		core->MMR_BUFFER[CPU_IFR] |= (1 << exception_id);
		RW_UNLOCK(core->ifr_lock);
	} else
	{
		//need trigger IRQ to PIE
	}

	return ;
}

char* c54x_get_cpu_exception_name_by_id(conf_object_t *obj, int exception_id)
{
	c54x_core_t* core = (c54x_core_t *)obj->obj;
	strcpy(core->exception_name, "IRQ");

	assert(exception_id < C54X_IRQ_NUM);
	sprintf(core->exception_name, "%s%d", core->exception_name, exception_id);
	return core->exception_name;
}

void register_pairs_save(c54x_core_t* core){
	int sp = core->MMR_BUFFER[CPU_SP];
	sp -= 1;
	WRITE_MMR(core, CPU_SP, sp);
	c54x_memory_write(core, sp, core->pc);

	sp -= 1;
	WRITE_MMR(core, CPU_SP, sp);
	c54x_memory_write(core, sp, core->MMR_BUFFER[CPU_XPC]);
	
	return;
}
static void check_intr(c54x_core_t* core) {
	if (core->intr_flag)
		return;
	if (core->MMR_BUFFER[CPU_IFR] != 0) {
		int i = 0;
		int intm = (core->MMR_BUFFER[CPU_ST1] >> INTM) & 0x1;
		int isr_addr = 0;
		while (((core->MMR_BUFFER[CPU_IFR] >> i) & 0x1) == 0)
			i++;
		if (i < 16) {
			if ((core->MMR_BUFFER[CPU_IMR] & (1 << i)) && (intm == 0)) {
				/* save the intr return address and registers to memory */
				register_pairs_save(core);

				core->MMR_BUFFER[CPU_ST1] |= (0x1 << INTM); /* disable global interrupt */ 
				isr_addr = core->MMR_BUFFER[CPU_PMST] + i * 4;
				core->pc = c54x_memory_read(core, isr_addr); 

				core->intr_flag = 1;
			}
			core->MMR_BUFFER[CPU_IFR] &= ~(0x1 << i); /* clear ifr */
		}
	}
	
}

exception_t c54x_space_read(conf_object_t* obj, generic_address_t addr, void* buf, size_t count){
	      c54x_core_t *core = (c54x_core_t*)obj->obj;
	      memory_space_intf* iface = core->memory_space_iface;
	      return iface->read(core->memory_space, addr, buf, count);
}

exception_t c54x_space_write(conf_object_t* obj, generic_address_t addr, void* buf, size_t count){
	      c54x_core_t *core = (c54x_core_t*)obj->obj;
	      memory_space_intf* iface = core->memory_space_iface;
	      return iface->write(core->memory_space, addr, buf, count);
}

void  c54x_stepi(conf_object_t* obj, uint64_t steps){
	c54x_core_t* core = (c54x_core_t *)obj->obj;
	core->stop_step = core->insn_num + steps;
	return;
}

void c54x_step_once (conf_object_t* conf_obj)
{
	c54x_core_t* core = (c54x_core_t *)conf_obj->obj;
	uint32_t addr = core->pc;
	int status;
	insn_cache_t* cache = &(core->insn_cache[HASH(addr)]);
	if(cache->addr != addr) /* miss */{
		/* do translate and replace the old one */
		uint16_t insn;
		cache->addr = addr;
		status = c54x_space_read(conf_obj, addr, &insn, 2);
		cache->insn = insn;		
		trans_opcode_fields(insn, &(cache->fields));
	}

	cache->action = get_insn_action(core, cache->insn);
	SKY_pc_hap_occurred(NULL, conf_obj, core->pc);

	exec_insn_cache(core, cache);
	int id = check_breakpoint(&(core->bp_manager), core->pc, core->insn_num);

	if(core->stop_step != 0 && core->insn_num != 0 && core->stop_step == core->insn_num){
		SIM_stop(NULL);
		core->stop_step = 0;
 	}
	if(core->pc == 0){
		skyeye_log(Error_log, __func__, "Get the error ic value\n");
		skyeye_exit(-1);
	}
	check_intr(core);
	return;
}


void c54x_reset(conf_object_t* obj){
	c54x_core_t* core = (c54x_core_t *)obj->obj;
	return;
}
void per_cpu_step(conf_object_t* obj)
{
	c54x_step_once(obj);
	return;
}

endian_t c54x_get_endian(conf_object_t* obj){
	return Little_endian;
}
align_t c54x_get_alignment(conf_object_t* obj){
	return Align;
}
exception_t c54x_signal(conf_object_t* conf_obj, interrupt_signal_t *signal){ /* called in another thread */
	c54x_core_t* core = (c54x_core_t *)conf_obj->obj;
	c5k_signal_t *c54_signal = &signal->c54_signal;
	int i = 0;
	for(; i < 16; i++){
		if(i >= 9) /* reserved */ //C541x IFR
			continue;
		if(c54_signal->ifr[i] != Prev_level){
			RW_WRLOCK(core->ifr_lock);
			if(c54_signal->ifr[i] == Low_level){
				core->MMR_BUFFER[CPU_IFR] &= ~(1 << i);
			}
			else{ /* high level */
				core->MMR_BUFFER[CPU_IFR] |= (1 << i);
			}
			RW_UNLOCK(core->ifr_lock);
			break;
		}
	}
	return No_exp;
}

uint64_t c54x_get_steps(conf_object_t* conf_obj){
	c54x_core_t* core = (c54x_core_t *)conf_obj->obj;
        return core->insn_num;
}

void per_cpu_stop(conf_object_t* obj)
{
	return;
}

int c54x_insert_bp(conf_object_t* conf_obj, generic_address_t addr){
	c54x_core_t* core = (c54x_core_t *)conf_obj->obj;
	int ret;
	ret = create_a_breakpoint(&core->bp_manager, addr);
	if(ret != 0)
		return ret;
	return 0;
}

int c54x_delete_bp(conf_object_t* conf_obj, generic_address_t addr){
	c54x_core_t* core = (c54x_core_t *)conf_obj->obj;
	int ret;
	ret = remove_bp_by_addr(&core->bp_manager, addr);
	if(ret != 0)
		return ret;
	return 0;
}

int c54x_get_bp_numbers(conf_object_t *conf_obj){
	c54x_core_t* core = (c54x_core_t *)conf_obj->obj;
	return get_bp_numbers(&core->bp_manager);
}

uint32_t c54x_get_bp_addr_by_id(conf_object_t *conf_obj, int id){
	c54x_core_t* core = (c54x_core_t *)conf_obj->obj;
	breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);
	if(bp)
		return bp->address;
	return 0;
}

int c54x_get_trigger(conf_object_t *conf_obj)
{
	c54x_core_t* core = (c54x_core_t *)conf_obj->obj;
	return get_breakpoint_hit_id(&core->bp_manager);
}

bool_t c54x_check_bp_trigger(conf_object_t *conf_obj)
{
	c54x_core_t* core = (c54x_core_t *)conf_obj->obj;
	int ret;
	ret = check_breakpoint_hit_flag(&core->bp_manager);
	if(ret == 0)
		return True;
	return False;
}

int c54x_clear_bp_trigger(conf_object_t *conf_obj)
{
	c54x_core_t* core = (c54x_core_t *)conf_obj->obj;
	clear_breakpoint_hit_flag(&core->bp_manager);
	return 0;
}

exception_t memory_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	c54x_core_t* core = (c54x_core_t *)obj->obj;
        core->memory_space = obj2;
   	if(core->memory_space == NULL){
		skyeye_log(Warning_log, __FUNCTION__, "Set 'memory space' attribute Error");
		return  Not_found_exp;
	}
    	core->memory_space_iface = (memory_space_intf*)SKY_get_iface(core->memory_space, MEMORY_SPACE_INTF_NAME);
    if(core->memory_space_iface == NULL){
		skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' interface from %s fail.", obj2->objname);
		return  Not_found_exp;
		}
	return No_exp;
}
 
exception_t memory_space_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	c54x_core_t* core = (c54x_core_t *)obj->obj;
	*obj2 = core->memory_space;
	*port = NULL;
	return No_exp;
}

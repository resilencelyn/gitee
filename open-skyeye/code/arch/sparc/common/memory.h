/*
 * =====================================================================================
 *
 *       Filename:  memory.h
 *
 *    Description:  Memory module header file
 *
 *        Version:  1.0
 *        Created:  15/04/08 17:30:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

#ifndef _SPARC_MEMORY_
#define _SPARC_MEMORY_

#include "skyeye_config.h"
#include "mmu/mmu.h"

#ifndef __SPARC_TYPES_H_
#error "arch/sparc/common/types.h header fle must be included before memory.h"
#endif

#define MAX_STRING      10

#if 0
#define sparc_memory_store_byte(offset, value) bus_write(8, offset, value)
#define sparc_memory_store_word16(offset, value) bus_write(16, offset, value)
#define sparc_memory_store_word32(offset, value)    \
    ({  \
     bus_write(32, offset, value);  \
     })

#define sparc_memory_read_word32(value,offset) 	bus_read(32, offset, value)
#define sparc_memory_read_word16(value,offset) 	bus_read(16, offset, value)
#define sparc_memory_read_byte(value,offset) 	bus_read(8, offset, value)
#else
extern mmu_ops_t sparc_mmu_ops;

#define sparc_memory_store_byte(conf_obj, offset, value) sparc_mmu_ops.write_byte(conf_obj, offset, value)
#define sparc_memory_store_word16(conf_obj, offset, value) sparc_mmu_ops.write_halfword(conf_obj, offset, value)
#define sparc_memory_store_word32(conf_obj, offset, value) sparc_mmu_ops.write_word(conf_obj, offset, value)

#define sparc_memory_read_word32(conf_obj, value,offset) 	sparc_mmu_ops.read_word(conf_obj, offset, value)
#define sparc_memory_read_word16(conf_obj, value,offset) 	sparc_mmu_ops.read_halfword(conf_obj, offset, value)
#define sparc_memory_read_byte(conf_obj, value,offset) 	sparc_mmu_ops.read_byte(conf_obj, offset, value)
#define sparc_memory_read_instr(value, offset)	sparc_mmu_ops.load_instr(obj, offset, value)

#define sparc_write_mmu_regs(conf_obj, value, offset)	sparc_mmu_ops.write_reg(conf_obj, offset, value)
#define sparc_read_mmu_regs(conf_obj, value, offset)	sparc_mmu_ops.read_reg(conf_obj, offset, value)
#endif

#endif

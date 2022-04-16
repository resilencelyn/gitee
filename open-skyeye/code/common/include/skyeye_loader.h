#ifndef __LOADER_H__
#define __LOADER_H__
#include "skyeye_types.h"
#include "skyeye_addr_space.h"
#ifdef __cplusplus
 extern "C" {
#endif

exception_t SKY_load_elf(const char* elf_filename, addr_type_t addr_type);
exception_t load_file(conf_object_t *obj, const char* filename, generic_address_t load_addr);
exception_t load_data(void* src, size_t len, generic_address_t load_addr);
endian_t get_elf_endian(const char* elf_filename);
exception_t load_exec_by_space (addr_space_t* space, const char *file);
exception_t get_elf_start_addr(const char* filename, generic_address_t* start_addr,generic_address_t * code_start_addr, generic_address_t * code_end_addr);
exception_t get_coff_start_addr(const char* filename, generic_address_t* start_addr,generic_address_t * code_start_addr, generic_address_t * code_end_addr);
exception_t get_hex_start_addr(const char* filename, generic_address_t* start_addr,generic_address_t * code_start_addr, generic_address_t * code_end_addr);
bool_t is_elf(const char* filename);
bool_t is_coff(const char* filename);
bool_t is_hex(const char* filename);
exception_t SKY_load_exec(memory_space_intf * memory_space, const char* filename);
exception_t SKY_load_file(conf_object_t* obj, memory_space_intf * memory_space, const char* filename);
#ifdef __cplusplus
}
#endif

#endif

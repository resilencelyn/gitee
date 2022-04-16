#ifndef __SKYEYE_ARCH_H__
#define __SKYEYE_ARCH_H__
#include "skyeye_types.h"
#include "skyeye_config.h"

#ifdef __cplusplus
 extern "C" {
#endif
/*
 * a running instance for a specific archteciture.
 */
typedef struct generic_arch_s
{
	char* arch_name;
	void (*init) (void);
	void (*reset) (void);
	void (*step_once) (void);
	void (*set_pc)(generic_address_t addr);
	generic_address_t (*get_pc)(void);
	uint64_t (*get_step)(void);
	//chy 2004-04-15 
	//int (*ICE_write_byte) (generic_address_t addr, uint8_t v);
	//int (*ICE_read_byte)(generic_address_t addr, uint8_t *pv);
	uint32 (*get_regval_by_id)(int id);
	uint32 (*get_regnum)(void);
	const char* (*get_regname_by_id)(int id);
	uint32 (*get_regid_by_name)(char* name);
	exception_t (*set_regval_by_id)(int id, uint32 value);
	/*
	 * read a data by virtual address.
	 */
	exception_t (*mmu_read)(short size, generic_address_t addr, uint32_t * value);
	/*
	 * write a data by a virtual address.
	 */
	exception_t (*mmu_write)(short size, generic_address_t addr, uint32_t value);
	/**
	 * get a signal from external
	 */
	exception_t (*signal)(interrupt_signal_t* signal);

	endian_t endianess;
	align_t alignment;
} generic_arch_t;

/*
 * a running instance for a core.
 */
typedef struct generic_core_s
{
	char* core_name;
	void (*init) (void);
	void (*reset) (void);
	void (*step_once) (void);
	int (*get_current_cycles)(void);
	int (*get_current_steps)(void);
	void (*set_pc)(generic_address_t addr);
	generic_address_t (*get_pc)(void);
	uint32 (*get_regval_by_id)(int id);
	exception_t (*set_register_by_id)(int id, uint32 value);
	endian_t endianess;
} generic_core_t;

/*
 * register a simulation module for an architecture.
 */
void register_arch(arch_config_t * arch);
//generic_arch_t* get_default_arch(void);

extern generic_arch_t* running_arch_list;
void alloc_arch_instance(void);
static inline generic_arch_t * get_arch_instance(const char* arch_name) {
	if(running_arch_list == NULL){
		//alloc_arch_instance();
	}
	return NULL;
}


typedef enum{
	ARM,
	C6K,
	POWERPC,
	SPARC,
	Unknow
}arch_t;


arch_t get_arch_by_cpucls(char *cpucls);



#ifdef __cplusplus
}
#endif

#endif

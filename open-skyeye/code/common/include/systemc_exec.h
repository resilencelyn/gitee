#ifndef __SYSTEMC_EXEC_H__
#define __SYSTEMC_EXEC_H__
#include "skyeye_thread.h"
#include "skyeye_types.h"
#include "skyeye_queue.h"
#ifdef __cplusplus
 extern "C" {
#endif

struct systemc_exec_s{
	void (*run)(conf_object_t *obj);
	void (*stop)(conf_object_t *obj);
	conf_object_t* priv_data;
	uint64_t cycles;
	LIST_ENTRY (systemc_exec_s)list_entry;
};

typedef struct systemc_exec_s systemc_exec_t;

systemc_exec_t* create_systemc_exec();

int add_to_exec_list(systemc_exec_t *exec);

int del_from_exec_list(systemc_exec_t *exec);

int exec_post_one_cycle();

int exec_post_cycles(unsigned count);
#ifdef __cplusplus
}
#endif

#endif

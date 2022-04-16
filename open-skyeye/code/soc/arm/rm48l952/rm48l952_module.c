#include <stdlib.h>
#include <skyeye_module.h>
#include <skyeye_mach.h>

const char *skyeye_module = "rm48l952";

extern void init_rm48l952_mach();
void module_init() {
	init_rm48l952_mach();
}

void module_fini() {
}

#include <stdlib.h>
#include <skyeye_module.h>
#include <skyeye_mach.h>

const char *skyeye_module = "omapl138";

extern void init_omapl138_mach();
void module_init() {
	init_omapl138_mach();
}

void module_fini() {
}

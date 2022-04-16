#include <skyeye_module.h>

const char* skyeye_module = "base_mach";

extern void init_base_mach();
void module_init() {
	init_base_mach();
}

void module_fini() {
}

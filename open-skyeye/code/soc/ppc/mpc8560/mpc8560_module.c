#include <stdlib.h>
#include <skyeye_module.h>
#include <skyeye_mach.h>

const char *skyeye_module = "mpc8560";

extern void init_mpc8560_mach();
void module_init() {
	init_mpc8560_mach();
}

void module_fini() {
}

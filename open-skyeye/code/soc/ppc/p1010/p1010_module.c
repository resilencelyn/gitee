#include <stdlib.h>
#include <skyeye_module.h>
#include <skyeye_mach.h>

const char *skyeye_module = "p1010";

extern void init_p1010_mach();
void module_init() {
	init_p1010_mach();
}

void module_fini() {
}

#include <stdlib.h>
#include <skyeye_module.h>
#include <skyeye_mach.h>

const char *skyeye_module = "am3359";

extern void init_am3359_mach();
void module_init() {
	init_am3359_mach();
}

void module_fini() {
}

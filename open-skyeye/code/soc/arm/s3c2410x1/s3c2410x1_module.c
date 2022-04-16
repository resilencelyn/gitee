#include <stdlib.h>
#include <skyeye_module.h>
#include <skyeye_mach.h>

const char *skyeye_module = "s3c2410x1";

extern void init_s3c2410x1_mach();
void module_init() {
	init_s3c2410x1_mach();
}

void module_fini() {
}

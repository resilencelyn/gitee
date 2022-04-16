#include <stdlib.h>
#include <skyeye_module.h>
#include <skyeye_mach.h>
#include "s3c6410x.h"

const char *skyeye_module = "s3c6410x";

	extern void init_s3c6410x_mach();
void module_init() {
	init_s3c6410x_mach();
}

void module_fini() {
}

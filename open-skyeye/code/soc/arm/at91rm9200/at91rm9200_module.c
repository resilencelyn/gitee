#include <stdlib.h>
#include <skyeye_module.h>
#include <skyeye_mach.h>

const char *skyeye_module = "at91rm9200";

extern void init_at91rm9200_mach();
void module_init() {
	init_at91rm9200_mach();
}

void module_fini() {
}

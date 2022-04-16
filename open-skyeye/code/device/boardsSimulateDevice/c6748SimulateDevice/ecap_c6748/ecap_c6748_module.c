#include <skyeye_module.h>

const char* skyeye_module = "c6748_ecap";

extern void init_ecap_c6748();

void module_init(){
	init_ecap_c6748();
}

void module_fini(){
}

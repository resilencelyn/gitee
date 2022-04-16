#include "skyeye_module.h"

const char* skyeye_module = "psc_omapl138";

extern void init_omapl138_psc();

void module_init(){
	init_omapl138_psc();
}

void module_fini(){
}

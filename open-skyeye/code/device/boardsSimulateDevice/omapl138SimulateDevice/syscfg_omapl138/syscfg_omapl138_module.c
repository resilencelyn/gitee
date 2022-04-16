#include "skyeye_module.h"

const char* skyeye_module = "syscfg_omapl138";

extern void init_omapl138_syscfg();

void module_init(){
	init_omapl138_syscfg();
}

void module_fini(){
}

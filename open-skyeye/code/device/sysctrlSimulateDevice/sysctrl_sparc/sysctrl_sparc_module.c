#include "skyeye_module.h"

const char* skyeye_module = "sysctrl_sparc";

extern void init_sparc_sysctrl();

void module_init(){
	init_sparc_sysctrl();
}

void module_fini(){
}

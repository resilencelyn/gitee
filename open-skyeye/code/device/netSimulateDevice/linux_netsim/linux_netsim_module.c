#include "skyeye_module.h"

const char* skyeye_module = "linux_netsim";

extern void init_linux_netsim();

void module_init(){
	init_linux_netsim();
}

void module_fini(){
}

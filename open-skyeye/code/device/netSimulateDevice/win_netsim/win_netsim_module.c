#include "skyeye_module.h"

const char* skyeye_module = "win_netsim";

extern void init_win_netsim();

void module_init(){
	init_win_netsim();
}

void module_fini(){
}

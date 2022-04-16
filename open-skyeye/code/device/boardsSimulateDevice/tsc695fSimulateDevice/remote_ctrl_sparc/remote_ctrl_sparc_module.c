#include "skyeye_module.h"

const char* skyeye_module = "remote_ctrl_sparc";

extern void init_sparc_remote_ctrl();

void module_init(){
	init_sparc_remote_ctrl();
}

void module_fini(){
}

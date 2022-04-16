#include "skyeye_module.h"

const char* skyeye_module = "rapidio_mpc8641";

extern void init_mpc8641_rapidio();

void module_init(){
	init_mpc8641_rapidio();
}

void module_fini(){
}

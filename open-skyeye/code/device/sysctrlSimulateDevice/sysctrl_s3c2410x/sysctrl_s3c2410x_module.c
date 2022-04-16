#include "skyeye_module.h"

const char* skyeye_module = "sysctrl_s3c2410x";

extern void init_s3c2410x_sysctrl();

void module_init(){
	init_s3c2410x_sysctrl();
}

void module_fini(){
}

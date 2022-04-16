#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "sopc_i2c";

extern void init_sopc_i2c();

void module_init(){
	init_sopc_i2c();
}

void module_fini(){
}

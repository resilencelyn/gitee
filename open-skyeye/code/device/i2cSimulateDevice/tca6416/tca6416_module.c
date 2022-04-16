#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "tca6416";

extern void init_tca6416();

void module_init(){
	init_tca6416();
}

void module_fini(){
}

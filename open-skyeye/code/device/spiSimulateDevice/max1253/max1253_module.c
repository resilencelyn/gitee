#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "max1253";

extern void init_max1253();

void module_init(){
	init_max1253();
}

void module_fini(){
}

#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "ds3231";

extern void init_ds3231();

void module_init(){
	init_ds3231();
}

void module_fini(){
}

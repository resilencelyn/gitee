#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "rtc_am335x";

extern void init_am335x_rtc();

void module_init(){
	init_am335x_rtc();
}

void module_fini(){
}

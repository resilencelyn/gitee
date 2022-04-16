#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "eeprom_cat24c64";

extern void init_eeprom_cat24c64();

void module_init(){
	init_eeprom_cat24c64();
}

void module_fini(){
}

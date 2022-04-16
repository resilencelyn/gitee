#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "eeprom_sparc";

extern void init_eeprom();

void module_init()
{
    init_eeprom();
}

void module_fini()
{
}

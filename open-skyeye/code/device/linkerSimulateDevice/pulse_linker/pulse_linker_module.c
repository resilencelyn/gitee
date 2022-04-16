#include "skyeye_module.h"

const char* skyeye_module = "pulse_linker";

extern void init_pulse_linker();

void module_init()
{
    init_pulse_linker();
}

void module_fini()
{
}

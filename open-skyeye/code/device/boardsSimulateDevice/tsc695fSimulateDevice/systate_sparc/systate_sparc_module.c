#include "skyeye_module.h"

const char* skyeye_module = "systate_sparc";

extern void init_sparc_systate();

void module_init()
{
    init_sparc_systate();
}

void module_fini()
{
}

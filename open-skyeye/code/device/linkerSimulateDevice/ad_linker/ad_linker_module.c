#include "skyeye_module.h"

const char* skyeye_module = "ad_linker";

extern void init_ad_linker();

void module_init()
{
    init_ad_linker();
}

void module_fini()
{
}

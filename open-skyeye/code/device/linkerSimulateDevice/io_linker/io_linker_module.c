#include "skyeye_module.h"

const char* skyeye_module = "io_linker";

extern void init_io_linker();

void module_init()
{
    init_io_linker();
}

void module_fini()
{
}

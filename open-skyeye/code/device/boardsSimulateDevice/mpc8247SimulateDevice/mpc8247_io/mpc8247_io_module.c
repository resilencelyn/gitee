#include <skyeye_module.h>

const char* skyeye_module = "mpc8247_io";
extern void init_mpc8247_io();

void module_init()
{
    init_mpc8247_io();
}

void module_fini()
{
}

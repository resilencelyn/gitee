#include <skyeye_module.h>

const char* skyeye_module = "mpc8247_clockandreset";
extern void init_mpc8247_clockandreset();

void module_init()
{
    init_mpc8247_clockandreset();
}

void module_fini()
{
}

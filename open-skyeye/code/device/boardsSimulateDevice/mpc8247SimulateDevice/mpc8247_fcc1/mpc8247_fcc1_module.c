#include <skyeye_module.h>

const char* skyeye_module = "mpc8247_fcc1";
extern void init_mpc8247_fcc1();

void module_init()
{
    init_mpc8247_fcc1();
}

void module_fini()
{
}

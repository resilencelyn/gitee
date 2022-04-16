#include <skyeye_module.h>

const char* skyeye_module = "mpc8247_brg";
extern void init_mpc8247_brg();

void module_init()
{
    init_mpc8247_brg();
}

void module_fini()
{
}

#include <skyeye_module.h>

const char* skyeye_module = "mpc8247_dpram";
extern void init_mpc8247_dpram();

void module_init()
{
    init_mpc8247_dpram();
}

void module_fini()
{
}

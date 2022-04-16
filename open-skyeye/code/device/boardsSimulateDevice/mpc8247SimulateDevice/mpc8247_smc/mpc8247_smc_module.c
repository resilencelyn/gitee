#include <skyeye_module.h>

const char* skyeye_module = "mpc8247_smc";
extern void init_mpc8247_smc();

void module_init()
{
    init_mpc8247_smc();
}

void module_fini()
{
}

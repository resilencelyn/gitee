#include <skyeye_module.h>

const char* skyeye_module = "mpc8247_siu_ic";
extern void init_mpc8247_siu_ic();

void module_init()
{
    init_mpc8247_siu_ic();
}

void module_fini()
{
}

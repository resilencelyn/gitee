#include <skyeye_module.h>

const char* skyeye_module = "mpc8272_siu";
extern void init_mpc8272_siu();

void module_init()
{
    init_mpc8272_siu();
}

void module_fini()
{
}

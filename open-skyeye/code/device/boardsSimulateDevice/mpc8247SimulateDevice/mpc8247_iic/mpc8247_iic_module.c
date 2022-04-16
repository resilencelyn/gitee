#include <skyeye_module.h>

const char* skyeye_module = "mpc8247_iic";
extern void init_mpc8247_iic();

void module_init()
{
    init_mpc8247_iic();
}

void module_fini()
{
}

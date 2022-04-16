#include <skyeye_module.h>

const char* skyeye_module = "mpc5554_siu";
extern void init_mpc5554_siu();

void module_init()
{
    init_mpc5554_siu();
}

void module_fini()
{
}

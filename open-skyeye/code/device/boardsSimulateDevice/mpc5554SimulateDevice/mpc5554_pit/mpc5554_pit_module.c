#include <skyeye_module.h>

const char* skyeye_module = "mpc5554_pit";
extern void init_mpc5554_pit();

void module_init()
{
    init_mpc5554_pit();
}

void module_fini()
{
}

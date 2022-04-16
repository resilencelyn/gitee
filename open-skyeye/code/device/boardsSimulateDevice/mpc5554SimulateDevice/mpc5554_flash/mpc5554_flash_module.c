#include <skyeye_module.h>

const char* skyeye_module = "mpc5554_flash";
extern void init_mpc5554_flash();

void module_init()
{
    init_mpc5554_flash();
}

void module_fini()
{
}

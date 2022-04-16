#include <skyeye_module.h>

const char* skyeye_module = "mpc5554_esci";
extern void init_mpc5554_esci();

void module_init()
{
    init_mpc5554_esci();
}

void module_fini()
{
}

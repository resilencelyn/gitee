#include <skyeye_module.h>

const char* skyeye_module = "mpc5554_stm";
extern void init_mpc5554_stm();

void module_init()
{
    init_mpc5554_stm();
}

void module_fini()
{
}

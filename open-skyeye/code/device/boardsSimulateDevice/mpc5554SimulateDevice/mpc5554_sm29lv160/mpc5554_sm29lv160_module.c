#include <skyeye_module.h>

const char* skyeye_module = "mpc5554_sm29lv160";
extern void init_mpc5554_sm29lv160();

void module_init()
{
    init_mpc5554_sm29lv160();
}

void module_fini()
{
}

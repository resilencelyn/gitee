#include <skyeye_module.h>

const char* skyeye_module = "mpc5554_intc";
extern void init_mpc5554_intc();

void module_init()
{
    init_mpc5554_intc();
}

void module_fini()
{
}

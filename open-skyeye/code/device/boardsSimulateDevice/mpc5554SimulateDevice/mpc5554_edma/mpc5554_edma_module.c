#include <skyeye_module.h>

const char* skyeye_module = "mpc5554_edma";
extern void init_mpc5554_edma();

void module_init()
{
    init_mpc5554_edma();
}

void module_fini()
{
}

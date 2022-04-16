#include <skyeye_module.h>

const char* skyeye_module = "mpc5554_fmpll";
extern void init_mpc5554_fmpll();

void module_init()
{
    init_mpc5554_fmpll();
}

void module_fini()
{
}

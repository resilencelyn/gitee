#include <skyeye_module.h>

const char* skyeye_module = "mpc5554_ebi";
extern void init_mpc5554_ebi();

void module_init()
{
    init_mpc5554_ebi();
}

void module_fini()
{
}

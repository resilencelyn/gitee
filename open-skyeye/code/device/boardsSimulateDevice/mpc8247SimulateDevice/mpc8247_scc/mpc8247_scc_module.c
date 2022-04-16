#include <skyeye_module.h>

const char* skyeye_module = "mpc8247_scc";
extern void init_mpc8247_scc();

void module_init()
{
    init_mpc8247_scc();
}

void module_fini()
{
}

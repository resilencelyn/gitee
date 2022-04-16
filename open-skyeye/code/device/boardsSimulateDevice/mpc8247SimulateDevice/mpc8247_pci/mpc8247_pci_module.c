#include <skyeye_module.h>

const char* skyeye_module = "mpc8247_pci";
extern void init_mpc8247_pci();

void module_init()
{
    init_mpc8247_pci();
}

void module_fini()
{
}

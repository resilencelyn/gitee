#include <skyeye_module.h>

const char* skyeye_module = "pcie_linker";
extern void init_pcie_linker();

void module_init()
{
    init_pcie_linker();
}

void module_fini()
{
}

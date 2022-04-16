#include <skyeye_module.h>

const char* skyeye_module = "mpc8272_cpm_mux";
extern void init_mpc8272_cpm_mux();

void module_init()
{
    init_mpc8272_cpm_mux();
}

void module_fini()
{
}

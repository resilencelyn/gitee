#include <skyeye_module.h>

const char* skyeye_module = "ft2000a_gmac";

extern void init_ft2000a_gmac();
extern void init_ft2000a_gmac_config();

void module_init()
{
	init_ft2000a_gmac();
	init_ft2000a_gmac_config();
}

void module_fini()
{
}

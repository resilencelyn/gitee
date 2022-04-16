#include <skyeye_module.h>

const char* skyeye_module = "api_test_dev";
extern void init_api_test_dev();

void module_init()
{
	init_api_test_dev();
}

void module_fini()
{
}

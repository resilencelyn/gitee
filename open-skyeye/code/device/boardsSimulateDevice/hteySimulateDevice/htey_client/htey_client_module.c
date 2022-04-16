#include <skyeye_module.h>
const char* skyeye_module = "htey_client";
extern void init_htey_client();
void module_init()
{
	init_htey_client();
}
void module_fini()
{
}

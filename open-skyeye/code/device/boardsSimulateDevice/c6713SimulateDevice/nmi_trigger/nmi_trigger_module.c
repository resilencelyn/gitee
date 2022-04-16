#include <skyeye_module.h>
const char* skyeye_module = "nmi_trigger";
extern void init_nmi_trigger();
void module_init()
{
	init_nmi_trigger();
}
void module_fini()
{
}

#include <skyeye_module.h>
const char* skyeye_module = "htey_hdlc";
extern void init_htey_hdlc();
void module_init()
{
	init_htey_hdlc();
}
void module_fini()
{
}
